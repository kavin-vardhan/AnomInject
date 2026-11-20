---
name: path-a-blocked-mainworld-not-cooked
description: "Path (a) is structurally open but environmentally blocked — MainWorld is not cooked into ANY staged build, which also corrects G87's mechanism; the fix is the same re-cook G118 closure needs"
metadata: 
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-08-19T12:32:29.884Z
---

**🟥 GEOMETRY SURVEY 2026-08-19 (journal 045 §51–§61, plugin `8fdfe99`): *NO CROSSING PAIR EXISTS*.**
From the settled camera **no moving platform can FULLY occlude any selectable target**. Only **1 of 4**
platforms is in the frustum (others 68.9°/75.5°/75.0° off-axis); it is **farther** than every compact
target so they occlude *it*; it **is** in front of both `InstancedFoliageActor`s but its rect is
240×20 px against ~1280×713 ⇒ **containment impossible ⇒ partial only**. Full occlusion needs
containment. ⛔ **Scoped to ONE camera pose.** ✅ **Camera holds: last 200 frames dX 0.0004, rotation
exactly 0.** ✅ All four unbound platforms move — two on Z, **two horizontally**.
⚠ **CORRECTION: the platform is 126.67 cm/s, not the ~169 I first published** — I divided by 0.4 s
assuming captured frames are the only ticks; game Δt is 0.5333 s. **And cm-per-CAPTURED-FRAME depends
on the capture config** (5.630 vs 7.841): any "crosses at frame N" claim must state the config; read
`t` from `labels.jsonl`, never divide by `VideoFps`.
🚨 **TWO annotation fields would have inverted the answer:** `nodes[].bounds` is
`GetComponentsBoundingBox(true)` — the **whole actor** (a 42 m box centred 2.2 km away for
`BP_MovingPlatform`) — and `global_position` is the **actor origin**, not the geometry. **The projector
and `IsUnoccluded` use only SM/SK `Component->Bounds`.** `P6` does not move.
**G122** (asset vs runtime namespaces) · **G123** (a REPORTING-ONLY path that can kill the run).

*(Earlier, still true:)* **⬆ UNBLOCKED 2026-08-19 (journal 045 §41–§50, plugin `673c1e8`).** MainWorld is cooked in, **boots and
stays** (1102 probe ticks, all `level=MainWorld`, nothing redirects), has a **populated gameplay view**
and a **streaming source with no input**, and **all movers are loaded** (432 actors vs 419 external
files). ⚠ **R-4 SPLITS BY CLASS: `BP_MovingPlatform` (unbound) MOVES — Z +472.9 cm across a 3 s leg,
~169 cm/s; `BP_Stomper` (unbound) is STATIC** — 21 samples at two incommensurate cadences, one
position. **G-1's asset reading is confirmed at runtime for the platform and REFUTED for the stomper**
(n=1 each). 🚨 **The first R-4 leg targeted a BOUND Stomper and nearly reported a false HALT** — G-1
keyed on FILES, the leg keyed on RUNTIME UAID NAMES, and the two had never been joined; **always join
UAID → asset file before believing a runtime motion result.** Free observation, not acted on: on the
platform leg `bbox_valid` is 59/59 and the **occlusion count varies within one window (6/9→7/9→9/9)
with the 6/9 rows `valid:true`** — the `P-a1` band in live data. ⛔ **Path (a) is still UNDESIGNED and
UNRUN; `feature/stencil-capture` untouched.**

*(Everything below is the earlier blocked state, kept because its reasoning is still the reason the
cook happened.)*

**Path (a) — target visible at pick, occluded during the window — is STRUCTURALLY OPEN and
ENVIRONMENTALLY BLOCKED.** Journal `docs/sessions/2026-08-19-045-h4-cook-and-h5-mainworld-arc.md` §21–§28, plugin
`f1671d4`. Follows [[h4-preflight-halt]].

**STRUCTURALLY OPEN:** nothing re-validates a live target after the fire begins. The one occlusion
evaluation downstream of pick time runs **once** at the anchor, its **return value is discarded**, and
its sidecar is **suppressed in delivery mode** — so a delivered dataset carries no post-pick occlusion
evaluation at all. The only per-frame test is `Weak.Get()` (object *existence*, not visibility).

🚨 **ENVIRONMENTALLY BLOCKED, and this CORRECTS G87 → G120. `MainWorld` IS NOT COOKED INTO ANY STAGED
BUILD.** From each `StackOBot-Windows.utoc`: BenchGate `101AFEA4` = `CB_GateLevel`, `Entry`,
`MainMenu`; MidRepro and `Builds\Windows` = `Entry`, `MainMenu`. **G87's "the redirect is ACTIVE / every
approach is dead" was engine fallback to `GameDefaultMap` on a failed browse** — the only `OpenLevel`
in the project is in `HUD_MainMenu` and runs MainMenu → MainWorld. G87's headline rule (*check the
level NAME, not the picture*) is unaffected; only its explanation was wrong. ⚠ **Corrected but NOT
re-measured** — one packaged launch with the log read for the missing-map browse error settles it.

**No cooked level has unattended motion:** `CB_GateLevel` is STATIC throughout; `MainMenu` has only
skeletal animation (`ABP_Bot`) and 3 `CameraActor`s with nothing driving them. **`MainWorld` would
provide it** — `BP_Stomper` (*"When no trigger is referenced it move constantly"*), `BP_MovingPlatform`
(`InterpToMovement`, `PingPong`), `BP_Fan`, `BP_EnergyOrb`. ⚠ Per-instance `Trigger` assignment not
established. `MainWorld` is **World Partition + one-file-per-actor** (419 external actors), so
streamed-in geometry is itself a candidate occluder.

**PRE-COOK GATE PASSED 2026-08-19 (journal §29–§33, plugin `cc5ef41`). Owner ruled OPTION A — the
rebuild happens, carrying the MainWorld cook AND G118's token closure. Nothing cooked yet.**
- **G-1 PASS:** per placed instance, **`BP_Stomper` 5 of 7 and `BP_MovingPlatform` 4 of 6 leave
  `Trigger` EMPTY ⇒ 9 translating movers run unattended.** Discriminator verified both ways (16
  MainWorld files DO carry a serialised `Trigger`). ⚠ **`BP_Fan` uses `Triggers` (ARRAY)** — keyed on
  the scalar it read 4-of-4 unattended; re-keyed, **2 of 4 are bound**.
- **G-2: the debt list against `101AFEA4` is EMPTY** — it can be retired.
- **G-3: G87's correction is CONFIRMED** — `"The package to load does not exist on disk or in the
  loader"`. Log banked at `_bench_sessions_bank/G3_MAINWORLD_BROWSE_PROBE/`.

**How to apply — TWO COOK RISKS surfaced by the sweep, act on both:**
1. 🚨 **All four `.baseline` exes live INSIDE `Builds\BenchGate\...\Binaries\Win64\`, the directory a
   re-stage writes into. COPY THEM OUT FIRST**, with `101AFEA4` as the new `.m25-baseline`. G92 records
   that staging wipes `Saved`; nothing records what it does to `Binaries`.
2. 🚨 **Decide the cook's map set BEFORE running it and READ THE NEW `.utoc` MAP INDEX BACK AFTERWARDS**,
   asserting `CB_GateLevel` + `MainMenu` + `MainWorld`. **A cook that silently omitted a map is what
   created this whole situation** and nothing checks the cooked map set today.

✅ The MainWorld cook and [[g118-cooked-placeholder-token]] closure are **one operation, not two**.
Prediction set `P-a1`–`P-a5` is pre-declared (journal §22) — any path (a) design must say which it
exercises. ⛔ `feature/stencil-capture` stays untouched: no cure before the incidence question.
