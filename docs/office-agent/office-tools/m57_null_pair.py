"""m57 gates L1, L2, L4b and L6 from pixels: one lod_popping leg against its null twin on the same scripted camera path.

Both legs must hold the same session indices, each with its frame on disk, or the check refuses to run (exit 3): a
truncated or padded null leg cannot judge the frames it lacks. Every common frame is judged.

L1  For each lod_popping event, the frames labelled in annotation.json (injected_frames) equal the frames where the anomaly
    leg differs from the null leg inside the event's box, ignoring the event's flagged frames (transition_reason
    lod_subvisible or lod_unconfirmed); onset and offset are 0 frames off. A leg with no labelled frame at all FAILS:
    a session where nothing was labelled exercised nothing.
L2  Every run of labelled or flagged frames that holds at least one labelled frame is followed by a recorded swap on the
    first frame after the run (or by an invisible release, entry lod.state past_held on the run's last frame, or by the
    end of the session). A swap at a pixel edge (cause distance or view_entry, direction up, on screen) is a frame equal
    to the null leg inside the box. At least one up swap must be recorded in the leg.
L4b On every frame where the target is held and off screen (entry lod.held true, lod.on_screen false), the two legs are
    equal over the whole frame. A leg with no such frame reports L4b as not exercised.
L6  On every frame that carries no lod_popping entry, the two legs are equal over the whole frame: nothing outside an
    event, before or after it, changes the picture.

bbox_px is [x, y, w, h] (AnomalyLabelWriter); an event's box is the union of its valid entry boxes. A pixel differs when
any channel differs by more than --tol (default 8, the m45 identity convention); a region differs when more than
--min-px pixels differ (default 0). Run at the AA-off identity arbiter. Frames are matched by session_index
(Actual_Frames/frame_NNNNN.png). Exit 0 PASS, 1 FAIL, 3 cannot run. --selftest builds synthetic sessions and proves
the check fails on each doctored copy.
"""
import argparse
import json
import os
import shutil
import sys
import tempfile

from PIL import Image, ImageChops

FLAG_REASONS = ("lod_subvisible", "lod_unconfirmed")


def read_rows(session):
    rows = {}
    with open(os.path.join(session, "labels.jsonl"), encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if line:
                r = json.loads(line)
                rows[r["session_index"]] = r
    return rows


def read_json(path):
    with open(path, encoding="utf-8-sig") as fh:
        return json.load(fh)


def frame_path(session, si):
    return os.path.join(session, "Actual_Frames", "frame_%05d.png" % si)


def box_of(entries):
    boxes = []
    for x in entries.values():
        b = x.get("bbox_px")
        if x.get("bbox_valid") and isinstance(b, list) and len(b) == 4 and b[2] > 0 and b[3] > 0:
            boxes.append((int(b[0]), int(b[1]), int(b[0]) + int(b[2]) - 1, int(b[1]) + int(b[3]) - 1))
    if not boxes:
        return None
    return (min(b[0] for b in boxes), min(b[1] for b in boxes), max(b[2] for b in boxes), max(b[3] for b in boxes))


def diff_mask(pa, pb, tol):
    ia = Image.open(pa).convert("RGB")
    ib = Image.open(pb).convert("RGB")
    try:
        if ia.size != ib.size:
            raise ValueError("frame sizes differ: %s vs %s" % (ia.size, ib.size))
        r, g, b = ImageChops.difference(ia, ib).split()
        return ImageChops.lighter(ImageChops.lighter(r, g), b).point(lambda v: 255 if v > tol else 0)
    finally:
        ia.close()
        ib.close()


def count(mask, box=None):
    if box is None:
        return mask.histogram()[255]
    w, h = mask.size
    x0, y0 = max(0, box[0]), max(0, box[1])
    x1, y1 = min(w - 1, box[2]), min(h - 1, box[3])
    if x1 < x0 or y1 < y0:
        return 0
    return mask.crop((x0, y0, x1 + 1, y1 + 1)).histogram()[255]


def runs_of(indices):
    out = []
    for si in sorted(indices):
        if out and si == out[-1][1] + 1:
            out[-1][1] = si
        else:
            out.append([si, si])
    return out


def check(anomaly, null, event_id=None, tol=8, min_px=0):
    out = {"lines": [], "fails": [], "not_exercised": []}

    def cannot(msg):
        return 3, {"lines": ["cannot run: %s" % msg], "fails": [], "not_exercised": []}

    try:
        rows_a = read_rows(anomaly)
        rows_n = read_rows(null)
        ann = read_json(os.path.join(anomaly, "annotation.json"))
    except (OSError, ValueError, KeyError) as exc:
        return cannot(str(exc))
    sa, sn = set(rows_a), set(rows_n)
    if sa != sn:
        return cannot("the legs hold different session indices (anomaly only %s, null only %s)"
                      % (sorted(sa - sn)[:8], sorted(sn - sa)[:8]))
    common = sorted(sa)
    if not common:
        return cannot("the legs hold no frames")
    missing = [si for si in common if not (os.path.isfile(frame_path(anomaly, si)) and os.path.isfile(frame_path(null, si)))]
    if missing:
        return cannot("%d frame(s) missing on disk in one leg (first %s)" % (len(missing), missing[:5]))
    null_entries = sorted(si for si, r in rows_n.items() if r.get("anomalies"))
    if null_entries:
        return cannot("the null leg carries anomaly entries on %d frame(s) (first %s)" % (len(null_entries), null_entries[:5]))
    other = sorted(set(x.get("id") for r in rows_a.values() for x in r.get("anomalies") or [] if x.get("id") != "lod_popping"))
    if other:
        return cannot("the anomaly leg carries other anomaly types %s" % other)

    events = {}
    for si, r in rows_a.items():
        for x in r.get("anomalies") or []:
            eid = x.get("event_id")
            if eid:
                events.setdefault(eid, {})[si] = x
    if event_id:
        if event_id not in events:
            return cannot("no labels.jsonl entry carries event %s" % event_id)
    ann_events = {e.get("event_id"): e for e in ann.get("anomalies") or []
                  if e.get("anomaly_type") == "lod_popping" or "lod_trigger" in e}

    masks = {si: diff_mask(frame_path(anomaly, si), frame_path(null, si), tol) for si in common}
    whole = {si: count(m) for si, m in masks.items()}
    entry_frames = set(si for ents in events.values() for si in ents)

    total_labelled = 0
    up_swaps = 0
    held_unseen_total = 0
    for eid in sorted(events):
        if event_id and eid != event_id:
            continue
        ents = events[eid]
        ev = ann_events.get(eid)
        box = box_of(ents)
        if box is None:
            return cannot("event %s has no valid bbox_px" % eid)
        labelled = set(((ev or {}).get("injected_frames") or {}).get("frame_indices") or [])
        total_labelled += len(labelled)
        flagged = set(si for si, x in ents.items() if set(x.get("transition_reason") or []) & set(FLAG_REASONS))
        unseen = set(si for si, x in ents.items() if x.get("lod.held") is True and x.get("lod.on_screen") is False)
        held_unseen_total += len(unseen)
        differ = set(si for si in ents if count(masks[si], box) > min_px)
        judged = differ - flagged
        extra = sorted(labelled - judged)
        unlabelled = sorted(judged - labelled)
        out["lines"].append("event %s%s box %s entries %d..%d labelled %d differ %d flagged %d unseen %d"
                            % (eid, "" if ev else " (not in annotation.json: vetoed or dropped)", box, min(ents), max(ents),
                               len(labelled), len(differ), len(flagged), len(unseen)))
        if extra or unlabelled:
            on = (min(judged) - min(labelled)) if judged and labelled else None
            off = (max(judged) - max(labelled)) if judged and labelled else None
            out["fails"].append("L1")
            out["lines"].append("L1 FAIL %s labelled-but-equal %s, differ-but-unlabelled %s, onset %s offset %s"
                                % (eid, extra[:8], unlabelled[:8], on, off))
        else:
            out["lines"].append("L1 PASS %s labelled frames equal the differing frames (onset 0, offset 0)" % eid)
        swaps = (ev or {}).get("lod_swaps") or []
        swap_at = {}
        for w in swaps:
            swap_at.setdefault(w.get("frame"), []).append(w)
            if w.get("direction") == "up":
                up_swaps += 1
        l2_bad = []
        for lo, hi in runs_of(labelled | flagged):
            if not any(lo <= si <= hi for si in labelled):
                continue
            f = hi + 1
            if f > common[-1]:
                out["lines"].append("L2 reading %s run %d..%d ends at the end of the session: no frame shows a release"
                                    % (eid, lo, hi))
                continue
            if f in swap_at:
                continue
            if (ents.get(hi) or {}).get("lod.state") == "past_held":
                out["lines"].append("L2 reading %s run %d..%d ends in an invisible release (past_held)" % (eid, lo, hi))
                continue
            l2_bad.append("run %d..%d has no swap on %d" % (lo, hi, f))
        for w in swaps:
            if w.get("cause") not in ("distance", "view_entry") or w.get("direction") != "up" or not w.get("on_screen"):
                continue
            f = w.get("frame")
            if (f - 1) not in (labelled | flagged) or f in labelled or (f in masks and count(masks[f], box) > min_px):
                l2_bad.append("swap %s is not the first null-equal frame after a labelled run" % f)
        if l2_bad:
            out["fails"].append("L2")
            out["lines"].append("L2 FAIL %s %s" % (eid, l2_bad[:6]))
        else:
            out["lines"].append("L2 PASS %s every labelled run ends at a recorded swap; pixel-edge swaps equal the null" % eid)
        l4 = sorted(si for si in unseen if whole[si] > min_px)
        if l4:
            out["fails"].append("L4b")
            out["lines"].append("L4b FAIL %s held off screen yet the legs differ on frame(s) %s" % (eid, l4[:8]))
    if total_labelled == 0:
        out["fails"].append("L1")
        out["lines"].append("L1 FAIL no frame is labelled in this leg: it exercised nothing")
    if up_swaps == 0:
        out["fails"].append("L2")
        out["lines"].append("L2 FAIL no up swap is recorded in this leg")
    if held_unseen_total == 0:
        out["not_exercised"].append("L4b")
        out["lines"].append("L4b NOT EXERCISED no frame holds the target off screen")
    elif "L4b" not in out["fails"]:
        out["lines"].append("L4b PASS %d held off-screen frame(s), legs equal" % held_unseen_total)
    l6 = sorted(si for si in common if si not in entry_frames and whole[si] > min_px)
    if l6:
        out["fails"].append("L6")
        out["lines"].append("L6 FAIL %d frame(s) with no lod_popping entry differ from the null (first %s)" % (len(l6), l6[:8]))
    else:
        out["lines"].append("L6 PASS %d frame(s) with no lod_popping entry equal the null" % sum(1 for si in common if si not in entry_frames))
    return (1 if out["fails"] else 0), out


BOX = [36, 26, 14, 12]


def _make_session(root, name, n=24, held=(), sub=(), unseen=(), labelled=None, offscreen_diff=(), swaps=None,
                  entries=range(4, 16), after_diff=(), null_entries=False, past_held_last=False):
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    x0, y0, x1, y1 = BOX[0], BOX[1], BOX[0] + BOX[2] - 1, BOX[1] + BOX[3] - 1
    held, sub, unseen = set(held), set(sub), set(unseen)
    is_null = name.startswith("null")
    rows = []
    for si in range(n):
        img = Image.new("RGB", (64, 48), (90, 90, 90))
        img.paste((200, 120, 10), (x0 + 2, y0 + 2, x1 - 1, y1 - 1))
        if not is_null and si in held:
            img.paste((10, 30, 200), (x0 + 2, y0 + 2, x1 - 1, y1 - 1))
        if not is_null and (si in offscreen_diff or si in after_diff):
            img.paste((255, 255, 255), (0, 0, 4, 4))
        img.save(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si))
        ents = []
        if (not is_null or null_entries) and si in entries:
            e = {"id": "lod_popping", "target_name": "Crate", "start_frame": 100, "event_id": "lod_popping@100|Crate",
                 "bbox_valid": True, "bbox_px": list(BOX), "lod.held": si in unseen or si in held or si in sub,
                 "lod.on_screen": si not in unseen, "lod.state": "labelled" if si in held else "armed"}
            if si in sub:
                e["transition"] = 1
                e["transition_reason"] = ["lod_subvisible"]
                e["lod.state"] = "subvisible"
            if past_held_last and held and si == max(held):
                e["lod.state"] = "past_held"
            ents.append(e)
        rows.append({"session_index": si, "anomalies": ents})
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    lab = sorted(labelled) if labelled is not None else sorted(s for s in held if s not in sub)
    if swaps is None:
        swaps = [{"frame": max(held) + 1, "cause": "view_entry", "direction": "up", "on_screen": True}] if held else []
    ann = {"label_schema": 2, "label_schema_minor": 3, "anomalies": [] if is_null else [
        {"event_id": "lod_popping@100|Crate", "anomaly_type": "lod_popping", "anomaly_subtype": "view_entry",
         "lod_trigger": "view_entry", "injected_frames": {"frame_indices": lab}, "lod_swaps": swaps}]}
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump(ann, fh)
    return d


def selftest():
    root = tempfile.mkdtemp(prefix="m57np_")
    ok = True
    try:
        held = set(range(8, 12))
        sub = {6, 7}
        unseen = {4, 5}
        null = _make_session(root, "null", 24)
        null_short = _make_session(root, "null_short", 11)
        null_tagged = _make_session(root, "null_tagged", 24, null_entries=True)
        null_padded = _make_session(root, "null_padded", 26)
        late = set(s + 1 for s in held)
        cases = [
            ("clean_xywh_box", _make_session(root, "a_clean", held=held, sub=sub, unseen=unseen), null, 0, None),
            ("label_late_by_one_FAILS", _make_session(root, "a_late", held=held, sub=sub, unseen=unseen, labelled=late), null, 1, "L1"),
            ("swap_recorded_one_late_FAILS", _make_session(root, "a_swap", held=held, sub=sub, unseen=unseen,
                                                            swaps=[{"frame": 13, "cause": "view_entry", "direction": "up", "on_screen": True}]),
             null, 1, "L2"),
            ("run_without_swap_FAILS", _make_session(root, "a_noswap", held=held, sub=sub, unseen=unseen,
                                                      swaps=[{"frame": 30, "cause": "hold_cap", "direction": "up", "on_screen": True}]),
             null, 1, "L2"),
            ("invisible_release_reads_ok", _make_session(root, "a_inv", held=held, sub=sub, unseen=unseen, swaps=[
                {"frame": 30, "cause": "hold_cap", "direction": "up", "on_screen": True}], past_held_last=True), null, 0, None),
            ("unseen_changes_picture_FAILS", _make_session(root, "a_unseen", held=held, sub=sub, unseen=unseen, offscreen_diff={5}),
             null, 1, "L4b"),
            ("probe_C_never_held_nothing_labelled_FAILS", _make_session(root, "a_c"), null, 1, "L1"),
            ("probe_D_every_diff_subvisible_FAILS", _make_session(root, "a_d", held=held, sub=held | sub, unseen=unseen,
                                                                  swaps=[]), null, 1, "L1"),
            ("probe_E_null_truncated_before_swap_CANNOT_RUN", _make_session(root, "a_e", held=held, sub=sub, unseen=unseen),
             null_short, 3, None),
            ("probe_F_picture_differs_after_event_FAILS", _make_session(root, "a_f", held=held, sub=sub, unseen=unseen,
                                                                        after_diff={20}), null, 1, "L6"),
            ("null_leg_padded_CANNOT_RUN", _make_session(root, "a_p", held=held, sub=sub, unseen=unseen), null_padded, 3, None),
            ("null_leg_with_entries_CANNOT_RUN", _make_session(root, "a_n", held=held, sub=sub, unseen=unseen), null_tagged, 3, None),
        ]
        for name, d, nl, want, cat in cases:
            code, res = check(d, nl)
            good = code == want and (cat is None or cat in res["fails"])
            ok = ok and good
            print("M57-NULL-PAIR SELFTEST %-46s -> exit %d %s" % (name, code, "ok" if good else "BROKEN"))
            if not good:
                for line in res["lines"]:
                    print("    " + line)
        d = _make_session(root, "a_nounseen", held=held, sub=sub)
        code, res = check(d, null)
        good = code == 0 and res["not_exercised"] == ["L4b"] and any("L4b NOT EXERCISED" in l for l in res["lines"])
        ok = ok and good
        print("M57-NULL-PAIR SELFTEST %-46s -> exit %d %s" % ("no_unseen_frame_reports_L4b_not_exercised", code, "ok" if good else "BROKEN"))
        shutil.rmtree(os.path.join(null, "Actual_Frames"))
        code, res = check(cases[0][1], null)
        good = code == 3
        ok = ok and good
        print("M57-NULL-PAIR SELFTEST %-46s -> exit %d %s" % ("null_frames_missing_CANNOT_RUN", code, "ok" if good else "BROKEN"))
        n_cases = len(cases) + 2
    finally:
        shutil.rmtree(root, ignore_errors=True)
    print("M57-NULL-PAIR SELFTEST: %s" % (("OK - %d cases" % n_cases) if ok else "BROKEN"))
    return 0 if ok else 2


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--anomaly")
    ap.add_argument("--null")
    ap.add_argument("--event")
    ap.add_argument("--tol", type=int, default=8)
    ap.add_argument("--min-px", type=int, default=0)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if not a.anomaly or not a.null:
        ap.error("--anomaly and --null are required")
    code, res = check(a.anomaly, a.null, a.event, a.tol, a.min_px)
    for line in res["lines"]:
        print(line)
    if res.get("not_exercised"):
        print("M57-NULL-PAIR NOT EXERCISED %s" % ", ".join(res["not_exercised"]))
    print("M57-NULL-PAIR VERDICT %s" % ("PASS" if code == 0 else "FAIL" if code == 1 else "CANNOT RUN"))
    return code


if __name__ == "__main__":
    sys.exit(main())
