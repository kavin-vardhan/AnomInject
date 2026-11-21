"""m56 G5: every frame the release build wrote must be unchanged in the m56 build.

  python m56_nonsettle_compare.py <release session> <m56 session> [--control <second release session>] [--json out]
  python m56_nonsettle_compare.py --selftest

Rows are joined by tick offset (frame_index minus the session's first frame_index), not by session_index: the
release skipped two ticks after every edge, so its session_index drifts against the m56 build's. For every release
row the m56 row at the same offset must exist, must not be a settle frame, and must equal the release row on every
field (labels, masks, boxes, view, entry fields) except:
  - the renumbering keys session_index, image, mask_file, and m56's new tick_index / t_wall_run;
  - absolute clocks compared as offsets from each session's first row (frame_index, t, entry start_frame, event_id);
  - t_wall (absolute wall clock);
  - the run-unique set, measured when --control names a second release run of the same recipe: every field that
    differs between the two release runs is allowed, and is listed (A64 pose precondition first).
Only the tick span both sessions cover is compared; m56 rows at settle ticks have no release partner and are counted.
Intended m56 difference (095-06b amendment 1 to the predictions, S6): exposure_dip and exposure_dip_scope may differ on
non-settle frames, because the rolling 8-frame exposure window now includes the settle frames (plan 9 risk 8). They are
counted and listed separately and never fail; any other new difference fails.
Scope (CB-N1): labels.jsonl, plus (095-14 amendment 2 to the predictions, ruling 095-12 D2) the node identity of every
annotation.json event both sessions hold: affected_objects (count, primary_index and each node's name, path,
global_position, asset_name, component_class, bounds) must be equal. Events are joined by event_id with its frame number
taken as an offset from the session's first frame_index; an event only one session holds is listed as a reading, never
failed (a veto or a refusal can differ between builds; the labels check covers the frames). Intended difference: on a
texture event that changed only some meshes of the object, asset_name and component_class may differ, because the
integration build names a changed mesh (m59 S3, shared code). Such an event is recognised from the newer session only:
an entry of the event carries texcorrupt.components_skipped > 0 (uv/normal corruption with skipped parts), or the
annotation event says stuck_mip_route "proxy" (that route scopes the event the same way). These are counted and listed
by cause and never fail; any other node difference fails. annotation.json frame lists, run_summary and mask pixels are
still not compared here; m56_continuity_check.py and verify_capture.py --label-rule read those.
Pose precondition (A64): if the modal view rotation of the two sessions differs, the pair is INVALID, not FAILED.

Exit 0 PASS, 1 FAIL, 2 cannot run (missing labels.jsonl or annotation.json, or an annotation event without event_id),
3 INVALID (pose not matched).
"""
import argparse
import copy
import json
import os
import shutil
import sys
import tempfile

RENUMBER = ("session_index", "image", "mask_file", "tick_index", "t_wall_run")
ABSOLUTE_ROW = ("t_wall",)
NUM_TOL = 1e-9
SETTLE = "settle"
M56_INTENDED = ("exposure_dip", "exposure_dip_scope")
SCOPED_NODE_FIELDS = ("asset_name", "component_class")
TEXCORRUPT_SKIPPED = "texcorrupt.components_skipped"
CAUSE_SKIPPED = "texcorrupt_skipped_parts"
CAUSE_PROXY = "stuck_mip_proxy_route"


def read_rows(d):
    p = os.path.join(d, "labels.jsonl")
    if not os.path.isfile(p):
        return None
    with open(p, "r", encoding="utf-8-sig") as fh:
        rows = [json.loads(x) for x in fh if x.strip()]
    rows.sort(key=lambda r: r.get("session_index", 0))
    return rows


def read_annotation(d):
    p = os.path.join(d, "annotation.json")
    if not os.path.isfile(p):
        return None
    with open(p, "r", encoding="utf-8-sig") as fh:
        return json.load(fh)


def offset_event_id(eid, f0):
    if isinstance(eid, str) and "@" in eid and "|" in eid:
        head, rest = eid.split("@", 1)
        num, tail = rest.split("|", 1)
        try:
            return "%s@%d|%s" % (head, int(num) - f0, tail), int(num) - f0
        except ValueError:
            pass
    return None, None


def entry_event_key(a, f0):
    key, _ = offset_event_id(a.get("event_id"), f0)
    if key is None and isinstance(a.get("start_frame"), (int, float)):
        key, _ = offset_event_id("%s@%d|%s" % (a.get("id"), int(a["start_frame"]), a.get("target_name")), f0)
    return key


def annotation_events(rows, ann):
    f0 = rows[0]["frame_index"]
    out = {}
    unkeyed = 0
    for e in (ann or {}).get("anomalies") or []:
        key, off = offset_event_id(e.get("event_id"), f0)
        if key is None:
            unkeyed += 1
            continue
        out[key] = (off, e)
    return out, unkeyed


def scope_causes(rows, ann):
    f0 = rows[0]["frame_index"]
    causes = {}
    for r in rows:
        for a in r.get("anomalies") or []:
            v = a.get(TEXCORRUPT_SKIPPED)
            if isinstance(v, (int, float)) and not isinstance(v, bool) and v > 0:
                key = entry_event_key(a, f0)
                if key is not None:
                    causes[key] = CAUSE_SKIPPED
    for e in (ann or {}).get("anomalies") or []:
        if e.get("stuck_mip_route") == "proxy":
            key, _ = offset_event_id(e.get("event_id"), f0)
            if key is not None:
                causes.setdefault(key, CAUSE_PROXY)
    return causes


def node_field_key(p):
    parts = p.split("/")
    if len(parts) >= 3 and parts[1] == "nodes":
        return "node/" + parts[2]
    if len(parts) >= 2:
        return "node/" + parts[1]
    return "node"


def compare_nodes(rel_rows, rel_ann, m_rows, m_ann, span, allowed=frozenset()):
    A, _ua = annotation_events(rel_rows, rel_ann)
    B, _ub = annotation_events(m_rows, m_ann)
    causes = scope_causes(m_rows, m_ann)
    mismatch = {}
    first = {}
    intended = {}
    joined = 0
    only_rel = []
    only_new = []
    for k in sorted(set(A) | set(B)):
        off = A[k][0] if k in A else B[k][0]
        if off is not None and off > span:
            continue
        if k not in B:
            only_rel.append(k)
            continue
        if k not in A:
            only_new.append(k)
            continue
        joined += 1
        acc = []
        diff(A[k][1].get("affected_objects"), B[k][1].get("affected_objects"), ("node",), acc)
        for p in acc:
            fk = node_field_key(p)
            if fk in allowed:
                continue
            parts = p.split("/")
            if len(parts) == 3 and parts[1] == "nodes" and parts[2] in SCOPED_NODE_FIELDS and k in causes:
                intended.setdefault(causes[k], []).append("%s %s" % (k, parts[2]))
                continue
            mismatch[fk] = mismatch.get(fk, 0) + 1
            first.setdefault(fk, k)
    return dict(node_joined=joined, node_only_release=only_rel, node_only_new=only_new, node_mismatch=mismatch,
                node_first=first, node_intended=intended)


def is_settle(r):
    return isinstance(r.get("transition_reason"), list) and SETTLE in r["transition_reason"]


def modal_rot(rows):
    c = {}
    for r in rows:
        rot = tuple(round(float(x), 2) for x in ((r.get("view") or {}).get("rot") or []))
        c[rot] = c.get(rot, 0) + 1
    return max(c.items(), key=lambda kv: kv[1])[0] if c else None


def normalise(rows):
    f0 = rows[0]["frame_index"]
    t0 = rows[0].get("t", 0.0)
    out = {}
    for r in rows:
        o = r["frame_index"] - f0
        n = {k: copy.deepcopy(v) for k, v in r.items() if k not in RENUMBER and k not in ABSOLUTE_ROW}
        n["frame_index"] = o
        if isinstance(n.get("t"), (int, float)):
            n["t"] = round(n["t"] - t0, 6)
        ents = {}
        for a in n.get("anomalies") or []:
            a = dict(a)
            if isinstance(a.get("start_frame"), (int, float)):
                a["start_frame"] = a["start_frame"] - f0
            if isinstance(a.get("event_id"), str) and "@" in a["event_id"] and "|" in a["event_id"]:
                head, rest = a["event_id"].split("@", 1)
                num, tail = rest.split("|", 1)
                try:
                    a["event_id"] = "%s@%d|%s" % (head, int(num) - f0, tail)
                except ValueError:
                    pass
            key = (a.get("id"), a.get("target_name"), a.get("start_frame"))
            ents[key] = a
        n["anomalies"] = ents
        out[o] = (n, r)
    return out


def diff(a, b, path, acc):
    if isinstance(a, dict) and isinstance(b, dict):
        for k in set(a) | set(b):
            p = path + (str(k),)
            if k not in a or k not in b:
                acc.append("/".join(p))
            else:
                diff(a[k], b[k], p, acc)
        return
    if isinstance(a, list) and isinstance(b, list):
        if len(a) != len(b):
            acc.append("/".join(path))
            return
        for i, (x, y) in enumerate(zip(a, b)):
            diff(x, y, path, acc)
        return
    if isinstance(a, (int, float)) and isinstance(b, (int, float)) and not isinstance(a, bool) and not isinstance(b, bool):
        if abs(float(a) - float(b)) > NUM_TOL:
            acc.append("/".join(path))
        return
    if a != b:
        acc.append("/".join(path))


def field_key(p):
    parts = p.split("/")
    if parts[0] == "anomalies" and len(parts) >= 3:
        return "anomalies/<entry>/" + parts[-1]
    return parts[0]


def compare(rel_rows, m_rows, allowed=frozenset()):
    A = normalise(rel_rows)
    B = normalise(m_rows)
    span = min(max(A), max(B))
    counts = {}
    intended = {}
    first = {}
    joined = 0
    missing_partner = []
    partner_settle = []
    for o in sorted(A):
        if o > span:
            continue
        if o not in B:
            missing_partner.append(o)
            continue
        nb, rb = B[o]
        if is_settle(rb):
            partner_settle.append(o)
            continue
        joined += 1
        acc = []
        diff(A[o][0], nb, (), acc)
        for p in acc:
            k = field_key(p)
            if k in allowed:
                continue
            if k in M56_INTENDED:
                intended.setdefault(k, []).append(o)
                continue
            counts[k] = counts.get(k, 0) + 1
            first.setdefault(k, o)
    settle_rows = sum(1 for o, (_n, r) in B.items() if o <= span and is_settle(r))
    unpartnered_m56 = sorted(o for o, (_n, r) in B.items() if o <= span and not is_settle(r) and o not in A)
    return dict(span=span, joined=joined, missing_partner=missing_partner, partner_settle=partner_settle,
                settle_rows=settle_rows, unpartnered_m56=unpartnered_m56, field_mismatch=counts, first_offset=first,
                intended=intended)


def run_unique(c1, c2):
    res = compare(c1, c2)
    return set(res["field_mismatch"]), res


def annotation_problem(name, ann, rows):
    if ann is None:
        return "CANNOT RUN: %s has no annotation.json (the node identity check needs it)" % name
    _ev, unkeyed = annotation_events(rows, ann)
    if unkeyed:
        return "CANNOT RUN: %d annotation event(s) of %s carry no event_id, so events cannot be joined" % (unkeyed, name)
    return None


def main_compare(rel, m56, control=None):
    out = {"release": rel, "m56": m56}
    R = read_rows(rel)
    M = read_rows(m56)
    if not R or not M:
        out["verdict"] = "CANNOT RUN: missing labels.jsonl"
        return 2, out
    RA = read_annotation(rel)
    MA = read_annotation(m56)
    for name, ann, rows in (("the release session", RA, R), ("the m56 session", MA, M)):
        bad = annotation_problem(name, ann, rows)
        if bad:
            out["verdict"] = bad
            return 2, out
    ra, rb = modal_rot(R), modal_rot(M)
    out["modal_rot"] = [ra, rb]
    if ra != rb:
        out["verdict"] = "INVALID: the two legs rest in different camera poses (A64) - not compared"
        return 3, out
    allowed = set()
    node_allowed = set()
    if control:
        C = read_rows(control)
        if not C:
            out["verdict"] = "CANNOT RUN: control has no labels.jsonl"
            return 2, out
        CA = read_annotation(control)
        bad = annotation_problem("the control session", CA, C)
        if bad:
            out["verdict"] = bad
            return 2, out
        if modal_rot(C) != ra:
            out["verdict"] = "INVALID: the control pair rests in different camera poses (A64)"
            return 3, out
        allowed, cres = run_unique(R, C)
        out["run_unique_from_control"] = sorted(allowed)
        cnode = compare_nodes(R, RA, C, CA, cres["span"])
        node_allowed = set(cnode["node_mismatch"])
        out["node_run_unique_from_control"] = sorted(node_allowed)
    res = compare(R, M, frozenset(allowed))
    out.update(res)
    out.update(compare_nodes(R, RA, M, MA, res["span"], frozenset(node_allowed)))
    probs = []
    if res["missing_partner"]:
        probs.append("%d release row(s) have no m56 row at the same tick offset (first %s)" % (len(res["missing_partner"]), res["missing_partner"][:6]))
    if res["partner_settle"]:
        probs.append("%d release row(s) land on an m56 settle frame (first %s)" % (len(res["partner_settle"]), res["partner_settle"][:6]))
    if res["unpartnered_m56"]:
        probs.append("%d m56 non-settle row(s) inside the span have no release row (first %s)" % (len(res["unpartnered_m56"]), res["unpartnered_m56"][:6]))
    if res["field_mismatch"]:
        probs.append("fields differ: %s" % ", ".join("%s x%d (first offset %d)" % (k, v, res["first_offset"][k])
                                                     for k, v in sorted(res["field_mismatch"].items())))
    if not res["joined"]:
        probs.append("no row joined")
    if out["node_mismatch"]:
        probs.append("annotation node identity differs: %s" % ", ".join(
            "%s x%d (first event %s)" % (k, v, out["node_first"][k]) for k, v in sorted(out["node_mismatch"].items())))
    out["verdict"] = "PASS" if not probs else "FAIL: " + "; ".join(probs)
    return (0 if not probs else 1), out


def render(code, out):
    lines = ["M56 NON-SETTLE COMPARE  release %s  vs  m56 %s" % (out["release"], out["m56"])]
    if "joined" in out:
        lines.append("  common tick span 0..%d | release rows joined %d | m56 settle rows in span %d (no release partner, by design)"
                     % (out["span"], out["joined"], out["settle_rows"]))
        if out.get("run_unique_from_control") is not None:
            lines.append("  run-unique set from the control pair: %s" % (", ".join(out["run_unique_from_control"]) or "none"))
        lines.append("  intended m56 differences on non-settle frames (reported, not failed): %s" % (
            ", ".join("%s x%d (offsets %s)" % (k, len(v), v[:8]) for k, v in sorted(out.get("intended", {}).items())) or "none"))
        lines.append("  annotation node identity: events joined %d | only in the release %d %s | only in the m56 session %d %s (readings)"
                     % (out.get("node_joined", 0), len(out.get("node_only_release", [])), out.get("node_only_release", [])[:4],
                        len(out.get("node_only_new", [])), out.get("node_only_new", [])[:4]))
        if out.get("node_run_unique_from_control") is not None:
            lines.append("  node run-unique set from the control pair: %s" % (", ".join(out["node_run_unique_from_control"]) or "none"))
        lines.append("  intended node-name changes on scoped texture events (reported, not failed): %s" % (
            "; ".join("%s x%d (%s)" % (k, len(v), ", ".join(v[:4])) for k, v in sorted(out.get("node_intended", {}).items())) or "none"))
        lines.append("  scope: labels.jsonl plus annotation node identity (annotation frame lists, run_summary and mask pixels are read by other checks)")
    lines.append("M56 NON-SETTLE: %s (exit %d)" % (out["verdict"], code))
    return "\n".join(lines)


def _synth(root, name, offsets, settle=(), f0=10, tweak=None, anom="missing_texture", entry_extra=None, ann_tweak=None):
    d = os.path.join(root, name)
    os.makedirs(d)
    rows = []
    eid = "%s@%d|Rock" % (anom, f0 + 3)
    for si, o in enumerate(offsets):
        ent = {"id": anom, "target_name": "Rock", "start_frame": f0 + 3, "event_id": eid, "labelled": True,
               "bbox_px": [10, 20, 30, 40]}
        if entry_extra:
            ent.update(entry_extra)
        r = {"frame_index": f0 + o, "session_index": si, "t": 0.5 + o / 30.0, "t_wall": 1000.0 + si,
             "image": "Actual_Frames/frame_%05d.png" % si, "anomaly_present": 3 <= o <= 12,
             "view": {"origin": [0, 0, 0], "rot": [0, 0, 0]},
             "anomalies": ([ent] if 3 <= o <= 12 else [])}
        if o in settle:
            r["transition_present"] = True
            r["transition_reason"] = [SETTLE]
        if tweak:
            tweak(o, r)
        rows.append(r)
    with open(os.path.join(d, "labels.jsonl"), "w", encoding="utf-8") as fh:
        for r in rows:
            fh.write(json.dumps(r) + "\n")
    ev = {"event_id": eid, "anomaly_type": anom, "anomaly_subtype": anom,
          "affected_objects": {"count": 1, "primary_index": 0, "nodes": [
              {"name": "Rock", "path": "/Game/Map.Map:PersistentLevel.Rock", "global_position": [100.0, 0.0, 50.0],
               "asset_name": "SM_RockBody", "component_class": "StaticMeshComponent",
               "bounds": {"origin": [100.0, 0.0, 60.0], "extent": [40.0, 40.0, 30.0]}}]}}
    if ann_tweak:
        ann_tweak(ev)
    with open(os.path.join(d, "annotation.json"), "w", encoding="utf-8") as fh:
        json.dump({"label_schema": 2, "anomalies": [ev]}, fh)
    return d


def selftest():
    root = tempfile.mkdtemp(prefix="m56cmp_")
    results = []

    def case(name, code_out, want, must=None):
        code, out = code_out
        ok = code == want and (must is None or must in out["verdict"])
        results.append(ok)
        print("SELFTEST %-56s exit %d (want %d) %s" % (name, code, want, "ok" if ok else "*** WRONG ***"))
        if not ok:
            print(render(code, out))

    try:
        rel_off = [o for o in range(30) if o not in (4, 5, 14, 15, 20, 21)]
        rel = _synth(root, "rel", rel_off, f0=10)
        m56 = _synth(root, "m56", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500)
        case("identical non-settle rows, different absolute frame_index", main_compare(rel, m56), 0)

        def bump(o, r):
            if o == 7:
                r["anomalies"][0]["bbox_px"] = [11, 20, 30, 40]
        m56b = _synth(root, "m56b", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=bump)
        case("one changed box field fails", main_compare(rel, m56b), 1, "bbox_px")

        def unlab(o, r):
            if o == 9:
                r["anomalies"][0]["labelled"] = False
        m56c = _synth(root, "m56c", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=unlab)
        case("one changed label field fails", main_compare(rel, m56c), 1, "labelled")
        m56d = _synth(root, "m56d", list(range(30)), settle=(4, 6, 14, 15, 20, 21), f0=500)
        case("a release row landing on an m56 settle frame fails", main_compare(rel, m56d), 1, "settle")

        def pose(o, r):
            r["view"]["rot"] = [0, 2.5, 0]
        m56e = _synth(root, "m56e", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=pose)
        case("a different rest pose is INVALID, not FAIL", main_compare(rel, m56e), 3, "INVALID")

        def wall(o, r):
            r["t_wall"] = 5000.0 + o
            if o == 8:
                r["anomalies"][0]["target_pixels"] = 777
        rel2 = _synth(root, "rel2", rel_off, f0=40, tweak=wall)
        m56f = _synth(root, "m56f", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=wall)
        case("a field that differs between two release runs is run-unique", main_compare(rel, m56f, rel2), 0)

        def dip(o, r):
            if o in (8, 9):
                r["exposure_dip"] = True
                r["exposure_dip_scope"] = "global"
        m56g = _synth(root, "m56g", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=dip)
        cg = main_compare(rel, m56g)
        case("exposure_dip on non-settle frames is intended, reported, not failed", cg, 0)
        results.append(sorted(cg[1].get("intended", {}).get("exposure_dip", [])) == [8, 9])
        print("SELFTEST %-56s %s" % ("the intended difference is listed with its offsets", "ok" if results[-1] else "*** WRONG ***"))

        def dip_and_box(o, r):
            dip(o, r)
            if o == 10:
                r["anomalies"][0]["bbox_px"] = [12, 20, 30, 40]
        m56h = _synth(root, "m56h", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=dip_and_box)
        case("exposure_dip allowed but any other new difference still fails", main_compare(rel, m56h), 1, "bbox_px")

        def target_px(o, r):
            if o == 11:
                r["anomalies"][0]["target_pixels"] = 5
        m56i = _synth(root, "m56i", list(range(30)), settle=(4, 5, 14, 15, 20, 21), f0=500, tweak=target_px)
        case("a new entry field fails", main_compare(rel, m56i), 1, "target_pixels")

        S = (4, 5, 14, 15, 20, 21)
        allm = list(range(30))
        skipped = {TEXCORRUPT_SKIPPED: 2, "texcorrupt.components_corrupted": 1}
        noskip = {TEXCORRUPT_SKIPPED: 0, "texcorrupt.components_corrupted": 3}

        def rename(ev):
            ev["affected_objects"]["nodes"][0].update(asset_name="SM_RockTrim", component_class="InstancedStaticMeshComponent")

        def proxy(ev):
            ev["stuck_mip_route"] = "proxy"

        def hold(ev):
            ev["stuck_mip_route"] = "hold"

        relT = _synth(root, "relT", rel_off, f0=10, anom="uv_corruption", entry_extra=skipped)
        cT = main_compare(relT, _synth(root, "m56T", allm, settle=S, f0=500, anom="uv_corruption", entry_extra=skipped, ann_tweak=rename))
        case("node name change on a texture event with skipped parts passes", cT, 0)
        results.append(sorted(cT[1].get("node_intended", {}).get(CAUSE_SKIPPED, [])) ==
                       ["uv_corruption@3|Rock asset_name", "uv_corruption@3|Rock component_class"])
        print("SELFTEST %-56s %s" % ("the intended node change is listed by cause", "ok" if results[-1] else "*** WRONG ***"))
        relT0 = _synth(root, "relT0", rel_off, f0=10, anom="uv_corruption", entry_extra=noskip)
        case("the same change with components_skipped 0 fails",
             main_compare(relT0, _synth(root, "m56T0", allm, settle=S, f0=500, anom="uv_corruption", entry_extra=noskip, ann_tweak=rename)), 1, "node/asset_name")
        case("a node name change on missing_texture fails", main_compare(rel, _synth(root, "m56mt", allm, settle=S, f0=500, ann_tweak=rename)), 1, "node/asset_name")

        def rename_bounds(ev):
            rename(ev)
            ev["affected_objects"]["nodes"][0]["bounds"]["extent"] = [20.0, 40.0, 30.0]
        case("a scoped event may not change node.bounds",
             main_compare(relT, _synth(root, "m56Tb", allm, settle=S, f0=500, anom="uv_corruption", entry_extra=skipped, ann_tweak=rename_bounds)), 1, "node/bounds")

        def rename_actor(ev):
            rename(ev)
            ev["affected_objects"]["nodes"][0]["name"] = "Rock2"
        case("a scoped event may not change node.name",
             main_compare(relT, _synth(root, "m56Tn", allm, settle=S, f0=500, anom="uv_corruption", entry_extra=skipped, ann_tweak=rename_actor)), 1, "node/name")
        relP = _synth(root, "relP", rel_off, f0=10, anom="stuck_low_mip", ann_tweak=proxy)

        def proxy_rename(ev):
            proxy(ev)
            rename(ev)
        cP = main_compare(relP, _synth(root, "m56P", allm, settle=S, f0=500, anom="stuck_low_mip", ann_tweak=proxy_rename))
        case("node name change on a stuck_low_mip proxy event passes", cP, 0)
        results.append(len(cP[1].get("node_intended", {}).get(CAUSE_PROXY, [])) == 2)
        print("SELFTEST %-56s %s" % ("the proxy cause is listed on its own", "ok" if results[-1] else "*** WRONG ***"))
        relH = _synth(root, "relhold", rel_off, f0=10, anom="stuck_low_mip", ann_tweak=hold)

        def hold_rename(ev):
            hold(ev)
            rename(ev)
        case("the same change on the hold route fails",
             main_compare(relH, _synth(root, "m56hold", allm, settle=S, f0=500, anom="stuck_low_mip", ann_tweak=hold_rename)), 1, "node/asset_name")
        case("allowed node change plus a changed box still fails",
             main_compare(relT, _synth(root, "m56Tx", allm, settle=S, f0=500, anom="uv_corruption", entry_extra=skipped, ann_tweak=rename, tweak=bump)), 1, "bbox_px")
        m56na = _synth(root, "m56na", allm, settle=S, f0=500)
        os.remove(os.path.join(m56na, "annotation.json"))
        case("a session without annotation.json cannot run", main_compare(rel, m56na), 2, "annotation.json")

        def no_id(ev):
            ev.pop("event_id")
        case("an annotation event without event_id cannot run",
             main_compare(rel, _synth(root, "m56ni", allm, settle=S, f0=500, ann_tweak=no_id)), 2, "event_id")
        m56ex = _synth(root, "m56ex", allm, settle=S, f0=500)
        ap = os.path.join(m56ex, "annotation.json")
        with open(ap, "r", encoding="utf-8") as fh:
            ann = json.load(fh)
        extra = copy.deepcopy(ann["anomalies"][0])
        extra["event_id"] = "blinking@%d|Rock" % (500 + 20)
        ann["anomalies"].append(extra)
        with open(ap, "w", encoding="utf-8") as fh:
            json.dump(ann, fh)
        cx = main_compare(rel, m56ex)
        case("an event only one session holds is a reading, not a failure", cx, 0)
        results.append(cx[1].get("node_only_new") == ["blinking@20|Rock"])
        print("SELFTEST %-56s %s" % ("the unjoined event is listed", "ok" if results[-1] else "*** WRONG ***"))

        def move(ev):
            ev["affected_objects"]["nodes"][0]["global_position"] = [101.0, 0.0, 50.0]
        relmv = _synth(root, "relmv", rel_off, f0=40, ann_tweak=move)
        m56mv = _synth(root, "m56mv", allm, settle=S, f0=500, ann_tweak=move)
        case("a node field that differs between two release runs is run-unique", main_compare(rel, m56mv, relmv), 0)
        case("without the control the same node field fails", main_compare(rel, m56mv), 1, "node/global_position")
    finally:
        shutil.rmtree(root, ignore_errors=True)
    ok = all(results)
    print("M56 NON-SETTLE SELFTEST: %s - %d case(s)" % ("OK" if ok else "FAILED", len(results)))
    return 0 if ok else 1


def main(argv=None):
    ap = argparse.ArgumentParser(description="m56 G5 non-settle comparator (read-only).")
    ap.add_argument("release", nargs="?")
    ap.add_argument("m56", nargs="?")
    ap.add_argument("--control", default=None)
    ap.add_argument("--json", default=None)
    ap.add_argument("--selftest", action="store_true")
    a = ap.parse_args(argv)
    if a.selftest:
        return selftest()
    if not a.release or not a.m56:
        ap.print_usage()
        return 2
    code, out = main_compare(os.path.abspath(a.release), os.path.abspath(a.m56),
                             os.path.abspath(a.control) if a.control else None)
    print(render(code, out))
    if a.json:
        with open(a.json, "w", encoding="utf-8") as fh:
            json.dump({"exit": code, **out}, fh, indent=1, default=str)
    return code


if __name__ == "__main__":
    sys.exit(main())
