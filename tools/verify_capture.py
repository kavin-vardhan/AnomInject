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

Only NO-TRACE returns exit 2. Otherwise the session reports NO FAILURE FOUND, not label approval.
--report-only suppresses 2 but retains execution error 3. Event outcomes are worst-of the RUN
outcomes NO-TRACE > OFFSET-NOTE > PARTIAL > CONSISTENT. UNASSESSABLE/READING runs affect coverage
only; their observed edges cannot promote an event. With no assessable run, the event is
UNASSESSABLE, or READING when all runs are bbox-only. Separate run coverage remains visible.

Regional motion m_edge remains a reading and never refuses an edge. Values above the historical
0.42 marker add a run caveat; CONSISTENT counts either motion or lighting caveats. Unsatisfiable
tau and other coverage guards still apply. docs/verifier-characterisation.md records per-session
recovery classes and all wrong cells with competing peaks; the fixed planned denominator includes
an unscored ledger for ineligible runs. Recovery agreement does not establish label correctness.

Usage:
    python verify_capture.py --dir <sessionDir> [--out <annotatedDir>] [--quiet] [--red-only]
    python verify_capture.py --dir <sessionDir> --black-frame-gate [--black-threshold N]
    python verify_capture.py --dir <sessionDir> --label-pixel-gate [--report-only]
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
    if run["type"] not in NO_TRACE_TYPES:
        return R_UNASSESSABLE, "NO-TRACE unavailable for class %s" % run["type"]
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
    args = ap.parse_args()

    if args.selftest:
        if args.label_pixel_gate:
            src = os.path.abspath(args.dir) if args.dir and os.path.isdir(args.dir) else None
            sys.exit(_label_pixel_selftest(args.diff_threshold, args.edge_window,
                                           args.min_visible_px, src))
        sys.exit(_selftest())

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


if __name__ == "__main__":
    main()
