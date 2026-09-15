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

Ã¢Å¡Â  THE ASSET NAME COMES FROM annotation.json's affected_objects.nodes[] (m22), SO A BOX WITH NO EVENT
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

--------------------------------------------------------------------------------------------------
m49 LABEL-PIXEL GATE  (--label-pixel-gate) - and its CAMERA-MOTION ENVELOPE (079-03)
--------------------------------------------------------------------------------------------------
Per event it checks that the first labelled frame is the first frame whose pixels change, and that
the frame after end_frame is the first clean one. The statistic is

    d(k) = the fraction of the event's REGION whose pixels differ from frame k-1 by > threshold

which is BOUNDED IN [0,1], and the decision threshold is tau = max(median + K_SIGMA*MAD, FLOOR)
learned from that event's own nearest clean frames.

🚨 WHY THIS TOOL REFUSES TO JUDGE A SHAKY SESSION. tau is UNBOUNDED while d is not. Under camera
motion the CLEAN frames change too, so the learned tau climbs - and it can climb past 1.0, at which
point `d > tau` is unsatisfiable and EVERY event reads NOT-VISIBLE no matter what the pixels show.
That is not a hypothetical: on M2 field captures it marked almost every event NOT-VISIBLE on labels
the owner then verified BY EYE as correct, and on a banked heavy-motion session tau measured 1.0950
and 1.1938 (G259). So motion is MEASURED and the tool REFUSES above a cap, instead of reporting a
verdict it cannot support.

    M_med       = median over clean frames of the WHOLE-FRAME changed-pixel fraction
    MOTION_CAP  = 0.040

    cap=0.040 (2x margin below first incomplete recovery, LYRA_SMOKE_01 M_med=0.0801;
    only full recovery observed at 0.0049, A2L_LEGA; ruling 079-04)

WHERE THE CAP COMES FROM - it is MEASURED, not chosen. 079-03 took banked moving-camera sessions,
MOVED EVERY ANNOTATED WINDOW BY A KNOWN +/-1 (labels only; frames untouched), and asked whether the
gate reports that exact shift back with the opposite sign. The cap is the motion level at which that
ability disappears. Measured, on REAL sessions - and the cap is set by these alone:

    session          M_med    injected +/-1 recovered on decided events
    A2L_LEGA        0.0049    FULL          (6/6, 6/6, 6/6)
    LYRA_SMOKE_01   0.0801    INCOMPLETE    (one edge misplaced, one read as -2)
    A1L_LEGA        0.0852    INCOMPLETE    (one shift recovered with the WRONG SIGN)
    M50L_LG9        0.3491    NONE          (3 of 3 unshifted labels read NOT-VISIBLE)

A SYNTHETIC scroll ladder still recovers at M_med 0.75 and is DELIBERATELY NOT the binding number: a
uniform scroll has NO PARALLAX and therefore flatters the gate. It stays in --selftest as the proof
that a shift is still readable under motion at all, and it sets nothing.

⚠ WHY 0.040 AND NOT 0.0049. The measurement gives two bounds: the largest M_med that fully recovered
(0.0049) and a 2x margin below the first that did not (0.0801/2 = 0.040). A cap resting on the single
value it was derived from is fragile - a 0.0001 measurement drift would refuse the very session that
set it - so the cap is placed at the margin bound. It still refuses both incomplete sessions and
M50L_LG9.

🚨 THERE IS NO MIDDLE TIER, AND THE REASON IS THE COMPLAINT THIS TOOL EXISTS FOR. Zero FALSE shifts
were measured up to M_med 0.085 - the gate does not INVENT disagreements - and it is tempting to
therefore trust a PASS higher up than a SHIFT. Refused: A PASS FROM A GATE THAT CANNOT READ BACK A
ONE-FRAME SHIFT IS NOT EVIDENCE OF ALIGNMENT, IT IS THE CLIENT'S ORIGINAL COMPLAINT RESTATED. If the
instrument provably cannot see a one-frame error at that motion level, "no error found" carries no
information about whether one is there. Above the cap the answer is NOT-MEASURABLE, full stop.

⚠ HONEST LIMITS, because they decide whether a reading is worth anything:
  - A cap here still refuses most real gameplay footage. The tool is then HONEST (it never calls a
    correct label NOT-VISIBLE) but it is not USEFUL on that footage - it reports NOT-MEASURABLE with
    the numbers and stops. That trade is deliberate: a confident wrong answer about a client's
    dataset is worse than no answer.
  - Only ONE real session has been observed to recover fully, so the cap rests on a thin base and
    should be re-derived whenever more masked moving-camera sessions exist.
  - Nothing here says a refused session's labels are wrong. NOT-MEASURABLE is an UNREAD SURFACE.

🔻 CORRECTION TO A DOCUMENTED CLAIM ELSEWHERE, so nobody re-derives the dead end: 079-02 tried to
NORMALISE the motion away instead of refusing, thresholding net = d(region) - d(ambient ring), on
measure_label_offset.py:92-93's reasoning that "a whole-frame change lifts the ring as much as the
region and cancels". THAT IS TRUE FOR A GENUINELY GLOBAL CHANGE (exposure, a fade) AND FALSE FOR
CAMERA MOTION, which is parallax- and content-weighted. It also DESTROYED real signal on a still
camera, because these anomalies bleed outside their own bbox (m26's A35: hiding SM_Ramp2 changed
MORE outside its bbox than inside). Measured and reverted; see _region_frac.

Usage:
    python verify_capture.py --dir <sessionDir> [--out <annotatedDir>] [--quiet] [--red-only]
    python verify_capture.py --dir <sessionDir> --black-frame-gate [--black-threshold N]
    python verify_capture.py --dir <sessionDir> --label-pixel-gate [--report-only] [--motion-cap N]
    python verify_capture.py --all <bankRoot> --label-pixel-gate --report-only [--out <dir>]
    python verify_capture.py --label-pixel-gate --selftest
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


DIFF_THRESH_DEFAULT = 8
EDGE_WINDOW_DEFAULT = 4
MIN_VISIBLE_PX_DEFAULT = 1
BASELINE_GUARD_FRAMES = 2

MIN_BASELINE_FRAMES = 3
BASELINE_MAX_FRAMES = 24
MAX_REGION_FRAC = 0.90

ATTAINABLE_MAX = 1.0

MOTION_CAP = 0.040

V_PASS = "PASS"
V_NOTVIS = "NOT-VISIBLE"
V_NOTMEAS = "NOT-MEASURABLE"


def _offset_module():
    """Import measure_label_offset.py from beside this file.

    The region, baseline and threshold vocabulary is REUSED, never re-implemented: a second
    copy of that code is a second thing to drift. This gate adds one metric the module does
    not carry - the FRACTION of region pixels differing by more than a threshold - and takes
    everything else (frame cache, bbox extraction, label matching, K_SIGMA, SIGNAL_FLOOR,
    the checker/magenta classifier) from the module.
    """
    here = os.path.dirname(os.path.abspath(__file__))
    if here not in sys.path:
        sys.path.insert(0, here)
    try:
        import measure_label_offset as mlo
    except SystemExit:
        raise RuntimeError("measure_label_offset.py exited on import - it requires Pillow. "
                           "Install it with:  python -m pip install --upgrade Pillow")
    except ImportError:
        raise RuntimeError("measure_label_offset.py must sit beside verify_capture.py - this "
                           "gate reuses its region/baseline/threshold code.")
    return mlo


def _frame_paths(cap_dir, mlo):
    frames_dir = os.path.join(cap_dir, "Actual_Frames")
    paths = {}
    if not os.path.isdir(frames_dir):
        return paths
    for name in os.listdir(frames_dir):
        m = mlo.FRAME_RE.match(name)
        if m:
            paths[int(m.group(1))] = os.path.join(frames_dir, name)
    return paths


def _region_from_mask(path, wanted, frame_w, frame_h):
    """Region = the delivered mask's pixels for this event's value.

    `mask_value` is 0 on a row whose per-fire record was not resolved, while the PNG still
    carries the event's tag - observed in banked m45 sessions. So a value of 0 falls back to
    the PNG's sole non-zero value when there is exactly one, and refuses (returns None, which
    drops the caller to the bbox) when the frame carries several. It never guesses between
    two events' silhouettes.
    """
    from PIL import Image, ImageStat
    try:
        im = Image.open(path).convert("L")
    except Exception:
        return None
    if im.size != (frame_w, frame_h):
        return None
    hist = im.histogram()
    present = [i for i in range(1, 256) if hist[i] > 0]
    if not present:
        return None
    try:
        want = int(wanted) if wanted is not None else 0
    except Exception:
        want = 0
    if want > 0 and want in present:
        vals = set([want])
        note = "mask(v%d)" % want
    elif len(present) == 1:
        vals = set(present)
        note = "mask(sole v%d)" % present[0]
    else:
        return None
    binary = im.point(lambda v: 255 if v in vals else 0)
    bb = binary.getbbox()
    if not bb:
        return None
    crop = binary.crop(bb)
    npix = int(round(ImageStat.Stat(crop).sum[0] / 255.0))
    if npix < 1:
        return None
    return {"bin": crop, "box": bb, "npix": npix, "source": note}


def _build_region(cap_dir, row, entry, frame_w, frame_h, mlo):
    if row is not None and entry is not None:
        mf = row.get("mask_file")
        if mf:
            mpath = os.path.join(cap_dir, str(mf).replace("/", os.sep))
            if os.path.isfile(mpath):
                reg = _region_from_mask(mpath, entry.get("mask_value"), frame_w, frame_h)
                if reg:
                    return reg
    if entry is None:
        return None
    box, src = mlo.bbox_from_label_entry(entry, frame_w, frame_h)
    if not box:
        return None
    cb = mlo.clamp_box(box, frame_w, frame_h)
    if mlo.box_area(cb) < mlo.MIN_REGION_PX:
        return None
    return {"bin": None, "box": cb, "npix": mlo.box_area(cb), "source": src or "bbox"}


class _HotCache(object):
    """hot(k) = the WHOLE frame, 255 where |gray(k) - gray(k-1)| > thresh, else 0.

    Whole-frame on purpose: the region fraction and the ambient-ring fraction are two crops of
    the SAME binary image, so they are measured against each other on the same frame pair rather
    than against two independently computed diffs. Cached because every event re-reads the same
    frame pairs and a long field session has many events.
    """

    def __init__(self, cache, thresh, limit=48):
        self.cache = cache
        self.thresh = thresh
        self.limit = max(4, limit)
        self.hot = {}

    def get(self, k):
        hit = self.hot.get(k)
        if hit is not None:
            return hit
        from PIL import ImageChops
        try:
            a = self.cache.gray_of(k)
            b = self.cache.gray_of(k - 1)
        except Exception:
            return None
        if a.size != b.size:
            return None
        t = self.thresh
        img = ImageChops.difference(a, b).point(lambda v: 255 if v > t else 0)
        while len(self.hot) >= self.limit:
            self.hot.pop(next(iter(self.hot)))
        self.hot[k] = img
        return img

    def whole_frame(self, k):
        from PIL import ImageStat
        img = self.get(k)
        if img is None:
            return None
        w, h = img.size
        if w <= 0 or h <= 0:
            return None
        return (ImageStat.Stat(img).sum[0] / 255.0) / float(w * h)


def _region_frac(hot, k, region):
    """d(k): the fraction of REGION pixels differing by more than the threshold from frame k-1.

    THE DECISION STATISTIC. Bounded in [0, 1] by construction: the numerator counts pixels inside
    the region, the denominator is the size of that region.

    Ã°Å¸â€Â» 079-03: an earlier attempt thresholded `net = d(region) - d(ambient ring)` instead, on the
    reasoning quoted at measure_label_offset.py:92-93 - "a whole-frame change lifts the ring as
    much as the region and cancels". THAT REASONING IS CORRECT FOR A GENUINELY GLOBAL CHANGE
    (exposure, a fade) AND MEASURABLY WRONG FOR CAMERA MOTION, which is parallax- and
    content-weighted: a region of near, detailed geometry changes far more under a given camera
    move than a ring of flat distant wall, so the subtraction leaves a large content-dependent
    variance instead of cancelling. Measured 079-02: tau on `net` still read 0.18-0.44 on a
    heavy-motion session.

    It also DESTROYED a real signal on a still camera, because the ring's premise - that the
    anomaly is confined to the region - is false for these anomalies. On M49_GEDGE_MT_NAT the
    onset turned 1528 target pixels hot AND 812 ring pixels hot (bounce light), and because the
    clamped corner ring was less than half the silhouette's area its FRACTION was the larger, so
    `net` went NEGATIVE at the onset. m26's A35 had already measured this: hiding SM_Ramp2 changed
    MORE outside its own bbox than inside (peak-OUT 0.2955 vs peak-IN 0.1785).

    Ã¢â€¡â€™ THE RING IS NOT USED AT ALL. Camera motion is handled by measuring it GLOBALLY and refusing
    to judge above MOTION_CAP, not by trying to subtract it away.
    """
    from PIL import ImageChops, ImageStat
    img = hot.get(k)
    if img is None:
        return None
    npix = region.get("npix") or 0
    if npix <= 0:
        return None
    try:
        crop = img.crop(region["box"])
    except Exception:
        return None
    if region["bin"] is not None:
        if region["bin"].size != crop.size:
            return None
        crop = ImageChops.multiply(crop, region["bin"])
    return (ImageStat.Stat(crop).sum[0] / 255.0) / float(npix)


def _nearest_baseline(base_idx, lo, hi, cap):
    """The `cap` clean frames NEAREST this event's own window (F-D).

    A dense burst schedule leaves clean frames scattered between windows; taking the nearest ones
    keeps the baseline in the same motion regime as the event being judged, instead of averaging
    a whole session's camera behaviour into one threshold.
    """
    def dist(k):
        if k < lo:
            return lo - k
        if k > hi:
            return k - hi
        return 0
    return sorted(sorted(base_idx, key=lambda k: (dist(k), k))[:max(1, cap)])


def _dominant_edge(sigfn, lo, hi, region, paths, tau, foreign=None):
    """The edge is where the BIGGEST change in the neighbourhood is, not the first one above tau.

    Measured on banked m45 legs: the frame AFTER a hide still differs from its predecessor
    because temporal accumulation is still decaying the object out, so "the first frame above
    tau" reads the ghost and reports a one-frame shift that is not there. The frame after a
    reappearance has the same problem in the other direction. The dominant change is the
    transition itself in both cases, and it is what a viewer calls the edge.

    `foreign` is the set of frames belonging to OTHER events. A neighbourhood that reaches
    into another event's window would otherwise let that event's transition win the argmax -
    measured on a banked leg where two events fire on the SAME actor eight frames apart, and
    the second swap was read as the first one's end.

    Returns (frame_index_of_edge, its d) or (None, None) when nothing in the window clears tau.
    """
    best_k = None
    best_d = None
    for k in range(lo, hi + 1):
        if k not in paths or (k - 1) not in paths:
            continue
        if foreign and (k in foreign or (k - 1) in foreign):
            continue
        d = sigfn(k, region)
        if d is None or d <= tau:
            continue
        if best_d is None or d > best_d:
            best_d = d
            best_k = k
    return best_k, best_d


def _anchor_entry(rows, indices, node, mlo):
    """The first frame of the claim whose labels.jsonl row actually carries this event.

    Not simply frame_indices[0]: annotation.json and labels.jsonl are written by different
    paths, and if they disagree about where the event sits, the anchor row can be empty. The
    gate exists to MEASURE that kind of disagreement, so it must not refuse to run because of
    it - it takes the region from the first frame that has one.
    """
    for k in indices:
        r = rows.get(int(k))
        if not r:
            continue
        e = mlo.match_label_entry(r, node, None)
        if e is not None:
            return int(k), r, e
    return None, None, None


def _runs_of(indices):
    runs = []
    for i in sorted(set(int(v) for v in indices)):
        if runs and i == runs[-1][1] + 1:
            runs[-1][1] = i
        else:
            runs.append([i, i])
    return [(a, b) for a, b in runs]


def _load_gate_events(cap_dir, mlo):
    ann = mlo.read_json(os.path.join(cap_dir, "annotation.json"))
    if not isinstance(ann, dict):
        return None
    out = []
    for i, ev in enumerate(ann.get("anomalies") or []):
        if not isinstance(ev, dict):
            continue
        idxs, derived = mlo.event_indices(ev)
        out.append({
            "i": i,
            "type": ev.get("anomaly_type", ""),
            "node": mlo.event_node_name(ev),
            "indices": idxs,
            "derived": derived,
            "manifested": bool(ev.get("manifested", True)),
            "bbox_source": ev.get("bbox_source"),
            "obs_measured": ev.get("observability_measured"),
        })
    return out


def _provenance(ev):
    """`bbox=<source> obs=<measured|unmeasured>` - REPORTED, never a verdict input.

    An event can say `observability_measured: false` (the producer had no pixel evidence) and
    `bbox_source: "projected"` (the box came from bounds, not from what was drawn). It is tempting
    to read that as "unverifiable, skip it" - and it is REFUSED as a rule: the gate's job is to
    check the labels against the pixels whatever the producer claims about them, and the whole
    field case this tool exists for is projected boxes. Schema v1 sessions carry neither key and
    print n/a.
    """
    src = ev.get("bbox_source")
    obs = ev.get("obs_measured")
    return "bbox=%s obs=%s" % (src if src else "n/a",
                               "n/a" if obs is None else ("measured" if obs else "unmeasured"))


def _threshold_from(vals, mlo):
    med = mlo.median_or_none(vals)
    if med is None:
        return None, None, None
    mad = mlo.median_or_none([abs(v - med) for v in vals])
    if mad is None:
        mad = 0.0
    return max(med + mlo.K_SIGMA * mad, mlo.SIGNAL_FLOOR), med, mad


def _measurable_ceiling(windows, all_idx):
    """N = G//2 for the smallest CLEAN GAP BETWEEN annotated windows (G160).

    The session's head and tail are deliberately NOT gaps between windows: an event that
    ends on the session's final frame would otherwise drive the ceiling to zero and turn
    every reading in the session into UNMEASURABLE. They are used only when a single window
    is all there is, which is the module's own rule (measure_label_offset.measurement_ceiling).
    """
    if not windows:
        return None, None
    ordered = sorted(windows)
    if len(ordered) >= 2:
        gaps = [b[0] - a[1] - 1 for a, b in zip(ordered, ordered[1:])]
        gaps = [g for g in gaps if g >= 0]
        if not gaps:
            return None, None
        g = min(gaps)
        return g // 2, g
    if all_idx:
        head = ordered[0][0] - min(all_idx)
        tail = max(all_idx) - ordered[0][1]
        g = max(0, min(head, tail))
        return g // 2, g
    return None, None


def label_pixel_gate(cap_dir, thresh, edge_w, min_visible_px, quiet=False, motion_cap=None):
    """The label-vs-pixel gate. Returns (exit_code, lines).

    Per event, per contiguous run of labelled frames, on the RING-NORMALISED signal
    `net = d(region) - d(ambient ring)`:
      ONSET is aligned when net(start) > tau AND net(start-1) <= tau.
      END   is aligned when net(end+1) > tau AND net(end)     <= tau.
    A shift is reported SIGNED: n < 0 means the PIXELS changed BEFORE the label said so.
    Nothing is inferred about WHY; the tool reports the reading.

    VALIDITY ENVELOPE - what this gate can and cannot judge, stated rather than assumed. Every
    refusal below is NOT-MEASURABLE with its numbers printed, and NEVER NOT-VISIBLE: a region the
    instrument cannot read is an UNREAD SURFACE, not a failed label.

      CAMERA MOTION is MEASURED, not compensated. M_med is the median over clean frames of the
      WHOLE-FRAME changed-pixel fraction. Above MOTION_CAP the gate refuses the session's events.
      It does not try to subtract the motion away - 079-02 measured that a spatial ring does not
      cancel parallax, and that subtracting it destroys small real signals (see _region_frac).

      A REGION COVERING THE PICTURE is refused on its size alone (>= MAX_REGION_FRAC of the
      frame). A label that claims most of the picture cannot be localised against the picture.

      A THRESHOLD NO MEASUREMENT COULD CLEAR is an instrument fault, not a verdict. `d` is a
      fraction, so it can never exceed ATTAINABLE_MAX = 1.0; if tau reaches that, the event reads
      NOT-MEASURABLE(threshold unsatisfiable). ASSERTED, never clamped - clamping turns an
      impossible test into an absurd one that still FAILs, silently. This is the exact state that
      produced six confident FAILs on field data (G259).

      A DENSE BURST SCHEDULE starves the baseline. Each event is calibrated on the clean frames
      NEAREST ITS OWN WINDOW (up to BASELINE_MAX_FRAMES); below MIN_BASELINE_FRAMES the event
      reads NOT-MEASURABLE with the count printed.

    ORDER OF THE REFUSALS, and it matters: region size, then baseline, then tau, then
    UNSATISFIABLE, then MOTION. Unsatisfiable is tested BEFORE motion so that the more specific
    fault is the one reported when both hold, and so the selftest can exercise each separately.

    Constants: K_SIGMA / SIGNAL_FLOOR come from measure_label_offset, where they threshold a
    per-frame region difference - the same shape as `d`. MOTION_CAP is set by the 079-03
    known-answer procedure (see the module header), not by taste.
    """
    lines = []
    try:
        mlo = _offset_module()
    except RuntimeError as exc:
        return 3, ["LABEL-PIXEL GATE: CANNOT RUN - %s" % exc]

    events = _load_gate_events(cap_dir, mlo)
    if events is None:
        return 3, ["LABEL-PIXEL GATE: CANNOT RUN - no readable annotation.json in %s. "
                   "That is an UNREAD SURFACE, not a pass." % cap_dir]

    rows, labels_state = mlo.read_labels(os.path.join(cap_dir, "labels.jsonl"))
    if labels_state == "absent":
        return 3, ["LABEL-PIXEL GATE: CANNOT RUN - no labels.jsonl in %s. The gate needs the "
                   "per-frame bbox. In delivery mode this file is written by default "
                   "(IAI.Capture.DeliveryLabels). Not a pass." % cap_dir]

    paths = _frame_paths(cap_dir, mlo)
    if len(paths) < 3:
        return 3, ["LABEL-PIXEL GATE: CANNOT RUN - fewer than 3 frames found under "
                   "Actual_Frames in %s. Not a pass." % cap_dir]

    cache = mlo.FrameCache(paths, 0, limit=64)
    size = cache.frame_size()
    if not size:
        return 3, ["LABEL-PIXEL GATE: CANNOT RUN - the frames could not be opened. Not a pass."]
    frame_w, frame_h = size
    all_idx = sorted(paths.keys())

    windows = []
    for ev in events:
        if ev["indices"]:
            windows.append((min(ev["indices"]), max(ev["indices"])))
    ceiling, min_gap = _measurable_ceiling(windows, all_idx)

    base_idx = []
    guard_used = BASELINE_GUARD_FRAMES
    for guard in range(BASELINE_GUARD_FRAMES, -1, -1):
        blocked = set()
        for s, e in windows:
            for k in range(s - guard, e + guard + 1):
                blocked.add(k)
        base_idx = [k for k in all_idx
                    if k not in blocked and (k - 1) not in blocked and (k - 1) in paths]
        guard_used = guard
        if len(base_idx) >= MIN_BASELINE_FRAMES:
            break

    has_masks = any(r.get("mask_file") for r in rows.values())
    hot = _HotCache(cache, thresh)

    motion_vals = []
    for k in base_idx[:BASELINE_MAX_FRAMES]:
        m = hot.whole_frame(k)
        if m is not None:
            motion_vals.append(m)
    motion = mlo.median_or_none(motion_vals)

    cap = MOTION_CAP if motion_cap is None else float(motion_cap)
    over_cap = motion is not None and motion > cap

    lines.append("LABEL-PIXEL GATE   (m49 step 1; motion envelope since 079-03)")
    lines.append("  session                  %s" % cap_dir)
    lines.append("  frames / labels / events %d / %d / %d" % (len(paths), len(rows), len(events)))
    lines.append("  region mode              %s" % ("masks" if has_masks else "bbox-only"))
    lines.append("  signal                   d = fraction of REGION pixels changed since the "
                 "previous frame (bounded 0..1)")
    lines.append("  camera motion M_med      %s   cap %.4f%s"
                 % ("%.4f" % motion if motion is not None else "n/a", cap,
                    "   *** OVER CAP - events are refused as NOT-MEASURABLE ***"
                    if over_cap else ""))
    lines.append("  diff threshold           >%d/255 per pixel" % thresh)
    lines.append("  edge search window       +/-%d frames" % edge_w)
    lines.append("  constants                K_SIGMA=%.1f  SIGNAL_FLOOR=%.4f  baseline %d..%d "
                 "frames  region<%.0f%% of frame  attainable<=%.1f"
                 % (mlo.K_SIGMA, mlo.SIGNAL_FLOOR, MIN_BASELINE_FRAMES, BASELINE_MAX_FRAMES,
                    MAX_REGION_FRAC * 100.0, ATTAINABLE_MAX))
    if ceiling is None:
        lines.append("  MEASURABLE RANGE         n/a (no annotated window)")
    else:
        lines.append("  MEASURABLE RANGE         +/-%d frames (min clean gap %d) - a shift beyond "
                     "this is UNDER-READ, not absent" % (ceiling, min_gap))

    ev_lines = []
    n_pass = n_shift = n_notvis = n_notmeas = 0

    for ev in events:
        tag = "idx=%-3d %-18s %-22s" % (ev["i"], ev["type"], ev["node"] or "(no node)")

        if not ev["manifested"] or not ev["indices"]:
            ev_lines.append("%s %s(manifested-false-or-empty)  %s"
                            % (tag, V_NOTMEAS, _provenance(ev)))
            n_notmeas += 1
            continue
        if ev["derived"]:
            ev_lines.append("%s %s(no-frame_indices-in-annotation)  %s"
                            % (tag, V_NOTMEAS, _provenance(ev)))
            n_notmeas += 1
            continue

        anchor, row, entry = _anchor_entry(rows, ev["indices"], ev["node"], mlo)
        if entry is None:
            ev_lines.append("%s %s(no labels.jsonl row carries this event on frames %d..%d)  %s"
                            % (tag, V_NOTMEAS, ev["indices"][0], ev["indices"][-1],
                               _provenance(ev)))
            n_notmeas += 1
            continue

        mask_short = None
        if has_masks:
            for k in ev["indices"]:
                r = rows.get(k)
                if not r or not r.get("mask_file"):
                    continue
                e2 = mlo.match_label_entry(r, ev["node"], None)
                reg = _build_region(cap_dir, r, e2, frame_w, frame_h, mlo) if e2 else None
                if reg and reg["bin"] is not None and reg["npix"] < min_visible_px:
                    mask_short = (k, reg["npix"])
                    break
        if mask_short:
            ev_lines.append("%s %s (mask count %d < %d on frame %d)  %s"
                            % (tag, V_NOTVIS, mask_short[1], min_visible_px, mask_short[0],
                               _provenance(ev)))
            n_notvis += 1
            continue

        region = _build_region(cap_dir, row, entry, frame_w, frame_h, mlo)
        if region is None:
            ev_lines.append("%s %s(no-region: no usable mask or bbox at frame %d)  %s"
                            % (tag, V_NOTMEAS, anchor, _provenance(ev)))
            n_notmeas += 1
            continue

        bbox_only = region["bin"] is None
        only_tag = " (bbox-only)" if bbox_only else ""
        frame_px = float(frame_w * frame_h)

        region_px = mlo.box_area(region["box"])
        if frame_px > 0 and region_px >= MAX_REGION_FRAC * frame_px:
            ev_lines.append("%s %s(region covers the picture: %dpx = %.1f%% of the %dpx frame, so "
                            "it cannot be localised against it)  %s"
                            % (tag, V_NOTMEAS, int(region_px), 100.0 * region_px / frame_px,
                               int(frame_px), _provenance(ev)))
            n_notmeas += 1
            continue

        ev_lo, ev_hi = ev["indices"][0], ev["indices"][-1]
        ev_base = _nearest_baseline(base_idx, ev_lo, ev_hi, BASELINE_MAX_FRAMES)

        base_vals = []
        for k in ev_base:
            d = _region_frac(hot, k, region)
            if d is not None:
                base_vals.append(d)
        if len(base_vals) < MIN_BASELINE_FRAMES:
            ev_lines.append("%s %s(baseline: only %d clean frame(s) near [%d..%d], need %d)  %s"
                            % (tag, V_NOTMEAS, len(base_vals), ev_lo, ev_hi, MIN_BASELINE_FRAMES,
                               _provenance(ev)))
            n_notmeas += 1
            continue
        tau, med, mad = _threshold_from(base_vals, mlo)
        contaminated = sum(1 for v in base_vals if v > tau)

        if tau >= ATTAINABLE_MAX:
            ev_lines.append("%s %s(threshold unsatisfiable: tau=%.4f but d can never exceed %.1f - "
                            "no pixel change of ANY size could clear it, so this is a broken "
                            "instrument, not a reading)  %s"
                            % (tag, V_NOTMEAS, tau, ATTAINABLE_MAX, _provenance(ev)))
            n_notmeas += 1
            continue

        if over_cap:
            ev_lines.append("%s %s(camera motion: M_med=%.4f cap=%.4f - the picture is changing too "
                            "fast to attribute a local change to this label)  %s"
                            % (tag, V_NOTMEAS, motion, cap, _provenance(ev)))
            n_notmeas += 1
            continue

        own = set(int(v) for v in ev["indices"])
        foreign = set()
        for other in events:
            if other is ev:
                continue
            for v in other["indices"]:
                if int(v) not in own:
                    foreign.add(int(v))

        def sigfn(k, reg):
            return _region_frac(hot, k, reg)

        verdicts = []
        details = []
        ev_runs = _runs_of(ev["indices"])
        for run_i, (rs, re_) in enumerate(ev_runs):
            prev_end = ev_runs[run_i - 1][1] if run_i > 0 else None
            next_start = ev_runs[run_i + 1][0] if run_i + 1 < len(ev_runs) else None
            r_on = rows.get(rs)
            e_on = mlo.match_label_entry(r_on, ev["node"], None) if r_on else None
            reg_on = _build_region(cap_dir, r_on, e_on, frame_w, frame_h, mlo) if e_on else region
            reg_on = reg_on or region

            on_lo = max(min(all_idx) + 1, rs - edge_w)
            if prev_end is not None:
                on_lo = max(on_lo, prev_end + 2)
            on_hi = min(rs + edge_w, re_)
            onset_k, onset_d = _dominant_edge(sigfn, on_lo, on_hi, reg_on, paths, tau, foreign)

            r_end = rows.get(re_)
            e_end = mlo.match_label_entry(r_end, ev["node"], None) if r_end else None
            reg_end = _build_region(cap_dir, r_end, e_end, frame_w, frame_h, mlo) if e_end else region
            reg_end = reg_end or region

            end_k = None
            end_truncated = (re_ + 1) > max(all_idx)
            if not end_truncated:
                lo = max(min(all_idx) + 1, re_ + 1 - edge_w)
                if onset_k is not None:
                    lo = max(lo, onset_k + 1)
                hi = re_ + 1 + edge_w
                if next_start is not None:
                    hi = min(hi, next_start - 1)
                end_k, _end_d = _dominant_edge(sigfn, lo, hi, reg_end, paths, tau, foreign)

            d_on = sigfn(rs, reg_on) if rs in paths and (rs - 1) in paths else None
            d_off = (sigfn(re_ + 1, reg_end)
                     if (not end_truncated and (re_ + 1) in paths) else None)

            n_on = (onset_k - rs) if onset_k is not None else None
            n_end = (end_k - (re_ + 1)) if end_k is not None else None

            if onset_k is None and end_k is None:
                verdicts.append((V_NOTVIS, "run[%d..%d] no pixel change above tau anywhere in "
                                           "+/-%d of the claim%s"
                                           % (rs, re_, edge_w, only_tag)))
            elif ceiling is not None and ((n_on is not None and abs(n_on) > ceiling)
                                          or (n_end is not None and abs(n_end) > ceiling)):
                verdicts.append((V_NOTMEAS, "run[%d..%d] shift beyond the measurable range "
                                            "(+/-%d)" % (rs, re_, ceiling)))
            elif n_on not in (None, 0):
                verdicts.append(("ONSET-SHIFT(%+d)" % n_on,
                                 "run[%d..%d] pixels first change at %d, label starts at %d"
                                 % (rs, re_, onset_k, rs)))
            elif n_end not in (None, 0):
                verdicts.append(("END-SHIFT(%+d)" % n_end,
                                 "run[%d..%d] pixels first clear at %d, label ends at %d"
                                 % (rs, re_, end_k, re_)))
            elif n_on is None:
                verdicts.append((V_NOTMEAS, "run[%d..%d] onset not decidable" % (rs, re_)))
            elif end_truncated:
                verdicts.append((V_NOTMEAS, "run[%d..%d] end truncated by the session's last "
                                            "frame" % (rs, re_)))
            elif n_end is None:
                verdicts.append((V_NOTMEAS, "run[%d..%d] end not decidable" % (rs, re_)))
            else:
                verdicts.append((V_PASS, "run[%d..%d] onset %d end %d" % (rs, re_, rs, re_ + 1)))

            details.append("      run[%d..%d]  d(onset=%d)=%s  d(clear=%s)=%s  tau=%.4f  "
                           "region=%s/%dpx"
                           % (rs, re_, rs, ("%.4f" % d_on) if d_on is not None else "n/a",
                              str(re_ + 1) if not end_truncated else "-",
                              ("%.4f" % d_off) if d_off is not None else "n/a",
                              tau, reg_on["source"], reg_on["npix"]))

        worst = V_PASS
        for v, _why in verdicts:
            if v == V_NOTVIS:
                worst = v
                break
            if v.startswith("ONSET-SHIFT") or v.startswith("END-SHIFT"):
                worst = v
            elif v == V_NOTMEAS and worst == V_PASS:
                worst = v

        conf = "HIGH"
        if contaminated:
            conf = "LOW"
        elif region["bin"] is None:
            conf = "MED"

        extra = ""
        if ev["type"] in mlo.TEXTURE_TYPES:
            try:
                patch = cache.rgb(anchor).crop(region["box"])
                cls, _detail = mlo.classify_patch(patch)
                extra = "  appearance=%s" % cls
            except Exception:
                extra = "  appearance=n/a"

        ev_lines.append("%s %-16s [%s] tau=%.4f base=%d  %s%s%s"
                        % (tag, worst + (only_tag if worst == V_NOTVIS else ""), conf, tau,
                           len(base_vals), _provenance(ev),
                           ("  CONTAMINATED=%d" % contaminated) if contaminated else "", extra))
        if not quiet:
            ev_lines.extend(details)
            for v, why in verdicts:
                if v != V_PASS:
                    ev_lines.append("      %-16s %s" % (v, why))

        if worst == V_PASS:
            n_pass += 1
        elif worst == V_NOTVIS:
            n_notvis += 1
        elif worst == V_NOTMEAS:
            n_notmeas += 1
        else:
            n_shift += 1

    lines.append("  clean-frame pool         %d (guard %d frame(s) either side of every window%s); "
                 "each event calibrates on the %d NEAREST of them"
                 % (len(base_idx), guard_used,
                    "" if guard_used == BASELINE_GUARD_FRAMES
                    else "; RELAXED from %d - a dense burst schedule left too few clean frames"
                         % BASELINE_GUARD_FRAMES,
                    BASELINE_MAX_FRAMES))
    lines.append("-" * 78)
    lines.extend(ev_lines)
    lines.append("-" * 78)
    lines.append("  PASS %d   SHIFT %d   NOT-VISIBLE %d   NOT-MEASURABLE %d   (of %d event(s))"
                 % (n_pass, n_shift, n_notvis, n_notmeas, len(events)))
    bad = n_shift + n_notvis
    lines.append("  VERDICT                  %s%s"
                 % ("PASS" if bad == 0 else "FAIL",
                    "" if bad == 0 else "  - the labels and the pixels disagree on %d event(s)" % bad))
    if n_notmeas:
        lines.append("  NOT-MEASURABLE is NOT a pass and NOT a failure - it is an unread surface, "
                     "and the reason is printed on the event's own line.")
    return (0 if bad == 0 else 2), lines


def _synth_backdrop(w, h, total, pan, blocks, stripes):
    """A backdrop wide enough to be SCROLLED for `total` frames at `pan` px/frame.

    Two textures, both deterministic:
      stripes > 0 - hard 0/255 vertical bars of that width. Scrolling by exactly one bar width
                    flips EVERY pixel, so the changed fraction is 1.0 and nothing local can ever
                    exceed it. That is the "threshold unsatisfiable" fixture.
      otherwise   - blocky pseudo-random, block size `blocks`. Scrolling by `pan` changes the
                    pixels within `pan` of a block edge, i.e. a changed fraction of pan/blocks -
                    a dial for "how much is the camera moving".
    """
    from PIL import Image
    unit = max(blocks, stripes, 1)
    width = w + pan * total + unit * 2
    px = []
    for y in range(h):
        by = y // max(1, blocks)
        for x in range(width):
            if stripes > 0:
                px.append(255 if (x // stripes) % 2 == 0 else 0)
            else:
                px.append((((x // max(1, blocks)) * 73 + by * 151) * 37) % 256)
    im = Image.new("L", (width, h))
    im.putdata(px)
    return im


def _synth_session(root, name, shift=0, end_shift=0, blank_region=False, with_mask=False,
                   pan=0, blocks=16, stripes=0, fullframe_region=False):
    """Build a synthetic session with a KNOWN onset and end, then optionally lie about it.

    `pan == 0` keeps the ORIGINAL static fixture byte-for-byte: flat grey, a fixed dark square,
    and the target rectangle drawn on every frame. Those seven cases must go on reading exactly
    what they read before the ring normalisation.

    `pan > 0` scrolls a textured backdrop, which is the regime that had never been tested: on a
    moving camera the CLEAN frames change too, which is what drove the old learned threshold past
    its own statistic's ceiling (G259). The target is then drawn only while anomalous, so the
    region carries moving scene content when clean - exactly as a real session does.
    """
    from PIL import Image, ImageDraw
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    if with_mask:
        os.makedirs(os.path.join(d, "target_mask"))
    w, h = 160, 120
    box = (40, 30, 90, 80)
    true_start, true_end = 10, 17
    total = 30

    bg = _synth_backdrop(w, h, total, pan, blocks, stripes) if pan > 0 else None

    for i in range(total):
        anomalous = true_start <= i <= true_end
        if pan > 0:
            off = pan * i
            im = bg.crop((off, 0, off + w, h)).convert("RGB")
            dr = ImageDraw.Draw(im)
            if anomalous:
                dr.rectangle(list(box), fill=(230, 230, 230))
        else:
            im = Image.new("RGB", (w, h), (90, 90, 90))
            dr = ImageDraw.Draw(im)
            dr.rectangle([10, 10, 30, 30], fill=(60, 60, 60))
            dr.rectangle(list(box), fill=(230, 230, 230) if anomalous else (70, 70, 70))
        im.save(os.path.join(d, "Actual_Frames", "frame_%05d.png" % i))
        if with_mask:
            mk = Image.new("L", (w, h), 0)
            ImageDraw.Draw(mk).rectangle(list(box), fill=222)
            mk.save(os.path.join(d, "target_mask", "frame_%05d.png" % i))

    lo, hi = true_start + shift, true_end + shift + end_shift
    idxs = list(range(lo, hi + 1))
    if fullframe_region:
        label_box = [0, 0, w, h]
    elif blank_region:
        label_box = [5, 5, 8, 8]
    else:
        label_box = [box[0], box[1], box[2] - box[0], box[3] - box[1]]

    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for i in reversed(range(total)):
            row = {"frame_index": 1000 + i, "session_index": i,
                   "image": "Actual_Frames/frame_%05d.png" % i,
                   "width": w, "height": h,
                   "anomaly_present": i in idxs, "anomalies": [], "visible_positive": i in idxs}
            if with_mask:
                row["mask_file"] = "target_mask/frame_%05d.png" % i
                row["mask_state"] = "present"
            if i in idxs:
                a = {"id": "missing_texture", "target_name": "SynthTarget",
                     "start_frame": 1000 + lo, "bbox_valid": True, "bbox_px": label_box}
                if with_mask:
                    a["mask_value"] = 222
                row["anomalies"].append(a)
            fh.write(json.dumps(row) + "\n")

    ann = {"session_id": name,
           "video": {"path": "", "frames_dir": "Actual_Frames", "resolution": [w, h],
                     "fps": 30, "target_fps": 30, "total_frames": total},
           "anomalies": [{"anomaly_type": "missing_texture", "anomaly_subtype": "missing_texture",
                          "affected_frames": {"start_frame": lo, "end_frame": hi,
                                              "frame_count": len(idxs), "frame_indices": idxs},
                          "manifested": True, "coverage_ratio": 0.1, "coverage_pct": 10.0,
                          "affected_objects": {"count": 1, "primary_index": 0,
                                               "nodes": [{"name": "SynthTarget", "path": "",
                                                          "asset_name": "SynthMesh",
                                                          "component_class": "StaticMeshComponent"}]},
                          "mask": {"provided": bool(with_mask)}, "depth": {"provided": False}}]}
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    return d


def _shifted_copy_of(src_dir, dst_dir, delta):
    """Copy a real session's LABELS ONLY and move every annotated window by `delta`.

    The frames are referenced in place, never copied and never modified, and the source
    session is opened read-only. This is how a real banked leg becomes a known-answer
    fixture without touching the bank.
    """
    import shutil
    os.makedirs(dst_dir, exist_ok=True)
    for name in ("labels.jsonl", "run_summary.json"):
        s = os.path.join(src_dir, name)
        if os.path.isfile(s):
            shutil.copy2(s, os.path.join(dst_dir, name))
    for sub in ("Actual_Frames", "target_mask"):
        s = os.path.join(src_dir, sub)
        if os.path.isdir(s):
            try:
                os.symlink(s, os.path.join(dst_dir, sub), target_is_directory=True)
            except (OSError, NotImplementedError, AttributeError):
                shutil.copytree(s, os.path.join(dst_dir, sub))
    with open(os.path.join(src_dir, "annotation.json"), "r", encoding="utf-8-sig") as fh:
        ann = json.load(fh)
    for ev in ann.get("anomalies") or []:
        af = ev.get("affected_frames") or {}
        idx = af.get("frame_indices")
        if isinstance(idx, list) and idx:
            af["frame_indices"] = [int(v) + delta for v in idx]
            af["start_frame"] = af["frame_indices"][0]
            af["end_frame"] = af["frame_indices"][-1]
    with open(os.path.join(dst_dir, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    return dst_dir


def _label_pixel_selftest(thresh, edge_w, min_visible_px, source_dir=None, motion_cap=None):
    """Prove the gate can FAIL, in BOTH directions and on BOTH edges (G96/G142).

    Without --dir it builds synthetic sessions, so the check is portable and a client can
    run it with nothing but this file, measure_label_offset.py and Pillow. With --dir it
    additionally shifts a REAL session's labels on a copy - the frames and the source
    session are never written to.

    EACH CASE DECLARES ITS OWN MOTION CAP, and the reason is that two different things are being
    tested. The scrolling cases exist to exercise the STATISTIC under motion, so the motion escape
    is disabled for them - otherwise the synthetic backdrop's own scroll rate (pan/blocks, e.g.
    0.125 at pan 2) sits above MOTION_CAP and every one of them would short-circuit to
    NOT-MEASURABLE, testing nothing. ONE case, `moving_over_cap`, runs at the SHIPPED cap and is
    the proof that the escape fires. Stated here rather than left to be discovered, because a
    suite that silently refuses its own cases is the vacuous-pass shape (G146).
    """
    import shutil
    import tempfile
    try:
        from PIL import Image
    except ImportError:
        print("SELFTEST: ERROR - Pillow is required.", flush=True)
        return 2

    root = tempfile.mkdtemp(prefix="m49_labelpixel_selftest_")
    rc = 0
    checks = []
    try:
        OFF = 1.0
        cases = [
            ("clean", dict(shift=0), V_PASS, OFF),
            ("clean_masked", dict(shift=0, with_mask=True), V_PASS, OFF),
            ("label_late_1", dict(shift=1), "ONSET-SHIFT(-1)", OFF),
            ("label_early_1", dict(shift=-1), "ONSET-SHIFT(+1)", OFF),
            ("end_late_1", dict(end_shift=1), "END-SHIFT(-1)", OFF),
            ("end_early_1", dict(end_shift=-1), "END-SHIFT(+1)", OFF),
            ("blank_region", dict(shift=0, blank_region=True), V_NOTVIS, OFF),

            ("moving_clean", dict(shift=0, pan=2), V_PASS, OFF),
            ("moving_fast", dict(shift=0, pan=8), V_PASS, OFF),
            ("moving_label_late_1", dict(shift=1, pan=2), "ONSET-SHIFT(-1)", OFF),
            ("moving_label_early_1", dict(shift=-1, pan=2), "ONSET-SHIFT(+1)", OFF),
            ("moving_end_late_1", dict(end_shift=1, pan=2), "END-SHIFT(-1)", OFF),
            ("moving_end_early_1", dict(end_shift=-1, pan=2), "END-SHIFT(+1)", OFF),
            ("moving_blank_region", dict(shift=0, pan=2, blank_region=True), V_NOTVIS, OFF),
            ("moving_fullframe_region", dict(shift=0, pan=2, fullframe_region=True), V_NOTMEAS, OFF),
            ("moving_unsat", dict(shift=0, pan=4, stripes=4), V_NOTMEAS, OFF),
            ("moving_over_cap", dict(shift=0, pan=2), V_NOTMEAS, None),
        ]
        for name, kwargs, expect, cap in cases:
            d = _synth_session(root, name, **kwargs)
            code, lines = label_pixel_gate(d, thresh, edge_w, min_visible_px, quiet=True,
                                           motion_cap=(motion_cap if cap is None else cap))
            body = [l for l in lines if l.startswith("idx=")]
            read = body[0].split()[3] if body and len(body[0].split()) > 3 else "(none)"
            ok = read.startswith(expect)
            checks.append((name, expect, read, ok, code))
            if not ok:
                rc = 3

        if source_dir and os.path.isdir(source_dir):
            for delta, expect in ((1, "ONSET-SHIFT(-1)"), (-1, "ONSET-SHIFT(+1)")):
                dst = os.path.join(root, "real_%+d" % delta)
                try:
                    _shifted_copy_of(source_dir, dst, delta)
                except Exception as exc:
                    checks.append(("real%+d" % delta, expect, "copy failed: %s" % exc, False, -1))
                    rc = 3
                    continue
                code, lines = label_pixel_gate(dst, thresh, edge_w, min_visible_px, quiet=True,
                                               motion_cap=OFF)
                body = [l for l in lines if l.startswith("idx=")]
                hits = sum(1 for l in body if expect in l)
                ok = hits > 0 and code == 2
                checks.append(("real%+d (%d event lines)" % (delta, len(body)),
                               expect, "%d event(s) read it" % hits, ok, code))
                if not ok:
                    rc = 3

        print("LABEL-PIXEL GATE SELFTEST", flush=True)
        print("  %-28s %-18s %-28s %s" % ("case", "expected", "read", "exit"), flush=True)
        for name, expect, read, ok, code in checks:
            print("  %-28s %-18s %-28s %s   %s"
                  % (name, expect, read, code, "OK" if ok else "*** BROKEN ***"), flush=True)
        if rc == 0:
            print("SELFTEST: OK - the gate passes an aligned session, reads a +/-1 label shift "
                  "back with the opposite sign, and calls a region with no change NOT-VISIBLE. "
                  "Its PASS is a reading, not blindness.", flush=True)
            print("           BOTH HALVES: it does that on a STILL camera and on a MOVING one, it "
                  "still FAILS a wrongly-placed label under motion, and it refuses - as "
                  "NOT-MEASURABLE, never as NOT-VISIBLE - when the region covers the picture or "
                  "when no measurement could clear the threshold.", flush=True)
        else:
            print("SELFTEST: BROKEN - see the rows marked above. A gate that cannot fail is not "
                  "a gate.", flush=True)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    return rc


def _batch_sessions(root, max_depth=6):
    """Every directory under `root` that LOOKS like a capture session.

    A session is anything carrying annotation.json or an Actual_Frames folder - deliberately NOT
    "anything carrying labels.jsonl", so that a session MISSING labels.jsonl is FOUND and reported
    CANNOT RUN instead of quietly not existing. An unread surface has to be visible to be read.
    """
    out = []
    root = os.path.abspath(root)
    base_depth = root.rstrip(os.sep).count(os.sep)
    for cur, dirs, files in os.walk(root):
        if cur.rstrip(os.sep).count(os.sep) - base_depth >= max_depth:
            dirs[:] = []
        names = set(files)
        if "annotation.json" in names or os.path.isdir(os.path.join(cur, "Actual_Frames")):
            out.append(cur)
            dirs[:] = [d for d in dirs if d not in ("Actual_Frames", "target_mask", "annotated")]
    return sorted(set(out))


def label_pixel_batch(root, out_dir, thresh, edge_w, min_visible_px, quiet, report_only,
                      motion_cap=None):
    """--all: run the gate over every session under `root`, one line each.

    The SELFTEST runs FIRST and the batch REFUSES to start if it is not OK. A sweep of a hundred
    sessions with a broken instrument produces a hundred confident wrong lines, and the cost of
    finding that out later is the whole sweep.
    """
    import datetime
    sessions = _batch_sessions(root)
    if not sessions:
        print("BATCH: no session-shaped folder under %s - nothing to read. Not a pass." % root,
              flush=True)
        return 3

    if not out_dir:
        stamp = datetime.datetime.now().strftime("%Y%m%d-%H%M%S")
        out_dir = os.path.join(root, "verifier-batch-%s" % stamp)
    os.makedirs(out_dir, exist_ok=True)

    print("BATCH: proving the instrument before using it on %d session(s)..." % len(sessions),
          flush=True)
    st = _label_pixel_selftest(thresh, edge_w, min_visible_px, None, motion_cap)
    if st != 0:
        print("BATCH: REFUSING TO RUN - the selftest is BROKEN, so every reading below it would "
              "be unfounded. Fix the gate first.", flush=True)
        return 3

    summary = []
    worst = 0
    for sess in sessions:
        name = os.path.basename(sess.rstrip(os.sep)) or sess
        if not os.path.isfile(os.path.join(sess, "labels.jsonl")):
            line = "%-46s | CANNOT RUN | no labels.jsonl (the gate needs the per-frame bbox)" % name
            summary.append(line)
            print(line, flush=True)
            worst = max(worst, 3)
            continue
        try:
            code, lines = label_pixel_gate(sess, thresh, edge_w, min_visible_px, quiet,
                                           motion_cap=motion_cap)
        except Exception as exc:
            line = "%-46s | ERROR      | %s: %s" % (name, type(exc).__name__, exc)
            summary.append(line)
            print(line, flush=True)
            worst = max(worst, 3)
            continue
        with open(os.path.join(out_dir, "%s.txt" % name), "w", encoding="utf-8") as fh:
            fh.write(sess + "\n")
            fh.write("\n".join(lines) + "\n")
        counts = ""
        verdict = ""
        for l in lines:
            if l.strip().startswith("PASS "):
                counts = " ".join(l.split())
            elif l.strip().startswith("VERDICT"):
                verdict = " ".join(l.split()[1:])
        line = "%-46s | exit %-4d | %s | VERDICT %s" % (name, code, counts or "(no events)",
                                                        verdict or "(none)")
        summary.append(line)
        print(line, flush=True)
        worst = max(worst, code)

    with open(os.path.join(out_dir, "SUMMARY.txt"), "w", encoding="utf-8") as fh:
        fh.write("root: %s\n" % root)
        fh.write("sessions: %d\n\n" % len(sessions))
        fh.write("\n".join(summary) + "\n")
    print("BATCH: %d session(s); full per-session output in %s" % (len(sessions), out_dir),
          flush=True)
    return 0 if report_only else worst


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
                    help="m47: prove the black-frame gate can FAIL, against a synthetic black frame. "
                         "With --label-pixel-gate it proves THAT gate can fail instead.")
    ap.add_argument("--label-pixel-gate", action="store_true",
                    help="m49: run the label-vs-pixel gate INSTEAD of the overlay. Per event it "
                         "checks that the first labelled frame is the first frame whose pixels "
                         "change, and that the frame after end_frame is the first clean one.")
    ap.add_argument("--diff-threshold", type=int, default=DIFF_THRESH_DEFAULT,
                    help=f"a pixel COUNTS as changed when it differs from the previous frame by "
                         f"more than this, 0..255 (default {DIFF_THRESH_DEFAULT})")
    ap.add_argument("--edge-window", type=int, default=EDGE_WINDOW_DEFAULT,
                    help=f"how many frames either side of a claimed edge to search "
                         f"(default {EDGE_WINDOW_DEFAULT})")
    ap.add_argument("--min-visible-px", type=int, default=MIN_VISIBLE_PX_DEFAULT,
                    help=f"a positive frame whose mask carries fewer than this many pixels is "
                         f"NOT-VISIBLE (default {MIN_VISIBLE_PX_DEFAULT}; needs masks)")
    ap.add_argument("--report-only", action="store_true",
                    help="label-pixel gate: print the readings and exit 0 even on a shift")
    ap.add_argument("--motion-cap", type=float, default=MOTION_CAP,
                    help=f"label-pixel gate: refuse a session as NOT-MEASURABLE when its measured "
                         f"camera motion M_med exceeds this (default {MOTION_CAP}; set by the "
                         f"079-03 known-answer procedure, see the module header)")
    ap.add_argument("--all", metavar="ROOT", default=None,
                    help="label-pixel gate: run over EVERY session folder under ROOT, one summary "
                         "line each, full output under --out. Runs --selftest first and refuses "
                         "to start if it is not OK.")
    args = ap.parse_args()

    if args.selftest:
        if args.label_pixel_gate:
            src = os.path.abspath(args.dir) if args.dir and os.path.isdir(args.dir) else None
            sys.exit(_label_pixel_selftest(args.diff_threshold, args.edge_window,
                                           args.min_visible_px, src, args.motion_cap))
        sys.exit(_selftest())

    if args.all:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        sys.exit(label_pixel_batch(os.path.abspath(args.all),
                                   os.path.abspath(args.out) if args.out else None,
                                   args.diff_threshold, args.edge_window, args.min_visible_px,
                                   args.quiet, args.report_only, args.motion_cap))

    cap_dir = os.path.abspath(args.dir)

    if args.label_pixel_gate:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        code, lines = label_pixel_gate(cap_dir, args.diff_threshold, args.edge_window,
                                       args.min_visible_px, args.quiet, args.motion_cap)
        for line in lines:
            print(line, flush=True)
        sys.exit(0 if (args.report_only and code != 3) else code)

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
