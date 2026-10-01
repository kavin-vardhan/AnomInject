"""anomaly_refusal_counts.py - why an anomaly did or did not fire, from a game or editor log. It prints numbers only.

  python anomaly_refusal_counts.py "<the game's or the editor's log>"
  python anomaly_refusal_counts.py --selftest

It reads the plugin's own log wording and prints, as counts:
  - the IAI-STARTUP line(s): world type, editor or not, Nanite policy, Nanite probe registered or MISSING, r.VirtualTextures,
    r.Nanite.ProjectEnabled;
  - Auto.Yield lines: per anomaly type, the reasons it had zero eligible candidates;
  - auto-pool and targeted fires per anomaly type: applied / not applied;
  - the Nanite gate: REFUSED-NANITE, REFUSED-NANITE-PROBE-MISSING;
  - uv_corruption / normal_corruption: applied, refused per reason, slot dispositions, TEXCORRUPT-SHADERMAP (render-thread
    complete or not), and the IAI-TEXCORRUPT-CENSUS lines;
  - stuck_low_mip: the m52_log_counts.py readings (the same folder).
No actor, component, material, texture, path or map name is printed: a reason's detail after ':' is dropped.
Standard library only (Python 3.8 or newer). Exit codes: 0 read, 1 selftest failed, 2 cannot read the log.
"""
import argparse
import os
import re
import sys
import tempfile
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import m52_log_counts as m52

STARTUP = re.compile(r"IAI-STARTUP world='[^']*' type=(\w+) editor=(\d) nanite_policy=(\S+) nanite_probe=(\w+) "
                     r"virtual_textures=(-?\d+) nanite_project=(-?\d+)")
YIELD = re.compile(r"Auto\.Yield ([a-z_]+): 0 of (\d+) candidates eligible - (.*?) \((auto-pool|targeted), (\d+) round")
YIELD_ITEM = re.compile(r"([a-z_]+)(?::\S*)? (\d+)")
AUTO_FIRE = re.compile(r"Auto\.Fire: '([a-z_]+)' on '.*?' -> (applied|0 matched)\.")
AUTO_SPEC = re.compile(r"Auto\.FireSpecific: '([a-z_]+)' on '.*?' -> (applied|not applied|0 matched|refused)")
AUTO_DRAW = re.compile(r"Auto\.Draw attempt=\d+ .*? id=([a-z_-]+) .*? result=(\w+)")
NANITE = re.compile(r"REFUSED-NANITE actor=")
NANITE_PROBE = re.compile(r"REFUSED-NANITE-PROBE-MISSING actor=")
TC_HEAD = re.compile(r"TEXCORRUPT-(DECIDE|REFUSED) family=(uv|normal) mode=\S+ target='.*?' final=([A-Za-z_]+)")
TC_SLOT = re.compile(r"TEXCORRUPT-(?:DECIDE|REFUSED)\s+slot \S+ disposition=([a-z_]+)")
TC_SHADERMAP = re.compile(r"TEXCORRUPT-SHADERMAP '.*?' game_thread_complete=(\d) render_thread_complete=(\d)")
CENSUS = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 (scope=\S+ candidates=\d+|id=\S+ eligible=\d+ refused=\d+|"
                    r"scanned=\d+ of=\d+ gone=\d+ stopped=\w+ seconds=[\d.]+|end stats_unchanged=\d)(?: reasons=(\S+))?")


def reason_heads(text):
    out = Counter()
    for part in (text or "").split(","):
        if not part or part == "-":
            continue
        key, _, n = part.rpartition(":")
        if not n.isdigit():
            continue
        out[key.split(":")[0]] += int(n)
    return out


def count(lines):
    c = dict(startup=[], yield_lines=Counter(), yield_reasons={}, yield_candidates=Counter(), auto=Counter(), spec=Counter(),
             draw=Counter(), nanite=0, nanite_probe=0, tc_applied=Counter(), tc_refused={}, tc_slots=Counter(),
             tc_shadermap=Counter(), census=[])
    for raw in lines:
        line = raw.rstrip("\r\n")
        m = STARTUP.search(line)
        if m:
            c["startup"].append(m.groups())
            continue
        m = YIELD.search(line)
        if m:
            t = m.group(1)
            c["yield_lines"][t] += 1
            c["yield_candidates"][t] += int(m.group(2))
            r = c["yield_reasons"].setdefault(t, Counter())
            for k, n in YIELD_ITEM.findall(m.group(3)):
                r[k] += int(n)
            continue
        m = AUTO_FIRE.search(line)
        if m:
            c["auto"][(m.group(1), m.group(2))] += 1
            continue
        m = AUTO_SPEC.search(line)
        if m:
            c["spec"][(m.group(1), m.group(2))] += 1
            continue
        m = AUTO_DRAW.search(line)
        if m:
            c["draw"][(m.group(1), m.group(2))] += 1
            continue
        if NANITE_PROBE.search(line):
            c["nanite_probe"] += 1
            continue
        if NANITE.search(line):
            c["nanite"] += 1
            continue
        m = TC_HEAD.search(line)
        if m:
            fam = "uv_corruption" if m.group(2) == "uv" else "normal_corruption"
            if m.group(1) == "DECIDE":
                c["tc_applied"][fam] += 1
            else:
                c["tc_refused"].setdefault(fam, Counter())[m.group(3)] += 1
            continue
        m = TC_SLOT.search(line)
        if m:
            c["tc_slots"][m.group(1)] += 1
            continue
        m = TC_SHADERMAP.search(line)
        if m:
            c["tc_shadermap"]["render_thread_complete=%s" % m.group(2)] += 1
            continue
        m = CENSUS.search(line)
        if m:
            head = m.group(1)
            if m.group(2):
                rs = reason_heads(m.group(2))
                head += " reasons=" + (",".join("%s:%d" % (k, rs[k]) for k in sorted(rs)) or "-")
            c["census"].append(head)
    return c


def fmt_counter(cn):
    return " | ".join("%s %d" % (k, v) for k, v in sorted(cn.items(), key=lambda kv: (-kv[1], kv[0]))) or "-"


def render(c, m52c):
    out = []
    if c["startup"]:
        for s in c["startup"]:
            out.append("STARTUP                   type %s | editor %s | nanite_policy %s | nanite_probe %s | "
                       "r.VirtualTextures %s | r.Nanite.ProjectEnabled %s" % s)
    else:
        out.append("STARTUP                   no IAI-STARTUP line (a build before 090-10b, or not this plugin's log)")
    types = sorted(set(list(c["yield_lines"]) + [k[0] for k in c["auto"]] + [k[0] for k in c["spec"]]))
    for t in types:
        a = c["auto"].get((t, "applied"), 0) + c["spec"].get((t, "applied"), 0)
        n = sum(v for (tt, r), v in c["auto"].items() if tt == t and r != "applied")
        n += sum(v for (tt, r), v in c["spec"].items() if tt == t and r != "applied")
        out.append("FIRES %-19s applied %d | not applied %d" % (t, a, n))
        if c["yield_lines"].get(t):
            out.append("  ZERO-ELIGIBLE lines     %d (candidates %d): %s" % (
                c["yield_lines"][t], c["yield_candidates"][t], fmt_counter(c["yield_reasons"][t])))
    if c["draw"]:
        out.append("AUTO.DRAW results         %s" % fmt_counter(Counter({"%s:%s" % k: v for k, v in c["draw"].items()})))
    out.append("NANITE GATE               refused %d | refused probe-missing %d" % (c["nanite"], c["nanite_probe"]))
    for fam in ("uv_corruption", "normal_corruption"):
        out.append("%-25s applied %d | refused %d: %s" % (
            fam.upper(), c["tc_applied"].get(fam, 0), sum(c["tc_refused"].get(fam, Counter()).values()),
            fmt_counter(c["tc_refused"].get(fam, Counter()))))
    out.append("TEXCORRUPT slots          %s" % fmt_counter(c["tc_slots"]))
    out.append("TEXCORRUPT-SHADERMAP      %s" % fmt_counter(c["tc_shadermap"]))
    for line in c["census"]:
        out.append("CENSUS                    %s" % line)
    out.append("STUCK_LOW_MIP (m52_log_counts.py):")
    out.extend("  " + l for l in m52.render(m52c))
    return out


SAMPLE = [
    "[2026.10.01-08.00.00:000][  1]LogAnomaly: Display: IAI-STARTUP world='SecretMap' type=PIE editor=1 "
    "nanite_policy=refused(compiled) nanite_probe=registered virtual_textures=1 nanite_project=0 - Nanite targets are refused",
    "[2026.10.01-08.00.01:000][  2]LogAnomaly: TEXCORRUPT-REFUSED family=uv mode=tile target='=SM_SecretCrate_7' "
    "final=shader_map_incomplete step=V1 slots=1 qualified=0",
    "[2026.10.01-08.00.01:000][  2]LogAnomaly: TEXCORRUPT-REFUSED   slot SM_SecretCrate_7.StaticMeshComponent0[0] "
    "disposition=shader_map_incomplete raw=MI_Secret(len 1)",
    "[2026.10.01-08.00.02:000][  3]LogAnomaly: TEXCORRUPT-REFUSED family=normal mode=n_rotate target='=SM_SecretCrate_7' "
    "final=over_budget:need_1_available_0_cap_134217728 step=V2",
    "[2026.10.01-08.00.03:000][  4]LogAnomaly: TEXCORRUPT-DECIDE family=uv mode=tile target='=SM_SecretBarrel_2' final=APPLY step=-",
    "[2026.10.01-08.00.03:000][  4]LogAnomaly: TEXCORRUPT-DECIDE   slot SM_SecretBarrel_2.StaticMeshComponent0[0] "
    "disposition=qualified raw=MI_Secret2(len 1)",
    "[2026.10.01-08.00.03:000][  4]LogAnomaly: TEXCORRUPT-SHADERMAP 'MI_Secret2' game_thread_complete=0 render_thread_complete=1 "
    "-> renders as itself.",
    "[2026.10.01-08.00.04:000][  5]LogAnomaly: Warning: Auto.Yield uv_corruption: 0 of 7 candidates eligible - "
    "shader_map_incomplete 5, virtual_texture 2 (auto-pool, 3 round(s) since the last line). Nothing of this type",
    "[2026.10.01-08.00.04:000][  5]LogAnomaly: Auto.Fire: 'uv_corruption' on 'SM_SecretCrate_7' -> 0 matched.",
    "[2026.10.01-08.00.05:000][  6]LogAnomaly: Auto.FireSpecific: 'uv_corruption' on 'SM_SecretBarrel_2' -> applied.",
    "[2026.10.01-08.00.06:000][  7]LogAnomaly: Error: REFUSED-NANITE-PROBE-MISSING actor='SM_SecretRock' class=StaticMeshActor",
    "[2026.10.01-08.00.06:000][  7]LogAnomaly: Warning: REFUSED-NANITE actor='SM_SecretRock2' class=StaticMeshActor site=auto_pool",
    "[2026.10.01-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 scope=all candidates=12 cap_bytes=134217728 "
    "uv_modes=tile normal_modes=n_rotate",
    "[2026.10.01-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 id=uv_corruption eligible=3 refused=9 "
    "reasons=over_budget:4,virtual_texture:5",
    "[2026.10.01-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 scanned=12 of=12 gone=0 stopped=complete seconds=0.4",
    "[2026.10.01-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 end stats_unchanged=1",
]


def selftest():
    lines = []
    ok_all = True

    def check(name, ok):
        nonlocal ok_all
        ok_all = ok_all and ok
        lines.append("SELFTEST %-78s %s" % (name, "OK" if ok else "FAILED"))

    c = count(SAMPLE)
    r = render(c, m52.count(SAMPLE))
    joined = "\n".join(r)
    check("startup line read (PIE, editor 1, probe registered, VT 1)", c["startup"] == [
        ("PIE", "1", "refused(compiled)", "registered", "1", "0")])
    check("uv refused 1 (shader_map_incomplete), applied 1; normal refused over_budget 1",
          c["tc_refused"]["uv_corruption"] == Counter(shader_map_incomplete=1) and c["tc_applied"]["uv_corruption"] == 1
          and c["tc_refused"]["normal_corruption"] == Counter(over_budget=1))
    check("slot dispositions: shader_map_incomplete 1, qualified 1",
          c["tc_slots"] == Counter(shader_map_incomplete=1, qualified=1))
    check("shadermap line counted by its render-thread flag", c["tc_shadermap"] == Counter({"render_thread_complete=1": 1}))
    check("yield line: uv 0 of 7, shader_map_incomplete 5 virtual_texture 2",
          c["yield_candidates"]["uv_corruption"] == 7
          and c["yield_reasons"]["uv_corruption"] == Counter(shader_map_incomplete=5, virtual_texture=2))
    check("fires: uv applied 1 (targeted), not applied 1 (auto-pool)",
          "FIRES uv_corruption       applied 1 | not applied 1" in joined)
    check("Nanite gate: refused 1, probe-missing 1 (the probe-missing line is not double counted)",
          c["nanite"] == 1 and c["nanite_probe"] == 1)
    check("census: 4 lines, reasons kept as counts", len(c["census"]) == 4
          and "reasons=over_budget:4,virtual_texture:5" in c["census"][1])
    check("output carries no actor, material, map or path name",
          not re.search(r"Secret|SM_|MI_|\\|/Game", joined))
    check("over_budget detail (need/available/cap) is dropped", "need_" not in joined)
    tmp = tempfile.mkdtemp(prefix="refusal_counts_")
    try:
        for enc in ("utf-8-sig", "utf-16", "utf-8"):
            p = os.path.join(tmp, "game_%s.log" % enc)
            with open(p, "w", encoding=enc, newline="\r\n") as f:
                f.write("\n".join(SAMPLE) + "\n")
            got = count(m52.read_lines(p))
            check("the same counts from a %s file" % enc, got == c)
    finally:
        for fn in os.listdir(tmp):
            os.remove(os.path.join(tmp, fn))
        os.rmdir(tmp)
    empty = render(count([]), m52.count([]))
    check("an empty log prints zeros and says the startup line is absent", "no IAI-STARTUP line" in empty[0])
    for l in lines:
        print(l)
    print("SELFTEST %d check(s): %s" % (len(lines), "OK" if ok_all else "FAILED"))
    return 0 if ok_all else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="Why anomalies did or did not fire, from a game or editor log: numbers only.")
    ap.add_argument("log", nargs="?", help="the game's or the editor's log file")
    ap.add_argument("--selftest", action="store_true", help="run the known-answer self test")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest()
    if not a.log:
        ap.print_help()
        return 2
    if not os.path.isfile(a.log):
        print("cannot read the log: file not found")
        return 2
    lines = list(m52.read_lines(a.log))
    for l in render(count(lines), m52.count(lines)):
        print(l)
    return 0


if __name__ == "__main__":
    sys.exit(main())
