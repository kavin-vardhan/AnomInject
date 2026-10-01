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

--------------------------------------------------------------------------------------------------
LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)
--------------------------------------------------------------------------------------------------
Masks identify pixels, not cause. No output certifies a label or independently confirms producer
observable flags. Each pair uses the union of its two target masks. Actual masks take precedence.
When an OUT-OF-SPAN frame lacks a target mask, use this run's nearest labelled boundary mask, at
most edge_window + N_BASE frames away (N_BASE = BASELINE_MAX_FRAMES = 24; default limit 28).
Masks outside the labelled span are extrapolated from the nearest labelled frame and disclosed on
each line. A real per-frame mask is never replaced by an extrapolated one.
In-span missing masks, unreadable masks, identity errors and missing RGB remain unassessable.
Every affected observation tags its extrapolation sources/max signed distance, including baseline
use; structured detail retains every source/delta. The run reports its maximum extrapolation.
An unresolved ID can use a sole value, visibly tagged, but cannot support NO-TRACE.

Edges report TRANSITION k d=... tau=... label s delta=k-s, NO-TRANSITION [a..b], or
UNASSESSABLE(reason). Labelled bounds are inclusive. A local peak exceeds tau and is at
least 1.5 times the smaller neighbour (outside this edge's window counts as zero). Within each
target/mode group, process edges in labelled order and choose the nearest unused peak,
tie larger d, then earlier. Print tied strengths, alternatives and multi-peak run assignments. An empty peak
set gives NO-TRANSITION; an entirely consumed set gives UNASSESSABLE. Exclusion ensures
unique assignments; a decrease in assigned frames is noted on the later edge's run.
Bbox-only runs never constrain masked runs. Input/coverage refusals take precedence.

CONSISTENT means two zero-delta transitions and does not establish cause. A coincident high ring
change adds a RUN caveat asking for inspection of k-1..k+1; the summary counts caveated consistent
events. OFFSET-NOTE is a reading for human inspection, never a failure. Ring change is diagnostic.
NO-TRACE means: no change above the noise floor (tau=...) on this target across frames s..e;
the label claims a visible change. This requires coverage of the entire span AND both windows,
resolved identity and an eligible producer-class contract. Extrapolated edge pairs are permitted
and disclosed. Tau is a fraction of the target-region pixels; 1/2500 = 0.0004 is below the default
floor 0.004 (0.4%). Each pixel also must change by more than the configured RGB-channel threshold.

The producer-class contract has two tiers. Most classes are eligible outright: hiding an object or
swapping its material changes pixels wherever the object is drawn, so no change across the span
contradicts the label. A HELD-GATED class is eligible only on runs where the producer's own
per-frame flag says the condition was held on EVERY labelled frame - stuck_low_mip is the first,
gated on stuck_mip.held, because its effect is produced by the texture streamer on its own schedule
and a frame the hold had not engaged on is a frame nothing was expected to change. This narrows the
class rather than trusting it: an unheld or missing flag leaves the run UNASSESSABLE, which is the
same verdict the class had before it was listed at all.

Only NO-TRACE returns exit 2. Otherwise the session reports NO FAILURE FOUND, not label approval.
--report-only suppresses 2 but retains execution error 3. Event outcomes are worst-of the RUN
outcomes NO-TRACE > OFFSET-NOTE > PARTIAL > CONSISTENT. UNASSESSABLE/READING runs affect coverage
only; their observed edges cannot promote an event. With no assessable run, the event is
UNASSESSABLE, or READING when all runs are bbox-only. Separate run coverage remains visible.

--------------------------------------------------------------------------------------------------
m55 CHANGE ORACLE  (--change-oracle [SESSION], a SEPARATE mode; every mode above is untouched)
--------------------------------------------------------------------------------------------------
Recomputes every change_evidence.jsonl pair row with chg_measured: true from the delivered PNGs:
Actual_Frames/frame_%05d.png for session_index and prev_session_index, target_mask/frame_%05d.png
for session_index. Target = mask == mask_value, control = mask == 0, d = max over R,G,B of the byte
difference, counted when d > tau_px. Count against count exactly (n, gt8, sum, the eight bins) and
mean against mean within 0.00005, because the producer publishes sum/n/255 rounded to four
decimals. A row that names a reference (ref_session_index with ref_gt8/ref_mean) is recomputed
against that delivered frame too. empty_region refusals are cross-read against the mask PNG.
The producer's numbers are read only to be compared after the recomputation. JPEG, resampled,
backbuffer, missing or unreadable deliveries are reported UNAVAILABLE with the reason, never
guessed. A measured row must also be self-consistent: reason null, chg_eligible and pair_valid true,
expected_prev_session_index = session_index - 1 when present, and chg_hist/chg_sum/ctl_hist/ctl_sum
present; a contradiction is a mismatch. An empty_region refusal whose delivered mask holds both
target and control pixels DISAGREES and is a mismatch; a refusal with no mask PNG (none is written
for an all-zero mask) or an unreadable one is UNVERIFIABLE, never a disagreement. Input it cannot
interpret - a line that is not UTF-8 JSON, a line that is not a JSON object, a measured row whose
pair ids, mask_value or tau_px are not integers or whose receipt is not an object or whose receipt
rect is not an array, chg_measured that is not a boolean, or a row the recomputation cannot
process - is counted UNINTERPRETABLE with its line number. A tau_px outside 0..255 is a mismatch. Exit precedence: 1 when any comparison mismatches or any
empty_region refusal disagrees (a proven defect outranks incompleteness); otherwise 3 when anything
was uninterpretable or it cannot run; otherwise 0 (coverage 0 included - a gate that needs rows
fails itself on coverage 0). The summary always prints both the mismatch and the uninterpretable
counts. It never exits through a traceback. It trusts the row's own pair ids: a producer that
published consistent wrong ids AND the numbers of the frames those ids name would match. It checks
arithmetic and transport only: not renderer pairing, not which frame was chosen as a phase's
reference (another identical image would match), and not the full record schema. Its output ends
with the sentence it exists to state: agreement validates arithmetic and transport only; it does not
establish renderer pairing, visible effect, or cause.
--change-oracle --selftest proves it can agree, can disagree (five must-fail mutations, a
contradicted empty_region refusal, contradictory measured rows) and refuses what it cannot read.

Regional motion m_edge remains a reading and never refuses an edge. Values above the historical
0.42 marker add a run caveat; CONSISTENT counts either motion or lighting caveats. Unsatisfiable
tau and other coverage guards still apply. docs/verifier-characterisation.md records per-session
recovery classes and all wrong cells with competing peaks; the fixed planned denominator includes
an unscored ledger for ineligible runs. Recovery agreement does not establish label correctness.

--------------------------------------------------------------------------------------------------
LABEL-RULE READER  (--label-rule, a SEPARATE mode; also printed at the end of the overlay mode) - 084-07b
--------------------------------------------------------------------------------------------------
From build 084-07 on, every labels.jsonl anomaly entry carries `labelled` (the frame is in that event's
annotation.json frame list) and visible_positive = anomaly_present AND a labelled entry with a valid box.
Earlier builds wrote no `labelled`, and their visible_positive also counted frames on which an event was
running but its effect was not in the picture (a blinking visible phase, a lod_popping frame between pops).
The reader picks the rule from the session itself - NEW when every entry carries `labelled`, OLD when none
does, and it REFUSES a session that mixes them - and SAYS which. Under NEW it recomputes visible_positive,
counts the active-but-unlabelled rows, and cross-checks `labelled` against annotation.json both ways:
a labelled entry on a frame its event does not list (LABELLED-EXTRA; reported, not failed, when
run_summary counts vetoed events, because a vetoed event leaves annotation.json but not labels.jsonl) and a
listed frame with no labelled entry (LABELLED-MISSING). Under OLD it checks visible_positive against the old
rule and counts the rows the new rule would turn false. On both it reads the transition flags:
transition_present equals "an entry carries transition", every flagged entry names a known reason, each
reason appears only on the anomaly that produces it, partial and camera_clipping_unconfirmed only on
labelled entries, and temporal_aa / hide_return only when run_summary.label_temporal_aa is not false.
A build before 084-07 wrote `transition` with no `transition_reason`; under OLD that is counted as legacy,
not failed. Exit 0 no mismatch, 1 mismatch, 3 cannot run. --label-rule --selftest (and bare --selftest) prove it can
fail both ways on synthetic sessions.

Usage:
    python verify_capture.py --dir <sessionDir> [--out <annotatedDir>] [--quiet] [--red-only]
    python verify_capture.py --dir <sessionDir> --label-rule [--quiet]
    python verify_capture.py --label-rule --selftest
    python verify_capture.py --dir <sessionDir> --black-frame-gate [--black-threshold N]
    python verify_capture.py --dir <sessionDir> --label-pixel-gate [--report-only]
    python verify_capture.py --all <bankRoot> --label-pixel-gate --report-only [--out <dir>]
    python verify_capture.py --label-pixel-gate --selftest
    python verify_capture.py --selftest
    python verify_capture.py --change-oracle <sessionDir> [--quiet] [--oracle-json <file>]
    python verify_capture.py --change-oracle --selftest

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
    Then runs the label-rule selftest (084-07b); the result is OK only when both are.
    """
    rc = _black_frame_selftest()
    lr = _label_rule_selftest()
    return rc if rc != 0 else lr


def _black_frame_selftest():
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

PEAK_NEIGHBOUR_RATIO = 1.5

HIGH_REGIONAL_CHANGE_MARKER = 0.42

O_TRANSITION = "TRANSITION"
O_NONE = "NO-TRANSITION"
O_UNASSESSABLE = "UNASSESSABLE"
R_CONSISTENT = "CONSISTENT"
R_OFFSET = "OFFSET-NOTE"
R_NO_TRACE = "NO-TRACE"
R_PARTIAL = "PARTIAL"
R_UNASSESSABLE = "UNASSESSABLE"
R_READING = "READING"
RUN_OUTCOMES = (R_CONSISTENT, R_OFFSET, R_NO_TRACE, R_PARTIAL, R_UNASSESSABLE, R_READING)

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


class _HotCache(object):
    """hot(k) = 255 where ANY RGB channel changes by more than thresh, else 0.

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
            a = self.cache.rgb(k)
            b = self.cache.rgb(k - 1)
        except Exception:
            return None
        if a.size != b.size:
            return None
        t = self.thresh
        channels = ImageChops.difference(a, b).split()
        magnitude = ImageChops.lighter(ImageChops.lighter(channels[0], channels[1]), channels[2])
        img = magnitude.point(lambda v: 255 if v > t else 0)
        while len(self.hot) >= self.limit:
            self.hot.pop(next(iter(self.hot)))
        self.hot[k] = img
        return img

def _region_frac(hot, k, region):
    """Fraction of changed pixels in the supplied per-pair mask union or bbox."""
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


def _target_entry(row, node):
    """A sole unrelated label entry cannot supply the requested target's identity."""
    return next((e for e in (row.get("anomalies") or [])
                 if isinstance(e, dict) and node and str(e.get("target_name") or "") == node), None)


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
        e = _target_entry(r, node)
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


class _PairSignal:
    """Read actual frame pairs; masks identify pixels, never the cause of a change."""

    def __init__(self, cap_dir, rows, node, entry, mode, hot, paths, size, mlo, min_px,
                 span=None, edge_w=EDGE_WINDOW_DEFAULT):
        self.cap_dir, self.rows, self.node = cap_dir, rows, node
        self.entry, self.mode, self.hot, self.paths = entry, mode, hot, paths
        self.w, self.h = size
        self.mlo, self.min_px = mlo, min_px
        self.span = span
        self.extrapolation_limit = edge_w + BASELINE_MAX_FRAMES
        self.actual_masks = {}
        self.masks = {}
        self.pairs = {}

    def _actual_mask(self, k):
        """Return (region, error, unresolved, absent); corrupt evidence is not absence."""
        if k in self.actual_masks:
            return self.actual_masks[k]
        from PIL import Image, ImageStat
        row = self.rows.get(k) or {}
        entry = _target_entry(row, self.node) or self.entry
        try:
            wanted = int(entry.get("mask_value") or 0)
        except (ValueError, TypeError):
            wanted = 0
        mf = row.get("mask_file")
        result = (None, "mask missing at frame %d" % k, False, True)
        if mf:
            try:
                with Image.open(os.path.join(self.cap_dir, str(mf))) as src:
                    im = src.convert("L")
                if im.size != (self.w, self.h):
                    result = (None, "mask unreadable at frame %d (dimensions)" % k, False, False)
                else:
                    present = [v for v, n in enumerate(im.histogram()) if v and n]
                    if not present:
                        result = (None, "mask missing at frame %d (empty)" % k, False, True)
                    elif wanted != 0 and wanted not in present:
                        result = (None, "mask id %d not present at frame %d" % (wanted, k), False, True)
                    elif wanted == 0 and len(present) != 1:
                        result = (None, "mask id unresolved at frame %d (multiple values)" % k, False, False)
                    else:
                        value = wanted if wanted != 0 else present[0]
                        binary = im.point(lambda v: 255 if v == value else 0)
                        box = binary.getbbox()
                        crop = binary.crop(box)
                        npix = int(round(ImageStat.Stat(crop).sum[0] / 255.0))
                        reg = {"bin": crop, "box": box, "npix": npix,
                               "source": "mask(v%d)" % value}
                        result = (reg, None, wanted == 0, False)
            except FileNotFoundError:
                pass
            except (OSError, ValueError):
                result = (None, "mask unreadable at frame %d" % k, False, False)
        while len(self.actual_masks) >= 64:
            self.actual_masks.pop(next(iter(self.actual_masks)))
        self.actual_masks[k] = result
        return result

    def mask(self, k):
        if k in self.masks:
            return self.masks[k]
        reg, error, unresolved, absent = self._actual_mask(k)
        extrapolated = []
        if absent and self.span and not self.span[0] <= k <= self.span[1]:
            source = self.span[0] if k < self.span[0] else self.span[1]
            delta = k - source
            if abs(delta) > self.extrapolation_limit:
                error += "; extrapolation distance %d > limit %d" % (abs(delta), self.extrapolation_limit)
            else:
                template, template_error, template_unresolved, _absent = self._actual_mask(source)
                if template_error:
                    error += "; boundary mask unavailable: " + template_error
                else:
                    reg, error, unresolved = template, None, template_unresolved
                    extrapolated = [(source, delta)]
        result = (reg, error, unresolved, extrapolated)
        while len(self.masks) >= 64:
            self.masks.pop(next(iter(self.masks)))
        self.masks[k] = result
        return result

    def pair(self, k):
        if k in self.pairs:
            return self.pairs[k]
        from PIL import Image, ImageChops, ImageStat
        reg, error, unresolved, extrapolated = None, None, False, []
        if k not in self.paths or k - 1 not in self.paths:
            error = "RGB missing for pair %d..%d" % (k - 1, k)
        elif self.mode == "mask":
            left, le, lu, lx = self.mask(k - 1)
            right, re, ru, rx = self.mask(k)
            error, unresolved = le or re, lu or ru
            extrapolated = sorted(set(lx + rx))
            if error is None:
                box = (min(left["box"][0], right["box"][0]),
                       min(left["box"][1], right["box"][1]),
                       max(left["box"][2], right["box"][2]),
                       max(left["box"][3], right["box"][3]))
                a = Image.new("L", (box[2] - box[0], box[3] - box[1]))
                b = Image.new("L", a.size)
                a.paste(left["bin"], (left["box"][0] - box[0], left["box"][1] - box[1]))
                b.paste(right["bin"], (right["box"][0] - box[0], right["box"][1] - box[1]))
                union = ImageChops.lighter(a, b)
                reg = {"bin": union, "box": box,
                       "npix": int(round(ImageStat.Stat(union).sum[0] / 255.0)),
                       "source": "mask union(%d,%d)" % (k - 1, k)}
        else:
            row = self.rows.get(k) or self.rows.get(k - 1) or {}
            entry = self.mlo.match_label_entry(row, self.node, None) or self.entry
            box, source = self.mlo.bbox_from_label_entry(entry, self.w, self.h)
            box = self.mlo.clamp_box(box, self.w, self.h) if box else None
            if box and self.mlo.box_area(box):
                reg = {"bin": None, "box": box, "npix": self.mlo.box_area(box),
                       "source": source or "bbox"}
            else:
                error = "no usable bbox at frame %d" % k
        d = None
        if error is None:
            if self.mlo.box_area(reg["box"]) >= MAX_REGION_FRAC * self.w * self.h:
                error = "region covers the picture at frame %d" % k
            elif reg["npix"] < self.min_px:
                error = "mask too small at frame %d (%d px < %d)" % (k, reg["npix"], self.min_px)
            else:
                hot = self.hot.get(k)
                if hot is None:
                    error = "RGB unreadable for pair %d..%d" % (k - 1, k)
                elif hot.size != (self.w, self.h):
                    error = "RGB dimensions changed for pair %d..%d" % (k - 1, k)
                else:
                    d = _region_frac(self.hot, k, reg)
        result = {"d": d, "region": reg, "error": error, "unresolved": unresolved,
                  "mask_extrapolations": extrapolated}
        while len(self.pairs) >= 64:
            self.pairs.pop(next(iter(self.pairs)))
        self.pairs[k] = result
        return result

    def ring(self, k, reg):
        """Ten-pixel bounding ring, excluding the union mask. Diagnostic only."""
        from PIL import Image, ImageChops, ImageStat
        hot = self.hot.get(k)
        if hot is None or reg is None:
            return None
        x0, y0, x1, y1 = reg["box"]
        outer = (max(0, x0 - 10), max(0, y0 - 10), min(self.w, x1 + 10), min(self.h, y1 + 10))
        ring = Image.new("L", (outer[2] - outer[0], outer[3] - outer[1]), 255)
        if reg["bin"] is not None:
            ring.paste(ImageChops.invert(reg["bin"]), (x0 - outer[0], y0 - outer[1]))
        else:
            ring.paste(0, (x0 - outer[0], y0 - outer[1], x1 - outer[0], y1 - outer[1]))
        count = ImageStat.Stat(ring).sum[0] / 255.0
        if count < 1:
            return None
        return ImageStat.Stat(ImageChops.multiply(hot.crop(outer), ring)).sum[0] / (255.0 * count)


def _local_peaks(window, tau):
    """079-13 one-sided prominence; outside this observed window counts as zero."""
    values = {k: p["d"] for k, p in window}
    return [k for k, d in sorted(values.items()) if d > tau
            and d >= PEAK_NEIGHBOUR_RATIO * min(values.get(k - 1, 0.0), values.get(k + 1, 0.0))]


def _measure_edge(kind, nominal, signal, base_idx, lo, hi):
    rec = {"kind": kind, "nominal": nominal, "observation": O_UNASSESSABLE,
           "status": "unassessable", "reason": None, "tau": None, "m_edge": None,
           "mad": None, "base_n": 0, "baseline_missing": 0, "best_k": None,
           "best_d": None, "eligible": 0, "peaks": [], "available_peaks": [],
           "consumed_peaks": [], "other_peaks": [],
           "peak_values": [], "tie_peaks": [], "assignment_inversion": None,
           "scanned": 0, "found": False, "offset": None, "lo": lo, "hi": hi,
           "window": max(0, hi - lo + 1), "missing": 0, "mode": signal.mode,
           "mask_id_unresolved": False, "mask_extrapolations": [], "ring_d": None, "ring_note": False,
           "region_source": None, "region_npix": None, "d_nominal": None}
    if lo > hi:
        rec["reason"] = "empty labelled search window"
        return rec
    window = [(k, signal.pair(k)) for k in range(lo, hi + 1)]
    bad = [p["error"] for _k, p in window if p["error"]]
    rec["scanned"], rec["missing"] = len(window) - len(bad), len(bad)
    rec["mask_id_unresolved"] = any(p["unresolved"] for _k, p in window)
    rec["mask_extrapolations"] = sorted({x for _k, p in window for x in p["mask_extrapolations"]})
    if bad:
        rec["reason"] = bad[0]
        return rec
    values = []
    for k in sorted(base_idx, key=lambda k: (abs(k - nominal), k)):
        p = signal.pair(k)
        if p["error"]:
            rec["baseline_missing"] += 1
            continue
        values.append(p["d"])
        rec["mask_id_unresolved"] |= p["unresolved"]
        rec["mask_extrapolations"] = sorted(set(rec["mask_extrapolations"]) | set(p["mask_extrapolations"]))
        if len(values) == BASELINE_MAX_FRAMES:
            break
    rec["base_n"] = len(values)
    if len(values) < MIN_BASELINE_FRAMES:
        rec["reason"] = "baseline: only %d valid clean pairs, need %d (%d unavailable)" % (
            len(values), MIN_BASELINE_FRAMES, rec["baseline_missing"])
        return rec
    tau, med, mad = _threshold_from(values, signal.mlo)
    rec.update(tau=tau, m_edge=med, mad=mad)
    if tau >= ATTAINABLE_MAX:
        rec["reason"] = "threshold unsatisfiable: tau=%.4f >= %.1f" % (tau, ATTAINABLE_MAX)
        return rec
    rec["d_nominal"] = next((p["d"] for k, p in window if k == nominal), None)
    rec["peaks"] = _local_peaks(window, tau)
    rec["peak_values"] = [{"k": k, "d": p["d"]} for k, p in window if k in rec["peaks"]]
    rec["eligible"] = len(rec["peaks"])
    if not rec["peaks"]:
        rec.update(observation=O_NONE, status="observed")
    return rec


def _assign_edge(rec, signal, assigned):
    """Greedy nearest unused peak; hard refusals consume no frames (079-12)."""
    if rec["reason"] is not None:
        return
    available = [k for k in rec["peaks"] if k not in assigned]
    rec["available_peaks"] = available
    rec["consumed_peaks"] = [k for k in rec["peaks"] if k in assigned]
    if not available:
        if rec["peaks"]:
            rec.update(observation=O_UNASSESSABLE, status="unassessable",
                       reason="all peaks in window assigned to earlier edges: " +
                       ", ".join(map(str, rec["consumed_peaks"])))
        return
    values = {p["k"]: p["d"] for p in rec["peak_values"]}
    distance = min(abs(k - rec["nominal"]) for k in available)
    nearest = [k for k in available if abs(k - rec["nominal"]) == distance]
    k = min(nearest, key=lambda k: (-values[k], k))
    if len(nearest) > 1:
        rec["tie_peaks"] = [{"k": other, "d": values[other]} for other in nearest]
    p = signal.pair(k)
    assigned.add(k)
    rec["other_peaks"] = [other for other in available if other != k]
    rec.update(best_k=k, best_d=p["d"], region_source=p["region"]["source"],
               region_npix=p["region"]["npix"])
    ring = signal.ring(k, p["region"])
    rec.update(observation=O_TRANSITION, status="observed", found=True, offset=k - rec["nominal"],
               ring_d=ring, ring_note=ring is not None and ring > rec["tau"])


def _observe_windows(runs, first, last, edge_w, base_idx):
    """Label-only bounds, then ordered nearest unused peaks within each target/mode."""
    grouped = {}
    for run in runs:
        for kind, nominal in (("onset", run["start"]), ("end", run["end"] + 1)):
            grouped.setdefault((run["node"], run["mode"]), []).append((nominal, kind, run))
    for group in grouped.values():
        ordered = sorted(group, key=lambda item: (item[0], item[1] != "onset",
                                                 item[2]["event"], item[2]["ordinal"]))
        assigned = set()
        previous_selected = None
        for i, (nominal, kind, run) in enumerate(ordered):
            lo, hi = max(first + 1, nominal - edge_w), min(last, nominal + edge_w)
            if i:
                lo = max(lo, ordered[i - 1][0])
            if i + 1 < len(ordered):
                hi = min(hi, ordered[i + 1][0])
            rec = _measure_edge(kind, nominal, run["signal"], base_idx, lo, hi)
            if nominal <= first or nominal > last:
                rec.update(observation=O_UNASSESSABLE, status="unassessable", found=False,
                           offset=None, reason="edge truncated by the session boundary")
            _assign_edge(rec, run["signal"], assigned)
            if rec["found"]:
                if previous_selected is not None and rec["best_k"] < previous_selected:
                    rec["assignment_inversion"] = (previous_selected, rec["best_k"])
                previous_selected = rec["best_k"]
            rec.update(event=run["event"], ordinal=run["ordinal"], run=(run["start"], run["end"]),
                       node=run["node"], type=run["type"])
            run["edges"].append(rec)
    for run in runs:
        run["edges"].sort(key=lambda e: e["kind"] != "onset")


NO_TRACE_TYPES = frozenset(("missing_object", "blink", "blinking", "missing_texture",
                            "corrupted_texture", "lod_popping", "lod_corruption"))

HELD_GATED_TYPES = {"stuck_low_mip": "stuck_mip.held"}


def _held_gate(rows, node, a, b, key):
    """Every labelled frame of the run must carry the producer's held flag as true.

    A held-gated class earns NO-TRACE only where the engine itself says the condition was
    held on every frame of the span. One unheld frame makes "no change across the whole
    span" the expected reading rather than a contradiction, so the run is left
    UNASSESSABLE. A missing key is not held.
    """
    seen = 0
    for k in range(a, b + 1):
        entry = _target_entry(rows.get(k) or {}, node)
        if entry is None or entry.get(key) is not True:
            return False, seen
        seen += 1
    return seen > 0, seen


def _no_trace_available(run):
    if run["type"] in NO_TRACE_TYPES:
        return True, None
    key = HELD_GATED_TYPES.get(run["type"])
    if key is None:
        return False, "NO-TRACE unavailable for class %s" % run["type"]
    if run.get("held_gate_ok"):
        return True, None
    return False, ("NO-TRACE unavailable for class %s - %s is not true on every labelled frame of "
                   "frames %d..%d (%d of %d carried it), so no change across the span is the "
                   "expected reading rather than a contradiction" % (
                       run["type"], key, run["start"], run["end"],
                       run.get("held_gate_frames", 0), run["end"] - run["start"] + 1))


def _run_outcome(run):
    edges = run["edges"]
    if run["mode"] == "bbox":
        return R_READING, "bbox-only observations"
    observed = [e for e in edges if e["observation"] != O_UNASSESSABLE]
    transitions = [e for e in observed if e["observation"] == O_TRANSITION]
    if not observed:
        return R_UNASSESSABLE, "neither edge observed"
    if len(observed) != 2:
        return R_PARTIAL, "one edge observed; the other is unassessable"
    if any(e["offset"] != 0 for e in transitions):
        return R_OFFSET, ("a target transition sits off the label - possible label offset OR an unrelated "
                          "change (lighting, occlusion, neighbouring event); a person must look at "
                          "the listed frames k-1..k+1; does not establish cause")
    if len(transitions) == 2:
        return R_CONSISTENT, "consistent with the label; does not establish cause"
    if transitions:
        return R_PARTIAL, "one zero-delta transition; the other edge has NO-TRANSITION"
    lo = min([run["start"]] + [e["lo"] for e in edges])
    hi = max([run["end"]] + [e["hi"] for e in edges])
    span = [(k, run["signal"].pair(k)) for k in range(lo, hi + 1)]
    errors = [p["error"] for _k, p in span if p["error"]]
    if errors:
        return R_UNASSESSABLE, "whole-span check: " + errors[0]
    if any(e["mask_id_unresolved"] for e in edges) or any(p["unresolved"] for _k, p in span):
        return R_UNASSESSABLE, "NO-TRACE unavailable: mask id unresolved - sole value used"
    available, why = _no_trace_available(run)
    if not available:
        return R_UNASSESSABLE, why
    tau = min(e["tau"] for e in edges)
    changed = [(k, p["d"]) for k, p in span if p["d"] > tau]
    if changed:
        return R_UNASSESSABLE, "whole-span change at frame %d d=%.4f > tau=%.4f" % (
            changed[0][0], changed[0][1], tau)
    reason = ("no change above the noise floor (tau=%.4f) on this target across frames %d..%d; "
              "the label claims a visible change" % (tau, run["start"], run["end"]))
    if any(p["mask_extrapolations"] for _k, p in span):
        reason += "; edge pairs use extrapolated masks"
    return R_NO_TRACE, reason


def _event_token(runs):
    outcomes = [r["outcome"] for r in runs]
    for outcome in (R_NO_TRACE, R_OFFSET, R_PARTIAL, R_CONSISTENT):
        if outcome in outcomes:
            return outcome
    if outcomes and all(v == R_READING for v in outcomes):
        return R_READING
    return R_UNASSESSABLE


def _edge_line(rec):
    fmt = lambda x: "n/a" if x is None else "%.4f" % x
    obs = rec["observation"]
    if obs == O_TRANSITION:
        text = "TRANSITION %d d=%s tau=%s label %d delta=%+d ring d=%s" % (
            rec["best_k"], fmt(rec["best_d"]), fmt(rec["tau"]), rec["nominal"],
            rec["offset"], fmt(rec["ring_d"]))
        text += " look at frames %d..%d" % (rec["best_k"] - 1, rec["best_k"] + 1)
        if rec["ring_note"]:
            text += " note: whole-region change (lighting/camera?)"
        if rec["other_peaks"]:
            text += "; other peaks in window: " + ", ".join(map(str, rec["other_peaks"]))
        if rec["tie_peaks"]:
            text += "; tie: " + " vs ".join("%d d=%.6f" % (p["k"], p["d"])
                                           for p in rec["tie_peaks"]) + " -> %d" % rec["best_k"]
    elif obs == O_NONE:
        text = "NO-TRANSITION [%d..%d] tau=%s label %d" % (rec["lo"], rec["hi"],
                                                           fmt(rec["tau"]), rec["nominal"])
    else:
        text = "UNASSESSABLE(%s) label %d" % (rec["reason"], rec["nominal"])
    text += " m_edge=%s base=%d pairs=%d/%d" % (fmt(rec["m_edge"]), rec["base_n"],
                                               rec["scanned"], rec["window"])
    if rec["mask_id_unresolved"]:
        text += " (mask id unresolved - sole value used)"
    for source in sorted({s for s, _d in rec["mask_extrapolations"]}):
        deltas = [d for s, d in rec["mask_extrapolations"] if s == source]
        worst = max(deltas, key=abs)
        text += " (mask extrapolated from frame %d, delta=%+d; max distance, %d frames incl baseline)" % (
            source, worst, len(deltas))
    if rec["mode"] == "bbox":
        text += " (bbox-only)"
    return "        %-5s %s" % (rec["kind"], text)


def _assignment_notes(edges):
    return "".join("; note: assigned frames out of label order (%d > %d)" % e["assignment_inversion"]
                   for e in edges if e["assignment_inversion"] is not None)


def label_pixel_gate(cap_dir, thresh, edge_w, min_visible_px, quiet=False, out_detail=None):
    """Return observations/consistency counts. Only whole-span NO-TRACE returns 2."""
    from collections import Counter
    try:
        mlo = _offset_module()
    except RuntimeError as exc:
        return 3, ["LABEL-PIXEL: CANNOT RUN - %s" % exc]
    events = _load_gate_events(cap_dir, mlo)
    if events is None:
        return 3, ["LABEL-PIXEL: CANNOT RUN - no readable annotation.json"]
    rows, state = mlo.read_labels(os.path.join(cap_dir, "labels.jsonl"))
    if state == "absent" or not rows:
        return 3, ["LABEL-PIXEL: CANNOT RUN - no readable labels.jsonl"]
    paths = _frame_paths(cap_dir, mlo)
    if len(paths) < 3:
        return 3, ["LABEL-PIXEL: CANNOT RUN - fewer than 3 RGB frames"]
    cache = mlo.FrameCache(paths, 0, limit=64)
    size = cache.frame_size()
    if not size:
        return 3, ["LABEL-PIXEL: CANNOT RUN - unreadable RGB frames"]
    session_indices = set(rows) | set(paths)
    first, last = min(session_indices), max(session_indices)
    hot = _HotCache(cache, thresh)
    windows = [(min(e["indices"]), max(e["indices"])) for e in events if e["indices"]]
    for guard in range(BASELINE_GUARD_FRAMES, -1, -1):
        blocked = {k for a, b in windows for k in range(a - guard, b + guard + 1)}
        base_idx = [k for k in sorted(paths) if k not in blocked and k - 1 not in blocked and k - 1 in paths]
        if len(base_idx) >= MIN_BASELINE_FRAMES:
            break
    runs, by_event, event_errors = [], {}, {}
    for ev in events:
        by_event[ev["i"]] = []
        if not ev["manifested"] or not ev["indices"] or ev["derived"]:
            event_errors[ev["i"]] = "non-manifested/empty event or no explicit frame_indices"
            continue
        _anchor, _row, entry = _anchor_entry(rows, ev["indices"], ev["node"], mlo)
        if entry is None:
            event_errors[ev["i"]] = "no matching labels.jsonl entry"
            continue
        for ordinal, (a, b) in enumerate(_runs_of(ev["indices"])):
            def present(k):
                mf = (rows.get(k) or {}).get("mask_file")
                return bool(mf and os.path.isfile(os.path.join(cap_dir, str(mf))))
            mode = "mask" if any(present(k) for k in range(a, b + 1)) else "bbox"
            row = rows.get(a) or {}
            run_entry = _target_entry(row, ev["node"]) or entry
            run = {"event": ev["i"], "ordinal": ordinal, "start": a, "end": b,
                   "node": ev["node"], "type": ev["type"], "mode": mode, "edges": []}
            held_key = HELD_GATED_TYPES.get(ev["type"])
            if held_key:
                run["held_gate_ok"], run["held_gate_frames"] = _held_gate(
                    rows, ev["node"], a, b, held_key)
            run["signal"] = _PairSignal(cap_dir, rows, ev["node"], run_entry, mode,
                                         hot, paths, size, mlo, min_visible_px, (a, b), edge_w)
            runs.append(run)
            by_event[ev["i"]].append(run)
    _observe_windows(runs, first, last, edge_w, base_idx)
    for run in runs:
        run["outcome"], run["reason"] = _run_outcome(run)
        run["lighting_caveat"] = run["outcome"] == R_CONSISTENT and any(e["ring_note"] for e in run["edges"])
        run["max_m_edge"] = max((e["m_edge"] for e in run["edges"] if e["m_edge"] is not None), default=None)
        run["motion_caveat"] = run["max_m_edge"] is not None and run["max_m_edge"] > HIGH_REGIONAL_CHANGE_MARKER
        run["caveat"] = run["lighting_caveat"] or run["motion_caveat"]
        run["mask_extrapolation_max"] = max((abs(d) for e in run["edges"]
                                             for _s, d in e["mask_extrapolations"]), default=0)
        for rec in run["edges"]:
            rec.update(run_outcome=run["outcome"], run_reason=run["reason"],
                       run_eligible=run["outcome"] not in (R_READING, R_UNASSESSABLE),
                       run_caveat=run["caveat"], run_lighting_caveat=run["lighting_caveat"],
                       run_motion_caveat=run["motion_caveat"], run_max_m_edge=run["max_m_edge"],
                       run_mask_extrapolation_max=run["mask_extrapolation_max"], session=cap_dir)
            if out_detail is not None:
                out_detail.append(dict(rec))
    lines = ["LABEL-PIXEL CONSISTENCY OBSERVATIONS (079-13)",
             "  session                  %s" % cap_dir,
             "  frames / labels / events %d / %d / %d" % (len(paths), len(rows), len(events)),
             "  semantics                masks identify pixels, not cause; no output certifies a label as correct",
             "  region                   union of both masks; actual target masks take precedence; bbox-only runs are READING",
             "  boundary masks           missing OUTSIDE the span only: extrapolate same-run boundary <=%d frames (window+N_BASE=%d+%d)" % (edge_w + BASELINE_MAX_FRAMES, edge_w, BASELINE_MAX_FRAMES),
             "                           A real per-frame mask is never replaced; extrapolation is used only where the producer wrote none, and every affected line says so.",
             "  signal                   fraction with an RGB-channel difference >%d/255; edge tau=max(median+6*MAD,0.004)" % thresh,
             "  regional change          m_edge is a reading; >%.2f adds a caveat (historical marker, not a boundary)" % HIGH_REGIONAL_CHANGE_MARKER,
             "  characterisation         docs/verifier-characterisation.md",
             "                           Per-session recovery readings and all wrong cells with competing peaks; known limitations, not label approval.",
             "  clean-frame pool         %d (guard %d); nearest 3..24 valid per-pair regions for EACH edge" % (len(base_idx), guard),
             "  search                   +/-%d, inclusive labelled bounds; peaks >tau and >=1.5 x min(neighbours), outside window=0" % edge_w,
             "  assignment               per target/mode, labelled order: nearest unused peak; tie larger d, then earlier; inversions disclosed",
             "  producer metadata        bbox/observable flags are reported, not confirmed by this tool",
             "  NO-TRACE scope           no change above the noise floor (tau) over the complete span AND edge windows",
             "  NO-TRACE classes         %s; plus %s only where the producer's own per-frame flag is true on EVERY labelled frame" % (
                 ", ".join(sorted(NO_TRACE_TYPES)),
                 ", ".join("%s (%s)" % (t, k) for t, k in sorted(HELD_GATED_TYPES.items()))),
             "  detection limit          tau is a fraction of target-region pixels; RGB-channel threshold also applies",
             "-" * 78]
    counts = Counter()
    consistent_caveats = 0
    for ev in events:
        ev_runs = by_event[ev["i"]]
        token = _event_token(ev_runs)
        counts[token] += 1
        consistent_caveats += token == R_CONSISTENT and any(r["outcome"] == R_CONSISTENT and r["caveat"] for r in ev_runs)
        rc = Counter(r["outcome"] for r in ev_runs)
        summary = " ".join("%s=%d" % (name, rc[name]) for name in RUN_OUTCOMES)
        lines.append("idx=%-3d %-18s %-22s %s runs{%s} %s" % (
            ev["i"], ev["type"], ev["node"] or "(no node)", token, summary, _provenance(ev)))
        if ev["i"] in event_errors:
            lines.append("      UNASSESSABLE(%s)" % event_errors[ev["i"]])
        for run in ev_runs:
            label = run["outcome"]
            if run["lighting_caveat"]:
                label += " (caveat: edge coincides with a whole-region change - inspect frames k-1..k+1)"
            if label == R_OFFSET:
                label += "(%s)" % ",".join("%+d" % e["offset"] if e["offset"] is not None else
                                            "no-transition" for e in run["edges"])
            if run["motion_caveat"]:
                label += " (caveat: high regional change m=%.4f - readings under motion are less reliable)" % run["max_m_edge"]
            mask_note = ""
            if run["mode"] == "mask":
                mask_note = "; masks: in-span actual, edges " + (
                    "extrapolated <=%d (including baseline)" % run["mask_extrapolation_max"]
                    if run["mask_extrapolation_max"] else "actual")
            if any(len(e["peaks"]) > 1 for e in run["edges"]):
                mask_note += "; assignment: " + ", ".join(
                    "%s %s (nearest of {%s})" % (e["kind"],
                        str(e["best_k"]) if e["found"] else "none",
                        ", ".join(map(str, e["available_peaks"]))) for e in run["edges"])
            mask_note += _assignment_notes(run["edges"])
            lines.append("      run[%d..%d] %s%s%s" % (run["start"], run["end"], label,
                " (bbox-only)" if run["mode"] == "bbox" else " - " + run["reason"], mask_note))
            lines.extend(_edge_line(e) for e in run["edges"])
    lines.extend(["-" * 78, "  " + "  ".join(("%s %d (%d with caveat)" % (name, counts[name], consistent_caveats)
                  if name == R_CONSISTENT else "%s %d" % (name, counts[name])) for name in RUN_OUTCOMES)
                  + "  (of %d events)" % len(events)])
    run_counts = Counter(r["outcome"] for r in runs)
    lines.append("  run coverage             " + "  ".join("%s %d" % (name, run_counts[name]) for name in RUN_OUTCOMES)
                 + "  (of %d runs; unassessable/readings never promote an event)" % len(runs))
    if counts[R_NO_TRACE]:
        lines.append("  VERDICT                  FAIL: NO-TRACE on %d event(s) - no change above the noise floor; labels claim visible changes" % counts[R_NO_TRACE])
    else:
        lines.append("  VERDICT                  NO FAILURE FOUND (%d consistent (%d with caveat), %d offset-notes for human review, %d partial, %d unassessable, %d readings)" %
                     (counts[R_CONSISTENT], consistent_caveats, counts[R_OFFSET], counts[R_PARTIAL], counts[R_UNASSESSABLE], counts[R_READING]))
        if events and counts[R_READING] == len(events):
            lines.append("  coverage                 UNREAD-BBOX-ONLY")
    return (2 if counts[R_NO_TRACE] else 0), lines


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
    """The counterexample fixtures from the 079-07 review.

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
    """The counterexample fixtures from the 079-05 review.

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


def _codex08_fixture(root, name, true_runs=((60, 67),), label_runs=None, lighting=False, occluder=False,
            move=False, mask_mode="precise", ring_burst=False, missing_masks=(), missing_rgb=()):
    """Target exists throughout. A 10x10 texture fault occupies its right side only on true_runs.

    Precise masks contain exactly visible target pixels on EVERY frame, including during occlusion.
    Deliberately invalid masks (lag/dilated/foreign) are explicit fault-injection variants.
    """
    from PIL import Image, ImageDraw
    import pathlib
    p = pathlib.Path(root) / name
    (p / "Actual_Frames").mkdir(parents=True)
    (p / "target_mask").mkdir()
    w, h, y, side = 180, 120, 30, 40
    total = 100
    lr = label_runs if label_runs is not None else true_runs
    idx = [k for a, b in lr for k in range(a, b + 1)]
    rows = []
    def xpos(k):
        return 70 if move and k >= 61 else 40
    for k in range(total):
        x = xpos(k)
        im = Image.new("RGB", (w, h), (90, 90, 90))
        draw = ImageDraw.Draw(im)
        if ring_burst and k >= 61:
            draw.rectangle((x-10, y-10, x+side+9, y+side+9), fill=(150, 150, 150))
        draw.rectangle((x, y, x+side-1, y+side-1), fill=(80, 80, 80))
        if any(a <= k <= b for a, b in true_runs):
            draw.rectangle((x+28, y+5, x+37, y+14), fill=(230, 230, 230))
        occluded = occluder and 61 <= k <= 65
        if occluded:
            draw.rectangle((x, y, x+19, y+side-1), fill=(180, 180, 180))
        if mask_mode == "foreign":
            c = 230 if 61 <= k <= 67 else 70
            draw.rectangle((130, 30, 149, 49), fill=(c, c, c))
        if lighting and k >= 61:
            im = im.point(lambda value: min(255, value+40))
        if k not in missing_rgb:
            im.save(p / "Actual_Frames" / ("frame_%05d.png" % k))
        mk = Image.new("L", (w, h), 0)
        md = ImageDraw.Draw(mk)
        mx = xpos(k-1) if mask_mode == "lag" else x
        box = (mx+(20 if occluded else 0), y, mx+side-1, y+side-1)
        if mask_mode == "dilated":
            box = (mx-10, y-10, mx+side+9, y+side+9)
        elif mask_mode == "foreign":
            box = (130, 30, 149, 49)
        md.rectangle(box, fill=111 if mask_mode == "foreign" else 222)
        if mask_mode != "none" and k not in missing_masks:
            mk.save(p / "target_mask" / ("frame_%05d.png" % k))
        entry = {"id": "missing_texture", "target_name": "SynthTarget", "start_frame": 1000+min(idx),
                 "bbox_valid": True, "bbox_px": [x, y, side, side], "mask_value": 222}
        row = {"frame_index": 1000+k, "session_index": k, "width": w, "height": h,
               "image": "Actual_Frames/frame_%05d.png" % k, "anomaly_present": k in idx,
               "visible_positive": k in idx, "anomalies": [entry] if k in idx else []}
        if mask_mode != "none":
            row.update(mask_file="target_mask/frame_%05d.png" % k, mask_state="present")
        rows.append(row)
    (p / "labels.jsonl").write_text("".join(json.dumps(r)+"\n" for r in rows), encoding="utf-8")
    ann = {"anomalies": [{"anomaly_type": "missing_texture", "manifested": True,
        "affected_frames": {"start_frame": min(idx), "end_frame": max(idx), "frame_count": len(idx),
                            "frame_indices": idx},
        "affected_objects": {"primary_index": 0, "nodes": [{"name": "SynthTarget"}]}}]}
    (p / "annotation.json").write_text(json.dumps(ann), encoding="utf-8")
    return str(p)

def _label_pixel_selftest(thresh, edge_w, min_visible_px, source_dir=None):
    """Permanent review regressions, with per-edge checks and whole-span negatives."""
    import tempfile
    import pathlib
    checks = []
    print("LABEL-PIXEL OBSERVATIONS SELFTEST", flush=True)

    def check(name, path, expected, verify=None):
        detail = []
        code, lines = label_pixel_gate(str(path), thresh, edge_w, min_visible_px,
                                       out_detail=detail)
        tokens = [line.split()[3] for line in lines if line.startswith("idx=")]
        ok = bool(tokens) and all(token in expected for token in tokens)
        ok &= code == (2 if R_NO_TRACE in tokens else 0)
        ok &= not any(word in line for line in lines for word in ("PASS ", "SHIFT(", "NOT-VISIBLE"))
        if verify:
            ok &= bool(verify(detail, lines))
        checks.append(ok)
        print("  %-43s %-32s exit%d %s" % (name, ",".join(tokens), code, "OK" if ok else "MISMATCH"), flush=True)
        if not ok:
            print("    expected %s" % (expected,), flush=True)
            for line in lines:
                print(line, flush=True)
        return detail, lines

    def all_unassessed(detail, _lines):
        return all(e["observation"] == O_UNASSESSABLE for e in detail)

    with tempfile.TemporaryDirectory(prefix="m49_consistency_selftest_") as root:
        original = [
            ("clean", {}, (R_CONSISTENT,)),
            ("clean_masked", {}, (R_CONSISTENT,)),
            ("label_late_1", {"shift": 1}, (R_OFFSET,)),
            ("label_early_1", {"shift": -1}, (R_OFFSET,)),
            ("end_late_1", {"end_shift": 1}, (R_OFFSET,)),
            ("end_early_1", {"end_shift": -1}, (R_OFFSET,)),
            ("blank_region", {"blank_region": True}, (R_NO_TRACE,)),
            ("moving_clean", {"pan": 2}, (R_CONSISTENT,)),
            ("moving_fast", {"pan": 8}, (R_CONSISTENT,)),
            ("moving_label_late_1", {"pan": 2, "shift": 1}, (R_OFFSET,)),
            ("moving_label_early_1", {"pan": 2, "shift": -1}, (R_OFFSET,)),
            ("moving_end_late_1", {"pan": 2, "end_shift": 1}, (R_OFFSET,)),
            ("moving_end_early_1", {"pan": 2, "end_shift": -1}, (R_OFFSET,)),
            ("moving_blank_region", {"pan": 2, "blank_region": True}, (R_NO_TRACE,)),
            ("moving_fullframe_region", {"pan": 2, "fullframe_region": True}, (R_UNASSESSABLE,)),
            ("moving_unsat", {"pan": 4, "stripes": 4}, (R_UNASSESSABLE,)),
            ("moving_over_cap", {"pan": 2}, (R_CONSISTENT,)),
        ]
        for name, args, expected in original:
            check(name, _synth_session(root, name, with_mask=True, **args), expected)
        for name, shift in (("bboxonly_clean", 0), ("bboxonly_label_late_1", 1)):
            check(name, _synth_session(root, name, shift=shift), (R_READING,))
        older = [
            ("codex_quiet_prefix", {}),
            ("codex_small_region", {"local_only": True}),
            ("codex_alternating", {"local_only": True, "alternating": True}),
            ("codex_quiet_prefix_shift1", {"shift": 1}),
            ("codex_small_region_shift1", {"local_only": True, "shift": 1}),
            ("codex_quiet_prefix_noburst_shift1", {"shift": 1, "burst": False}),
            ("codex_small_region_noburst_shift1", {"local_only": True, "shift": 1, "burst": False}),
        ]
        for name, args in older:
            check(name, _codex_fixture(root, name, **args), (R_READING,))
        for local in (False, True):
            name = "codex_masked_negative_%s" % local
            check(name, _codex_fixture(root, name, local_only=local, burst=False, shift=1,
                                       with_mask=True), (R_OFFSET,))

        cases07 = [
            ("burst_aligned", {"burst_at": 61}),
            ("burst_late", {"burst_at": 61, "onset_delta": 1}),
            ("static_aligned", {}),
            ("static_late", {"onset_delta": 1}),
            ("onset_early_refusal", {"runs": ((10, 11),), "burst_at": 10, "full_boxes": (10,)}),
            ("previous_end_refusal", {"runs": ((10, 12), (15, 16)), "label_runs": ((10, 11), (15, 16)),
                                       "full_boxes": (11,), "patch_sides": (40, 15)}),
            ("partial_multirun", {"runs": ((10, 11), (25, 26)), "full_boxes": (25, 26)}),
            ("missing_edge_frames", {"drop_frames": tuple(range(55, 74))}),
            ("early_refusal_control", {"runs": ((10, 11),), "burst_at": 10}),
            ("previous_end_control", {"runs": ((10, 12), (15, 16)), "label_runs": ((10, 11), (15, 16)),
                                       "patch_sides": (40, 15)}),
            ("below_tau", {"patch_sides": (1,)}),
        ]
        for name, args in cases07:
            check("c07_" + name, _codex07_fixture(root, "c07_" + name, **args), (R_READING,))
        masked07 = [
            ("burst_aligned_masked", {"burst_at": 61}, (R_CONSISTENT,)),
            ("burst_late_masked", {"burst_at": 61, "onset_delta": 1}, (R_OFFSET,)),
            ("onset_early_refusal_masked", {"runs": ((10, 11),), "burst_at": 10,
                                           "full_masks": (10,)}, (R_UNASSESSABLE,)),
            ("previous_end_refusal_masked", {"runs": ((10, 12), (15, 16)),
                "label_runs": ((10, 11), (15, 16)), "full_masks": (11,), "patch_sides": (40, 15)},
                (R_PARTIAL,)),
            ("previous_end_control_masked", {"runs": ((10, 12), (15, 16)),
                "label_runs": ((10, 11), (15, 16)), "patch_sides": (40, 15)}, (R_OFFSET,)),
            ("partial_multirun_masked", {"runs": ((10, 11), (25, 26)), "full_masks": (25, 26)},
                (R_CONSISTENT,)),
            ("mixed_runs", {"runs": ((10, 11), (25, 26)), "mask_runs": ((10, 11),)},
                (R_CONSISTENT,)),
            ("missing_edge_frames_masked", {"drop_frames": tuple(range(55, 74))}, (R_UNASSESSABLE,)),
            ("below_tau_masked", {"mask": "region", "patch_sides": (1,)}, (R_NO_TRACE,)),
            ("zero_cap_static_masked", {}, (R_CONSISTENT,)),
            ("zero_cap_burst_masked", {"burst_at": 61}, (R_CONSISTENT,)),
            ("no_envelope_masked", {}, (R_CONSISTENT,)),
        ]
        for name, args, expected in masked07:
            args = dict(args)
            args.setdefault("mask", "patch")
            check("c07_" + name, _codex07_fixture(root, "c07_" + name, **args), expected)

        cases08 = [
            ("lighting_aligned", {"lighting": True}, (R_CONSISTENT,)),
            ("lighting_late_onset", {"lighting": True, "label_runs": ((61, 67),)}, (R_CONSISTENT,)),
            ("occluder_aligned", {"occluder": True}, (R_CONSISTENT,)),
            ("static_aligned", {}, (R_CONSISTENT,)),
            ("static_late_onset", {"label_runs": ((61, 67),)}, (R_OFFSET,)),
            ("missing_mask61", {"missing_masks": (61,)}, (R_PARTIAL,)),
            ("missing_rgb61", {"missing_rgb": (61,)}, (R_PARTIAL,)),
            ("precise_mask_ring_burst", {"ring_burst": True}, (R_CONSISTENT,)),
            ("dilated_mask_ring_burst", {"ring_burst": True, "mask_mode": "dilated"}, (R_CONSISTENT,)),
            ("precise_mask_target_move", {"move": True}, (R_CONSISTENT,)),
            ("lagged_mask_target_move", {"move": True, "mask_mode": "lag", "label_runs": ((61, 67),)},
                (R_CONSISTENT,)),
            ("foreign_mask_value", {"mask_mode": "foreign"}, (R_UNASSESSABLE,)),
            ("no_detected_end", {"true_runs": ((60, 90),), "label_runs": ((60, 67),)}, (R_PARTIAL,)),
            ("mixed_run", {"missing_masks": (67,)}, (R_PARTIAL,)),
            ("one_frame_aligned", {"true_runs": ((60, 60),)}, (R_CONSISTENT,)),
            ("one_frame_late", {"true_runs": ((60, 60),), "label_runs": ((61, 61),)}, (R_PARTIAL,)),
            ("one_frame_early", {"true_runs": ((60, 60),), "label_runs": ((59, 59),)}, (R_OFFSET,)),
        ]
        for name, args, expected in cases08:
            def verify(detail, lines, name=name):
                if name == "lighting_late_onset":
                    return (any(e["ring_note"] for e in detail) and all(e["run_outcome"] != R_NO_TRACE for e in detail)
                            and all(e["run_caveat"] for e in detail)
                            and any("CONSISTENT (caveat:" in s for s in lines)
                            and any("CONSISTENT 1 (1 with caveat)" in s for s in lines))
                if name == "one_frame_late":
                    return ([e["best_k"] for e in detail] == [61, None]
                            and detail[0]["peaks"] == [60, 61]
                            and detail[1]["consumed_peaks"] == [61]
                            and detail[1]["reason"] == "all peaks in window assigned to earlier edges: 61")
                if name == "missing_mask61":
                    return detail[0]["observation"] == O_UNASSESSABLE and "mask missing at frame 61" in detail[0]["reason"]
                if name == "foreign_mask_value":
                    return all("mask id 222 not present" in e["reason"] for e in detail)
                if name == "mixed_run":
                    return all(e["run_eligible"] and e["run_outcome"] == R_PARTIAL for e in detail)
                if name == "no_detected_end":
                    return [e["observation"] for e in detail] == [O_TRANSITION, O_NONE]
                return True
            check("c08_" + name, _codex08_fixture(root, "c08_" + name, **args), expected, verify=verify)

        for name, labels, mask_runs in (
            ("adjacent_control", ((10, 11), (15, 16)), None),
            ("adjacent_late_second_onset", ((10, 11), (16, 16)), None),
            ("adjacent_early_second_onset", ((10, 11), (14, 16)), None),
            ("mixed_late_second_onset", ((10, 11), (16, 16)), ((15, 99),))):
            path = _codex07_fixture(root, name, runs=((10, 11), (15, 16)), label_runs=labels,
                                    patch_sides=(10, 30), mask="region", mask_runs=mask_runs)
            def verify(detail, _lines, name=name):
                if name == "adjacent_late_second_onset":
                    return ([e["best_k"] for e in detail] == [10, 12, 15, 17] and
                            [e["offset"] for e in detail] == [0, 0, -1, 0])
                return True
            expected = (R_CONSISTENT,) if name == "adjacent_control" else (R_OFFSET,)
            check(name, path, expected, verify=verify)

        for name, truth, labels, expected, selected, offsets, outcomes in (
            ("r6_blink_aligned", ((4, 5), (8, 9)), ((4, 5), (8, 9)), R_CONSISTENT,
             [4, 6, 8, 10], [0, 0, 0, 0], [R_CONSISTENT] * 4),
            ("r6_blink_late_second_onset", ((4, 5), (8, 9)), ((4, 5), (9, 9)), R_OFFSET,
             [4, 6, 8, 10], [0, 0, -1, 0], [R_CONSISTENT] * 2 + [R_OFFSET] * 2),
            ("r6_blink_missing_peak6", ((4, 7), (10, 99)), ((4, 5), (8, 9)), R_OFFSET,
             [4, 8, 10, None], [0, 2, 2, None], [R_OFFSET] * 2 + [R_PARTIAL] * 2)):
            path = _codex07_fixture(root, name, runs=truth, label_runs=labels, mask="patch")
            def verify(detail, lines, selected=selected, offsets=offsets, outcomes=outcomes):
                return ([e["best_k"] for e in detail] == selected and
                        [e["offset"] for e in detail] == offsets and
                        [e["run_outcome"] for e in detail] == outcomes and
                        any("assignment: onset" in s for s in lines) and
                        (not any(e["other_peaks"] for e in detail) or
                         any("other peaks in window:" in s for s in lines)) and
                        (selected[-1] is not None or detail[-1]["reason"] ==
                         "all peaks in window assigned to earlier edges: 8, 10"))
            check(name, path, (expected,), verify=verify)

        name = "r6_blink_one_frame_second"
        path = _codex07_fixture(root, name, runs=((4, 6), (10, 10)), mask="patch")
        check(name, path, (R_CONSISTENT,), verify=lambda d, _l:
              [e["best_k"] for e in d] == [4, 7, 10, 11] and
              [e["offset"] for e in d] == [0, 0, 0, 0] and
              [e["run_outcome"] for e in d] == [R_CONSISTENT] * 4)

        blank = _codex08_fixture(root, "blank_whole_span", true_runs=(), label_runs=((40, 70),))
        check("blank_whole_span", blank, (R_NO_TRACE,))
        missing = _codex08_fixture(root, "blank_missing_interior", true_runs=(),
                                   label_runs=((40, 70),), missing_masks=(55,))
        check("blank_missing_interior", missing, (R_UNASSESSABLE,),
              verify=lambda d, _l: all(e["run_outcome"] == R_UNASSESSABLE and
                                       "mask missing at frame 55" in e["run_reason"] for e in d))
        interior = _codex08_fixture(root, "interior_change", true_runs=((55, 56),), label_runs=((40, 70),))
        check("interior_change", interior, (R_UNASSESSABLE,),
              verify=lambda d, _l: all(e["run_outcome"] == R_UNASSESSABLE for e in d))
        for variant in ("unresolved", "unsupported_class", "empty_mask", "unreadable_mask"):
            path = pathlib.Path(_codex08_fixture(root, variant, true_runs=(), label_runs=((40, 70),)))
            if variant in ("unresolved", "unsupported_class"):
                rows = [json.loads(line) for line in (path / "labels.jsonl").read_text().splitlines()]
                for row in rows:
                    for entry in row["anomalies"]:
                        if variant == "unresolved":
                            entry["mask_value"] = 0
                        else:
                            entry["id"] = "time_dilation"
                (path / "labels.jsonl").write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
                if variant == "unsupported_class":
                    ann = json.loads((path / "annotation.json").read_text())
                    ann["anomalies"][0]["anomaly_type"] = "time_dilation"
                    (path / "annotation.json").write_text(json.dumps(ann), encoding="utf-8")
            elif variant == "empty_mask":
                from PIL import Image
                Image.new("L", (180, 120)).save(path / "target_mask/frame_00041.png")
            else:
                (path / "target_mask/frame_00041.png").write_bytes(b"invalid image")
            expected = (R_UNASSESSABLE,) if variant in ("unresolved", "unsupported_class") else (R_PARTIAL,)
            check(variant, path, expected, verify=lambda d, _l: all(e["run_outcome"] != R_NO_TRACE for e in d))
        dimensions = pathlib.Path(_codex08_fixture(root, "rgb_dimensions", true_runs=(),
                                                    label_runs=((40, 70),)))
        from PIL import Image, ImageDraw
        for k in range(35, 45):
            Image.new("RGB", (200, 130), (90, 90, 90)).save(
                dimensions / "Actual_Frames" / ("frame_%05d.png" % k))
        check("rgb_dimensions", dimensions, (R_PARTIAL,),
              verify=lambda d, _l: d[0]["observation"] == O_UNASSESSABLE and
              "RGB dimensions changed" in d[0]["reason"])
        colour = pathlib.Path(_codex08_fixture(root, "equal_luma_colour"))
        assert Image.new("RGB", (1, 1), (255, 0, 0)).convert("L").getpixel((0, 0)) == \
               Image.new("RGB", (1, 1), (0, 130, 0)).convert("L").getpixel((0, 0))
        for k in range(100):
            im = Image.new("RGB", (180, 120), (90, 90, 90))
            ImageDraw.Draw(im).rectangle((40, 30, 79, 69),
                fill=(0, 130, 0) if 60 <= k <= 67 else (255, 0, 0))
            im.save(colour / "Actual_Frames" / ("frame_%05d.png" % k))
        check("equal_luma_colour", colour, (R_CONSISTENT,),
              verify=lambda d, _l: all(e["observation"] == O_TRANSITION and e["best_d"] == 1 for e in d))
        for name, span, missing, edge in (("missing_first_rgb", (3, 8), 0, 0),
                                          ("missing_last_rgb", (90, 97), 99, 1)):
            path = _codex08_fixture(root, name, true_runs=(), label_runs=(span,), missing_rgb=(missing,))
            check(name, path, (R_PARTIAL,),
                  verify=lambda d, _l, edge=edge: d[edge]["observation"] == O_UNASSESSABLE and
                  "RGB missing" in d[edge]["reason"] and d[1-edge]["observation"] == O_NONE)
        outside = tuple(k for k in range(100) if not 40 <= k <= 70)
        for name, true_runs, expected in (("span_masks_control", ((40, 70),), (R_CONSISTENT,)),
                                           ("span_masks_blank", (), (R_NO_TRACE,))):
            path = _codex08_fixture(root, name, true_runs=true_runs, label_runs=((40, 70),), missing_masks=outside)
            check(name, path, expected, verify=lambda d, lines, name=name:
                  all(e["mask_extrapolations"] and e["run_mask_extrapolation_max"] <= edge_w + BASELINE_MAX_FRAMES
                      for e in d) and any("masks: in-span actual, edges extrapolated" in s for s in lines)
                  and (name != "span_masks_blank" or any("edge pairs use extrapolated masks" in s for s in lines)))
        for name, missing_frames, expected in (
            ("span_masks_missing_interior", outside + (55,), (R_UNASSESSABLE,)),
            ("span_masks_missing_onset", outside + (40,), (R_PARTIAL,)),
            ("span_masks_missing_end", outside + (70,), (R_PARTIAL,))):
            path = _codex08_fixture(root, name, true_runs=(), label_runs=((40, 70),), missing_masks=missing_frames)
            check(name, path, expected, verify=lambda d, _l: all(e["run_outcome"] != R_NO_TRACE for e in d))
        for name, missing_frames, expected in (
            ("consistent_with_unassessable_run", (41, 46), (R_CONSISTENT,)),
            ("consistent_with_partial_run", (41,), (R_PARTIAL,))):
            path = _codex08_fixture(root, name, true_runs=((10, 17), (40, 47)), missing_masks=missing_frames)
            check(name, path, expected, verify=lambda d, lines, name=name:
                  any(e["run_outcome"] == R_CONSISTENT for e in d) and
                  (name != "consistent_with_unassessable_run" or
                   any("run coverage" in s and "UNASSESSABLE 1" in s for s in lines)))
        for variant, held, expected in (("stuck_mip_held", True, (R_NO_TRACE,)),
                                        ("stuck_mip_unheld", False, (R_UNASSESSABLE,)),
                                        ("stuck_mip_no_flag", None, (R_UNASSESSABLE,))):
            path = pathlib.Path(_codex08_fixture(root, variant, true_runs=(), label_runs=((40, 70),)))
            rows = [json.loads(line) for line in (path / "labels.jsonl").read_text().splitlines()]
            for row in rows:
                for entry in row["anomalies"]:
                    entry["id"] = "stuck_low_mip"
                    if held is not None:
                        entry["stuck_mip.held"] = held
            if variant == "stuck_mip_unheld":
                for row in rows:
                    for entry in row["anomalies"]:
                        if row["session_index"] == 55:
                            entry["stuck_mip.held"] = False
                        else:
                            entry["stuck_mip.held"] = True
            (path / "labels.jsonl").write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
            ann = json.loads((path / "annotation.json").read_text())
            ann["anomalies"][0]["anomaly_type"] = "stuck_low_mip"
            (path / "annotation.json").write_text(json.dumps(ann), encoding="utf-8")
            check(variant, path, expected, verify=lambda d, _l, variant=variant:
                  all(e["run_outcome"] != R_NO_TRACE for e in d) if variant != "stuck_mip_held"
                  else all(e["run_outcome"] == R_NO_TRACE for e in d))

        check("event_mixed_fabricated_run",
              _codex08_fixture(root, "event_mixed_fabricated_run",
                               true_runs=((24, 24),), label_runs=((10, 15), (24, 24))),
              (R_NO_TRACE,),
              verify=lambda d, lines: sorted({e["run_outcome"] for e in d}) == [R_CONSISTENT, R_NO_TRACE] and
              any("run coverage" in s and "CONSISTENT 1" in s and "NO-TRACE 1" in s for s in lines))
        if source_dir:
            for delta in (-1, 1):
                dst = os.path.join(root, "real_%+d" % delta)
                _shifted_copy_of(source_dir, dst, delta)
                check("real_%+d" % delta, dst, (R_CONSISTENT, R_OFFSET, R_PARTIAL, R_UNASSESSABLE, R_READING))
    print("SELFTEST: %s - %d cases; observations, whole-span coverage and run eligibility" %
          ("OK" if all(checks) else "MISMATCH", len(checks)), flush=True)
    return 0 if all(checks) else 2


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


def label_pixel_batch(root, out_dir, thresh, edge_w, min_visible_px, quiet, report_only):
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
    st = _label_pixel_selftest(thresh, edge_w, min_visible_px)
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
            code, lines = label_pixel_gate(sess, thresh, edge_w, min_visible_px, quiet)
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
            if l.strip().startswith("CONSISTENT "):
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


CHANGE_ORACLE_SIDECAR = "change_evidence.jsonl"
CHANGE_ORACLE_SENTENCE = ("agreement validates arithmetic and transport only — that the numbers in the "
                          "sidecar are the numbers the delivered images contain. It does not establish "
                          "renderer pairing, visible effect, or cause.")
CHANGE_ORACLE_MEAN_TOLERANCE = 0.00005
CHANGE_ORACLE_TAU_V1 = 8
CHANGE_ORACLE_BINS = ((0, 0), (1, 2), (3, 4), (5, 8), (9, 16), (17, 32), (33, 64), (65, 255))
CHANGE_ORACLE_MATCH = "MATCH"
CHANGE_ORACLE_MISMATCH = "MISMATCH"
CHANGE_ORACLE_UNAVAILABLE = "UNAVAILABLE"


def _oracle_stats(d_hist, tau):
    """Region statistics from a 256-bin histogram of d = max(|dR|,|dG|,|dB|) over the region.

    Integer arithmetic throughout; the mean is returned unrounded so the comparison against the
    producer's four-decimal value is made against the exact quotient, not a second rounding.
    """
    n = int(sum(d_hist))
    total = int(sum(i * int(c) for i, c in enumerate(d_hist)))
    gt = int(sum(d_hist[tau + 1:])) if 0 <= tau <= 255 else 0
    bins = [int(sum(d_hist[lo:hi + 1])) for lo, hi in CHANGE_ORACLE_BINS]
    return {"n": n, "gt": gt, "sum": total, "hist": bins,
            "mean": (total / float(n) / 255.0) if n else None}


def _oracle_round4(value):
    import math
    return math.floor(value * 10000.0 + 0.5) / 10000.0


class _OracleImages(object):
    """Decoded delivered PNGs by session index, read from disk and nothing else.

    Colour frames are Actual_Frames/frame_%05d.png and masks target_mask/frame_%05d.png, the
    naming the writer uses (session index, G161). A file that is not a PNG, or a mask that is not
    8-bit grayscale, is refused with its reason rather than decoded into a guess.
    """

    def __init__(self, cap_dir, limit=8):
        self.cap_dir = cap_dir
        self.limit = max(4, limit)
        self.cache = {}

    def _open(self, key, rel, want_mode):
        hit = self.cache.get(key)
        if hit is not None:
            return hit
        from PIL import Image
        path = os.path.join(self.cap_dir, rel)
        result = None
        if not os.path.isfile(path):
            stem = os.path.splitext(path)[0]
            if want_mode == "RGB" and any(os.path.isfile(stem + ext) for ext in (".jpg", ".jpeg")):
                result = (None, "jpeg delivery - lossy, the delivered bytes are not the measured buffer")
            else:
                result = (None, "%s missing on disk" % rel.replace("\\", "/"))
        else:
            try:
                im = Image.open(path)
                fmt = im.format
                im.load()
            except Exception as exc:
                result = (None, "%s unreadable (%s)" % (rel.replace("\\", "/"), exc.__class__.__name__))
            else:
                if fmt != "PNG":
                    result = (None, "%s is %s, not PNG" % (rel.replace("\\", "/"), fmt))
                elif want_mode == "L" and im.mode != "L":
                    result = (None, "%s mode %s is not 8-bit grayscale" % (rel.replace("\\", "/"), im.mode))
                elif want_mode == "RGB" and im.mode not in ("RGB", "RGBA"):
                    result = (None, "%s mode %s is not 8-bit RGB" % (rel.replace("\\", "/"), im.mode))
                else:
                    result = (im.convert(want_mode) if im.mode != want_mode else im, None)
        while len(self.cache) >= self.limit:
            self.cache.pop(next(iter(self.cache)))
        self.cache[key] = result
        return result

    def colour(self, si):
        return self._open(("c", si), os.path.join("Actual_Frames", "frame_%05d.png" % si), "RGB")

    def mask(self, si):
        return self._open(("m", si), os.path.join("target_mask", "frame_%05d.png" % si), "L")


def _oracle_d(cur, prev):
    """d(p) = max over R,G,B of |cur - prev|, as an 8-bit image."""
    from PIL import ImageChops
    r, g, b = ImageChops.difference(cur, prev).split()
    return ImageChops.lighter(ImageChops.lighter(r, g), b)


def _oracle_region(mask, value):
    return mask.point([255 if v == value else 0 for v in range(256)])


def _oracle_int(value):
    if isinstance(value, bool) or not isinstance(value, int):
        return None
    return value


def _oracle_compare(rec, prefix, published_row, stats):
    """Compare one region's published statistics against the recomputed ones, field by field."""
    fields = []

    def check(name, published, recomputed, ok):
        fields.append({"field": name, "published": published, "recomputed": recomputed, "ok": bool(ok)})

    n_pub = published_row.get(prefix + "_n")
    check(prefix + "_n", n_pub, stats["n"], _oracle_int(n_pub) == stats["n"])
    gt_pub = published_row.get(prefix + "_gt8")
    check(prefix + "_gt8", gt_pub, stats["gt"], _oracle_int(gt_pub) == stats["gt"])
    sum_pub = published_row.get(prefix + "_sum")
    if sum_pub is not None or prefix + "_sum" in published_row:
        check(prefix + "_sum", sum_pub, stats["sum"], _oracle_int(sum_pub) == stats["sum"])
    if prefix + "_hist" in published_row:
        hist_pub = published_row.get(prefix + "_hist")
        ok = (isinstance(hist_pub, list) and len(hist_pub) == len(stats["hist"])
              and all(_oracle_int(a) == b for a, b in zip(hist_pub, stats["hist"])))
        check(prefix + "_hist", hist_pub, stats["hist"], ok)
    mean_pub = published_row.get(prefix + "_mean")
    exact = stats["mean"]
    if exact is None:
        check(prefix + "_mean", mean_pub, None, False)
    elif isinstance(mean_pub, bool) or not isinstance(mean_pub, (int, float)):
        check(prefix + "_mean", mean_pub, round(exact, 8), False)
    else:
        check(prefix + "_mean", mean_pub, round(exact, 8),
              abs(float(mean_pub) - exact) <= CHANGE_ORACLE_MEAN_TOLERANCE + 1e-12)
    rec["fields"].extend(fields)
    return all(f["ok"] for f in fields)


def _oracle_json_type(value):
    if value is None:
        return "null"
    if isinstance(value, bool):
        return "boolean"
    if isinstance(value, (int, float)):
        return "number"
    if isinstance(value, str):
        return "string"
    if isinstance(value, list):
        return "array"
    if isinstance(value, dict):
        return "object"
    return type(value).__name__


def _oracle_uninterpretable_row(row):
    """Why a claimed (chg_measured: true) pair row cannot be interpreted, or None when it can."""
    for key in ("session_index", "prev_session_index", "mask_value", "tau_px"):
        if _oracle_int(row.get(key)) is None:
            return "%s is %s, an integer is required" % (key, "absent" if key not in row
                                                         else "a JSON " + _oracle_json_type(row.get(key)))
    receipt = row.get("receipt")
    if not isinstance(receipt, dict):
        return "receipt is %s, an object is required" % ("absent" if "receipt" not in row
                                                          else "a JSON " + _oracle_json_type(receipt))
    if "rect" not in receipt:
        return "receipt.rect is absent, an array is required"
    if not isinstance(receipt.get("rect"), list):
        return "receipt.rect is a JSON %s, an array is required" % _oracle_json_type(receipt.get("rect"))
    return None


def _oracle_contradictions(row):
    """Fields a measured row cannot carry together with chg_measured: true."""
    out = []

    def bad(field, published, required):
        out.append({"field": "measured_row:" + field, "published": published, "recomputed": required, "ok": False})

    if row.get("reason") is not None:
        bad("reason", row.get("reason"), None)
    if row.get("chg_eligible") is not True:
        bad("chg_eligible", row.get("chg_eligible", "<absent>"), True)
    if row.get("pair_valid") is not True:
        bad("pair_valid", row.get("pair_valid", "<absent>"), True)
    si = _oracle_int(row.get("session_index"))
    if "expected_prev_session_index" in row and si is not None \
            and _oracle_int(row.get("expected_prev_session_index")) != si - 1:
        bad("expected_prev_session_index", row.get("expected_prev_session_index"), si - 1)
    for key in ("chg_hist", "chg_sum", "ctl_hist", "ctl_sum"):
        if key not in row:
            bad(key, "<absent>", "present")
    return out


def _oracle_measured_row(row, images, capture_path):
    si = _oracle_int(row.get("session_index"))
    prev = _oracle_int(row.get("prev_session_index"))
    tag = _oracle_int(row.get("mask_value"))
    tau = _oracle_int(row.get("tau_px"))
    contradictions = _oracle_contradictions(row)
    rec = {"session_index": si, "prev_session_index": prev, "mask_value": tag,
           "event": row.get("event"), "window_index": row.get("window_index"),
           "status": None, "reason": None, "fields": list(contradictions), "recomputed": {}, "ref": None,
           "contradictions": len(contradictions)}

    def unavailable(reason):
        rec["status"] = CHANGE_ORACLE_MISMATCH if contradictions else CHANGE_ORACLE_UNAVAILABLE
        rec["reason"] = reason
        return rec

    if si is None or prev is None:
        return unavailable("pair ids unreadable")
    if tau is None:
        return unavailable("tau_px unreadable")
    if not 0 <= tau <= 255:
        rec["fields"].append({"field": "tau_px", "published": tau, "recomputed": CHANGE_ORACLE_TAU_V1, "ok": False})
        rec["status"] = CHANGE_ORACLE_MISMATCH
        rec["reason"] = "tau_px outside 0..255, arithmetic not attempted"
        return rec
    if str(capture_path or "").lower() == "backbuffer":
        return unavailable("backbuffer grab point - no receipt, measurement unsupported in v1")
    frame_file = row.get("frame_file")
    expected_file = "Actual_Frames/frame_%05d.png" % si
    cur, why = images.colour(si)
    if cur is None:
        return unavailable("current colour: " + why)
    prev_img, why = images.colour(prev)
    if prev_img is None:
        return unavailable("predecessor colour: " + why)
    mask, why = images.mask(si)
    if mask is None:
        return unavailable("current mask: " + why)
    if not (cur.size == prev_img.size == mask.size):
        return unavailable("size differs across current %s / predecessor %s / mask %s"
                           % (cur.size, prev_img.size, mask.size))
    rect = (row.get("receipt") or {}).get("rect")
    if not (isinstance(rect, list) and len(rect) == 4 and all(_oracle_int(v) is not None for v in rect)):
        return unavailable("no receipt rect, so native delivery cannot be established")
    if (rect[2], rect[3]) != cur.size:
        return unavailable("resampled delivery - PNG %dx%d against receipt rect %dx%d"
                           % (cur.size[0], cur.size[1], rect[2], rect[3]))

    def ident(name, published, recomputed):
        rec["fields"].append({"field": name, "published": published, "recomputed": recomputed,
                              "ok": published == recomputed})

    ident("pair_ids", [prev, si], [si - 1, si])
    ident("tau_px", tau, CHANGE_ORACLE_TAU_V1)
    if frame_file is not None:
        ident("frame_file", frame_file, expected_file)
    ident("mask_value_nonzero", tag is not None and tag > 0, True)

    d = _oracle_d(cur, prev_img)
    target_region = _oracle_region(mask, tag if tag is not None else -1)
    control_region = _oracle_region(mask, 0)
    tstats = _oracle_stats(d.histogram(target_region), tau)
    cstats = _oracle_stats(d.histogram(control_region), tau)
    rec["recomputed"] = {"chg": tstats, "ctl": cstats}
    ident("denominators_positive", True, tstats["n"] > 0 and cstats["n"] > 0)
    _oracle_compare(rec, "chg", row, tstats)
    _oracle_compare(rec, "ctl", row, cstats)

    ref_si = _oracle_int(row.get("ref_session_index"))
    ref_claimed = row.get("ref_gt8") is not None or row.get("ref_mean") is not None
    if ref_claimed:
        ref = {"ref_session_index": ref_si, "status": None, "reason": None, "fields": []}
        rec["ref"] = ref
        ref_img, why = images.colour(ref_si) if ref_si is not None and ref_si >= 0 else (None, "no reference index published")
        if ref_img is None:
            ref["status"] = CHANGE_ORACLE_UNAVAILABLE
            ref["reason"] = "reference colour: " + why
        elif ref_img.size != cur.size:
            ref["status"] = CHANGE_ORACLE_UNAVAILABLE
            ref["reason"] = "reference size %s differs from current %s" % (ref_img.size, cur.size)
        else:
            rstats = _oracle_stats(_oracle_d(cur, ref_img).histogram(target_region), tau)
            rec["recomputed"]["ref"] = rstats
            sub = {"fields": []}
            gt_pub = row.get("ref_gt8")
            sub["fields"].append({"field": "ref_gt8", "published": gt_pub, "recomputed": rstats["gt"],
                                  "ok": _oracle_int(gt_pub) == rstats["gt"]})
            mean_pub = row.get("ref_mean")
            ok_mean = (rstats["mean"] is not None and not isinstance(mean_pub, bool)
                       and isinstance(mean_pub, (int, float))
                       and abs(float(mean_pub) - rstats["mean"]) <= CHANGE_ORACLE_MEAN_TOLERANCE + 1e-12)
            sub["fields"].append({"field": "ref_mean", "published": mean_pub,
                                  "recomputed": None if rstats["mean"] is None else round(rstats["mean"], 8),
                                  "ok": ok_mean})
            ref["fields"] = sub["fields"]
            ref["status"] = CHANGE_ORACLE_MATCH if all(f["ok"] for f in sub["fields"]) else CHANGE_ORACLE_MISMATCH
    rec["status"] = CHANGE_ORACLE_MATCH if all(f["ok"] for f in rec["fields"]) else CHANGE_ORACLE_MISMATCH
    return rec


def _oracle_empty_region_row(row, images):
    """Cross-read of an empty_region refusal: do the delivered images show a zero denominator?"""
    si = _oracle_int(row.get("session_index"))
    tag = _oracle_int(row.get("mask_value"))
    if si is None:
        return "unverifiable", "session index unreadable"
    mask, why = images.mask(si)
    if mask is None:
        return "unverifiable", ("current mask: " + why + " (no mask PNG is written for an all-zero "
                                "mask, and none after a failed mask write)")
    hist = mask.histogram()
    target = hist[tag] if tag is not None and 0 < tag <= 255 else 0
    control = hist[0]
    if target == 0 or control == 0:
        return "agrees", "target %d / control %d" % (target, control)
    return "disagrees", "target %d / control %d are both positive" % (target, control)


def _oracle_read_sidecar(path):
    """Read the sidecar at the input boundary: every line is decoded, parsed and type-checked here.

    Returns (rows, uninterpretable) where rows is a list of (line_number, object) and uninterpretable
    a list of {line, why}. Raises OSError only when the file itself cannot be read.
    """
    with open(path, "rb") as fh:
        data = fh.read()
    rows = []
    bad = []
    for number, raw in enumerate(data.split(b"\n"), 1):
        if number == 1 and raw.startswith(b"\xef\xbb\xbf"):
            raw = raw[3:]
        if not raw.strip():
            continue
        try:
            text = raw.decode("utf-8")
        except UnicodeDecodeError:
            bad.append({"line": number, "why": "not UTF-8 text"})
            continue
        try:
            obj = json.loads(text)
        except (ValueError, RecursionError) as exc:
            bad.append({"line": number, "why": "malformed JSON (%s)" % str(exc).split(":")[0][:80]})
            continue
        if not isinstance(obj, dict):
            bad.append({"line": number, "why": "a JSON %s, not an object" % _oracle_json_type(obj)})
            continue
        rows.append((number, obj))
    return rows, bad


def _oracle_resolve(cap_dir):
    if os.path.isfile(os.path.join(cap_dir, CHANGE_ORACLE_SIDECAR)):
        return cap_dir, None
    found = []
    if os.path.isdir(cap_dir):
        for name in sorted(os.listdir(cap_dir)):
            sub = os.path.join(cap_dir, name)
            if os.path.isdir(sub) and os.path.isfile(os.path.join(sub, CHANGE_ORACLE_SIDECAR)):
                found.append(sub)
    if len(found) == 1:
        return found[0], "resolved to its only session folder %s" % os.path.basename(found[0])
    return None, ("no %s in %s%s" % (CHANGE_ORACLE_SIDECAR, cap_dir,
                                      "" if not found else " and %d session folders below it" % len(found)))


def change_oracle(cap_dir, quiet=False):
    """m55 Stage 3 oracle: recompute each measured pair row from the delivered PNGs on disk.

    Returns (code, lines, detail). code is 1 when any compared row or reference comparison mismatches
    (a contradictory measured row included) or any empty_region refusal is contradicted by its
    delivered mask; otherwise 3 when anything in the sidecar could not be interpreted or the oracle
    cannot run; otherwise 0 (including when nothing was compared - coverage is printed and a gate
    that needs rows treats coverage 0 as its own failure). It changes no other mode's exit code. The
    producer's chg_/ctl_/ref_ values are read only to be compared after the recomputation.
    """
    lines = ["CHANGE-ORACLE (m55 Stage 3)"]
    detail = {"session": cap_dir, "rows": [], "not_claimed": {}, "unavailable": {}, "uninterpretable": [],
              "empty_region": {"agrees": 0, "disagrees": 0, "unverifiable": 0, "rows": []},
              "mean_tolerance": CHANGE_ORACLE_MEAN_TOLERANCE}
    try:
        from PIL import Image
    except ImportError:
        lines.append("CHANGE-ORACLE: CANNOT RUN - Pillow is required")
        lines.append(CHANGE_ORACLE_SENTENCE)
        return 3, lines, detail
    session, note = _oracle_resolve(os.path.abspath(cap_dir))
    if session is None:
        lines.append("CHANGE-ORACLE: CANNOT RUN - %s" % note)
        lines.append(CHANGE_ORACLE_SENTENCE)
        return 3, lines, detail
    detail["session"] = session
    lines.append("  session                  %s" % session)
    if note:
        lines.append("  note                     %s" % note)
    try:
        numbered, uninterpretable = _oracle_read_sidecar(os.path.join(session, CHANGE_ORACLE_SIDECAR))
    except OSError as exc:
        lines.append("CHANGE-ORACLE: CANNOT RUN - sidecar unreadable (%s)" % exc.__class__.__name__)
        lines.append(CHANGE_ORACLE_SENTENCE)
        return 3, lines, detail
    bad_lines = len(uninterpretable)
    rows = [obj for _n, obj in numbered]
    capture_path = None
    try:
        with open(os.path.join(session, "run_summary.json"), "r", encoding="utf-8") as fh:
            capture_path = json.load(fh).get("capture_path")
    except (OSError, ValueError, AttributeError):
        capture_path = None
    if capture_path is not None and not isinstance(capture_path, str):
        capture_path = None
    images = _OracleImages(session)
    pairs = [(n, r) for n, r in numbered if r.get("kind") == "pair"]
    events = [r for _n, r in numbered if r.get("kind") == "event"]
    claimed = []
    claimed_all = 0
    bad_rows = 0

    def refuse(number, row, why):
        uninterpretable.append({"line": number, "session_index": row.get("session_index"), "why": why})

    for n, r in pairs:
        if "chg_measured" not in r:
            key = "stage-1 identity row (no measurement fields)"
            detail["not_claimed"][key] = detail["not_claimed"].get(key, 0) + 1
        elif not isinstance(r.get("chg_measured"), bool):
            refuse(n, r, "chg_measured is a JSON %s, a boolean is required" % _oracle_json_type(r.get("chg_measured")))
            bad_rows += 1
        elif r.get("chg_measured") is True:
            claimed_all += 1
            why = _oracle_uninterpretable_row(r)
            if why:
                refuse(n, r, why)
                bad_rows += 1
            else:
                claimed.append((n, r))
        else:
            key = "refused: %s" % r.get("reason")
            detail["not_claimed"][key] = detail["not_claimed"].get(key, 0) + 1
            if r.get("reason") == "empty_region":
                try:
                    verdict, why = _oracle_empty_region_row(r, images)
                except Exception as exc:
                    verdict, why = "unverifiable", "cross-read failed (%s)" % exc.__class__.__name__
                detail["empty_region"][verdict] += 1
                detail["empty_region"]["rows"].append({"line": n, "session_index": r.get("session_index"),
                                                       "mask_value": r.get("mask_value"),
                                                       "reading": verdict, "why": why})
    claimed.sort(key=lambda nr: (_oracle_int(nr[1].get("session_index")) or 0, str(nr[1].get("event")),
                                 _oracle_int(nr[1].get("mask_value")) or 0))
    counts = {CHANGE_ORACLE_MATCH: 0, CHANGE_ORACLE_MISMATCH: 0, CHANGE_ORACLE_UNAVAILABLE: 0}
    ref_counts = {"claimed": 0, CHANGE_ORACLE_MATCH: 0, CHANGE_ORACLE_MISMATCH: 0, CHANGE_ORACLE_UNAVAILABLE: 0}
    ref_unavailable = {}
    contradicted = 0
    for n, r in claimed:
        try:
            rec = _oracle_measured_row(r, images, capture_path)
        except Exception as exc:
            refuse(n, r, "the recomputation could not process this row (%s: %s)"
                   % (exc.__class__.__name__, str(exc)[:80]))
            bad_rows += 1
            continue
        rec["line"] = n
        contradicted += 1 if rec.get("contradictions") else 0
        detail["rows"].append(rec)
        counts[rec["status"]] += 1
        if rec["status"] == CHANGE_ORACLE_UNAVAILABLE:
            detail["unavailable"][rec["reason"]] = detail["unavailable"].get(rec["reason"], 0) + 1
        if rec["ref"] is not None:
            ref_counts["claimed"] += 1
            ref_counts[rec["ref"]["status"]] += 1
            if rec["ref"]["status"] == CHANGE_ORACLE_UNAVAILABLE:
                ref_unavailable[rec["ref"]["reason"]] = ref_unavailable.get(rec["ref"]["reason"], 0) + 1
        elif r.get("ref_gt8") is not None and rec["status"] == CHANGE_ORACLE_UNAVAILABLE:
            ref_counts["claimed"] += 1
            ref_counts[CHANGE_ORACLE_UNAVAILABLE] += 1
            ref_unavailable["pair unavailable"] = ref_unavailable.get("pair unavailable", 0) + 1
    compared = counts[CHANGE_ORACLE_MATCH] + counts[CHANGE_ORACLE_MISMATCH]
    er = detail["empty_region"]
    detail["uninterpretable"] = sorted(uninterpretable, key=lambda u: u["line"])
    detail["summary"] = {
        "sidecar_rows": len(rows), "unparseable_lines": bad_lines, "pair_rows": len(pairs),
        "event_rows": len(events), "claimed": claimed_all, "compared": compared,
        "matched": counts[CHANGE_ORACLE_MATCH], "mismatched": counts[CHANGE_ORACLE_MISMATCH],
        "unavailable": counts[CHANGE_ORACLE_UNAVAILABLE], "contradicted": contradicted,
        "ref_claimed": ref_counts["claimed"], "ref_compared": ref_counts[CHANGE_ORACLE_MATCH] + ref_counts[CHANGE_ORACLE_MISMATCH],
        "ref_matched": ref_counts[CHANGE_ORACLE_MATCH], "ref_mismatched": ref_counts[CHANGE_ORACLE_MISMATCH],
        "ref_unavailable": ref_counts[CHANGE_ORACLE_UNAVAILABLE],
        "empty_region_agrees": er["agrees"], "empty_region_disagrees": er["disagrees"],
        "empty_region_unverifiable": er["unverifiable"],
        "uninterpretable": len(uninterpretable), "uninterpretable_lines": bad_lines, "uninterpretable_rows": bad_rows,
        "capture_path": capture_path}
    detail["ref_unavailable"] = ref_unavailable
    s = detail["summary"]
    lines.append("  sidecar                  %d rows (%d pair, %d event, %d unparseable)"
                 % (len(rows), len(pairs), len(events), bad_lines))
    lines.append("  uninterpretable          %d  (%d line(s) that are not a JSON object, %d row(s) with required "
                 "fields of the wrong type)" % (s["uninterpretable"], bad_lines, bad_rows))
    for item in detail["uninterpretable"]:
        lines.append("    UNINTERPRETABLE line %d%s: %s"
                     % (item["line"], "" if item.get("session_index") is None else " si=%s" % item["session_index"],
                        item["why"]))
    lines.append("  rows claimed             %d  (pair rows with chg_measured: true)" % s["claimed"])
    lines.append("  rows compared            %d" % s["compared"])
    lines.append("    matched                %d" % s["matched"])
    lines.append("    mismatched             %d" % s["mismatched"])
    lines.append("      contradictory rows   %d  (measured rows carrying a refusal field or missing a statistic)"
                 % s["contradicted"])
    lines.append("  rows unavailable         %d" % s["unavailable"])
    for reason in sorted(detail["unavailable"]):
        lines.append("    %-4d %s" % (detail["unavailable"][reason], reason))
    lines.append("  reference comparisons    claimed %d  compared %d  matched %d  mismatched %d  unavailable %d"
                 % (s["ref_claimed"], s["ref_compared"], s["ref_matched"], s["ref_mismatched"], s["ref_unavailable"]))
    for reason in sorted(ref_unavailable):
        lines.append("    %-4d %s" % (ref_unavailable[reason], reason))
    lines.append("  pair rows not claimed    %d" % sum(detail["not_claimed"].values()))
    for reason in sorted(detail["not_claimed"]):
        lines.append("    %-4d %s" % (detail["not_claimed"][reason], reason))
    if er["agrees"] or er["disagrees"] or er["unverifiable"]:
        lines.append("  empty_region cross-read  agrees %d  disagrees %d  unverifiable %d"
                     % (er["agrees"], er["disagrees"], er["unverifiable"]))
        for item in er["rows"]:
            if item["reading"] == "disagrees":
                lines.append("    %s DISAGREES si=%s tag=%s %s - a refusal the delivered mask contradicts"
                             % (CHANGE_ORACLE_MISMATCH, item["session_index"], item["mask_value"], item["why"]))
    lines.append("  compared                 count against count exactly; mean against mean within %.5f "
                 "(the producer publishes sum/n/255 rounded to 4 dp)" % CHANGE_ORACLE_MEAN_TOLERANCE)
    for rec in detail["rows"]:
        bad = [f for f in rec["fields"] if not f["ok"]]
        ref_bad = [f for f in (rec["ref"] or {}).get("fields", []) if not f["ok"]]
        loud = rec["status"] == CHANGE_ORACLE_MISMATCH or bad or ref_bad
        if quiet and not loud:
            continue
        head = "  si=%-5s prev=%-5s tag=%-3s w=%s %s" % (rec["session_index"], rec["prev_session_index"],
                                                       rec["mask_value"], rec["window_index"], rec["event"])
        if rec["status"] == CHANGE_ORACLE_UNAVAILABLE:
            lines.append("%s  %s (%s)" % (head, rec["status"], rec["reason"]))
            continue
        if rec["reason"]:
            head += "  [arithmetic unavailable: %s]" % rec["reason"]
        t = rec["recomputed"].get("chg", {})
        c = rec["recomputed"].get("ctl", {})
        ref_note = ""
        if rec["ref"] is not None:
            ref_note = "  ref %s" % rec["ref"]["status"]
            if rec["ref"]["status"] == CHANGE_ORACLE_UNAVAILABLE:
                ref_note += " (%s)" % rec["ref"]["reason"]
        lines.append("%s  %s  chg n=%s gt8=%s  ctl n=%s gt8=%s%s"
                     % (head, rec["status"], t.get("n"), t.get("gt"), c.get("n"), c.get("gt"), ref_note))
        for f in bad + ref_bad:
            lines.append("      %s %s published=%s recomputed=%s"
                         % (CHANGE_ORACLE_MISMATCH, f["field"], json.dumps(f["published"]), json.dumps(f["recomputed"])))
    lines.append("  SUMMARY                  claimed %d  compared %d  matched %d  mismatched %d  unavailable %d  "
                 "ref_mismatched %d  empty_region_disagrees %d  uninterpretable %d"
                 % (s["claimed"], s["compared"], s["matched"], s["mismatched"], s["unavailable"],
                    s["ref_mismatched"], s["empty_region_disagrees"], s["uninterpretable"]))
    code = _oracle_exit_code(s)
    detail["exit"] = code
    lines.append("  EXIT %d  %s" % (code, {1: "a comparison mismatched or a refusal was contradicted (this outranks "
                                           "uninterpretable input)",
                                        3: "part of the sidecar could not be interpreted; nothing readable mismatched",
                                        0: "every comparison matched and every line was interpretable"}[code]))
    lines.append(CHANGE_ORACLE_SENTENCE)
    return code, lines, detail


def _oracle_exit_code(s):
    if s.get("mismatched") or s.get("ref_mismatched") or s.get("empty_region_disagrees"):
        return 1
    if s.get("uninterpretable"):
        return 3
    return 0


ORACLE_FIXTURE_W = 24
ORACLE_FIXTURE_H = 16
ORACLE_TAG_A = 200
ORACLE_TAG_B = 201


def _oracle_fixture_stats(cur, prev, mask, select):
    """Stand-in producer for the selftest: a per-pixel loop, deliberately not the oracle's code path."""
    import math

    def flat(img):
        getter = getattr(img, "get_flattened_data", None)
        return list(getter() if getter else img.getdata())
    cd, pd, md = flat(cur), flat(prev), flat(mask)
    n = gt = total = 0
    hist = [0] * 8
    for i, m in enumerate(md):
        if not select(i, m):
            continue
        a, b = cd[i], pd[i]
        d = max(abs(a[0] - b[0]), abs(a[1] - b[1]), abs(a[2] - b[2]))
        n += 1
        total += d
        gt += 1 if d > 8 else 0
        hist[0 if d == 0 else 1 if d <= 2 else 2 if d <= 4 else 3 if d <= 8 else 4 if d <= 16
             else 5 if d <= 32 else 6 if d <= 64 else 7] += 1
    mean = math.floor(total / n / 255.0 * 10000.0 + 0.5) / 10000.0 if n else None
    return {"n": n, "gt8": gt, "sum": total, "hist": hist, "mean": mean}


def _oracle_fixture_row(frames, masks, si, prev, tag, window, ref=None, event="solid_swap@3",
                        target_select=None, control_select=None):
    cur, pv, mk = frames[si], frames[prev], masks[si]
    t = _oracle_fixture_stats(cur, pv, mk, target_select or (lambda _i, m: m == tag))
    c = _oracle_fixture_stats(cur, pv, mk, control_select or (lambda _i, m: m == 0))
    row = {"kind": "pair", "stage_version": 2, "session_index": si, "prev_session_index": prev,
           "expected_prev_session_index": si - 1, "frame_file": "Actual_Frames/frame_%05d.png" % si,
           "pair_valid": True, "reason": None,
           "receipt": {"rect": [0, 0, cur.size[0], cur.size[1]], "extent": [cur.size[0], cur.size[1]]},
           "event": event, "phase_ordinal": 0, "window_index": window, "mask_value": tag,
           "chg_measured": True, "chg_eligible": True, "tau_px": 8,
           "chg_n": t["n"], "chg_gt8": t["gt8"], "chg_sum": t["sum"], "chg_hist": t["hist"], "chg_mean": t["mean"],
           "ctl_n": c["n"], "ctl_gt8": c["gt8"], "ctl_sum": c["sum"], "ctl_hist": c["hist"], "ctl_mean": c["mean"],
           "ref_session_index": -1 if ref is None else ref, "ref_gt8": None, "ref_mean": None,
           "prev_target_pixels": -1}
    if ref is not None:
        r = _oracle_fixture_stats(cur, frames[ref], mk, target_select or (lambda _i, m: m == tag))
        row["ref_gt8"] = r["gt8"]
        row["ref_mean"] = r["mean"]
    return row


def _oracle_fixture_mask(full_tag=None):
    from PIL import Image
    w, h = ORACLE_FIXTURE_W, ORACLE_FIXTURE_H
    if full_tag is not None:
        return Image.new("L", (w, h), full_tag)
    m = Image.new("L", (w, h), 0)
    for y in range(4, 10):
        for x in range(4, 12):
            m.putpixel((x, y), ORACLE_TAG_A)
    for y in range(10, 14):
        for x in range(16, 20):
            m.putpixel((x, y), ORACLE_TAG_B)
    return m


def _oracle_fixture_frame(k, target_rgb=None, control_jitter=True):
    """Deterministic textured background, the tag-A rectangle painted target_rgb when given,
    and a few control pixels that move by 1, 5, 20 and 100 so every control bin can fill."""
    from PIL import Image
    w, h = ORACLE_FIXTURE_W, ORACLE_FIXTURE_H
    im = Image.new("RGB", (w, h))
    for y in range(h):
        for x in range(w):
            im.putpixel((x, y), ((x * 7 + y * 13) % 200 + 20, (x * 11 + y * 3) % 180 + 30, (x * 5 + y * 17) % 160 + 40))
    if target_rgb is not None:
        for y in range(4, 10):
            for x in range(4, 12):
                im.putpixel((x, y), target_rgb)
    if control_jitter:
        for j, (x, y, step) in enumerate(((0, 0, 1), (1, 0, 5), (2, 0, 20), (3, 0, 100), (22, 15, 7))):
            base = im.getpixel((x, y))
            im.putpixel((x, y), ((base[0] + step * (k % 3)) % 256, base[1], base[2]))
    return im


def _oracle_write_session(root, name, frames, masks, rows, run_summary=None, jpeg=(), mask_mode=None):
    import shutil
    d = os.path.join(root, name)
    if os.path.isdir(d):
        shutil.rmtree(d)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    os.makedirs(os.path.join(d, "target_mask"))
    for si, im in frames.items():
        if si in jpeg:
            im.save(os.path.join(d, "Actual_Frames", "frame_%05d.jpg" % si), "JPEG", quality=100)
        else:
            im.save(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si))
    for si, m in masks.items():
        if m is None:
            continue
        out = m.convert(mask_mode) if mask_mode else m
        out.save(os.path.join(d, "target_mask", "frame_%05d.png" % si))
    with open(os.path.join(d, CHANGE_ORACLE_SIDECAR), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
        json.dump(run_summary if run_summary is not None else {"capture_path": "sve"}, fh)
    return d


def _oracle_baseline(root, name="baseline", mutate=None, **write_kw):
    """Onset at si 3 on tag A; window pairs (2,3) (3,4) (4,5) (5,6), all against the si-2 reference.
    Target moves by 2, 10 and 40 inside the window so adjacent pairs land in different bins."""
    frames = {0: _oracle_fixture_frame(0), 1: _oracle_fixture_frame(1), 2: _oracle_fixture_frame(2),
              3: _oracle_fixture_frame(3, (250, 10, 240)), 4: _oracle_fixture_frame(4, (248, 10, 240)),
              5: _oracle_fixture_frame(5, (238, 10, 240)), 6: _oracle_fixture_frame(6, (198, 10, 240))}
    masks = {si: (_oracle_fixture_mask() if si >= 3 else None) for si in frames}
    rows = [_oracle_fixture_row(frames, masks, si, si - 1, ORACLE_TAG_A, si - 3, ref=2) for si in (3, 4, 5, 6)]
    if mutate:
        mutate(frames, masks, rows)
    return _oracle_write_session(root, name, frames, masks, rows, **write_kw)


def _oracle_row_of(detail, si):
    return next((r for r in detail["rows"] if r["session_index"] == si), None)


def _oracle_bad_fields(rec):
    return [f["field"] for f in rec["fields"] if not f["ok"]] + \
        [f["field"] for f in (rec.get("ref") or {}).get("fields", []) if not f["ok"]]


def _change_oracle_selftest():
    """Prove the oracle can agree, can disagree, and refuses what it cannot read (G96).

    Synthetic 24x16 sessions. Their sidecar numbers come from a per-pixel loop that stands in for
    the producer, so agreement on the clean fixtures is two independent computations agreeing, and
    each must-fail mutation has to be caught by the oracle's own recomputation.
    """
    import shutil
    import tempfile
    try:
        from PIL import Image
    except ImportError:
        print("SELFTEST: ERROR - Pillow is required.", flush=True)
        return 2
    root = tempfile.mkdtemp(prefix="m55_oracle_selftest_")
    checks = []

    def run(path):
        return change_oracle(path, quiet=True)

    def record(name, ok, outcome):
        checks.append(bool(ok))
        _emit("  %-40s %-44s %s" % (name, outcome, "OK" if ok else "FAIL"))

    try:
        base = _oracle_baseline(root)
        code, lines, det = run(base)
        s = det.get("summary", {})
        record("baseline_all_match", code == 0 and s.get("claimed") == 4 and s.get("matched") == 4
               and s.get("mismatched") == 0 and s.get("ref_matched") == 4 and CHANGE_ORACLE_SENTENCE in lines,
               "claimed %s matched %s ref %s" % (s.get("claimed"), s.get("matched"), s.get("ref_matched")))
        rec = _oracle_row_of(det, 3)
        hist_bins = rec["recomputed"]["ctl"]["hist"] if rec else []
        record("baseline_control_bins_exercised", rec is not None and sum(1 for v in hist_bins if v) >= 3,
               "ctl hist %s" % hist_bins)

        def garbage(frames, masks, rows):
            for r in rows:
                for key in list(r.keys()):
                    if key.startswith(("chg_", "ctl_", "ref_")) and key not in ("chg_measured", "chg_eligible", "ref_session_index"):
                        v = r[key]
                        r[key] = [x + 7 for x in v] if isinstance(v, list) else (v * 3 + 1 if isinstance(v, (int, float)) and not isinstance(v, bool) else v)
        g = _oracle_baseline(root, "independence", mutate=garbage)
        _c, _l, gdet = run(g)
        same = all(_oracle_row_of(gdet, si)["recomputed"] == _oracle_row_of(det, si)["recomputed"] for si in (3, 4, 5, 6))
        record("independence_recompute_ignores_published", same and gdet["summary"]["mismatched"] == 4,
               "recomputed identical, mismatched %d" % gdet["summary"]["mismatched"])

        walk = {k: Image.new("RGB", (ORACLE_FIXTURE_W, ORACLE_FIXTURE_H), (40, 90, 140)) for k in range(6)}
        for k, level in ((1, 0), (2, 3), (3, 6), (4, 9), (5, 12)):
            for y in range(4, 10):
                for x in range(4, 12):
                    walk[k].putpixel((x, y), (level, level, level))
        gmasks = {k: (_oracle_fixture_mask() if k >= 2 else None) for k in walk}
        grows = [_oracle_fixture_row(walk, gmasks, si, si - 1, ORACLE_TAG_A, si - 2, ref=1, event="grad@2")
                 for si in (2, 3, 4, 5)]
        gpath = _oracle_write_session(root, "grad_0_3_6_9_12", walk, gmasks, grows)
        _c, _l, gd = run(gpath)
        adj = [_oracle_row_of(gd, si)["recomputed"]["chg"]["gt"] for si in (2, 3, 4, 5)]
        refs = [_oracle_row_of(gd, si)["recomputed"]["ref"]["gt"] for si in (2, 3, 4, 5)]
        n_target = _oracle_row_of(gd, 5)["recomputed"]["chg"]["n"]
        record("G-GRAD(b)_adjacent_zero_reference_full", adj == [0, 0, 0, 0] and refs == [0, 0, 48, 48]
               and refs[-1] == n_target and gd["summary"]["matched"] == 4 and gd["summary"]["ref_matched"] == 4,
               "adjacent %s reference %s n %d" % (adj, refs, n_target))

        tie = {0: Image.new("RGB", (ORACLE_FIXTURE_W, ORACLE_FIXTURE_H), (60, 60, 60))}
        tie[1] = tie[0].copy()
        tie[2] = tie[0].copy()
        for y in range(4, 10):
            for x in range(4, 12):
                tie[1].putpixel((x, y), (68, 60, 60))
                tie[2].putpixel((x, y), (77, 60, 60))
        tmasks = {k: (_oracle_fixture_mask() if k >= 1 else None) for k in tie}
        trows = [_oracle_fixture_row(tie, tmasks, 1, 0, ORACLE_TAG_A, 0, event="tie@1"),
                 _oracle_fixture_row(tie, tmasks, 2, 1, ORACLE_TAG_A, 1, event="tie@1")]
        _c, _l, td = run(_oracle_write_session(root, "ties_8_9", tie, tmasks, trows))
        r8 = _oracle_row_of(td, 1)["recomputed"]["chg"]
        r9 = _oracle_row_of(td, 2)["recomputed"]["chg"]
        record("G-TIES_d8_not_counted", r8["gt"] == 0 and r8["hist"][3] == 48 and _oracle_row_of(td, 1)["status"] == CHANGE_ORACLE_MATCH,
               "d=8 gt8 %d bin[5-8] %d" % (r8["gt"], r8["hist"][3]))
        record("G-TIES_d9_counted", r9["gt"] == 48 and r9["hist"][4] == 48 and _oracle_row_of(td, 2)["status"] == CHANGE_ORACLE_MATCH,
               "d=9 gt8 %d bin[9-16] %d" % (r9["gt"], r9["hist"][4]))

        def a2b10_red(v10):
            word = (3 << 30) | (v10 & 0x3FF)
            return ((word >> 0) & 0x3FF) >> 2
        ten = {0: Image.new("RGB", (ORACLE_FIXTURE_W, ORACLE_FIXTURE_H), (a2b10_red(0), 50, 50))}
        ten[1] = ten[0].copy()
        for y in range(4, 10):
            for x in range(4, 12):
                ten[1].putpixel((x, y), (a2b10_red(35), 50, 50))
        tenm = {0: None, 1: _oracle_fixture_mask()}
        _c, _l, tend = run(_oracle_write_session(root, "ties_a2b10g10r10", ten, tenm,
                                                 [_oracle_fixture_row(ten, tenm, 1, 0, ORACLE_TAG_A, 0, event="ten@1")]))
        rt = _oracle_row_of(tend, 1)["recomputed"]["chg"]
        normalised = 35 / 1023.0 * 255.0
        record("G-TIES_a2b10g10r10_0_to_35_is_8", a2b10_red(35) == 8 and rt["gt"] == 0 and rt["hist"][3] == 48
               and normalised > 8 and tend["summary"]["matched"] == 1,
               "bytes %d->%d gt8 %d (normalised %.3f)" % (a2b10_red(0), a2b10_red(35), rt["gt"], normalised))

        full = {0: _oracle_fixture_frame(0), 1: _oracle_fixture_frame(1, (250, 10, 240))}
        fullm = {0: None, 1: _oracle_fixture_mask(full_tag=ORACLE_TAG_A)}
        claim = _oracle_fixture_row(full, fullm, 1, 0, ORACLE_TAG_A, 0, event="denom@1")
        _c, _l, dd = run(_oracle_write_session(root, "G-DENOM_control_empty", full, fullm, [claim]))
        rec = _oracle_row_of(dd, 1)
        record("G-DENOM_control_empty_claim_refused", rec["status"] == CHANGE_ORACLE_MISMATCH
               and "denominators_positive" in _oracle_bad_fields(rec) and rec["recomputed"]["ctl"]["n"] == 0,
               "ctl_n %d flagged %s" % (rec["recomputed"]["ctl"]["n"], _oracle_bad_fields(rec)[:2]))
        normal = {0: _oracle_fixture_frame(0), 1: _oracle_fixture_frame(1, (250, 10, 240))}
        normm = {0: None, 1: _oracle_fixture_mask()}
        absent = _oracle_fixture_row(normal, normm, 1, 0, 202, 0, event="denom@1")
        _c, _l, dd2 = run(_oracle_write_session(root, "G-DENOM_target_empty", normal, normm, [absent]))
        rec = _oracle_row_of(dd2, 1)
        record("G-DENOM_target_empty_claim_refused", rec["status"] == CHANGE_ORACLE_MISMATCH
               and "denominators_positive" in _oracle_bad_fields(rec) and rec["recomputed"]["chg"]["n"] == 0,
               "chg_n %d flagged %s" % (rec["recomputed"]["chg"]["n"], _oracle_bad_fields(rec)[:2]))
        refused = dict(claim, chg_measured=False, chg_eligible=False, pair_valid=False, reason="empty_region")
        c3, _l, dd3 = run(_oracle_write_session(root, "G-DENOM_refusal_agrees", full, fullm, [refused]))
        record("G-DENOM_empty_region_refusal_agrees", c3 == 0 and dd3["empty_region"]["agrees"] == 1
               and dd3["summary"]["claimed"] == 0, "exit %d agrees %d" % (c3, dd3["empty_region"]["agrees"]))
        wrong = dict(_oracle_fixture_row(normal, normm, 1, 0, ORACLE_TAG_A, 0, event="denom@1"),
                     chg_measured=False, chg_eligible=False, pair_valid=False, reason="empty_region")
        c4, _l, dd4 = run(_oracle_write_session(root, "G-DENOM_refusal_contradicted", normal, normm, [wrong]))
        record("G-DENOM_contradicted_empty_region_exits_1", c4 == 1 and dd4["empty_region"]["disagrees"] == 1
               and dd4["summary"]["empty_region_disagrees"] == 1 and dd4["summary"]["mismatched"] == 0,
               "exit %d disagrees %d" % (c4, dd4["empty_region"]["disagrees"]))
        c5, _l, dd5 = run(_oracle_write_session(root, "G-DENOM_refusal_no_png", normal, {0: None, 1: None}, [wrong]))
        record("G-DENOM_refusal_without_mask_png_unverifiable", c5 == 0 and dd5["empty_region"]["unverifiable"] == 1
               and dd5["summary"]["empty_region_disagrees"] == 0,
               "exit %d unverifiable %d" % (c5, dd5["empty_region"]["unverifiable"]))

        def contradicted(name, change, fields):
            def mutate(frames, masks, rows):
                change(rows[1])
            cc, _lc, cd = run(_oracle_baseline(root, "contra_" + name, mutate=mutate))
            rec_c = _oracle_row_of(cd, 4)
            bad_c = _oracle_bad_fields(rec_c) if rec_c else []
            ok = (cc == 1 and rec_c is not None and rec_c["status"] == CHANGE_ORACLE_MISMATCH
                  and all("measured_row:" + f in bad_c for f in fields) and cd["summary"]["contradicted"] == 1
                  and all(_oracle_row_of(cd, si)["status"] == CHANGE_ORACLE_MATCH for si in (3, 5, 6)))
            record("contradiction_" + name, ok, "exit %d %s" % (cc, ",".join(f.split(":")[-1] for f in bad_c)[:44]))

        contradicted("refusal_fields_on_measured_row",
                     lambda r: r.update(chg_eligible=False, pair_valid=False, reason="budget_exceeded",
                                        expected_prev_session_index=71),
                     ("reason", "chg_eligible", "pair_valid", "expected_prev_session_index"))
        contradicted("hist_and_sum_absent",
                     lambda r: [r.pop(k) for k in ("chg_hist", "chg_sum", "ctl_hist", "ctl_sum")],
                     ("chg_hist", "chg_sum", "ctl_hist", "ctl_sum"))

        def sidecar_lines(name, lines_out):
            path = _oracle_baseline(root, "input_" + name)
            with open(os.path.join(path, CHANGE_ORACLE_SIDECAR), "w", encoding="utf-8") as fh:
                fh.write("".join(x + "\n" for x in lines_out))
            return run(path)

        with open(os.path.join(base, CHANGE_ORACLE_SIDECAR), encoding="utf-8") as fh:
            good_lines = [x.rstrip("\n") for x in fh if x.strip()]
        ci, _li, di = sidecar_lines("malformed_only", ["{bad json"])
        record("input_malformed_only_exits_3", ci == 3 and di["summary"]["uninterpretable"] == 1
               and di["uninterpretable"][0]["line"] == 1, "exit %d uninterpretable %d" % (ci, di["summary"]["uninterpretable"]))
        ci, _li, di = sidecar_lines("valid_plus_malformed", good_lines + ["{bad json", "[1, 2]"])
        record("input_valid_plus_malformed_exits_3", ci == 3 and di["summary"]["matched"] == 4
               and di["summary"]["uninterpretable"] == 2 and [u["line"] for u in di["uninterpretable"]] == [5, 6],
               "exit %d matched %d lines %s" % (ci, di["summary"]["matched"], [u["line"] for u in di["uninterpretable"]]))
        wrong_line = json.loads(good_lines[2])
        wrong_line["chg_gt8"] += 1
        ci, _li, di = sidecar_lines("mismatch_plus_malformed", good_lines[:2] + [json.dumps(wrong_line)] + good_lines[3:]
                                    + ["{bad json"])
        record("input_mismatch_plus_malformed_exits_1", ci == 1 and di["summary"]["mismatched"] == 1
               and di["summary"]["uninterpretable"] == 1, "exit %d mismatched %d uninterpretable %d"
               % (ci, di["summary"]["mismatched"], di["summary"]["uninterpretable"]))
        for label, receipt in (("list_with_item", [1]), ("empty_list", [])):
            bad_receipt = json.loads(good_lines[1])
            bad_receipt["receipt"] = receipt
            ci, _li, di = sidecar_lines("receipt_" + label, [good_lines[0], json.dumps(bad_receipt)] + good_lines[2:])
            record("input_receipt_%s_exits_3" % label, ci == 3 and di["summary"]["uninterpretable"] == 1
                   and di["uninterpretable"][0]["line"] == 2 and di["summary"]["matched"] == 3,
                   "exit %d %s" % (ci, di["uninterpretable"][0]["why"][:36] if di["uninterpretable"] else "-"))
        for label, key, value, want in (("tau_string", "tau_px", "8", 3), ("mask_value_null", "mask_value", None, 3),
                                        ("tau_out_of_range", "tau_px", 300, 1)):
            bad_scalar = json.loads(good_lines[1])
            bad_scalar[key] = value
            ci, _li, di = sidecar_lines(label, [good_lines[0], json.dumps(bad_scalar)] + good_lines[2:])
            record("input_%s_exits_%d" % (label, want), ci == want and di["summary"]["matched"] == 3
                   and (di["summary"]["uninterpretable"] == 1 if want == 3 else di["summary"]["mismatched"] == 1),
                   "exit %d uninterpretable %d mismatched %d" % (ci, di["summary"]["uninterpretable"], di["summary"]["mismatched"]))

        def expect_unavailable(name, path, fragment):
            _c2, _l2, ud = run(path)
            rec2 = _oracle_row_of(ud, 4)
            ok = (rec2 is not None and rec2["status"] == CHANGE_ORACLE_UNAVAILABLE and fragment in rec2["reason"]
                  and ud["summary"]["mismatched"] == 0)
            record(name, ok, "%s" % (rec2["reason"][:44] if rec2 else "row missing"))
        expect_unavailable("unsupported_jpeg", _oracle_baseline(root, "jpeg", jpeg=(4,)), "jpeg delivery")

        def resampled(frames, masks, rows):
            rows[1]["receipt"]["rect"] = [0, 0, 48, 32]
        expect_unavailable("unsupported_resampled", _oracle_baseline(root, "resampled", mutate=resampled), "resampled")
        _c, _l, bd = run(_oracle_baseline(root, "backbuffer", run_summary={"capture_path": "backbuffer"}))
        record("unsupported_backbuffer", bd["summary"]["unavailable"] == 4 and bd["summary"]["compared"] == 0,
               "unavailable %d" % bd["summary"]["unavailable"])

        def drop_mask(frames, masks, rows):
            masks[4] = None
        expect_unavailable("missing_mask_is_unavailable", _oracle_baseline(root, "nomask", mutate=drop_mask), "missing on disk")
        _c, _l, rgbd = run(_oracle_baseline(root, "rgbmask", mask_mode="RGB"))
        record("mask_not_grayscale_is_unavailable", rgbd["summary"]["unavailable"] == 4 and rgbd["summary"]["mismatched"] == 0,
               "unavailable %d" % rgbd["summary"]["unavailable"])

        def mutation(name, mutate, si, want_field):
            path = _oracle_baseline(root, "mut_" + name, mutate=mutate)
            c3, _l3, md = run(path)
            rec3 = _oracle_row_of(md, si)
            others = [r for r in md["rows"] if r["session_index"] != si]
            ok = (c3 == 1 and rec3 is not None and rec3["status"] == CHANGE_ORACLE_MISMATCH
                  and want_field in _oracle_bad_fields(rec3)
                  and all(r["status"] == CHANGE_ORACLE_MATCH for r in others))
            record("mutation_" + name, ok, "exit %d fired on %s" % (c3, ",".join(_oracle_bad_fields(rec3)[:3]) if rec3 else "-"))

        def off_by_one(frames, masks, rows):
            rows[2]["chg_gt8"] += 1
        mutation("count_off_by_one", off_by_one, 5, "chg_gt8")

        def swapped_predecessor(frames, masks, rows):
            swapped = _oracle_fixture_row(frames, masks, 4, 2, ORACLE_TAG_A, 1, ref=2)
            swapped["prev_session_index"] = 3
            swapped["expected_prev_session_index"] = 3
            rows[1] = swapped
        mutation("swapped_predecessor", swapped_predecessor, 4, "chg_gt8")

        def wrong_mask_value(frames, masks, rows):
            rows[1]["mask_value"] = ORACLE_TAG_B
        mutation("wrong_mask_value", wrong_mask_value, 4, "chg_n")

        def pair_id_shift(frames, masks, rows):
            shifted = dict(rows[1])
            shifted["session_index"] = 5
            shifted["prev_session_index"] = 4
            shifted["expected_prev_session_index"] = 4
            shifted["frame_file"] = "Actual_Frames/frame_00005.png"
            rows[:] = [rows[0], shifted, rows[3]]
        mutation("pair_id_shifted_by_one", pair_id_shift, 5, "chg_gt8")

        def control_includes_target(frames, masks, rows):
            stolen = 4 * ORACLE_FIXTURE_W + 4
            rows[0] = _oracle_fixture_row(frames, masks, 3, 2, ORACLE_TAG_A, 0, ref=2,
                                          control_select=lambda i, m: m == 0 or i == stolen)
        mutation("control_includes_target_pixel", control_includes_target, 3, "ctl_n")

        def ref_only(frames, masks, rows):
            rows[3]["ref_gt8"] += 1
        rc, _rl, rd = run(_oracle_baseline(root, "ref_only_mismatch", mutate=ref_only))
        record("exit_1_on_reference_mismatch_only", rc == 1 and rd["summary"]["mismatched"] == 0
               and rd["summary"]["ref_mismatched"] == 1, "exit %d rows %d ref %d"
               % (rc, rd["summary"]["mismatched"], rd["summary"]["ref_mismatched"]))
        nc, _nl, nd = run(_oracle_baseline(root, "nothing_compared", run_summary={"capture_path": "backbuffer"}))
        record("exit_0_when_nothing_compared", nc == 0 and nd["summary"]["compared"] == 0,
               "exit %d compared %d" % (nc, nd["summary"]["compared"]))

        empty = os.path.join(root, "empty_dir")
        os.makedirs(empty)
        code, lines, _d = run(empty)
        record("cannot_run_exit_3_sentence_printed", code == 3 and CHANGE_ORACLE_SENTENCE in lines,
               "exit %d" % code)
    except Exception as exc:
        import traceback
        traceback.print_exc()
        checks.append(False)
        _emit("SELFTEST: BROKEN - the selftest raised %s" % exc.__class__.__name__)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    if all(checks) and checks:
        _emit("SELFTEST: OK - %d change-oracle cases; it agrees with an independent computation, catches "
              "every must-fail mutation and refuses what it cannot read." % len(checks))
        return 0
    _emit("SELFTEST: BROKEN - %d of %d change-oracle cases failed." % (checks.count(False), len(checks)))
    return 2


def _emit(line):
    try:
        print(line, flush=True)
    except UnicodeEncodeError:
        sys.stdout.flush()
        sys.stdout.buffer.write((line + "\n").encode("utf-8"))
        sys.stdout.flush()


LABEL_RULE_REASONS = ("temporal_aa", "hide_return", "partial", "camera_clipping_unconfirmed", "unresolved",
                      "effect_interrupted", "nanite_unmaskable")
LABEL_RULE_FIRE_WINDOW_TYPES = ("missing_texture", "corrupted_texture", "uv_corruption", "normal_corruption",
                                "lighting_mismatch", "lod_corruption", "null_effect", "solid_swap", "time_dilation")
LABEL_RULE_REASON_TYPES = {
    "temporal_aa": ("stuck_low_mip",),
    "hide_return": ("blinking", "missing_object"),
    "partial": ("stuck_low_mip",),
    "camera_clipping_unconfirmed": ("camera_clipping",),
    "unresolved": ("stuck_low_mip",),
    "effect_interrupted": LABEL_RULE_FIRE_WINDOW_TYPES,
    "nanite_unmaskable": tuple(t for t in LABEL_RULE_FIRE_WINDOW_TYPES if t != "time_dilation")
    + ("blinking", "missing_object", "stuck_low_mip", "lod_popping"),
}
LABEL_RULE_LABELLED_ONLY = ("partial", "camera_clipping_unconfirmed", "unresolved")
LABEL_RULE_UNLABELLED_ONLY = ("effect_interrupted", "nanite_unmaskable")
LABEL_RULE_TEMPORAL_ONLY = ("temporal_aa", "hide_return")
RULE_NEW = "NEW"
RULE_OLD = "OLD"
RULE_NONE = "NONE"
RULE_SHOT = "LEGACY_SHOT"
LABEL_RULE_CHECKS = ("VP-MISMATCH", "TRANSITION-PRESENT-MISMATCH", "REASON-MISSING", "REASON-WITHOUT-TRANSITION",
                     "REASON-UNKNOWN", "REASON-MISPLACED", "REASON-ON-UNLABELLED", "REASON-ON-LABELLED",
                     "REASON-WITHOUT-TEMPORAL-AA",
                     "LABELLED-EXTRA", "LABELLED-MISSING", "ENTRY-MISSING", "FRAME-MISSING")


def _lr_read_rows(cap_dir):
    path = os.path.join(cap_dir, "labels.jsonl")
    if not os.path.isfile(path):
        return None, "no labels.jsonl"
    rows = []
    try:
        with open(path, "r", encoding="utf-8") as fh:
            for n, line in enumerate(fh, 1):
                if not line.strip():
                    continue
                try:
                    rec = json.loads(line)
                except ValueError:
                    return None, "labels.jsonl line %d is not JSON" % n
                if not isinstance(rec, dict) or not isinstance(rec.get("session_index"), int):
                    return None, "labels.jsonl line %d has no integer session_index" % n
                rows.append(rec)
    except (OSError, UnicodeDecodeError) as exc:
        return None, "labels.jsonl unreadable (%s)" % exc.__class__.__name__
    rows.sort(key=lambda r: r["session_index"])
    return rows, None


def _lr_read_events(cap_dir):
    path = os.path.join(cap_dir, "annotation.json")
    if not os.path.isfile(path):
        return None, "no annotation.json"
    try:
        with open(path, "r", encoding="utf-8") as fh:
            ann = json.load(fh)
    except (OSError, ValueError, UnicodeDecodeError) as exc:
        return None, "annotation.json unreadable (%s)" % exc.__class__.__name__
    out = []
    for ev in ann.get("anomalies", []) or []:
        lst = ev.get("injected_frames")
        which = "injected_frames"
        if not isinstance(lst, dict):
            lst = ev.get("affected_frames", {}) or {}
            which = "affected_frames"
        nodes = (ev.get("affected_objects", {}) or {}).get("nodes", []) or []
        out.append({"type": ev.get("anomaly_type", ""), "names": set(n.get("name", "") for n in nodes),
                    "frames": set(lst.get("frame_indices", []) or []), "list": which,
                    "manifested": bool(ev.get("manifested", True))})
    return out, None


def _lr_first(sis, n=8):
    s = sorted(set(sis))
    return ", ".join(str(x) for x in s[:n]) + (" ..." if len(s) > n else "")


def label_rule_check(cap_dir, quiet=False):
    """Read one session's per-frame label fields against the rule its own build wrote them under.

    A session whose anomaly entries carry `labelled` (084-07 and later) is read under the NEW rule:
    visible_positive = anomaly_present AND at least one labelled entry with a valid box, and every labelled
    entry must sit on a frame its event lists in annotation.json (injected_frames), and every listed frame must
    carry a labelled entry. A session with no `labelled` anywhere is read under the OLD rule and says so:
    visible_positive = anomaly_present AND an active entry with a valid box, which does NOT say the effect is in the
    picture; for such a session the per-frame truth is annotation.json's frame lists. A session that mixes the two
    is refused. Transition reasons are read on both: transition_present must equal "an entry carries transition",
    every flagged entry must name a known reason, each reason may appear only on the anomaly that produces it,
    partial and camera_clipping_unconfirmed only on labelled entries (NEW rule), and temporal_aa / hide_return only
    when run_summary says the run used temporal anti-aliasing. Exit 0 no mismatch, 1 mismatch, 3 cannot run.
    """
    lines = ["LABEL-RULE (084-07b): anomaly_present / labelled / visible_positive / transition, read from the session"]
    rows, err = _lr_read_rows(cap_dir)
    if rows is None:
        lines.append("LABEL-RULE: CANNOT RUN - %s" % err)
        return 3, lines, {"rule": None}
    shot_rows = sum(1 for r in rows if r.get("label_rule") == "legacy_shot")
    if 0 < shot_rows < len(rows):
        lines.append("LABEL-RULE: CANNOT RUN - %d of %d rows carry label_rule legacy_shot; an IAI.Capture.Shot row cannot "
                     "share a file with capture-run rows" % (shot_rows, len(rows)))
        return 3, lines, {"rule": None}
    events, err = _lr_read_events(cap_dir)
    if events is None and shot_rows and err == "no annotation.json":
        events = []
    if events is None:
        lines.append("LABEL-RULE: CANNOT RUN - %s" % err)
        return 3, lines, {"rule": None}
    rs = load_run_summary(cap_dir)
    entries = [a for r in rows for a in (r.get("anomalies") or [])]
    with_key = sum(1 for a in entries if isinstance(a, dict) and "labelled" in a)
    if entries and 0 < with_key < len(entries):
        lines.append("LABEL-RULE: CANNOT RUN - %d of %d entries carry `labelled`; one session cannot mix the two rules"
                     % (with_key, len(entries)))
        return 3, lines, {"rule": None}
    if shot_rows:
        rule = RULE_SHOT
    elif not entries:
        rule = RULE_NONE
    else:
        rule = RULE_NEW if with_key == len(entries) else RULE_OLD
    temporal = rs.get("label_temporal_aa")
    vetoed = int(rs.get("vetoed_events", 0) or 0)

    fails = {}

    def fail(cat, si):
        fails.setdefault(cat, []).append(si)

    by_si = {}
    for r in rows:
        by_si[r["session_index"]] = r
    n_present = 0
    n_vp_written = 0
    n_vp_rule = 0
    n_active_unlabelled = 0
    n_old_vp_unlisted = 0
    n_transition_rows = 0
    n_vetoed_labelled = 0
    n_legacy_unreasoned = 0
    reason_counts = {}
    per_type = {}

    def listed(entry, si):
        t = client_type(entry.get("id", ""))
        tgt = entry.get("target_name", "")
        return any(e["type"] == t and tgt in e["names"] and si in e["frames"] for e in events)

    for r in rows:
        si = r["session_index"]
        ents = [a for a in (r.get("anomalies") or []) if isinstance(a, dict)]
        present = r.get("anomaly_present") is True
        written = r.get("visible_positive") is True
        n_present += 1 if present else 0
        n_vp_written += 1 if written else 0
        if rule == RULE_NEW:
            rule_vp = present and any(a.get("labelled") is True and a.get("bbox_valid") is True for a in ents)
            if written != rule_vp:
                fail("VP-MISMATCH", si)
            if present and not any(a.get("labelled") is True for a in ents):
                n_active_unlabelled += 1
        else:
            strict = present and any(a.get("bbox_valid") is True and not a.get("transition") for a in ents)
            loose = present and any(a.get("bbox_valid") is True for a in ents)
            rule_vp = written if written in (strict, loose) else loose
            if written not in (strict, loose):
                fail("VP-MISMATCH", si)
            if written and not any(listed(a, si) for a in ents):
                n_old_vp_unlisted += 1
        n_vp_rule += 1 if rule_vp else 0
        for a in ents:
            pt = per_type.setdefault(a.get("id", ""), [0, 0, 0])
            pt[0] += 1
            if a.get("labelled") is True:
                pt[1] += 1
            if rule_vp and a.get("labelled") is True and a.get("bbox_valid") is True:
                pt[2] += 1
        flagged = [a for a in ents if a.get("transition")]
        if flagged:
            n_transition_rows += 1
        if (r.get("transition_present") is True) != bool(flagged):
            fail("TRANSITION-PRESENT-MISMATCH", si)
        for a in ents:
            has_t = bool(a.get("transition"))
            reasons = a.get("transition_reason")
            if has_t and not reasons:
                if rule == RULE_NEW:
                    fail("REASON-MISSING", si)
                else:
                    n_legacy_unreasoned += 1
                continue
            if reasons and not has_t:
                fail("REASON-WITHOUT-TRANSITION", si)
            for reason in reasons or []:
                if reason not in LABEL_RULE_REASONS:
                    fail("REASON-UNKNOWN", si)
                    continue
                key = (a.get("id", ""), reason)
                reason_counts[key] = reason_counts.get(key, 0) + 1
                if a.get("id", "") not in LABEL_RULE_REASON_TYPES[reason]:
                    fail("REASON-MISPLACED", si)
                if rule == RULE_NEW and reason in LABEL_RULE_LABELLED_ONLY and a.get("labelled") is not True:
                    fail("REASON-ON-UNLABELLED", si)
                if reason in LABEL_RULE_UNLABELLED_ONLY and a.get("labelled") is True:
                    fail("REASON-ON-LABELLED", si)
                if reason in LABEL_RULE_TEMPORAL_ONLY and temporal is False:
                    fail("REASON-WITHOUT-TEMPORAL-AA", si)
        if rule == RULE_NEW:
            for a in ents:
                if a.get("labelled") is True and not listed(a, si):
                    if vetoed > 0:
                        n_vetoed_labelled += 1
                    else:
                        fail("LABELLED-EXTRA", si)

    for e in events:
        for si in sorted(e["frames"]):
            r = by_si.get(si)
            if r is None:
                fail("FRAME-MISSING", si)
                continue
            mine = [a for a in (r.get("anomalies") or []) if isinstance(a, dict)
                    and client_type(a.get("id", "")) == e["type"] and a.get("target_name", "") in e["names"]]
            if not mine:
                fail("ENTRY-MISSING", si)
            elif rule == RULE_NEW and not any(a.get("labelled") is True for a in mine):
                fail("LABELLED-MISSING", si)

    lines.append("  session                : %s" % cap_dir)
    if rule == RULE_NEW:
        lines.append("  rule                   : NEW - every anomaly entry carries `labelled`; visible_positive = anomaly_present "
                     "AND a labelled entry with a valid box")
    elif rule == RULE_SHOT:
        lines.append("  rule                   : LEGACY_SHOT - every row carries label_rule legacy_shot (IAI.Capture.Shot); "
                     "visible_positive has the OLD meaning (anomaly_present AND an active entry with a valid box) and is NOT "
                     "the capture-run rule: a one-shot has no render record and no transition history")
    elif rule == RULE_OLD:
        lines.append("  rule                   : OLD - no entry carries `labelled` (a build before 084-07); visible_positive = "
                     "anomaly_present AND an active entry with a valid box, which does NOT say the effect is in the picture. "
                     "Per-frame truth for this session is annotation.json's frame lists.")
    else:
        lines.append("  rule                   : NONE - the session has no anomaly entries, so the rule cannot be read; "
                     "visible_positive must be false on every row")
    lines.append("  rows                   : %d   anomaly_present %d   visible_positive written %d, by the rule %d"
                 % (len(rows), n_present, n_vp_written, n_vp_rule))
    if rule == RULE_NEW:
        lines.append("  active, not labelled   : %d row(s) - the event is running but its effect is not applied on the frame "
                     "(blinking visible phases, lod_popping un-forced phases, texture boxes off screen)" % n_active_unlabelled)
        for t in sorted(per_type):
            c = per_type[t]
            lines.append("    %-22s entries %5d   labelled %5d   counted visible %5d" % (t or "(none)", c[0], c[1], c[2]))
        if n_vetoed_labelled:
            lines.append("  labelled, event vetoed : %d entr(ies) - labelled on frames no annotation.json event lists, with "
                         "run_summary vetoed_events %d; annotation.json decides" % (n_vetoed_labelled, vetoed))
    elif rule == RULE_OLD:
        lines.append("  old-rule positives not listed by annotation.json: %d row(s) - under the NEW rule these rows would read "
                     "visible_positive false" % n_old_vp_unlisted)
    reason_txt = ", ".join("%s/%s %d" % (k[0], k[1], v) for k, v in sorted(reason_counts.items())) or "none"
    lines.append("  transitions            : %d row(s) with transition_present; entries by anomaly/reason: %s"
                 % (n_transition_rows, reason_txt))
    if n_legacy_unreasoned:
        lines.append("  transitions, no reason : %d entr(ies) - a build before 084-07 wrote `transition` without "
                     "`transition_reason`; read as temporal_aa or hide_return, not as a defect" % n_legacy_unreasoned)
    lines.append("  label_temporal_aa      : %s (run_summary)" % ("absent" if temporal is None else temporal))
    lines.append("  annotation.json        : %d event(s), %d listed frame(s) (%s)"
                 % (len(events), sum(len(e["frames"]) for e in events),
                    ", ".join(sorted(set(e["list"] for e in events))) or "no list"))
    for cat in LABEL_RULE_CHECKS:
        sis = fails.get(cat)
        if sis:
            lines.append("  FAIL %-28s %d  [%s]" % (cat, len(sis), _lr_first(sis)))
        elif not quiet:
            lines.append("  pass %s" % cat)
    code = 1 if fails else 0
    if code:
        lines.append("LABEL-RULE: MISMATCH - %d check(s) failed (%s)" % (len(fails), ", ".join(sorted(fails))))
    else:
        lines.append("LABEL-RULE: NO MISMATCH under the %s rule" % rule)
    detail = {"rule": rule, "fails": {k: sorted(set(v)) for k, v in fails.items()}, "rows": len(rows),
              "present": n_present, "vp_written": n_vp_written, "vp_rule": n_vp_rule,
              "active_unlabelled": n_active_unlabelled, "old_vp_unlisted": n_old_vp_unlisted,
              "vetoed_labelled": n_vetoed_labelled, "legacy_unreasoned": n_legacy_unreasoned,
              "reasons": {"%s/%s" % k: v for k, v in reason_counts.items()}}
    return code, lines, detail


def _lr_entry(aid, target, labelled=None, bbox=True, reasons=None):
    e = {"id": aid, "target_name": target, "bbox_valid": bbox}
    if labelled is not None:
        e["labelled"] = labelled
    if reasons:
        e["transition"] = 1
        e["transition_reason"] = list(reasons)
    return e


def _lr_write(root, name, rows, events, run_summary=None):
    d = os.path.join(root, name)
    os.makedirs(d)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    ann = {"label_schema": 2, "anomalies": []}
    for etype, target, frames in events:
        fl = sorted(frames)
        ann["anomalies"].append({"anomaly_type": etype, "manifested": bool(fl),
                                 "affected_objects": {"nodes": [{"name": target}]},
                                 "injected_frames": {"frame_indices": fl},
                                 "affected_frames": {"frame_indices": fl}})
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
        json.dump(run_summary if run_summary is not None else {"label_temporal_aa": True, "vetoed_events": 0}, fh)
    return d


def _lr_row(si, ents, vp=None, present=False):
    p = bool(present)
    clean = [{k: v for k, v in e.items() if k != "_transition_only"} for e in ents]
    row = {"session_index": si, "anomaly_present": p, "anomalies": clean}
    if any(e.get("transition") for e in clean):
        row["transition_present"] = True
    if vp is None:
        if clean and all("labelled" in e for e in clean):
            vp = p and any(e.get("labelled") is True and e.get("bbox_valid") is True for e in clean)
        else:
            vp = p and any(e.get("bbox_valid") is True for e in clean)
    row["visible_positive"] = vp
    return row


def _lr_blink_session(labelled_keys=True, vp_override=None):
    hidden = {4, 5, 9, 10}
    rows = []
    for si in range(0, 14):
        ents = []
        if 3 <= si <= 10:
            lab = (si in hidden) if labelled_keys else None
            reasons = ("hide_return",) if si in (6, 11) else None
            ents.append(_lr_entry("blinking", "Cube", lab, True, reasons))
        present = 3 <= si <= 10
        vp = vp_override.get(si) if vp_override and si in vp_override else None
        rows.append(_lr_row(si, ents, vp=vp, present=present))
    return rows, [("blink", "Cube", hidden)]


def _label_rule_selftest():
    """Prove the label-rule reader can fail, both ways, on synthetic sessions (G96).

    NEW-rule sessions: an event-active unlabelled row must NOT count as a visible positive, and a row that says it does
    is a mismatch. OLD-rule sessions (no `labelled`): read under the old rule, said so, and not failed for following it.
    Plus: labelled vs annotation.json in both directions, the veto exception, transition-reason placement, and the
    refusal of a session that mixes the two rules.
    """
    import shutil
    import tempfile
    root = tempfile.mkdtemp(prefix="lr_selftest_")
    results = []

    def case(name, rows, events, want_code, want_rule=None, want_fail=None, rs=None, extra=None):
        d = _lr_write(root, name, rows, events, rs)
        code, lines, detail = label_rule_check(d, quiet=True)
        ok = code == want_code
        if want_rule is not None:
            ok = ok and detail.get("rule") == want_rule
        if want_fail is not None:
            ok = ok and want_fail in detail.get("fails", {})
        if extra is not None:
            ok = ok and extra(detail, lines)
        results.append(ok)
        print("LABEL-RULE SELFTEST %-44s -> exit %d rule %-4s %s" % (name, code, detail.get("rule"),
                                                                     "ok" if ok else "BROKEN"), flush=True)
        if not ok:
            for line in lines:
                print("    " + line, flush=True)

    try:
        rows, ev = _lr_blink_session()
        case("new_rule_blink_clean", rows, ev, 0, RULE_NEW,
             extra=lambda d, _l: d["vp_rule"] == 4 and d["active_unlabelled"] == 4)
        rows, ev = _lr_blink_session(vp_override={6: True})
        case("new_rule_active_unlabelled_counted_vp_FAILS", rows, ev, 1, RULE_NEW, "VP-MISMATCH")
        rows, ev = _lr_blink_session(labelled_keys=False)
        case("old_rule_blink_read_as_old", rows, ev, 0, RULE_OLD,
             extra=lambda d, l: d["vp_rule"] == 8 and d["old_vp_unlisted"] == 4 and any("OLD" in x for x in l))
        rows, ev = _lr_blink_session(labelled_keys=False, vp_override={5: False})
        case("old_rule_vp_missing_on_active_box_FAILS", rows, ev, 1, RULE_OLD, "VP-MISMATCH")

        tex = set(range(3, 11))
        good = [_lr_row(si, [_lr_entry("missing_texture", "Rock", si in tex)] if 3 <= si <= 10 else [],
                        present=3 <= si <= 10) for si in range(12)]
        case("new_rule_texture_labelled_clean", good, [("missing_texture", "Rock", tex)], 0, RULE_NEW)
        bad = [_lr_row(si, [_lr_entry("missing_texture", "Rock", False)] if 3 <= si <= 10 else [],
                       present=3 <= si <= 10) for si in range(12)]
        case("new_rule_texture_unlabelled_but_listed_FAILS", bad, [("missing_texture", "Rock", tex)], 1, RULE_NEW,
             "LABELLED-MISSING")

        extra_rows = [_lr_row(si, [_lr_entry("lod_popping", "Rock", si in (4, 5, 6))] if 3 <= si <= 8 else [],
                              present=3 <= si <= 8) for si in range(10)]
        case("new_rule_labelled_not_listed_FAILS", extra_rows, [("lod_popping", "Rock", {4, 5})], 1, RULE_NEW,
             "LABELLED-EXTRA")
        veto_rows = [_lr_row(si, [_lr_entry("blinking", "Plane", si in (4, 5))] if 3 <= si <= 8 else [],
                             present=3 <= si <= 8) for si in range(10)]
        case("new_rule_vetoed_event_reported_not_failed", veto_rows, [], 0, RULE_NEW,
             rs={"label_temporal_aa": True, "vetoed_events": 1}, extra=lambda d, _l: d["vetoed_labelled"] == 2)

        held = {5, 6, 7, 8}
        mip_rows = []
        for si in range(12):
            ents = []
            if si in held:
                rs_ = ("temporal_aa", "partial") if si == 5 else (("temporal_aa",) if si in (6, 7) else None)
                ents.append(_lr_entry("stuck_low_mip", "Rock", True, True, rs_))
            elif si in (9, 10):
                e = _lr_entry("stuck_low_mip", "Rock", False, True, ("temporal_aa",))
                e["_transition_only"] = True
                ents.append(e)
            mip_rows.append(_lr_row(si, ents, present=si in held))
        case("new_rule_stuck_mip_reasons_clean", mip_rows, [("stuck_low_mip", "Rock", held)], 0, RULE_NEW,
             extra=lambda d, _l: d["reasons"].get("stuck_low_mip/partial") == 1)
        wrong = [_lr_row(si, [_lr_entry("missing_texture", "Rock", True, True, ("partial",) if si == 4 else None)]
                         if 3 <= si <= 6 else [], present=3 <= si <= 6) for si in range(8)]
        case("new_rule_partial_on_texture_swap_FAILS", wrong, [("missing_texture", "Rock", {3, 4, 5, 6})], 1,
             RULE_NEW, "REASON-MISPLACED")
        rows, ev = _lr_blink_session()
        for r in rows:
            r.pop("transition_present", None)
        case("transition_present_missing_FAILS", rows, ev, 1, RULE_NEW, "TRANSITION-PRESENT-MISMATCH")
        rows, ev = _lr_blink_session()
        case("hide_return_without_temporal_aa_FAILS", rows, ev, 1, RULE_NEW, "REASON-WITHOUT-TEMPORAL-AA",
             rs={"label_temporal_aa": False, "vetoed_events": 0})
        rows, ev = _lr_blink_session()
        rows[6]["anomalies"][0].pop("transition_reason")
        case("new_rule_transition_without_reason_FAILS", rows, ev, 1, RULE_NEW, "REASON-MISSING")
        rows, ev = _lr_blink_session(labelled_keys=False)
        rows[6]["anomalies"][0].pop("transition_reason")
        case("old_rule_transition_without_reason_is_legacy", rows, ev, 0, RULE_OLD,
             extra=lambda d, _l: d["legacy_unreasoned"] == 1)
        off = {3, 4, 5, 8, 9, 10}
        fixed = [_lr_row(si, [_lr_entry("missing_texture", "Rock", si in off, si in off)] if 3 <= si <= 10 else [],
                         present=3 <= si <= 10) for si in range(12)]
        case("new_rule_texture_box_offscreen_unlisted_clean", fixed, [("missing_texture", "Rock", off)], 0, RULE_NEW,
             extra=lambda d, _l: d["vp_rule"] == 6 and d["active_unlabelled"] == 2)
        ff41 = [_lr_row(si, [_lr_entry("missing_texture", "Rock", False, si in off)] if 3 <= si <= 10 else [],
                        present=3 <= si <= 10) for si in range(12)]
        case("FF41BFF3_shape_texture_labelled_false_FAILS", ff41, [("missing_texture", "Rock", off)], 1, RULE_NEW,
             "LABELLED-MISSING", extra=lambda d, _l: d["vp_rule"] == 0 and len(d["fails"]["LABELLED-MISSING"]) == 6)
        unres = [_lr_row(si, [_lr_entry("stuck_low_mip", "Rock", True, True, ("unresolved",) if si == 5 else None)]
                         if 4 <= si <= 7 else [], present=4 <= si <= 7) for si in range(10)]
        case("new_rule_unresolved_on_stuck_mip_clean", unres, [("stuck_low_mip", "Rock", {4, 5, 6, 7})], 0, RULE_NEW,
             extra=lambda d, _l: d["reasons"].get("stuck_low_mip/unresolved") == 1)
        unres_bad = [_lr_row(si, [_lr_entry("stuck_low_mip", "Rock", si != 5, True, ("unresolved",) if si == 5 else None)]
                             if 4 <= si <= 7 else [], present=4 <= si <= 7) for si in range(10)]
        case("new_rule_unresolved_on_unlabelled_FAILS", unres_bad, [("stuck_low_mip", "Rock", {4, 6, 7})], 1, RULE_NEW,
             "REASON-ON-UNLABELLED")
        unres_tex = [_lr_row(si, [_lr_entry("missing_texture", "Rock", True, True, ("unresolved",) if si == 4 else None)]
                             if 3 <= si <= 6 else [], present=3 <= si <= 6) for si in range(8)]
        case("new_rule_unresolved_on_texture_swap_FAILS", unres_tex, [("missing_texture", "Rock", {3, 4, 5, 6})], 1,
             RULE_NEW, "REASON-MISPLACED")

        def f1_rows(aid, labelled_extra=None):
            out = []
            for si in range(44):
                if 10 <= si <= 39:
                    on = si <= 19 or si == labelled_extra
                    out.append(_lr_row(si, [_lr_entry(aid, "Rock", on, True, None if si <= 19 else ("effect_interrupted",))],
                                       present=on))
                else:
                    out.append(_lr_row(si, [], present=False))
            return out
        f1_listed = set(range(10, 20))
        case("F1_raw_revert_labels_stop_at_revert_clean", f1_rows("missing_texture"), [("missing_texture", "Rock", f1_listed)],
             0, RULE_NEW, extra=lambda d, _l: d["reasons"].get("missing_texture/effect_interrupted") == 20
             and d["vp_rule"] == 10)
        case("F1_effect_interrupted_on_labelled_FAILS", f1_rows("corrupted_texture", 25),
             [("corrupted_texture", "Rock", f1_listed | {25})], 1, RULE_NEW, "REASON-ON-LABELLED")
        case("F1_effect_interrupted_on_blinking_FAILS", f1_rows("blinking"), [("blinking", "Rock", f1_listed)], 1,
             RULE_NEW, "REASON-MISPLACED")

        def nan_rows(aid, labelled_extra=None, why=("nanite_unmaskable",)):
            out = []
            for si in range(44):
                if 10 <= si <= 39:
                    on = si <= 19 or si == labelled_extra
                    out.append(_lr_row(si, [_lr_entry(aid, "Rock", on, True, None if si <= 19 else why)], present=on))
                else:
                    out.append(_lr_row(si, [], present=False))
            return out
        case("R5_nanite_midevent_lod_popping_clean", nan_rows("lod_popping"), [("lod_popping", "Rock", f1_listed)], 0, RULE_NEW,
             extra=lambda d, _l: d["reasons"].get("lod_popping/nanite_unmaskable") == 20)
        case("R5_nanite_with_effect_interrupted_texture_clean",
             nan_rows("missing_texture", why=("effect_interrupted", "nanite_unmaskable")),
             [("missing_texture", "Rock", f1_listed)], 0, RULE_NEW,
             extra=lambda d, _l: d["reasons"].get("missing_texture/nanite_unmaskable") == 20)
        case("R5_nanite_unmaskable_on_labelled_FAILS", nan_rows("stuck_low_mip", 25),
             [("stuck_low_mip", "Rock", f1_listed | {25})], 1, RULE_NEW, "REASON-ON-LABELLED")
        case("R5_nanite_unmaskable_on_camera_clipping_FAILS", nan_rows("camera_clipping"),
             [("camera_clipping", "Rock", f1_listed)], 1, RULE_NEW, "REASON-MISPLACED")
        shot = [_lr_row(0, [_lr_entry("blinking", "Cube", None, True)], present=True)]
        shot[0]["label_rule"] = "legacy_shot"
        case("legacy_shot_read_under_old_meaning", shot, [], 0, RULE_SHOT,
             extra=lambda d, l: d["vp_rule"] == 1 and any("LEGACY_SHOT" in x for x in l))
        shot_bad = [_lr_row(0, [_lr_entry("blinking", "Cube", None, True)], vp=False, present=True)]
        shot_bad[0]["label_rule"] = "legacy_shot"
        case("legacy_shot_vp_missing_FAILS", shot_bad, [], 1, RULE_SHOT, "VP-MISMATCH")
        d_shot = _lr_write(root, "legacy_shot_no_annotation", shot, [])
        os.remove(os.path.join(d_shot, "annotation.json"))
        code_s, lines_s, det_s = label_rule_check(d_shot, quiet=True)
        ok_s = code_s == 0 and det_s.get("rule") == RULE_SHOT
        results.append(ok_s)
        print("LABEL-RULE SELFTEST %-44s -> exit %d rule %-4s %s" % ("legacy_shot_without_annotation_reads", code_s,
                                                                     det_s.get("rule"), "ok" if ok_s else "BROKEN"), flush=True)
        rows, ev = _lr_blink_session()
        rows[7]["label_rule"] = "legacy_shot"
        case("shot_rows_mixed_with_run_rows_refused", rows, ev, 3)
        rows, ev = _lr_blink_session()
        rows[5]["anomalies"][0].pop("labelled")
        case("mixed_rules_refused", rows, ev, 3)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    if all(results):
        print("LABEL-RULE SELFTEST: OK - %d cases; an event-active unlabelled row is not a visible positive, an old "
              "session is read under the old rule and says so, and every must-fail case fails." % len(results),
              flush=True)
        return 0
    print("LABEL-RULE SELFTEST: BROKEN - %d of %d cases failed." % (results.count(False), len(results)), flush=True)
    return 2


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
                    help="Report per-pair pixel observations and consistency. Only a complete "
                         "whole-span NO-TRACE returns 2; offsets are notes for human inspection.")
    ap.add_argument("--diff-threshold", type=int, default=DIFF_THRESH_DEFAULT,
                    help=f"a pixel COUNTS as changed when it differs from the previous frame by "
                         f"more than this, 0..255 (default {DIFF_THRESH_DEFAULT})")
    ap.add_argument("--edge-window", type=int, default=EDGE_WINDOW_DEFAULT,
                    help=f"how many frames either side of a claimed edge to search "
                         f"(default {EDGE_WINDOW_DEFAULT})")
    ap.add_argument("--min-visible-px", type=int, default=MIN_VISIBLE_PX_DEFAULT,
                    help=f"a pair whose mask carries fewer than this many pixels is "
                         f"UNASSESSABLE (default {MIN_VISIBLE_PX_DEFAULT}; needs masks)")
    ap.add_argument("--report-only", action="store_true",
                    help="label-pixel observations: suppress NO-TRACE exit 2, preserve execution error 3")
    ap.add_argument("--all", metavar="ROOT", default=None,
                    help="label-pixel gate: run over EVERY session folder under ROOT, one summary "
                         "line each, full output under --out. Runs --selftest first and refuses "
                         "to start if it is not OK.")
    ap.add_argument("--change-oracle", nargs="?", const="", default=None, metavar="SESSION",
                    help="m55 Stage 3: recompute every measured change_evidence.jsonl pair row from the "
                         "delivered PNGs and compare count with count and mean with mean. SESSION "
                         "defaults to --dir. Exit 1 when any comparison mismatches, a measured row "
                         "contradicts itself or an empty_region refusal is contradicted by its mask; "
                         "otherwise 3 when part of the sidecar cannot be interpreted or it cannot run; "
                         "otherwise 0. Arithmetic and transport only. With --selftest it proves the "
                         "oracle can agree and disagree.")
    ap.add_argument("--oracle-json", metavar="PATH", default=None,
                    help="with --change-oracle: also write the per-row comparison detail as JSON")
    ap.add_argument("--label-rule", action="store_true",
                    help="read the session's anomaly_present / labelled / visible_positive / transition fields against "
                         "the rule its build wrote them under (NEW with `labelled`, OLD without, said which), cross-checked "
                         "against annotation.json. Exit 0 no mismatch, 1 mismatch, 3 cannot run. With --selftest it proves "
                         "the reader can fail both ways.")
    args = ap.parse_args()

    if args.selftest:
        if args.label_pixel_gate:
            src = os.path.abspath(args.dir) if args.dir and os.path.isdir(args.dir) else None
            sys.exit(_label_pixel_selftest(args.diff_threshold, args.edge_window,
                                           args.min_visible_px, src))
        if args.change_oracle is not None:
            sys.exit(_change_oracle_selftest())
        if args.label_rule:
            sys.exit(_label_rule_selftest())
        sys.exit(_selftest())

    if args.label_rule:
        code, lines, _detail = label_rule_check(os.path.abspath(args.dir), args.quiet)
        for line in lines:
            _emit(line)
        sys.exit(code)

    if args.change_oracle is not None:
        target = args.change_oracle or args.dir
        try:
            code, lines, detail = change_oracle(os.path.abspath(target), args.quiet)
        except Exception as exc:
            code, detail = 3, {"session": target, "internal_error": "%s: %s" % (exc.__class__.__name__, str(exc)[:200])}
            lines = ["CHANGE-ORACLE (m55 Stage 3)",
                     "CHANGE-ORACLE: CANNOT RUN - internal error %s: %s" % (exc.__class__.__name__, str(exc)[:200]),
                     CHANGE_ORACLE_SENTENCE]
        for line in lines:
            _emit(line)
        if args.oracle_json:
            try:
                with open(args.oracle_json, "w", encoding="utf-8") as fh:
                    json.dump(detail, fh, indent=1, default=str)
            except (OSError, TypeError, ValueError) as exc:
                _emit("CHANGE-ORACLE: detail JSON not written to %s (%s)" % (args.oracle_json, exc.__class__.__name__))
                if code != 1:
                    code = 3
        sys.exit(code)

    if args.all:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        sys.exit(label_pixel_batch(os.path.abspath(args.all),
                                   os.path.abspath(args.out) if args.out else None,
                                   args.diff_threshold, args.edge_window, args.min_visible_px,
                                   args.quiet, args.report_only))

    cap_dir = os.path.abspath(args.dir)

    if args.label_pixel_gate:
        try:
            import PIL
        except ImportError:
            sys.exit("ERROR: Pillow is required for the label-pixel gate.")
        code, lines = label_pixel_gate(cap_dir, args.diff_threshold, args.edge_window,
                                       args.min_visible_px, args.quiet)
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
    lr_code, lr_lines, _lr_detail = label_rule_check(cap_dir, quiet=True)
    for line in lr_lines:
        _emit(line)


if __name__ == "__main__":
    main()
