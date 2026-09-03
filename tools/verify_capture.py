#!/usr/bin/env python3
"""
verify_capture.py - draw the capture's bounding boxes onto COPIES of the frames, for HUMAN INSPECTION.

THIS IS NOT A LABEL PRODUCER. The engine-side labels are authoritative and this tool never writes,
edits or second-guesses them. It reads what was written and draws it, so a person can look at a frame
and see what the dataset claims about it. The use case it exists for: an anomaly nobody can spot by
eye - rocks on the ground with a missing texture - was annotated correctly, and the overlay is how you
confirm that rather than take it on faith.

It writes annotated COPIES into <dir>/annotated/ and never modifies a captured frame.

ONLY FRAMES THAT CARRY AT LEAST ONE BOX ARE WRITTEN. On a typical session most frames have no
anomaly on them, and an annotated copy of such a frame is a byte-for-byte duplicate of the original
apart from the legend - at 3200x2000 that is a lot of disk for nothing. So the output directory is a
SPARSE, NON-CONTIGUOUS sequence, and the gaps are frames with nothing to draw, not missing data.
The summary line always states how many frames had boxes, how many images were written, and out of
how many total frames, so a gap never has to be guessed at.

THE OUTPUT FILENAME KEEPS THE ORIGINAL FRAME INDEX - frame_00045.png becomes
frame_00045_annotated.png. Nothing is ever renumbered: the number IS the 0-based session index, and
cross-referencing an overlay against annotation.json is the entire point of the tool.

A frame counts as carrying a box if it has at least one RED (shipped) or AMBER (candidate) box.
AMBER-ONLY frames ARE written - finding where a dropped or non-shipped label sat is exactly what
this is used for. --red-only narrows it to frames with a shipped label; it is OFF by default.

TWO COLOURS, and the difference is the point:

  RED    the event is in annotation.json for that frame. This is a SHIPPED label - it is in the
         dataset the client receives.

  AMBER  the box is in labels.jsonl but NOT in annotation.json for that frame. It is a CANDIDATE that
         did not become a shipped label. Each amber box is tagged with why, as far as the artifacts
         can tell:
           OUTSIDE-SUBSET  the event IS annotated, but not on this frame. For hide-type anomalies
                           (blink, missing_object) annotation.json carries only the frames where the
                           object was actually HIDDEN, while labels.jsonl covers the whole fire-active
                           window including the lead-in and the un-hidden half of each blink. This is
                           BY DESIGN and is the common case by a wide margin.
           NON-MANIFESTED  the event is annotated with manifested:false - the hide was ordered but
                           never reached the pixels, so it carries no positive frames (m23).
           VETOED          no such event survives in annotation.json and run_summary reports vetoed
                           events - the pixel veto measured the target drawing zero pixels and removed
                           the event before annotation.json was written (m26/m27).
           UNMATCHED       no such event in annotation.json and nothing reports a veto. Unexpected;
                           worth reporting.

BOX LABELS READ "<anomaly> <asset_name> (<actor_name>)". The ASSET NAME is primary because the actor
name is frequently uninformative - a level actor placed in the editor is named StaticMeshActor_66 or
BP_MovingPlatform_C_UAID_B42E9936..., which identifies nothing to a human looking at a frame. The
actor name is kept, dimmed and in parentheses, because it is the join key against annotation.json and
labels.jsonl and must stay readable off the image.

⚠ THE ASSET NAME COMES FROM annotation.json's affected_objects.nodes[] (m22), SO A BOX WITH NO EVENT
IN annotation.json HAS NO ASSET NAME TO SHOW. That is exactly the VETOED and UNMATCHED categories:
labels.jsonl carries only target_name (the actor name), so those boxes fall back to the actor name
alone. The summary says so explicitly when it happens, rather than leaving the reader to wonder why
some boxes are named differently from others.

--------------------------------------------------------------------------------------------------
m47 BLACK-FRAME GATE  (--black-frame-gate, a SEPARATE mode; the overlay path is untouched)
--------------------------------------------------------------------------------------------------
Two readings, reported separately because they fail for different reasons:

  (1) WHOLE-FRAME BLACK - any captured frame whose mean luminance is below the threshold FAILS the
      run and is listed by index. This is the session-ruining artifact: a burst of black frames
      carrying positive labels.
  (2) DARK FIRST FRAME - per event, the target-region luminance on its FIRST labelled frame against
      that event's own mean. Scored against the event, never an absolute, because the absolute
      depends on the scene. REPORTED; a gate only where it must be zero (a packaged cook).

WHERE THE THRESHOLD COMES FROM - it is DERIVED from this bench's own DARKEST LEGITIMATE FRAME, not
chosen. Measured over the m47 legs on CB_GateLevel at 1280x720, marker off, auto-exposure off, whole
frame mean luminance on the 0..255 scale:

    editor, first run after a build      min  60.111   (E1 attempt 1 - a cold shader cache)
    editor, cold material DDC            min  59.992   (E2b, the strongest form of the condition)
    editor, warm cache                   min 100.787   (E1 attempt 2)
    packaged, prewarm off / on           min  99.467 / 99.611   (P1 / P2)

The two dark legs are the ones whose shader cache was cold: the level's lighting has not converged
while the compiler is busy, and the session brightens from about 60 to about 105 across its 90
frames. A warm editor leg and both packaged legs never go below 99. So the darkest frame this fixture
legitimately produces sits at about 60, or 23.5% of full scale, while the state this gate exists to
catch - "the whole picture is black for a burst of frames and then recovers" - reads essentially 0.
The two are not close, and the threshold is placed an ORDER OF MAGNITUDE below the legitimate floor:

    BLACK_FRAME_LUMA_DEFAULT = 6.0

A legitimate frame would have to lose 90% of its brightness before this fires. That margin is the
point: a gate that sits just under the observed floor fails on the first darker level somebody
captures, and a gate that has to be re-tuned per level is not a gate. ON A DARKER TITLE THE NUMBER IS
WRONG AND MUST BE RE-DERIVED ON THAT HOST'S OWN FRAMES - --black-threshold exists for exactly that,
and the derivation rule ("an order of magnitude below the darkest legitimate frame") travels even
though the number does not.

--selftest PROVES THE GATE CAN FAIL, both directions, against a synthetic mid-grey frame and a
synthetic all-black one. A gate that has never fired is not a gate (G96).

Usage:
    python verify_capture.py --dir <sessionDir> [--out <annotatedDir>] [--quiet] [--red-only]
    python verify_capture.py --dir <sessionDir> --black-frame-gate [--black-threshold N]
    python verify_capture.py --selftest

Requires Pillow:  pip install pillow
"""

import argparse
import json
import os
import sys

DEFAULT_DIR = r"D:/IntrusiveAnomalies/StackOBot/Saved/AnomalyCaptures/manual"

RED = (255, 40, 40)
AMBER = (255, 176, 0)
GREY = (140, 140, 140)

CAT_SHIPPED = "SHIPPED"
CAT_OUTSIDE = "OUTSIDE-SUBSET"
CAT_NONMANIF = "NON-MANIFESTED"
CAT_VETOED = "VETOED"
CAT_UNMATCHED = "UNMATCHED"

BLACK_FRAME_LUMA_DEFAULT = 6.0

DARK_FIRST_FRAME_RATIO_DEFAULT = 0.5


def client_type(engine_id):
    return "blink" if engine_id == "blinking" else engine_id


def load_events(cap_dir):
    path = os.path.join(cap_dir, "annotation.json")
    if not os.path.isfile(path):
        return None, {}
    with open(path, "r", encoding="utf-8") as f:
        ann = json.load(f)
    events = []
    assets = {}
    for ev in ann.get("anomalies", []) or []:
        af = ev.get("affected_frames", {}) or {}
        nodes = (ev.get("affected_objects", {}) or {}).get("nodes", []) or []
        for n in nodes:
            actor = n.get("name", "") or ""
            asset = n.get("asset_name", "") or ""
            if actor and asset and actor not in assets:
                assets[actor] = asset
        events.append({
            "type": ev.get("anomaly_type", ""),
            "names": set(n.get("name", "") for n in nodes),
            "start": af.get("start_frame"),
            "end": af.get("end_frame"),
            "idx": set(af.get("frame_indices", []) or []),
            "manifested": bool(ev.get("manifested", True)),
        })
    return events, assets


def label_for(engine_id, actor, assets):
    """Primary label + dimmed actor suffix. Empty asset_name falls back to the actor name alone."""
    asset = assets.get(actor, "")
    if asset:
        return f"{engine_id} {asset}", f"({actor})"
    return f"{engine_id} {actor}", ""


def target_for(actor, assets):
    asset = assets.get(actor, "")
    return f"{asset} ({actor})" if asset else actor


def draw_tag(draw, font, x, y, primary, dimmed, suffix, colour):
    """Primary in the box colour, actor name dimmed, category suffix back in the box colour."""
    try:
        cx = x
        draw.text((cx, y), primary, fill=colour, font=font)
        cx += draw.textlength(primary, font=font)
        if dimmed:
            cx += 5
            draw.text((cx, y), dimmed, fill=GREY, font=font)
            cx += draw.textlength(dimmed, font=font)
        if suffix:
            cx += 5
            draw.text((cx, y), suffix, fill=colour, font=font)
    except Exception:
        flat = "  ".join(p for p in (primary, dimmed, suffix) if p)
        draw.text((x, y), flat, fill=colour, font=font)


def load_run_summary(cap_dir):
    path = os.path.join(cap_dir, "run_summary.json")
    if not os.path.isfile(path):
        return {}
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def classify(frame_key, engine_id, target, events, any_vetoed):
    """Return (category, colour). Engine labels are authoritative; this only reads them."""
    if events is None:
        return CAT_SHIPPED, RED

    ctype = client_type(engine_id)
    cands = [e for e in events if e["type"] == ctype and target in e["names"]]

    if any(frame_key in e["idx"] for e in cands):
        return CAT_SHIPPED, RED
    if not cands:
        return (CAT_VETOED if any_vetoed else CAT_UNMATCHED), AMBER
    if any(not e["manifested"] for e in cands):
        return CAT_NONMANIF, AMBER
    return CAT_OUTSIDE, AMBER


def draw_legend(draw, font, width, has_amber):
    pad = 6
    sw = 14
    lines = [(RED, "RED  in annotation.json - a shipped label")]
    if has_amber:
        lines.append((AMBER, "AMBER  candidate only - not in annotation.json"))
    box_w = 360
    box_h = pad * 2 + len(lines) * 20
    draw.rectangle([0, 0, box_w, box_h], fill=(0, 0, 0))
    y = pad
    for colour, text in lines:
        draw.rectangle([pad, y + 3, pad + sw, y + 3 + sw], fill=colour)
        draw.text((pad + sw + 8, y), text, fill=(255, 255, 255), font=font)
        y += 20


def _mean_luma(path):
    from PIL import Image
    hist = Image.open(path).convert("L").histogram()
    n = sum(hist)
    return (sum(i * c for i, c in enumerate(hist)) / float(n)) if n else 0.0


def _region_mean_luma(frame_path, mask_path, wanted_values):
    from PIL import Image
    fr = Image.open(frame_path).convert("L")
    mk = Image.open(mask_path).convert("L")
    if mk.size != fr.size:
        mk = mk.resize(fr.size, Image.NEAREST)
    fp, mp = fr.load(), mk.load()
    w, h = fr.size
    want = set(int(v) for v in wanted_values if int(v) > 0)
    tot = n = 0
    for y in range(h):
        for x in range(w):
            m = mp[x, y]
            if m and (not want or m in want):
                tot += fp[x, y]
                n += 1
    return (tot / float(n)) if n else None


def black_frame_gate(cap_dir, threshold, dark_ratio, quiet=False):
    """m47 BLACK-FRAME GATE. Returns (ok, lines).

    Two independent readings, reported separately because they fail for different reasons:

      (1) WHOLE-FRAME BLACK. Any captured frame whose mean luminance is below `threshold`
          FAILS the run and is listed by index. This is the session-ruining artifact: a
          burst of black frames carrying positive labels.

      (2) DARK FIRST FRAME. For each event, the target-region luminance on its FIRST
          labelled frame is compared against that event's own mean. A first frame below
          `dark_ratio` of the event mean is counted and listed. This is the shape a
          material that has not finished compiling produces: the target draws the engine
          fallback for a frame or two and then snaps to the right appearance.

    Reading (2) is REPORTED, and is a gate only where the count must be zero - a packaged
    cook. In an editor build it can be legitimately non-zero, and calling that a failure
    would make the gate meaningless on the very builds it is diagnosing.
    """
    lines = []
    labels = os.path.join(cap_dir, "labels.jsonl")
    if not os.path.isfile(labels):
        return False, [f"BLACK-FRAME GATE: CANNOT RUN - no labels.jsonl in {cap_dir}. "
                       f"That is an UNREAD SURFACE, not a pass."]

    rows = []
    with open(labels, "r", encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if line:
                rows.append(json.loads(line))
    rows.sort(key=lambda r: r.get("session_index", 0))
    if not rows:
        return False, ["BLACK-FRAME GATE: CANNOT RUN - labels.jsonl is empty. Not a pass."]

    black = []
    lumas = []
    per_index = {}
    for r in rows:
        img = os.path.join(cap_dir, r.get("image", "").replace("/", os.sep))
        if not os.path.isfile(img):
            continue
        lum = _mean_luma(img)
        lumas.append(lum)
        per_index[r["session_index"]] = (r, img, lum)
        if lum < threshold:
            black.append((r["session_index"], lum, r.get("image", "")))

    if not lumas:
        return False, ["BLACK-FRAME GATE: CANNOT RUN - no frame images found on disk. Not a pass."]

    events, cur = [], []
    for si in sorted(per_index):
        r = per_index[si][0]
        if r.get("anomalies"):
            cur.append(si)
        elif cur:
            events.append(cur); cur = []
    if cur:
        events.append(cur)

    dark_first = []
    for ev in events:
        vals = {}
        for si in ev:
            r, img, _ = per_index[si]
            mf = r.get("mask_file")
            if not mf:
                continue
            mp = os.path.join(cap_dir, mf.replace("/", os.sep))
            if os.path.isfile(mp):
                v = _region_mean_luma(img, mp, [a.get("mask_value", 0) for a in r.get("anomalies", [])])
                if v is not None:
                    vals[si] = v
        if len(vals) < 2 or ev[0] not in vals:
            continue
        mean = sum(vals.values()) / float(len(vals))
        first = vals[ev[0]]
        if mean > 0 and (first / mean) < dark_ratio:
            dark_first.append((ev[0], first, mean, first / mean))

    lines.append("m47 BLACK-FRAME GATE")
    lines.append(f"  frames read              {len(lumas)}")
    lines.append(f"  whole-frame luminance    min {min(lumas):.3f}  max {max(lumas):.3f}  "
                 f"mean {sum(lumas) / len(lumas):.3f}   (0..255)")
    lines.append(f"  threshold                {threshold:.3f}")
    lines.append(f"  BLACK FRAMES             {len(black)}")
    if black and not quiet:
        for si, lum, img in black:
            lines.append(f"      si={si:<5d} luma={lum:.3f}   {img}")
    lines.append(f"  events scored            {len(events)}")
    lines.append(f"  DARK FIRST FRAMES        {len(dark_first)}   "
                 f"(first-frame target luminance below {dark_ratio:.2f} x that event's own mean)")
    for si, first, mean, ratio in dark_first:
        lines.append(f"      si={si:<5d} first={first:.3f} event_mean={mean:.3f} ratio={ratio:.3f}")

    ok = len(black) == 0
    lines.append(f"  VERDICT                  {'PASS' if ok else 'FAIL'}"
                 f"{'' if ok else '  - the run carries whole-frame-black captured frames'}")
    return ok, lines


def _selftest():
    """Prove the gate can FAIL. A gate that has never fired is not a gate (G96).

    Builds two synthetic one-frame sessions - one mid-grey, one all black - and asserts
    the gate PASSES the first and FAILS the second. Both directions, every run.
    """
    import shutil
    import tempfile
    try:
        from PIL import Image
    except ImportError:
        print("SELFTEST: ERROR - Pillow is required.", flush=True)
        return 2

    root = tempfile.mkdtemp(prefix="m47_selftest_")
    rc = 0
    try:
        results = {}
        for name, value in (("grey", 128), ("black", 0)):
            d = os.path.join(root, name)
            os.makedirs(os.path.join(d, "Actual_Frames"))
            Image.new("RGB", (64, 48), (value, value, value)).save(
                os.path.join(d, "Actual_Frames", "frame_00000.png"))
            with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
                fh.write(json.dumps({"session_index": 0, "image": "Actual_Frames/frame_00000.png",
                                     "anomalies": []}) + "\n")
            ok, lines = black_frame_gate(d, BLACK_FRAME_LUMA_DEFAULT,
                                         DARK_FIRST_FRAME_RATIO_DEFAULT, quiet=True)
            results[name] = ok
            print(f"SELFTEST {name:<6} -> {'PASS' if ok else 'FAIL'}", flush=True)

        if results.get("grey") is not True:
            print("SELFTEST: BROKEN - the gate failed a legitimate mid-grey frame.", flush=True)
            rc = 3
        if results.get("black") is not False:
            print("SELFTEST: BROKEN - the gate PASSED an all-black frame. It cannot fire, so any "
                  "green verdict it gives is blindness rather than a reading.", flush=True)
            rc = 4
        if rc == 0:
            print("SELFTEST: OK - the gate passes a legitimate frame and FAILS an all-black one, "
                  "so its zero is a reading.", flush=True)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    return rc


def main():
    ap = argparse.ArgumentParser(
        description="Draw capture bboxes onto copies of the frames for human inspection (never edits labels).")
    ap.add_argument("--dir", default=DEFAULT_DIR, help="session dir containing labels.jsonl + frames")
    ap.add_argument("--out", default=None, help="output dir for annotated copies (default: <dir>/annotated)")
    ap.add_argument("--quiet", action="store_true", help="suppress the per-frame table (keep progress + summary)")
    ap.add_argument("--red-only", action="store_true",
                    help="write only frames carrying a RED (shipped) box; default is RED or AMBER")
    ap.add_argument("--black-frame-gate", action="store_true",
                    help="m47: run the black-frame gate INSTEAD of the overlay and exit nonzero if "
                         "any captured frame is whole-frame black")
    ap.add_argument("--black-threshold", type=float, default=BLACK_FRAME_LUMA_DEFAULT,
                    help=f"whole-frame mean luminance (0..255) below which a frame FAILS the gate "
                         f"(default {BLACK_FRAME_LUMA_DEFAULT}, derived from this bench's darkest "
                         f"legitimate frame; re-derive it on a darker title)")
    ap.add_argument("--dark-first-frame-ratio", type=float, default=DARK_FIRST_FRAME_RATIO_DEFAULT,
                    help=f"an event's first labelled frame counts as DARK when its target-region "
                         f"luminance is below this fraction of that event's own mean "
                         f"(default {DARK_FIRST_FRAME_RATIO_DEFAULT})")
    ap.add_argument("--selftest", action="store_true",
                    help="m47: prove the black-frame gate can FAIL, against a synthetic black frame")
    args = ap.parse_args()

    if args.selftest:
        sys.exit(_selftest())

    cap_dir = os.path.abspath(args.dir)

    if args.black_frame_gate:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the black-frame gate.")
        ok, lines = black_frame_gate(cap_dir, args.black_threshold,
                                     args.dark_first_frame_ratio, args.quiet)
        for line in lines:
            print(line, flush=True)
        sys.exit(0 if ok else 1)

    out_dir = os.path.abspath(args.out) if args.out else os.path.join(cap_dir, "annotated")
    sidecar = os.path.join(cap_dir, "labels.jsonl")

    if not os.path.isfile(sidecar):
        sys.exit(f"ERROR: no labels.jsonl in {cap_dir}\n"
                 f"       In delivery mode this file is written only when "
                 f"IAI.Capture.DeliveryLabels is ON (it is ON by default).")

    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError:
        sys.exit("ERROR: Pillow is required for the overlay tool.\n"
                 "       Install it with:  python -m pip install --upgrade Pillow")

    events, assets = load_events(cap_dir)
    rs = load_run_summary(cap_dir)
    any_vetoed = int(rs.get("vetoed_events", 0) or 0) > 0

    os.makedirs(out_dir, exist_ok=True)
    try:
        font = ImageFont.truetype("arial.ttf", 16)
    except Exception:
        font = ImageFont.load_default()

    with open(sidecar, "r", encoding="utf-8") as f:
        rows = [json.loads(line) for line in f if line.strip()]

    total = len(rows)
    counts = {CAT_SHIPPED: 0, CAT_OUTSIDE: 0, CAT_NONMANIF: 0, CAT_VETOED: 0, CAT_UNMATCHED: 0}
    targets_by_cat = {k: set() for k in counts}
    n_boxes = 0
    n_missing_img = 0
    n_frames_with_boxes = 0
    n_written = 0
    n_label_no_drawable_box = 0
    n_actor_only = 0

    if events is None:
        print("NOTE: no annotation.json beside labels.jsonl - every box is drawn RED and nothing is "
              "classified. Run this on a finished session dir.", flush=True)

    print(f"session   : {cap_dir}", flush=True)
    print(f"annotated : {out_dir}", flush=True)
    if not args.quiet:
        print(f"{'frame':>7}  {'present':>7}  anomalies", flush=True)
        print("-" * 78, flush=True)

    for i, rec in enumerate(rows, 1):
        frame_key = rec.get("session_index", rec.get("frame_index"))
        img_name = rec.get("image", "")
        img_path = os.path.join(cap_dir, img_name)
        anoms = rec.get("anomalies", []) or []

        classified = []
        for a in anoms:
            cat, colour = classify(frame_key, a.get("id", ""), a.get("target_name", ""),
                                   events, any_vetoed)
            counts[cat] += 1
            targets_by_cat[cat].add(target_for(a.get("target_name", ""), assets))
            classified.append((a, cat, colour))

        if not args.quiet:
            summary = ", ".join(f"{a.get('id')}->{target_for(a.get('target_name', ''), assets)}[{cat}]"
                                for a, cat, _ in classified) or "(none)"
            print(f"{frame_key:>7}  {str(rec.get('anomaly_present', False)):>7}  {summary}", flush=True)

        drawable = []
        for a, cat, colour in classified:
            x, y, w, h = a.get("bbox_px", [0, 0, 0, 0])
            if not a.get("bbox_valid", False):
                colour = GREY
            if w > 0 and h > 0:
                drawable.append((a, cat, colour, x, y, w, h))

        if classified and not drawable:
            n_label_no_drawable_box += 1

        want = bool(drawable) and (not args.red_only
                                   or any(cat == CAT_SHIPPED for _, cat, _, _, _, _, _ in drawable))
        if not want:
            print(f"[progress] {i}/{total}", flush=True)
            continue

        n_frames_with_boxes += 1

        if not os.path.isfile(img_path):
            n_missing_img += 1
            print(f"[progress] {i}/{total} (image missing: {img_name})", flush=True)
            continue

        im = Image.open(img_path).convert("RGB")
        draw = ImageDraw.Draw(im)
        has_amber = False
        for a, cat, colour, x, y, w, h in drawable:
            draw.rectangle([x, y, x + w, y + h], outline=colour, width=3)
            n_boxes += 1
            if colour == AMBER:
                has_amber = True
            primary, dimmed = label_for(a.get("id", ""), a.get("target_name", ""), assets)
            if not dimmed:
                n_actor_only += 1
            suffix = f"[{cat}]" if cat != CAT_SHIPPED else ""
            draw_tag(draw, font, x + 2, max(0, y - 20), primary, dimmed, suffix, colour)

        draw_legend(draw, font, im.width, has_amber)
        out_path = os.path.join(out_dir, os.path.splitext(os.path.basename(img_name))[0] + "_annotated.png")
        im.save(out_path)
        n_written += 1
        print(f"[progress] {i}/{total}", flush=True)

    print("-" * 78, flush=True)
    print(f"{n_frames_with_boxes} frame(s) had boxes, {n_written} image(s) written, "
          f"out of {total} total frame(s).", flush=True)
    print(f"  {total - n_frames_with_boxes} frame(s) had nothing to draw and were SKIPPED - the "
          f"output is a sparse, non-contiguous sequence and the gaps are frames with no anomaly on "
          f"them, not missing data. Filenames keep the original 0-based frame index.", flush=True)
    if args.red_only:
        print("  --red-only was set: AMBER-only frames were skipped as well.", flush=True)
    print(f"{n_boxes} box(es) drawn into {out_dir}", flush=True)
    print(f"  RED   {CAT_SHIPPED:<16} {counts[CAT_SHIPPED]:>5}   {sorted(targets_by_cat[CAT_SHIPPED])}", flush=True)
    for cat in (CAT_OUTSIDE, CAT_NONMANIF, CAT_VETOED, CAT_UNMATCHED):
        if counts[cat]:
            print(f"  AMBER {cat:<16} {counts[cat]:>5}   {sorted(targets_by_cat[cat])}", flush=True)
    if n_actor_only:
        print(f"  {n_actor_only} box(es) are labelled with the ACTOR NAME ONLY - no asset name was "
              f"available for them. asset_name comes from annotation.json's nodes, so a box whose "
              f"event is not in annotation.json (VETOED, UNMATCHED) has none: labels.jsonl carries "
              f"only target_name. This is a limit of the artifacts, not a missing asset.", flush=True)
    if n_missing_img:
        print(f"  {n_missing_img} frame(s) with boxes had no image on disk", flush=True)
    if n_label_no_drawable_box:
        print(f"  {n_label_no_drawable_box} frame(s) carried a label whose bbox had zero width or "
              f"height, so there was no box to draw and no image was written", flush=True)
    if counts[CAT_UNMATCHED]:
        print("  NOTE: UNMATCHED means a candidate box has no matching event in annotation.json and "
              "run_summary reports no vetoes. That combination is not expected - worth reporting.", flush=True)


if __name__ == "__main__":
    main()
