---
name: h4-preflight-halt
description: "H4 ran 2026-08-19 after its brief was amended and came back H4-SUPPORTED (mechanism, path b, 8/8 events); the pre-flight that halted the original brief is part one of the same journal"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T08:53:03.115Z
---

**H4 IS `H4-SUPPORTED` — the mechanism is OBSERVED, path (b), 8/8 events.** Plugin `5336802`
(docs only, pushed, **NO TAG**), CaptureBench `f74291a` (local-only). Journal
`docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md` — **PART TWO is the run**, part one is the
pre-flight that stopped the *original* brief. **ZERO plugin production code across both parts.**
Follows [[s4-certified-m25]].

**THE OBSERVATION.** On `StaticMeshActor_100` (fully occluded — `_86` blocks all 9 rays alone):
label path emitted `bbox_valid:true` on **59/59** rows, `visible_positive` on 59, `coverage_ratio
0.01160339`, 8 events all `manifested:true` — while provenance read `valid:false`, `0/0`,
`coverage_pct −1` on all 8. **That pairing (provenance false WITH bbox_valid true) is the C4
divergence and had never appeared in 780 banked records.** Raw in-bbox series (a SERIES, not an A54
verdict): ≤ **2.0e-4**, ranges overlapping 7/8, vs the control's **0.1023–0.1116** — factor ~500.
Control `_49`: provenance 9/9 valid:true ×8, A54 **7/7 ALIGNED 7/7 decidable** median margin 0.10527,
positive control decisive both ways.

⛔ **SUPPORTED, not CONFIRMED** (the A54 leg is unobtainable — G117). ⛔ **No incidence claim** — path
(b) only; path (a) cannot be produced in `CB_GateLevel`. ⛔ **n = 1 leg.** Routes to
`feature/stencil-capture`, **not touched, not rebased**.

**The four amendments that made it runnable** (all ruled chat-side first): two legs not one run;
shipping defaults with **no `IAI.SetPollRadius`**; raw series instead of an A54 verdict; and the
sharpened C3/C4 discriminator in the branch table before the run.

**Why it worked:** the pre-flight caught four defects in the original brief before a run was spent —
two targets in one run is impossible without production code; A54 cannot judge any target but `_49`
(**G117**); `StaticMeshActor_11` would have "confirmed" H4 via the **poll-radius cull**; and
`coverage_pct` is **−1, not 0**. **G116's standing practice: grep the bank for a pre-declared
signature BEFORE the run — if it already occurs you have found the incumbent producer.**

**How to apply:** one clause link is honestly *not* from the artifact — **C2 (poll radius) is not
decidable from the capture**, since the projector has no radius test and `poll_distance` is the −1
sentinel; it is excluded by the certified offline model plus a live `pollRadius` read-back. Say so
when citing this result. Also: **[[g118-cooked-placeholder-token]]** was found in passing and is NOT
fixed. `P6` DOES NOT MOVE (its first observation is recorded, nothing changed).

**NEXT — the open question is PATH (a), NOT the cure.** `feature/stencil-capture` stays untouched and
unrebased: no cure is built before we know whether the shipped path is exposed (the D-B lesson — the
SVE migration would not have cured P3). Recon done (journal 045 §19, read-only): **nothing
re-validates a live target after the fire begins** — the one occlusion evaluation downstream of pick
time runs **once** at the anchor, its **return value is discarded**, and its sidecar is **suppressed in
delivery mode**, so a delivered dataset carries **no** post-pick occlusion evaluation at all. The
pick-time guard returns "unoccluded" on the **FIRST clear ray of 9**, traces to **AABB corners not the
mesh**, and uses **simple collision** — so its bar for "visible" and this project's 9-of-9 bar for
"occluded" are **not complements**. 🚨 **G87 blocks the obvious environment: MainWorld's redirect to
MainMenu is ACTIVE, not a startup race**, so a packaged path-(a) test cannot simply launch it. Design
is chat-side first.
