import argparse
import datetime
import glob
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import time

PLUGIN_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_ROOT = os.path.join(PLUGIN_ROOT, "Source")
GEN_DIR_NAME = "StrictIncludeCheckGenerated"
STRICT_SENTINEL = "IncludeOrderVersion = EngineIncludeOrderVersion.Latest;"
STRICT_LINES = [
    "bUseUnity = false;",
    "PCHUsage = PCHUsageMode.NoPCHs;",
    STRICT_SENTINEL,
    "bEnforceIWYU = true;",
]
DIAG_RE = re.compile(r"(?P<loc>[^\s][^\r\n]*?)(?:\((?P<line>\d+)(?:,\d+)?\))?\s*:\s*(?P<kind>fatal error|error|warning)\s+(?P<code>[A-Z]+\d+)\s*:\s*(?P<msg>.*)$")
UBT_ERROR_RE = re.compile(r"^(ERROR|Error):\s", re.MULTILINE)


def sha256(path):
    with open(path, "rb") as f:
        return hashlib.sha256(f.read()).hexdigest().upper()


def git(*args):
    r = subprocess.run(["git", "-C", PLUGIN_ROOT] + list(args), capture_output=True, text=True)
    return r.stdout.strip()


def find_modules():
    mods = []
    for build_cs in sorted(glob.glob(os.path.join(SOURCE_ROOT, "*", "*.Build.cs"))):
        name = os.path.basename(build_cs)[: -len(".Build.cs")]
        mods.append({"name": name, "dir": os.path.dirname(build_cs), "build_cs": build_cs})
    return mods


def skip_string(text, j):
    n = len(text)
    verbatim = j > 0 and text[j - 1] == "@"
    j += 1
    while j < n:
        c = text[j]
        if verbatim:
            if c == '"':
                if j + 1 < n and text[j + 1] == '"':
                    j += 2
                    continue
                return j + 1
        else:
            if c == "\\":
                j += 2
                continue
            if c == '"':
                return j + 1
        j += 1
    raise ValueError("unterminated string literal")


def find_ctor_close(text, module):
    m = re.search(r"public\s+" + re.escape(module) + r"\s*\(\s*ReadOnlyTargetRules\s+Target\s*\)\s*:\s*base\s*\(\s*Target\s*\)", text)
    if not m:
        raise ValueError("constructor not found for module " + module)
    j = text.index("{", m.end())
    depth = 0
    n = len(text)
    while j < n:
        c = text[j]
        if c == '"':
            j = skip_string(text, j)
            continue
        if c == "'":
            k = j + 1
            if k < n and text[k] == "\\":
                k += 1
            j = text.index("'", k + 1) + 1
            continue
        if text.startswith("//", j):
            e = text.find("\n", j)
            j = n if e < 0 else e
            continue
        if text.startswith("/*", j):
            j = text.index("*/", j) + 2
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return j
        j += 1
    raise ValueError("unbalanced braces in " + module + ".Build.cs")


def patch_build_cs(mod):
    raw = open(mod["build_cs"], "rb").read()
    text = raw.decode("utf-8-sig")
    bom = raw.startswith(b"\xef\xbb\xbf")
    nl = "\r\n" if "\r\n" in text else "\n"
    close = find_ctor_close(text, mod["name"])
    line_start = text.rfind("\n", 0, close) + 1
    block = "".join("\t\t" + s + nl for s in STRICT_LINES)
    patched = text[:line_start] + block + text[line_start:]
    data = patched.encode("utf-8")
    if bom:
        data = b"\xef\xbb\xbf" + data
    with open(mod["build_cs"], "wb") as f:
        f.write(data)
    return raw


def generate_header_tus(mod):
    headers = []
    for root, dirs, files in os.walk(mod["dir"]):
        dirs[:] = [d for d in dirs if d != GEN_DIR_NAME]
        for fn in files:
            if fn.lower().endswith(".h"):
                headers.append(os.path.join(root, fn))
    headers.sort()
    if not headers:
        return None, []
    gen_dir = os.path.join(mod["dir"], "Private", GEN_DIR_NAME)
    os.makedirs(gen_dir, exist_ok=False)
    made = []
    for i, h in enumerate(headers):
        rel = os.path.relpath(h, gen_dir).replace("\\", "/")
        base = os.path.splitext(os.path.basename(h))[0]
        tu = os.path.join(gen_dir, "SIC_%s_%03d_%s.cpp" % (mod["name"], i, base))
        with open(tu, "w", newline="\n") as f:
            f.write('#include "%s"\n' % rel)
        made.append({"tu": tu, "header": os.path.relpath(h, PLUGIN_ROOT).replace("\\", "/")})
    return gen_dir, made


def discover_targets(host_dir):
    found = {}
    for tcs in glob.glob(os.path.join(host_dir, "Source", "*.Target.cs")):
        text = open(tcs, encoding="utf-8-sig").read()
        m = re.search(r"class\s+(\w+)Target\b", text)
        t = re.search(r"TargetType\.(\w+)", text)
        if m and t:
            found[t.group(1)] = m.group(1)
    return found


def engine_root_for(uproject):
    data = json.load(open(uproject, encoding="utf-8-sig"))
    assoc = data.get("EngineAssociation", "")
    if os.path.isdir(assoc):
        return assoc
    try:
        import winreg
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Epic Games\Unreal Engine\Builds") as k:
            val, _ = winreg.QueryValueEx(k, assoc)
            return val
    except OSError:
        return None


def parse_diagnostics(output):
    diags = {}
    for line in output.splitlines():
        if " error " not in line and " warning " not in line and "fatal error" not in line:
            continue
        m = DIAG_RE.search(line.strip())
        if not m:
            continue
        loc = m.group("loc").strip()
        key = (m.group("kind"), m.group("code"), os.path.normpath(loc) if loc else "", m.group("line") or "", m.group("msg").strip())
        diags[key] = diags.get(key, 0) + 1
    return diags


def module_sources(mod):
    out = []
    for root, dirs, files in os.walk(mod["dir"]):
        for fn in files:
            if fn.lower().endswith(".cpp"):
                out.append(fn)
    return sorted(out)


def verify_strict_took_effect(host_dir, target, kind, modules):
    if kind == "Editor":
        base = os.path.join(PLUGIN_ROOT, "Intermediate", "Build", "Win64", "UnrealEditor", "Development")
    else:
        base = os.path.join(host_dir, "Intermediate", "Build", "Win64", target, "Development")
    problems = []
    per_mod = {}
    gen_seen = 0
    for mod in modules:
        mdir = os.path.join(base, mod["name"])
        count = 0
        for fn in module_sources(mod):
            rsp = os.path.join(mdir, fn + ".obj.response")
            if not os.path.isfile(rsp):
                problems.append("no per-file compile response (unity still on?): " + rsp)
                continue
            body = open(rsp, encoding="utf-8", errors="replace").read()
            if "/Yu" in body or "/Yc" in body:
                problems.append("PCH still in use: " + rsp)
                continue
            count += 1
            if fn.startswith("SIC_"):
                gen_seen += 1
        per_mod[mod["name"]] = count
    return per_mod, gen_seen, problems


def run_build(engine_root, target, uproject, log_path):
    bat = os.path.join(engine_root, "Engine", "Build", "BatchFiles", "Build.bat")
    cmd = [bat, target, "Win64", "Development", "-project=" + uproject, "-waitmutex"]
    t0 = time.time()
    chunks = []
    with open(log_path, "w", encoding="utf-8") as f:
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        for raw in p.stdout:
            line = raw.decode("utf-8", errors="replace")
            chunks.append(line)
            f.write(line)
            f.flush()
        p.wait()
    out = "".join(chunks)
    actions = re.findall(r"Building (\d+) action", out)
    return {"target": target, "exit": p.returncode, "seconds": round(time.time() - t0, 1), "log": log_path,
            "actions": int(actions[-1]) if actions else None, "output": out}


def summarize(label, res, diags):
    errs = sum(1 for k in diags if k[0] != "warning")
    warns = sum(1 for k in diags if k[0] == "warning")
    ubt_errs = len(UBT_ERROR_RE.findall(res["output"]))
    print("[%s] %s exit=%d actions=%s seconds=%s unique_errors=%d unique_warnings=%d ubt_error_lines=%d log=%s" % (
        label, res["target"], res["exit"], res["actions"], res["seconds"], errs, warns, ubt_errs, res["log"]))
    for k, cnt in sorted(diags.items(), key=lambda kv: (kv[0][2], kv[0][3])):
        print("    %s %s %s(%s): %s  [x%d]" % (k[0], k[1], os.path.relpath(k[2], PLUGIN_ROOT) if k[2].lower().startswith(PLUGIN_ROOT.lower()) else k[2], k[3], k[4], cnt))
    return errs, warns


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--uproject", required=True)
    ap.add_argument("--engine-root")
    ap.add_argument("--targets", nargs="*")
    ap.add_argument("--log-dir")
    ap.add_argument("--no-header-check", action="store_true")
    ap.add_argument("--no-normal-rebuild", action="store_true")
    a = ap.parse_args()

    uproject = os.path.abspath(a.uproject)
    host_dir = os.path.dirname(uproject)
    engine_root = a.engine_root or engine_root_for(uproject)
    if not engine_root or not os.path.isdir(engine_root):
        print("REFUSED: engine root not found for " + uproject)
        return 2
    found = discover_targets(host_dir)
    kinds = {name: kind for kind, name in found.items()}
    targets = a.targets or [found[k] for k in ("Game", "Editor") if k in found]
    for t in targets:
        if t not in kinds:
            kinds[t] = "Editor" if t.endswith("Editor") else "Game"
    if not targets:
        print("REFUSED: no Game/Editor target found under " + host_dir)
        return 2
    stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    log_dir = a.log_dir or os.path.join(host_dir, "Saved", "StrictIncludeBuild", stamp)
    os.makedirs(log_dir, exist_ok=True)

    modules = find_modules()
    for mod in modules:
        text = open(mod["build_cs"], encoding="utf-8-sig").read()
        if STRICT_SENTINEL in text:
            print("REFUSED: %s already carries the strict block (a previous run did not restore it). Restore it with git first." % mod["build_cs"])
            return 2
        if os.path.exists(os.path.join(mod["dir"], "Private", GEN_DIR_NAME)):
            print("REFUSED: leftover generated directory in " + mod["dir"])
            return 2

    print("plugin=%s head=%s" % (PLUGIN_ROOT, git("rev-parse", "HEAD")))
    print("host=%s engine=%s targets=%s log_dir=%s" % (uproject, engine_root, targets, log_dir))
    print("strict settings per plugin module: " + " ".join(STRICT_LINES))
    status_before = git("status", "--porcelain")
    backup_dir = os.path.join(log_dir, "buildcs_backup")
    os.makedirs(backup_dir, exist_ok=True)
    originals = {}
    hashes = {}
    gen_dirs = []
    all_tus = []
    results = {"strict": [], "normal": [], "restore_ok": False, "lever": {}}
    strict_errors = 0
    try:
        for mod in modules:
            hashes[mod["build_cs"]] = sha256(mod["build_cs"])
            shutil.copy2(mod["build_cs"], os.path.join(backup_dir, os.path.basename(mod["build_cs"])))
        for mod in modules:
            originals[mod["build_cs"]] = patch_build_cs(mod)
            if not a.no_header_check:
                gd, tus = generate_header_tus(mod)
                if gd:
                    gen_dirs.append(gd)
                    all_tus.extend(tus)
        print("header self-containment TUs generated: %d" % len(all_tus))
        with open(os.path.join(log_dir, "header_tus.json"), "w") as f:
            json.dump(all_tus, f, indent=1)
        for t in targets:
            res = run_build(engine_root, t, uproject, os.path.join(log_dir, "strict_%s.log" % t))
            diags = parse_diagnostics(res["output"])
            errs, warns = summarize("STRICT", res, diags)
            per_mod, gen_seen, problems = verify_strict_took_effect(host_dir, t, kinds[t], modules)
            print("    lever check: per-file no-PCH compile responses per module %s, header TUs %d of %d, problems %d" % (
                per_mod, gen_seen, len(all_tus), len(problems)))
            for p in problems:
                print("    LEVER PROBLEM: " + p)
            if res["exit"] != 0 and errs == 0:
                errs = 1
                print("    nonzero exit with no parsed compiler error; read the log")
            strict_errors += errs + len(problems)
            results["strict"].append({"target": t, "exit": res["exit"], "errors": errs, "warnings": warns, "lever_problems": problems,
                                      "compile_tus": per_mod, "header_tus_compiled": gen_seen,
                                      "diagnostics": [list(k) + [c] for k, c in diags.items()], "log": res["log"]})
    finally:
        for path, raw in originals.items():
            with open(path, "wb") as f:
                f.write(raw)
        for gd in gen_dirs:
            shutil.rmtree(gd, ignore_errors=True)
        ok = all(sha256(p) == h for p, h in hashes.items()) and not any(os.path.exists(g) for g in gen_dirs)
        status_after = git("status", "--porcelain")
        ok = ok and status_after == status_before
        results["restore_ok"] = ok
        print("RESTORE %s: Build.cs hashes %s, generated dirs removed %s, git status unchanged %s" % (
            "OK" if ok else "FAILED",
            "identical" if all(sha256(p) == h for p, h in hashes.items()) else "DIFFER",
            not any(os.path.exists(g) for g in gen_dirs), status_after == status_before))
    normal_bad = 0
    if not a.no_normal_rebuild:
        for t in targets:
            res = run_build(engine_root, t, uproject, os.path.join(log_dir, "normal_%s.log" % t))
            diags = parse_diagnostics(res["output"])
            errs, warns = summarize("NORMAL", res, diags)
            if res["exit"] != 0:
                errs = max(errs, 1)
            normal_bad += errs + warns
            results["normal"].append({"target": t, "exit": res["exit"], "errors": errs, "warnings": warns, "log": res["log"]})
    with open(os.path.join(log_dir, "summary.json"), "w") as f:
        json.dump(results, f, indent=1)
    verdict = strict_errors == 0 and results["restore_ok"] and normal_bad == 0
    print("VERDICT %s: strict errors %d, restore %s, normal errors+warnings %d, summary %s" % (
        "PASS" if verdict else "FAIL", strict_errors, "ok" if results["restore_ok"] else "FAILED", normal_bad,
        os.path.join(log_dir, "summary.json")))
    return 0 if verdict else 1


if __name__ == "__main__":
    sys.exit(main())
