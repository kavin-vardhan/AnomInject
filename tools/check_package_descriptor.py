#!/usr/bin/env python3
"""G354 package check: the SHIPPED plugin descriptor lists exactly the intended modules, and no bench fixture is cooked.

A packaged game starts only the modules its COOKED AnomalyInjector.uplugin lists, and a console command whose
module never started is dropped from -ExecCmds without a word (G354). So the descriptor is read OUT OF THE
PACKAGE, never off the source tree, and compared module by module with the intended set: the delivered
branch's descriptor MINUS AnomalyBench (ruling F9). A missing module, an extra module (AnomalyBench above all)
or any differing field is a STOP.

The same run scans the container index (.utoc, both encodings) for bench fixture content: the fixture
folder names AnomalyFixtures and CaptureBenchGate (the index stores path components without slashes) and the
bench maps. Any hit in a delivery package is
a STOP. A BENCH cook legitimately carries AnomalyBench and the fixtures: pass --bench-cook, and the descriptor
must then equal the branch descriptor exactly.

  python check_package_descriptor.py --build <packaged build root>
  python check_package_descriptor.py --pak <...-Windows.pak> [--utoc <...-Windows.utoc>]
  python check_package_descriptor.py --descriptor <AnomalyInjector.uplugin> [--utoc <...>]
  python check_package_descriptor.py --selftest

Exit 0 PASS, 1 STOP, 2 cannot read (no verdict).
"""
import argparse
import glob
import hashlib
import json
import os
import shutil
import subprocess
import sys
import tempfile

BENCH_MODULES = ("AnomalyBench",)
FIXTURE_NEEDLES = ("AnomalyFixtures", "CaptureBenchGate", "CB_GateLevel", "CB_LodCalib", "CB_LodFixture",
                   "CB_TexCorruptLevel")
DEFAULT_UNREALPAK = r"D:\UESource\UnrealEngine\Engine\Binaries\Win64\UnrealPak.exe"
COMPARED_TOP = ("Modules", "Plugins")


def sha256(data):
    return hashlib.sha256(data).hexdigest().upper()


def load_json_bytes(data):
    return json.loads(data.decode("utf-8-sig"))


def intended_from(path, bench_cook):
    with open(path, "rb") as f:
        d = load_json_bytes(f.read())
    if not bench_cook:
        d["Modules"] = [m for m in d.get("Modules", []) if m.get("Name") not in BENCH_MODULES]
    return d


def extract_descriptor(pak, unrealpak, out):
    if not os.path.isfile(unrealpak):
        out("G354 REFUSED UnrealPak not found at %s (pass --unrealpak)" % unrealpak)
        return None
    tmp = tempfile.mkdtemp(prefix="g354_")
    try:
        r = subprocess.run([unrealpak, pak, "-Extract", tmp, "-Filter=*.uplugin"], capture_output=True, text=True)
        hits = [p for p in glob.glob(os.path.join(tmp, "**", "AnomalyInjector.uplugin"), recursive=True)]
        if r.returncode != 0 or len(hits) != 1:
            out("G354 REFUSED UnrealPak exit %d, %d AnomalyInjector.uplugin found in %s" % (r.returncode, len(hits), pak))
            return None
        with open(hits[0], "rb") as f:
            return f.read()
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def compare(shipped, intended, out):
    stops = []
    ship_mods = {m.get("Name"): m for m in shipped.get("Modules", [])}
    want_mods = {m.get("Name"): m for m in intended.get("Modules", [])}
    out("G354 SHIPPED  modules=%d: %s" % (len(ship_mods), ", ".join(m.get("Name") for m in shipped.get("Modules", []))))
    out("G354 INTENDED modules=%d: %s" % (len(want_mods), ", ".join(m.get("Name") for m in intended.get("Modules", []))))
    for name in want_mods:
        if name not in ship_mods:
            stops.append("MISSING %s" % name)
            out("G354 MODULE %-22s MISSING from the shipped descriptor -> STOP (a module the build needs never starts)" % name)
            continue
        diffs = []
        for key in sorted(set(want_mods[name]) | set(ship_mods[name])):
            if want_mods[name].get(key) != ship_mods[name].get(key):
                diffs.append("%s intended=%s shipped=%s" % (key, json.dumps(want_mods[name].get(key)), json.dumps(ship_mods[name].get(key))))
        if diffs:
            stops.append("DIFF %s" % name)
            out("G354 MODULE %-22s DIFFERS -> STOP: %s" % (name, "; ".join(diffs)))
        else:
            out("G354 MODULE %-22s identical" % name)
    for name in ship_mods:
        if name not in want_mods:
            stops.append("EXTRA %s" % name)
            why = "the bench module must not ship (F9)" if name in BENCH_MODULES else "not in the intended set"
            out("G354 MODULE %-22s EXTRA in the shipped descriptor -> STOP (%s)" % (name, why))
    if shipped.get("Plugins") != intended.get("Plugins"):
        stops.append("PLUGINS")
        out("G354 PLUGINS differ -> STOP: intended=%s shipped=%s" % (json.dumps(intended.get("Plugins")), json.dumps(shipped.get("Plugins"))))
    else:
        out("G354 PLUGINS identical: %s" % json.dumps(shipped.get("Plugins")))
    return stops


def scan_fixtures(data, out):
    hits = []
    for needle in FIXTURE_NEEDLES:
        a = data.count(needle.encode("ascii"))
        u = data.count(needle.encode("utf-16-le"))
        if a + u:
            hits.append("%s(ascii=%d utf16=%d)" % (needle, a, u))
    return hits


def check(descriptor_bytes, source, intended_path, utoc, bench_cook, out):
    try:
        shipped = load_json_bytes(descriptor_bytes)
        intended = intended_from(intended_path, bench_cook)
    except Exception as e:
        out("G354 REFUSED cannot parse a descriptor: %s" % e)
        return 2
    out("G354 DESCRIPTOR source=%s sha256=%s bytes=%d" % (source, sha256(descriptor_bytes), len(descriptor_bytes)))
    out("G354 INTENDED from %s%s" % (intended_path, " (bench cook: AnomalyBench expected)" if bench_cook else " minus AnomalyBench (F9)"))
    stops = compare(shipped, intended, out)
    if utoc:
        with open(utoc, "rb") as f:
            data = f.read()
        hits = scan_fixtures(data, out)
        out("G354 FIXTURE container=%s sha256=%s hits=%s" % (utoc, sha256(data)[:16], ", ".join(hits) if hits else "none"))
        if hits and not bench_cook:
            stops.append("FIXTURE")
            out("G354 FIXTURE bench fixture content is cooked into this package -> STOP (exclude /Game/AnomalyFixtures, "
                "/Game/CaptureBenchGate and the CB_ maps from the delivery cook)")
    else:
        out("G354 FIXTURE not checked (no container index given)")
    out("G354 VERDICT %s%s" % ("PASS" if not stops else "STOP", "" if not stops else " " + ", ".join(stops)))
    return 0 if not stops else 1


def selftest(intended_path):
    with open(intended_path, "rb") as f:
        branch = load_json_bytes(f.read())
    delivery = json.loads(json.dumps(branch))
    delivery["Modules"] = [m for m in delivery["Modules"] if m["Name"] not in BENCH_MODULES]
    missing = json.loads(json.dumps(delivery))
    missing["Modules"] = [m for m in missing["Modules"] if m["Name"] != "AnomalyCapture"]
    phase = json.loads(json.dumps(delivery))
    for m in phase["Modules"]:
        if m["Name"] == "AnomalyShaders":
            m["LoadingPhase"] = "Default"
    noplug = json.loads(json.dumps(delivery))
    noplug["Plugins"] = []
    clean_utoc = "/Game/StackOBot/Maps/MainWorld".encode("utf-16-le")
    fixture_utoc = clean_utoc + "/Game/AnomalyFixtures/CB_LodFixture".encode("utf-16-le")
    cases = [
        ("delivery descriptor, clean container", delivery, clean_utoc, False, 0),
        ("branch descriptor (AnomalyBench present)", branch, clean_utoc, False, 1),
        ("a needed module missing (AnomalyCapture)", missing, clean_utoc, False, 1),
        ("a module field changed (LoadingPhase)", phase, clean_utoc, False, 1),
        ("the plugin dependency list dropped", noplug, clean_utoc, False, 1),
        ("delivery descriptor, fixture in container", delivery, fixture_utoc, False, 1),
        ("bench cook: branch descriptor + fixture", branch, fixture_utoc, True, 0),
        ("bench cook: descriptor without AnomalyBench", delivery, fixture_utoc, True, 1),
    ]
    tmp = tempfile.mkdtemp(prefix="g354_selftest_")
    ok_all = True
    try:
        for label, desc, utoc_bytes, bench, want in cases:
            up = os.path.join(tmp, "u.bin")
            with open(up, "wb") as f:
                f.write(utoc_bytes)
            sink = []
            rc = check(json.dumps(desc, indent="\t").encode("utf-8"), "synthetic", intended_path, up, bench, sink.append)
            ok = rc == want
            ok_all = ok_all and ok
            print("G354 SELFTEST %-46s -> %s (exit %d, expected %d)%s" % (
                label, "ok" if ok else "*** WRONG ***", rc, want,
                "" if rc == 0 else "  [" + sink[-1].replace("G354 VERDICT ", "") + "]"))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    print("G354 SELFTEST %d case(s): %s" % (len(cases), "OK" if ok_all else "FAILED"))
    return 0 if ok_all else 1


def main():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    ap = argparse.ArgumentParser(description="G354: shipped descriptor = intended modules, and no bench fixture cooked.")
    src = ap.add_mutually_exclusive_group()
    src.add_argument("--build", help="packaged build root (searched for Content/Paks/*-Windows.pak)")
    src.add_argument("--pak", help="the project .pak holding the cooked descriptor")
    src.add_argument("--descriptor", help="an already-extracted AnomalyInjector.uplugin")
    ap.add_argument("--utoc", help="the project .utoc to scan for fixture content (found beside --pak/--build if omitted)")
    ap.add_argument("--intended", default=os.path.join(here, "AnomalyInjector.uplugin"))
    ap.add_argument("--bench-cook", action="store_true", help="a bench cook: AnomalyBench and fixtures are expected")
    ap.add_argument("--unrealpak", default=DEFAULT_UNREALPAK)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    out = print
    if a.selftest:
        return selftest(a.intended)
    pak = a.pak
    utoc = a.utoc
    if a.build:
        paks = [p for p in glob.glob(os.path.join(a.build, "**", "Content", "Paks", "*-Windows.pak"), recursive=True)
                if not os.path.basename(p).lower().startswith("global")]
        if len(paks) != 1:
            out("G354 REFUSED expected one project .pak under %s, found %d" % (a.build, len(paks)))
            return 2
        pak = paks[0]
    if pak and not utoc:
        cand = os.path.splitext(pak)[0] + ".utoc"
        utoc = cand if os.path.isfile(cand) else None
    if pak:
        data = extract_descriptor(pak, a.unrealpak, out)
        if data is None:
            return 2
        return check(data, pak, a.intended, utoc, a.bench_cook, out)
    if a.descriptor:
        with open(a.descriptor, "rb") as f:
            data = f.read()
        return check(data, a.descriptor, a.intended, utoc, a.bench_cook, out)
    ap.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main())
