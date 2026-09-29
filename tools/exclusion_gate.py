import json
import os
import shutil
import sys
import tempfile

M52_IDS = frozenset(["stuck_low_mip"])
M53_IDS = frozenset(["uv_corruption", "normal_corruption"])

PASS = 0
FAIL = 1
INVALID = 2


def is_m52_entry(entry):
    return entry.get("id", "") in M52_IDS and (entry.get("labelled") is True or bool(entry.get("transition")))


def is_m53_entry(entry):
    return entry.get("id", "") in M53_IDS


def row_entries(row):
    return [a for a in (row.get("anomalies") or []) if isinstance(a, dict)]


def is_coentry_row(row):
    ents = row_entries(row)
    return any(is_m52_entry(a) for a in ents) and any(is_m53_entry(a) for a in ents)


def coentry_frames(rows):
    return sorted(r.get("session_index") for r in rows if is_coentry_row(r))


def load_rows(session_dir):
    path = os.path.join(session_dir, "labels.jsonl")
    rows = []
    with open(path, "r", encoding="utf-8") as fh:
        for line in fh:
            line = line.strip()
            if line:
                rows.append(json.loads(line))
    return rows


def load_run_summary(session_dir):
    path = os.path.join(session_dir, "run_summary.json")
    if not os.path.isfile(path):
        return None
    with open(path, "r", encoding="utf-8") as fh:
        return json.load(fh)


def excl_a_gate(session_dir):
    result = {"session": session_dir, "verdict": INVALID, "reason": "", "coentry_frames": [], "rows": 0,
              "m52_entry_frames": 0, "m53_entry_frames": 0, "overlap_counter": None, "admitted_after_unresolved": None}
    if not os.path.isfile(os.path.join(session_dir, "labels.jsonl")):
        result["reason"] = "no labels.jsonl"
        return result
    rows = load_rows(session_dir)
    result["rows"] = len(rows)
    result["m52_entry_frames"] = sum(1 for r in rows if any(is_m52_entry(a) for a in row_entries(r)))
    result["m53_entry_frames"] = sum(1 for r in rows if any(is_m53_entry(a) for a in row_entries(r)))
    result["coentry_frames"] = coentry_frames(rows)
    summary = load_run_summary(session_dir)
    if summary is None:
        result["reason"] = "no run_summary.json"
        return result
    result["overlap_counter"] = summary.get("texcorrupt_m52_overlap_frames")
    result["admitted_after_unresolved"] = summary.get("texcorrupt_admitted_after_unresolved")
    if result["overlap_counter"] is None:
        result["reason"] = "run_summary has no texcorrupt_m52_overlap_frames"
        return result
    if result["admitted_after_unresolved"]:
        result["reason"] = ("texcorrupt_admitted_after_unresolved=%d: an m53 fire was admitted past restore_unresolved, "
                            "the accepted exception, so this leg cannot test the exclusion strictly"
                            % result["admitted_after_unresolved"])
        return result
    if result["overlap_counter"] != len(result["coentry_frames"]):
        result["verdict"] = FAIL
        result["reason"] = ("run_summary.texcorrupt_m52_overlap_frames=%d disagrees with the %d co-entry frame(s) in "
                            "labels.jsonl" % (result["overlap_counter"], len(result["coentry_frames"])))
        return result
    if result["coentry_frames"]:
        result["verdict"] = FAIL
        result["reason"] = "%d frame(s) carry an m52 entry (labelled or transition) and an m53 entry" % len(
            result["coentry_frames"])
        return result
    result["verdict"] = PASS
    result["reason"] = "0 frames carry both an m52 entry (labelled or transition) and an m53 entry"
    return result


def describe(result):
    names = {PASS: "PASS", FAIL: "FAIL", INVALID: "INVALID"}
    first = ",".join(str(s) for s in result["coentry_frames"][:12])
    return ("EXCL-A-COENTRY %s rows=%d m52_entry_frames=%d m53_entry_frames=%d coentry=%d [%s] overlap_counter=%s "
            "admitted_after_unresolved=%s - %s" % (names[result["verdict"]], result["rows"], result["m52_entry_frames"],
                                                   result["m53_entry_frames"], len(result["coentry_frames"]), first,
                                                   result["overlap_counter"], result["admitted_after_unresolved"],
                                                   result["reason"]))


def _entry(aid, labelled=None, transition=False):
    e = {"id": aid, "target_name": "T"}
    if labelled is not None:
        e["labelled"] = labelled
    if transition:
        e["transition"] = 1
        e["transition_reason"] = ["temporal_aa"]
    return e


def _write(root, name, rows, summary):
    d = os.path.join(root, name)
    os.makedirs(d)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    if summary is not None:
        with open(os.path.join(d, "run_summary.json"), "w", encoding="utf-8") as fh:
            json.dump(summary, fh)
    return d


def selftest():
    ok = True

    def expect(cond, what):
        nonlocal ok
        print(("OK   " if cond else "FAIL ") + what)
        ok = ok and cond

    m52 = "stuck_low_mip"
    uv = "uv_corruption"
    nm = "normal_corruption"
    cases = [
        ("m52 labelled + m53 labelled", [_entry(m52, True), _entry(uv, True)], True),
        ("m52 transition + m53 labelled", [_entry(m52, False, True), _entry(nm, True)], True),
        ("m52 transition + an unlabelled m53 entry is still a co-entry", [_entry(m52, False, True), _entry(uv, False)], True),
        ("other entries do not hide it", [_entry(m52, True), _entry("missing_texture", True), _entry(nm, False)], True),
        ("an m52 entry neither labelled nor transition is not one", [_entry(m52, False), _entry(uv, True)], False),
        ("no m53 entry", [_entry(m52, True), _entry("missing_texture", True, True)], False),
        ("m53 only", [_entry(uv, True), _entry(nm, False)], False),
        ("an empty frame", [], False),
    ]
    for name, ents, want in cases:
        expect(is_coentry_row({"session_index": 0, "anomalies": ents}) == want, "row: " + name)

    root = tempfile.mkdtemp(prefix="excl_gate_selftest_")
    try:
        clean_rows = [{"session_index": 0, "anomalies": [_entry(m52, True)]},
                      {"session_index": 1, "anomalies": [_entry(m52, False, True)]},
                      {"session_index": 2, "anomalies": []},
                      {"session_index": 3, "anomalies": [_entry(uv, True)]}]
        planted = [{"session_index": 0, "anomalies": [_entry(m52, True)]},
                   {"session_index": 1, "anomalies": [_entry(m52, False, True), _entry(uv, True)]},
                   {"session_index": 2, "anomalies": [_entry(m52, False, True), _entry(uv, False)]},
                   {"session_index": 3, "anomalies": [_entry(uv, True)]}]
        base = {"texcorrupt_m52_overlap_frames": 0, "texcorrupt_admitted_after_unresolved": 0}
        r = excl_a_gate(_write(root, "clean", clean_rows, base))
        expect(r["verdict"] == PASS and r["coentry_frames"] == [], "session: clean leg PASSes")
        r = excl_a_gate(_write(root, "planted", planted, dict(base, texcorrupt_m52_overlap_frames=2)))
        expect(r["verdict"] == FAIL and r["coentry_frames"] == [1, 2], "session: planted co-entry frames 1,2 FAIL (can fail)")
        r = excl_a_gate(_write(root, "counter_blind", planted, base))
        expect(r["verdict"] == FAIL and "disagrees" in r["reason"], "session: an in-run counter reading 0 beside co-entry frames FAILs")
        r = excl_a_gate(_write(root, "counter_extra", clean_rows, dict(base, texcorrupt_m52_overlap_frames=1)))
        expect(r["verdict"] == FAIL, "session: a non-zero counter on a clean artifact FAILs")
        r = excl_a_gate(_write(root, "unresolved", planted, dict(base, texcorrupt_admitted_after_unresolved=1,
                                                                   texcorrupt_m52_overlap_frames=2)))
        expect(r["verdict"] == INVALID, "session: admitted_after_unresolved>0 is INVALID for EXCL-A, not PASS")
        r = excl_a_gate(_write(root, "no_counter", clean_rows, {"texcorrupt_admitted_after_unresolved": 0}))
        expect(r["verdict"] == INVALID, "session: a build without the counter is INVALID, not PASS")
        r = excl_a_gate(_write(root, "no_summary", clean_rows, None))
        expect(r["verdict"] == INVALID, "session: no run_summary is INVALID, not PASS")
        expect("EXCL-A-COENTRY" in describe(excl_a_gate(os.path.join(root, "planted"))), "describe: grep token present")
    finally:
        shutil.rmtree(root, ignore_errors=True)
    print("exclusion gate selftest: %s" % ("OK" if ok else "FAILED"))
    return 0 if ok else 1


def main(argv):
    if len(argv) == 2 and argv[1] == "--selftest":
        return selftest()
    if len(argv) != 2:
        print("usage: exclusion_gate.py <session_dir> | --selftest")
        return INVALID
    result = excl_a_gate(argv[1])
    print(describe(result))
    return result["verdict"]


if __name__ == "__main__":
    sys.exit(main(sys.argv))
