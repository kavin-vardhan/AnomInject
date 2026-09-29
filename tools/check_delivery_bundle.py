#!/usr/bin/env python3
"""Check an assembled delivery bundle before it goes to a client: the dashboard token file is IN it, and every
script path the README tells the client to run exists in it.

The M2 incident: the client's dashboard could not log in. The dashboard's token file is `dashboard\\config.json`
(it replaced the build-time `.env` at M2), make_delivery.py deliberately never copies it (the dev copy carries
the owner's token), and Setup.bat writes it WITHOUT a token when it is absent. So a bundle assembled by the
tooling alone reaches the client with an EMPTY token unless the owner writes the delivered one in. This check
makes that a STOP instead of a support call.

  python check_delivery_bundle.py <delivery root> [--expect-token-ini <DefaultGame.ini> | --expect-token-log <game log>]

It never prints a token - only its length and whether it matches.
Exit 0 PASS, 1 STOP, 2 cannot read.
"""
import argparse
import json
import os
import re
import shutil
import sys
import tempfile

REQUIRED = ("Setup.bat", "Run.bat", "README.md", "dashboard/index.html", "dashboard/config.json",
            "host-tools/encode_watcher.py", "host-tools/verify_capture.py", "host-tools/measure_label_offset.py",
            "host-tools/write_config.py", "host-tools/serve_dashboard.py", "host-tools/label_sync_check.py",
            "host-tools/OFFICE-CHECK.md")
PLACEHOLDER = re.compile(r"TESTVALUE|TESTTOKEN|CHANGEME|placeholder|^TEST$", re.I)
MIN_TOKEN = 32
README_CMD = re.compile(r"\bpython[0-9.]*\s+((?:<[^>]+>[\\/])?[\w.\\/-]+\.py)\b", re.I)


def token_from_ini(path):
    section = None
    with open(path, "r", encoding="utf-8-sig", errors="replace") as f:
        for line in f:
            s = line.strip()
            m = re.match(r"^\[(.+)\]$", s)
            if m:
                section = m.group(1)
                continue
            if section == "AnomalyControlServer":
                m = re.match(r"^Token\s*=\s*(\S+)\s*$", s)
                if m:
                    return m.group(1)
    return None


def token_from_log(path):
    tok = None
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        for line in f:
            m = re.search(r"=== Control server token: (\S+) \(", line)
            if m:
                tok = m.group(1)
    return tok


def check(root, expected=None, expected_src=None, out=print):
    stops = []
    if not os.path.isdir(root):
        out("BUNDLE REFUSED %s is not a folder" % root)
        return 2
    out("BUNDLE root=%s" % root)
    for rel in REQUIRED:
        present = os.path.isfile(os.path.join(root, rel.replace("/", os.sep)))
        if not present:
            stops.append("MISSING %s" % rel)
        out("BUNDLE FILE %-36s %s" % (rel, "present" if present else "*** MISSING ***"))

    cfg = os.path.join(root, "dashboard", "config.json")
    if os.path.isfile(cfg):
        try:
            with open(cfg, "r", encoding="utf-8-sig") as f:
                c = json.load(f)
            tok = c.get("controlToken", "")
            if not isinstance(tok, str):
                tok = ""
            problem = None
            if not tok:
                problem = "EMPTY - the dashboard opens on its manual connect screen and the client has no token to type"
            elif PLACEHOLDER.search(tok):
                problem = "a PLACEHOLDER value"
            elif len(tok) < MIN_TOKEN:
                problem = "only %d chars (need >= %d)" % (len(tok), MIN_TOKEN)
            elif expected is not None and tok != expected:
                problem = "DIFFERENT from the game's token (%s)" % expected_src
            if problem:
                stops.append("TOKEN")
            out("BUNDLE TOKEN dashboard/config.json controlToken: %d chars%s -> %s" % (
                len(tok), "" if expected is None else (", matches " + expected_src if tok == expected else ""),
                "OK" if not problem else "STOP: " + problem))
        except Exception as e:
            stops.append("TOKEN")
            out("BUNDLE TOKEN dashboard/config.json does not parse (%s) -> STOP" % e)
    else:
        out("BUNDLE TOKEN dashboard/config.json absent -> STOP (write it: python host-tools\\write_config.py "
            "--file <bundle>\\dashboard\\config.json --token <the delivered token>)")

    strays = []
    for base, _dirs, files in os.walk(root):
        for fn in files:
            if fn.lower() in (".env", "env", "config.json.txt", ".env.txt", "env.txt"):
                strays.append(os.path.relpath(os.path.join(base, fn), root))
    for s in strays:
        out("BUNDLE WARN stray token-looking file %s - the dashboard does not read it; its token file is dashboard\\config.json" % s)

    readme = os.path.join(root, "README.md")
    if os.path.isfile(readme):
        with open(readme, "r", encoding="utf-8-sig", errors="replace") as f:
            text = f.read()
        paths = []
        for m in README_CMD.finditer(text):
            p = re.sub(r"^<[^>]+>[\\/]", "", m.group(1)).replace("\\", "/")
            if p not in paths:
                paths.append(p)
        for p in paths:
            ok = os.path.isfile(os.path.join(root, p.replace("/", os.sep)))
            if not ok:
                stops.append("README %s" % p)
            out("BUNDLE README runs python %-40s %s" % (p, "exists in the bundle" if ok else "*** NOT IN THE BUNDLE -> STOP ***"))
        if not paths:
            out("BUNDLE README names no python script")

    out("BUNDLE VERDICT %s%s" % ("PASS" if not stops else "STOP", "" if not stops else " " + ", ".join(stops)))
    return 0 if not stops else 1


def selftest():
    good_tok = "A" * 40 + "b" * 24
    readme_ok = "Run:\n```\npython host-tools\\verify_capture.py --dir <s>\npython <delivery-root>\\host-tools\\verify_capture.py --x\n```\n"
    readme_bad = readme_ok + "python tools\\verify_capture.py --dir <s>\n"

    def make(d, token=good_tok, cfg=True, readme=readme_ok, stray=False, ini_token=None):
        for rel in REQUIRED:
            p = os.path.join(d, rel.replace("/", os.sep))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, "w", encoding="utf-8") as f:
                f.write("x")
        with open(os.path.join(d, "README.md"), "w", encoding="utf-8") as f:
            f.write(readme)
        cp = os.path.join(d, "dashboard", "config.json")
        if cfg:
            with open(cp, "w", encoding="utf-8") as f:
                json.dump({"controlToken": token, "capturesRoot": "", "serverUrl": "ws://127.0.0.1:12040"}, f)
        else:
            os.remove(cp)
        if stray:
            with open(os.path.join(d, "dashboard", "env"), "w") as f:
                f.write("VITE_CONTROL_TOKEN=x")
        ini = os.path.join(d, "DefaultGame.ini")
        with open(ini, "w", encoding="utf-8") as f:
            f.write("[AnomalyControlServer]\nToken=%s\n" % (ini_token or good_tok))
        return ini

    cases = [
        ("complete bundle, token matches the game", {}, 0),
        ("dashboard/config.json absent (the M2 shape)", {"cfg": False}, 1),
        ("token empty (Setup.bat's own default)", {"token": ""}, 1),
        ("placeholder token", {"token": "TESTVALUE123" + "x" * 30}, 1),
        ("token differs from the game's", {"ini_token": "Z" * 64}, 1),
        ("README runs tools\\verify_capture.py", {"readme": readme_bad}, 1),
        ("stray env file only (warns)", {"stray": True}, 0),
    ]
    tmp = tempfile.mkdtemp(prefix="bundle_selftest_")
    ok_all = True
    try:
        for i, (label, kw, want) in enumerate(cases):
            d = os.path.join(tmp, "b%d" % i)
            os.makedirs(d)
            ini = make(d, **kw)
            sink = []
            rc = check(d, token_from_ini(ini), "DefaultGame.ini", sink.append)
            ok = rc == want
            ok_all = ok_all and ok
            print("BUNDLE SELFTEST %-44s -> %s (exit %d, expected %d)%s" % (
                label, "ok" if ok else "*** WRONG ***", rc, want, "" if rc == 0 else "  [" + sink[-1].replace("BUNDLE VERDICT ", "") + "]"))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    print("BUNDLE SELFTEST %d case(s): %s" % (len(cases), "OK" if ok_all else "FAILED"))
    return 0 if ok_all else 1


def main():
    ap = argparse.ArgumentParser(description="Delivery bundle check: dashboard token file present and right; README paths exist.")
    ap.add_argument("root", nargs="?")
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--expect-token-ini", help="the delivered game's DefaultGame.ini ([AnomalyControlServer] Token)")
    g.add_argument("--expect-token-log", help="a log of the delivered build (its '=== Control server token:' line)")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if not a.root:
        ap.print_help()
        return 2
    expected, src = None, None
    if a.expect_token_ini:
        expected, src = token_from_ini(a.expect_token_ini), "the ini"
    elif a.expect_token_log:
        expected, src = token_from_log(a.expect_token_log), "the game log"
    if (a.expect_token_ini or a.expect_token_log) and not expected:
        print("BUNDLE REFUSED no token found in %s" % (a.expect_token_ini or a.expect_token_log))
        return 2
    return check(a.root, expected, src)


if __name__ == "__main__":
    sys.exit(main())
