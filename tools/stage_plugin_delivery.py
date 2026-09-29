#!/usr/bin/env python3
"""Stage the plugin folder a client build receives, with the bench module and everything internal excluded.

The copy comes from `git archive <ref>`, so only TRACKED files at that ref can ship (no Binaries/, Intermediate/,
untracked notes or local edits). It is an ALLOWLIST: a new top-level entry does not ship until it is listed here.

  SHIP      AnomalyInjector.uplugin (rewritten: the AnomalyBench module entry removed), LICENSE.txt,
            Content/, Shaders/, Source/
  EXCLUDE   Source/AnomalyBench/        the bench fixture module (ruling F9): fixture map names, target names
                                        and camera coordinates live in its strings
            docs/, tools/, CLAUDE.md, WebClient/, .gitignore   internal only; the client-facing readme and
                                        verify_capture.py reach the client through the dashboard bundle
  DESCRIPTOR drop module "AnomalyBench"

Bench fixture CONTENT lives in the host project, not here; keep it out of a delivery cook with the lines this
tool prints (and check the cooked package with check_package_descriptor.py).

After staging it verifies its own output: the staged descriptor must pass the G354 check against the branch
descriptor minus AnomalyBench, and the staged Source must pass the lever audit.

  python stage_plugin_delivery.py --out <empty folder> [--ref HEAD]

Exit 0 staged and verified, 1 staged but a check failed (do not deliver), 2 refused.
"""
import argparse
import io
import json
import os
import subprocess
import sys
import tarfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)

import check_package_descriptor
import lever_audit

SHIP_TOP = ("AnomalyInjector.uplugin", "LICENSE.txt", "Content/", "Shaders/", "Source/")
EXCLUDE_PREFIXES = ("Source/AnomalyBench/",)
DROP_MODULES = ("AnomalyBench",)
COOK_EXCLUSION_LINES = (
    '[/Script/UnrealEd.ProjectPackagingSettings]',
    '+DirectoriesToNeverCook=(Path="/Game/AnomalyFixtures")',
    '+DirectoriesToNeverCook=(Path="/Game/CaptureBenchGate")',
    '+DirectoriesToNeverCook=(Path="/Game/CaptureBenchTexCorrupt")',
)


def shipped(name):
    if any(name.startswith(p) for p in EXCLUDE_PREFIXES):
        return False
    return any(name == t or (t.endswith("/") and name.startswith(t)) for t in SHIP_TOP)


def main():
    ap = argparse.ArgumentParser(description="Stage the delivered plugin folder (AnomalyBench and internal files excluded).")
    ap.add_argument("--out", required=True, help="destination; must not exist or be empty")
    ap.add_argument("--ref", default="HEAD", help="git ref to stage (default HEAD)")
    ap.add_argument("--repo", default=ROOT)
    a = ap.parse_args()

    out = os.path.abspath(a.out)
    if os.path.isdir(out) and os.listdir(out):
        print("STAGE REFUSED %s is not empty" % out)
        return 2
    r = subprocess.run(["git", "-C", a.repo, "archive", "--format=tar", a.ref], capture_output=True)
    if r.returncode != 0:
        print("STAGE REFUSED git archive %s failed: %s" % (a.ref, r.stderr.decode(errors="replace").strip()))
        return 2
    sha = subprocess.run(["git", "-C", a.repo, "rev-parse", a.ref], capture_output=True, text=True).stdout.strip()

    kept, excluded = {}, {}
    with tarfile.open(fileobj=io.BytesIO(r.stdout)) as tar:
        for m in tar.getmembers():
            if not m.isfile():
                continue
            top = m.name.split("/", 1)[0] + ("/" if "/" in m.name else "")
            if not shipped(m.name):
                key = next((p for p in EXCLUDE_PREFIXES if m.name.startswith(p)), top)
                excluded[key] = excluded.get(key, 0) + 1
                continue
            data = tar.extractfile(m).read()
            if m.name == "AnomalyInjector.uplugin":
                desc = json.loads(data.decode("utf-8-sig"))
                before = [x["Name"] for x in desc.get("Modules", [])]
                desc["Modules"] = [x for x in desc.get("Modules", []) if x.get("Name") not in DROP_MODULES]
                data = (json.dumps(desc, indent="\t") + "\n").encode("utf-8")
                print("STAGE DESCRIPTOR modules %s -> %s" % (",".join(before), ",".join(x["Name"] for x in desc["Modules"])))
            dest = os.path.join(out, m.name.replace("/", os.sep))
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            with open(dest, "wb") as f:
                f.write(data)
            kept[top] = kept.get(top, 0) + 1

    print("STAGE ref=%s (%s) out=%s" % (a.ref, sha[:12], out))
    for k in sorted(kept):
        print("STAGE SHIP    %-26s %d file(s)" % (k, kept[k]))
    for k in sorted(excluded):
        print("STAGE EXCLUDE %-26s %d file(s)" % (k, excluded[k]))
    if os.path.isdir(os.path.join(out, "Source", "AnomalyBench")):
        print("STAGE FAIL Source/AnomalyBench is present in the staged tree")
        return 1

    with open(os.path.join(out, "AnomalyInjector.uplugin"), "rb") as f:
        staged_desc = f.read()
    intended = os.path.join(a.repo, "AnomalyInjector.uplugin")
    lines = []
    rc1 = check_package_descriptor.check(staged_desc, os.path.join(out, "AnomalyInjector.uplugin"), intended, None, False, lines.append)
    print("STAGE G354 %s" % lines[-1].replace("G354 VERDICT ", ""))
    lines = []
    rc2 = lever_audit.audit(out, out=lines.append)
    print("STAGE LEVER-AUDIT %s" % lines[-1].replace("G-LEVER-AUDIT VERDICT ", ""))
    print("STAGE COOK-EXCLUSION lines for the host's Config/DefaultGame.ini of any delivery cook made on a bench host:")
    for l in COOK_EXCLUSION_LINES:
        print("    " + l)
    print("      and no /Game/CaptureBenchGate, /Game/AnomalyFixtures or /Game/CaptureBenchTexCorrupt map on the -map= list.")
    ok = rc1 == 0 and rc2 == 0
    print("STAGE VERDICT %s" % ("PASS - deliverable" if ok else "FAIL - do not deliver this folder"))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
