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


DIFF_THRESH_DEFAULT = 8
EDGE_WINDOW_DEFAULT = 4
MIN_VISIBLE_PX_DEFAULT = 1
BASELINE_GUARD_FRAMES = 2

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


def _frac_above(cache, k, thresh, region):
    """d(k): the fraction of REGION pixels differing by more than `thresh` from frame k-1.

    This is the O4 metric (m44_gates.py prints the same quantity at 8/255 over the whole
    frame); here it is region-scoped so a small target is not averaged away.
    """
    from PIL import ImageChops, ImageStat
    try:
        a = cache.gray_of(k)
        b = cache.gray_of(k - 1)
    except Exception:
        return None
    if a.size != b.size:
        return None
    try:
        diff = ImageChops.difference(a, b).crop(region["box"])
    except Exception:
        return None
    hot = diff.point(lambda v: 255 if v > thresh else 0)
    if region["bin"] is not None:
        if region["bin"].size != hot.size:
            return None
        hot = ImageChops.multiply(hot, region["bin"])
    s = ImageStat.Stat(hot).sum[0] / 255.0
    return (s / float(region["npix"])) if region["npix"] > 0 else None


def _dominant_edge(cache, lo, hi, thresh, region, paths, tau, foreign=None):
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
        d = _frac_above(cache, k, thresh, region)
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
        })
    return out


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


def label_pixel_gate(cap_dir, thresh, edge_w, min_visible_px, quiet=False):
    """The label-vs-pixel gate. Returns (exit_code, lines).

    Per event, per contiguous run of labelled frames:
      ONSET is aligned when d(start) > tau AND d(start-1) <= tau.
      END   is aligned when d(end+1) > tau AND d(end)     <= tau.
    A shift is reported SIGNED: n < 0 means the PIXELS changed BEFORE the label said so.
    Nothing is inferred about WHY; the tool reports the reading.
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
        if len(base_idx) >= 3:
            break

    has_masks = any(r.get("mask_file") for r in rows.values())

    lines.append("LABEL-PIXEL GATE   (m49 step 1)")
    lines.append("  session                  %s" % cap_dir)
    lines.append("  frames / labels / events %d / %d / %d" % (len(paths), len(rows), len(events)))
    lines.append("  masks present            %s" % ("yes" if has_masks else "no (bbox-only mode)"))
    lines.append("  diff threshold           >%d/255 per pixel, region-scoped" % thresh)
    lines.append("  edge search window       +/-%d frames" % edge_w)
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
            ev_lines.append("%s %s(manifested-false-or-empty)" % (tag, V_NOTMEAS))
            n_notmeas += 1
            continue
        if ev["derived"]:
            ev_lines.append("%s %s(no-frame_indices-in-annotation)" % (tag, V_NOTMEAS))
            n_notmeas += 1
            continue

        anchor, row, entry = _anchor_entry(rows, ev["indices"], ev["node"], mlo)
        if entry is None:
            ev_lines.append("%s %s(no labels.jsonl row carries this event on frames %d..%d)"
                            % (tag, V_NOTMEAS, ev["indices"][0], ev["indices"][-1]))
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
            ev_lines.append("%s %s (mask count %d < %d on frame %d)"
                            % (tag, V_NOTVIS, mask_short[1], min_visible_px, mask_short[0]))
            n_notvis += 1
            continue

        region = _build_region(cap_dir, row, entry, frame_w, frame_h, mlo)
        if region is None:
            ev_lines.append("%s %s(no-region: no usable mask or bbox at frame %d)"
                            % (tag, V_NOTMEAS, anchor))
            n_notmeas += 1
            continue

        base_vals = []
        for k in base_idx:
            d = _frac_above(cache, k, thresh, region)
            if d is not None:
                base_vals.append(d)
        if len(base_vals) < 3:
            ev_lines.append("%s %s(baseline: only %d clean frame(s), need 3)"
                            % (tag, V_NOTMEAS, len(base_vals)))
            n_notmeas += 1
            continue
        tau, med, mad = _threshold_from(base_vals, mlo)
        contaminated = sum(1 for v in base_vals if v > tau)

        own = set(int(v) for v in ev["indices"])
        foreign = set()
        for other in events:
            if other is ev:
                continue
            for v in other["indices"]:
                if int(v) not in own:
                    foreign.add(int(v))

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
            onset_k, onset_d = _dominant_edge(cache, on_lo, on_hi, thresh, reg_on,
                                              paths, tau, foreign)

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
                end_k, _end_d = _dominant_edge(cache, lo, hi, thresh, reg_end, paths, tau,
                                               foreign)

            d_on = _frac_above(cache, rs, thresh, reg_on) if rs in paths and (rs - 1) in paths else None
            d_off = (_frac_above(cache, re_ + 1, thresh, reg_end)
                     if (not end_truncated and (re_ + 1) in paths) else None)

            n_on = (onset_k - rs) if onset_k is not None else None
            n_end = (end_k - (re_ + 1)) if end_k is not None else None

            if onset_k is None and end_k is None:
                verdicts.append((V_NOTVIS, "run[%d..%d] no pixel change above tau anywhere in "
                                           "+/-%d of the claim" % (rs, re_, edge_w)))
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

        ev_lines.append("%s %-16s [%s] tau=%.4f base=%d%s%s"
                        % (tag, worst, conf, tau, len(base_vals),
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

    lines.append("  baseline frames          %d (guard %d frame(s) either side of every window%s)"
                 % (len(base_idx), guard_used,
                    "" if guard_used == BASELINE_GUARD_FRAMES
                    else "; RELAXED from %d - a dense burst schedule left too few clean frames"
                         % BASELINE_GUARD_FRAMES))
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


def _synth_session(root, name, shift=0, end_shift=0, blank_region=False, with_mask=False):
    """Build a synthetic session with a KNOWN onset and end, then optionally lie about it."""
    from PIL import Image, ImageDraw
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    if with_mask:
        os.makedirs(os.path.join(d, "target_mask"))
    w, h = 160, 120
    box = (40, 30, 90, 80)
    true_start, true_end = 10, 17
    total = 30

    for i in range(total):
        im = Image.new("RGB", (w, h), (90, 90, 90))
        dr = ImageDraw.Draw(im)
        dr.rectangle([10, 10, 30, 30], fill=(60, 60, 60))
        anomalous = true_start <= i <= true_end
        dr.rectangle(list(box), fill=(230, 230, 230) if anomalous else (70, 70, 70))
        im.save(os.path.join(d, "Actual_Frames", "frame_%05d.png" % i))
        if with_mask:
            mk = Image.new("L", (w, h), 0)
            ImageDraw.Draw(mk).rectangle(list(box), fill=222)
            mk.save(os.path.join(d, "target_mask", "frame_%05d.png" % i))

    lo, hi = true_start + shift, true_end + shift + end_shift
    idxs = list(range(lo, hi + 1))
    label_box = [5, 5, 8, 8] if blank_region else [box[0], box[1], box[2] - box[0], box[3] - box[1]]

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


def _label_pixel_selftest(thresh, edge_w, min_visible_px, source_dir=None):
    """Prove the gate can FAIL, in BOTH directions and on BOTH edges (G96/G142).

    Without --dir it builds synthetic sessions, so the check is portable and a client can
    run it with nothing but this file, measure_label_offset.py and Pillow. With --dir it
    additionally shifts a REAL session's labels on a copy - the frames and the source
    session are never written to.
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
        cases = [
            ("clean", dict(shift=0), V_PASS),
            ("clean_masked", dict(shift=0, with_mask=True), V_PASS),
            ("label_late_1", dict(shift=1), "ONSET-SHIFT(-1)"),
            ("label_early_1", dict(shift=-1), "ONSET-SHIFT(+1)"),
            ("end_late_1", dict(end_shift=1), "END-SHIFT(-1)"),
            ("end_early_1", dict(end_shift=-1), "END-SHIFT(+1)"),
            ("blank_region", dict(shift=0, blank_region=True), V_NOTVIS),
        ]
        for name, kwargs, expect in cases:
            d = _synth_session(root, name, **kwargs)
            code, lines = label_pixel_gate(d, thresh, edge_w, min_visible_px, quiet=True)
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
                code, lines = label_pixel_gate(dst, thresh, edge_w, min_visible_px, quiet=True)
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
        else:
            print("SELFTEST: BROKEN - see the rows marked above. A gate that cannot fail is not "
                  "a gate.", flush=True)
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
    args = ap.parse_args()

    if args.selftest:
        if args.label_pixel_gate:
            src = os.path.abspath(args.dir) if args.dir and os.path.isdir(args.dir) else None
            sys.exit(_label_pixel_selftest(args.diff_threshold, args.edge_window,
                                           args.min_visible_px, src))
        sys.exit(_selftest())

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
