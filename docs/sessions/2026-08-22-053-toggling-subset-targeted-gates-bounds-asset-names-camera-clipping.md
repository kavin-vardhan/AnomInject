# 2026-08-22 — session 053 — the label-correctness set, the day before the client build

**This file is SELF-CONTAINED.** Five plugin commits and two AnomDash commits, all pushed.

⛔ **NOTHING WAS TAGGED. `m31` IS STILL THE OPEN MILESTONE AND STILL UNTAGGED. Highest tag is
`m30`.** `feature/stencil-capture` untouched at `76cac74`. **`P6`'s `node.bounds` half MOVED —
deliberately, item 3 — and `annotation.json`'s FIELD SET did not.** No force-push. No ratio.

Plugin `c15e961..050db0c` · AnomDash `21d9fae..2b4264c`.

---

## §0 ONE SCREEN

| # | Commit | What |
|---|---|---|
| 1 | `8c64be6` | `feat(capture)` toggling anomalies label the ACTIVE FRAME SUBSET |
| 2 | `2818384` | `feat(lod)` targeted fire bypasses the auto-pool distance and coverage gates |
| 3 | `e7b5080` | `fix(labels)` `node.bounds` from rendering components only |
| 4 | `335b453` | `feat(dashboard)` asset names in the targets snapshot |
| 5 | `050db0c` | `feat(camera)` proximity-triggered targeted camera clipping |

AnomDash: `b563eda` asset name as the primary label · `2b4264c` targeted list follows `targetable`.

**Build identity (`G121`): staged exe `F9E0941C`, built == staged verified. THE CONTAINER WAS NOT
RE-COOKED** — this is a code-only hot-swap (`G103`); the session-051 quartet still stands
(utoc `E4FE9B35` · ucas `D9929F6F` · pak `BFB95333`). The pre-change binary `8F58661B` is archived
and hash-verified at `_binary_baselines\StackOBot.exe.session052-prechange-8F58661B`.

**A44, both encodings, BOTH DIRECTIONS:** new symbols present at utf16 (`TOGGLING-SUBSET`,
`active-state table`, `TARGETED FIRE on`, `CameraClippingTriggerRadiusCm`,
`IAI.Anomaly.CameraClipTriggerRadius`, `clipRadius=`, `NO renderable geometry component`) and the
**retired** ones ABSENT (`IsHideTypeAnomaly` 0, `hide-type table` 0). A scan that matched some and
not others is a reading, not blindness.

---

## §1 THE HEADLINE, MEASURED IN PIXELS

`lod_popping` labelled its ENTIRE fire window positive whether or not the forced LOD was applied.
The same MainWorld auto-pool leg, same seed, pre-change binary vs post-change binary:

```
BEFORE   frame_indices 267,268,269,270,271,272,273,274      (8 = the whole window)
AFTER    frame_indices                 272,273,274          (3 = the ACTIVE SUBSET)
```

Every other event in the leg — 2 blink, 4 missing_texture, 2 corrupted_texture — **IDENTICAL**.

🎯 **AND THE PIXELS SETTLE IT.** In-bbox strong-pixel count (per-channel delta ≥ 8/255) against a
clean reference frame, POST leg, the rock's own 118×720 label box:

| frame | 264 | 265 | 266 | 267 | 268 | 269 | 270 | 271 | **272** | **273** | **274** | 275 | 276 | 277 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| strong px | 225 | 255 | 279 | 290 | 375 | 414 | 426 | 487 | **2024** | **2449** | **2942** | 2041 | 1345 | 1313 |
| labelled BEFORE | . | . | . | Y | Y | Y | Y | Y | Y | Y | Y | . | . | . |
| labelled AFTER | . | . | . | . | . | . | . | . | **Y** | **Y** | **Y** | . | . | . |

Frames 267–271 sit at **1.1–2.0×** the no-anomaly ambient drift (264–266 read 225–279 with nothing
firing). Frames 272–274 sit at **9–14×** it. ⇒ **before this fix, 5 of that event's 8 positive
frames (62.5 %) carried no pop at all.** After it, none do.

🚨 **THE CONTROL THAT MAKES IT EVIDENCE: the PRE leg's pixel series is the SAME** (231/327/381/385/444
then 1971/2377/2857) — the pixels did not move, only the labels did. Run-to-run deltas of ~3 % are the
known same-binary pixel variation (`A47`/`G158`).

### §1.1 The mechanism, and it is arithmetic

`FAnomaly_LodPopping` counts `FramesSinceToggle` in **GAME TICKS SINCE APPLY**; the annotated window
is counted in **CAPTURED FRAMES**, and the burst FSM spends `SettleFrames` ticks between them that
capture nothing. With `SettleFrames = 2` and `half_period = 8`:

```
tick capturing 267 -> apply, count 1     271 -> 7
settle (uncaptured) -> 2, 3              272 -> 8  => TOGGLE TO POPPED
268 -> 4   269 -> 5   270 -> 6           273,274 popped;  275 = revert tick
```
⇒ predicted popped set `{272,273,274}`. **Measured `{272,273,274}`.** The popped interval is a
SUFFIX of the window, starting 5 captured frames in — which is why the old labelling was wrong at
the front and looked right at the back.

The owner's second data point reproduces the same arithmetic: `IAI.Anomaly.LodHalfPeriod 2`,
annotated 51–58, manifested **54, 55, 58** — exactly `{54,55,58,(59)}` from the same model.

---

## §2 THE "PAST THE WINDOW" QUESTION — ANSWERED **(b)**, AND (a) IS REFUTED BY MEASUREMENT

The owner sees manifestation running past the annotated window (105–108 against 99–106). The brief
asked which: **(a)** the anomalous state persisting after the window closes, i.e. a late revert, or
**(b)** render-side lag.

**It is (b).** Read the tail of the table in §1: 275 = 2041, 276 = 1345, 277 = 1313 — a **DECAY**,
and the head is a **RAMP** (2024 → 2449 → 2942, still rising on the last labelled frame).

- A **late revert** produces a STEP: frame 275 would be FULLY popped (≥ 2942, continuing the ramp)
  and 276 clean. It reads **2041 — it DROPPED from 2942.** ⇒ the geometry reverted on 275's render.
  **(a) is refuted for this target, by measurement, not by argument.**
- What remains after the revert decays over ~3 frames, and the onset ramps over ~3 frames. That is
  the signature of a temporal accumulation filter, not of a discrete state change.

**Leading mechanism, NAMED AS A CANDIDATE AND NOT ESTABLISHED (the standing invariant):** the
running process's own log shows **TSR/TemporalAA active** (`r.TemporalAA.Quality:2`,
`r.TSR.History.UpdateQuality:3`, `r.TSR.History.ScreenPercentage:100`, from
`AntiAliasingQuality@3`). A temporal history explains both the ramp and the decay. **I did not
measure the filter itself, so this is a supported candidate, not a finding.**

⛔ **NO TOLERANCE CONSTANT WAS INVENTED, and none should be.** A temporal ramp has no single
frame count: it depends on the AA setting, the frame rate and the size of the change.

⚠ **SOURCE-READ, SEPARATE, AND STILL OPEN FOR SKELETAL TARGETS.** `UStaticMeshComponent::
SetForcedLodModel` calls `MarkRenderStateDirty()`, whose recreate is flushed inside
`BeginRenderingViewFamilies` in the SAME frame — which is why the static rock reverts on time.
`USkinnedMeshComponent::SetForcedLOD` does **NOT**: it writes `ForcedLodModel` and notifies
streaming only, and the effective level is recomputed in `UpdateLODStatus()` from
`TickComponent` (`SkinnedMeshComponent.cpp:1186`), which runs in a tick group
(`LevelTick.cpp:1537`) **BEFORE** `FTickableGameObject::TickObjects` (`LevelTick.cpp:1606`) where our
subsystems tick. ⇒ **on a SKELETAL target a forced-LOD write from our tick lands one game tick
late, in both directions.** Not measured — no skeletal multi-LOD target was drawn in any leg here.

🧭 **THE ONE CAPTURE THAT SETTLES THE REMAINDER, and it is the owner's, because it needs his
content:** on the client host, one delivery-OFF targeted leg —
`IAI.Capture.Delivery 0` · `IAI.Capture.Config 2 4 8 14 0` ·
`IAI.Capture.Start "<OUT>\LODCHK" png 777 120 lod_popping =<the object he saw pop late>` — then
read `component_class` in that session's `annotation.json`. **`SkeletalMeshComponent` ⇒ one frame of
the overhang is the skinned forced-LOD lag above and is fixable in one line;
`StaticMeshComponent` ⇒ the whole overhang is the temporal ramp and there is nothing to fix.**

---

## §3 THE GENERALISATION (`8c64be6`)

`IsHideTypeAnomaly` is **WIDENED into ONE table**, `ResolveAnomalyActiveSource`, keyed by anomaly ID
over an explicit set — never inferred from sampling outcome, which was `P3b`, the dataset-poisoning
amplifier (`G94`).

| source | per-frame state read from | ids |
|---|---|---|
| `actor-hidden` | `AActor::IsHidden()` | `blinking`, `missing_object` |
| `anomaly-state` | `IAnomaly::IsCurrentlyAnomalous()` | `lod_popping`, `camera_clipping` |
| `fire-window` | anomalous for the whole window | the other five |

**ONE table, not a sibling predicate**, and the reason is the failure mode: two predicates over
overlapping id sets can disagree silently, whereas the case that MUST stay loud — an id in neither —
is one lookup away in a single table. A planned lighting `flickering` anomaly adds one row.

`IAnomaly` gains `IsCurrentlyAnomalous()` **defaulting to `IsActive()`**, so all eight existing
anomalies are unchanged; `lod_popping` overrides it with `bPoppedPhase`. The sample is taken in
**m20's deferred slot** — the top of the NEXT capture tick, after the injector subsystem has ticked —
so it describes the state the render of that frame actually used (`G81`). Sampling at arm time is
one game tick stale; that is m20's Bug B.

**Zero sampled-active reuses m23's F-LABEL guard** — `manifested:false`, zero positives, a loud
warning naming the state source, `non_manifested_events++`. No second mechanism was built.

**A global-scoped fire (no target actor) records active = 1 by construction**, because such a fire is
appended to a frame only when it has ALREADY been discriminated positive at append time. That is what
keeps the session-global `camera_clipping` path untouched.

`annotation.json`'s FIELD SET does not move: gapped sets already ship (blink has emitted them since
m23) and the overlay and measurement tooling already handle them.

---

## §4 ITEM 2 — WHERE THE GATES SAT, AND BOTH DIRECTIONS ON ONE BINARY (`2818384`)

**Reported rather than assumed: BOTH gates were in the FIRE path** (`FAnomaly_LodPopping::Apply`),
not the selection path, so they applied identically to an auto-pool draw and to an explicitly named
target. `UAnomalyInjectorSubsystem` now carries a scope flag set by
`UAnomalyAutoInjectorSubsystem::TryFireOnce` **around its `ApplyAnomaly` call and by nothing else**.

✅ **PROVEN BOTH WAYS ON THE SAME BINARY (`G96`), MainWorld, seed 777:**

```
AUTO-POOL, shipped 200 cm default : REFUSED 'StaticMeshComponent0' poll_distance_cm 863.91
                                    exceeds the 200.00 cm maximum   ->  0 lod_popping events / 300 frames
TARGETED, same default            : rock at 1221.19 cm FIRES
                                    "[targeted, proximity gates BYPASSED] - 1 qualified"
                                    8 events, every one a 3-frame gapped subset [8,9,10] [20,21,22] ...
```

**The single-LOD guard is deliberately NOT bypassed** — it is not a proximity gate; forcing a LOD on
a single-LOD mesh pops it to itself whoever picked the target.

---

## §5 ITEM 3 — `node.bounds`, AND A CORRECTION TO THE BRIEF'S EXPECTATION (`e7b5080`)

ONE definition, `AnomalyViewport::GetActorRenderableBounds` = the union of the actor's
`UStaticMeshComponent` / `USkinnedMeshComponent` bounds, split out of `IsRenderableComponent` as
`IsRenderableGeometryComponent` so the selection predicate reads as *geometry AND policy*.

**MEASURED before/after, `node.bounds` extent in cm, packaged MainWorld leg:**

| actor | before | after | |
|---|---|---|---|
| `BP_SpawnPad_C` | (209.2, 199.3, **237.9**) | (182.3, 181.9, **10.8**) | **Z × 22.05** |
| `BP_SplineSpawn_C` | (18129.0, 16776.2, 1898.7) | (18104.3, 16760.7, 1898.7) | × 1.001 |
| `BP_Bot_C` (pawn, `SKM_Bot`) | — | — | **UNCHANGED** |
| `SM_Ramp2`, `SM_rock`, `SM_rock_02`, `RoomBuilderSquare_C` | — | — | **UNCHANGED** |

The two blueprint results are the honest span: `BP_SpawnPad`'s box was 22× too tall in Z because of a
non-rendering primitive and that is gone; `BP_SplineSpawn` barely moves because its **own** instanced
component bounds are 181 m across — the `G124` oversized-bounds case, which no component-set change
can touch.

✅ **`BP_Bot` UNCHANGED IN A PACKAGED BUILD CONFIRMS JOURNAL 035'S PREDICTION**, which was left
explicitly unmeasured for a year of sessions: the 1010-unit cube is `UDrawFrustumComponent`, created
under `WITH_EDITORONLY_DATA`, so it exists in the editor and PIE and not in a cooked build.

🚨 **CORRECTION TO THE BRIEF, MEASURED NOT ARGUED: `bbox_px`, `bbox_norm` AND `coverage` DID NOT
MOVE, AND THE m30 CALIBRATION IS NOT VOIDED.** `ProjectActorBoundsToScreenRect` has ALWAYS been a
type-only SM/SK union, and `GetActorScreenCoveragePct` — the quantity the 7.0 % gate tests — unions
over `IsRenderableComponent`. **Only `node.bounds` ever used the whole-actor union.** Evidence:
`bbox_px` byte-identical for every `(id, target)` across the pre/post pair; `coverage_ratio`
identical to the last digit on 9 of 10 events; the post-change gate log reads
`bounds_coverage_pct=9.2572 (threshold 7.0000, ENFORCED)`.

⛔ **NO RE-CALIBRATION WAS PERFORMED AND THE COMMIT SUBJECT WAS CHANGED TO SAY SO.** Re-deriving a
threshold whose input did not move would have manufactured a number. The m30 anchors (last visible
**9.3453 %**, first invisible **3.9045 %**) still describe the quantity the gate tests.

⚠ **`coverage_pct` moved in the 4th–7th significant figure on three events — and it is NOISE, with
the control to prove it.** A SAME-BINARY control pair (`POST_MW` vs `POST_MW2`) exhibits the same
wobble on a DIFFERENT three events (`SM_Ramp2` blink `7.2403564 → 7.1982660`, larger than any
pre/post difference), and the pawn leg moves it in **both directions** — which a shrinking union
cannot do. Anchor-frame camera-pose noise (`A47`/`G158`).

**Nothing else is coverage-derived:** the m26/m27 mask veto counts drawn pixels and never reads
bounds; `node.bounds` is an output-only field with no engine-side consumer.

---

## §6 ITEM 5 — TARGETED `camera_clipping` (`050db0c`)

`Args[0]` beginning with `=` selects targeted mode — the sentinel every targeted path already builds.
Per tick, the near plane goes anomalous while the player is within
`[AnomalyInjector] CameraClippingTriggerRadiusCm` (compiled **200**, plus
`IAI.Anomaly.CameraClipTriggerRadius <cm|default>` for the standing `G88` reason) of the target, and
is restored outside it. Range `[1..1000000]`, out of range **REFUSED not clamped**, and **0 is out of
range on purpose** — a zero radius never fires while looking like a working configuration.

**200 is not a new constant.** It is deliberately the number already chosen for `lod_popping`'s
proximity preference, measured with the SAME metric (sphere-approx bounds distance from
`ResolvePollOrigin`), so the product carries ONE "right next to the player" distance rather than two.

`IsNearClipSlicingNow` moved into `AnomalyViewport::IsGeometryWithinNearClipRadius` so the anomaly and
the capture subsystem share ONE definition of m30's per-frame discriminator.

✅ **VACUITY: THE EXISTING GUARD COVERS THIS PATH — CONFIRMED, NOT ASSUMED.** One target at a
measured 863.91 cm, radius the only variable:

```
radius  200  ->  0 proximity transition(s), last state outside   near clip never pushed
radius 1000  ->  1 proximity transition(s), last state INSIDE    near clip 10.000 -> 100.000
```
Both legs: 8 events, **all `manifested:false`, all `frame_indices` empty**,
`run_summary.non_manifested_events = 8`, and 8 `NEVER MANIFESTED` warnings naming the state source.
**The trigger discriminates and the guard fires.** `coverage_ratio` = 1 on every event, so the
whole-frame extent rule holds for a targeted global-scoped fire.

**WHOLE-FRAME EXTENT IS A PROPERTY OF THE ANOMALY, NOT OF HOW IT WAS FIRED.** A near-plane push
slices whatever is in front of the camera, so pointing the label at the trigger object would
mis-place it — the trigger may be behind the camera when the clipping shows. `FAutoLiveFire` carries
`bWholeFrameExtent`, set for Global-scoped ids at fire time; both the annotation accumulator and
`labels.jsonl` take the whole-frame branch on it, and the session-global route (null target actor)
still takes it too.

⚠ **NAMED, NOT GLOSSED — THE GLOBAL DETERMINISM PAIR IS VACUOUS ON THE POSITIVE BRANCH (`G146`).**
The MainWorld pair applied `camera_clipping` as a session global on BOTH sides (near-clip
`10.000 → 100.000` in the log and `camera.near: 100` in the artifact) and the event set is identical —
but both legs report **"Frames labelled positive: 0, negative: 300"**, so only the NEGATIVE branch of
the discriminator was exercised. m30 needed a purpose-built near-wall scene to see the positive
branch and this bench has no pose that produces it. Equality on the positive branch rests on the
STRUCTURAL argument (a global fire has no target actor ⇒ active = 1 ⇒ the subset equals the appended
set, which is what the pre-change code wrote), not on this measurement.

---

## §7 GATES

| gate | result |
|---|---|
| Clean builds, BOTH StackOBot targets, at HEAD | ✅ exit 0 both, tree clean |
| **Item 1 REGRESSION — `blinking` `frame_indices`** | ✅ **BYTE-IDENTICAL**, 8/8 events |
| **Item 1 REGRESSION — `missing_object` `frame_indices`** | ✅ **BYTE-IDENTICAL**, 8/8 events |
| Same on a skeletal, animated, moving target (`BP_Bot`) | ✅ **BYTE-IDENTICAL**, 8/8 events |
| Comparator positive control (`G96`) | ✅ blink vs missing_object → **DIFFERS**, exit 1 |
| `subset_gate.py` control pair + test pair | ✅ **PASS, EXTRAS = 0**, invariant core ALL IDENTICAL |
| Item 1 forward check — `measure_label_offset.py` | ⚠ **see §8 — DID NOT GO AS PRE-REGISTERED** |
| Item 2 both directions on one binary | ✅ auto-pool REFUSES 863.91 cm / targeted FIRES 1221.19 cm |
| Item 3 before/after, ≥ 3 actors incl. a blueprint pawn | ✅ §5 |
| Item 5 trigger both ways + F-LABEL guard | ✅ §6 |
| Item 5 global determinism pair | ✅ identical — ⚠ vacuous on the positive branch, §6 |
| AnomDash `tsc` + `vite build` + unit tests | ✅ clean, **69/69** |

Legs banked under `_bench_sessions_bank\S53_*` (11 sessions).

---

## §8 🚨 THE PRE-REGISTERED FORWARD CHECK DID **NOT** FIRE — SAID LOUDLY, AS INSTRUCTED

**Pre-registration, verbatim:** *"`lod_popping` was UNMEASURABLE before this change; after it, it
should become MEASURABLE and read near +0 with coverage near 1.00. If it stays UNMEASURABLE, SAY SO
LOUDLY — that means the labels still are not describing the pixels and Item 1 has not worked."*

**It stayed UNMEASURABLE, on BOTH legs**, with `--require-gap 12` satisfied (ceiling ±7, min clean
gap 14) and the instrument's `--selftest` passing 10/10:

```
PRE   lod_popping   1 event   0 measurable   1 unmeasurable   no-manifestation-above-noise(peak/T=-0.16, n=0)
POST  lod_popping   1 event   0 measurable   1 unmeasurable   no-manifestation-above-noise(peak/T=-0.28, n=0)
```

⛔ **BUT THE CONCLUSION THE PRE-REGISTRATION ATTACHES TO THAT BRANCH IS REFUTED BY A DIRECT PIXEL
MEASUREMENT, AND THE INSTRUMENT IS THE THING THAT IS BLIND.** §1's table shows the labelled frames
carry **9–14× the ambient** and the de-labelled frames carry **1.1–2.0×**. The labels ARE describing
the pixels; `measure_label_offset.py` cannot see it.

**Why, and it is a property of the instrument, not of the build:** its detectors are calibrated for
whole-region appearance swaps — magenta and checker — which change most of a bbox. This pop changes
**2.4–3.5 % of the bbox** (2024–2942 px of 84,960), which sits under `min_measurable=0.50` and
`peak/T>=3.0`. m30's own eyeball gate `G-P1` measured the same order for this rock: **2,090 strong
px in-bbox**. **The instrument was never sensitive to `lod_popping` and this run does not make it
so** — which is exactly why it read UNMEASURABLE *before* the change too, and why yesterday's clean
zero-offset validation could not see that this type was wrong (`G135`'s shape, again).

📌 **CONSEQUENCE, RECORDED NOT CHASED: `lod_popping` HAS NO AUTOMATED OFFSET ORACLE.** Its
verification is a direct in-bbox strong-pixel series against the annotated set — the method used in
§1, which lives in this journal and not yet in a committed tool. Building that detector is a
separate change with its own gates and it was NOT started the day before delivery.

---

## §9 NOT DONE, NAMED

- ⛔ **NO TAG.** `m31` still open, still awaiting Concorde V-3/V-4.
- ⛔ **NO RE-CALIBRATION of the 7.0 % gate** — its input did not move (§5). Deliberate.
- ⛔ **The `bbox_px`/`coverage` movement the brief predicted did not occur** — reported as a
  correction rather than manufactured.
- ⛔ **`measure_label_offset.py` was NOT extended to see `lod_popping`** (§8). Filed, not fixed.
- ⛔ **The skeletal forced-LOD one-tick lag is SOURCE-READ, NOT MEASURED** — no skeletal multi-LOD
  target was drawn in any leg (§2). One owner capture settles it.
- ⛔ **The positive branch of the global `camera_clipping` discriminator was not exercised** (§6).
- ⛔ **The ini route for `CameraClippingTriggerRadiusCm` is unproven here** (`G88`) — the console
  route and the shared resolve/echo path are proven; Concorde's cook proves the rest via
  `clipRadius=<n>cm(ini)` on the StartRun line.
- ⛔ **The dashboard changes are NOT visually verified** — `App.tsx` renders `ConnectScreen` without a
  live WS connection, the m28 limitation. `tsc` + build + 69/69 tests only.
- ⛔ **No cook.** Code-only hot-swap; the container is still session 051's quartet.
- ⛔ `feature/stencil-capture` untouched at `76cac74` · no force-push · no ratio, no threshold
  proposed anywhere.

## §10 THE OWNER'S PASS ON THE CLIENT HOST

1. **`lod_popping` labels.** Auto-pool leg, delivery OFF, `IAI.Capture.Config 2 4 8 14 0`. Expect
   `frame_indices` **GAPPED and SHORTER than the window** and one
   `Capture: TOGGLING-SUBSET id=lod_popping ... source=anomaly-state positives=N of 8` line per event.
   A `lod_popping` event still showing 8 consecutive positives means the build did not take.
2. **Targeted fire wins.** `IAI.Capture.Start "<OUT>\T" png 777 90 lod_popping =<a far object>` —
   expect `[targeted, proximity gates BYPASSED]` and a fire. Then the same object on the auto-pool
   path — expect `REFUSED ... exceeds the 200.00 cm maximum`.
3. **`node.bounds`.** Any leg on a blueprint actor: `nodes[].bounds.extent` should now match the mesh,
   not the collision volume. `bbox_px` and `coverage_ratio` must NOT move.
4. **Asset names.** Pull AnomDash, `npm run build`, open the dashboard: the targets list and the
   viewport boxes should read `SM_rock_02`, with `StaticMeshActor_UAID_...` dimmed after it and on
   hover. **This is the one check that is faster for you than for me — I cannot reach the panel
   without a live PIE connection.**
5. **Targeted `camera_clipping`.** `IAI.Anomaly.CameraClipTriggerRadius 300`, then
   `IAI.Capture.Start "<OUT>\CC" png 777 120 camera_clipping =<an object you can walk up to>`.
   **WALK INTO IT AND AWAY AGAIN.** Expect `N proximity transition(s)` > 0 in the run log, positives
   only while you are close AND geometry is inside the near clip, and `manifested:false` with a
   `NEVER MANIFESTED` warning if you never approach. **This is the positive branch this bench
   structurally cannot produce.**
