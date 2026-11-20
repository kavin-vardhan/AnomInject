---
name: anomaly-collision-free-concurrency
description: Why m6 auto-injection is concurrent-but-collision-free without a coordinator (one-anomaly-per-actor)
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
---

The m6 auto-injector fires multiple anomalies at once with **no ref-count "hidden-by" coordinator**, by holding two scheduler invariants instead:
- **(i) one live fire per id** — the injector registry holds exactly one instance per id (re-Apply reverts-then-reapplies), so the scheduler never re-fires a still-live id. This is the natural concurrency ceiling (max live ≤ distinct enabled-id count) and gives clean revert accounting.
- **(ii) one anomaly per actor (OVERRIDE-1)** — `Candidates = visibleSet − {actors hosting ANY live fire}`. This single guard subsumes BOTH conflict groups (bHidden: `missing_object`/`flicker`; forced-LOD: `lod_corruption`/`lod_popping`) AND the hide-masks-LOD case (a hide hiding a LOD change = an invisible/mislabeled sample). Among the 4 pool ids all 6 cross-pairs are same-resource or visibility-masks-LOD, so one-per-actor is strictly simpler than a per-conflict-group guard and needs NO id→group table.

**Why this matters:** the original scoping plan proposed a per-conflict-group guard; chat-Claude's OVERRIDE-1 replaced it with one-anomaly-per-actor (simpler, also blocks hide-masks-LOD). The deferred ref-count coordinator (gotcha G12) is now only needed for *deliberate* compound/stacked same-actor anomalies — a future milestone, not v1.

**How to apply:** if asked to add concurrency/stacking to auto-injection, do NOT reintroduce a per-group table or build the coordinator unless the goal is explicitly *compound same-actor* anomalies. `IAnomaly::GetUsage()` is a human hint string and cannot supply machine-readable conflict-group identity (gotcha G31) — never parse it. Recorded in the repo as gotcha G30 + `docs/architecture.md`. Part of [[anomaly-injector-state]].
