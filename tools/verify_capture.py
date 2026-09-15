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
m49 LABEL-PIXEL GATE  (--label-pixel-gate) - its PER-EDGE VALIDITY ENVELOPE (079-06) and the
MASK-MODE SCOPE BOUNDARY (079-07)
--------------------------------------------------------------------------------------------------
🔑 VERDICTS ARE GIVEN ONLY WHERE A PER-FRAME MASK IDENTIFIES THE TARGET'S PIXELS. That is M3
datasets and bench captures. On a capture that carries only the producer's BOUNDING BOX, this tool
prints READINGS for a human to look at and NEVER a verdict, and the session ends UNREAD-BBOX-ONLY at
exit 0.

WHY, AND IT IS A BOUNDARY RATHER THAN A THRESHOLD. This gate asks "did the picture change where the
label says, when the label says". Inside a MASK the pixels being differenced are known to be the
target's, so the answer means something. Inside a BOX they are the target AND the floor behind it
AND whatever the camera swept past. Codex's 079-07 review built the minimal case: a 20x20 patch
appears on frame 60, an unrelated background scroll happens ONCE at frame 61 inside the labelled
window, and every clean baseline pair is unchanged. The burst changes 0.4960 of the box against the
real onset's 0.1600 - so it wins the argmax by 3.1x, clears every dominance and stillness test there
is, and the tool reported ONSET-SHIFT(+1) on CORRECT labels and PASS on a deliberately LATE one.
Lowering the cap does not touch it: the false shift survives at cap 0, because the baseline really
is still. NOTHING IN A BOX SAYS WHICH THING INSIDE IT MOVED, and no threshold can supply that.

Per event, in mask mode, it checks that the first labelled frame is the first frame whose pixels
change, and that the frame after end_frame is the first clean one. The statistic is

    d(k) = the fraction of the edge's REGION whose pixels differ from frame k-1 by > threshold

which is BOUNDED IN [0,1], and the decision threshold is tau = max(median + K_SIGMA*MAD, FLOOR)
learned from the clean frames NEAREST THAT EDGE, on THAT EDGE'S OWN REGION.

WHY THIS TOOL REFUSES TO JUDGE A REGION IT CANNOT READ. `d` is bounded by 1.0; tau is bounded only
by its inputs (at most 1 + 6*0.5 = 4.0) - so TAU CAN EXCEED WHAT d CAN EVER REACH. When the picture
is changing, the CLEAN frames change too, the learned tau climbs, and past 1.0 `d > tau` is
unsatisfiable: EVERY event reads NOT-VISIBLE no matter what the pixels show. That is not a
hypothetical. On M2 field captures it marked almost every event NOT-VISIBLE on labels the owner then
verified BY EYE as correct, and on a banked heavy-motion session tau measured 1.0950 and 1.1938
(G259). So judgeability is MEASURED, and an unreadable edge is REFUSED rather than given a verdict
the measurement cannot support.

WHAT IS MEASURED, AND WHERE - THIS IS THE WHOLE OF 079-06:

    m_edge = the MEDIAN of d over the clean frames NEAREST THAT EDGE, on THAT EDGE'S OWN REGION
    an edge with m_edge > REGION_CAP reads NOT-MEASURABLE(regional image change), per edge

It is an IMAGE-CHANGE PROXY, NOT A CAMERA-DISPLACEMENT MEASUREMENT. A still camera in front of a
waterfall reads high; a slow pan across a flat wall reads low. It answers only "does this region
change on its own when nothing is happening here", which is the question a local verdict needs.

THE PREVIOUS DESIGN WAS ONE WHOLE-FRAME MEDIAN OVER THE FIRST 24 CLEAN PAIRS, REUSED BY EVERY EVENT,
AND IT WAS BLIND TWICE OVER (Codex's 079-05 review; both reproduced here as selftest cases):
  IN TIME   a quiet opening authorised verdicts on a later moving scene. A fixture whose first 36
            frames are still reported motion 0.0000 while the value local to its labelled window
            was 0.1250, and the gate emitted a confident ONSET-SHIFT on a CORRECT label.
  IN SPACE  a whole-frame fraction can be tiny while the region changes completely. A 50x50 region
            scrolling inside a static 320x240 picture reads 0.0039 whole-frame and 0.1250
            regionally. Lowering a whole-frame constant cannot repair that - the quantity was wrong.
The whole-frame median is still PRINTED, now over ALL clean pairs, and it GATES NOTHING.

AN EDGE MUST ALSO DOMINATE ITS RUNNER-UP (>= 1.5x, among the frames that cleared tau) or the answer
is a refusal rather than a frame number. An argmax always returns something and never says the
contest was close; on a banked leg an adjacent event's transition won an onset search four frames
away and the event printed a confident shift anyway.

WHERE REGION_CAP COMES FROM - a PROVISIONAL HEURISTIC: derived, not chosen, and NOT a validated
client acceptance envelope. CaptureBench/tools/m079_shift_recovery.py takes banked moving-camera
sessions, MOVES THE LABELS BY A KNOWN +/-1 (onset-only, end-only and both; labels only, frames
untouched), and scores a FIXED COHORT: every contiguous run x both edges of the UNSHIFTED session,
x 7 perturbations x 2 label variants minus the redundant coherent delta 0 = 13 COMBINATIONS PER
ORIGINAL EDGE. The denominator is published before scoring and never shrinks, so losing coverage
can never raise a recovery rate. The cap is the largest m_edge at which that cohort produced ZERO
WRONG offsets with the guard disabled.

READ THE N BESIDE IT CAREFULLY, BECAUSE IT IS NOT WHAT IT LOOKS LIKE. N counts PERTURBATION CELLS
THAT REACHED A BASELINE. It is not a count of independent real edges and not a count of successful
validations - the same handful of original edges appears in it thirteen times over. In 079-06's
cohort of 780 cells, every single retained success came from FOUR ORIGINAL EDGES of two events in
ONE session, whose clean-pair medians ran from 0 to 0.00005820: static regions between events. And
the cap is a statement about QUIET CLEAN PAIRS, which is not the same thing as an ATTRIBUTABLE
change inside the labelled window - the 079-07 burst fixture has m_edge 0 at both edges and still
had its onset hijacked, which is why attribution is now a scope rule rather than a cap.

079-07 ALSO CORRECTS THE COHORT ITSELF. A bbox-only run can no longer return a verdict, so its
cells could only ever be refusals and would dilute every rate in the table. The cohort is therefore
MASK-MODE SESSIONS ONLY, and M50L_LG9 - bbox-only on 16 of 16 edges - leaves it. The table is in
that tool's header; the number and its N are in REGION_CAP_PROVENANCE below.

A CLAIM THIS FILE USED TO MAKE AND NO LONGER DOES. 079-03/079-04 set a whole-frame MOTION_CAP of
0.040 on the strength of "A2L_LEGA recovers an injected +/-1 fully". It does not: that score was
taken from the EVENT token, and underneath it two of eight runs returned offsets of -2 and -4 where
+1 was correct, while the event printed the expected ONSET-SHIFT(+1) from its OTHER run. The cap and
its positive anchor are WITHDRAWN (079-06). The numbers stay in the journals as history.

THERE IS NO MIDDLE TIER, AND THE REASON IS THE COMPLAINT THIS TOOL EXISTS FOR. It is tempting to
trust a PASS in conditions where a SHIFT could not have been read. Refused: A PASS FROM A GATE THAT
CANNOT READ BACK A ONE-FRAME SHIFT IS NOT EVIDENCE OF ALIGNMENT, IT IS THE CLIENT'S ORIGINAL
COMPLAINT RESTATED. Where the instrument provably cannot see a one-frame error, "no error found"
carries no information about whether one is there. The answer is NOT-MEASURABLE, full stop.

HONEST LIMITS, because they decide whether a reading is worth anything:
  - This refuses a great deal of real gameplay footage, now per EDGE rather than per session. The
    tool is then HONEST but not USEFUL on that footage - it prints the numbers and stops. That trade
    is deliberate: a confident wrong answer about a client's dataset is worse than no answer.
  - The cap rests on four banked sessions. It is a PROVISIONAL HEURISTIC, to be re-derived whenever
    more masked moving-camera sessions exist, and it is not a client acceptance envelope.
  - NOT-VISIBLE means NO DETECTABLE ABOVE-THRESHOLD CHANGE AT EITHER EDGE, within the searched
    windows, of a region judged readable, EVERY FRAME PAIR OF WHICH WAS ACTUALLY OBSERVED. IT IS
    NOT A GUARANTEE OF ABSENCE - a real change smaller than that region's learned tau reads
    exactly the same way. Measured: a single changed pixel inside a 50x50 mask gives d = 0.0004
    against tau = 0.0040, and the honest output for it is NOT-VISIBLE. A FAIL therefore says
    "labels not confirmed by pixels", which is what the measurement supports; it does NOT say the
    labels and the pixels disagree, which would claim more than a detection limit can carry.
  - `(bbox-only)` means the SUPPLIED BOUNDING BOX was used because no per-frame mask was
    available. Since 079-07 that is a READING and never a verdict.
  - `bbox=... obs=...` is PRODUCER METADATA copied off the labels. This checker measures neither.
  - PARTIAL means some edges of the event were read and agreed while others were refused. It is
    neither a pass nor a failure and is counted separately from both.
  - Nothing here says a refused edge's label is wrong. NOT-MEASURABLE is an UNREAD SURFACE.

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
    python verify_capture.py --dir <sessionDir> --label-pixel-gate [--report-only] [--region-cap N]
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

DOMINANCE = 1.5

REGION_CAP = 0.00014
REGION_CAP_N = 572
REGION_CAP_PROVENANCE = (
    "PROVISIONAL HEURISTIC, re-derived 079-07 on the MASK-MODE cohort: 44 original run-edges of "
    "A2L_LEGA, LYRA_SMOKE_01 and A1L_LEGA x 13 combinations (7 label perturbations x 2 variants "
    "minus the redundant coherent delta 0) = 572 CELLS, every one of which reached a baseline. "
    "M50L_LG9 left the cohort because it is bbox-only on 16 of 16 edges and can no longer return a "
    "verdict. With the guard DISABLED that cohort recovers 368, refuses 175, reads 12 no-signal and "
    "gets 17 WRONG; the cap is the largest m_edge below the smallest wrong cell, m_edge=0.00014459 "
    "(A1L_LEGA event 0 run[4..5] onset, annotation-only both+1, which INVERTED the SIGN of the "
    "injected shift). At the cap: 52 recovered, 0 wrong, 520 refused. "
    "READ N AS WHAT IT IS: 572 PERTURBATION CELLS, not 572 independent edges and not 572 "
    "validations - and all 52 retained successes come from FOUR original edges of two events in "
    "ONE session (A2L_LEGA events 4 and 5), i.e. static regions between events. NOT a validated "
    "client acceptance envelope and NOT sufficient for correctness - a still baseline says nothing "
    "about whether an in-window change is ATTRIBUTABLE, which is why masks, not this number, are "
    "what license a verdict. See m079_shift_recovery.py")

V_PASS = "PASS"
V_NOTVIS = "NOT-VISIBLE"
V_NOTMEAS = "NOT-MEASURABLE"
V_PARTIAL = "PARTIAL"
V_READING = "READING"

NO_ADMISSIBLE_ENVELOPE = "NO ADMISSIBLE ENVELOPE"


def _normalise_cap(value):
    """Turn a caller's `region_cap` into (numeric cap or None, refuse_all).

    Three states, kept apart on purpose (079-07 / Codex F6):
      a NUMBER          refuse an edge whose m_edge is GREATER than it. A number of 0.0 therefore
                        still JUDGES a perfectly still region (m_edge == 0), which is every one of
                        the pinned bench's 64 edges - so 0.0 must never be described as refusing
                        everything.
      None              the guard is inert and every edge is judged.
      NO ADMISSIBLE
      ENVELOPE          the calibration cohort's smallest-change cell was already wrong, so no
                        positive cap is defensible. EVERY edge is refused. This is a state, not a
                        number, and it is carried as one rather than smuggled in as a zero.
    """
    if value is None:
        return None, False
    if isinstance(value, str):
        if value.strip().upper() == NO_ADMISSIBLE_ENVELOPE:
            return None, True
        return float(value), False
    return float(value), False


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

    Whole-frame on purpose: ONE binary difference image per frame pair serves every region that
    asks about that pair, so two regions are never compared across two independently computed
    diffs. Cached because a session's events, runs and per-edge baselines re-read the same frame
    pairs many times over.
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

    Ã¢â€¡â€™ THE RING IS NOT USED AT ALL. Picture change is handled by MEASURING IT WHERE THE VERDICT
    WILL BE MADE - per edge, on that edge's own region, over the clean frames nearest it - and
    refusing above REGION_CAP, not by trying to subtract it away.
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


def _edge_search(sigfn, lo, hi, region, paths, tau, foreign=None):
    """Rank every candidate frame in the window and report whether the winner DOMINATES.

    The edge is where the BIGGEST change in the neighbourhood is, not the first one above tau.
    Measured on banked m45 legs: the frame AFTER a hide still differs from its predecessor
    because temporal accumulation is still decaying the object out, so "the first frame above
    tau" reads the ghost and reports a one-frame shift that is not there. The frame after a
    reappearance has the same problem in the other direction.

    `foreign` is the set of frames belonging to OTHER events. A neighbourhood that reaches
    into another event's window would otherwise let that event's transition win the argmax -
    measured on a banked leg where two events fire on the SAME actor eight frames apart, and
    the second swap was read as the first one's end.

    079-06 ADDS DOMINANCE. An argmax always returns something; it never says the contest was
    close. Codex's 079-05 review reproduced the consequence on a banked leg: an adjacent event's
    clear transition won an onset search four frames away and the event still printed a confident
    shift. So the winner must beat the RUNNER-UP by DOMINANCE (1.5x) or the answer is a refusal,
    not a frame number: a competitor within two thirds of the winner's magnitude is not
    distinguishable from it by this statistic, and the argmax between them is a coin flip.

    079-07 WITHDRAWS THE CLAIM THAT DOMINANCE IS "THE DIRECT CURE FOR SOMETHING ELSE WON THE
    ARGMAX". IT IS NOT, AND CODEX'S SECOND REVIEW MEASURED THE DIFFERENCE. Dominance decides
    whether the contest was CLOSE. It does not decide whether the winner belongs to the TARGET.
    In their fixture a 20x20 patch appears on frame 60 and an unrelated 10px background scroll
    happens once at 61, inside the labelled window, with every clean pair unchanged: the burst
    changes 0.4960 of the region against the real onset's 0.1600 and wins dominance by 3.1x, so
    CORRECT labels read ONSET-SHIFT(+1) and a deliberately LATE label read PASS. Lowering the cap
    does not help - the false shift survives at cap 0, because the baseline is already still.
    ATTRIBUTION, NOT SEPARATION, IS THE MISSING INGREDIENT, and the only thing that supplies it
    is a per-frame mask. See `attributed` in _measure_edge and the mask-mode rule in the header.

    COVERAGE IS REPORTED BESIDE THE ANSWER (079-07 / Codex F3). `missing` counts frame pairs of
    the window whose images are not on disk or could not be differenced; `excluded` counts pairs
    deliberately dropped because they belong to ANOTHER event. They are different facts: the
    first is an unavailable observation and refuses the edge, the second is part of the window's
    definition. An empty search used to set status "read" and report "0 of 0 frames above tau",
    which the run combiner then turned into NOT-VISIBLE - an absence claim from no observations.

    THE RUNNER-UP IS TAKEN FROM THE ELIGIBLE SET - the frames with d > tau - and NOT from the
    whole window, because that is the set the argmax actually ranges over. A frame that never
    cleared the detection threshold was never a candidate, and counting it would make the test
    depend on how quiet the window happened to be rather than on how close the contest was.
    With fewer than two eligible frames the winner is dominant by construction.

    Replaces `_dominant_edge`, which returned only the winner. THE NAME CHANGED ON PURPOSE: an
    audit that monkeypatched the old function would otherwise observe nothing and read as a
    clean result. A loud AttributeError is the better failure.
    """
    cands = []
    missing = []
    excluded = []
    for k in range(lo, hi + 1):
        if k not in paths or (k - 1) not in paths:
            missing.append(k)
            continue
        if foreign and (k in foreign or (k - 1) in foreign):
            excluded.append(k)
            continue
        d = sigfn(k, region)
        if d is None:
            missing.append(k)
            continue
        cands.append((k, d))
    eligible = sorted([c for c in cands if c[1] > tau], key=lambda c: (-c[1], c[0]))
    out = {"scanned": len(cands), "eligible": len(eligible), "best_k": None, "best_d": None,
           "second_k": None, "second_d": None, "ratio": None, "dominant": None,
           "window": max(0, hi - lo + 1), "missing": missing, "excluded": excluded}
    if not eligible:
        return out
    out["best_k"], out["best_d"] = eligible[0]
    if len(eligible) > 1:
        out["second_k"], out["second_d"] = eligible[1]
        out["ratio"] = (out["best_d"] / out["second_d"]) if out["second_d"] > 0 else None
        out["dominant"] = (out["ratio"] is None) or (out["ratio"] >= DOMINANCE)
    else:
        out["dominant"] = True
    return out


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


def _measure_edge(kind, nominal, region, region_fallback, frame_w, frame_h, mlo, hot, base_idx,
                  sigfn, paths, foreign, lo, hi, region_cap, ceiling, truncated, refuse_all=False):
    """One EDGE of one contiguous run, judged on its OWN region and its OWN local clean frames.

    079-06 / Codex finding 1. Until this function existed, measurability was decided ONCE per
    SESSION from a whole-frame changed-pixel median taken over the FIRST 24 clean pairs, and
    every event reused it. That is blind twice over:

      IN TIME   a quiet opening authorises a verdict on a later moving scene. Reproduced: a
                fixture whose first 36 frames are still and whose labelled window sits in the
                moving part reported motion 0.0000 while its local value was 0.1250.
      IN SPACE  a whole-frame fraction can be tiny while the REGION changes completely. A 50x50
                region scrolling inside a static 320x240 picture reads 0.0039 whole-frame and
                0.1250 regionally - the same picture, a factor of 32.

    So the question asked here is the only one that matters for a verdict: over the clean frames
    NEAREST THIS EDGE, how much does THIS EDGE'S OWN REGION change when nothing is happening?
    That number is `m_edge`, and above REGION_CAP the edge is refused.

    THE REGION IS THE EDGE'S OWN, not the event's anchor region. Codex named the mismatch: tau was
    learned on the anchor's box while later runs and the end used different masks, so a target
    that moves changed the sampled content without recalibrating.

    ATTRIBUTION IS A SEPARATE AXIS FROM MEASURABILITY, AND 079-07 IS WHERE THEY WERE SEPARATED.
    `attributed` is true only when the region came from the delivered PER-FRAME MASK, i.e. when
    the pixels being differenced are known to be the target's. An edge whose region is the
    supplied BOUNDING BOX is still measured here - every number below is computed and printed -
    but it may not produce a verdict, because nothing identifies which of the things inside that
    box moved. Codex's F1 fixture is the proof: a background scroll inside the box beat the real
    onset 3.1x and turned CORRECT labels into a confident shift, at a perfectly still baseline.

    ORDER OF THE REFUSALS, and it matters: region present, non-empty window, region size,
    end-truncation, baseline count, threshold satisfiable, SEARCH COVERAGE, no-admissible-
    envelope, regional change, dominance, then measurable range. The first group are all "the
    observation is not available"; the next two are "this instrument will not judge here"; the
    rest are about the quality of an answer that does exist. The more specific fault is the one
    reported when several hold, and each can be exercised separately.

    THE SEARCH RUNS BEFORE EVERY REFUSAL THAT CAN STILL BE SEARCHED (079-07 / Codex F2). `best_k`
    bounds the dependent edge from below - a run's clear cannot precede its own onset, and the
    next run's onset cannot precede this run's clear - and that bound must NOT depend on whether
    the preceding edge was TRUSTED. Codex measured both directions: a whole-frame onset box was
    refused before searching, so the end search was unbounded, re-found the onset transition and
    returned END-SHIFT(-2) on aligned labels; and a first end that was refused OR correctly read
    still released the second run's onset, which re-used the first run's clear as its own onset.
    A REFUSAL MUST REMOVE AN ANSWER, NEVER A CONSTRAINT. Where tau could not be learned, or is
    unsatisfiable, the search runs at SIGNAL_FLOOR and the result is marked `bound_only`: it is
    used as an ordering constraint and is never promoted to an answer.

    Returns a dict. `status` is "read" or "refused"; a read edge carries `offset` (None when
    nothing in the window cleared tau) and `found`.
    """
    rec = {"kind": kind, "nominal": nominal, "status": "refused", "reason": None,
           "tau": None, "m_edge": None, "mad": None, "base_n": 0, "contaminated": 0,
           "best_k": None, "best_d": None, "second_d": None, "ratio": None,
           "eligible": 0, "scanned": 0, "found": False, "offset": None,
           "region_source": None, "region_npix": None, "d_nominal": None,
           "attributed": False, "bound_only": False, "lo": lo, "hi": hi,
           "window": max(0, hi - lo + 1), "missing": 0, "excluded": 0}

    reg = region or region_fallback
    if reg is None:
        rec["reason"] = "no-region: no usable mask or bbox at frame %d" % nominal
        return rec
    rec["region_source"] = reg["source"]
    rec["region_npix"] = reg["npix"]
    rec["attributed"] = reg.get("bin") is not None

    if lo > hi:
        rec["reason"] = ("window emptied by the preceding edge: the ordering bound left no frame "
                         "to search around %d" % nominal)
        return rec

    base = _nearest_baseline(base_idx, nominal, nominal, BASELINE_MAX_FRAMES)
    vals = []
    for k in base:
        d = _region_frac(hot, k, reg)
        if d is not None:
            vals.append(d)
    rec["base_n"] = len(vals)
    tau = med = mad = None
    if len(vals) >= MIN_BASELINE_FRAMES:
        tau, med, mad = _threshold_from(vals, mlo)
        rec["tau"], rec["m_edge"], rec["mad"] = tau, med, mad
        rec["contaminated"] = sum(1 for v in vals if v > tau)

    tau_search = tau if (tau is not None and tau < ATTAINABLE_MAX) else mlo.SIGNAL_FLOOR
    rec["bound_only"] = (tau is None or tau >= ATTAINABLE_MAX)
    found = _edge_search(sigfn, lo, hi, reg, paths, tau_search, foreign)
    rec.update(scanned=found["scanned"], eligible=found["eligible"], best_k=found["best_k"],
               best_d=found["best_d"], second_d=found["second_d"], ratio=found["ratio"],
               window=found["window"], missing=len(found["missing"]),
               excluded=len(found["excluded"]))
    rec["d_nominal"] = (sigfn(nominal, reg)
                        if nominal in paths and (nominal - 1) in paths else None)

    frame_px = float(frame_w * frame_h)
    region_px = mlo.box_area(reg["box"])
    if frame_px > 0 and region_px >= MAX_REGION_FRAC * frame_px:
        rec["reason"] = ("region covers the picture: %dpx = %.1f%% of the %dpx frame, at or above "
                         "the %.0f%% bounding-rectangle limit, so it cannot be localised against it"
                         % (int(region_px), 100.0 * region_px / frame_px, int(frame_px),
                            MAX_REGION_FRAC * 100.0))
        return rec

    if truncated:
        rec["reason"] = "end truncated by the session's last frame"
        return rec

    if len(vals) < MIN_BASELINE_FRAMES:
        rec["reason"] = ("baseline: only %d clean frame(s) near frame %d, need %d"
                         % (len(vals), nominal, MIN_BASELINE_FRAMES))
        return rec

    if tau >= ATTAINABLE_MAX:
        rec["reason"] = ("threshold unsatisfiable: tau=%.4f but d can never exceed %.1f - no pixel "
                         "change of ANY size could clear it, so this is a broken instrument, not a "
                         "reading" % (tau, ATTAINABLE_MAX))
        return rec

    if found["missing"]:
        rec["reason"] = ("frames missing in window: %d of the %d frame pair(s) in %d..%d could not "
                         "be differenced (%s) - an absence of OBSERVATIONS is not an observed "
                         "absence" % (len(found["missing"]), found["window"], lo, hi,
                                      mlo.compress_indices(found["missing"])))
        return rec

    if found["scanned"] == 0:
        rec["reason"] = ("nothing to search in %d..%d: %d frame pair(s) all belong to another "
                         "event and were excluded" % (lo, hi, len(found["excluded"])))
        return rec

    if refuse_all:
        rec["reason"] = ("%s: the calibration cohort's smallest-change cell was already wrong, so "
                         "no positive regional-change cap is defensible and EVERY edge is refused"
                         % NO_ADMISSIBLE_ENVELOPE.lower())
        return rec

    if region_cap is not None and med is not None and med > region_cap:
        rec["reason"] = ("regional image change: m=%.6f cap=%.6f over the %d clean pair(s) nearest "
                         "frame %d - this region is not still enough for a local change to be "
                         "attributed to the label" % (med, region_cap, len(vals), nominal))
        return rec

    if found["best_k"] is None:
        rec["status"] = "read"
        rec["found"] = False
        return rec

    if not found["dominant"]:
        rec["reason"] = ("ambiguous edge: best d=%.4f at %d, runner-up d=%.4f at %d, ratio %.2fx "
                         "below the %.2fx needed - the argmax between them is a coin flip"
                         % (found["best_d"], found["best_k"], found["second_d"], found["second_k"],
                            found["ratio"] if found["ratio"] is not None else 0.0, DOMINANCE))
        return rec

    offset = found["best_k"] - nominal
    if ceiling is not None and abs(offset) > ceiling:
        rec["reason"] = ("shift beyond the measurable range (+/-%d): read %+d, which is UNDER-READ "
                         "rather than located" % (ceiling, offset))
        return rec

    rec["status"] = "read"
    rec["found"] = True
    rec["offset"] = offset
    return rec


def _edge_reading(rec):
    """One edge stated as a READING - a number and a frame, with no claim attached.

    This is what a BBOX-ONLY edge produces (079-07). Every quantity the verdict path would have
    used is printed; what is withheld is the CONCLUSION, because nothing in a bounding box says
    which of the things inside it changed.
    """
    if rec["status"] == "refused":
        return "%s: not searchable (%s)" % (rec["kind"], rec["reason"])
    if not rec["found"]:
        return ("%s: no above-threshold change in %d..%d (tau=%s, %d pair(s) scanned)"
                % (rec["kind"], rec["lo"], rec["hi"],
                   ("%.4f" % rec["tau"]) if rec["tau"] is not None else "n/a", rec["scanned"]))
    dom = "n/a" if rec["ratio"] is None else ("%.2fx" % rec["ratio"])
    label = "label start" if rec["kind"] == "onset" else "label end+1"
    return ("%s: change at frame %d (d=%.4f, tau=%.4f, dominance=%s) vs %s %d (delta=%+d)"
            % (rec["kind"], rec["best_k"], rec["best_d"], rec["tau"], dom, label,
               rec["nominal"], rec["offset"]))


def _edge_line(rec):
    if rec["status"] == "refused":
        tag = V_READING if not rec["attributed"] else V_NOTMEAS
        return "        %-5s  %s(%s)" % (rec["kind"], tag, rec["reason"])
    dom = "n/a" if rec["ratio"] is None else ("%.1fx" % rec["ratio"])
    if rec["found"]:
        verdict = "ALIGNED" if rec["offset"] == 0 else ("SHIFT(%+d)" % rec["offset"])
        where = "edge at %d" % rec["best_k"]
    else:
        verdict = "no change above tau"
        where = "0 of %d frame(s) above tau" % rec["scanned"]
    if not rec["attributed"]:
        verdict = "%s  <- READING ONLY (no mask)" % verdict
    return ("        %-5s  k=%-4d d=%-7s tau=%.4f m_edge=%.6f base=%-3d dom=%-6s %-26s %s/%dpx  %s"
            % (rec["kind"], rec["nominal"],
               ("%.4f" % rec["d_nominal"]) if rec["d_nominal"] is not None else "n/a",
               rec["tau"], rec["m_edge"], rec["base_n"], dom, where,
               rec["region_source"], rec["region_npix"], verdict))


def _run_verdict(edges, only_tag):
    """Combine one run's two edges. EVERY edge is reported; none is absorbed by the other.

    079-06 / Codex finding 2. Before this, a run collapsed into one token and an event collapsed
    into one token again, so a WRONG end edge could sit behind a correct onset shift and an
    entirely unread run could sit behind another run's expected token. Measured on A2L_LEGA: at
    delta -1 two of eight runs returned offsets of -2 and -4 where +1 was correct, and the event
    still printed ONSET-SHIFT(+1) from its other run.

    NOT-VISIBLE requires BOTH edges to be judgeable and BOTH to find nothing (V4), and since
    079-07 "judgeable" includes having actually observed every frame pair of the window. A
    region the instrument could not read is an UNREAD SURFACE and must not produce an absence
    claim, and neither must a window it never looked at.

    079-07: A RUN IN WHICH ANY EDGE LACKS A MASK PRODUCES NO VERDICT AT ALL. The mode is decided
    per RUN rather than per edge because the ordering constraints run BETWEEN the edges - an
    onset's detected frame bounds its own end, and a run's detected clear bounds the next run's
    onset. Letting an unattributed onset bound an attributed end would import exactly the
    ambiguity the rule exists to remove.
    """
    if any(not e["attributed"] for e in edges):
        return V_READING, (" | ".join(_edge_reading(e) for e in edges) + " | masks=none")
    read = [e for e in edges if e["status"] == "read"]
    shifted = [e for e in read if e["found"] and e["offset"] != 0]
    aligned = [e for e in read if e["found"] and e["offset"] == 0]
    if len(read) == len(edges) and not shifted and not aligned:
        return V_NOTVIS, ("no change above tau at either edge of a judgeable region%s" % only_tag)
    if shifted:
        e = shifted[0]
        kind = "ONSET" if e["kind"] == "onset" else "END"
        return ("%s-SHIFT(%+d)" % (kind, e["offset"]),
                "pixels change at %d, the label puts that edge at %d" % (e["best_k"], e["nominal"]))
    if aligned and len(aligned) == len(edges):
        return V_PASS, "both edges read and aligned"
    if aligned:
        return V_PARTIAL, ("%d edge(s) read and aligned, %d NOT read"
                           % (len(aligned), len(edges) - len(aligned)))
    return V_NOTMEAS, "neither edge could be read"


def _is_shift(v):
    return v.startswith("ONSET-SHIFT") or v.startswith("END-SHIFT")


def _event_token(runs):
    """The event's token, from COVERAGE over ALL its edges (079-07 / Codex F7).

    `runs` is a list of (verdict, [edge, ...]) - the RUN is the unit that carries mask mode, so an
    edge counts toward coverage only when ITS OWN RUN is attributed. Counting attributed edges
    individually would let a half-masked run contribute a PASS the run itself refused to give.

    AMENDMENT 4's V5 took the WORST run token by a fixed severity order, and Codex measured what
    that costs: one run fully read and aligned beside one refused run printed NOT-MEASURABLE for
    the event and UNREAD for the session, while the detail lines correctly said `edges=2/4-read`
    and printed the first run's PASS. That contradicts both PARTIAL's own definition and the
    client README's statement that UNREAD means nothing could be read. AMENDMENT 5 corrects the
    design rule as well as the implementation.

    A SHIFT still wins outright, and that is deliberate rather than an exception: a shift is a
    POSITIVE detection on an edge that was attributed, fully observed and dominant, so it stands
    on its own evidence. NOT-VISIBLE and PASS are claims about the WHOLE event and therefore
    require every edge of it to have been read.
    """
    verdicts = [v for v, _e in runs]
    for v in verdicts:
        if _is_shift(v):
            return v
    total = sum(len(e) for _v, e in runs)
    read = [x for v, edges in runs if v != V_READING
            for x in edges if x["status"] == "read"]
    if all(v == V_READING for v in verdicts):
        return V_READING
    if not read:
        return V_NOTMEAS
    if len(read) == total:
        return V_NOTVIS if V_NOTVIS in verdicts else V_PASS
    return V_PARTIAL


def label_pixel_gate(cap_dir, thresh, edge_w, min_visible_px, quiet=False, region_cap=None,
                     motion_cap=None, out_detail=None):
    """The label-vs-pixel gate. Returns (exit_code, lines).

    Per event, per contiguous run of labelled frames, per EDGE, on the raw region statistic
    `d(k) = the fraction of that edge's REGION whose pixels differ from frame k-1 by > threshold`:
      ONSET is aligned when the dominant change in the onset window sits on `start`.
      END   is aligned when the dominant change in the end window sits on `end + 1`.
    A shift is reported SIGNED: n < 0 means the PIXELS changed BEFORE the label said so.
    Nothing is inferred about WHY; the tool reports the reading.

    A VERDICT REQUIRES TARGET ATTRIBUTION, AND ONLY A PER-FRAME MASK SUPPLIES IT (079-07). Where
    the delivered mask gives the target's silhouette, the questions above are answerable and the
    full verdict vocabulary applies. Where the only region available is the supplied BOUNDING
    BOX, every number is still measured and PRINTED - as a READING - and no verdict is issued,
    because a change inside a box could be the target, the floor behind it, or the camera moving.
    The session then ends UNREAD-BBOX-ONLY at exit 0: readings for a human, never a claim.

    VALIDITY ENVELOPE - what this gate can and cannot judge, stated rather than assumed. Every
    refusal below is NOT-MEASURABLE with its numbers printed, and NEVER NOT-VISIBLE: a region the
    instrument cannot read is an UNREAD SURFACE, not a failed label.

      AN EDGE MUST HAVE BEEN OBSERVED BEFORE IT CAN BE JUDGED. Every frame pair of the edge's
      FINAL search window must be on disk and differenceable; a missing one reads
      NOT-MEASURABLE(frames missing in window). The window is narrowed deliberately - by the
      run's own extent, by the ordering bounds, by the other-event exclusion and by the session's
      ends - and that narrowing is the window's definition, not a gap in it.

      MEASURABILITY IS DECIDED PER EDGE, LOCAL IN TIME AND IN REGION (079-06). `m_edge` is the
      MEDIAN of that edge's own region's changed-pixel fraction over the clean frames NEAREST
      that edge; above REGION_CAP the edge is refused. It is an IMAGE-CHANGE PROXY, not a camera
      displacement, and it is measured where the verdict will be made. A session-level whole-frame
      median is printed as a READING ONLY and gates nothing - taken over an opening it can be
      zero while the labelled window moves, and taken over the frame it can be tiny while a small
      region changes completely (both reproduced, see _measure_edge).

      AN EDGE MUST DOMINATE ITS RUNNER-UP by DOMINANCE (1.5x) among the frames that cleared tau,
      or the argmax is a coin flip and the answer is a refusal rather than a frame number.

      A REGION COVERING THE PICTURE is refused on its size alone (>= MAX_REGION_FRAC of the
      frame, measured on the region's BOUNDING RECTANGLE). A label that claims most of the
      picture cannot be localised against the picture.

      A THRESHOLD NO MEASUREMENT COULD CLEAR is an instrument fault, not a verdict. `d` is a
      fraction, so it can never exceed ATTAINABLE_MAX = 1.0; if tau reaches that, the edge reads
      NOT-MEASURABLE(threshold unsatisfiable). ASSERTED, never clamped - clamping turns an
      impossible test into an absurd one that still FAILs, silently. This is the exact state that
      produced six confident FAILs on field data (G259).

      A DENSE BURST SCHEDULE starves the baseline. Each EDGE is calibrated on the clean frames
      NEAREST ITSELF (up to BASELINE_MAX_FRAMES); below MIN_BASELINE_FRAMES the edge reads
      NOT-MEASURABLE with the count printed.

    EVERY RUN AND EVERY EDGE IS PRINTED. An event whose edges disagree about whether they could
    be read is PARTIAL, not PASS - one token per event hid real failures before 079-06 (G262) -
    and since 079-07 the event's token comes from COVERAGE over all its edges rather than from
    the worst run, so a fully read run beside an unread one reads PARTIAL and not UNREAD.

    Constants: K_SIGMA / SIGNAL_FLOOR come from measure_label_offset, where they threshold a
    per-frame region difference - the same shape as `d`. REGION_CAP is a PROVISIONAL HEURISTIC set
    by the 079-06 cohort procedure (see the module header), not by taste.

    `motion_cap` is accepted as a deprecated alias for `region_cap` so that callers written
    against the 079-03/079-04 signature keep working; passing 1.0 disables the guard as before.
    `out_detail`, when a list is passed, receives one dict per EDGE - the structured form of what
    the detail lines print, so an audit does not have to parse text.
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

    raw_cap = region_cap if region_cap is not None else motion_cap
    if raw_cap is None:
        raw_cap = REGION_CAP
    cap, refuse_all = _normalise_cap(raw_cap)

    whole = [hot.whole_frame(k) for k in base_idx]
    whole = [v for v in whole if v is not None]
    motion = mlo.median_or_none(whole)

    lines.append("LABEL-PIXEL GATE   (m49 step 1; per-edge validity since 079-06)")
    lines.append("  session                  %s" % cap_dir)
    lines.append("  frames / labels / events %d / %d / %d" % (len(paths), len(rows), len(events)))
    lines.append("  region mode              %s" % ("masks" if has_masks else "bbox-only"))
    lines.append("  VERDICTS ARE GIVEN       only where a PER-FRAME MASK identifies the target's "
                 "pixels (M3 datasets and bench captures). Without masks this tool prints")
    lines.append("                           READINGS for human review and NEVER a verdict - a "
                 "change inside a supplied box cannot be attributed to the target.")
    lines.append("  signal                   d = fraction of REGION pixels changed since the "
                 "previous frame (bounded 0..1)")
    lines.append("  judgeability             m_edge = median d over the %d clean pair(s) NEAREST "
                 "each edge, on that edge's own region" % BASELINE_MAX_FRAMES)
    if refuse_all:
        lines.append("  regional change cap      *** %s - no positive cap is defensible from the "
                     "calibration cohort, so EVERY edge is refused ***" % NO_ADMISSIBLE_ENVELOPE)
    elif cap is None:
        lines.append("  regional change cap      *** NONE - the judgeability guard is INERT and "
                     "every edge will be judged ***")
    else:
        lines.append("  regional change cap      %.6f   (an edge refuses when m_edge > this; a cap "
                     "of 0.0 therefore STILL JUDGES a perfectly still" % cap)
        lines.append("                           region and is NOT refuse-all - that state is "
                     "'%s' and is printed as such)" % NO_ADMISSIBLE_ENVELOPE)
        lines.append("                           %s" % REGION_CAP_PROVENANCE)
    lines.append("  whole-frame change M_med %s   READING ONLY over all %d clean pair(s); it "
                 "gates nothing (079-06)"
                 % ("%.4f" % motion if motion is not None else "n/a", len(whole)))
    lines.append("  diff threshold           >%d/255 per pixel" % thresh)
    lines.append("  edge search window       +/-%d frames" % edge_w)
    lines.append("  constants                K_SIGMA=%.1f  SIGNAL_FLOOR=%.4f  baseline %d..%d "
                 "frames  region<%.0f%% of frame  attainable<=%.1f  dominance>=%.2fx"
                 % (mlo.K_SIGMA, mlo.SIGNAL_FLOOR, MIN_BASELINE_FRAMES, BASELINE_MAX_FRAMES,
                    MAX_REGION_FRAC * 100.0, ATTAINABLE_MAX, DOMINANCE))
    lines.append("  bbox=.. obs=..           PRODUCER METADATA copied off the labels; this checker "
                 "measures neither of them")
    if ceiling is None:
        lines.append("  MEASURABLE RANGE         n/a (no annotated window)")
    else:
        lines.append("  MEASURABLE RANGE         +/-%d frames (min clean gap %d) - a shift beyond "
                     "this is UNDER-READ, not absent" % (ceiling, min_gap))

    ev_lines = []
    n_pass = n_shift = n_notvis = n_notmeas = n_partial = n_reading = 0
    m_seen = []

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
            ev_lines.append("%s %s (the delivered mask carries %d px < %d on frame %d)  %s"
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

        details = []
        run_results = []
        ev_edges = []
        ev_runs = _runs_of(ev["indices"])
        prev_clear = None
        for run_i, (rs, re_) in enumerate(ev_runs):
            prev_end = ev_runs[run_i - 1][1] if run_i > 0 else None
            next_start = ev_runs[run_i + 1][0] if run_i + 1 < len(ev_runs) else None

            r_on = rows.get(rs)
            e_on = mlo.match_label_entry(r_on, ev["node"], None) if r_on else None
            reg_on = _build_region(cap_dir, r_on, e_on, frame_w, frame_h, mlo) if e_on else None
            on_lo = max(min(all_idx) + 1, rs - edge_w)
            if prev_end is not None:
                on_lo = max(on_lo, (prev_clear + 1) if prev_clear is not None else (prev_end + 2))
            on_hi = min(rs + edge_w, re_)
            onset = _measure_edge("onset", rs, reg_on, region, frame_w, frame_h, mlo, hot,
                                  base_idx, sigfn, paths, foreign, on_lo, on_hi, cap, ceiling,
                                  False, refuse_all)

            r_end = rows.get(re_)
            e_end = mlo.match_label_entry(r_end, ev["node"], None) if r_end else None
            reg_end = _build_region(cap_dir, r_end, e_end, frame_w, frame_h, mlo) if e_end else None
            truncated = (re_ + 1) > max(all_idx)
            end_lo = max(min(all_idx) + 1, re_ + 1 - edge_w)
            end_lo = max(end_lo, (onset["best_k"] + 1) if onset["best_k"] is not None else (rs + 1))
            end_hi = min(re_ + 1 + edge_w, max(all_idx))
            if next_start is not None:
                end_hi = min(end_hi, next_start - 1)
            end = _measure_edge("end", re_ + 1, reg_end, region, frame_w, frame_h, mlo, hot,
                                base_idx, sigfn, paths, foreign, end_lo, end_hi, cap, ceiling,
                                truncated, refuse_all)
            prev_clear = end["best_k"]

            for e in (onset, end):
                ev_edges.append(e)
                if e["m_edge"] is not None:
                    m_seen.append(e["m_edge"])
                if out_detail is not None:
                    rec = dict(e)
                    rec.update(event=ev["i"], run=(rs, re_), node=ev["node"], type=ev["type"],
                               session=cap_dir)
                    out_detail.append(rec)

            verdict, why = _run_verdict([onset, end], only_tag)
            run_results.append((verdict, [onset, end]))
            details.append("      run[%d..%d]  %-16s - %s" % (rs, re_, verdict, why))
            details.append(_edge_line(onset))
            details.append(_edge_line(end))

        worst = _event_token(run_results)

        taus = [e["tau"] for e in ev_edges if e["tau"] is not None]
        ms = [e["m_edge"] for e in ev_edges if e["m_edge"] is not None]
        n_read = sum(1 for v, edges in run_results if v != V_READING
                     for e in edges if e["status"] == "read")
        contaminated = max([0] + [e["contaminated"] for e in ev_edges])
        conf = "HIGH"
        if contaminated:
            conf = "LOW"
        elif bbox_only:
            conf = "MED"

        extra = ""
        if ev["type"] in mlo.TEXTURE_TYPES:
            try:
                patch = cache.rgb(anchor).crop(region["box"])
                cls, _detail = mlo.classify_patch(patch)
                extra = "  appearance=%s" % cls
            except Exception:
                extra = "  appearance=n/a"

        tau_txt = ("tau=%.4f" % taus[0]) if len(set(taus)) == 1 else (
            ("tau=%.4f..%.4f" % (min(taus), max(taus))) if taus else "tau=n/a")
        m_txt = ("m_edge<=%.6f" % max(ms)) if ms else "m_edge=n/a"
        suffix = only_tag if worst in (V_NOTVIS, V_READING) else ""
        ev_lines.append("%s %-16s [%s] %s %s edges=%d/%d-read  %s%s%s"
                        % (tag, worst + suffix, conf,
                           tau_txt, m_txt, n_read, len(ev_edges), _provenance(ev),
                           ("  CONTAMINATED=%d" % contaminated) if contaminated else "", extra))
        if worst == V_READING:
            ev_lines.append("      no per-frame mask identifies the target's pixels, so no "
                            "transition here can be attributed to it - readings only, no verdict")
        if not quiet:
            ev_lines.extend(details)

        if worst == V_PASS:
            n_pass += 1
        elif worst == V_NOTVIS:
            n_notvis += 1
        elif worst == V_NOTMEAS:
            n_notmeas += 1
        elif worst == V_PARTIAL:
            n_partial += 1
        elif worst == V_READING:
            n_reading += 1
        else:
            n_shift += 1

    lines.append("  clean-frame pool         %d (guard %d frame(s) either side of every window%s); "
                 "each EDGE calibrates on the %d NEAREST of them, on its own region"
                 % (len(base_idx), guard_used,
                    "" if guard_used == BASELINE_GUARD_FRAMES
                    else "; RELAXED from %d - a dense burst schedule left too few clean frames"
                         % BASELINE_GUARD_FRAMES,
                    BASELINE_MAX_FRAMES))
    lines.append("-" * 78)
    lines.extend(ev_lines)
    lines.append("-" * 78)
    if m_seen:
        lines.append("  regional change m_edge   min %.6f  median %.6f  max %.6f  over %d edge(s) "
                     "that reached a baseline"
                     % (min(m_seen), mlo.median_or_none(m_seen), max(m_seen), len(m_seen)))
    lines.append("  PASS %d   SHIFT %d   NOT-VISIBLE %d   PARTIAL %d   NOT-MEASURABLE %d   "
                 "READINGS %d   (of %d event(s))"
                 % (n_pass, n_shift, n_notvis, n_partial, n_notmeas, n_reading, len(events)))
    bad = n_shift + n_notvis
    total = len(events)
    if bad:
        verdict_line = "FAIL  - labels not confirmed by pixels on %d event(s)" % bad
    elif total and n_pass == total:
        verdict_line = "PASS  (%d of %d events fully checked)" % (n_pass, total)
    elif total and n_reading == total:
        verdict_line = ("UNREAD-BBOX-ONLY  (%d events; readings printed - bbox-only sessions "
                        "cannot be verified against pixels, a reviewer must look at the named "
                        "frames)" % total)
    elif n_pass == 0 and n_partial == 0:
        verdict_line = ("UNREAD  (0 of %d events checked - every event is an unread surface%s, "
                        "which is neither a pass nor a failure)"
                        % (total, (", %d of them bbox-only readings" % n_reading)
                           if n_reading else ""))
    else:
        verdict_line = ("PASS-PARTIAL  (%d fully, %d partly, %d unread%s of %d events - no "
                        "disagreement among the edges that could be read)"
                        % (n_pass, n_partial, n_notmeas + n_reading,
                           (" incl. %d bbox-only readings" % n_reading) if n_reading else "",
                           total))
    lines.append("  VERDICT                  %s" % verdict_line)
    if n_notmeas or n_partial:
        lines.append("  NOT-MEASURABLE is NOT a pass and NOT a failure - it is an unread surface, "
                     "and the reason is printed on the edge's own line.")
    if n_reading:
        lines.append("  READING is NOT a verdict - the labels were measured against a supplied "
                     "BOX, which cannot say WHICH thing inside it changed. Look at the frames "
                     "named on those lines.")
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

    079-07: `with_mask` writes the mask as the LABEL'S OWN RECTANGLE, not the true target
    rectangle. That matters because `blank_region` and `fullframe_region` exist to make the label
    LIE about where the target is; a mask drawn on the true target would quietly correct the lie
    and the two cases would stop testing anything. Masks are now written for almost every case,
    because under 079-07 a bbox-only run yields readings rather than verdicts, and a suite of
    readings cannot prove that the gate is still able to FAIL (G96).
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

    lo, hi = true_start + shift, true_end + shift + end_shift
    idxs = list(range(lo, hi + 1))
    if fullframe_region:
        label_box = [0, 0, w, h]
    elif blank_region:
        label_box = [5, 5, 8, 8]
    else:
        label_box = [box[0], box[1], box[2] - box[0], box[3] - box[1]]
    mask_rect = (label_box[0], label_box[1],
                 label_box[0] + label_box[2] - 1, label_box[1] + label_box[3] - 1)

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
            ImageDraw.Draw(mk).rectangle(list(mask_rect), fill=222)
            mk.save(os.path.join(d, "target_mask", "frame_%05d.png" % i))

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


def _codex07_fixture(root, name, runs=((60, 67),), label_runs=None, burst_at=None, onset_delta=0,
                     full_boxes=(), total=100, patch_sides=None, mask=None, mask_runs=None,
                     full_masks=(), drop_frames=()):
    """The 079-07 review's counterexample fixtures, ported with attribution.

    SOURCE: `_reviews/079-07-review-validation.py`, Codex's second independent merge review,
    function `fixture`. Kept VERBATIM IN CONSTRUCTION - a 160x120 picture that is static except
    for a 50x50 window onto a scrolling crop at (40,30), a white patch drawn over part of it on
    the frames named by `runs`, and `burst_at` scrolling that window by 10 px ONCE and then
    leaving it still. A paraphrase of a counterexample is not a counterexample.

    Parameters that are Codex's:
      runs         the RGB truth - which frames actually carry the patch
      label_runs   what the labels claim, when it differs from the truth
      burst_at     the single 10 px background scroll, the thing that hijacked the argmax
      onset_delta  moves the first labelled run's onset only
      full_boxes   frames whose labelled bbox is the WHOLE frame (forces a size refusal)
      patch_sides  per-run patch side length, for a weaker second run or a sub-tau one

    Parameters added here, all of them about 079-07's mask-mode rule:
      mask         None = bbox-only; "patch" = the mask is the target patch (Codex's F1 case,
                   where the burst is OUTSIDE the silhouette and the real onset is inside it);
                   "region" = the mask is the whole 50x50 object, for the detection-limit case
                   where the target IS the region and only a pixel of it changes
      mask_runs    frames that carry a mask at all, so a session can be mask-mode on one run and
                   bbox-only on another (the mixed case R3 names)
      full_masks   frames whose MASK is the whole frame - an ATTRIBUTED region that is still
                   refused on size, which is how a masked PARTIAL case is built
      drop_frames  frames to leave off disk, for the missing-observation case (Codex's F3)
    """
    from PIL import Image, ImageDraw
    p = os.path.join(root, name)
    os.makedirs(os.path.join(p, "Actual_Frames"))
    if mask:
        os.makedirs(os.path.join(p, "target_mask"))
    w, h = 160, 120
    x, y, rw, rh = 40, 30, 50, 50
    bg = _synth_backdrop(w, h, total, 10, 16, 0)
    lr = [tuple(r) for r in (label_runs if label_runs is not None else runs)]
    if onset_delta:
        lr[0] = (lr[0][0] + onset_delta, lr[0][1])
    idxs = [k for a, b in lr for k in range(a, b + 1)]
    rows = []
    for k in range(total):
        offset = 10 if burst_at is not None and k >= burst_at else 0
        im = Image.new("RGB", (w, h), (90, 90, 90))
        im.paste(bg.crop((offset, 0, offset + rw, rh)).convert("RGB"), (x, y))
        patch = None
        for n, (a, b) in enumerate(runs):
            if a <= k <= b:
                side = patch_sides[n] if patch_sides else 20
                patch = (x + 5, y + 5, x + 4 + side, y + 4 + side)
                ImageDraw.Draw(im).rectangle(patch, fill=(255, 255, 255))
        if k not in drop_frames:
            im.save(os.path.join(p, "Actual_Frames", "frame_%05d.png" % k))
        wants_mask = mask and (mask_runs is None or any(a <= k <= b for a, b in mask_runs))
        if wants_mask:
            side0 = patch_sides[0] if patch_sides else 20
            rect = ((0, 0, w - 1, h - 1) if k in full_masks
                    else ((x, y, x + rw - 1, y + rh - 1) if mask == "region"
                          else (x + 5, y + 5, x + 4 + side0, y + 4 + side0)))
            mk = Image.new("L", (w, h), 0)
            ImageDraw.Draw(mk).rectangle(list(rect), fill=222)
            mk.save(os.path.join(p, "target_mask", "frame_%05d.png" % k))
        box = [0, 0, w, h] if k in full_boxes else [x, y, rw, rh]
        entry = {"id": "missing_texture", "target_name": "SynthTarget",
                 "start_frame": 1000 + min(idxs), "bbox_valid": True, "bbox_px": box}
        if wants_mask:
            entry["mask_value"] = 222
        row = {"frame_index": 1000 + k, "session_index": k,
               "image": "Actual_Frames/frame_%05d.png" % k, "width": w, "height": h,
               "anomaly_present": k in idxs, "visible_positive": k in idxs,
               "anomalies": [entry] if k in idxs else []}
        if wants_mask:
            row["mask_file"] = "target_mask/frame_%05d.png" % k
            row["mask_state"] = "present"
        rows.append(row)
    with open(os.path.join(p, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    ann = {"anomalies": [{"anomaly_type": "missing_texture", "manifested": True,
                          "affected_frames": {"start_frame": min(idxs), "end_frame": max(idxs),
                                              "frame_count": len(idxs), "frame_indices": idxs},
                          "affected_objects": {"primary_index": 0,
                                               "nodes": [{"name": "SynthTarget"}]}}]}
    with open(os.path.join(p, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    return p


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


def _codex_fixture(root, name, local_only=False, alternating=False, burst=True, shift=0,
                   with_mask=False):
    """The 079-05 review's counterexample fixtures, ported with attribution.

    SOURCE: `_reviews/079-05-review-validation.py`, Codex's independent merge review, function
    `custom_motion_fixture`. They are kept here VERBATIM IN CONSTRUCTION - same geometry, same
    scroll schedule, same patch - because they are the three cases that broke the 079-04 gate and
    a paraphrase of a counterexample is not a counterexample.

      quiet_prefix    160x120; frames 0..35 flat grey, then a textured backdrop scrolls 2 px/frame.
                      The labelled window sits entirely in the moving part, so a whole-frame median
                      taken over the OPENING reads 0.0000 for a region whose local value is 0.1250.
      small_region    320x240; the picture is static except a 50x50 window onto a scrolling crop -
                      the region is ALL of the movement, and the whole-frame fraction (0.0039) is
                      32x smaller than the regional one (0.1250).
      alternating     the same small region scrolling 1 px then 3 px, which lifts tau far enough
                      that the true edges (10% of the region at BOTH of them) fall under it - the
                      false NOT-VISIBLE, with tau satisfiable so the unsatisfiable escape cannot
                      help.

    `burst` reproduces their +10 px background jump on the frame AFTER the onset, which is what
    hijacked the argmax. `burst=False` removes it so the true edge IS the dominant change, which
    is what makes the shifted variants a fair NEGATIVE control: without them the new refusals
    could have DISABLED the verdict rather than scoping it (G96).

    `shift` lies about the label by that many frames, exactly as _synth_session does.

    `with_mask` (079-07) writes the TARGET PATCH as the per-frame mask, so the same fixture can
    be run in mask mode. It is what carries the two burst-free negative controls forward: under
    079-07 a bbox-only run produces readings only, so without a masked twin the "it can still
    fail" half of this suite would have quietly disappeared.
    """
    from PIL import Image, ImageDraw
    d = os.path.join(root, name)
    frames = os.path.join(d, "Actual_Frames")
    os.makedirs(frames)
    if with_mask:
        os.makedirs(os.path.join(d, "target_mask"))
    width, height = (320, 240) if local_only else (160, 120)
    total, onset, end = 100, 60, 67
    x, y, rw, rh = 40, 30, 50, 50
    bg = _synth_backdrop(width, height, total, 10, 16, 0)
    offset = 0
    rows = []
    lo, hi = onset + shift, end + shift
    side = 10 if alternating else 20
    patch = (x + 12, y + 12, x + 11 + side, y + 11 + side)
    for k in range(total):
        if alternating:
            offset += 1 if k % 2 == 0 else 3
        elif local_only or k >= 36:
            offset += 10 if (burst and k == onset + 1) else 2
        if local_only:
            im = Image.new("RGB", (width, height), (90, 90, 90))
            im.paste(bg.crop((offset, 0, offset + rw, rh)).convert("RGB"), (x, y))
        elif k < 36:
            im = Image.new("RGB", (width, height), (90, 90, 90))
        else:
            im = bg.crop((offset, 0, offset + width, height)).convert("RGB")
        if onset <= k <= end:
            ImageDraw.Draw(im).rectangle(patch, fill=(255, 255, 255))
        im.save(os.path.join(frames, "frame_%05d.png" % k))
        if with_mask:
            mk = Image.new("L", (width, height), 0)
            ImageDraw.Draw(mk).rectangle(patch, fill=222)
            mk.save(os.path.join(d, "target_mask", "frame_%05d.png" % k))
        entry = {"id": "missing_texture", "target_name": "SynthTarget", "start_frame": 1000 + lo,
                 "bbox_valid": True, "bbox_px": [x, y, rw, rh]}
        if with_mask:
            entry["mask_value"] = 222
        row = {"frame_index": 1000 + k, "session_index": k,
               "image": "Actual_Frames/frame_%05d.png" % k, "width": width, "height": height,
               "anomaly_present": lo <= k <= hi, "visible_positive": lo <= k <= hi,
               "anomalies": [entry] if lo <= k <= hi else []}
        if with_mask:
            row["mask_file"] = "target_mask/frame_%05d.png" % k
            row["mask_state"] = "present"
        rows.append(row)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    ann = {"anomalies": [{"anomaly_type": "missing_texture", "manifested": True,
                          "affected_frames": {"start_frame": lo, "end_frame": hi,
                                              "frame_count": hi - lo + 1,
                                              "frame_indices": list(range(lo, hi + 1))},
                          "affected_objects": {"primary_index": 0, "nodes": [{"name": "SynthTarget"}]}}]}
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    return d


def _label_pixel_selftest(thresh, edge_w, min_visible_px, source_dir=None, region_cap=None):
    """Prove the gate can FAIL, in BOTH directions and on BOTH edges (G96/G142).

    Without --dir it builds synthetic sessions, so the check is portable and a client can
    run it with nothing but this file, measure_label_offset.py and Pillow. With --dir it
    additionally shifts a REAL session's labels on a copy - the frames and the source
    session are never written to.

    EACH CASE DECLARES ITS OWN JUDGEABILITY CAP, and the reason is that two different things are
    being tested. The scrolling cases exist to exercise the STATISTIC under motion, so the guard is
    disabled for them - otherwise the synthetic backdrop's own scroll rate (pan/blocks, e.g. 0.125
    at pan 2) sits far above REGION_CAP and every one of them would short-circuit to
    NOT-MEASURABLE, testing nothing. Cases that run at the SHIPPED cap prove the guard FIRES.
    Stated here rather than left to be discovered, because a suite that silently refuses its own
    cases is the vacuous-pass shape (G146).

    THE TWO HALVES THAT MUST BOTH HOLD:
      CAN REFUSE  the three 079-05 counterexample fixtures carry ALIGNED labels and must never
                  read SHIFT or NOT-VISIBLE at the shipped cap; the two hijacked shifted ones must
                  never read PASS.
      CAN STILL FAIL  the burst-free shifted variants must still report the shift under the same
                  regional motion with the guard off. Without those, refusing everything would
                  pass the suite.
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
        SHIPPED = None
        NOENV = NO_ADMISSIBLE_ENVELOPE
        M = dict(with_mask=True)
        cases = [
            ("clean", dict(shift=0, **M), (V_PASS,), OFF),
            ("clean_masked", dict(shift=0, with_mask=True), (V_PASS,), OFF),
            ("label_late_1", dict(shift=1, **M), ("ONSET-SHIFT(-1)",), OFF),
            ("label_early_1", dict(shift=-1, **M), ("ONSET-SHIFT(+1)",), OFF),
            ("end_late_1", dict(end_shift=1, **M), ("END-SHIFT(-1)",), OFF),
            ("end_early_1", dict(end_shift=-1, **M), ("END-SHIFT(+1)",), OFF),
            ("blank_region", dict(shift=0, blank_region=True, **M), (V_NOTVIS,), OFF),

            ("moving_clean", dict(shift=0, pan=2, **M), (V_PASS,), OFF),
            ("moving_fast", dict(shift=0, pan=8, **M), (V_PASS,), OFF),
            ("moving_label_late_1", dict(shift=1, pan=2, **M), ("ONSET-SHIFT(-1)",), OFF),
            ("moving_label_early_1", dict(shift=-1, pan=2, **M), ("ONSET-SHIFT(+1)",), OFF),
            ("moving_end_late_1", dict(end_shift=1, pan=2, **M), ("END-SHIFT(-1)",), OFF),
            ("moving_end_early_1", dict(end_shift=-1, pan=2, **M), ("END-SHIFT(+1)",), OFF),
            ("moving_blank_region", dict(shift=0, pan=2, blank_region=True, **M),
             (V_NOTVIS,), OFF),
            ("moving_fullframe_region", dict(shift=0, pan=2, fullframe_region=True, **M),
             (V_NOTMEAS,), OFF),
            ("moving_unsat", dict(shift=0, pan=4, stripes=4, **M), (V_NOTMEAS,), OFF),
            ("moving_over_cap", dict(shift=0, pan=2, **M), (V_NOTMEAS,), SHIPPED),

            ("bboxonly_clean", dict(shift=0), (V_READING,), OFF),
            ("bboxonly_label_late_1", dict(shift=1), (V_READING,), OFF),
        ]
        codex = [
            ("codex_quiet_prefix", dict(), (V_READING,), SHIPPED),
            ("codex_small_region", dict(local_only=True), (V_READING,), SHIPPED),
            ("codex_alternating", dict(local_only=True, alternating=True), (V_READING,), SHIPPED),
            ("codex_quiet_prefix_shift1", dict(shift=1), (V_READING,), SHIPPED),
            ("codex_small_region_shift1", dict(local_only=True, shift=1), (V_READING,), SHIPPED),
            ("codex_quiet_prefix_noburst_shift1", dict(shift=1, burst=False), (V_READING,), OFF),
            ("codex_small_region_noburst_shift1", dict(local_only=True, shift=1, burst=False),
             (V_READING,), OFF),
            ("codex_quiet_prefix_noburst_shift1_masked",
             dict(shift=1, burst=False, with_mask=True), ("ONSET-SHIFT(-1)",), OFF),
            ("codex_small_region_noburst_shift1_masked",
             dict(local_only=True, shift=1, burst=False, with_mask=True),
             ("ONSET-SHIFT(-1)",), OFF),
        ]
        codex07 = [
            ("c07_burst_aligned", dict(burst_at=61), (V_READING,), SHIPPED),
            ("c07_burst_aligned_masked", dict(burst_at=61, mask="patch"), (V_PASS,), SHIPPED,
             (0, 0)),
            ("c07_burst_late", dict(burst_at=61, onset_delta=1), (V_READING,), SHIPPED),
            ("c07_burst_late_masked", dict(burst_at=61, onset_delta=1, mask="patch"),
             ("ONSET-SHIFT(-1)",), SHIPPED, (-1, 0)),
            ("c07_onset_early_refusal",
             dict(runs=((10, 11),), burst_at=10, full_boxes=(10,)), (V_READING,), SHIPPED),
            ("c07_onset_early_refusal_masked",
             dict(runs=((10, 11),), burst_at=10, mask="patch", full_masks=(10,)),
             (V_PASS, V_PARTIAL, V_NOTMEAS), SHIPPED, (None, 0)),
            ("c07_previous_end_refusal",
             dict(runs=((10, 12), (15, 16)), label_runs=((10, 11), (15, 16)), full_boxes=(11,),
                  patch_sides=(40, 15)), (V_READING,), SHIPPED),
            ("c07_previous_end_refusal_masked",
             dict(runs=((10, 12), (15, 16)), label_runs=((10, 11), (15, 16)), full_masks=(11,),
                  patch_sides=(40, 15), mask="patch"), (V_PASS, V_PARTIAL, V_NOTMEAS), SHIPPED,
             (0, None, 0, 0)),
            ("c07_previous_end_control_masked",
             dict(runs=((10, 12), (15, 16)), label_runs=((10, 11), (15, 16)),
                  patch_sides=(40, 15), mask="patch"), ("END-SHIFT(+1)",), SHIPPED,
             (0, 1, 0, 0)),
            ("c07_partial_multirun_masked",
             dict(runs=((10, 11), (25, 26)), mask="patch", full_masks=(25, 26)),
             (V_PARTIAL,), SHIPPED),
            ("c07_partial_multirun_bbox",
             dict(runs=((10, 11), (25, 26)), full_boxes=(25, 26)), (V_READING,), SHIPPED),
            ("c07_mixed_runs",
             dict(runs=((10, 11), (25, 26)), mask="patch", mask_runs=((10, 12),)),
             (V_PARTIAL,), SHIPPED),
            ("c07_missing_edge_frames_masked",
             dict(mask="patch", drop_frames=tuple(range(55, 74))), (V_NOTMEAS,), SHIPPED),
            ("c07_missing_edge_frames",
             dict(drop_frames=tuple(range(55, 74))), (V_READING,), SHIPPED),
            ("c07_below_tau_masked", dict(patch_sides=(1,), mask="region"), (V_NOTVIS,), SHIPPED),
            ("c07_zero_cap_static_masked", dict(mask="patch"), (V_PASS,), 0.0),
            ("c07_zero_cap_burst_masked", dict(burst_at=61, mask="patch"), (V_PASS,), 0.0),
            ("c07_no_envelope_masked", dict(mask="patch"), (V_NOTMEAS,), NOENV),
        ]
        for case in cases + codex + codex07:
            name, kwargs, expect, cap = case[:4]
            want_edges = case[4] if len(case) > 4 else None
            if name.startswith("c07_"):
                d = _codex07_fixture(root, name, **kwargs)
            elif name.startswith("codex_"):
                d = _codex_fixture(root, name, **kwargs)
            else:
                d = _synth_session(root, name, **kwargs)
            detail = []
            code, lines = label_pixel_gate(d, thresh, edge_w, min_visible_px, quiet=True,
                                           region_cap=(region_cap if cap is None else cap),
                                           out_detail=detail)
            body = [l for l in lines if l.startswith("idx=")]
            read = body[0].split()[3] if body and len(body[0].split()) > 3 else "(none)"
            ok = any(read.startswith(e) for e in expect)
            shown = read
            if want_edges is not None:
                got = tuple(e["offset"] if e["status"] == "read" and e["found"] else None
                            for e in detail)
                if got != tuple(want_edges):
                    ok = False
                shown = "%s edges=%s" % (read, ",".join("r" if g is None else "%+d" % g
                                                        for g in got))
            checks.append((name, "|".join(expect) + ("" if want_edges is None else
                                                     "  edges=%s" % ",".join(
                                                         "r" if g is None else "%+d" % g
                                                         for g in want_edges)),
                           shown, ok, code))
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
                                               region_cap=OFF)
                body = [l for l in lines if l.startswith("idx=")]
                hits = sum(1 for l in body if expect in l)
                ok = hits > 0 and code == 2
                checks.append(("real%+d (%d event lines)" % (delta, len(body)),
                               expect, "%d event(s) read it" % hits, ok, code))
                if not ok:
                    rc = 3

        print("LABEL-PIXEL GATE SELFTEST", flush=True)
        print("  %-36s %-30s %-30s %s" % ("case", "expected", "read", "exit"), flush=True)
        for name, expect, read, ok, code in checks:
            print("  %-36s %-30s %-30s %s   %s"
                  % (name, expect, read, code, "OK" if ok else "*** BROKEN ***"), flush=True)
        if rc == 0:
            print("SELFTEST: OK - the gate passes an aligned session, reads a +/-1 label shift "
                  "back with the opposite sign, and calls a region with no change NOT-VISIBLE. "
                  "Its PASS is a reading, not blindness.", flush=True)
            print("           BOTH HALVES: it does that on a STILL camera and on a MOVING one, it "
                  "still FAILS a wrongly-placed label under motion, and it refuses - as "
                  "NOT-MEASURABLE, never as NOT-VISIBLE - when the region covers the picture, when "
                  "no measurement could clear the threshold, or when the region is changing on its "
                  "own.", flush=True)
            print("           AND IT SURVIVES BOTH SETS OF COUNTEREXAMPLES: the 079-05 fixtures "
                  "whose ALIGNED labels the older design called SHIFT or NOT-VISIBLE, and the "
                  "079-07 burst fixture where an unrelated background scroll INSIDE the labelled "
                  "window beat the real onset 3.1x. The second set is answered by SCOPE, not by "
                  "a threshold: a bbox-only run yields READINGS and never a verdict, while the "
                  "same fixture with a mask reads PASS on correct labels and ONSET-SHIFT(-1) on "
                  "a late one.", flush=True)
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


def _batch_key(root, sess):
    """A batch identifier that is UNIQUE, because a folder basename is not.

    079-05 finding 4. Both the summary identifier and the output filename used only the last path
    component, opened with "w". Two sessions called `session_same` under different parents both
    RAN, both printed a line, and only ONE report survived - the summary listed the same name
    twice with different verdicts and no way to tell which path each belonged to. A read-only
    inventory of the bench bank found 1,409 session-shaped directories in 509 colliding basename
    groups, so this is the normal case here, not a corner one.

    079-07 finding 8. The 079-06 recipe - the relative path with separators replaced by "__" -
    is NOT a unique encoding, and Codex produced the collision: `a__b/session_same` and
    `a/b__session_same` both flatten to `a__b__session_same`, the summary printed that identifier
    twice with opposite verdicts, and only the second report survived the `"w"` open. A separator
    substitution cannot be injective when the separator is a legal character in a path component.

    So the key now carries the first 8 hex of the SHA-1 OF THE RELATIVE PATH beside the flattened
    form: the flattened part stays readable, the hash makes two different paths differ. That is
    an argument about collision LIKELIHOOD, not a proof, which is why label_pixel_batch ALSO
    asserts uniqueness across the whole batch before it writes anything.

    Returns (key, rel) so the caller can print the path the identifier stands for.
    """
    import hashlib
    try:
        rel = os.path.relpath(sess, root)
    except ValueError:
        rel = sess
    rel = rel.replace("\\", "/").strip("/")
    if rel in ("", "."):
        rel = os.path.basename(root.rstrip(os.sep)) or "root"
    key = rel.replace("/", "__")
    for ch in ':*?"<>|':
        key = key.replace(ch, "_")
    digest = hashlib.sha1(rel.encode("utf-8")).hexdigest()[:8]
    return "%s__%s" % (key, digest), rel


def label_pixel_batch(root, out_dir, thresh, edge_w, min_visible_px, quiet, report_only,
                      region_cap=None):
    """--all: run the gate over every session under `root`, one line each.

    The SELFTEST runs FIRST and the batch REFUSES to start if it is not OK. A sweep of a hundred
    sessions with a broken instrument produces a hundred confident wrong lines, and the cost of
    finding that out later is the whole sweep.

    --report-only SUPPRESSES ONLY THE VERDICT-FAILURE CODE (2), exactly as single-session mode
    does. 079-05 finding 5: it used to return 0 after a session with no labels.jsonl, an
    unreadable input or an exception, so a batch that read NOTHING reported success to its caller
    while single-session mode on the same folder returned 3. CANNOT-RUN and ERROR are execution
    failures, not verdicts, and they survive --report-only.
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
    st = _label_pixel_selftest(thresh, edge_w, min_visible_px, None, region_cap)
    if st != 0:
        print("BATCH: REFUSING TO RUN - the selftest is BROKEN, so every reading below it would "
              "be unfounded. Fix the gate first.", flush=True)
        return 3

    keys = {}
    collisions = []
    for sess in sessions:
        name, rel = _batch_key(root, sess)
        if name in keys:
            collisions.append((name, keys[name], rel))
        keys[name] = rel
    if collisions:
        print("BATCH: REFUSING TO RUN - two sessions map to the same identifier, so one report "
              "would silently overwrite the other:", flush=True)
        for name, a, b in collisions:
            print("  %s  <-  %s  AND  %s" % (name, a, b), flush=True)
        return 3

    summary = []
    worst = 0
    cannot_run = 0
    for sess in sessions:
        name, rel = _batch_key(root, sess)
        if not os.path.isfile(os.path.join(sess, "labels.jsonl")):
            line = ("%-58s | CANNOT RUN | no labels.jsonl (the gate needs the per-frame bbox) | %s"
                    % (name, rel))
            summary.append(line)
            print(line, flush=True)
            worst = max(worst, 3)
            cannot_run += 1
            continue
        try:
            code, lines = label_pixel_gate(sess, thresh, edge_w, min_visible_px, quiet,
                                           region_cap=region_cap)
        except Exception as exc:
            line = "%-58s | ERROR      | %s: %s | %s" % (name, type(exc).__name__, exc, rel)
            summary.append(line)
            print(line, flush=True)
            worst = max(worst, 3)
            cannot_run += 1
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
        line = "%-58s | exit %-4d | %s | VERDICT %s | %s" % (name, code, counts or "(no events)",
                                                             verdict or "(none)", rel)
        summary.append(line)
        print(line, flush=True)
        worst = max(worst, code)

    with open(os.path.join(out_dir, "SUMMARY.txt"), "w", encoding="utf-8") as fh:
        fh.write("root: %s\n" % root)
        fh.write("sessions: %d (identifiers are the path RELATIVE to the root with separators as "
                 "__ plus 8 hex of its sha1, because basenames collide AND the flattened form "
                 "collides too; uniqueness is asserted before anything is written. The relative "
                 "path is printed as the last field of every line.)\n\n" % len(sessions))
        fh.write("\n".join(summary) + "\n")
    print("BATCH: %d session(s)%s; full per-session output in %s"
          % (len(sessions),
             ("; %d COULD NOT RUN" % cannot_run) if cannot_run else "", out_dir), flush=True)
    if report_only:
        return 3 if worst == 3 else 0
    return worst


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
    ap.add_argument("--region-cap", "--motion-cap", dest="region_cap", type=str,
                    default=str(REGION_CAP),
                    help=f"label-pixel gate: refuse an EDGE as NOT-MEASURABLE when the region it "
                         f"is judged on changes by more than this on its own nearby clean frames "
                         f"(default {REGION_CAP}, a PROVISIONAL HEURISTIC set by the 079-06 cohort "
                         f"procedure - see the module header). --motion-cap is a deprecated alias "
                         f"for the same flag; pass 1.0 to disable the guard entirely. Pass the "
                         f"literal '{NO_ADMISSIBLE_ENVELOPE}' to refuse EVERY edge - that state is "
                         f"not the same as a cap of 0.0, which still judges a perfectly still "
                         f"region.")
    ap.add_argument("--all", metavar="ROOT", default=None,
                    help="label-pixel gate: run over EVERY session folder under ROOT, one summary "
                         "line each, full output under --out. Runs --selftest first and refuses "
                         "to start if it is not OK.")
    args = ap.parse_args()
    if isinstance(args.region_cap, str):
        txt = args.region_cap.strip()
        if txt.upper() == NO_ADMISSIBLE_ENVELOPE:
            args.region_cap = NO_ADMISSIBLE_ENVELOPE
        else:
            try:
                args.region_cap = float(txt)
            except ValueError:
                sys.exit("ERROR: --region-cap takes a number or the literal '%s', not %r"
                         % (NO_ADMISSIBLE_ENVELOPE, args.region_cap))

    if args.selftest:
        if args.label_pixel_gate:
            src = os.path.abspath(args.dir) if args.dir and os.path.isdir(args.dir) else None
            sys.exit(_label_pixel_selftest(args.diff_threshold, args.edge_window,
                                           args.min_visible_px, src, args.region_cap))
        sys.exit(_selftest())

    if args.all:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        sys.exit(label_pixel_batch(os.path.abspath(args.all),
                                   os.path.abspath(args.out) if args.out else None,
                                   args.diff_threshold, args.edge_window, args.min_visible_px,
                                   args.quiet, args.report_only, args.region_cap))

    cap_dir = os.path.abspath(args.dir)

    if args.label_pixel_gate:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        code, lines = label_pixel_gate(cap_dir, args.diff_threshold, args.edge_window,
                                       args.min_visible_px, args.quiet,
                                       region_cap=args.region_cap)
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
