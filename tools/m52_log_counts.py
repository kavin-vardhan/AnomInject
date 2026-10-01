"""m52_log_counts.py - stuck_low_mip readings from a game log. It prints numbers only.

  python m52_log_counts.py "<the game's log>"
  python m52_log_counts.py --selftest

The same readings as the office PowerShell counter (090-04b): the lines that mention stuck_low_mip or Capture(m52)
are selected and counted by the plugin's own log wording. No path, object or texture name is printed.
Standard library only (Python 3.8 or newer). Exit codes: 0 read, 1 selftest failed, 2 cannot read the log.
"""
import argparse
import os
import re
import sys
import tempfile

SELECT = re.compile(r"stuck_low_mip|Capture\(m52\)", re.I)
HOLDING = re.compile(r" - HOLDING \d+ of \d+", re.I)
BUCKETS = re.compile(r"\] - (\d+) virtual, (\d+) not streamable[^,]*, (\d+) excluded[^,]*, (\d+) shared[^,]*, "
                     r"(\d+) too small[^,]*, (\d+) imperceptible", re.I)
SHARED = re.compile(r"shared_world - (\d+) user", re.I)
BUCKET_NAMES = ("virtual", "not streamable", "excluded", "shared", "too small", "imperceptible")

FORMATS = {
    "purity": (
        "stuck_low_mip: PURITY ENUMERATION for '%s' - world '%s' (type=%s), scope ALL LOADED LEVELS OF THAT WORLD (%d, "
        "active or not): %d component(s) of every actor in them scanned, REGISTERED OR NOT (primitives of every "
        "type, plus decals), for %d candidate texture(s) in %.2f ms; %d user(s) sit in an inactive loaded level "
        "and %d are unregistered. Other worlds (in Play In Editor, the editor's own world) are not scanned and "
        "their users are not counted. A texture is held only if it has exactly ONE user component in that scope "
        "and that component belongs to the target. Visibility is NOT consulted: an off-screen user still shows the "
        "blur the moment the camera turns to it."),
    "holding": (
        "stuck_low_mip: matched %d component(s) for '%s' - HOLDING %d of %d candidate texture(s) [%s]; "
        "%d virtual, %d not streamable, %d excluded group, %d shared, %d too small for the ratio, %d "
        "imperceptible at an explicit depth. %d owning actor(s) are WATCHED for destruction via "
        "AActor::OnEndPlay. The label is driven by the MEASURED resident mip count, so frames before the "
        "streamer completes the stream-out are NOT labelled."),
    "held_none": (
        "stuck_low_mip: matched %d component(s) for '%s' with %d candidate texture(s) but HELD NONE [%s] - "
        "%d virtual, %d not streamable or already at the floor, %d excluded LOD group, %d shared with another "
        "user component in this world, %d too small for the ratio at the deepest achievable hold, %d imperceptible at "
        "the explicitly requested depth. Applying nothing, so no fire is recorded and no label is written."),
    "shared_world": (
        "stuck_low_mip: REFUSED TEXTURE '%s' shared_world - %d user component(s) in the whole loaded "
        "world (%d not a target component), and the rule is exactly ONE. Holding it would blur every "
        "other user, on screen or not, while the label and mask name only the target. Users: [ %s]. "
        "The gate is PER TEXTURE, so the target's other textures are still eligible."),
    "baseline_pending": (
        "stuck_low_mip: REFUSED TEXTURE '%s' baseline_pending - a stream operation is IN FLIGHT, so the "
        "game-thread resident count (%d) is not yet the count the renderer draws with. A baseline read "
        "now could be the pre- or post-transition value, and every later frame's held/restored verdict "
        "is measured against it. Refusing costs one fire; a wrong baseline costs a wrong label."),
    "contaminated": (
        "stuck_low_mip: HOLD CONTAMINATED - %s while '%s' holds it. The purity rule (exactly one user component) "
        "no longer holds, so the blur now reaches an object the label and mask do not name. REVERTING THE HOLD NOW; "
        "every labelled frame from here until the render record shows the texture back at baseline is flagged "
        "stuck_mip.contaminated = 1."),
    "capture_contaminated": (
        "Capture(m52): HOLD CONTAMINATED event=%s from si=%d - %s. The hold was reverted immediately; every "
        "labelled frame of this event from si=%d until its render record shows baseline carries "
        "stuck_mip.contaminated = 1."),
    "matched0": "stuck_low_mip: matched 0 mesh component(s) for '%s'.",
    "already_held": (
        "stuck_low_mip: REFUSED already_held - '%s' resolves to '%s', which is the target this anomaly is "
        "HOLDING RIGHT NOW (%d texture(s)). Reverting and re-applying in one call would restart the hold "
        "with a baseline read from the ALREADY-HELD state, so the second event would record a baseline "
        "that is itself the anomaly. No fire is recorded and nothing is changed."),
    "lod_matched0": "lod_corruption: matched 0 mesh component(s) for '%s'.",
}

SOURCES = {
    "purity": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "stuck_low_mip: PURITY ENUMERATION for '%s'"),
    "holding": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "for '%s' - HOLDING %d of %d"),
    "held_none": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "but HELD NONE [%s]"),
    "shared_world": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "shared_world - %d user component(s)"),
    "baseline_pending": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "baseline_pending - a stream operation"),
    "contaminated": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "stuck_low_mip: HOLD CONTAMINATED - %s"),
    "capture_contaminated": ("AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp", "Capture(m52): HOLD CONTAMINATED event="),
    "matched0": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "stuck_low_mip: matched 0 mesh component(s)"),
    "already_held": ("AnomalyInjector/Private/Anomalies/Anomaly_StuckLowMip.cpp", "stuck_low_mip: REFUSED already_held"),
    "lod_matched0": ("AnomalyInjector/Private/Anomalies/Anomaly_LodCorruption.cpp", "lod_corruption: matched 0 mesh component(s)"),
}

TEXT_LIT = re.compile(r'TEXT\("((?:[^"\\]|\\.)*)"\)')


def detect_encoding(head):
    if head.startswith(b"\xef\xbb\xbf"):
        return "utf-8-sig"
    if head.startswith(b"\xff\xfe"):
        return "utf-16"
    if head.startswith(b"\xfe\xff"):
        return "utf-16"
    if len(head) >= 4 and head[1:2] == b"\x00" and head[3:4] == b"\x00":
        return "utf-16-le"
    return "utf-8"


def read_lines(path):
    with open(path, "rb") as f:
        head = f.read(4)
    with open(path, "r", encoding=detect_encoding(head), errors="replace", newline=None) as f:
        for line in f:
            yield line.rstrip("\n")


def count(lines):
    total = 0
    sel = []
    for line in lines:
        total += 1
        if SELECT.search(line):
            sel.append(line)
    low = [l.lower() for l in sel]
    held_none = [l for l, x in zip(sel, low) if "held none" in x]
    buckets = [0] * 6
    for l in held_none:
        m = BUCKETS.search(l)
        if m:
            for i in range(6):
                buckets[i] += int(m.group(i + 1))
    shared = []
    for l in sel:
        m = SHARED.search(l)
        if m:
            shared.append(int(m.group(1)))
    return dict(
        lines=total, selected=len(sel),
        purity=sum(1 for x in low if "purity enumeration" in x),
        holding=sum(1 for l in sel if HOLDING.search(l)),
        held_none=len(held_none), buckets=buckets, shared=shared,
        baseline_pending=sum(1 for x in low if "baseline_pending -" in x),
        contaminated=sum(1 for x in low if "hold contaminated" in x),
        matched0=sum(1 for x in low if "stuck_low_mip: matched 0 mesh" in x))


def render(c):
    sw = c["shared"]
    out = [
        "PURITY ENUMERATION        %d" % c["purity"],
        "HOLDING                   %d" % c["holding"],
        "HELD NONE                 %d" % c["held_none"],
        "  HELD NONE buckets       " + " | ".join("%s %d" % (n, v) for n, v in zip(BUCKET_NAMES, c["buckets"])),
        "shared_world              %d" % len(sw),
        "  users 2 to 8            %d" % sum(1 for v in sw if v <= 8),
        "  users 9 or more         %d" % sum(1 for v in sw if v >= 9),
    ]
    if sw:
        out.append("  users min / max         %d / %d" % (min(sw), max(sw)))
    out += [
        "baseline_pending          %d" % c["baseline_pending"],
        "HOLD CONTAMINATED         %d" % c["contaminated"],
        "matched 0 mesh            %d" % c["matched0"],
        "lines read %d | stuck_low_mip or Capture(m52) lines %d" % (c["lines"], c["selected"]),
    ]
    return out


def source_formats(src):
    res = {}
    for key, (rel, anchor) in SOURCES.items():
        path = os.path.join(src, rel)
        if not os.path.isfile(path):
            return None
        with open(path, encoding="utf-8-sig") as f:
            text = f.read()
        groups, cur, last = [], [], None
        for m in TEXT_LIT.finditer(text):
            if cur and text[last:m.start()].strip() == "":
                cur.append(m.group(1))
            else:
                if cur:
                    groups.append("".join(cur))
                cur = [m.group(1)]
            last = m.end()
        if cur:
            groups.append("".join(cur))
        hits = [g for g in groups if anchor in g]
        res[key] = hits[0] if len(hits) == 1 else None
    return res


SAMPLE = [
    ("Log", "purity", ("SM_Rock", "MainWorld", "PIE", 3, 812, 4, 1.25, 0, 0)),
    ("Log", "purity", ("SM_Pipe", "MainWorld", "Game", 3, 812, 2, 0.75, 1, 0)),
    ("Log", "holding", (1, "SM_Rock", 2, 3, "auto-pool, gates ENFORCED", 0, 0, 0, 1, 0, 0, 1)),
    ("Warning", "held_none", (1, "SM_Crate", 3, "auto-pool, gates ENFORCED", 1, 0, 0, 2, 1, 0)),
    ("Warning", "held_none", (2, "SM_Pipe", 2, "targeted, selection gates BYPASSED", 0, 1, 0, 1, 0, 0)),
    ("Warning", "shared_world", ("T_Rock_N", 3, 2, "A.M0 B.M0 ")),
    ("Warning", "shared_world", ("T_Rock_D", 8, 7, "A.M0 B.M0 C.M0 D.M0 ")),
    ("Warning", "shared_world", ("T_Crate_D", 9, 8, "A.M0 ")),
    ("Warning", "shared_world", ("T_Wall_D", 12, 11, "A.M0 ")),
    ("Warning", "baseline_pending", ("T_Pipe_D", 7)),
    ("Warning", "contaminated", ("new user A.M1(StaticMeshComponent) of held texture 'T_Rock_D' appeared mid-hold (register)",
                                 "SM_Rock")),
    ("Warning", "capture_contaminated", ("stuck_low_mip@1234|SM_Rock", 57, "new user A.M1", 57)),
    ("Log", "matched0", ("SM_Nothing",)),
    ("Warning", "already_held", ("SM_Rock", "SM_Rock", 2)),
    ("Log", "lod_matched0", ("SM_Nothing",)),
]
SAMPLE_CATEGORY = {"capture_contaminated": "LogAnomalyCapture"}
SAMPLE_PLAIN = [
    "[2026.09.30-12.00.00:001][  0]LogAnomalyCapture: Capture(m52): EFFECTIVE FOR THIS RUN - stuck_low_mip label source RENDER RECORD",
    "[2026.09.30-12.00.00:002][  0]LogTemp: HELD NONE and HOLDING 1 of 2 written by something else",
]
SAMPLE_EXPECT = dict(purity=2, holding=1, held_none=2, buckets=[1, 1, 0, 3, 1, 0], shared=[3, 8, 9, 12],
                     baseline_pending=1, contaminated=2, matched0=1)


def sample_lines(formats):
    out = list(SAMPLE_PLAIN)
    for i, (verb, key, args) in enumerate(SAMPLE):
        cat = SAMPLE_CATEGORY.get(key, "LogAnomaly")
        pre = "[2026.09.30-12.00.%02d:%03d][%3d]%s: " % (1 + i, 10 * i, 10 + i, cat)
        if verb != "Log":
            pre += verb + ": "
        out.append(pre + (formats[key] % args))
    return out


def selftest():
    lines = []
    ok_all = True

    def check(label, good):
        nonlocal ok_all
        ok_all = ok_all and good
        lines.append("SELFTEST %-70s %s" % (label, "ok" if good else "*** WRONG ***"))

    src = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "Source")
    got = source_formats(src) if os.path.isdir(src) else None
    if got is None:
        lines.append("SELFTEST %-70s %s" % ("format strings compared with the plugin source", "skipped (no Source beside this tool)"))
    else:
        same = [k for k in FORMATS if got.get(k) == FORMATS[k]]
        check("format strings equal the plugin source's own (%d of %d)" % (len(same), len(FORMATS)), len(same) == len(FORMATS))
    text = sample_lines(FORMATS)
    tmp = tempfile.mkdtemp(prefix="m52_counts_")
    try:
        for enc, name in (("utf-8-sig", "UTF-8 with BOM"), ("utf-16", "UTF-16"), ("utf-8", "UTF-8 without BOM")):
            p = os.path.join(tmp, "game_%s.log" % enc)
            with open(p, "w", encoding=enc, newline="\r\n") as f:
                f.write("\n".join(text) + "\n")
            c = count(read_lines(p))
            good = all(c[k] == v for k, v in SAMPLE_EXPECT.items()) and c["lines"] == len(text)
            check("sample log (%s): PURITY 2, HOLDING 1, HELD NONE 2, buckets 1/1/0/3/1/0" % name, good)
        c = count(text)
        r = render(c)
        want = [
            "PURITY ENUMERATION        2", "HOLDING                   1", "HELD NONE                 2",
            "  HELD NONE buckets       virtual 1 | not streamable 1 | excluded 0 | shared 3 | too small 1 | imperceptible 0",
            "shared_world              4", "  users 2 to 8            2", "  users 9 or more         2",
            "  users min / max         3 / 12", "baseline_pending          1", "HOLD CONTAMINATED         2",
            "matched 0 mesh            1"]
        check("printed lines equal the PowerShell counter's for the sample", r[:len(want)] == want)
        check("HOLDING RIGHT NOW (an already-held refusal) is not counted as HOLDING", c["holding"] == 1)
        check("lod_corruption's matched 0 mesh is not counted", c["matched0"] == 1)
        check("lines naming neither stuck_low_mip nor Capture(m52) are not selected (2 of %d)" % len(text),
              c["selected"] == len(text) - 2)
        check("output carries no object, texture or path name",
              not any(re.search(r"SM_|T_[A-Z]|\\|\.log", l) for l in r))
    finally:
        for fn in os.listdir(tmp):
            os.remove(os.path.join(tmp, fn))
        os.rmdir(tmp)
    for l in lines:
        print(l)
    print("SELFTEST %d check(s): %s" % (len(lines), "OK" if ok_all else "FAILED"))
    return 0 if ok_all else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="stuck_low_mip readings from a game log: numbers only.")
    ap.add_argument("log", nargs="?", help="the game's log file")
    ap.add_argument("--selftest", action="store_true", help="run the known-answer self test")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest()
    if not a.log:
        ap.print_help()
        return 2
    if not os.path.isfile(a.log):
        print("cannot read the log: file not found")
        return 2
    for l in render(count(read_lines(a.log))):
        print(l)
    return 0


if __name__ == "__main__":
    sys.exit(main())
