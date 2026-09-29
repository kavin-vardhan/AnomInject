import argparse
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from census_readonly_check import definitions, norm, rx_sub, rep, mutate

PATHS = {
    "acs": "Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp",
    "ais": "Source/AnomalyInjector/Private/AnomalyInjectorSubsystem.cpp",
    "aai": "Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp",
    "atc": "Source/AnomalyInjector/Private/Anomalies/Anomaly_TexCorrupt.cpp",
    "state": "Source/AnomalyInjector/Private/Anomalies/TexCorruptState.cpp",
}

PROVIDER_SIG = r"\buint8\s+UAnomalyCaptureSubsystem\s*::\s*QueryExclusionTrail\s*\("
EVAL_SIG = r"\bFAnomalyPartnerExclusion\s+UAnomalyInjectorSubsystem\s*::\s*EvaluatePartnerExclusion\s*\("
FIRE_SIG = r"\bbool\s+UAnomalyAutoInjectorSubsystem\s*::\s*TryFireSpecific\s*\("
APPLY_SIG = r"\bbool\s+FAnomaly_TexCorrupt\s*::\s*Apply\s*\("
LEVER_SIG = r"\bFString\s+TargetedNoModeBenchLever\s*\("


def body_of(files, key, sig, label, failures):
    defs = definitions(files[key], sig)
    if len(defs) != 1:
        failures.append(f"{label}: definitions: {len(defs)} (expected exactly 1)")
        return None
    return files[key][defs[0][0]:defs[0][1]]


def check_provider(files, failures):
    body = body_of(files, "acs", PROVIDER_SIG, "(p) QueryExclusionTrail", failures)
    if body is None:
        return
    if not re.search(r"\bPartnerFamily\s*=\s*AnomalyExclusion\s*::\s*FamilyOf\s*\(", body):
        failures.append("(p) QueryExclusionTrail: the partner family is not derived with AnomalyExclusion::FamilyOf")
    pending = re.search(r"for\s*\(\s*const\s+FAutoLiveFireInfo\s*&\s*F\s*:\s*Pending\.Value\.Fires\s*\)\s*\{(.*?)\}", body, re.S)
    if not pending:
        failures.append("(p) QueryExclusionTrail: no scan of the pending snapshots' fires")
    elif not re.search(r"\bPendingFireOwesFrames\s*\(\s*PartnerFamily\s*,\s*IsRenderTruthFire\s*\(\s*F\s*\)\s*\)", pending.group(1)):
        failures.append("(p) QueryExclusionTrail: the pending scan does not ask PendingFireOwesFrames(PartnerFamily, ...), so a "
                        "FireWindow m53 snapshot is not counted")
    gate = re.search(r"if\s*\(\s*AnomalyExclusion\s*::\s*RetainedLiveFireOwesFrames\s*\(\s*PartnerFamily\s*\)\s*\)\s*\{", body)
    if not gate:
        failures.append("(p) QueryExclusionTrail: the auto-injector's retained live fires are not read for the partner family")
    else:
        from census_readonly_check import match_close
        close = match_close(body, gate.end() - 1)
        block = body[gate.end():close]
        if not re.search(r"\bGetLiveFires\s*\(\s*\)", block):
            failures.append("(p) QueryExclusionTrail: the retained-fire block does not read GetLiveFires()")
        if not re.search(r"\bF\s*\.\s*Id\s*!=\s*PartnerId", block) and not re.search(r"\bF\s*\.\s*Id\s*==\s*PartnerId", block):
            failures.append("(p) QueryExclusionTrail: the retained-fire block does not match the partner id")
        if not re.search(r"\bConsider\s*\(\s*AnomalyExclusion\s*::\s*EmitterTrail\s*\(\s*true\s*\)", block):
            failures.append("(p) QueryExclusionTrail: a retained partner fire is not considered as an emitter that owes frames")
    body_eval = body_of(files, "ais", EVAL_SIG, "(p) EvaluatePartnerExclusion", failures)
    if body_eval is not None:
        for want in (r"TrailProvider\s*\(\s*UvName\s*,", r"TrailProvider\s*\(\s*NormalName\s*,"):
            if not re.search(want, body_eval):
                failures.append(f"(p) EvaluatePartnerExclusion: an m52 candidate does not ask the provider for '{want}'")


def check_lever(files, failures):
    fire = body_of(files, "aai", FIRE_SIG, "(n) TryFireSpecific", failures)
    if fire is not None:
        m = re.search(r"\bBenchLever\s*=\s*\(\s*bTexCorrupt\s*&&\s*!\s*bModeGiven\s*\)\s*\?\s*AnomalyTexCorrupt\s*::\s*"
                      r"TargetedNoModeBenchLever\s*\(\s*Id\s*\)", fire)
        if not m:
            failures.append("(n) TryFireSpecific: the bench lever is not read for a no-mode m53 fire")
        call = re.search(r"TexCorruptPure\s*::\s*TargetedAttempt\s*\(([^;]*)\)\s*;", fire, re.S)
        if not call or not norm(call.group(1)).endswith("!BenchLever.IsEmpty()"):
            failures.append("(n) TryFireSpecific: TargetedAttempt is not told the bench lever defines the mode")
        if "mode_source=bench_lever" not in fire:
            failures.append("(n) TryFireSpecific: no mode_source=bench_lever log line")
        if not re.search(r"if\s*\(\s*Targeted\s*\.\s*bBenchLever\s*\)\s*\{[^{}]*\}\s*else\s+if\s*\(\s*Targeted\s*\.\s*bRoundRobin\s*\)",
                         fire):
            failures.append("(n) TryFireSpecific: the round-robin branch is not an else of the bench-lever branch")
    apply = body_of(files, "atc", APPLY_SIG, "(n) FAnomaly_TexCorrupt::Apply", failures)
    if apply is not None:
        if not re.search(r"switch\s*\(\s*TexCorruptPure\s*::\s*NoModeLever\s*\(\s*L\s*\.\s*TileProbe\s*,\s*L\s*\.\s*bIdentityRedraw\s*,"
                         r"\s*L\s*\.\s*bIdentity\s*,\s*bAutoPool\s*,", apply):
            failures.append("(n) Apply: the no-mode branch does not select through TexCorruptPure::NoModeLever")
        if re.search(r"\bL\s*\.\s*(?:TileProbe\s*==|bIdentityRedraw\s*\)|bIdentity\s*\))", apply):
            failures.append("(n) Apply: a private copy of the no-mode lever rule remains beside NoModeLever")
    lever = body_of(files, "state", LEVER_SIG, "(n) TargetedNoModeBenchLever", failures)
    if lever is not None:
        m = re.search(r"TexCorruptPure\s*::\s*NoModeLever\s*\(([^;]*)\)\s*;", lever, re.S)
        if not m:
            failures.append("(n) TargetedNoModeBenchLever: does not use TexCorruptPure::NoModeLever")
        else:
            args = [norm(a) for a in m.group(1).split(",")]
            if len(args) != 5 or args[3] != "false":
                failures.append("(n) TargetedNoModeBenchLever: must ask as a targeted (not auto-pool) fire, bAutoPool=false")


def check(files):
    failures = []
    check_provider(files, failures)
    check_lever(files, failures)
    return failures


def build_mutants(files):
    return [
        ("provider ignores the retained auto live fires", "retained live fires are not read",
         mutate(files, "acs", PROVIDER_SIG, rx_sub(r"if\s*\(\s*AnomalyExclusion::RetainedLiveFireOwesFrames\(PartnerFamily\)\)",
                                                    "if (false)"))),
        ("retained scan reads no live fires", "does not read GetLiveFires()",
         mutate(files, "acs", PROVIDER_SIG, rep("Auto->GetLiveFires()", "TArray<FAutoLiveFireInfo>()"))),
        ("pending scan back to render-truth fires only", "FireWindow m53 snapshot is not counted",
         mutate(files, "acs", PROVIDER_SIG, rep("AnomalyExclusion::PendingFireOwesFrames(PartnerFamily, IsRenderTruthFire(F))",
                                                "IsRenderTruthFire(F)"))),
        ("provider not asked for normal_corruption", "NormalName",
         mutate(files, "ais", EVAL_SIG, rep("TrailProvider(NormalName, NormalKey)", "AnomalyExclusion::ETrail::None"))),
        ("TryFireSpecific does not pass the lever", "TargetedAttempt is not told",
         mutate(files, "aai", FIRE_SIG, rep("ModeCounter, !BenchLever.IsEmpty());", "ModeCounter);"))),
        ("TryFireSpecific never reads the lever", "the bench lever is not read",
         mutate(files, "aai", FIRE_SIG, rx_sub(r"AnomalyTexCorrupt::TargetedNoModeBenchLever\(Id\)", "FString()"))),
        ("round-robin branch no longer an else of the lever branch", "not an else of the bench-lever branch",
         mutate(files, "aai", FIRE_SIG, rep("else if (Targeted.bRoundRobin)", "if (Targeted.bRoundRobin)"))),
        ("Apply keeps a private lever rule", "does not select through TexCorruptPure::NoModeLever",
         mutate(files, "atc", APPLY_SIG, rx_sub(r"switch \(TexCorruptPure::NoModeLever\(L\.TileProbe, L\.bIdentityRedraw, L\.bIdentity, "
                                                r"bAutoPool, Family == EFamily::UV\)\)",
                                                "switch (L.bIdentity ? TexCorruptPure::ENoModeLever::Identity : "
                                                "TexCorruptPure::ENoModeLever::None)"))),
        ("TargetedNoModeBenchLever asks as an auto-pool pick", "bAutoPool=false",
         mutate(files, "state", LEVER_SIG, rep("GLevers.bIdentity, false, Id == Uv)", "GLevers.bIdentity, true, Id == Uv)"))),
    ]


def selftest(files):
    ok = True
    clean = check(files)
    print(f"M53-GLUE SELFTEST clean copy -> {'PASS' if not clean else 'FAIL'}")
    for f in clean:
        print(f"M53-GLUE SELFTEST   clean failure: {f}")
    ok &= not clean
    mutants = build_mutants(files)
    for name, expect, mutant in mutants:
        if mutant is None:
            print(f"M53-GLUE SELFTEST NOT-BUILT {name} (the planted edit did not apply to this tree)")
            ok = False
            continue
        f = check(mutant)
        hit = [x for x in f if expect in x]
        if hit:
            print(f"M53-GLUE SELFTEST OK {name} -> FAILED as expected [{hit[0]}]")
        else:
            print(f"M53-GLUE SELFTEST BLIND {name} -> {'failed elsewhere: ' + f[0] if f else 'PASSED'}")
            ok = False
    print(f"M53-GLUE SELFTEST {1 + len(mutants)} case(s): {'OK' if ok else 'FAILED'}")
    return ok


def main():
    ap = argparse.ArgumentParser(description="m53 engine glue: the exclusion provider reads retained m53 fires and FireWindow "
                                             "snapshots, and a targeted no-mode fire leaves the mode to an active bench lever.")
    ap.add_argument("--root", default=str(Path(__file__).resolve().parent.parent))
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    root = Path(a.root)
    files = {k: (root / p).read_text(encoding="utf-8", errors="surrogateescape").replace("\r\n", "\n") for k, p in PATHS.items()}
    if a.selftest:
        sys.exit(0 if selftest(files) else 1)
    failures = check(files)
    for f in failures:
        print(f"M53-GLUE {root} FAIL {f}")
    print(f"M53-GLUE {root} VERDICT {'FAIL' if failures else 'PASS'}")
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
