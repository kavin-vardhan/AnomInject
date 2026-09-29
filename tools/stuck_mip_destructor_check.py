import argparse
import re
import sys
from pathlib import Path

CPP = "Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp"
HDR = "Source/AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.h"
DEF_RE = re.compile(r"FAnomaly_StuckLowMip::~FAnomaly_StuckLowMip\s*\(\s*\)\s*\{")
CTOR_RE = re.compile(r"FAnomaly_StuckLowMip::FAnomaly_StuckLowMip\s*\(\s*\)\s*\{")
DECL_RE = re.compile(r"virtual\s+~FAnomaly_StuckLowMip\s*\(\s*\)\s*override\s*;")
JOBS = [
    ("registry removal", re.compile(r"LiveStuckMipInstances\s*\(\s*\)\s*\.\s*RemoveSingleSwap\s*\(\s*this\s*\)\s*;")),
    ("hold monitor stop", re.compile(r"\bStopHoldMonitor\s*\(\s*\)\s*;")),
]
CTOR_JOB = re.compile(r"LiveStuckMipInstances\s*\(\s*\)\s*\.\s*Add\s*\(\s*this\s*\)\s*;")


def body_after(text, m):
    depth = 0
    i = m.end() - 1
    start = i
    while i < len(text):
        c = text[i]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return text[start + 1:i]
        i += 1
    return None


def check(cpp_text, hdr_text):
    failures = []
    defs = list(DEF_RE.finditer(cpp_text))
    if len(defs) != 1:
        failures.append(f"destructor definitions: {len(defs)} (expected exactly 1)")
    decls = DECL_RE.findall(hdr_text)
    if len(decls) != 1:
        failures.append(f"destructor declarations in the header: {len(decls)} (expected exactly 1, virtual ... override)")
    if defs:
        body = body_after(cpp_text, defs[0]) or ""
        for name, rx in JOBS:
            n = len(rx.findall(body))
            if n != 1:
                failures.append(f"destructor body: {name} called {n} time(s) (expected exactly 1)")
    ctors = list(CTOR_RE.finditer(cpp_text))
    if len(ctors) != 1:
        failures.append(f"constructor definitions: {len(ctors)} (expected exactly 1)")
    else:
        cbody = body_after(cpp_text, ctors[0]) or ""
        if len(CTOR_JOB.findall(cbody)) != 1:
            failures.append("constructor body: registry add not found exactly once")
    return failures


def report(label, failures):
    if failures:
        for f in failures:
            print(f"STUCKMIP-DTOR {label} FAIL {f}")
        print(f"STUCKMIP-DTOR {label} VERDICT FAIL ({len(failures)} failure(s))")
    else:
        print(f"STUCKMIP-DTOR {label} VERDICT PASS (one destructor; it removes the registry entry and stops the hold monitor)")


def selftest(cpp_text, hdr_text):
    ok = True
    clean = check(cpp_text, hdr_text)
    print(f"STUCKMIP-DTOR SELFTEST clean copy -> {'PASS' if not clean else 'FAIL'}")
    ok &= not clean
    m = DEF_RE.search(cpp_text)
    body = body_after(cpp_text, m) if m else None
    if body is None:
        print("STUCKMIP-DTOR SELFTEST cannot locate the destructor body in the clean copy")
        return False
    full_def = cpp_text[m.start():m.end()] + body + "}"
    mutants = []
    b1 = JOBS[0][1].sub("", body, count=1)
    mutants.append(("registry removal dropped", cpp_text.replace(body, b1, 1), hdr_text))
    b2 = JOBS[1][1].sub("", body, count=1)
    mutants.append(("hold monitor stop dropped", cpp_text.replace(body, b2, 1), hdr_text))
    mutants.append(("second destructor definition (the merge shape)", cpp_text + "\n" + full_def + "\n", hdr_text))
    mutants.append(("header declaration duplicated", cpp_text, hdr_text.replace(DECL_RE.search(hdr_text).group(0), DECL_RE.search(hdr_text).group(0) + "\n\t" + DECL_RE.search(hdr_text).group(0), 1) if DECL_RE.search(hdr_text) else hdr_text + "\nvirtual ~FAnomaly_StuckLowMip() override;\nvirtual ~FAnomaly_StuckLowMip() override;\n"))
    mutants.append(("stop called twice", cpp_text.replace(body, body + "\n\tStopHoldMonitor();\n", 1), hdr_text))
    for name, c, h in mutants:
        f = check(c, h)
        fired = bool(f)
        print(f"STUCKMIP-DTOR SELFTEST {name} -> {'FAILED as expected' if fired else 'PASSED (the check is blind)'}")
        ok &= fired
    print(f"STUCKMIP-DTOR SELFTEST {1 + len(mutants)} case(s): {'OK' if ok else 'FAILED'}")
    return ok


def main():
    ap = argparse.ArgumentParser(description="The merged FAnomaly_StuckLowMip has ONE destructor that does both jobs.")
    ap.add_argument("--root", default=str(Path(__file__).resolve().parent.parent))
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    root = Path(a.root)
    cpp_text = (root / CPP).read_text(encoding="utf-8", errors="surrogateescape").replace("\r\n", "\n")
    hdr_text = (root / HDR).read_text(encoding="utf-8", errors="surrogateescape").replace("\r\n", "\n")
    if a.selftest:
        sys.exit(0 if selftest(cpp_text, hdr_text) else 1)
    failures = check(cpp_text, hdr_text)
    report(str(root), failures)
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
