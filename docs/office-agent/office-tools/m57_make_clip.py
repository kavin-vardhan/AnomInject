"""Look-first clip for one m57 leg (095-08 plan section 8.3): the anomaly leg with its label boxes beside its null twin.

  python m57_make_clip.py --verify <plugin checkout>/tools/verify_capture.py --anomaly <session> [--null <session>] --out <folder> [--name clip]
  python m57_make_clip.py --verify <plugin checkout>/tools/verify_capture.py --selftest

Step 1  verify_capture.py --dir <anomaly> --out <out>/<name>_overlay draws the label boxes (red = labelled, amber =
        transition-only) on copies of the anomaly leg's frames. The delivered frames are never edited, and nothing is
        written inside the session folder. The tool writes frame_NNNNN_annotated.png, and only for frames that carry a
        box, so the sequence it writes has gaps. --verify names that tool; use the plugin checkout at the tested commit.
Step 2  The sequence is filled: <out>/<name>_frames/frame_NNNNN.png is the annotated copy where one exists and the raw
        Actual_Frames frame otherwise, for every session index from 0.
Step 3  ffmpeg -framerate 30 -start_number 0 -i frame_%05d.png encodes the filled sequence and the null leg (the call the
        dashboard's encode_watcher.py makes), libx264, yuv420p.
Step 4  ffmpeg hstack puts anomaly | null side by side into <out>/<name>.mp4.
Without --null (a played path has no null twin) the filled anomaly sequence alone is encoded into <out>/<name>.mp4.
Refuses (exit 3) when --verify does not name a file, a leg has no frames, the two legs' frame sets differ, the frames
are not contiguous from 0, or --out is inside a leg's folder: a side-by-side of unmatched frames would show a difference
that is not the anomaly.
--dry-run prints the plan. --selftest runs the real overlay tool on a synthetic leg, fills its sequence and, when
ffmpeg is found, encodes the clip, with and without a null twin.
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

FRAME = re.compile(r"^frame_(\d{5})\.png$")
ANNOTATED = re.compile(r"^frame_(\d{5})_annotated\.png$")


def frames(directory):
    if not os.path.isdir(directory):
        return set()
    return set(int(m.group(1)) for m in (FRAME.match(n) for n in os.listdir(directory)) if m)


def inside(path, folder):
    path, folder = os.path.normcase(os.path.abspath(path)), os.path.normcase(os.path.abspath(folder))
    return path == folder or path.startswith(folder.rstrip("\\/") + os.sep)


def plan(anomaly, null, out, name, verify, ffmpeg="ffmpeg", fps=30):
    if not verify or not os.path.isfile(verify):
        return None, "--verify %s is not a file; name verify_capture.py in the plugin checkout's tools folder" % verify
    fa = frames(os.path.join(anomaly, "Actual_Frames"))
    if null is None:
        if not fa:
            return None, "the anomaly leg has no frames"
    else:
        fn = frames(os.path.join(null, "Actual_Frames"))
        if not fa or not fn:
            return None, "a leg has no frames (anomaly %d, null %d)" % (len(fa), len(fn))
        if fa != fn:
            only_a, only_n = sorted(fa - fn), sorted(fn - fa)
            return None, "frame sets differ: anomaly-only %s null-only %s" % (only_a[:5], only_n[:5])
    if min(fa) != 0 or max(fa) != len(fa) - 1:
        return None, "frames are not contiguous from 0 (%d frames, %d..%d)" % (len(fa), min(fa), max(fa))
    if inside(out, anomaly) or (null is not None and inside(out, null)):
        return None, "--out %s is inside a leg's folder; the clip is written beside the sessions, never in them" % out
    overlay = os.path.join(out, name + "_overlay")
    filled = os.path.join(out, name + "_frames")
    a_mp4 = os.path.join(out, name + "_anomaly.mp4")
    n_mp4 = os.path.join(out, name + "_null.mp4")
    sbs = os.path.join(out, name + ".mp4")
    enc = lambda src, dst: [ffmpeg, "-y", "-loglevel", "error", "-framerate", str(fps), "-start_number", "0", "-i",
                            os.path.join(src, "frame_%05d.png"), "-c:v", "libx264", "-pix_fmt", "yuv420p", dst]
    if null is None:
        return [
            ("clean", overlay),
            ("run", [sys.executable, verify, "--dir", anomaly, "--out", overlay, "--quiet"]),
            ("fill", (anomaly, overlay, filled, len(fa))),
            ("run", enc(filled, sbs)),
        ], "%d frames, anomaly leg alone (no null twin)" % len(fa)
    steps = [
        ("clean", overlay),
        ("run", [sys.executable, verify, "--dir", anomaly, "--out", overlay, "--quiet"]),
        ("fill", (anomaly, overlay, filled, len(fa))),
        ("run", enc(filled, a_mp4)),
        ("run", enc(os.path.join(null, "Actual_Frames"), n_mp4)),
        ("run", [ffmpeg, "-y", "-loglevel", "error", "-i", a_mp4, "-i", n_mp4, "-filter_complex", "hstack=inputs=2",
                 "-c:v", "libx264", "-pix_fmt", "yuv420p", sbs]),
    ]
    return steps, "%d frames" % len(fa)


def clean(folder, pattern):
    if os.path.isdir(folder):
        for f in os.listdir(folder):
            if pattern.match(f):
                os.remove(os.path.join(folder, f))


def fill(anomaly, overlay, filled, n):
    clean(filled, FRAME)
    os.makedirs(filled, exist_ok=True)
    annotated = 0
    for si in range(n):
        src = os.path.join(overlay, "frame_%05d_annotated.png" % si)
        if os.path.isfile(src):
            annotated += 1
        else:
            src = os.path.join(anomaly, "Actual_Frames", "frame_%05d.png" % si)
        shutil.copyfile(src, os.path.join(filled, "frame_%05d.png" % si))
    return annotated


def execute(steps, dry_run=False):
    for kind, step in steps:
        if kind == "clean":
            print("M57-CLIP clear earlier frame_NNNNN_annotated.png in %s" % step)
            if not dry_run:
                clean(step, ANNOTATED)
            continue
        if kind == "fill":
            print("M57-CLIP fill %s from %s + %s/Actual_Frames (%d frames)" % (step[2], step[1], step[0], step[3]))
            if not dry_run:
                n = fill(*step)
                print("M57-CLIP filled: %d annotated frame(s), %d raw" % (n, step[3] - n))
            continue
        print("M57-CLIP " + " ".join(step))
        if not dry_run:
            r = subprocess.run(step, stdout=subprocess.DEVNULL)
            if r.returncode != 0:
                print("M57-CLIP FAILED at that step (exit %d)" % r.returncode)
                return 1
    return 0


def _leg(root, name, n, labelled=(), with_labels=False):
    from PIL import Image
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    rows = []
    for si in range(n):
        Image.new("RGB", (64, 48), (90, 90, 90 + si)).save(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si))
        ents = []
        if si in labelled:
            ents.append({"id": "lod_popping", "target_name": "Crate", "event_id": "lod_popping@3|Crate", "bbox_valid": True,
                         "bbox_px": [10, 8, 30, 24], "labelled": True})
        rows.append({"session_index": si, "frame_index": 100 + si, "image": "Actual_Frames/frame_%05d.png" % si,
                     "anomaly_present": bool(ents), "anomalies": ents})
    if with_labels:
        with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
            for r in rows:
                fh.write(json.dumps(r) + "\n")
        with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
            json.dump({"label_schema": 2, "label_schema_minor": 3, "anomalies": [
                {"event_id": "lod_popping@3|Crate", "anomaly_type": "lod_popping", "start_frame": 3,
                 "target": {"name": "Crate"}, "injected_frames": {"frame_indices": sorted(labelled)}}]}, fh)
    return d


def selftest(verify, ffmpeg="ffmpeg"):
    if not verify or not os.path.isfile(verify):
        print("M57-CLIP SELFTEST: CANNOT RUN - give --verify <plugin checkout>/tools/verify_capture.py (got %s)" % verify)
        return 2
    root = tempfile.mkdtemp(prefix="m57clip_")
    ok = True
    n_cases = 0

    def report(name, good, why=""):
        nonlocal ok, n_cases
        ok = ok and good
        n_cases += 1
        print("M57-CLIP SELFTEST %-40s -> %s %s" % (name, why, "ok" if good else "BROKEN"))

    try:
        a = _leg(root, "a", 10, labelled=range(3, 7), with_labels=True)
        n = _leg(root, "n", 10)
        out = os.path.join(root, "out")
        steps, why = plan(a, n, out, "c1", verify, ffmpeg)
        report("matched_legs_plan", steps is not None and len(steps) == 6 and steps[5][1][-1].endswith("c1.mp4")
               and "hstack=inputs=2" in steps[5][1] and steps[1][1][5] == os.path.join(out, "c1_overlay"), why)
        os.makedirs(os.path.join(out, "c1_overlay"))
        open(os.path.join(out, "c1_overlay", "frame_00008_annotated.png"), "wb").close()
        execute(steps[:1])
        r = subprocess.run(steps[1][1], capture_output=True, text=True, env=dict(os.environ, PYTHONDONTWRITEBYTECODE="1"))
        overlay = os.path.join(out, "c1_overlay")
        wrote = sorted(os.listdir(overlay)) if os.path.isdir(overlay) else []
        report("real_overlay_tool_writes_annotated_gaps", r.returncode == 0
               and wrote == ["frame_%05d_annotated.png" % i for i in range(3, 7)], "exit %d, %d file(s)" % (r.returncode, len(wrote)))
        report("session_folder_untouched", sorted(os.listdir(a)) == ["Actual_Frames", "annotation.json", "labels.jsonl"])
        got = fill(a, overlay, os.path.join(out, "c1_frames"), 10)
        filled = sorted(os.listdir(os.path.join(out, "c1_frames")))
        same_raw = open(os.path.join(out, "c1_frames", "frame_00001.png"), "rb").read() == \
            open(os.path.join(a, "Actual_Frames", "frame_00001.png"), "rb").read()
        boxed = open(os.path.join(out, "c1_frames", "frame_00004.png"), "rb").read() != \
            open(os.path.join(a, "Actual_Frames", "frame_00004.png"), "rb").read()
        report("fill_is_contiguous_with_boxes", got == 4 and filled == ["frame_%05d.png" % i for i in range(10)]
               and same_raw and boxed, "%d annotated" % got)
        if shutil.which(ffmpeg):
            code = execute(steps[3:])
            mp4 = os.path.join(out, "c1.mp4")
            report("ffmpeg_encodes_side_by_side", code == 0 and os.path.isfile(mp4) and os.path.getsize(mp4) > 0,
                   "exit %d" % code)
        else:
            print("M57-CLIP SELFTEST ffmpeg not found: the encode steps were not exercised")
        for name, legs, o in (("frame_sets_differ_REFUSED", (a, _leg(root, "n2", 9)), out),
                              ("empty_leg_REFUSED", (a, _leg(root, "n3", 0)), out),
                              ("out_inside_session_REFUSED", (a, n), os.path.join(a, "clip"))):
            s, why = plan(legs[0], legs[1], o, "x", verify, ffmpeg)
            report(name, s is None, why)
        g = _leg(root, "a4", 4)
        os.remove(os.path.join(g, "Actual_Frames", "frame_00002.png"))
        h = _leg(root, "n4", 4)
        os.remove(os.path.join(h, "Actual_Frames", "frame_00002.png"))
        s, why = plan(g, h, out, "x", verify, ffmpeg)
        report("gap_in_frames_REFUSED", s is None, why)
        s, why = plan(a, n, out, "x", os.path.join(root, "no_verify_capture.py"), ffmpeg)
        report("verify_not_a_file_REFUSED", s is None, why)
        alone, why = plan(a, None, out, "c2", verify, ffmpeg)
        report("anomaly_alone_plan", alone is not None and len(alone) == 4 and alone[3][1][-1] == os.path.join(out, "c2.mp4")
               and not any("hstack=inputs=2" in step for kind, step in alone if kind == "run")
               and alone[1][1][5] == os.path.join(out, "c2_overlay"), why)
        for name, leg, o in (("alone_empty_leg_REFUSED", _leg(root, "a5", 0), out),
                             ("alone_gap_in_frames_REFUSED", g, out),
                             ("alone_out_inside_session_REFUSED", a, os.path.join(a, "clip"))):
            s, why = plan(leg, None, o, "x", verify, ffmpeg)
            report(name, s is None, why)
        if shutil.which(ffmpeg):
            code = execute(alone)
            mp4 = os.path.join(out, "c2.mp4")
            report("ffmpeg_encodes_anomaly_alone", code == 0 and os.path.isfile(mp4) and os.path.getsize(mp4) > 0
                   and sorted(os.listdir(a)) == ["Actual_Frames", "annotation.json", "labels.jsonl"], "exit %d" % code)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    print("M57-CLIP SELFTEST: %s" % (("OK - %d cases" % n_cases) if ok else "BROKEN"))
    return 0 if ok else 2


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--anomaly")
    ap.add_argument("--null")
    ap.add_argument("--out")
    ap.add_argument("--name", default="clip")
    ap.add_argument("--verify", default=None)
    ap.add_argument("--ffmpeg", default="ffmpeg")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest(a.verify, a.ffmpeg)
    if not (a.anomaly and a.out and a.verify):
        ap.error("--verify, --anomaly and --out are required (--null is optional)")
    steps, why = plan(a.anomaly, a.null, a.out, a.name, a.verify, a.ffmpeg)
    if steps is None:
        print("M57-CLIP REFUSED: " + why)
        return 3
    if not a.dry_run:
        os.makedirs(a.out, exist_ok=True)
    if execute(steps, a.dry_run):
        return 1
    print("M57-CLIP %s (%s)" % ("PLANNED" if a.dry_run else "WROTE " + os.path.join(a.out, a.name + ".mp4"), why))
    return 0


if __name__ == "__main__":
    sys.exit(main())
