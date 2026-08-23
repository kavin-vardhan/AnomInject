# 2026-08-23 — session 054 — the LOD-contrast gate, a tunable coverage threshold, and the mask-pass hitching finding

**This file is SELF-CONTAINED.** Two plugin commits plus this journal, all pushed.

⛔ **NOTHING WAS TAGGED. `m31` IS STILL THE OPEN MILESTONE AND STILL UNTAGGED. Highest tag is
`m30`.** `feature/stencil-capture` untouched at `76cac74`. `P6` did not move. No force-push.
No cook — code-only hot-swap. Staged exe **`060F7B07`**, built == staged verified; the pre-change
binary is archived and hash-verified at
`_binary_baselines\StackOBot.exe.session053-prechange-F9E0941C`.

**All six of session 053's owner tests PASSED on the client host** — gapped subset labelling for
`lod_popping` confirmed on his content, targeted fire bypasses the auto-pool gates, targeted
`camera_clipping` triggers on proximity with accurate labels.

---

## §0 ONE SCREEN

| # | Commit | What |
|---|---|---|
| 1 | `4ec07ed` | `feat(lod)` tunable coverage threshold for `lod_popping` |
| 2 | `befca64` | `feat(lod)` auto-pool prefers highest-LOD candidates |

⚠ **COMMIT ORDER IS THE REVERSE OF THE BRIEF'S, DELIBERATELY.** Both levers land in the same
`AnomalyDefaults` insertion block and the same `Apply` function; committing the highest-LOD gate
first would have left its run-config echo referencing a `Describe…` function that arrives in the
next commit. Coverage first is the order in which each commit stands alone.

**The owner's finding this answers:** after a real playthrough, auto-pool `lod_popping` is "very
very thin", and — the part that matters — *"in some objects it's very visible, in some it's not."*

---

## §1 THE ROOT CAUSE, AND WHY DISTANCE AND COVERAGE COULD NOT HAVE FIXED IT

`lod_popping`'s visible magnitude is the **CONTRAST between the LOD an object is CURRENTLY
rendering and the one forced onto it.** Forcing a low LOD onto something already at a low LOD pops
it to something close to itself. Distance and screen coverage are *proxies* for that contrast — a
distant, small object is usually already at a reduced LOD — but they are proxies, and an object can
be near and large and still be at LOD 1 (wide FOV, a LOD-bias cvar, an aggressive asset LOD curve).
**This gate is the contrast itself.**

It is the **GRADED form of the existing single-LOD guard**, which refuses a mesh that cannot pop at
all. Stated so the relationship is not re-derived later: single-LOD ⇒ zero contrast ⇒ refused
outright, in BOTH modes, because forcing a LOD on a one-LOD mesh pops it to itself whoever picked
it. Multi-LOD but already reduced ⇒ *some* contrast, strictly less than from LOD 0 ⇒ refused in
AUTO-POOL only.

### §1.1 RESTRICT, not PREFER — and the reason is the seeded draw protocol

A "prefer" has to re-pick inside selection. `R-SEED` is deliberately independent of apply-result,
`m22` gated on *"seed 4242, two runs byte-identical"*, and journal 045 §103 recorded
reject-and-re-pick as **permanently rejected** because it makes the draw count depend on a
per-candidate property. So this is a REFUSAL in `Apply`, in the same place and the same shape as the
single-LOD guard, the distance gate and the coverage gate.

---

## §2 THE API, AND THE STANDING-RULE SELF-CHECK

`AnomalyLod::GetCurrentLod(World, Component)` returns `{Level, ScreenSize, Source, bKnown}` and
reports its own provenance:

| component | source | what it is |
|---|---|---|
| any, with `ForcedLodModel > 0` | `forced-lod-model` | the force already in effect wins |
| skinned | `component-predicted-lod` | `USkinnedMeshComponent::GetPredictedLODLevel()` |
| static | `predicted-from-screen-size` | engine's `ComputeBoundsScreenSize` vs the asset's `RenderData->ScreenSize[]` |

**Skinned** is REAL GAME-THREAD STATE — `PredictedLODLevel` is written by `UpdateLODStatus()` inside
the component's own tick.

**Static has no game-thread cache at all** — the renderer picks the level on the render thread inside
`ComputeStaticMeshLOD`, which needs an `FSceneView`. So the level is **PREDICTED** using the engine's
own exported `ComputeBoundsScreenSize(BoundsOrigin, SphereRadius, ViewOrigin, ProjMatrix)`
(`SceneManagement.h`, `ENGINE_API`, pure math on parameters) against the asset's own authored
per-LOD thresholds, walking backwards exactly as `ComputeStaticMeshLOD` does:

```
for LodIndex = NumLods-1 .. 0:  if ScreenSize[LodIndex] > computedScreenSize:  return LodIndex
return 0
```

🚨 **THE SELF-CHECK, RUN AGAINST THE STANDING RULE RATHER THAN ASSUMED.** This project has
PERMANENTLY REFUSED engine-side "was it rendered" reads — `GetLastRenderTimeOnScreen` is
occlusion-gated and `GetLastRenderTime` is bumped by the shadow path (`G126`), and both are
render-thread-written timestamps that can lie or lag. **NEITHER PATH HERE IS IN THAT FAMILY.** The
static path is a deterministic screen-size computation from game-thread data (component bounds + the
view we already build) — the branch the rule explicitly permits. The skinned path is game-thread
state, not a timestamp. Both are read on the game thread, synchronously, with no cross-thread hop.

⚠ **NAMED FIDELITY LIMITS, NOT HIDDEN.** The prediction does not model `FSceneView::LODDistanceFactor`
(default 1.0), `r.StaticMeshLODDistanceScale`, or per-platform `MinLOD`. Where a scalability factor
is in play the prediction can DISAGREE with the renderer. Mitigation, not a claim of accuracy:
**every candidate logs `CURRENT-LOD` with its computed screen size and its source**, so a
disagreement shows up in the run log instead of silently refusing everything.

✅ **NO NEW MODULE DEPENDENCY.** `SceneManagement.h` and `StaticMeshResources.h` are Engine *public*
headers and `AnomalyInjector` already depends on `Engine`; the editor and game targets both compile
with `AnomalyInjector.Build.cs` untouched. The game-agnostic invariant is unmoved.

---

## §3 PROVEN BY BREAKING IT — one binary, one object, one variable

`SM_rock_02` (`StaticMeshComponent0`, worst LOD 4), MainWorld, seed 777. The only variable is the LOD
the object is at; FOV was read back from the artifact (`camera.fov_deg` = 150), not assumed.

| leg | FOV | mode | CURRENT-LOD | outcome |
|---|---|---|---|---|
| `S54_LODFILT_ON` | 90 | auto-pool, gate ON | `level=0 screen_size=0.826110` | **ACCEPTED** — "0 refused (not at highest LOD)", 1 `lod_popping` event |
| `S54_BREAK_FOV_ON` | 150 | auto-pool, gate ON | `level=1 screen_size=0.224736` | **REFUSED** — 0 `lod_popping` events |
| `S54_BREAK_FOV_TARGETED` | 150 | targeted, bypassed | `level=1 screen_size=0.213030` | **WARNS, FIRES** — 8 events, gapped 3-frame subsets |

The greppable refusal, verbatim:

```
lod_popping: REFUSED 'StaticMeshComponent0' — it is ALREADY AT LOD 1 (source
predicted-from-screen-size, screen_size 0.224736), not its highest-detail LOD 0, so forcing LOD 4
onto it would change little or nothing. This is the GRADED form of the single-LOD guard and it
gates AUTO-POOL SELECTION only; a targeted fire on a named object warns and fires anyway.
IAI.Anomaly.LodRequireHighestLod 0 disables it.
```

and the targeted warning, which is what will explain a weak-looking targeted fire to the owner:

```
lod_popping: TARGETED FIRE on 'StaticMeshComponent0' which is ALREADY AT LOD 1 ... FIRING ANYWAY
— your pick wins — but the pop will be WEAKER than on the same object at LOD 0 ... Move closer, or
expect a small change.
```

⚠ **THE FIRST ATTEMPT TO BREAK IT FAILED, AND THAT IS RECORDED RATHER THAN QUIETLY REPLACED.**
With both proximity gates disabled (`LodMaxDistance 0`, `LodMinCoverage 0`) the gate still never
fired: on this bench **exactly one candidate ever reaches the LOD check and it is at LOD 0.** The
wide-FOV leg is what produced a real LOD ≥ 1 — a genuine rendering condition (the renderer drops the
LOD too), not a fabricated flag.

📌 **AN INCIDENTAL CONFIRMATION, FREE:** the FOV-150 leg produced a `camera_clipping` event —
the **positive branch of the global near-clip discriminator, which session 053 had to report as
VACUOUS** because no pose on this bench produced it. It produces it at FOV 150.

---

## §4 FIRE RATE — MEASURED, REPORTED, NOT TUNED

MainWorld auto-pool, seed 777, 300 frames, `IAI.Capture.Config 2 4 8 14 0`, and
`IAI.Anomaly.LodMaxDistance 2000` so `lod_popping` can fire at all (at the shipped 200 cm it fires
zero on this bench — session 052's measurement, unchanged):

| leg | filter | events | `lod_popping` |
|---|---|---|---|
| `S54_LODFILT_OFF` | OFF | 10 | **1** — `[272, 273, 274]` |
| `S54_LODFILT_ON` | ON (default) | 10 | **1** — `[272, 273, 274]` |

**BYTE-IDENTICAL EVENT SET.** The filter costs NOTHING here.

🚨 **SAID LOUDLY, BECAUSE A ZERO-COST RESULT ON THIS BENCH IS NOT EVIDENCE ABOUT HIS CONTENT.** An
unattended run settles at a fixed pose and the pawn never walks up to anything, so the only
multi-LOD candidate it ever draws is the near rock at 8.6 m filling 0.83 of the screen — already at
LOD 0, and therefore never refused. **The filter's real cost can only be measured where a player
moves.** If it cuts his rate to zero, `IAI.Anomaly.LodRequireHighestLod 0` restores the previous
candidate set exactly, with no re-cook.

---

## §5 THE TUNABLE COVERAGE THRESHOLD (`4ec07ed`)

`[AnomalyInjector] LodPoppingMinCoveragePct` + `IAI.Anomaly.LodMinCoverage <pct|default>`, the same
shape as `IAI.Anomaly.LodMaxDistance`. Precedence **console > ini > compiled**. Range `[0..100]`,
out of range **REFUSED not clamped**. `0` disables the COVERAGE gate only. AUTO-POOL ONLY.

**Effective value and provenance on the existing run-config line**, read back from a real run:

```
... | blinkHalf=3(compiled) lodHalf=8(compiled) lodMaxDist=2000cm(console)
      lodMinCov=7.0000%(compiled) lodHighestOnly=on(compiled) clipRadius=200cm(compiled) | ...
```

⚠ **THE COMPILED DEFAULT STAYS 7.0 AND WAS NOT TOUCHED.** It is a MEASURED number: m30 calibrated it
against a last-visible anchor of **9.3453 %** and a first-invisible anchor of **3.9045 %**, margins
1.34× below the visible anchor and 1.79× above the invisible one, biased toward REFUSING because a
positive label with no visible change is the dataset-poisoning direction. Both the resolver's log
line and the console help say **in those words** that tuning it at runtime is an operator decision
and is NOT a re-calibration, so nobody later reads a tuned value as a measured one. The number now
has ONE home (`AnomalyDefaults::LodPoppingMinCoverageCompiled`) with a `static_assert` tying the
anomaly's own constant to it.

---

## §6 🚨 THE FULL GATE STACK FOR AN AUTO-POOL `lod_popping` CANDIDATE — the owner's question

In order. Every threshold, and where each comes from.

**A — the `G33` selection chokepoint, `AnomalyViewport::IsRenderableComponent`, per component**
1. `Component->IsVisible()`
2. owner is not an `AInstancedFoliageActor` (m27, hard type test)
3. no `[AnomalyInjector] ExcludedTargetNamePatterns` substring matches its ACTOR / COMPONENT / MESH-ASSET name — **compiled default EMPTY**
4. ISM/HISM only: `GetInstanceCount() > 0`
5. type is `UStaticMeshComponent` or `USkinnedMeshComponent`

**B — `GetVisibleRenderableActors` → `ClassifyRenderableVisibleLive`**
6. **POLL RADIUS `GPollRadius` = 1800 cm** — sphere-approx bounds distance from the **player pawn**. `IAI.SetPollRadius`, dashboard slider.
7. frustum test on component bounds
8. occlusion — `IsUnoccluded`, 9 rays camera→bounds centre + 8 AABB corners, passes on **any** clear ray
9. **GENERAL SCREEN COVERAGE `GMinScreenCoveragePct` = 6.0 %** — on the UNION of the actor's renderable-visible component bounds. `IAI.SetMinScreenCoverage`, dashboard slider.

**C — `UAnomalyAutoInjectorSubsystem::TryFireOnce`**
10. id enabled in the pool, and not already live (one live fire per id)
11. id is not Global-scoped
12. `LiveFires.Num() < MaxConcurrent` (4)
13. actor not already live (**OVERRIDE-1**, one anomaly per actor)

**D — `FAnomaly_LodPopping::Apply`, per component**
14. name match (`AnomalyLod::ResolveLodComponents`)
15. **≥2-LOD guard** — both modes, never bypassed
16. **DISTANCE `LodPoppingMaxDistanceCm` = 200 cm** — the SAME metric as (6). `IAI.Anomaly.LodMaxDistance`. **auto-pool only**
17. **LOD COVERAGE `LodPoppingMinCoveragePct` = 7.0 %** — m30-calibrated. `IAI.Anomaly.LodMinCoverage`. **auto-pool only.** NEW: tunable
18. **HIGHEST-LOD `LodPoppingRequireHighestLod` = ON** — `IAI.Anomaly.LodRequireHighestLod`. **auto-pool only.** NEW

**E — after the fire, at `FinishRun`**
19. the m26/m27 **mask veto** — a `MEASURED_ZERO` event is deleted from `annotation.json`

### §6.1 THE ANSWER: YES, THE GENERAL 6 % THRESHOLD ALSO GATES THESE CANDIDATES

Gate (9) and gate (17) are **two different thresholds on two slightly different quantities**:

- **(9) 6.0 %** unions only the components that passed poll radius + frustum + occlusion (`CollectRenderableVisibleUnion`).
- **(17) 7.0 %** unions **all** `IsRenderableComponent` components, ignoring frustum, occlusion and poll radius (`GetActorScreenCoveragePct`).

Because 6.0 < 7.0, **the lod-specific gate is currently the binding one and the general gate is inert
for `lod_popping`.** ⚠ **But that inverts the moment he tunes below 6.0: `IAI.Anomaly.LodMinCoverage
4` would have NO EFFECT below the 6 % floor, because selection would already have removed the
candidate — and the `lod_popping` log would show nothing at all, because a candidate refused at (9)
never reaches (17) to be logged.** That is the trap worth knowing about.

⛔ **A LOD-SPECIFIC OVERRIDE OF THE GENERAL THRESHOLD WAS NOT BUILT, AS INSTRUCTED — and on the
evidence it is not warranted today.** It only matters below 6 %, which is below m30's
first-invisible anchor (3.9045 %) plus its margin — i.e. in territory the calibration already calls
invisible. The general 6 % was itself measured (m19: the Bot is 9.98 % of the viewport; 10 % and
12 % culled the hero character, 6 % kept it), and `IAI.SetMinScreenCoverage` already exists as a
session-level lever if he ever needs to go lower. Reported, not built.

---

## §7 ITEM 3 — HITCHING: RESOLVED BY THE OWNER'S BISECT. RECORD ONLY, NO CODE WRITTEN.

The owner's A/B settled it before any instrumentation was written:

```
empty anomaly pool, frames still captured and written  -> NO hitching
full pool                                              -> hitching, DURING anomaly windows,
                                                          not only at boundaries
IAI.Capture.Mask 0, full pool, StackOBot               -> hitching GONE
IAI.Capture.Mask 0, full pool, client host             -> reduced (RDP confounds the subjective read)
```

⇒ **ELIMINATED:** per-frame capture/write cost · the m11 pacer · the tick pin (the symptom appears on
StackOBot, where the pin compiles out) · anomaly apply/revert cost (that would be a boundary spike,
not a during-window cost).
⇒ **IDENTIFIED:** the m26/m27 mask measure pass.

⚖ **OWNER DECISION, TAKEN, RECORDED SO IT IS NOT RELITIGATED: THE MASK STAYS ON.** He ran a capture
with `Mask 0` and saw many missed (invisible) objects admitted as labels — the client's original
complaint #1 returning — and that outweighs the hitch. The hitch ships as a DOCUMENTED LIMITATION.
Chat recommended mask-off; **the owner's direct evidence overrode it.**

### §7.1 WHAT THE MASK PASS COSTS PER ARMED FRAME — from source

🚨 **THE READBACK IS NOT BLOCKING, AND IT IS NOT ON THE GAME THREAD. That corrects the question's
framing.**

- `AddEnqueueCopyPass(GraphBuilder, Readback.Get(), MaskRT)` enqueues the GPU→CPU copy inside RDG,
  on the render thread (`AnomalyMaskSceneViewExtension.cpp:123-124`).
- `Drain_RenderThread` tests `Item.Readback->IsReady()` and `continue`s when it is not ready
  (`:183-186`) — **a poll, never a wait**.
- `Lock()` / `Unlock()` run on the **RENDER THREAD**, only after `IsReady()` is true (`:190`, `:262`).
- `EnqueueDrain()` is a fire-and-forget `ENQUEUE_RENDER_COMMAND` from the game thread (`:158-170`).

**There is no blocking GPU readback on the game thread anywhere in the mask path.**

**What it does cost, per armed frame:**

1. a full-screen mask pixel-shader pass at the view rect, after tonemap;
2. a GPU→CPU copy of a view-rect R8 surface — **921,600 B at 1280×720; 6,400,000 B at 3200×2000, ×6.94**;
3. 🚨 **the reduction: a full W×H single-byte CPU scan ON THE RENDER THREAD** (`:208-224`) — 6.4 M
   iterations at 3200×2000 — accumulating a fixed 256-entry count array plus per-tag min/max.
   Journal 045's `T-4` costed the earlier `TMap::FindOrAdd` form at ~0.1–0.3 ms clean and
   ~9–28 ms at 100 % tagged **at 1280×720**; the shipped form uses the fixed array so worst ≈ best,
   but **the scan itself scales with VIEW-RECT AREA**, and the client host is 3200×2000.

**ONCE PER ARMED FRAME, NOT ONCE PER ARMED TARGET.** `ArmIfMeasurable` returns after arming the
FIRST eligible record (`AnomalyMaskMeasure.cpp:198`), so at most one arm per game tick;
`PostProcessPassAfterTonemap_RenderThread` pops `PendingArms[0]`, so one mask pass per armed frame;
and the shader masks **all** currently-assigned tags at once, so a single readback covers every
tagged target. `IsActiveThisFrame_Internal` returns `PendingArms.Num() > 0`, so on unarmed frames
the extension does nothing at all.

**How often is a frame armed?** `MaxArmsPerEvent = 4`. With ~10 events in a 300-frame run that is
~40 armed frames of 300 ≈ **13 %**, and by construction they fall INSIDE anomaly windows (an arm
requires a live event whose target is not hidden). ⚠ **That is the shape the owner's bisect
measured — during windows, not at boundaries. Stated as CONSISTENT WITH, not established: no
profile was taken.**

📌 **A SECOND, DISTINCT COST, NAMED SO IT IS NOT CONFLATED WITH THE HITCH.** `r.CustomDepth` is
forced to 3 for the whole run, and tagged primitives keep `bRenderCustomDepth` until `FinishRun`'s
`UntagAll` — so from the FIRST arm onward the engine runs a custom-depth pass **every frame**, not
only on armed frames. That is a baseline shift, not a windowed spike, so it does **not** match the
symptom's shape; it is real and it is on for the whole capture.

### §7.2 IS A NON-BLOCKING READBACK ARCHITECTURALLY POSSIBLE? — one paragraph, NOT a design

It is already non-blocking, so that is not the lever. The remaining per-armed-frame cost is the
full-surface GPU→CPU copy plus the render-thread W×H CPU reduction, and the veto only needs its
answer at `FinishRun` — latency is entirely free here, which is unusual and is the whole opportunity.
The post-delivery candidate is therefore **not** "make the readback async" but **"stop reading back a
full surface at all"**: do the per-tag reduction on the GPU (one compute pass producing a ~1 KB
per-tag count-and-bounds buffer) and read back only that, at which point the copy is negligible, the
render-thread scan disappears, and results may land arbitrarily many frames later without affecting
correctness. Journal 045 already filed exactly this as *"GPU-side reduction: PREMATURE, FILED NOT
BUILT"* on the grounds that no measurement said it was a problem — **the owner's bisect is now that
measurement.** ⛔ **A MILESTONE CANDIDATE, NOT WORK FOR TODAY, AND DELIBERATELY NOT DESIGNED HERE.**

---

## §8 RECORDED: THE TICK PIN IS ALSO A LARGE PERFORMANCE WIN ON THE DECOUPLED HOST

Measured by the owner on the client host, packaged:

| condition | `speed_ratio` |
|---|---|
| TICKPIN 1, empty pool | **1.0006** |
| TICKPIN 1, full pool | **1.0012** |
| TICKPIN 0, empty pool | **1.3627** |
| TICKPIN 0, full pool | **1.6568** |
| TICKPIN 1 + `IAI.Capture.Mask 0`, full pool | **1.00090** |

Unpinned, the free-running renderer does roughly **2.6×** the render work the capture consumes and
starves it. **Pinned capture runs at ratio ~1.00 — the regime in which every label guarantee this
project has was validated.** The pin was adopted for label alignment; it is also the difference
between a starved capture and a healthy one on that host.

⚠ **CORRECTION (2026-08-23, the m33 diagnosis — appended, nothing above deleted): the two PINNED
rows in this table are CLOCK-AGREEMENT readings, not health readings.** On this host under the pin
the world game clock advances WITH WALL (owner artifact, keyed by `session_index`: labels `t` span
34.219865 s vs `t_wall` span 34.220319 s against a fixed-step prediction of 3.967 s), so the
pre-m33 `speed_ratio = WallSpan / GameSpan` compares a clock with itself and reads ~1.000 at ANY
starvation — the sped-up-video defect rode exactly this blindness. The UNPINNED rows
(1.3627 / 1.6568) remain meaningful: the fork's decoupled mode keeps a fixed sim tick there, so the
denominator was genuinely pinned below wall. *"Pinned capture runs at ratio ~1.00"* was therefore
not evidence of health; the pin's perf benefit is real but must be read from wall math
(`frames / VideoFps` vs the `t_wall` span), not from this ratio. m33 re-keys the ratio's
denominator onto plugin-owned tick counts, after which pinned readings carry meaning again, and
emits the old world-clock form beside it as `game_clock_speed_ratio` (≈1.000 there = this
signature, made visible).

⚠ **A `P1` CONNECTION, RECORDED AS A CANDIDATE AND EXPLICITLY NOT A CLAIM, AND NOT TO BE
INVESTIGATED:** the client's original never-reproduced one-frame offset (`P1`) was reported at
`speed_ratio` **≈1.2**, and her host runs **1.36–1.66 unpinned**. That is an adjacency between two
numbers, nothing more — no mechanism is proposed and none should be inferred.

---

## §9 GATES

| gate | result |
|---|---|
| Clean builds, BOTH StackOBot targets | ✅ exit 0 both, no `Build.cs` change |
| A44 both encodings, staged artifact | ✅ 10/10 new symbols present at utf16 |
| Item 1 break-it-on-purpose | ✅ §3 — refused at LOD 1, accepted at LOD 0, targeted warns and fires |
| Item 1 fire-rate A/B, same seed | ✅ §4 — **BYTE-IDENTICAL EVENT SET**, filter costs nothing here |
| Item 2 effective-value echo + provenance | ✅ `lodMinCov=7.0000%(compiled)` on the StartRun line |
| Item 2 compiled default still 7.0 | ✅ read back from the running process, not from source |
| Inertness, no new keys set, cross-binary | ✅ **BYTE-IDENTICAL EVENT SET**, 9 events (`S53_MW_GATE200` vs `S54_DET_MW_A`) |
| Determinism pair, `subset_gate.py` | ⚠ **EXIT 1 — reported as it printed, see below** |

⚠ **THE DETERMINISM GATE EXITS 1 AND WAS NOT RELABELLED A PASS.** Invariant core: event count 9,
**every `frame_indices` set, every `manifested` flag**, types/subtypes, the video block,
`positive_frames` 96, `bursts_done` 13, `zero_match_bursts` 2, `non_manifested_events` 0 — **ALL
IDENTICAL**. The differences are `run_summary/end_frame` 358→359 and four EXTRAS that are
**entirely camera-derived** — `camera/rotation[2]`, `coverage_pct` and `coverage_ratio` on one
event, and `labels.jsonl/view/rot[2]`. That is `A64`'s exact predicted shape (a control pair that
under-sampled the pose variance) and `G158`'s run-to-run pixel/pose variation, on a World-Partition
map where `end_frame` is not deterministic. ⛔ **But the pose indicator was NOT pre-declared before
the comparison, so the rule that would license calling the pair INVALID rather than FAILED was not
satisfied — the exit code is reported as it printed.** The pose-independent event-set comparison
above is the claim that carries.

⚠ **The CB_GateLevel determinism pair was ABANDONED, not silently dropped:** `DET_PRE_A` and
`DET_PRE_A2` each failed the B1 pose gate on all three attempts (six consecutive), which is the
harness working — a leg is discarded for HOW it ran. The comparison moved to MainWorld, where B1 is
declared NOT APPLICABLE and, more usefully, where the new code actually runs.

Legs banked under `_bench_sessions_bank\S54_*`.

---

## §10 NOT DONE, NAMED

- ⛔ **NO TAG.** `m31` still open, still awaiting Concorde V-3/V-4.
- ⛔ **No GPU-side mask reduction** (§7.2) — milestone candidate, not designed, not started.
- ⛔ **No lod-specific override of the general 6 % selection coverage gate** (§6.1) — reported, not built.
- ⛔ **The prediction's fidelity limits are named, not closed** (§2): `LODDistanceFactor`,
  `r.StaticMeshLODDistanceScale` and per-platform `MinLOD` are not modelled.
- ⛔ **The filter's real fire-rate cost is UNMEASURED on moving-player content** (§4). This bench
  structurally cannot produce it.
- ⛔ **No profile of the mask pass was taken** — §7.1 is a source reading plus an existing analytical
  cost, and the match to the symptom's shape is stated as consistency, not causation.
- ⛔ **The ini route for both new keys is unproven here** (`G88`); the console route and the shared
  resolve/echo path are proven. Concorde's cook proves the rest via `lodMinCov=…(ini)` /
  `lodHighestOnly=…(ini)` on the StartRun line.
- ⛔ No cook · `P6` did not move · `feature/stencil-capture` untouched at `76cac74` · no force-push ·
  no ratio, no threshold invented anywhere.

## §11 THE OWNER'S PASS ON THE CLIENT HOST

1. **Highest-LOD filter.** Auto-pool leg, delivery OFF. Grep `CURRENT-LOD` — every `lod_popping`
   candidate now logs its level, screen size and source. Expect refusals reading
   `REFUSED ... ALREADY AT LOD n` on objects you are not standing next to, and the surviving pops to
   look **stronger**. If the rate drops to zero, `IAI.Anomaly.LodRequireHighestLod 0` restores the
   old candidate set with no re-cook.
2. **Targeted fire still wins, and now explains itself.** Target something far away with
   `lod_popping` — it FIRES, and warns `TARGETED FIRE on ... ALREADY AT LOD n ... expect a small
   change`. That line is the answer to "why did that one look weak".
3. **Coverage tuning.** `IAI.Anomaly.LodMinCoverage 5` then start a capture — read
   `lodMinCov=5.0000%(console)` off the StartRun line. ⚠ **Do not go below 6.0**: the general
   selection coverage gate is 6 % and would silently become the binding one (§6.1).
4. **Hitching.** Unchanged and expected — the mask stays ON by your decision. `IAI.Capture.Mask 0`
   remains the bisect if you ever want to confirm a hitch is the mask and not something new.
5. **Tick pin.** Nothing to run — recorded. Keep `TICKPIN 1`; unpinned costs ratio 1.36–1.66 on your
   host and pinned holds ~1.00.
