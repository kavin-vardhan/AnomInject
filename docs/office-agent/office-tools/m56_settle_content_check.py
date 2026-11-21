"""m56 N6 (095-06b): what a settle frame's label says.

  python m56_settle_content_check.py <session dir> [--json out]
  python m56_settle_content_check.py --selftest

Reads labels.jsonl (sorted by session_index), run.json (settle_frames K) and the target_mask folder.
  S1 per event, the count of frames whose entry is labelled AND flagged settle (an onset-settle frame):
     fire-window types and missing_object must have exactly K (P6, P10); blinking and others are reported only;
     stuck_low_mip (deferred onset) and session globals are not judged. An event cut by the session's end is not judged.
  S2 every onset-settle frame of a judged event has row mask_state present or empty, never unmeasured or absent (P12),
     and a present mask names a file that exists and is not empty.
Exit 0 PASS, 1 FAIL, 2 cannot run, 3 nothing judged.
"""
import argparse
import json
import os
import shutil
import sys
import tempfile

SETTLE = "settle"
EXACT_K_TYPES = ("missing_texture", "corrupted_texture", "uv_corruption", "normal_corruption", "missing_object")
NOT_JUDGED = ("stuck_low_mip", "camera_clipping", "time_dilation")


def is_int(v):
    return isinstance(v, int) and not isinstance(v, bool)


def reasons(x):
    v = x.get("transition_reason")
    return v if isinstance(v, list) else []


def check(d):
    out = {"session": d, "events": {}, "fails": [], "notes": []}
    p = os.path.join(d, "labels.jsonl")
    if not os.path.isfile(p):
        out["verdict"] = "CANNOT RUN: no labels.jsonl"
        return 2, out
    with open(p, "r", encoding="utf-8-sig") as fh:
        rows = sorted((json.loads(x) for x in fh if x.strip()), key=lambda r: r.get("session_index", 0))
    k = None
    rj = os.path.join(d, "run.json")
    if os.path.isfile(rj):
        with open(rj, "r", encoding="utf-8-sig") as fh:
            k = json.load(fh).get("settle_frames")
    if not is_int(k):
        out["verdict"] = "CANNOT RUN: run.json gives no settle_frames"
        return 2, out
    if not rows:
        out["verdict"] = "CANNOT RUN: no rows"
        return 2, out
    last_frame = max(r.get("frame_index", -1) for r in rows)
    events = {}
    for r in rows:
        for a in r.get("anomalies") or []:
            if not isinstance(a, dict) or not is_int(a.get("start_frame")):
                continue
            key = "%s@%d|%s" % (a.get("id"), a["start_frame"], a.get("target_name"))
            ev = events.setdefault(key, {"id": a.get("id"), "start": a["start_frame"], "labelled_settle": [], "mask_bad": []})
            if a.get("labelled") is True and (SETTLE in reasons(a) or SETTLE in reasons(r)):
                ev["labelled_settle"].append(r.get("session_index"))
                state = r.get("mask_state")
                if state not in ("present", "empty"):
                    ev["mask_bad"].append((r.get("session_index"), "mask_state %r" % state))
                elif state == "present":
                    mf = r.get("mask_file")
                    fp = os.path.join(d, mf.replace("\\", "/")) if isinstance(mf, str) else None
                    if not fp or not os.path.isfile(fp) or os.path.getsize(fp) == 0:
                        ev["mask_bad"].append((r.get("session_index"), "mask file missing or empty: %r" % mf))
    judged = 0
    for key, ev in sorted(events.items()):
        n = len(ev["labelled_settle"])
        cut = ev["start"] + k > last_frame
        rule = "not judged"
        if ev["id"] in NOT_JUDGED or cut:
            rule = "not judged (%s)" % ("session end" if cut else ev["id"])
        else:
            judged += 1
            if ev["id"] in EXACT_K_TYPES:
                rule = "exactly %d" % k
                if n != k:
                    out["fails"].append("%s: %d labelled settle frame(s), want %d" % (key, n, k))
            else:
                rule = "reported"
            for si, why in ev["mask_bad"]:
                out["fails"].append("%s si %s: %s" % (key, si, why))
        out["events"][key] = {"labelled_settle": n, "rule": rule, "mask_bad": len(ev["mask_bad"])}
    if not judged:
        out["verdict"] = "NOTHING JUDGED"
        return 3, out
    out["verdict"] = "PASS" if not out["fails"] else "FAIL: " + "; ".join(out["fails"][:12])
    return (0 if not out["fails"] else 1), out


def render(code, out):
    lines = ["M56 SETTLE CONTENT  %s" % out.get("session")]
    for key, e in sorted(out.get("events", {}).items()):
        lines.append("  %-60s labelled+settle %d  rule %s  mask problems %d" % (key, e["labelled_settle"], e["rule"], e["mask_bad"]))
    lines.append("M56 SETTLE CONTENT: %s (exit %d)" % (out.get("verdict"), code))
    return "\n".join(lines)


def _synth(root, name, k=2, aid="missing_texture", tweak=None, cycles=2, ticks=34):
    d = os.path.join(root, name)
    os.makedirs(os.path.join(d, "target_mask"))
    with open(os.path.join(d, "run.json"), "w", encoding="utf-8") as fh:
        json.dump({"settle_frames": k}, fh)
    rows = []
    for t in range(ticks):
        r = {"session_index": t, "frame_index": 100 + t, "anomalies": []}
        for c in range(cycles):
            on = 3 + 16 * c
            if on <= t < on + k + 8:
                e = {"id": aid, "target_name": "Rock", "start_frame": 100 + on, "labelled": True}
                if on < t <= on + k:
                    e["transition"] = 1
                    e["transition_reason"] = [SETTLE]
                    r["transition_reason"] = [SETTLE]
                r["anomalies"].append(e)
                mf = "target_mask/frame_%05d.png" % t
                with open(os.path.join(d, mf), "wb") as fh:
                    fh.write(b"\x89PNG")
                r["mask_state"] = "present"
                r["mask_file"] = mf
        if tweak:
            tweak(t, r, d)
        rows.append(r)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    return d


def selftest():
    root = tempfile.mkdtemp(prefix="m56settle_")
    results = []

    def case(name, d, want, must=None):
        code, out = check(d)
        ok = code == want and (must is None or must in out.get("verdict", ""))
        results.append(ok)
        print("SELFTEST %-60s exit %d (want %d) %s" % (name, code, want, "ok" if ok else "*** WRONG ***"))
        if not ok:
            print(render(code, out))

    try:
        case("healthy: K labelled settle frames per event, masks present", _synth(root, "ok"), 0)

        def unflag(t, r, d):
            if t == 5:
                r.pop("transition_reason", None)
                for e in r["anomalies"]:
                    e.pop("transition", None)
                    e.pop("transition_reason", None)
        case("one onset-settle frame lost its flag", _synth(root, "unflag", tweak=unflag), 1, "want 2")

        def unlabel(t, r, d):
            if t == 4:
                for e in r["anomalies"]:
                    e["labelled"] = False
        case("one onset-settle frame not labelled", _synth(root, "unlabel", tweak=unlabel), 1, "want 2")

        def unmeasured(t, r, d):
            if t == 20:
                r["mask_state"] = "unmeasured"
                r.pop("mask_file", None)
        case("an onset-settle frame with mask_state unmeasured", _synth(root, "unmeasured", tweak=unmeasured), 1, "unmeasured")

        def zero_mask(t, r, d):
            if t == 21:
                open(os.path.join(d, r["mask_file"]), "wb").close()
        case("an onset-settle mask file of zero bytes", _synth(root, "zeromask", tweak=zero_mask), 1, "missing or empty")
        case("K = 0: no labelled settle frame expected", _synth(root, "k0", k=0), 0)
        case("blinking is reported, not judged on the count", _synth(root, "blink", aid="blinking", tweak=unflag), 0)
        case("stuck_low_mip only: nothing judged", _synth(root, "slm", aid="stuck_low_mip"), 3)
    finally:
        shutil.rmtree(root, ignore_errors=True)
    ok = all(results)
    print("M56 SETTLE CONTENT SELFTEST: %s - %d case(s)" % ("OK" if ok else "FAILED", len(results)))
    return 0 if ok else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="m56 N6 settle-frame label content check (read-only).")
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
            json.dump({"exit": code, **out}, fh, indent=1, default=str)
    return code


if __name__ == "__main__":
    sys.exit(main())
