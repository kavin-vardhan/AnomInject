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
        print("  %-5s %-9s %-9s %-8s %-6s %-6s %-6s %-6s %-10s %s"
              % ("si", "resident", "baseline", "forced", "floor", "held", "obs", "tgtpx", "topres_px", "flags"))
        held_true = 0
        first_labelled = None
        for si, r, a in stuck_rows:
            held = a.get("stuck_mip.held")
            obs = a.get("observable")
            flags = [k.rsplit(".", 1)[-1] for k in a
                     if k.startswith("stuck_mip.fail_") or k == "stuck_mip.bench_no_hold"]
            if held:
                held_true += 1
            if first_labelled is None:
                first_labelled = si
            print("  %-5s %-9s %-9s %-8s %-6s %-6s %-6s %-6s %-10s %s"
                  % (si, a.get("stuck_mip.resident_mips"), a.get("stuck_mip.baseline_mips"),
                     a.get("stuck_mip.forced_mips"), a.get("stuck_mip.floor_mips"),
                     held, obs, a.get("target_pixels"), a.get("stuck_mip.top_resident_px"),
                     ",".join(flags) if flags else "-"))
        print()
        print("  HELD true on %d of %d labelled frames" % (held_true, len(stuck_rows)))
        print("  FIRST labelled session_index = %s" % first_labelled)
        f0 = stuck_rows[0][2]
        print("  onset check: first labelled frame resident %s < baseline %s  -> %s"
              % (f0.get("stuck_mip.resident_mips"), f0.get("stuck_mip.baseline_mips"),
                 "YES" if (f0.get("stuck_mip.resident_mips") is not None
                           and f0.get("stuck_mip.baseline_mips") is not None
                           and f0["stuck_mip.resident_mips"] < f0["stuck_mip.baseline_mips"]) else "NO"))
        print("  texture=%s co_affected_visible=%s textures_armed=%s"
              % (f0.get("stuck_mip.texture"), f0.get("stuck_mip.co_affected_visible"),
                 f0.get("stuck_mip.textures_armed")))

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
        for e in ev:
            af = e.get("affected_frames", {}) or {}
            inj = e.get("injected_frames", {}) or {}
            print("    type=%-14s manifested=%-5s observability_measured=%-5s injected=%s affected=%s"
                  % (e.get("anomaly_type"), e.get("manifested"), e.get("observability_measured"),
                     inj.get("frame_indices"), af.get("frame_indices")))


def main():
    for s in sys.argv[1:]:
        report(s)


if __name__ == "__main__":
    main()
