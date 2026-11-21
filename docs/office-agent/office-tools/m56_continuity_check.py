"""m56 G1 continuity check: is every engine tick of the capture span a written frame, and are the settle flags
exactly where the burst schedule puts them?

  python m56_continuity_check.py <session folder> [--json out.json]
  python m56_continuity_check.py --selftest

Clauses (plan 095-02 section 7 G1):
  C1 rows        labels.jsonl rows sorted by session_index; no session_index missing (a missing number is a lost frame)
  C2 tick_index  present on every row, an integer >= 0
  C3 tick step   consecutive rows differ by exactly 1 in tick_index
  C4 engine step consecutive rows differ by exactly 1 in frame_index (engine cross-check of C3)
  C5 run clock   t_wall_run present on every row, 0 on the first row, never decreasing
  C6 summary     run_summary: total_frames = rows, capture_span_ticks = total_frames, frames_dropped 0,
                 frames_dropped_ticks empty, every frames_dropped_by_cause value 0, ticks_per_captured_frame 1.000
  C7 settle      the rows carrying the row-level transition_reason "settle" are exactly the K ticks after every
                 onset tick and the K ticks after every revert tick (K, pre, positive, post from run.json); onsets
                 are anchored on the data (the row whose frame_index equals an event's start_frame), the revert on
                 the last tick the burst's events are live (+1). A burst with a stuck_low_mip fire (deferred onset,
                 restore trail) is judged after its onset only; its after-offset window is reported NOT JUDGED.
  C8 files       every row's image exists and is not empty (judged only when the Actual_Frames folder exists)

A session written before m56 has no tick_index; C7 then reads ticks as frame_index minus the first frame_index.

Exit 0 every clause passed, 1 a clause failed, 2 cannot run, 3 no failure but a clause (or part of C7) not judged.
"""
import argparse
import json
import os
import shutil
import struct
import sys
import tempfile
import zlib

SETTLE = "settle"
GLOBAL_TYPES = ("camera_clipping", "time_dilation")
DEFERRED_TYPES = ("stuck_low_mip",)
TPCF_TOL = 0.0005


def jload(path):
    try:
        with open(path, "r", encoding="utf-8-sig") as fh:
            return json.load(fh)
    except (OSError, ValueError):
        return None


def read_rows(d):
    p = os.path.join(d, "labels.jsonl")
    if not os.path.isfile(p):
        return None, "no labels.jsonl"
    rows = []
    with open(p, "r", encoding="utf-8-sig") as fh:
        for n, line in enumerate(fh, 1):
            if not line.strip():
                continue
            try:
                r = json.loads(line)
            except ValueError:
                return None, "labels.jsonl line %d is not JSON" % n
            if not isinstance(r, dict) or not isinstance(r.get("session_index"), int):
                return None, "labels.jsonl line %d has no integer session_index" % n
            rows.append(r)
    rows.sort(key=lambda r: r["session_index"])
    return rows, None


def is_int(v):
    return isinstance(v, int) and not isinstance(v, bool)


def row_settle(r):
    why = r.get("transition_reason")
    return isinstance(why, list) and SETTLE in why


def entry_reasons(a):
    why = a.get("transition_reason")
    return [str(x) for x in why] if isinstance(why, list) else []


def ticks_of(rows):
    if rows and all(is_int(r.get("tick_index")) for r in rows):
        return [r["tick_index"] for r in rows], "tick_index"
    if rows and all(is_int(r.get("frame_index")) for r in rows):
        f0 = rows[0]["frame_index"]
        return [r["frame_index"] - f0 for r in rows], "frame_index minus the first frame_index (no tick_index)"
    return None, None


def expected_settle(rows, ticks, run):
    """Return (expected tick set, judged tick ranges, notes, ok-to-judge)."""
    notes = []
    if not isinstance(run, dict):
        return None, [], ["no run.json: the burst schedule is unknown"], False
    try:
        K = int(run["settle_frames"])
        pre = int(run["pre_frames"])
        P = int(run["positive_frames"])
        Q = int(run["post_frames"])
    except (KeyError, TypeError, ValueError):
        return None, [], ["run.json lacks settle_frames / pre_frames / positive_frames / post_frames"], False
    if pre < 1 or P < 1 or Q < 1:
        return None, [], ["schedule %d/%d/%d/%d has a zero phase; such a schedule skips a tick by design" % (K, pre, P, Q)], False
    last = ticks[-1]
    tick_of_frame = {}
    for r, t in zip(rows, ticks):
        if is_int(r.get("frame_index")):
            tick_of_frame[r["frame_index"]] = t
    events = {}
    for r, t in zip(rows, ticks):
        for a in r.get("anomalies") or []:
            if not isinstance(a, dict) or a.get("id") in GLOBAL_TYPES or not is_int(a.get("start_frame")):
                continue
            key = (a.get("id"), a.get("target_name"), a["start_frame"])
            ev = events.setdefault(key, {"on": tick_of_frame.get(a["start_frame"]), "live": set()})
            rs = entry_reasons(a)
            if SETTLE not in rs or (ev["on"] is not None and t <= ev["on"] + K):
                ev["live"].add(t)
    onsets = {}
    for key, ev in events.items():
        if ev["on"] is None:
            notes.append("event %s@%d has no row at its start_frame (its onset tick is not in the data)" % (key[0], key[2]))
            continue
        onsets.setdefault(ev["on"], []).append((key, ev))
    expected = set()
    judged = []
    cycle = 2 * K + P + Q
    t_on = pre - 1
    lo = 0
    guard = 0
    while t_on <= last and guard < 100000:
        guard += 1
        near = sorted((abs(o - t_on), o) for o in onsets if o >= lo and abs(o - t_on) < cycle)
        if near and near[0][1] != t_on:
            notes.append("onset re-anchored on the data at tick %d (schedule predicted %d)" % (near[0][1], t_on))
            t_on = near[0][1]
        evs = onsets.get(t_on, [])
        for t in range(t_on + 1, t_on + K + 1):
            expected.add(t)
        deferred = any(k[0] in DEFERRED_TYPES for k, _e in evs)
        if evs:
            live = set()
            for _k, ev in evs:
                live |= ev["live"]
            t_rv = (max(live) + 1) if live else None
        else:
            t_rv = t_on + K + P
        if t_rv is None:
            notes.append("burst at tick %d: its events are never live in the data; after-offset window not judged" % t_on)
            judged.append((lo, t_on + K))
            nxt = [o for o in sorted(onsets) if o > t_on + K]
            if not nxt:
                return expected, judged, notes, True
            lo = nxt[0]
            t_on = nxt[0]
            continue
        if not deferred and t_rv != t_on + K + P and t_rv <= last:
            notes.append("burst at tick %d: revert anchored on the data at tick %d (schedule predicted %d)"
                         % (t_on, t_rv, t_on + K + P))
        if deferred:
            notes.append("burst at tick %d carries a stuck_low_mip fire (deferred onset / restore trail): after-offset "
                         "window NOT JUDGED" % t_on)
            judged.append((lo, t_on + K))
            nxt = [o for o in sorted(onsets) if o > t_on + K]
            if not nxt:
                return expected, judged, notes, True
            lo = nxt[0]
            t_on = nxt[0]
            continue
        for t in range(t_rv + 1, t_rv + K + 1):
            expected.add(t)
        nxt_on = t_rv + K + Q
        judged.append((lo, min(last, nxt_on)))
        lo = nxt_on
        t_on = nxt_on
    if lo <= last:
        judged.append((lo, last))
    return expected, judged, notes, True


def in_judged(t, judged):
    return any(a <= t <= b for a, b in judged)


def check(d):
    out = {"session": d, "clauses": {}, "readings": {}}
    rows, err = read_rows(d)
    if rows is None or not rows:
        out["verdict"] = "CANNOT RUN"
        out["why"] = err or "labels.jsonl has no rows"
        return 2, out
    rs = jload(os.path.join(d, "run_summary.json"))
    run = jload(os.path.join(d, "run.json"))
    C = out["clauses"]
    R = out["readings"]

    sis = [r["session_index"] for r in rows]
    dup = len(sis) - len(set(sis))
    missing_si = sorted(set(range(sis[0], sis[-1] + 1)) - set(sis))
    R["rows"] = len(rows)
    R["session_index_range"] = [sis[0], sis[-1]]
    R["session_index_missing"] = missing_si[:32]
    C["C1 rows"] = ("PASS" if not dup and not missing_si and sis[0] == 0 else
                    "FAIL: %d duplicate(s), %d missing session_index (first %s), first %d" % (dup, len(missing_si), missing_si[:8], sis[0]))

    no_tick = [r["session_index"] for r in rows if not (is_int(r.get("tick_index")) and r["tick_index"] >= 0)]
    R["rows_without_tick_index"] = len(no_tick)
    C["C2 tick_index"] = "PASS" if not no_tick else "FAIL: %d of %d row(s) carry no integer tick_index" % (len(no_tick), len(rows))

    if not no_tick:
        steps = {}
        bad = []
        for a, b in zip(rows, rows[1:]):
            dt = b["tick_index"] - a["tick_index"]
            steps[dt] = steps.get(dt, 0) + 1
            if dt != 1:
                bad.append((a["session_index"], b["session_index"], dt))
        R["tick_steps"] = {str(k): v for k, v in sorted(steps.items())}
        first_tick = rows[0]["tick_index"]
        C["C3 tick step"] = ("PASS" if not bad and first_tick == 0 else
                             "FAIL: %d step(s) other than 1 (first %s), first tick %d" % (len(bad), bad[:6], first_tick))
    else:
        C["C3 tick step"] = "FAIL: no tick_index to step"

    fsteps = {}
    fbad = []
    if all(is_int(r.get("frame_index")) for r in rows):
        for a, b in zip(rows, rows[1:]):
            df = b["frame_index"] - a["frame_index"]
            fsteps[df] = fsteps.get(df, 0) + 1
            if df != 1:
                fbad.append((a["session_index"], b["session_index"], df))
        R["frame_index_steps"] = {str(k): v for k, v in sorted(fsteps.items())}
        R["frame_index_jumps"] = len(fbad)
        C["C4 engine step"] = ("PASS" if not fbad else "FAIL: %d frame_index step(s) other than 1, steps %s, first %s"
                               % (len(fbad), R["frame_index_steps"], fbad[:6]))
    else:
        C["C4 engine step"] = "FAIL: a row has no integer frame_index"

    wr = [r.get("t_wall_run") for r in rows]
    if all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in wr):
        dec = sum(1 for a, b in zip(wr, wr[1:]) if b < a)
        C["C5 run clock"] = ("PASS" if abs(wr[0]) < 1e-9 and dec == 0 else
                             "FAIL: first t_wall_run %.6f, %d decrease(s)" % (wr[0], dec))
        R["t_wall_run_last"] = wr[-1]
    else:
        C["C5 run clock"] = "FAIL: %d row(s) carry no t_wall_run" % sum(1 for v in wr if not isinstance(v, (int, float)))

    if not isinstance(rs, dict):
        C["C6 summary"] = "FAIL: no run_summary.json"
    else:
        probs = []
        tf = rs.get("total_frames")
        span = rs.get("capture_span_ticks")
        R["total_frames"] = tf
        R["capture_span_ticks"] = span
        R["frames_dropped"] = rs.get("frames_dropped")
        R["ticks_per_captured_frame"] = rs.get("ticks_per_captured_frame")
        R["frames_dropped_by_cause"] = rs.get("frames_dropped_by_cause")
        if tf != len(rows):
            probs.append("total_frames %s != %d rows" % (tf, len(rows)))
        if "capture_span_ticks" not in rs:
            probs.append("capture_span_ticks absent")
        elif span != tf:
            probs.append("capture_span_ticks %s != total_frames %s" % (span, tf))
        if "frames_dropped" not in rs:
            probs.append("frames_dropped absent")
        elif rs.get("frames_dropped") != 0:
            probs.append("frames_dropped %s" % rs.get("frames_dropped"))
        if rs.get("frames_dropped_ticks") not in ([], None) or "frames_dropped_ticks" not in rs:
            probs.append("frames_dropped_ticks %s" % ("absent" if "frames_dropped_ticks" not in rs else rs.get("frames_dropped_ticks")[:8]))
        bc = rs.get("frames_dropped_by_cause")
        if not isinstance(bc, dict):
            probs.append("frames_dropped_by_cause absent")
        elif any(v for v in bc.values()):
            probs.append("frames_dropped_by_cause %s" % {k: v for k, v in bc.items() if v})
        tp = rs.get("ticks_per_captured_frame")
        if not isinstance(tp, (int, float)) or abs(float(tp) - 1.0) > TPCF_TOL:
            probs.append("ticks_per_captured_frame %s (want 1.000)" % (None if tp is None else round(float(tp), 4)))
        C["C6 summary"] = "PASS" if not probs else "FAIL: " + "; ".join(probs)

    ticks, basis = ticks_of(rows)
    R["tick_basis"] = basis
    if ticks is None:
        C["C7 settle"] = "NOT JUDGED: no tick_index and no frame_index"
    else:
        exp, judged, notes, ok = expected_settle(rows, ticks, run)
        R["settle_notes"] = notes[:24]
        actual = set(t for r, t in zip(rows, ticks) if row_settle(r))
        R["settle_rows"] = len(actual)
        if not ok:
            C["C7 settle"] = "NOT JUDGED: " + "; ".join(notes)
        else:
            present = set(ticks)
            exp_j = set(t for t in exp if in_judged(t, judged))
            act_j = set(t for t in actual if in_judged(t, judged))
            missing_rows = sorted(t for t in exp_j if t not in present)
            unflagged = sorted(t for t in exp_j if t in present and t not in actual)
            stray = sorted(act_j - exp_j)
            outside = sorted(actual - act_j)
            R["settle_expected"] = len(exp_j)
            R["settle_expected_without_row"] = len(missing_rows)
            probs = []
            if missing_rows:
                probs.append("%d expected settle tick(s) have no row at all (first %s)" % (len(missing_rows), missing_rows[:8]))
            if unflagged:
                probs.append("%d expected settle row(s) carry no settle flag (first %s)" % (len(unflagged), unflagged[:8]))
            if stray:
                probs.append("%d row(s) carry settle outside a settle window (first %s)" % (len(stray), stray[:8]))
            verdict = "PASS" if not probs else "FAIL: " + "; ".join(probs)
            uncovered = sorted(t for t in present if not in_judged(t, judged))
            if not probs and uncovered:
                verdict = "NOT JUDGED (part): %d tick(s) outside a judged window, %d settle row(s) there" % (
                    len(uncovered), len(outside))
            C["C7 settle"] = verdict + " | expected %d, flagged %d" % (len(exp_j), len(actual))

    fr = os.path.join(d, "Actual_Frames")
    if os.path.isdir(fr):
        miss, empty = [], []
        for r in rows:
            fp = os.path.join(d, (r.get("image") or "Actual_Frames/frame_%05d.png" % r["session_index"]).replace("\\", "/"))
            if not os.path.isfile(fp):
                miss.append(r["session_index"])
            elif os.path.getsize(fp) == 0:
                empty.append(r["session_index"])
        bad8 = []
        if miss:
            bad8.append("%d row image(s) missing (first %s)" % (len(miss), miss[:8]))
        if empty:
            bad8.append("%d row image(s) are zero bytes (first %s)" % (len(empty), empty[:8]))
        C["C8 files"] = "PASS" if not bad8 else "FAIL: " + "; ".join(bad8)
    else:
        C["C8 files"] = "NOT JUDGED: no Actual_Frames folder"

    fails = [k for k, v in C.items() if v.startswith("FAIL")]
    nj = [k for k, v in C.items() if v.startswith("NOT JUDGED")]
    if fails:
        out["verdict"] = "FAIL (%s)" % ", ".join(fails)
        return 1, out
    if nj:
        out["verdict"] = "PASS with %s not judged" % ", ".join(nj)
        return 3, out
    out["verdict"] = "PASS"
    return 0, out


def render(code, out):
    lines = ["M56 CONTINUITY CHECK  %s" % out["session"]]
    if code == 2:
        lines.append("CANNOT RUN: %s" % out.get("why"))
        return "\n".join(lines)
    for k, v in out["clauses"].items():
        lines.append("  %-16s %s" % (k, v))
    R = out["readings"]
    lines.append("  readings: rows %s | tick basis %s | frame_index steps %s | ticks_per_captured_frame %s | frames_dropped %s | "
                 "capture_span_ticks %s | settle rows %s (expected %s)" % (
                     R.get("rows"), R.get("tick_basis"), R.get("frame_index_steps"), R.get("ticks_per_captured_frame"),
                     R.get("frames_dropped"), R.get("capture_span_ticks"), R.get("settle_rows"), R.get("settle_expected")))
    for n in R.get("settle_notes") or []:
        lines.append("  note: %s" % n)
    lines.append("M56 CONTINUITY: %s (exit %d)" % (out["verdict"], code))
    return "\n".join(lines)


def _png1(path):
    raw = b"\x00\x00\x00\x00"
    ihdr = struct.pack(">IIBBBBB", 1, 1, 8, 2, 0, 0, 0)

    def chunk(t, b):
        return struct.pack(">I", len(b)) + t + b + struct.pack(">I", zlib.crc32(t + b) & 0xffffffff)
    with open(path, "wb") as fh:
        fh.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", ihdr) + chunk(b"IDAT", zlib.compress(raw[:4])) + chunk(b"IEND", b""))


def synth_session(root, name, K=2, pre=4, P=8, Q=4, ticks=40, frame0=100, drop_ticks=(), move_settle=None,
                  summary_override=None, with_images=True):
    """A synthetic m56 session: one targeted fire-window event per burst, every tick written."""
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "Actual_Frames"))
    settle = set()
    live = {}
    t_on = pre - 1
    while t_on < ticks:
        for t in range(t_on + 1, t_on + K + 1):
            settle.add(t)
        for t in range(t_on, t_on + K + P):
            live[t] = t_on
        t_rv = t_on + K + P
        for t in range(t_rv + 1, t_rv + K + 1):
            settle.add(t)
        t_on = t_rv + K + Q
    if move_settle is not None:
        a, b = move_settle
        settle.discard(a)
        settle.add(b)
    rows = []
    si = 0
    last_on = None
    for t in range(ticks):
        if t in drop_ticks:
            continue
        r = {"frame_index": frame0 + t, "session_index": si, "tick_index": t, "t_wall_run": round(t / 30.0, 6),
             "image": "Actual_Frames/frame_%05d.png" % si, "anomalies": []}
        if t in live:
            last_on = live[t]
            ent = {"id": "missing_texture", "target_name": "Rock", "start_frame": frame0 + live[t], "labelled": True}
            if t in settle:
                ent["transition"] = 1
                ent["transition_reason"] = [SETTLE]
            r["anomalies"].append(ent)
        elif t in settle and last_on is not None:
            r["anomalies"].append({"id": "missing_texture", "target_name": "Rock", "start_frame": frame0 + last_on,
                                   "labelled": False, "transition": 1, "transition_reason": [SETTLE]})
        if t in settle:
            r["transition_present"] = True
            r["transition_reason"] = [SETTLE]
        rows.append(r)
        if with_images:
            _png1(os.path.join(d, "Actual_Frames", "frame_%05d.png" % si))
        si += 1
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    n = len(rows)
    rs = {"total_frames": n, "capture_span_ticks": ticks, "frames_dropped": ticks - n,
          "frames_dropped_ticks": sorted(drop_ticks), "frames_dropped_by_cause": {"unknown": 0, "bench_drop": ticks - n},
          "ticks_per_captured_frame": round(ticks / float(n), 6)}
    if summary_override:
        rs.update(summary_override)
    with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
        json.dump(rs, fh)
    with open(os.path.join(d, "run.json"), "w", encoding="utf-8") as fh:
        json.dump({"settle_frames": K, "pre_frames": pre, "positive_frames": P, "post_frames": Q}, fh)
    return d


def selftest():
    root = tempfile.mkdtemp(prefix="m56cont_")
    results = []

    def case(name, d, want, must_contain=None):
        code, out = check(d)
        ok = code == want and (must_contain is None or must_contain in out["verdict"] + " ".join(out["clauses"].values()))
        results.append(ok)
        print("SELFTEST %-52s exit %d (want %d) %s" % (name, code, want, "ok" if ok else "*** WRONG ***"))
        if not ok:
            print(render(code, out))

    try:
        case("healthy continuous session passes", synth_session(root, "healthy"), 0)
        case("K=0 schedule: no settle frames, passes", synth_session(root, "k0", K=0, ticks=36), 0)
        case("a dropped tick fails C3/C6", synth_session(root, "drop", drop_ticks=(11,),
             summary_override={"frames_dropped": 0, "frames_dropped_ticks": [], "frames_dropped_by_cause": {"unknown": 0}}),
             1, "C3 tick step")
        case("a dropped tick honestly counted still fails (not continuous)", synth_session(root, "drop_counted", drop_ticks=(11,)),
             1, "frames_dropped 1")
        case("a settle flag moved one tick later fails C7", synth_session(root, "moved", move_settle=(5, 6)), 1, "C7 settle")
        case("a settle flag moved onto the revert tick fails C7", synth_session(root, "moved_rv", move_settle=(14, 13)), 1,
             "C7 settle")
        d = synth_session(root, "noimg")
        os.remove(os.path.join(d, "Actual_Frames", "frame_00007.png"))
        case("a missing frame file fails C8", d, 1, "C8 files")
        d = synth_session(root, "zeroimg")
        open(os.path.join(d, "Actual_Frames", "frame_00009.png"), "wb").close()
        case("a zero-byte frame file fails C8", d, 1, "zero bytes")
        d = synth_session(root, "norun")
        os.remove(os.path.join(d, "run.json"))
        case("no run.json: settle placement not judged (exit 3)", d, 3, "NOT JUDGED")
        d = synth_session(root, "tpcf", summary_override={"ticks_per_captured_frame": 1.344})
        case("ticks_per_captured_frame 1.344 fails C6", d, 1, "ticks_per_captured_frame")
    finally:
        shutil.rmtree(root, ignore_errors=True)
    ok = all(results)
    print("M56 CONTINUITY SELFTEST: %s - %d case(s)" % ("OK" if ok else "FAILED", len(results)))
    return 0 if ok else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="m56 G1 continuity check (read-only).")
    ap.add_argument("session", nargs="?")
    ap.add_argument("--json", default=None)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest()
    if not a.session:
        ap.print_usage()
        return 2
    code, out = check(os.path.abspath(a.session))
    print(render(code, out))
    if a.json:
        with open(a.json, "w", encoding="utf-8") as fh:
            json.dump({"exit": code, **out}, fh, indent=1)
    return code


if __name__ == "__main__":
    sys.exit(main())
