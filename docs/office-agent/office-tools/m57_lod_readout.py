"""m57 gates L3, L5 and L6 from one session's annotation.json, run_summary.json and run log.

At least one expectation must be given; with none the check refuses to run (exit 3), because a readout that checks
nothing must never print PASS.

L3  --expect no_events: annotation.json holds no lod_popping event, run_summary carries lod_untriggered, and the leg tried:
    either it armed at least one fire and every armed fire is listed untriggered (count of 'lod_popping: ARMED' lines ==
    lod_untriggered == len(lod_untriggered_event_ids)), or it armed none and the log holds at least one 'NONE CAN BE ARMED
    NOW [lod_not_armable:<reason>]' refusal (and lod_untriggered is 0). A leg that neither armed nor was refused fails.
    --refusal <reason> additionally requires every refusal to name that reason.
    --expect events: at least one lod_popping event (the lever leg that shows a no-event reading is not blindness).
L5  --expect-mismatch 0: run_summary lod_render_mismatch_frames and label_transition_lod_unconfirmed_entries are both
    present and 0, and lod_render_checked_frames is above 0 when the session has labelled lod frames.
    --expect-mismatch any: lod_render_mismatch_frames above 0 and label_transition_lod_unconfirmed_entries above 0: the
    stale-view lever's mismatches reached the labels as lod_unconfirmed.
L6  Each 'lod_popping: ARMED' line is paired with the next 'lod_popping: REVERT' line for the same mesh, before that mesh
    is armed again. An armed fire with no revert, or a revert with no fire, fails.
    --expect-restored yes: at least one pair, and every pair reads RESTORED or NOTHING TO RESTORE (mesh destroyed).
    --expect-restored no: at least one pair reads NOT RESTORED (the skip-restore lever).
The log is <session>/anomaly_log.txt unless --log names another file. Exit 0 PASS, 1 FAIL, 3 cannot run.
--selftest proves each expectation can fail.
"""
import argparse
import json
import os
import re
import shutil
import sys
import tempfile

ARMED = re.compile(r"lod_popping: ARMED '([^']*)' mode=(\S+)")
REFUSED = re.compile(r"lod_popping: matched \d+ component\(s\) for '[^']*' but NONE CAN BE ARMED NOW \[lod_not_armable:([^\]]+)\]")
REVERT = re.compile(r"lod_popping: REVERT '([^']*)' mode=(\S+) (?:.*?(NOT RESTORED|RESTORED)|(NOTHING TO RESTORE))")


def load(session, log):
    with open(os.path.join(session, "annotation.json"), encoding="utf-8-sig") as fh:
        ann = json.load(fh)
    with open(os.path.join(session, "run_summary.json"), encoding="utf-8-sig") as fh:
        rs = json.load(fh)
    path = log or os.path.join(session, "anomaly_log.txt")
    with open(path, encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    return ann, rs, text


def pair_reverts(text):
    open_fire = {}
    pairs, unmatched_fires, orphans = [], [], []
    for line in text.splitlines():
        m = ARMED.search(line)
        if m:
            mesh = m.group(1)
            if mesh in open_fire:
                unmatched_fires.append(mesh)
            open_fire[mesh] = line
            continue
        m = REVERT.search(line)
        if m:
            mesh = m.group(1)
            result = m.group(3) or m.group(4)
            if mesh in open_fire:
                del open_fire[mesh]
                pairs.append((mesh, result))
            else:
                orphans.append(mesh)
    unmatched_fires.extend(open_fire)
    return pairs, unmatched_fires, orphans


def check(session, log=None, expect=None, mismatch=None, restored=None, refusal=None):
    if not (expect or mismatch or restored):
        return 3, ["cannot run: no expectation given (--expect, --expect-mismatch or --expect-restored); "
                   "a readout that checks nothing does not pass"]
    try:
        ann, rs, text = load(session, log)
    except (OSError, ValueError) as exc:
        return 3, ["cannot run: %s" % exc]
    lines, fails = [], []
    events = [e for e in ann.get("anomalies") or [] if e.get("anomaly_type") == "lod_popping"]
    labelled = sum(len(((e.get("injected_frames") or {}).get("frame_indices")) or []) for e in events)
    armed = len(ARMED.findall(text))
    refused = REFUSED.findall(text)
    untriggered = rs.get("lod_untriggered")
    ids = rs.get("lod_untriggered_event_ids")
    mm, ck = rs.get("lod_render_mismatch_frames"), rs.get("lod_render_checked_frames")
    unconf = rs.get("label_transition_lod_unconfirmed_entries")
    lines.append("lod events %d labelled frames %d armed %d untriggered %r ids %r mismatch %r checked %r unconfirmed %r"
                 % (len(events), labelled, armed, untriggered, len(ids) if isinstance(ids, list) else ids, mm, ck, unconf))
    if expect == "no_events":
        why = []
        if events:
            why.append("%d lod event(s) in annotation.json" % len(events))
        if armed == 0 and not refused:
            why.append("no ARMED line and no lod_not_armable refusal: the leg never tried, so 'no event' says nothing")
        if refusal and (not refused or any(r != refusal for r in refused)):
            why.append("refusals %s, wanted every one to read %s" % (sorted(set(refused)), refusal))
        if untriggered is None:
            why.append("run_summary has no lod_untriggered key")
        elif untriggered != armed or not isinstance(ids, list) or len(ids) != armed:
            why.append("armed %d, lod_untriggered %r, lod_untriggered_event_ids %r" % (armed, untriggered, ids))
        if why:
            fails.append("L3")
            lines.append("L3 FAIL " + "; ".join(why))
        else:
            lines.append("L3 PASS no lod event; %d armed fire(s), all listed untriggered; %d refusal(s) %s"
                         % (armed, len(refused), sorted(set(refused))))
    elif expect == "events":
        if not events:
            fails.append("L3")
            lines.append("L3 FAIL expected lod events (lever leg)")
        else:
            lines.append("L3 PASS %d lod event(s)" % len(events))
    if mismatch == "0":
        why = []
        if mm != 0:
            why.append("lod_render_mismatch_frames %r" % mm)
        if unconf != 0:
            why.append("label_transition_lod_unconfirmed_entries %r" % unconf)
        if labelled > 0 and not ck:
            why.append("%d labelled lod frame(s) but lod_render_checked_frames %r" % (labelled, ck))
        if why:
            fails.append("L5")
            lines.append("L5 FAIL " + "; ".join(why))
        else:
            lines.append("L5 PASS no render mismatch and no lod_unconfirmed entry over %r checked frame(s)" % ck)
    elif mismatch == "any":
        if not mm or not unconf:
            fails.append("L5")
            lines.append("L5 FAIL the stale-view lever gave mismatch %r and lod_unconfirmed entries %r (both must be above 0)"
                         % (mm, unconf))
        else:
            lines.append("L5 PASS lever produced %r mismatch frame(s), %r lod_unconfirmed entr(ies)" % (mm, unconf))
    if restored:
        pairs, unmatched, orphans = pair_reverts(text)
        bad = []
        if unmatched:
            bad.append("armed with no revert %s" % unmatched[:6])
        if orphans:
            bad.append("revert with no fire %s" % orphans[:6])
        not_restored = [m for m, r in pairs if r == "NOT RESTORED"]
        destroyed = [m for m, r in pairs if r == "NOTHING TO RESTORE"]
        if restored == "yes":
            if not pairs:
                bad.append("no fire/revert pair in the log")
            if not_restored:
                bad.append("NOT RESTORED %s" % not_restored[:6])
        elif not not_restored:
            bad.append("the skip-restore lever left every revert restored")
        if bad:
            fails.append("L6")
            lines.append("L6 FAIL " + "; ".join(bad))
        else:
            lines.append("L6 PASS %d fire/revert pair(s): %d restored, %d mesh destroyed, %d not restored"
                         % (len(pairs), len(pairs) - len(not_restored) - len(destroyed), len(destroyed), len(not_restored)))
    return (1 if fails else 0), lines


def _session(root, name, events=0, armed=1, untriggered=1, ids=None, mismatch=0, checked=12, unconfirmed=0, restored=True,
             reverts=None, drop_keys=(), destroyed=False, refusals=()):
    d = os.path.join(root, name)
    os.makedirs(d)
    ann = {"anomalies": [{"anomaly_type": "lod_popping", "injected_frames": {"frame_indices": [5, 6]}}] * events}
    rs = {"lod_untriggered": untriggered, "lod_untriggered_event_ids": ids if ids is not None else ["lod_popping@%d|Crate" % i
                                                                                                     for i in range(untriggered)],
          "lod_render_mismatch_frames": mismatch, "lod_render_checked_frames": checked,
          "label_transition_lod_unconfirmed_entries": unconfirmed}
    for k in drop_keys:
        rs.pop(k, None)
    log = "".join("LogAnomaly: Warning: lod_popping: matched 2 component(s) for '=CB_LodFix_N2' but NONE CAN BE ARMED NOW "
                  "[lod_not_armable:%s] - a spatial pop needs ...\n" % r for r in refusals)
    for i in range(armed):
        log += "LogAnomaly: lod_popping: ARMED 'SM_LodFix_N_%d' mode=distance_threshold held_lod=3\n" % i
    for i in range(armed if reverts is None else reverts):
        if destroyed:
            log += ("LogAnomaly: lod_popping: REVERT 'SM_LodFix_N_%d' mode=distance_threshold NOTHING TO RESTORE (mesh destroyed): "
                    "the held component no longer exists; swaps=0, last state armed.\n" % i)
        else:
            log += "LogAnomaly: lod_popping: REVERT 'SM_LodFix_N_%d' mode=distance_threshold forced_lod_model read back %d (saved 0) %s; swaps=0\n" % (
                i, 0 if restored else 2, "RESTORED" if restored else "NOT RESTORED")
    for fname, obj in (("annotation.json", ann), ("run_summary.json", rs)):
        with open(os.path.join(d, fname), "w", encoding="utf-8") as fh:
            json.dump(obj, fh)
    with open(os.path.join(d, "anomaly_log.txt"), "w", encoding="utf-8") as fh:
        fh.write(log)
    return d


def selftest():
    root = tempfile.mkdtemp(prefix="m57ro_")
    ok = True
    try:
        cases = [
            ("still_no_event_clean", _session(root, "a"), dict(expect="no_events", mismatch="0", restored="yes"), 0, None),
            ("no_expectation_CANNOT_RUN", _session(root, "r1", events=1, mismatch=5), dict(), 3, None),
            ("still_with_an_event_FAILS", _session(root, "b", events=1), dict(expect="no_events"), 1, "L3"),
            ("armed_not_listed_untriggered_FAILS", _session(root, "c", untriggered=0), dict(expect="no_events"), 1, "L3"),
            ("never_tried_FAILS", _session(root, "c2", armed=0, untriggered=0), dict(expect="no_events"), 1, "L3"),
            ("still_refused_clean", _session(root, "c4", armed=0, untriggered=0, refusals=("not_approaching",) * 3),
             dict(expect="no_events", refusal="not_approaching"), 0, None),
            ("refused_for_another_reason_FAILS", _session(root, "c5", armed=0, untriggered=0, refusals=("at_finest_lod",)),
             dict(expect="no_events", refusal="not_approaching"), 1, "L3"),
            ("untriggered_key_missing_FAILS", _session(root, "c3", drop_keys=("lod_untriggered",)), dict(expect="no_events"), 1, "L3"),
            ("lever_events_clean", _session(root, "d", events=1, untriggered=0), dict(expect="events"), 0, None),
            ("lever_without_events_FAILS", _session(root, "e"), dict(expect="events"), 1, "L3"),
            ("render_mismatch_FAILS", _session(root, "f", events=1, mismatch=2), dict(mismatch="0"), 1, "L5"),
            ("unconfirmed_entries_FAILS", _session(root, "f2", events=1, unconfirmed=3), dict(mismatch="0"), 1, "L5"),
            ("unconfirmed_key_missing_FAILS", _session(root, "f3", events=1, drop_keys=("label_transition_lod_unconfirmed_entries",)),
             dict(mismatch="0"), 1, "L5"),
            ("labelled_but_never_checked_FAILS", _session(root, "g", events=1, checked=0), dict(mismatch="0"), 1, "L5"),
            ("stale_view_lever_clean", _session(root, "h", events=1, mismatch=3, unconfirmed=3), dict(mismatch="any"), 0, None),
            ("stale_view_lever_not_flagged_FAILS", _session(root, "h2", events=1, mismatch=3, unconfirmed=0), dict(mismatch="any"), 1, "L5"),
            ("not_restored_FAILS", _session(root, "i", restored=False), dict(restored="yes"), 1, "L6"),
            ("fire_without_revert_FAILS", _session(root, "i2", armed=2, untriggered=2, reverts=1), dict(restored="yes"), 1, "L6"),
            ("no_fire_no_revert_FAILS", _session(root, "i3", armed=0, untriggered=0), dict(restored="yes"), 1, "L6"),
            ("destroyed_mesh_revert_pairs", _session(root, "i4", destroyed=True), dict(restored="yes"), 0, None),
            ("skip_restore_lever_clean", _session(root, "j", restored=False), dict(restored="no"), 0, None),
        ]
        for name, d, kw, want, cat in cases:
            code, lines = check(d, **kw)
            good = code == want and (cat is None or any(l.startswith(cat + " FAIL") for l in lines))
            ok = ok and good
            print("M57-LOD-READOUT SELFTEST %-36s -> exit %d %s" % (name, code, "ok" if good else "BROKEN"))
            if not good:
                for line in lines:
                    print("    " + line)
        code, _l = check(os.path.join(root, "missing"), expect="events")
        ok = ok and code == 3
        print("M57-LOD-READOUT SELFTEST %-36s -> exit %d %s" % ("missing_session_cannot_run", code, "ok" if code == 3 else "BROKEN"))
        n_cases = len(cases) + 1
    finally:
        shutil.rmtree(root, ignore_errors=True)
    print("M57-LOD-READOUT SELFTEST: %s" % (("OK - %d cases" % n_cases) if ok else "BROKEN"))
    return 0 if ok else 2


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--dir")
    ap.add_argument("--log")
    ap.add_argument("--expect", choices=("no_events", "events"))
    ap.add_argument("--expect-mismatch", choices=("0", "any"))
    ap.add_argument("--expect-restored", choices=("yes", "no"))
    ap.add_argument("--refusal")
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args()
    if a.selftest:
        return selftest()
    if not a.dir:
        ap.error("--dir is required")
    code, lines = check(a.dir, a.log, a.expect, a.expect_mismatch, a.expect_restored, a.refusal)
    for line in lines:
        print(line)
    print("M57-LOD-READOUT VERDICT %s" % ("PASS" if code == 0 else "FAIL" if code == 1 else "CANNOT RUN"))
    return code


if __name__ == "__main__":
    sys.exit(main())
