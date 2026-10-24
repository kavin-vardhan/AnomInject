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
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent / "europa"))

PLUGIN_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE_ROOT = os.path.join(PLUGIN_ROOT, "Source")
GEN_DIR_NAME = "StrictIncludeCheckGenerated"
STRICT_SENTINEL = 'PrivateDefinitions.Add("ANOMALY_STRICT_INCLUDE=1");'
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
    if r.returncode:
        raise RuntimeError(r.stderr)
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


def generated_sources(host_dir, target, module):
    manifest = Path(host_dir) / "Intermediate/Build/Win64" / target / "Development" / (target + ".uhtmanifest")
    entries = [row for row in json.loads(manifest.read_text(encoding="utf-8-sig"))["Modules"] if row["Name"] == module]
    if not entries:
        return set()
    if len(entries) != 1:
        raise RuntimeError("Duplicate module in UHT manifest: " + module)
    entry = entries[0]
    if Path(entry["BaseDirectory"]).resolve(strict=True) != (Path(PLUGIN_ROOT) / "Source" / module).resolve(strict=True):
        raise RuntimeError("UHT module source root mismatch: " + module)
    directory = Path(entry["OutputDirectory"]).resolve(strict=True)
    if not any(directory.is_relative_to(root.resolve(strict=True)) for root in
               (Path(PLUGIN_ROOT) / "Intermediate", Path(host_dir) / "Intermediate")):
        raise RuntimeError("UHT output outside owned host/plugin: " + str(directory))
    files = {p.resolve() for p in directory.rglob("*.gen.cpp")}
    expected = {module + ".init.gen.cpp"}
    for field in ("ClassesHeaders", "PublicHeaders", "PrivateHeaders"):
        expected.update(Path(p).stem + ".gen.cpp" for p in entry.get(field, []))
    if {p.name for p in files} != expected:
        raise RuntimeError("UHT generated files disagree with manifest header denominator: " + module)
    return files


def verify_strict_took_effect(host_dir, target, kind, modules, actions_path, engine_name, source_tus=None, response_archive=None):
    data = json.loads(Path(actions_path).read_text(encoding="utf-8-sig"))
    actions = [x for x in data["Actions"] if x["Type"] == "Compile"]
    problems = []
    per_mod = {}
    gen_seen = 0
    for mod in modules:
        compiled = set()
        generated = set()
        for action in actions:
            from europa.working_build import response_paths, compile_sources
            paths = list(set([Path(p) for p in action.get("PrerequisiteItems", [])] + response_paths(action) + compile_sources(action, response_archive)))
            outputs = [Path(p) for p in action.get("ProducedItems", [])]
            if not any(p.parent.name == mod["name"] for p in outputs):
                continue
            if (Path(action.get("CommandPath", "")).name.casefold() == "rc.exe"
                    and outputs and all(p.suffix.casefold() == ".res" for p in outputs)
                    and any(p.suffix.casefold() in (".rc", ".rc2") for p in paths)
                    and not any(p.suffix.casefold() == ".cpp" for p in paths)):
                continue
            responses = [p for p in paths if p.suffix.lower() in (".response", ".rsp")]
            if not responses:
                problems.append("no response for actual compile action: " + str(outputs))
                continue
            from europa.working_build import response_flags
            from europa.a4_evidence import strict_response_problems
            bodies = [response_flags(Path((response_archive or {}).get(str(p), p)))[1] for p in responses]
            if any(strict_response_problems(body, []) for body in bodies):
                problems.append("PCH in actual compile action: " + str(responses))
                continue
            cpp = [p for p in paths if p.suffix.lower() == ".cpp"]
            if strict_response_problems("", cpp):
                problems.append("unity compile in strict action: " + str(cpp))
            compiled.update(p.name for p in cpp)
            for p in cpp:
                if p.name.endswith(".gen.cpp"):
                    generated.add(p.resolve())
                    text = p.read_text(encoding="utf-8-sig", errors="replace")
                    for include in re.findall(r'#include\s+"([^"]+\.gen\.cpp)"', text):
                        nested = Path(include)
                        generated.add((nested if nested.is_absolute() else p.parent / nested).resolve())
        count = 0
        required_sources = module_sources(mod) if source_tus is None else [Path(p).name for p in source_tus if Path(p).is_relative_to(Path(mod["dir"]))]
        for fn in required_sources:
            if fn not in compiled:
                problems.append("source absent from actual no-PCH compile actions: " + mod["name"] + "/" + fn)
                continue
            count += 1
            if fn.startswith("SIC_"):
                gen_seen += 1
        required_generated = generated_sources(host_dir, target, mod["name"])
        for missing in sorted(required_generated - generated):
            problems.append("generated TU absent from actual no-PCH actions: " + str(missing))
        if any(".generated.h" in Path(p).read_text(encoding="utf-8-sig") for p in Path(mod["dir"]).rglob("*.h")) and not required_generated:
            problems.append("reflected module has zero generated-TU denominator: " + mod["name"])
        per_mod[mod["name"]] = count
    if not actions or not gen_seen:
        problems.append("zero compile/header denominator")
    return per_mod, gen_seen, problems


def run_build(engine_root, target, uproject, log_path, mutation_manifest, single_file=None, expected_failure=None):
    sys.path.insert(0, str(Path(__file__).parent / "europa"))
    from a4_build import execute
    version = json.loads((Path(engine_root) / "Engine/Build/Build.version").read_text())
    host = "425" if version["MajorVersion"] == 4 else "51port"
    folder = Path(log_path).with_suffix("")
    result = execute(host, target, "Development", folder, dirty_manifest=mutation_manifest,
                     single_file=single_file, expected_failure=expected_failure)
    output = "\n".join(Path(p).read_text(encoding="utf-8", errors="replace") for p in result["logs"])
    Path(log_path).write_text(output, encoding="utf-8")
    return {"target": target, "exit": result["exit_code"], "seconds": result["duration_seconds"],
            "log": log_path, "actions": sum(result["action_counts"][k] for k in ("project", "plugin", "engine_source_in_host", "engine")),
            "output": output, "actions_path": result["actions_path"], "receipt": str(folder / "result.json")}


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
    global PLUGIN_ROOT, SOURCE_ROOT, STRICT_LINES
    ap = argparse.ArgumentParser()
    ap.add_argument("--uproject", required=True)
    ap.add_argument("--engine-root", required=True)
    ap.add_argument("--plugin-root", required=True)
    ap.add_argument("--targets", nargs="*")
    ap.add_argument("--log-dir", required=True)
    a = ap.parse_args()

    uproject = os.path.abspath(a.uproject)
    host_dir = os.path.dirname(uproject)
    engine_root = os.path.abspath(a.engine_root)
    owned = Path("D:/IA_Europa")
    allowed_hosts = [owned / "StylizedParisStreet", owned / "Baseline51/StackOBot", owned / "ReGate51/StackOBot"]
    host = Path(host_dir)
    plugin = Path(a.plugin_root).absolute()
    if host == allowed_hosts[1]:
        raise RuntimeError("Strict injection is forbidden in the release control")
    if host not in allowed_hosts or host.resolve(strict=True) != host or plugin != host / "Plugins/AnomalyInjector" or plugin.resolve(strict=True) != plugin:
        raise RuntimeError("Strict runs are restricted to the three physical-D port checkouts")
    PLUGIN_ROOT, SOURCE_ROOT = str(plugin), str(plugin / "Source")
    if not engine_root or not os.path.isdir(engine_root):
        print("REFUSED: engine root not found for " + uproject)
        return 2
    version = json.loads((Path(engine_root) / "Engine/Build/Build.version").read_text())
    pair = (version["MajorVersion"], version["MinorVersion"])
    required_engine = Path("E:/EpicGames/UE_4.25") if host == allowed_hosts[0] else Path("D:/UESource/UnrealEngine")
    if Path(engine_root).resolve(strict=True) != required_engine or pair != ((4, 25) if host == allowed_hosts[0] else (5, 1)):
        raise RuntimeError("Explicit engine/host version mismatch")
    engine_name = "UE4Editor" if pair == (4, 25) else "UnrealEditor"
    if pair == (5, 1):
        STRICT_LINES = STRICT_LINES + ["IncludeOrderVersion = EngineIncludeOrderVersion.Latest;"]
    found = discover_targets(host_dir)
    kinds = {name: kind for kind, name in found.items()}
    targets = a.targets or [found[k] for k in ("Game", "Editor") if k in found]
    for t in targets:
        if t not in kinds:
            raise RuntimeError("Target is not declared in this owned host: " + t)
    if not targets:
        print("REFUSED: no Game/Editor target found under " + host_dir)
        return 2
    stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
    log_dir = a.log_dir or os.path.join(host_dir, "Saved", "StrictIncludeBuild", stamp)
    if not Path(log_dir).resolve().is_relative_to(owned.resolve(strict=True)):
        raise RuntimeError("Strict evidence must stay on physical D under IA_Europa")
    if not os.environ.get("EUROPA_OPERATION_DIR"):
        raise RuntimeError("Strict build requires an active scoped write-ledger operation")
    os.makedirs(log_dir, exist_ok=True)

    modules = find_modules()
    for mod in modules:
        text = open(mod["build_cs"], encoding="utf-8-sig").read()
        if STRICT_SENTINEL in text or "EngineIncludeOrderVersion.Latest" in text:
            print("REFUSED: %s already carries the strict block (a previous run did not restore it). Restore it with git first." % mod["build_cs"])
            return 2
        if os.path.exists(os.path.join(mod["dir"], "Private", GEN_DIR_NAME)):
            print("REFUSED: leftover generated directory in " + mod["dir"])
            return 2

    print("plugin=%s head=%s" % (PLUGIN_ROOT, git("rev-parse", "HEAD")))
    print("host=%s engine=%s targets=%s log_dir=%s" % (uproject, engine_root, targets, log_dir))
    print("strict settings per plugin module: " + " ".join(STRICT_LINES))
    status_before = git("status", "--porcelain")
    if status_before:
        raise RuntimeError("Commit first; strict verification requires a clean checkout")
    backup_dir = os.path.join(log_dir, "buildcs_backup")
    os.makedirs(backup_dir, exist_ok=True)
    originals = {}
    hashes = {}
    gen_dirs = []
    all_tus = []
    results = {"strict": [], "normal": [], "restore_ok": False, "lever": {}}
    strict_errors = 0
    mutation_manifest = Path(log_dir) / "source-mutations.json"
    allowed = [str(Path(m["build_cs"]).relative_to(plugin)).replace("\\", "/") for m in modules]
    allowed += [str((Path(m["dir"]) / "Private" / GEN_DIR_NAME).relative_to(plugin)).replace("\\", "/") for m in modules]
    mutation_manifest.write_text(json.dumps({"allowed_changes": allowed, "commit": git("rev-parse", "HEAD")}, indent=2))
    mutant = None
    try:
        for mod in modules:
            hashes[mod["build_cs"]] = sha256(mod["build_cs"])
            shutil.copy2(mod["build_cs"], os.path.join(backup_dir, os.path.basename(mod["build_cs"])))
        for mod in modules:
            originals[mod["build_cs"]] = patch_build_cs(mod)
            gd, tus = generate_header_tus(mod)
            if gd:
                gen_dirs.append(gd)
                all_tus.extend(tus)
        print("header self-containment TUs generated: %d" % len(all_tus))
        with open(os.path.join(log_dir, "header_tus.json"), "w") as f:
            json.dump(all_tus, f, indent=1)
        from europa.a4_evidence import self_tests
        self_tests(Path(log_dir) / "reader-can-fails.json")
        mutant = plugin / "Source/AnomalyInjector/Private" / GEN_DIR_NAME / "SIC_MissingDefine09408.cpp"
        positive = '#include "CoreMinimal.h"\n#include "Components/StaticMeshComponent.h"\nclass UStaticMeshComponent;\nint EuropaStrictDefiningInclude09408(UStaticMeshComponent* Component) { return Component->GetNumMaterials(); }\n'
        negative = positive.replace('#include "Components/StaticMeshComponent.h"\n', '')
        mutant.write_text(positive, encoding="utf-8")
        Path(log_dir, "defining-include-mutant.json").write_text(json.dumps({
            "path": str(mutant), "positive_sha256": hashlib.sha256(positive.encode()).hexdigest(),
            "negative_sha256": hashlib.sha256(negative.encode()).hexdigest(),
            "prediction": "Positive compiles; deleting the defining StaticMeshComponent include leaves an incomplete UStaticMeshComponent and fails at the mutant source (C2027/C2039/C2227).",
            "positive": positive, "negative": negative}, indent=2))
        first = found["Editor"]
        results["can_fail_positive"] = run_build(engine_root, first, uproject, os.path.join(log_dir, "include_positive.log"), mutation_manifest, single_file=mutant)
        mutant.write_text(negative, encoding="utf-8")
        results["can_fail_negative"] = run_build(engine_root, first, uproject, os.path.join(log_dir, "include_negative.log"), mutation_manifest, single_file=mutant, expected_failure=mutant.name)
        if mutant.resolve(strict=True) != mutant or mutant.read_text(encoding="utf-8") != negative:
            raise RuntimeError("Mutant changed before its receipted cleanup")
        mutant.unlink()
        mutant = None
        for t in targets:
            res = run_build(engine_root, t, uproject, os.path.join(log_dir, "strict_%s.log" % t), mutation_manifest)
            diags = parse_diagnostics(res["output"])
            errs, warns = summarize("STRICT", res, diags)
            if Path(res["actions_path"]).is_file():
                per_mod, gen_seen, problems = verify_strict_took_effect(host_dir, t, kinds[t], modules, res["actions_path"], engine_name)
            else:
                per_mod, gen_seen, problems = {}, 0, ["No action list; strict compile not proven"]
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
        if mutant is not None and mutant.exists():
            if mutant.resolve(strict=True) != mutant or not mutant.is_relative_to(plugin / "Source"):
                raise RuntimeError("Mutant cleanup outside owned source")
            mutant.unlink()
        for path, raw in originals.items():
            with open(path, "wb") as f:
                f.write(raw)
        for gd in gen_dirs:
            resolved = Path(gd).resolve(strict=True)
            if not resolved.is_relative_to(plugin.resolve(strict=True)) or resolved.name != GEN_DIR_NAME:
                raise RuntimeError("Generated cleanup path escaped owned plugin")
            receipt = {str(Path(row["tu"]).resolve()) for row in all_tus if Path(row["tu"]).parent == Path(gd)}
            actual = {str(path.resolve()) for path in resolved.rglob("*") if path.is_file()}
            if actual != receipt:
                raise RuntimeError("Unreceipted file in generated cleanup directory")
            shutil.rmtree(resolved)
        ok = all(sha256(p) == h for p, h in hashes.items()) and not any(os.path.exists(g) for g in gen_dirs)
        status_after = git("status", "--porcelain")
        ok = ok and status_after == status_before
        results["restore_ok"] = ok
        results["restoration"] = {"rules": [{"path": p, "before": h, "after": sha256(p)} for p, h in hashes.items()],
                                  "status_before": status_before, "status_after": status_after}
        Path(log_dir, "restoration.json").write_text(json.dumps(results["restoration"], indent=2))
        Path(log_dir, "summary.json").write_text(json.dumps(results, indent=2))
        print("RESTORE %s: Build.cs hashes %s, generated dirs removed %s, git status unchanged %s" % (
            "OK" if ok else "FAILED",
            "identical" if all(sha256(p) == h for p, h in hashes.items()) else "DIFFER",
            not any(os.path.exists(g) for g in gen_dirs), status_after == status_before))
    normal_bad = 0
    if results["restore_ok"]:
        for t in targets:
            res = run_build(engine_root, t, uproject, os.path.join(log_dir, "normal_%s.log" % t), None)
            diags = parse_diagnostics(res["output"])
            errs, warns = summarize("NORMAL", res, diags)
            if res["exit"] != 0:
                errs = max(errs, 1)
            normal_bad += errs
            results["normal"].append({"target": t, "exit": res["exit"], "errors": errs, "warnings": warns, "log": res["log"]})
    with open(os.path.join(log_dir, "summary.json"), "w") as f:
        json.dump(results, f, indent=1)
    verdict = strict_errors == 0 and results["restore_ok"] and normal_bad == 0
    print("VERDICT %s: strict errors %d, restore %s, normal errors %d, summary %s" % (
        "PASS" if verdict else "FAIL", strict_errors, "ok" if results["restore_ok"] else "FAILED", normal_bad,
        os.path.join(log_dir, "summary.json")))
    return 0 if verdict else 1


if __name__ == "__main__":
    sys.exit(main())
