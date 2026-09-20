"""m52 gate readout: what the stuck_low_mip telemetry says, frame by frame.

Reads a banked capture session and prints, for every frame that carries a
stuck_low_mip anomaly entry, the engine's own resident-mip facts beside the
label's own observable decision. Prints the run_summary stuck_mip_* counters
and the schema key sets so the additive check is a reading, not an assertion.

Usage: python m52_readout.py <session-dir> [<session-dir> ...]
"""

import json
import os
import sys


def load_rows(session):
    path = os.path.join(session, "labels.jsonl")
    rows = []
    if not os.path.isfile(path):
        return rows
    with open(path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            try:
                rows.append(json.loads(line))
            except json.JSONDecodeError:
                pass
    rows.sort(key=lambda r: r.get("session_index", -1))
    return rows


def report(session):
    print("=" * 100)
    print("SESSION %s" % session)
    rows = load_rows(session)
    print("  labels.jsonl rows: %d" % len(rows))

    row_keys = set()
    anom_keys = set()
    stuck_rows = []
    for r in rows:
        row_keys |= set(r.keys())
        for a in r.get("anomalies", []) or []:
            anom_keys |= set(a.keys())
            if a.get("id") == "stuck_low_mip":
                stuck_rows.append((r.get("session_index"), r, a))

    print("  row key count      : %d" % len(row_keys))
    print("  anomaly key count  : %d" % len(anom_keys))
    print("  stuck_mip.* keys   : %s" % sorted(k for k in anom_keys if k.startswith("stuck_mip")))
    print("  frames with a stuck_low_mip entry: %d" % len(stuck_rows))

    if stuck_rows:
        print()
        print("  %-5s %-9s %-9s %-8s %-6s %-6s %-8s %-6s %-6s %-6s %-10s %s"
              % ("si", "pri_res", "pri_base", "forced", "floor", "held", "held_all", "armed",
                 "obs", "tgtpx", "topres_px", "flags"))
        held_true = 0
        held_all_true = 0
        first_held = None
        for si, r, a in stuck_rows:
            held = a.get("stuck_mip.held")
            obs = a.get("observable")
            flags = [k.rsplit(".", 1)[-1] for k in a
                     if k.startswith("stuck_mip.fail_") or k == "stuck_mip.bench_no_hold"]
            if held:
                held_true += 1
                if first_held is None:
                    first_held = si
            if a.get("stuck_mip.held_all"):
                held_all_true += 1
            print("  %-5s %-9s %-9s %-8s %-6s %-6s %-8s %-6s %-6s %-6s %-10s %s"
                  % (si, a.get("stuck_mip.primary_resident_mips"), a.get("stuck_mip.primary_baseline_mips"),
                     a.get("stuck_mip.forced_mips"), a.get("stuck_mip.floor_mips"),
                     held, a.get("stuck_mip.held_all"), a.get("stuck_mip.textures_armed"),
                     obs, a.get("target_pixels"), a.get("stuck_mip.top_resident_px"),
                     ",".join(flags) if flags else "-"))
        print()
        print("  HELD true on %d of %d frames carrying an entry; held_all true on %d"
              % (held_true, len(stuck_rows), held_all_true))
        print("  FIRST HELD session_index = %s" % first_held)
        f0 = stuck_rows[0][2]
        print("  texture=%s co_affected_visible=%s textures_armed=%s onset_latency_frames=%s"
              % (f0.get("stuck_mip.texture"), f0.get("stuck_mip.co_affected_visible"),
                 f0.get("stuck_mip.textures_armed"), f0.get("stuck_mip.onset_latency_frames")))
        for si, r, a in stuck_rows:
            per = a.get("stuck_mip.textures")
            if isinstance(per, list) and per:
                print("  per-texture at si=%s (%d entr%s):" % (si, len(per), "y" if len(per) == 1 else "ies"))
                for t in per:
                    print("    %-34s baseline=%-3s forced=%-3s resident=%-3s at_onset=%-3s co=%-3s held=%s"
                          % (t.get("name"), t.get("baseline_mips"), t.get("forced_mips"),
                             t.get("resident_mips"), t.get("resident_mips_at_onset"),
                             t.get("co_affected_visible"), t.get("held")))
                break

    rs = os.path.join(session, "run_summary.json")
    if os.path.isfile(rs):
        j = json.load(open(rs, encoding="utf-8"))
        print()
        print("  run_summary keys: %d" % len(j))
        for k in sorted(j):
            if k.startswith("stuck_mip"):
                print("    %-42s %s" % (k, j[k]))
        for k in ("total_frames", "positive_frames", "observable_frames", "frames_condition_lost",
                  "vetoed_events", "non_manifested_events", "speed_ratio", "sustained_wall_fps",
                  "label_schema"):
            if k in j:
                print("    %-42s %s" % (k, j[k]))

    ann = os.path.join(session, "annotation.json")
    if os.path.isfile(ann):
        j = json.load(open(ann, encoding="utf-8"))
        ev = j.get("anomalies", []) or []
        print()
        print("  annotation.json root keys: %d   events: %d" % (len(j), len(ev)))
        held_by_si = {si: bool(a.get("stuck_mip.held")) for si, _r, a in stuck_rows}
        for e in ev:
            af = e.get("affected_frames", {}) or {}
            inj = e.get("injected_frames", {}) or {}
            print("    type=%-14s manifested=%-5s observability_measured=%-5s injected=%s affected=%s"
                  % (e.get("anomaly_type"), e.get("manifested"), e.get("observability_measured"),
                     inj.get("frame_indices"), af.get("frame_indices")))
            if e.get("anomaly_type") != "stuck_low_mip" or not e.get("manifested"):
                continue
            idx = inj.get("frame_indices") or []
            if not idx:
                continue
            unheld = [k for k in idx if not held_by_si.get(k, False)]
            preceding = [k for k in sorted(held_by_si) if k < min(idx) and held_by_si[k]]
            print("      WINDOW  n=%d  first=%s  contiguous=%s  unheld-in-window=%s  held-before-window=%s"
                  % (len(idx), min(idx),
                     "YES" if idx == list(range(min(idx), max(idx) + 1)) else "NO",
                     unheld if unheld else "none", preceding if preceding else "none"))


def frame_times(session):
    """Per-captured-frame wall deltas from the capture's own t_wall stamps (m11).

    THIS IS ONLY A FRAME-TIME MEASUREMENT WITH PACING OFF. With IAI.Capture.Pace 1 the
    pacer sleeps each tick up to 1/VideoFps, which is exactly the variance a hitch test
    is trying to see, so a paced leg reports the pacer and not the cost (the m35 G-M6
    lesson). A paced leg is still printed, and labelled, so the reading cannot be
    mistaken for the other one.
    """
    rows = load_rows(session)
    stamps = [(r.get("session_index"), r.get("t_wall")) for r in rows
              if isinstance(r.get("t_wall"), (int, float))]
    stamps.sort(key=lambda p: p[0])
    deltas = [(stamps[i][0], (stamps[i][1] - stamps[i - 1][1]) * 1000.0)
              for i in range(1, len(stamps))]
    if not deltas:
        print("  FRAME TIME: no t_wall stamps in this session - nothing to measure")
        return None
    vals = sorted(d for _si, d in deltas)
    n = len(vals)
    p99 = vals[min(n - 1, int(round(0.99 * (n - 1))))]
    p95 = vals[min(n - 1, int(round(0.95 * (n - 1))))]
    worst = max(deltas, key=lambda p: p[1])
    paced = None
    rs = os.path.join(session, "run_summary.json")
    if os.path.isfile(rs):
        paced = json.load(open(rs, encoding="utf-8")).get("paced")
    print("  FRAME TIME (ms, from t_wall deltas over %d captured frames) paced=%s%s"
          % (n, paced, "   <-- PACED: this measures the pacer, not the cost" if paced else ""))
    print("    min=%.3f  median=%.3f  mean=%.3f  p95=%.3f  p99=%.3f  max=%.3f (at si=%s)"
          % (vals[0], vals[n // 2], sum(vals) / n, p95, p99, worst[1], worst[0]))
    return {"n": n, "min": vals[0], "median": vals[n // 2], "mean": sum(vals) / n,
            "p95": p95, "p99": p99, "max": worst[1], "max_si": worst[0], "paced": paced}


def main():
    args = [a for a in sys.argv[1:] if a != "--frame-time"]
    if "--frame-time" in sys.argv[1:]:
        stats = []
        for s in args:
            print("=" * 100)
            print("SESSION %s" % s)
            stats.append((s, frame_times(s)))
        if len(stats) == 2 and stats[0][1] and stats[1][1]:
            a, b = stats[0][1], stats[1][1]
            print()
            print("DELTA (second minus first), ms:  max %+.3f   p99 %+.3f   p95 %+.3f   mean %+.3f"
                  % (b["max"] - a["max"], b["p99"] - a["p99"], b["p95"] - a["p95"], b["mean"] - a["mean"]))
            print("A difference not larger than the within-build spread is BELOW THE RESOLUTION OF THIS")
            print("INSTRUMENT (G169), never 'no cost'.")
        return
    for s in args:
        report(s)


if __name__ == "__main__":
    main()
