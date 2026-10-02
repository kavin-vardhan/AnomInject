"""anomaly_refusal_counts.py - why an anomaly did or did not fire, from a game or editor log. It prints numbers only.

  python anomaly_refusal_counts.py "<the game's or the editor's log>"
  python anomaly_refusal_counts.py --selftest

It reads the plugin's own log wording and prints, as counts:
  - the IAI-STARTUP line(s): world type, editor or not, Nanite policy, Nanite probe registered or MISSING, r.VirtualTextures,
    r.Nanite.ProjectEnabled;
  - Auto.Yield lines: per anomaly type, the reasons it had zero eligible candidates;
  - auto-pool and targeted fires per anomaly type: applied / not applied;
  - the Nanite gate: REFUSED-NANITE, REFUSED-NANITE-PROBE-MISSING;
  - uv_corruption / normal_corruption: applied, refused per reason, slot dispositions, TEXCORRUPT-SHADERMAP (admitted or
    refused on the vertex-factory shaders), and the IAI-TEXCORRUPT-CENSUS lines;
  - stuck_low_mip: the m52_log_counts.py readings (the same folder);
  - since 090-10c: the census 'subs' lines (first refusal with its sub-reason, reason/sub) and the 'allreasons' table
    (IAI.TexCorrupt.Census allreasons: every blocking key with objects failing it and failing ONLY it, the notes, the top
    combinations). The plugin reduces any sub that could name content to the reason; this tool keeps a key only if it is
    made of [A-Za-z0-9_./].
  python anomaly_refusal_counts.py --allreasons-check "<log>"   exit 0 when the allreasons table shows at least one
    object failing more than one check (a first-failure census never can), 1 when it does not, 2 when there is no table.
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
YIELD_ITEM = re.compile(r"([a-z_]+(?:/[A-Za-z0-9_.]+)?)(?::\S*)? (\d+)")
KEY = r"[A-Za-z0-9_./]+"
IDS = r"((?:uv|normal)_corruption|stuck_low_mip)"
SUBS = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 subs id=" + IDS + r" subs=(-|" + KEY + r":\d+(?:," + KEY + r":\d+)*)\s*$")
AR_HEAD = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 allreasons id=" + IDS + r" objects=(\d+) eligible=(\d+) blocked=(\d+) "
                     r"unassessed=(\d+) b_gain_objects=(\d+)\s*$")
AR_KEY = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 allreasons id=" + IDS + r" key=(" + KEY + r") objects=(\d+) only=(\d+)\s*$")
AR_NOTE = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 allreasons id=" + IDS + r" note=(" + KEY + r") objects=(\d+)\s*$")
AR_COMBO = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 allreasons id=" + IDS + r" combo=(" + KEY + r"(?:\+" + KEY + r")*) "
                      r"objects=(\d+)\s*$")
AUTO_FIRE = re.compile(r"Auto\.Fire: '([a-z_]+)' on '.*?' -> (applied|0 matched)\.")
AUTO_SPEC = re.compile(r"Auto\.FireSpecific: '([a-z_]+)' on '.*?' -> (applied|not applied|0 matched|refused)")
AUTO_DRAW = re.compile(r"Auto\.Draw attempt=\d+ .*? id=([a-z_-]+) .*? result=(\w+)")
NANITE = re.compile(r"REFUSED-NANITE actor=")
NANITE_PROBE = re.compile(r"REFUSED-NANITE-PROBE-MISSING actor=")
TC_HEAD = re.compile(r"TEXCORRUPT-(DECIDE|REFUSED) family=(uv|normal|blur) mode=\S+ target='.*?' final=([A-Za-z_]+)")
ROUTES = re.compile(r"IAI-TEXCORRUPT-CENSUS v1 routes id=stuck_low_mip hold_eligible=(\d+) proxy_eligible=(\d+) auto_hold=(\d+) auto_proxy=(\d+) selected=(auto|hold|proxy)\s*$")
ROUTE_FIRE = re.compile(r"stuck_low_mip: ROUTE route=(hold|proxy) result=applied ")
TC_SLOT = re.compile(r"TEXCORRUPT-(?:DECIDE|REFUSED)\s+slot \S+ disposition=([a-z_]+)")
TC_SHADERMAP = re.compile(r"TEXCORRUPT-SHADERMAP '.*?' game_thread_complete=(\d) .*?-> (admitted|refused)(?: (\w+))?")
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
             tc_shadermap=Counter(), census=[], subs={}, allreasons={}, routes=[], route_fires=Counter())
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
            fam = dict(uv="uv_corruption", normal="normal_corruption", blur="stuck_low_mip")[m.group(2)]
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
            c["tc_shadermap"][m.group(2) + (":" + m.group(3) if m.group(3) else "")] += 1
            continue
        m = SUBS.search(line)
        if m:
            sc = c["subs"].setdefault(m.group(1), Counter())
            if m.group(2) != "-":
                for part in m.group(2).split(","):
                    k, _, n = part.rpartition(":")
                    sc[k] += int(n)
            continue
        m = AR_HEAD.search(line)
        if m:
            a = c["allreasons"].setdefault(m.group(1), dict(head=None, keys={}, notes=Counter(), combos=Counter()))
            a["head"] = tuple(int(x) for x in m.groups()[1:])
            continue
        m = AR_KEY.search(line)
        if m:
            a = c["allreasons"].setdefault(m.group(1), dict(head=None, keys={}, notes=Counter(), combos=Counter()))
            a["keys"][m.group(2)] = (int(m.group(3)), int(m.group(4)))
            continue
        m = AR_NOTE.search(line)
        if m:
            a = c["allreasons"].setdefault(m.group(1), dict(head=None, keys={}, notes=Counter(), combos=Counter()))
            a["notes"][m.group(2)] = int(m.group(3))
            continue
        m = AR_COMBO.search(line)
        if m:
            a = c["allreasons"].setdefault(m.group(1), dict(head=None, keys={}, notes=Counter(), combos=Counter()))
            a["combos"][m.group(2)] = int(m.group(3))
            continue
        m = ROUTES.search(line)
        if m:
            c["routes"].append(tuple(int(x) for x in m.groups()[:4]) + (m.group(5),))
            continue
        m = ROUTE_FIRE.search(line)
        if m:
            c["route_fires"][m.group(1)] += 1
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
    for fam in ("uv_corruption", "normal_corruption", "stuck_low_mip"):
        out.append("%-25s applied %d | refused %d: %s" % (
            fam.upper(), c["tc_applied"].get(fam, 0), sum(c["tc_refused"].get(fam, Counter()).values()),
            fmt_counter(c["tc_refused"].get(fam, Counter()))))
    out.append("TEXCORRUPT slots          %s" % fmt_counter(c["tc_slots"]))
    out.append("TEXCORRUPT-SHADERMAP      %s" % fmt_counter(c["tc_shadermap"]))
    for line in c["census"]:
        out.append("CENSUS                    %s" % line)
    for routes in c["routes"]:
        out.append("CENSUS ROUTES stuck_low_mip hold_eligible %d | proxy_eligible %d | auto_hold %d | auto_proxy %d | selected %s" % routes)
    out.append("STUCK_LOW_MIP ROUTE FIRES  hold %d | proxy %d" % (c["route_fires"]["hold"], c["route_fires"]["proxy"]))
    for fam in sorted(c["subs"]):
        out.append("CENSUS SUBS %-13s %s" % (fam, fmt_counter(c["subs"][fam])))
    for fam in sorted(c["allreasons"]):
        a = c["allreasons"][fam]
        if a["head"]:
            out.append("ALL-REASONS %-13s objects %d | eligible %d | blocked %d | unassessed %d | b_gain_objects %d" % ((fam,) + a["head"]))
        for k, (n, only) in sorted(a["keys"].items(), key=lambda kv: (-kv[1][0], kv[0])):
            out.append("  key  %-60s objects %6d | only %6d" % (k, n, only))
        for k, n in sorted(a["notes"].items(), key=lambda kv: (-kv[1], kv[0])):
            out.append("  note %-60s objects %6d" % (k, n))
        for k, n in sorted(a["combos"].items(), key=lambda kv: (-kv[1], kv[0])):
            out.append("  combo %-59s objects %6d" % (k, n))
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
    "[2026.10.01-08.00.03:000][  4]LogAnomaly: TEXCORRUPT-SHADERMAP 'MI_Secret2' game_thread_complete=0 compilation_finished=1 "
    "vertex_factory=FLocalVertexFactory shaders=12 -> admitted. A whole-map completeness flag",
    "[2026.10.01-08.00.03:000][  4]LogAnomaly: TEXCORRUPT-SHADERMAP 'MI_Secret3' game_thread_complete=1 compilation_finished=1 "
    "vertex_factory=FInstancedStaticMeshVertexFactory shaders=0 base_pass_vs=0 base_pass_ps=0 -> refused no_vertex_factory_shaders. The",
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


def allreasons_check(c):
    if not c["allreasons"]:
        return 2, "no allreasons table in this log"
    multi = []
    for fam, a in sorted(c["allreasons"].items()):
        combos = sum(n for k, n in a["combos"].items() if "+" in k)
        partial = sum(1 for k, (n, only) in a["keys"].items() if only < n)
        if combos or partial:
            multi.append("%s: %d object(s) in multi-key combinations, %d key(s) with only < objects" % (fam, combos, partial))
    if multi:
        return 0, "PASS - every check is evaluated: " + "; ".join(multi)
    return 1, ("FAIL - no object fails more than one check (only == objects for every key, no combination): that is a "
               "first-failure census, not an all-reasons one")


def route_check(c):
    head = c["allreasons"].get("stuck_low_mip", {}).get("head")
    if not c["routes"] or not head:
        return 2, "missing blur route or allreasons counts"
    hold, proxy, auto_hold, auto_proxy, selected = c["routes"][-1]
    expected = dict(auto=auto_hold + auto_proxy, hold=hold, proxy=proxy)[selected]
    ok = head[1] == expected and auto_hold <= hold and auto_proxy <= proxy and max(hold, proxy) <= head[0]
    return (0 if ok else 1), ("PASS" if ok else "FAIL") + " - per-route eligible accounting"


AR_SAMPLE_REAL = [
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 subs id=uv_corruption "
    "subs=runtime_lod_bias/streaming_budget:6,host_mid/override.mid:2",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption objects=10 eligible=2 "
    "blocked=8 unassessed=0 b_gain_objects=1",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "key=runtime_lod_bias/streaming_budget objects=6 only=2",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "key=not_fully_resident objects=4 only=0",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "combo=not_fully_resident+runtime_lod_bias/streaming_budget objects=4",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "key=nanite_override/M Secret objects=1 only=1",
]
AR_SAMPLE_FIRST_ONLY = [
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption objects=10 eligible=2 "
    "blocked=8 unassessed=0 b_gain_objects=1",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "key=runtime_lod_bias/streaming_budget objects=6 only=6",
    "[2026.10.02-08.00.07:000][  8]LogAnomaly: Display: IAI-TEXCORRUPT-CENSUS v1 allreasons id=uv_corruption "
    "key=host_mid/override.mid objects=2 only=2",
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
    check("shadermap lines counted by verdict and reason (admitted 1, refused no_vertex_factory_shaders 1)",
          c["tc_shadermap"] == Counter({"admitted": 1, "refused:no_vertex_factory_shaders": 1}))
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
    ar = count(AR_SAMPLE_REAL + [SAMPLE[4]])
    check("yield item with a sub-reason is read whole (runtime_lod_bias/per_texture 21)",
          count(["LogAnomaly: Warning: Auto.Yield uv_corruption: 0 of 21 candidates eligible - runtime_lod_bias/per_texture 21 "
                 "(auto-pool, 1 round(s) since the last line)."])["yield_reasons"]["uv_corruption"]
          == Counter({"runtime_lod_bias/per_texture": 21}))
    check("subs line read as reason/sub counts",
          ar["subs"]["uv_corruption"] == Counter({"runtime_lod_bias/streaming_budget": 6, "host_mid/override.mid": 2}))
    check("allreasons table read (head, 2 keys, 1 combo); a key carrying a space is dropped",
          ar["allreasons"]["uv_corruption"]["head"] == (10, 2, 8, 0, 1) and len(ar["allreasons"]["uv_corruption"]["keys"]) == 2
          and ar["allreasons"]["uv_corruption"]["combos"]["not_fully_resident+runtime_lod_bias/streaming_budget"] == 4)
    check("allreasons-check PASSes an all-reasons table", allreasons_check(ar)[0] == 0)
    check("allreasons-check FAILs a first-failure table (the mutant that stops at the first failure)",
          allreasons_check(count(AR_SAMPLE_FIRST_ONLY))[0] == 1)
    check("allreasons-check says 2 when there is no table", allreasons_check(count(SAMPLE))[0] == 2)
    check("rendered all-reasons table carries no name", not re.search(r"Secret| M ", "\n".join(render(ar, m52.count([])))))
    blur = [x.replace("uv_corruption", "stuck_low_mip") for x in AR_SAMPLE_REAL]
    blur += ["IAI-TEXCORRUPT-CENSUS v1 routes id=stuck_low_mip hold_eligible=1 proxy_eligible=2 auto_hold=1 auto_proxy=1 selected=auto",
             "stuck_low_mip: ROUTE route=proxy result=applied target='secret' k=4", "stuck_low_mip: ROUTE route=hold result=applied target='secret' k=3"]
    bc = count(blur)
    check("blur allreasons, per-route census and fire counts are read", bc["routes"] == [(1, 2, 1, 1, "auto")]
          and bc["route_fires"] == Counter(hold=1, proxy=1) and allreasons_check(bc)[0] == 0)
    check("blur first-failure mutant fails allreasons check", allreasons_check(count(
          [x.replace("uv_corruption", "stuck_low_mip") for x in AR_SAMPLE_FIRST_ONLY]))[0] == 1)
    check("missing route mutant cannot report route counts", not count(blur[:-3])["routes"])
    check("swapped route values are distinguishable", count([blur[-3].replace("auto_hold=1", "auto_hold=0")])["routes"] != bc["routes"])
    check("blur output carries no name", "secret" not in "\n".join(render(bc, m52.count([]))))
    check("route accounting passes consistent counts", route_check(bc)[0] == 0)
    check("route accounting rejects missing route mutant", route_check(count(blur[:-3]))[0] == 2)
    check("route accounting rejects inconsistent eligible mutant", route_check(count(
          [x.replace("auto_hold=1", "auto_hold=0") for x in blur]))[0] == 1)
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
    ap.add_argument("--allreasons-check", action="store_true",
                    help="exit 0 if the log's allreasons table shows objects failing more than one check, 1 if not, 2 if absent")
    ap.add_argument("--route-check", action="store_true", help="check blur per-route eligible accounting")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest()
    if a.allreasons_check or a.route_check:
        if not a.log or not os.path.isfile(a.log):
            print("cannot read the log: file not found")
            return 2
        rc, msg = (route_check if a.route_check else allreasons_check)(count(list(m52.read_lines(a.log))))
        print(("ROUTE-CHECK " if a.route_check else "ALLREASONS-CHECK ") + msg)
        return rc
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
