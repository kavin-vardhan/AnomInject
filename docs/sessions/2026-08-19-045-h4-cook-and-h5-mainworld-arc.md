# 2026-08-19 — 045 — H4, the re-cook, MainWorld, and H5

**Plugin:** `AnomalyInjector` — **docs only, in every part. ZERO production code. NO tag.**
**Bench:** `CaptureBench` — read-only instruments throughout. Local-only.
**Base:** `caaac09` (tag `m25` → `ebf1f16` → `d8482a2`).

⚠ **ONE INVESTIGATION, NINE PARTS.** It began as a one-run H4 test and became the arc that unblocked
path (a)'s environment and then found a different lead entirely. It is not split because it is not
separable — each part exists because the one before it produced something unexpected. **Renamed from
`…-045-h4-preflight-halt.md` on 2026-08-19; that title described only Part One.**

> 🧭 **COLD READER: GO STRAIGHT TO THE `HANDOFF` SECTION AT THE END OF PART THIRTY-ONE.** It
> states `m26`'s state (**FUNCTIONALLY COMPLETE — slices 1, 2 and 3 all shipped and gated; the
> veto is ZERO-ONLY and it fires; next is the owner's play-gate smoke, and NO TAG until then**),
> what is **proven and must not be re-proved**, and the rulings that travel. **You do not need to
> read the thirty-one parts above it — though §198 is worth one minute: the clean result at the end
> was not a clean path.**

## PART INDEX

| part | §§ | what it is | outcome |
|---|---|---|---|
| **One** | 0–9 | **H4 pre-flight** — the original brief's design checked before running | ⛔ **SCOPE GATE.** No run. Three blockers found first. **G116**, **G117** |
| **Two** | 10–16 | **The H4 run**, on the amended design | ✅ **H4 SUPPORTED** as a mechanism, path (b), 8/8 events |
| **Three** | 17–20 | The claim fixed in words · `G112` corrected · path-(a) recon | **G118**, **G119**. Path (a) structurally open |
| **Four** | 21–28 | **Environment scout** | 🚨 **MainWorld was never cooked.** G87's mechanism wrong → **G120** |
| **Five** | 29–33 | **Pre-cook gate** G-1/G-2/G-3 | ✅ all pass. G-3 confirms the G87 correction |
| **Six** | 34–40 | **The re-cook** — MainWorld in, `G118` closed, smoke green | **G121** — the exe hash does not identify the build |
| **Seven** | 41–50 | **MainWorld first launch** (recon) | Movers loaded; `BP_MovingPlatform` moves, `BP_Stomper` does not |
| **Eight** | 51–61 | **Geometry survey** | 🛑 **NO CROSSING PAIR** for full occlusion. **G122**, **G123** |
| **Nine** | 62–68 | **Owner evidence redirects the lead → `H5`** | ✅ **H5 class (ii) SUPPORTED**, reproduced here. **G124** |
| **Ten** | 69–76 | **Traceability characterised · `G124` generalises** | Only `node.name` degrades; **3/13 non-foliage actors collapse the poll cull**. **G125** — a marker contaminated PART NINE's numbers |
| **Eleven** | 77–82 | **The cure measurement** | 🚨 **The cure needs a NEW MEASUREMENT, not a new threshold.** A 2nd class-(ii) instance, and it is not foliage |
| **Twelve** | 83–96 | **Cure OPTIONS costed from source · the disk prune** | **C-1 alone covers both `H5` classes; C-5 is a measured NO-OP; NO candidate is blocked by delivery mode.** **G126**, **G127**, **G128**. 6.23 GB recovered |
| **Thirteen** | 97–102 | **`C-1` RULED the direction · the TIMING design** | 🚨 **selection → fire is ZERO frames, longest gap SIX ⇒ a 12-frame pre-flight does not fit** · `annotation.json` still OPEN at `FinishRun` · **Shipping has no capture, so a non-Shipping cure leaves no hole** |
| **Fourteen** | 103–109 | **Shape ruled (c)+(b) · `M-1` · `M-2` · the ONE definition** | 🚨 **readback latency is ONE frame, so 10-vs-12 was two budgets and never reality** · **`RQT_Occlusion` counts the BOUNDING BOX ⇒ disqualified on CORRECTNESS** · `mask.provided` alone separates *never measured* from *measured zero* |
| **Fifteen** | 110–118 | **`m26` — the implementation PLAN, file by file** | Design CLOSED · the negative branch is a **SHIP GATE** · 🚨 **`P-2` is the riskiest item: a hidden target reads zero and would invalidate EVERY hide-type event** — survivable only because *no qualifying frame* lands in `NOT_MEASURED` |
| **Sixteen** | 119–123 | **`m26` SLICE 1 written · the four amendments · HALT** | Compiles clean, **cannot be validated**: 🚨 **a new GLOBAL SHADER needs a COOK — a hot-swap cannot deliver it, and it fails at engine init EVEN WITH THE SWITCH OFF.** `A-3`'s collision **IS** detectable, two ways |
| **Seventeen** | 124–128 | **Cook preconditions · `G-3` amended · DISK HALT** | Quartet preserved **6/6 hash-verified**; map set declared; 🛑 **cook did NOT run — 0.94 GB free against a 10 GB floor.** 🚨 **`Saved\AnomalyCaptures`: 21 sessions, ZERO banked** |
| **Eighteen** | 129–134 | **Cleanup executed · 21 sessions banked · cook IN FLIGHT** | **0.94 → 21.93 GB**; 21/21 banked incl. **the `m23` play-gate smoke**; **G130**. 🚨 **Ruling 1's order was IMPOSSIBLE — banking 3.89 GB needs 3.89 GB.** Cook is memory-bound at **1 process** (`G97`) |
| **Nineteen** | 136–142 | **`E:` junctions · cook SUCCEEDS · build still cannot boot** | Disk solved, map gate PASS — 🛑 **HALT on `LoadingPhase`: a global shader must load at `PostConfigInit`.** **G131**; runbook §8.6 step 3.5; `G115` fired on me and the diffstat caught it |
| **Twenty** | 143–148 | **Option B ships · build BOOTS · the MEASUREMENT is wrong** | `AnomalyShaders` at `PostConfigInit`, **no `Renderer` dep, load order untouched**; all four gates PASS — 🛑 **HALT: `S4` + an `S3` observation.** Tag 255 pollutes the mask; a control measured ZERO. **`H5` legs deliberately NOT run** |
| **Twenty-one** | 149–156 | **255 IS the engine's `StencilDummy` — ESTABLISHED** | 🚨 **`FColor::White`, bound when custom depth is not produced.** The range candidate is **REFUTED**, and repairing it would have **silenced the detector and vetoed everything**. Detector no longer names an unestablished cause. ⚠ **Its `D-4` "one-frame ordering" explanation is SUPERSEDED by Part Twenty-two** |
| **Twenty-two** | 157–161 | **`F-1` refutes the fix direction — the design cannot be written** | 🚨 **The proxy is ALREADY up to date: `SendAllEndOfFrameUpdates` runs inside `BeginRenderingViewFamilies` in the SAME frame.** Zero ticks needed. Pass point and cvar priority also exonerated ⇒ **source-only diagnosis EXHAUSTED**; the unmeasured `r.CustomDepth` is next. `F-4`/`F-5`/`F-6` answered |
| **Twenty-three** | 162–168 | 🚨 **THE MASK WORKS — 7.25 % vs a 7.80 % banked rect** | Branch **THEY DISAGREE**: cvar is **3 everywhere** (exonerated) but custom depth is produced on **exactly half** the armed frames in a fixed per-burst pattern (mechanism NOT established) — **and my own EVENT-scoped collision discard threw away the frames that did measure** |
| **Twenty-four** | 169–174 | **Fault (i) FIXED · fault (ii) MECHANISM ESTABLISHED** | Frame-scoped discard (`795f2a4`): control **MEASURED_NONZERO 7.25 %** on every full event, all-discarded event lands **NOT_MEASURED** (admit path demonstrated live). 🚨 **The arm gate's hidden read is ONE TICK STALE vs the rendered frame** — 15/15 dummies hidden at render, 0 refuter violations, `missing_texture` control 32/32 REAL. **G132** (`GFrameCounter++` precedes `OnEndFrame`). ⛔ **Fault (ii) NOT fixed; `F-6` NOT claimed; `H5` still blocked; NO tag** |
| **Twenty-five** | 175–181 | **The fault-(ii) fix DESIGNED — `RULING 1` governs it. NO CODE** | 🚨 **Ruling 1: the 255 dummy is a property of THIS BENCH — the fix must close the stale read itself.** Chosen: **Option B — arm from `OnWorldTickEnd`** (post-toggle by position, `LevelTick.cpp:1814`; pre-draw, `GameEngine.cpp:1891`; zero behaviour change outside `AnomalyCapture`) + the **M-4 sampler becomes an ENFORCING whitelist confirmation** (the render is bracketed) + the **item-5 probe** + the **`bRunning` guard** (retires §172's stray arm). Options A/C/D rejected with reasons. Budgets: blinking 4/2 → 4/4; `missing_object` 4-of-6 post-revert; no type drops to zero. ⛔ **NOT implemented — owner's ruling next** |
| **Twenty-six** | 182–187 | **The fix SHIPS and PASSES its four legs · the `SM_Ramp2` control FAILS `F-6` item 2. HALT** | Implementation `4a9631a` + A-1/A-2; **L1–L4 met every pre-declared prediction** (blinking 4/4, `missing_object` 0 in-window + 4 post-revert, the probe firing all three detectors on demand — items 1/3/4/5 PASS, stray arm verified gone). 🛑 **Item 2 FAIL, two limits exposed: `G134` — the instrument is STRUCTURALLY BLIND TO NANITE in 5.1** (proxy relevance never sets `bRenderCustomDepth`; the ramp draws from the identical `CM_CM_RAMP` camera) **and `G133` — the 255 detector is a SINGLE-PIXEL, view-contingent signal**, so event 1 contributed a clean `MEASURED_ZERO` from a never-run pass. ⛔ **`H5` NOT run; extent precondition NOT added (same-turn rule); owner's ruling next** |
| **Twenty-seven** | 188–192 | 🎯 **Both `H5` targets are NON-NANITE — the cure reaches what it was built for** · the extent precondition ships | **`T-1` first, as ruled:** `SM_Bush` and `SM_GenericPlane` **plain**; the discriminator `StaticMeshActor_49` = `/Engine/BasicShapes/Cube` **plain** ⇒ `G134`'s explanation CLOSES, signature predicts measurability **5/5**. ⚠ **But StackOBot's authored structural geometry is overwhelmingly Nanite — "common case" is MEASURED, not projected.** **Ruling 1 built (`3beb3ba`) and gated: `SM_Ramp2` `MEASURED_ZERO` → `NOT_MEASURED` ×8; L1–L4 unchanged** — `F-T2-A`'s literal firing traced to POSE (identical arm ids, identical dispositions, counts group by pose not build) and **the criterion corrected in the record**. **Ruling 2: `SM_Ramp2` retired to a known-Nanite control; NO non-Nanite A35-shaped control exists — said so; A35 → tag as UNTESTED.** **Ruling 3: scope drafted; scene depth IS Nanite-inclusive (`NaniteMaterials.cpp:896,930`) — answer only** |
| **Twenty-eight** | 193–197 | 🎯 **`F-6` COMPLETE · THE `H5` LEGS RUN — the cure identifies both instances that motivated it** | Rulings recorded: the third field APPROVED (`run_summary` +3, and the scope statement now SAYS +3) · the Nanite entanglement into the HEADLINE of `client-delivery.md` · **`G135`** (a restricted-asset calibration level cannot exhibit defects outside its asset set, and the blindness reads as a CLEAN PASS — tension stated, CB_GateLevel NOT changed). **`N-2` replacement `StaticMeshActor_73`: 8/8 non-zero at 5.27 % vs its claimed 6.87 % ⇒ item 2 PASS ⇒ ALL FIVE `F-6` ITEMS PASS.** 🎯 **`BP_SplineSpawn_C` → `MEASURED_ZERO` ×8 against a 22.89 % claim (branch `Z1`); `InstancedFoliageActor_0_0_0` → 1.4 % drawn against a `coverage_pct 100` / whole-frame-bbox claim (branch `Z2`) — every bucket clean, 29/29 frames view-sized, branch `X` did not fire.** ⛔ No incidence claim · no threshold · not a veto test · slices 2/3 not started · NO TAG |
| **Twenty-nine** | 198–203 | ✅ **SLICE 2 SHIPS — `mask.provided` carries the tri-state** | **All five known-answer rows correct: `SM_Ramp2` → `false`, `BP_SplineSpawn_C` (MEASURED_ZERO) → `true`** — the two zeros stay distinguishable, tested BOTH ways; the guarantee is one function switching on `State` alone that never sees a magnitude. G-2 **48/48 in BOTH delivery modes**, INERT when off, 0 mapping mismatches. 🚨 **`A64` justified itself: the first delivery leg read `provided:false` everywhere and looked like a delivery-orthogonality break — it was a bifurcated POSE, refuted by a pose-matched delivery leg reading `true` AND by the `Cylinder` leg reproducing the signature with delivery OFF via frustum culling.** ⚠ **`G-9` path-level subset INCONCLUSIVE (my control pair under-sampled pitch) — invariant core passes POSITIVELY.** §198 records that the clean result was not a clean path. ⛔ **Slice 3 NOT started; no threshold exists anywhere** |
| **Thirty** | 204–208 | ✅ **`G-9` CLOSES — `EXTRAS = 0`** · `framesNoPass` defined everywhere a reader meets it | **Route (a) chosen — make the confound ABSENT, not EXCUSED; (b)'s laundering hazard restated as the reason it was refused.** All three legs settled at the SAME pose (`0/0/0`, `coverage_ratio 0.077977`) first try, so 🎯 **the run-unique set shrank 26 → 4** (`session_id`, `speed_ratio`, `sustained_wall_fps`, `video/path`) and the OFF/ON difference is exactly those **plus `delivery_mode`** — the gate got HARDER and passed. Invariant core re-asserted identical. **Ruling 2: `framesNoPass` is NOT a Nanite counter — corrected in the NO-PASS line, the NOT_MEASURED warning, `G134` and `client-delivery.md`.** **Ruling 3: the "false NEVER means the target drew nothing" sentence into the tag scope statement.** §207 reports the raw measured-vs-claimed pixel areas, **numbers only**. ⛔ **Slice 3 still NOT started** |
| **Thirty-one** | 209–213 | 🎯 **SLICE 3 SHIPS — the veto is ZERO-ONLY and IT FIRES. `m26` functionally complete** | **The ruling recorded VERBATIM (§209), including why a ratio was refused despite the four targets separating cleanly: every GOOD target measured is a convex primitive, and a complex-silhouette legitimate target — which would break a ratio rule — does not exist in the measured set (`G135`'s failure shape).** Implementation tests the ENUM STATE only; `manifested` evaluated first so `vetoed_events`/`non_manifested_events` are disjoint by construction. **`BP_SplineSpawn_C`: 8/8 vetoed, annotation `anomalies: []`, `vetoed_events=8`.** 🚨 **The foliage at ~1.4 % of its claim: NOT vetoed — the rule's own guard.** 🚨 **`SM_Ramp2`'s 8 `NOT_MEASURED` events: ALL KEPT — the data-destroying direction closed.** `G-9` re-run at slice 3: **EXTRAS 0**, event SET identical across modes. `P6` 48/48, `run_summary` +4. **The accepted cost, the `A35` ruling and `L1`–`L3` into the tag statement and `client-delivery.md`.** ⛔ **NO TAG — owner play-gate smoke first** |

⚠ **ONE INVESTIGATION, THIRTY-ONE PARTS** *(the "nine" in the note below predates Parts Ten
onward; the reason it is not split is unchanged).*

**WHERE IT ENDS — SESSION CLOSED 2026-08-20 AT THE END OF PART THIRTY-ONE.** 🎯 **`m26` IS FUNCTIONALLY
COMPLETE — SLICES 1, 2 AND 3 ALL SHIPPED AND GATED.** Both faults fixed (`795f2a4`, `4a9631a`), the
extent precondition shipped (`3beb3ba`), `F-6` COMPLETE on all five items, the `H5` LEGS RUN (the
cure identifies both instances that motivated it), slice 2 REPORTS through `mask.provided`
(`ece343f`) with the two zeros proven distinguishable both ways, `G-9` CLOSED at `EXTRAS = 0` on a
pose-matched pair (PART THIRTY), and **SLICE 3's VETO SHIPS (`65deadc`) — ZERO-ONLY: it removes
`BP_SplineSpawn_C`'s 8 events and removes NOTHING else, including the foliage at ~1.4 % of its
claim.** ⛔ **NO RATIO, NO THRESHOLD — the reasoning for refusing one is recorded verbatim at
§209.** ⚠ **Limits that travel: the ACCEPTED COST (the over-claim case is NOT cured) · `A35` (a
zero-silhouette target may still cast shadow; vetoed anyway, by ruling) · `L1`–`L3` · `G134` — the
mask cannot see Nanite geometry on UE 5.1, and on this title that is the COMMON CASE; `G133` closed;
`G135` records why the calibration level could never have shown either.**
⛔ **NO INCIDENCE CLAIM · class (i) still ENUMERATED-NOT-OBSERVED · the A35 over-fire property
UNTESTED, no control for it exists.** `feature/stencil-capture` **untouched** throughout — *mined,
never resumed*. **`P6` never moved (measured 48/48 in Parts 24, 26, 27, 28, 29 and 31). NO TAG since
`m25` — THE OWNER'S PLAY-GATE SMOKE COMES FIRST.**
⚠ **Production code appears for the first time in PART FOURTEEN (log-only `M-1` instrumentation, on
owner permission); Parts One–Thirteen carry ZERO.**

🧭 **→ THE `HANDOFF` SECTION AT THE END OF PART THIRTY-ONE IS THE COLD-START ENTRY POINT.**

---

# PART ONE — H4 pre-flight: the run is BLOCKED, and the block was found before it ran

**NO RUN HAPPENED. No branch obtained. This part records a pre-flight that reached the
SCOPE GATE, and the three findings that made stopping the right answer rather than a delay.**

---

## 0. Environment, verified at cold start (G43, A44)

| track | state |
|---|---|
| `AnomalyInjector` | `caaac09`, clean, **0 unpushed**, `m25` → `ebf1f16` → `d8482a2`, on remote |
| `CaptureBench` | `bb79012`, clean, **no remote** (local-only by design) |
| staged exe | `D:\...\Builds\BenchGate\Windows\StackOBot\Binaries\Win64\StackOBot.exe` = **`101AFEA4`** = `m25` ✅ |
| baselines beside it | `.s4-2` `259BF64F` · `.s4-0` `834BB30A` · `.m24` `3BA854FB` · `.m23` `85A39CFB` — all match the status block |
| bank | **85** session dirs |

Everything matched the expected state exactly. Nothing was reconciled.

---

## 1. The three blockers, in the order they bite

H4's run design is *"ONE packaged run, TWO TARGETS IN THE SAME RUN — the occluded one by targeted
fire, and a known-visible control, same run."* **That run cannot be executed on shipping code, and
would not be gradable if it could be.**

### B1 — a capture run carries EXACTLY ONE targeted (anomaly, actor) pair

`AnomalyCaptureSubsystem.cpp:1093` — `BeginFire()` is the only producer of session events:

```
const bool bFired = Auto
    ? (bTargetedMode ? Auto->TryFireSpecific(TargetAnomalyId, TargetActorName, TargetAnomalyArgs)
                     : Auto->TryFireOnce())
    : false;
```

`AccumulateFrameEvents` iterates `Auto->GetLiveFires()` and nothing else, so **a target that is not in
`LiveFires` produces no session event, no label row, no `annotation.json` entry and no provenance
record.** Every route into `LiveFires` was checked:

| route | reaches a NAMED occluded actor? |
|---|---|
| `BeginFire` → `TryFireSpecific` | **yes** — resolves via `AnomalyTargeting::FindActorsMatching("=name")`, no viewport predicate (G33's escape hatch). But it takes the run's **single** configured pair. |
| `IAI.Auto.FireOnce` / WS `auto_fire_once` → `TryFireOnce` | **no** — draws from `GetVisibleRenderableActors`, which routes through `IsUnoccluded`. |
| `IAI.Apply` / WS `inject` | **no** — calls `Injector->ApplyAnomaly` directly. Never creates a `LiveFire`, so it is invisible to the label writer. |

`IAI.Capture.Start`'s own usage string is explicit: *"Pass BOTH [anomaly] and [targetActor] for a
TARGETED run (fires only that anomaly on only that actor each burst)."*

⇒ **two targets in one run requires a production change. SCOPE GATE.**

### B2 — even with that change, the certified oracle could not grade the result

`a54_oracle.py :: a56_check` takes **one modal bbox per leg**, pooled across every `anomalies[]` entry
of every label row. Two simultaneous targets ⇒ two bboxes ⇒ modal coverage ≈ 0.50, against
`A56_MIN_MODAL = 0.90` ⇒ `NOT-A54-CERTIFIABLE` **by construction, before any pixel is read**.

The oracle's own header says it: *"the bbox is the leg's MODAL bbox, taken once per leg."*
A two-target leg is outside its design, not merely awkward for it.

### B3 — the oracle cannot return ABSENT on ANY target other than `StaticMeshActor_49`

`CALIB_BBOX = (0.0, 485.2, 306.1, 234.8)` is **`StaticMeshActor_49`'s pixel bbox**, and
`pose_match(modal)` is a **conjunct of `a56_check`**. A leg fired at any other actor exits **2 /
`NOT-A54-CERTIFIABLE`**.

**So "A54 = ABSENT on the occluded target" — half of the pre-declared H4-CONFIRMED signature — is not
obtainable from the certified instrument in ANY run design.** And the oracle's failure text would
print *"P8: this leg's camera settled in a pose TAU was NOT calibrated on"* — **a cause it has not
established**, since the real cause is the target, not the pose.

> This is **S4-1's ruling recurring on a second axis.** S4-1 established that `CALIB_BBOX` is frozen in
> PIXELS and therefore cannot judge an off-calibration **resolution**. The same constant is equally
> frozen against the **target**, and that had not been stated. Same constant, same shape, different
> axis — and the harness's own new rule (*a gate that fails safe still misleads if its label names a
> cause it has not established*) applies verbatim. → **G117**.

---

## 2. PF1 — the target re-verified, and `StaticMeshActor_11` FAILS

The recon figures were **not** trusted. They were rebuilt offline from the two frozen, readable
sources — `make_gate_level.py`'s fully deterministic geometry and the camera samples actually banked —
in `CaptureBench/tools/h4_recon.py`. **No MCP bridge was used, so A59 does not arise: nothing read a
live editor and there was no second-project ambiguity to resolve (G97).**

**The model was certified against three independent banked quantities first (G96 — a model is checked
against a known answer before its unknown answers are read):**

| quantity | model | banked engine | Δ |
|---|---|---|---|
| `poll_distance(_49)` | `418.09227699` | `418.09228516` | `8e-6` |
| `bbox_px(_49)` modal pose | `(0.0, 485.2174, 306.0870, 234.7826)` | `(0.0, 485.21737, 306.08701, 234.78263)` | exact to 5 d.p. |
| occlusion `_49` / `_73` / `_85` | 9/9 · 9/9 · 9/9 | 9/9 · 9/9 · 9/9 | — |

Only then were the candidates read.

### 2.1 The clause that the brief could not have named

`EvaluateSelectionProvenance` short-circuits through `IsComponentRenderableVisibleInternal`, whose
clauses are evaluated **in this order**:

```
C1 renderable (SM/SK ∧ IsVisible)
C2 poll radius   dist(pollOrigin, bounds.origin) − sphereRadius > GPollRadius (default 1800 cm)
C3 frustum
C4 occlusion                                   <- the ONLY clause H4 is about
```

**All four collapse to the same artifact string** — `valid:false`, `0/0`, `coverage_pct −1`,
`poll_distance −1`. So a target that fails **C2** manufactures H4's predicted CAUSE signature for a
reason that has nothing to do with occlusion.

| actor | shape | poll dist (cm) | on-screen | C2 (≤1800) | occluded 9/9 |
|---|---|---|---|---|---|
| `_5` | cube | 1528.8 | 92.2 % | pass | full |
| **`_11`** | cylinder | **2543.7** | 96.6 % | **FAIL → C2** | full |
| `_22` | cylinder | 2279.5 | 96.7 % | **FAIL → C2** | full |
| `_24` | cube | 2654.7 | 97.2 % | **FAIL → C2** | full |
| `_33` | cylinder | 2020.3 | 97.2 % | **FAIL → C2** | full |
| **`_100`** | cone | **1031.9** | **97.6 %** | **pass** | **full** |
| `_139` | sphere | 1848.0 | 97.4 % | **FAIL → C2** (by 48 cm) | full |
| `_135` | sphere | 1264.5 | 7.5 % | pass | full — but off-screen |

⛔ **`StaticMeshActor_11` — the brief's first choice — is REJECTED.** Not on the on-screen fraction
(96.6 %, comfortably over the 94 % line) but on **C2**: at 2543.7 cm it is outside the default
1800 cm poll radius, so its `valid:false` would be the poll cull, not occlusion. **Of the brief's
named candidate set `{_11, _22, _24, _33, _100, _139}`, only `_100` survives**, and it is the
joint-highest on-screen member of that set.

### 2.2 The occlusion evidence, per target, not as a count

Rigorous floor model — **cube occluders only**, because for a box the collision primitive *is* the
AABB and no approximation of curved collision is involved. Occlusion is invariant across the A47
bifurcation because the eye position is invariant (`(-1500, 0, 260)` on **844/844** banked samples,
up from journal 036's 369 as the bank has grown).

- **`StaticMeshActor_100`** (cone, `(-500, 500, 60)`) — **`StaticMeshActor_86` blocks all 9 rays on its
  own.** `_86` is a cube at `(-900, 300, 135)`, extent 60; the eye→`_100` ray passes through
  `(-900, 300, 140)`, dead inside it. `CB_Floor` additionally blocks 4 rays, and **0 rays are blocked
  ONLY by the floor** — so the verdict survives removing the floor from the model entirely.
- **Control `StaticMeshActor_49`** — **9/9 rays clear** in the *most permissive* model (every shape,
  including the floor, admitted as an occluder). Matches the banked engine value on every non-degenerate
  run.

Two corrections to journal 036's table, both from the growth of the bank and the stricter model:
the fully-occluded floor set is **10**, not 8 (`_32` and `_113` were missed; `_32` is occluded **only**
because of the floor — 4 of its 9 rays have the floor as their sole blocker — and is therefore a weak
candidate on its own terms), and the on-screen percentages move (e.g. `_11` 94.9 % → 96.6 %) because
the denominator is now 844 samples rather than 369.

---

## 3. PF2 — the CAUSE signature is **NOT unique to H4**, and the bank already proves it

**This is the finding of the pre-flight.**

`selection_provenance.json` exists, is written on this build and this path
(`AnomalyCaptureSubsystem.cpp:1720-1737`) — ⚠ **only when `!bDeliveryMode`**, so the H4 run must be
non-delivery — and `valid:false` is reachable from source.

But PF2 asks more than "is it reachable": *a guard that has never fired is not a guard.* It has fired.
**21 of 780 banked provenance records report `valid:false` with `0/0` samples** — and **every single one
is on `StaticMeshActor_49`, which is provably unoccluded on all 9 rays.**

Every one was explained (`CaptureBench/tools/h4_provenance_false.py`), by recomputing the frustum test
at that frame's actual banked camera rotation:

```
records explained                     : 21
...where the target was OUT OF FRUSTUM: 21     <- clause C3
...where the target was FULLY OCCLUDED:  0     <- clause C4, H4's clause
...where view.valid was not true      :  0
...that STILL carried a label row     : 21
```

They sit in `DISC/DA_fps90` (4), `I10/L3_client39` (2 — the P8 bifurcated leg), `M23/HF2_nat240`
(13 — every anchor), `S3A2_BASE` (1) and `S3B_S4M4b_sp170_try1` (1). In each case the anchor frame
landed while the camera was still slewing through settle, 22°–116° of yaw off the modal pose.

### 3.1 The discriminator, sharpened — and this must go into the branch table before the run

⛔ **`valid:false` + `0/0` ALONE DOES NOT DISCRIMINATE.** Pre-declared as *"unique to H4"*; it is not.

✅ **The discriminating quantity is the PAIR** — provenance `valid:false` **together with the anchor
frame's `bbox_valid` from `labels.jsonl`:**

| clause | provenance | `bbox_valid` at the anchor frame |
|---|---|---|
| **C3 frustum** | `valid:false`, 0/0 | **`false`** — the projector agrees the target is off-screen |
| **C4 occlusion (H4)** | `valid:false`, 0/0 | **`true`** — the projector is blind to occlusion and still emits |

**All 21 banked cases read `bbox_valid: false`. There are ZERO banked instances of the H4 divergence.**
That is the honest reading of the bank: it does not corroborate H4, and it does not refute it either —
it establishes that the signature H4 was to be recognised by has an incumbent, unrelated producer.

→ **G116.**

### 3.2 And C2 is a third producer, which is why §2.1 rejects `_11`

The same collapse hides the poll-radius cull. `_11`'s `valid:false` would have been read as occlusion.

---

## 4. PF3 — the shadow property, recorded, not acted on

A35: hiding an object removes its **cast shadow**, brightening pixels *outside* its bbox. A54 keys
strictly *inside* the bbox, so a shadow change is a real visual change the oracle will not see. This is
a **caveat on the phrase "contributes no pixels"**, not a gate and not a disqualifier — and it travels
with the result either way.

`CB_Sun` is a movable, shadow-casting `DirectionalLight` at `Rotator(-45, 35, 0)` ⇒ light direction
`(0.5792, 0.4056, −0.7071)`. Projecting each target's AABB silhouette onto the floor plane `z = 0` and
asking how much of that patch the eye can reach:

| target | shadow centre | on-screen (modal) | patch samples occluded from the eye |
|---|---|---|---|
| **`_100` (the pick)** | `(−451, 534)` | yes | **7 / 9 — so 2 of 9 are VISIBLE** |
| `_11` | `(949, −1066)` | yes | 9 / 9 (fully hidden) |
| `_139` | `(149, 1134)` | yes | 9 / 9 (fully hidden) |
| control `_49` | `(−1010, −237)` | yes | 0 / 9 (fully visible) |

⚠ **RECORDED AS A PROPERTY OF `_100`: it does throw shadow into visible pixels.** So on `_100`,
"contributes no pixels" is **false as stated** — hiding it removes a shadow the camera can see, and
A54 would not see that change because it lies outside the bbox. The two targets whose shadows are
*also* fully hidden (`_11`, `_139`) are exactly the two rejected by C2. **There is no candidate in the
brief's set that is clean on both axes at once.**

---

## 5. PF4 — scoping is OFF by default, but it is NOT in any capture artifact

Default is OFF (`bViewportScopingEnabled` initialises false). It matters because
`Anomaly_Blinking::Apply` (`:47-49`) selects through `AnomalyViewport::FindVisibleActorsMatching`
(frustum ∧ `IsUnoccluded`) **when scoping is ON** and plain targeting when OFF — so scoping ON would
match 0 actors on an occluded target and read as a clean null for the wrong reason.

⚠ **`run.json` / `run_summary.json` / `annotation.json` do NOT carry viewport scoping.** A48 asks for
an effective read-back from the artifact, and there is no capture-artifact field to read. Three routes
were found; two are usable:

1. ✅ **`LogAnomaly: blinking: matched N actor(s) for '=<Actor>' …`** in `Saved/Logs/StackOBot.log` —
   which the packaged build writes **without `-log`** (verified against today's log). This is the
   **strongest** available echo and it is *behavioural*, not declarative: on a fully occluded target,
   `matched 1` is only possible if the occlusion-aware selection path did not run. **`matched 0` would
   be the scoping-ON signature and is exactly the false null PF4 exists to exclude.**
2. ✅ **WS `ControlSnapshot` → `viewportScoping`** (`ControlSnapshot.cpp:211`) — a live read of the
   subsystem member, fully independent of the console command. The client precedent exists
   (`verify_lastrundir.ps1`, token read from `DefaultGame.ini`, never hardcoded — G112).
3. ⛔ The heartbeat line that prints scoping is `UE_LOG(..., Verbose, ...)` — **not in the default log.**
   And `IAI.SetViewportScoping -> ON/OFF` is the setter echoing its own argument, which A48 explicitly
   does not accept.

---

## 6. The free measurement — a correction to the prediction, made BEFORE the run

The pre-declared P6 observation is *"`coverage_pct == 0` with `coverage_ratio > 0`"* (journal 036 §3.5,
carried into the H4 filing). **From source it is `−1`, not `0`:**

- `annotation.json` `coverage_ratio` ← `Ev.CoverageSum / Ev.CoverageCount`, accumulated at
  `AnomalyCaptureSubsystem.cpp:1610-1613` under `ProjectActorBoundsToScreenRect` — **occlusion-blind**,
  so it is `> 0` for an occluded but on-screen target.
- `annotation.json` `coverage_pct` ← `Ev.Provenance.CoveragePct` (`:1691`), and
  `FSelectionProvenance::CoveragePct` **defaults to `-1.0f`**. `EvaluateSelectionProvenance` returns
  before assigning it, so the sentinel survives.

**The bank confirms it: all 21 `valid:false` records read `coverage_pct: -1`.**

Third instance of a certified-adjacent claim resting on **prose** rather than a measurement — G106 was
the first, m24's `key_ring_*` the second. **P6 DOES NOT MOVE. No `annotation.json` field is added,
removed, renamed or recomputed.** The correction is to the *prediction*, not to the contract.

---

## 7. What was NOT done, and why that is the result

⛔ **No packaged run.** The SCOPE GATE fired at B1: the design as briefed needs plugin production code
to accept a second targeted pair. That is a scope change, not H4.

⛔ **No branch was read.** H4-CONFIRMED / H4-REFUTED / PROVENANCE-BLIND / VOID / POSE-TRAP were all
restated verbatim before anything was measured, and none of them obtained, because the measurement they
grade was never taken.

⛔ **`feature/stencil-capture` untouched.** ⛔ **No tag.** ⛔ **`B1`, `TAU`, `CALIB_BBOX`, `A54` and the
oracle are UNTOUCHED.**

**H4's status is unchanged: NAMED, NOT ADOPTED.** Nothing here is evidence for or against it.

---

## 8. Forward — the amendment the run needs, stated for a chat-side ruling

The **REFUTED branch survives all three blockers intact**: it rests on whether a label is emitted at
all (`labels.jsonl` `bbox_valid`, `annotation.json` `frame_indices`), on the scoping echo, and on the
control having produced a label. **It needs no oracle and no A54 verdict.** The asymmetry that made the
run worth one shot is undamaged.

What needs a ruling before anything runs:

1. **Two legs instead of one run** — occluded leg at `StaticMeshActor_100`, control leg at
   `StaticMeshActor_49`, same binary, same geometry, back to back, both banked. `run_leg.ps1` needs a
   **targeting parameter** (harness-side, explicitly blessed by the brief). Cost: the control no longer
   establishes the occluded leg's pose *in the same process*. Partial recovery is available and is
   precedented — `settle_window()` returns `modal_rot`, a **target-independent** camera reading present
   on any leg, so an A64-style **inter-leg pose match on `modal_rot` at the frozen `SETTLE_TOL_DEG`**
   can be asserted. **A discriminator, not a gate** — same standing as A64's `coverage_ratio`.
2. **`IAI.SetPollRadius 0`** on the occluded leg, echoed — or `_100` accepted as-is at 1031.9 cm.
   `_100` passes C2 on shipping defaults, so this is optional and the default is preferable
   (*"the whole point is current shipping behaviour"*). Recorded because the choice is real.
3. **What replaces "A54 = ABSENT" on the occluded leg.** Two options, and the difference matters:
   - **(a) a RAW in-bbox luminance series, explicitly NOT an A54 verdict** — precedented: S3 read L3's
     raw series before reporting when the oracle declined. Cheap, honest, licenses less.
   - **(b) extend the oracle with a per-leg calibration bbox** — a definition change to a certified
     instrument, requiring A53's known-answer re-gate against one known-ALIGNED and one known-ABSENT
     control. This is `B1`-NDC's sibling on the target axis and should be scheduled with it, not
     smuggled into a measurement turn.
4. **The sharpened discriminator from §3.1 must be written into the branch table** before the run, not
   after — provenance `valid:false` **paired with** anchor-frame `bbox_valid: true`.

Nothing above is adopted. It is the shape of the decision, not the decision.

---

## 9. Artifacts

**`CaptureBench` (local-only, read-only instruments, no run performed):**

| file | what it is |
|---|---|
| `tools/h4_recon.py` | rebuilds the gate-level occlusion / on-screen / shadow picture from `make_gate_level.py` + the bank. Refuses to report candidates until it reproduces the banked known answers. |
| `tools/h4_detail.py` | names the blocking occluder per target, isolates floor-only occlusion, and lists every banked `valid:false`. |
| `tools/h4_provenance_false.py` | separates clause C3 from clause C4 on every banked `valid:false` by recomputing the frustum test at that frame's actual rotation. |
| `tools/h4_target_pick.py` | screens candidates against **all four** clauses, and self-checks against `poll_distance` and `bbox_px` before printing. |

**`AnomalyInjector`:** this journal, **G116**, **G117**, and the status block. **Zero production code.**

---
---

# PART TWO — THE RUN. The brief was amended chat-side, and the run went ahead.

**All four decisions in §8 were ruled on chat-side before anything launched. The amended design was
executed the same turn. Still ZERO plugin production code; still no tag.**

## 10. What the rulings changed

| # | ruling | effect |
|---|---|---|
| 1 | **two legs, not one run** | occluded `_100`, control `_49`, same binary, same geometry, adjacent launches. `run_leg.ps1` gained `-Anomaly` / `-Target` / `-BankPrefix`. Lost intra-run pose anchor accepted; `modal_rot` inter-leg match adopted **as a discriminator, never a gate** |
| 2 | **shipping defaults** | **no `IAI.SetPollRadius`.** `_100` passes C2 at 1031.9 cm on the shipped 1800 cm radius, so no lever was used and none was needed |
| 3 | **option (a)** | raw in-bbox luminance series, **reported as a series**. No A54 verdict on the occluded leg. Option (b) (per-leg calibration bbox) **filed** with `B1`-NDC and `B2` |
| 4 | **sharpened discriminator** | the C3/C4 pairing entered the branch table **before** the run |
| 5 | **shadow resolved by wording** | no better target hunted, no level re-authored — the claim narrowed instead (§14) |
| 6 | **both A48 echoes, ranked** | echo 1 behavioural (primary), echo 2 WS state (corroborating), **disagreement ⇒ halt** |

The branch table was **pre-registered as a file before either leg launched**, not merely restated in
prose, so the restatement is an artifact rather than a claim about one.

## 11. How the legs ran (A63, every attempt banked)

| leg | target | attempts | verdict | B1 |
|---|---|---|---|---|
| `H4_CTRL_49` | `StaticMeshActor_49` | **2** | try1 **discarded and banked**, try2 accepted | **APPLIES** |
| `H4_OCC_100` | `StaticMeshActor_100` | **1** | accepted | **NOT APPLICABLE (G117)** |

⚠ **The control's discarded attempt 1 is worth reading precisely, because the harness's label is
generic and the failing conjunct was not the one the label names.** try1 printed
`bbox=(0.0, 485.2, 306.1, 234.8) pose_match=True` — the pose matched CALIB_BBOX **exactly** — but
`distinct=8, modal=45.8%`. So it failed A56's **self-consistency** conjuncts
(`coverage >= 0.90`, `distinct <= 3`), **not** `pose_match`. The harness said *"POSE GATE FAILED (B1) —
CAUSE NOT ESTABLISHED"*, which is the honest label it was given in `bb79012`, and this is a **third
distinct cause** behind that same label — after resolution scope (S4-1) and genuine A47 bifurcation.
Recorded; not attributed further.

**B1 was applied on the control and declared NOT APPLICABLE on the occluded leg**, exactly as G117
requires. It was **not skipped to make a leg pass** — an off-calibration leg has no pose gate
available at all, and `check_pose.py` still ran in reporting-only mode. Its output is instructive:
`ratio m/CALIB = (None, 0.9161, 0.3777, 0.394)` — non-uniform, with `modal_rot` **stable at (0,0,0)**
and `distinct=1, modal=100%`. Under the harness's own printed discriminator that is neither
"resolution scope" nor "genuine A47 bifurcation": it is **a different target**, the third cause the
discriminator does not enumerate.

## 12. The A48 echoes — both obtained, and they AGREE

**Echo 1 — behavioural, PRIMARY, banked with each leg** (`_leg_game.log`, copied out of
`Saved/Logs` per attempt because the game rotates it on the next launch):

```
LogAnomaly: blinking: matched 1 actor(s) for '=StaticMeshActor_100' at half-period 3 frame(s).   x8
LogAnomaly: blinking: matched 1 actor(s) for '=StaticMeshActor_49'  at half-period 3 frame(s).   x8
```

`matched 1` **on a target proven fully occluded** is only possible if `Anomaly_Blinking::Apply` took
the plain-targeting path. With scoping ON it takes `FindVisibleActorsMatching` (frustum ∧
`IsUnoccluded`) and **necessarily** logs `matched 0`. This is a direct observation of the exact
condition A48 cares about, not a setter echoing its own argument.

**Echo 2 — live subsystem read-back, CORROBORATING** (`ControlSnapshot.session` over WS):

```
viewportScoping   : False        <- agrees with echo 1
pollRadius        : 1800 cm      <- the shipped default, read back rather than assumed
minScreenCoverage : 6 %
```

**The two agree. No halt.** And per Ruling 2, recorded: **the 1800 cm radius is what admits `_100`**
(1031.9 cm). A target ~800 cm further out is culled at clause C2 **before occlusion is ever
consulted** — a real property of the shipping selector, not an artifact of this run. It is also why
`StaticMeshActor_11` was rejected in pre-flight.

### 12.1 🚨 Getting echo 2 exposed an unrelated, security-relevant defect → **G118**

The first attempt read the token from `Config/DefaultGame.ini` — the file a developer edits, and the
route `verify_lastrundir.ps1` established — and the server **rejected it**. The staged build's own
startup log says why:

```
=== Control server token: TESTVALUE123 (from DefaultGame.ini [AnomalyControlServer] Token) ===
```

The project ini carries a rotated 64-char token. **The cooked build enforces `TESTVALUE123`.** The
parenthetical is the trap: the binary says *"from DefaultGame.ini"* and means **the cooked one**,
which is a different file from the one on disk. So **G112's placeholder guard validates an artifact
that is not the one enforcing anything** — it fires on the source ini, which is clean, and stays
silent about the build that is actually listening.

Fixed harness-side by A44: the token is now read from the **running process's own log line**, which is
a read-back rather than a hardcoded secret, and the script prints the mismatch and the placeholder
warning loudly rather than proceeding quietly. **The measurement continued because this is orthogonal
to H4** — but the defect is real and is filed, not absorbed.

## 13. THE RESULT — **BRANCH H4-SUPPORTED**, on all four conjuncts, 8/8 events

Same binary (`101AFEA4` = m25), same `1280×720` windowed at 100 % scale, same `VideoFps 30` pinned and
paced, same burst schedule, `capture_path: "sve"` **not forced**, `content_clock: "wall"` asserted
positively on both, `delivery_mode: false` on both, adjacent launches, **identical `modal_rot`**.

| quantity | occluded `_100` | control `_49` |
|---|---|---|
| label rows for the target | 59 | 59 |
| **`bbox_valid` TRUE** | **59 / 59** | 59 / 59 |
| **`visible_positive` rows** | **59** | 59 |
| distinct bboxes / modal | 1 · `(905.7, 444.5, 115.6, 92.5)` 100 % | 1 · `(0.0, 485.2, 306.1, 234.8)` 100 % |
| annotation events | 8, **`manifested: true` on all 8** | 8, `manifested: true` on all 8 |
| **`coverage_ratio`** | **0.01160339** (> 0) | 0.07797734 |
| **`coverage_pct`** | **−1** | 7.7977 |
| **provenance `valid`** | **`false` × 8** | `true` × 8 |
| **occlusion samples** | **0 / 0 × 8** | **9 / 9 × 8** |
| `poll_distance` | −1 (sentinel) | 418.1 |
| anchor in the blink's hidden set | **false × 8** | false × 8 |
| **clause** | **C4 OCCLUSION — 8/8 DIVERGENCE** | selection succeeded — 8/8 |
| A54 | **not in scope (G117)** | **7/7 ALIGNED, 7/7 decidable**, median margin **0.10527** |
| A54 positive control | n/a | **decisive BOTH ways**: `+1` → 7/7 SHIFTED, `−1` → 8/8 SHIFTED |

**The C4 divergence — provenance `valid:false` + `0/0` with `bbox_valid: TRUE` on the same anchor
frame — had never been observed in 780 banked records. It occurred on 8 of 8 events here.**

### 13.1 The clause chain, and which link rests on what

The four short-circuit clauses all write the same artifact string (G116), so each had to be excluded
separately. **Two of the exclusions come from the artifact and one does not — that distinction is
load-bearing and is stated rather than smoothed over.**

| clause | excluded by | strength |
|---|---|---|
| **C1** renderable / `IsVisible` | **the artifact** — every anchor (3, 15, … 87) is **absent** from that event's `frame_indices`, i.e. the actor was **not** in the blink's hidden phase at the anchor. The control has the identical anchor/hidden structure and returns `valid:true` | direct |
| **C2** poll radius | **NOT decidable from the artifact** — the projector has no radius test, so `bbox_valid` cannot speak to it, and `poll_distance` is the −1 sentinel. Excluded by the certified offline model (`_100` at **1031.9 cm**, model verified to **8e-6** against `_49`'s banked `418.09228516`) **plus the live `pollRadius: 1800` read-back** | **computation + read-back, not artifact** |
| **C3** frustum | **the artifact** — `bbox_valid: TRUE` on all 59 rows and at all 8 anchors. This is the clause that produced all 21 banked impostors, and it is the one the sharpened discriminator was adopted to separate | direct |
| **C4** occlusion | **what remains**, and it is independently predicted: `StaticMeshActor_86` (a cube at `(-900, 300, 135)`) blocks all 9 rays to `_100` on its own, with 0 rays blocked only by the floor | remainder + prediction |

### 13.2 The raw series — reported AS a series (Ruling 3a), and it is not close

⛔ **No A54 verdict. No shift search. TAU printed as a scale reference only, never applied.**

| event | claimed mean | flank mean | diff | ranges |
|---|---|---|---|---|
| ev0 | 0.960815 | 0.960616 | **+0.000199** | OVERLAP |
| ev1 | 0.962640 | 0.962467 | +0.000173 | OVERLAP |
| ev2 | 0.963332 | 0.963323 | +0.000009 | OVERLAP |
| ev3 | 0.963529 | 0.963539 | −0.000011 | OVERLAP |
| ev4 | 0.963712 | 0.963720 | −0.000008 | OVERLAP |
| ev5 | 0.963866 | 0.963827 | +0.000040 | OVERLAP |
| ev6 | 0.964010 | 0.963999 | +0.000011 | OVERLAP |
| ev7 | 0.963982 | 0.964008 | −0.000025 | disjoint — but **one** flank frame and 2.5e-5 of separation |

Largest excursion **2.0 × 10⁻⁴**, claimed and flank ranges **overlapping on 7 of 8 events**. The
control, on the same binary and the same pose, scores **0.1023 – 0.1116**. That is a factor of roughly
**500**, and the sign is not even consistent across events.

**So the target is labelled positive on 59 frames, carries a projected bbox, reports
`coverage_ratio > 0` and `manifested: true` — while changing essentially nothing inside that bbox when
it is hidden.** That is H4, observed.

### 13.3 What this DOES NOT license

⛔ **It is SUPPORTED, not CONFIRMED**, and the word was chosen before the run. The A54 leg of the
original signature is structurally unobtainable (G117), so this rests on the provenance divergence
plus a raw series, **not** on an oracle verdict.

⛔ **NO INCIDENCE CLAIM.** This is path (b) — targeted fire on an *already*-occluded actor. Whether H4
causes the client's complaint is a question about path (a) — selected while visible, becomes occluded
during the window — which `CB_GateLevel` **cannot produce**: every target is `STATIC` and the eye
position is invariant on 844/844 banked samples. The projector reads the current view and the actor's
bounds and has no memory of how the target became occluded, which is what licenses the mechanism claim
and nothing beyond it.

⛔ **n = 1 leg on the occluded side.** Eight events within one leg are not eight legs.

## 14. The shadow wording (Ruling 5), carried into the result

There is no candidate clean on both poll radius and shadow: `_100` throws shadow into visible pixels
(2 of 9 patch samples reachable from the eye), and the two shadow-clean candidates (`_11`, `_139`) are
poll-culled. **No better target was hunted and the level was not re-authored.** The claim narrows to
what H4 is actually about:

> **H4 concerns whether the PROJECTOR emits a positive label for a target that contributes no pixels
> WITHIN ITS OWN BBOX. `_100` contributes none inside its bbox and does contribute shadow outside it
> (7/9 patch samples occluded, 2/9 visible). A54 keys strictly inside the bbox (A35), so the shadow is
> outside the claim's scope BY CONSTRUCTION — and outside the client's complaint, which is a labelled
> box around nothing.**

The measured series is consistent with this and could not have been fitted to it: the in-bbox
excursion is ~2e-4, i.e. the removal of the shadow **outside** the bbox left the **inside** unmoved,
which is exactly what A35 predicts and why A35 exists.

## 15. P6's first observation — recorded, and P6 DOES NOT MOVE

**`coverage_pct: −1` alongside `coverage_ratio: 0.01160339` on all 8 events of the occluded leg.**

Predicted from source in pre-flight, and the prediction included a correction: journal 036 §3.5 and
the original H4 filing both said `coverage_pct == 0`. **It is −1** — the `FSelectionProvenance`
sentinel — and the run confirms it directly. The divergence is real, ships to the client in delivery
mode, and is now **measured** rather than read from source.

⛔ **No `annotation.json` field is added, removed, renamed or recomputed. P6 remains OPEN and
UNMOVED.** This is an observation of an open item, not a licence to close it.

## 16. Final state

| | |
|---|---|
| plugin production code | **ZERO lines touched, across both parts of this session** |
| tag | **none** |
| `feature/stencil-capture` | **untouched** |
| `B1` / `TAU` / `CALIB_BBOX` / `POSE_TOL_PX` / A54 definition | **untouched** |
| `P6` | **unmoved** |
| bank | `H4_H4_CTRL_49`, `H4_H4_CTRL_49_try1` (discarded, banked), `H4_H4_CTRL_49_try2`, `H4_H4_OCC_100`, `H4_H4_OCC_100_try1` |
| staged exe | `101AFEA4`, unchanged throughout |

**Forward:** H4 is **SUPPORTED as a mechanism, path (b)**, and routes to `feature/stencil-capture`,
whose premise — report actual pixel contribution before hiding — is its cure. **That branch is NOT
touched and NOT rebased.** Path (a), and therefore any incidence claim, remains unbuilt and unclaimed.
The `CALIB_BBOX` cluster (`B1`-NDC, the per-leg calibration bbox from Ruling 3b, `B2`) now blocks two
named items and its priority rises accordingly, **without being scheduled**. **G118** is filed and is
independent of all of it.

---
---

# PART THREE — the claim fixed in words, G112 corrected, and read-only recon for path (a)

**No run. No code. No cure built.** Documentation and source reading only.

## 17. RULING 1 — the claim, fixed in these words. Record verbatim; this is what travels.

> **H4 is SUPPORTED as a mechanism, path (b), n=1 leg / 8 events. The label path emits a positive
> label, a valid projected bbox, `coverage_ratio > 0` and `manifested:true` for a target that is fully
> occluded and changes essentially nothing inside its own bbox (~2e-4 against a control's ~0.10, factor
> ~500, ranges overlapping on 7 of 8 events with inconsistent sign). The C4 divergence — provenance
> `valid:false` + `0/0` WITH `bbox_valid` TRUE — occurred 8/8 and had never been observed in 780+
> banked records.**
>
> **NOT CONFIRMED: the A54 leg of the original signature is structurally unobtainable (G117), so this
> rests on the provenance divergence plus a raw series, not an oracle verdict.**
> **NO INCIDENCE CLAIM. This used TARGETED fire, which bypasses `IsUnoccluded`. The shipped auto-pool
> screens occlusion AT PICK TIME. Whether the shipped path is exposed is a SEPARATE, UNASKED
> QUESTION.**
> **Path (a) — visible at pick, occluded during the window — is unbuilt and cannot be produced in
> `CB_GateLevel` (all targets STATIC, eye invariant 844/844).**

**`H4` moves from NAMED, NOT ADOPTED → SUPPORTED (path b), MECHANISM ONLY.** It does **not** become a
phenomenon number. It is a named hypothesis with evidence.

## 18. RULING 2 — G112 is not incomplete, it is WRONG, and it has been amended in place

`G112`'s runnable detector reads `StackOBot\Config\DefaultGame.ini`. **A packaged build enforces the
COOKED copy baked into its pak at cook time**, so the detector **returns PASS on a build enforcing a
12-character literal present in repo history**. A guard that passes the unsafe case is worse than no
guard, because it retires the vigilance that would otherwise catch it.

Landed this turn, documentation only, **no re-cook**:

- **`G112` AMENDED IN PLACE** with a marked amendment block: the source ini is **not** the enforcing
  artifact, the cooked config is, and the binary's own `(from DefaultGame.ini …)` log line **means the
  cooked one**. The check is **DEMOTED to NECESSARY BUT NOT SUFFICIENT** — not deleted, because it
  still catches a source-side regression; it simply cannot certify a build. Cross-references G118/G119.
- **`PRE-DELIVERY-CHECKLIST.md` §1** gains a second, mandatory box carrying the **read-back** check —
  start the build, read `Control server token:` from its own log, assert not-a-placeholder and ≥ 32
  chars — with the explicit line **"A build is checked by what it ENFORCES, not by what its source
  says"**, and a stop: if source and enforced disagree, the build is **STALE and must be re-cooked
  before delivery**.
- **`G118` gains its CLOSURE SEQUENCING**, stated as the owner's call:
  `G118 CLOSURE = re-cook + re-stage + re-bank (G92 wipes Saved) + re-run the A44 hash scan`, to run
  **after the current measurement sequence and never inside one**, because **closing it RETIRES staged
  exe `101AFEA4` as the m25 measurement binary**. ⛔ **Any result still owed against `101AFEA4` must
  land first.**
- **`G119` — the generalisation**, and it is the **third instance of the same shape**:

  | # | gotcha | channel trusted | never verified |
  |---|---|---|---|
  | 1 | **G92** | "it compiled" | that the binary was **staged** |
  | 2 | **G113** | an **exit code** | that the code was **earned** |
  | 3 | **G118 / G112-amended** | the **source** config | what the artifact **enforces** |

  *For anything BAKED — cooked config, embedded resources, compiled-in defaults, generated headers,
  packaged assets — the source file is an INPUT, not the artifact. Read it back out of the running
  system.* The diagnostic question: **"what would I observe if the thing I edited never reached the
  thing under test?"** If the answer is *"exactly what I am observing now"*, it is not a check.

## 19. RECON FOR PATH (a) — read-only, from source. No run, no code, no design.

### 19.1 Is there ANY re-validation of the selected target after `LiveFire` begins?

**No. Not occlusion, not renderability, not screen coverage, not distance. Nothing.**

The per-frame path was walked end to end. Every function on it, and what it does:

| per-frame function | what it does to the live target | any visibility test? |
|---|---|---|
| `UAnomalyAutoInjectorSubsystem::Tick` (`:83`) | `PollInput()` if enabled; `AdvanceTime()` if running | **none** |
| `└ AdvanceTime` (`:160`) | `ServiceReverts(dt)`, then decrement `FireTimer` and `TryFireOnce()` on expiry | **none** |
| `└ ServiceReverts` (`:511-535`) | `SecondsRemaining -= dt`; on `<= 0` → `RevertAnomaly` + `LiveFires.RemoveAt` | **none — a pure countdown** |
| `UAnomalyInjectorSubsystem::Tick` (`:159`) | dispatches `Tick` to each active anomaly | **none** |
| `└ FAnomaly_Blinking::Tick` (`:60-84`) | iterates `Targets`, `SetActorHiddenInGame(bHiddenPhase)` | **none** |
| `└ FAnomaly_MissingObject` | **has no `Tick` at all** — hides once at `Apply`, restores at `Revert` | **none** |
| `UAnomalyCaptureSubsystem::AccumulateFrameEvents` (`:1558`) | accrues `AffectedFrames`, `CoverageSum`, `HiddenByIndex` | **`ProjectActorBoundsToScreenRect` only — no trace** |
| `AnomalyLabel::BuildFrameLabelRecord` (`:37`) | writes `bbox_valid` / `bbox_px` / `visible_positive` per frame | **`ProjectActorBoundsToScreenRect` only — no trace** |

**`FAnomaly_Blinking::Targets` is resolved ONCE, inside `Apply` (`:47-49`), and never re-resolved.**
The only per-frame check on it is `Weak.Get()`, which is **object validity, not visibility** — it asks
whether the actor still exists, not whether it can be seen.

**The single occlusion evaluation anywhere downstream of pick time is
`EvaluateSelectionProvenance`** (`AnomalyCaptureSubsystem.cpp:1599`), and three properties of it matter:

1. it sits **inside the `if (SessionIndex < Ev->AnchorIndex)` block**, so it runs **once per event, at
   the anchor frame only** — never again for the rest of the window;
2. **its return value is discarded** (`AnomalyViewport::EvaluateSelectionProvenance(World, FActor, Ev->Provenance);`
   — a bare call). Nothing gates on it. It **records**;
3. it is written by `WriteSelectionProvenance` **only when `!bDeliveryMode`** (`:1720`).

⇒ **In delivery mode — the mode the client receives — there is NO occlusion evaluation after pick
time at all, and no record that one was ever made.** Path (a) is therefore **not closed by any
existing re-check**, because no re-check exists.

### 19.2 What does the pick-time `IsUnoccluded` actually test?

`AnomalyViewport.cpp:109-144`. Reached by the auto-pool via
`GetVisibleRenderableActors` → `ClassifyRenderableVisibleLive` → `CollectRenderableVisibleUnion` →
`IsComponentRenderableVisibleInternal` → `IsUnoccluded`.

| property | value |
|---|---|
| ray count | **9** |
| origin | **`View.Origin`** — the CAMERA. (Distinct from `ResolvePollOrigin`, which is the **pawn**, and is used only for the radius cull) |
| target points | bounds **centre** + the **8 corners of `Component->Bounds`** — the **AABB**, not the mesh |
| channel | **`ECC_Visibility`** |
| complexity | **`bTraceComplex = false`** → **simple collision only** |
| ignored | **the owner actor only** |
| verdict rule | **returns `true` on the FIRST clear ray** |

Companion pick-time guards on the same path: the **poll-radius cull** (`GPollRadius`, default
**1800 cm**, measured **pawn → bounds origin − sphere radius**) and the **screen-coverage floor**
(`GMinScreenCoveragePct`, default **6 %**, on the projected union rect).

**What could pass this guard while contributing no pixels — read from the code, not speculated:**

- **First-clear-ray semantics.** **1 of 9 clear ⇒ "unoccluded".** A target with 8 of 9 rays blocked is
  accepted. Contrast the *evidence* standard used for H4's target, which required **9 of 9 blocked**.
  **The guard's bar for "visible" and this project's bar for "occluded" are not complements — there is
  a wide band between them that is neither.**
- **AABB corners, not the mesh.** For a **sphere, cylinder or cone** the AABB corners lie **outside the
  rendered surface**. A ray can reach a corner of empty space while the mesh itself is entirely hidden.
  *(`CB_GateLevel` cycles cube/sphere/cylinder/cone, so 3 of 4 target shapes have this property.)*
- **`bTraceComplex = false`.** Occlusion is judged against **simple collision**. An occluder whose
  simple collision is smaller than its render mesh under-occludes; a large visual occluder with
  **`NoCollision`** or one that ignores `ECC_Visibility` — decals, foliage, translucent panels,
  purely-decorative meshes — **does not occlude at all**.
- **No pixel test anywhere.** Passing means *a ray reached one AABB corner*, never *pixels were drawn*.
  Materials, opacity, `bRenderInMainPass`, LOD/HLOD substitution and distance culling are all invisible
  to it.
- **It is a single sample in time**, taken with the view at pick time, and §19.1 establishes nothing
  re-takes it.

⚠ **Stated as source reading, NOT as measurement.** None of the above has been observed producing an
instance; H4's own lesson is that a mechanism read from source is not a mechanism seen.

### 19.3 Candidate environments for a path (a) test — NAMED ONLY, not evaluated

Every `.umap` in the project:

| level | size | note |
|---|---|---|
| `/Game/StackOBot/Maps/MainWorld` | 21 KB | the gameplay level |
| `/Game/StackOBot/Maps/Structures/Struct_001` | 301 KB | under `Maps/Structures/` |
| `/Game/StackOBot/Maps/Structures/Struct_002` | 9 KB | " |
| `/Game/StackOBot/Maps/Structures/Struct_003` | 15 KB | " |
| `/Game/StackOBot/Maps/Structures/Struct_004` | 1.66 MB | " |
| `/Game/StackOBot/UI/MainMenu/MainMenu` | 174 KB | the packaged **boot** map; a full 3D scene |
| `/Game/CaptureBenchGate/CB_GateLevel` | 1.10 MB | the frozen instrument; **all STATIC, eye invariant** |

🚨 **G87 FLAGGED, AND IT IS STRONGER THAN "CHECK THE NAME".** `GameDefaultMap` is
`/Game/StackOBot/UI/MainMenu/MainMenu`, MainMenu is a full 3D scene visually indistinguishable from
gameplay, **and the redirect is ACTIVE, not a startup race**: G87 records that
`StackOBot.exe /Game/StackOBot/Maps/MainWorld` loads MainWorld and then **immediately loads MainMenu**,
and that a deferred `OpenLevel` travels to MainWorld and **bounces back**. ⇒ **MainWorld is not
straightforwardly reachable in a packaged build**, and any path (a) design must confront that before
anything else. `CB_GateLevel` is unaffected only because nothing in it redirects.

⛔ **Not evaluated, per the brief.** Two things are recorded as *observations to verify*, not findings:
MainWorld's 21 KB size is consistent with a shell that streams the `Structures/` levels (a raw ASCII
scan of the `.umap` found no `Struct_00*` or `LevelStreaming*` strings, but the asset is compressed so
that scan is **not** evidence either way); and whether any of these contains motion **that occurs
without player input** is unknown — an unattended packaged run has no input, so a path (a) test needs a
moving occluder, a moving target, or a driven camera, and which of these exists in any of these levels
has **not** been checked.

## 20. State after PART THREE

**No run. No plugin production code. No tag. `feature/stencil-capture` untouched and not rebased.**
The cure is not built and will not be built before the incidence question is answered — the D-B lesson:
the SVE migration would not have cured P3, and building it first would have shipped a migration while
the poisoning continued.

---
---

# PART FOUR — the environment scout. Read-only. No run, no code, no re-cook.

## 21. RULING 1 — path (a)'s status, recorded verbatim

> **PATH (a) IS NOT CLOSED BY ANY EXISTING RE-CHECK, BECAUSE NO RE-CHECK EXISTS. In DELIVERY MODE — the
> client's shipping configuration — there is no occlusion evaluation after pick time at all, and no
> record that one was ever made. The system's only per-frame test on the target list is `Weak.Get()`,
> which asks whether the actor still EXISTS, not whether it can be SEEN.**

⛔ **`feature/stencil-capture` is NOT touched, NOT rebased, NOT scoped, NOT estimated.** On the record
for a cold reader: **D-B measured that the SVE migration would NOT have cured P3**, and building it
first would have shipped a migration while the poisoning continued. We hold a mechanism observed in a
lab condition (path b) and a shipped path with no re-validation (path a), and **ZERO observed instances
of path (a)**. Between *"this can happen"* and *"this reaches a delivered dataset"* sits exactly one
measurement, and it has not been taken.

## 22. RULING 3 — the PREDICTION SET, pre-declared before any path (a) instrument exists

Recon 2 was **source reading, not measurement**. Banked as numbered predictions. **Numbers never
reused.** Any path (a) design must state **which of these it exercises and which it leaves untested.**

| # | prediction |
|---|---|
| **P-a1** | **first-clear-ray** — a target with **8 of 9 rays blocked passes pick time**. `IsUnoccluded` returns `true` on the first clear ray. |
| **P-a2** | **AABB corners** — for **sphere / cylinder / cone** a clear ray can reach a corner of empty space while the mesh is fully hidden. The traced points are `Component->Bounds` corners, not the mesh. |
| **P-a3** | **`bTraceComplex = false`** — an occluder with **no collision**, or one ignoring **`ECC_Visibility`**, does not occlude at all for pick-time purposes. |
| **P-a4** | **no pixel test** — pick-time success **never implies pixels were drawn**. Materials, opacity, `bRenderInMainPass`, LOD/HLOD substitution and distance culling are invisible to it. |
| **P-a5** | **single sample in time** — nothing re-takes it (§19.1), so **pick-time truth decays** across the window. |

**None has been observed.** H4's own lesson is that a mechanism read from source is not a mechanism seen.

## 23. S-1 — THE MAINWORLD REDIRECT. 🚨 G87's MECHANISM IS WRONG. → **G120**

**The redirect does not exist. `MainWorld` is not in any staged build.**

Container index of every staged build, read directly from `StackOBot-Windows.utoc` (UTF-16 strings):

| build | exe | cooked maps |
|---|---|---|
| `Builds\BenchGate` (m25, the measurement binary) | `101AFEA4` | `CB_GateLevel`, `Entry`, `MainMenu` |
| `Builds\MidRepro` | `3814E080` | `Entry`, `MainMenu` |
| `Builds\Windows` | `B3A49D82` | `Entry`, `MainMenu` |

**`MainWorld.umap` is in NONE. Neither is any `Structures/Struct_00*`.** `Builds\Windows`' exe is dated
`2026-08-06`, the same day G87 was written — almost certainly the build it measured.

⇒ `LoadMap MainWorld` **fails because the map is not in the pak**, and the engine falls back to
`GameDefaultMap` = MainMenu. **Engine fallback, not an in-game redirect.**

**What performs a redirect, from an exhaustive source search — the answer is "nothing, in that
direction":**

| asset | what it actually contains |
|---|---|
| **`HUD_MainMenu`** | the **only** `OpenLevel` in the framework — `OpenLevel` + `MainWorld` + `LevelName` + `LoadingScreen` + `CreateWidget`. It travels **MainMenu → MainWorld** (the Play button). |
| `GI_StackOBot` | a **save-game manager** (`InitSaveGame`, `GetCurrentLevelName`, orb persistence). Its `LevelName` is a **save-slot key**. **No `OpenLevel`.** |
| `GM_InGame` | `ReceiveBeginPlay` only |
| `MainWorld.umap` level BP (`MainWorld_C`) | **no** `OpenLevel` / `MainMenu` / travel strings |

**Route to MainWorld in a packaged build:** a command-line map arg, a cvar and a post-boot console
command **all fail identically for the same reason — the asset is not in the container.** ⛔ **The only
route is A RE-COOK that includes the map.** Stated plainly because it is a real cost, and it collides
with `G118`'s closure sequencing: **a re-cook retires `101AFEA4`.**
✅ **But the two collapse: the re-cook `G118` closure needs and the re-cook MainWorld needs are THE SAME
OPERATION.** That is a scheduling gain, not an extra cost.

⛔ **NOT re-cooked. `GameDefaultMap` NOT changed. No packaged run.** ⚠ **The mechanism correction is
NOT re-measured** — it rests on a direct read of the artifact's map index plus the source search. The
one-command settlement is a packaged launch at MainWorld with the log read for the missing-map browse
error. **Corrected but unconfirmed.**

## 24. S-2 — MOTION INVENTORY. The honest answer is the one the brief anticipated.

**NO LEVEL PRESENT IN ANY STAGED BUILD HAS UNATTENDED MOTION CAPABLE OF CHANGING OCCLUSION STATE.**

| level | in a build? | unattended motion | evidence |
|---|---|---|---|
| **`CB_GateLevel`** | ✅ BenchGate | **NONE** | authored `STATIC` throughout by `make_gate_level.py`; no Blueprints; eye invariant on **844/844** banked samples |
| **`MainMenu`** | ✅ all three | **effectively none** | contents: 1 `SkeletalMeshActor` + `ABP_Bot`, 1 `Landscape`, 2 `StaticMeshActor`, `BP_Cable`, `BP_Spline`, 3 `CameraActor`, lights, post-process. **No `LevelSequence`, no Matinee, no auto-play, no movement component.** The only motion is the Bot's **skeletal animation** — mesh deformation inside one component, **not an actor translating**, so it cannot bring an occluder between camera and target. The 3 `CameraActor`s have **nothing to drive them.** |
| `Entry` | ✅ all three | engine template map; not a candidate |
| **`MainWorld`** | ❌ **NOT COOKED** | **rich** — see below | |
| `Struct_001..004` | ❌ **NOT COOKED** | **none found** | scanned `Struct_002` (814 external actor files) and `Struct_003` (389): **zero Blueprint actor instances**. They read as geometry sets. |

### 24.1 What MainWorld *would* provide — the classes are present in its external-actor set

MainWorld is a **World Partition** level with **419 external actor files** under
`Content/__ExternalActors__/StackOBot/Maps/MainWorld/` (which is why the `.umap` is only 21 KB — S-4).
Blueprint classes referenced there include `BP_EnergyOrb`, `BP_PressurePlate`, `BP_Button`,
`BP_Elevator`, `BP_Stomper`, `BP_Crate`, `BP_MovingPlatform`, `BP_Lamp`, `BP_SpawnPad`, `BP_Spline`,
`BP_Fan`, `BP_Cable`, `BP_Door`, `BP_Ramp`.

**Which move without input — and the assets say so in their own words.** The Blueprints carry developer
comments in the name table; these are direct quotes, not inference:

| asset | unattended? | the asset's own words / machinery |
|---|---|---|
| **`BP_Stomper`** | ✅ **yes, when untriggered** | *"The stomper moves up and down driven by a curve in the timeline. **When no trigger is referenced it move constantly.** An assigned trigger, such as the pressure plate, activates and deactivates the stomper."* Also *"If there is an assigned trigger … **If not, just start the stomper**"*. `ReceiveBeginPlay` + `TimelineComponent` + `bLoop`, **no overlap gate on the actor itself** |
| **`BP_MovingPlatform`** | ✅ **yes, when untriggered** | `InterpToMovementComponent`, **`EInterpToBehaviourType::PingPong`**, `Duration`, `DurationBetweenEnds`, `AddControlPointPosition`. *"as with the other objects it can be (de)activated by a trigger"* ⇒ trigger **optional**. Drives a Control Rig (`CR_MovingPlatform`) with a verlet simulation |
| **`BP_Fan`** | ✅ **yes** | `K2_AddLocalRotation` + `SetTimerDelegate` + `bLooping` + `Activate`/`Deactivate`. Its `FanArea` overlap exists to **push the Bot**, not to start the fan |
| **`BP_EnergyOrb`** | ✅ **yes** | *"Add a bit of rotation per tick"*, `bAutoActivate`, `ReceiveTick` — rotates unconditionally. **Small**, so a weak occluder |
| `BP_Elevator`, `BP_Button`, `BP_PressurePlate`, `BP_SpawnPad` | ❌ | `OnComponentBeginOverlap` + `K2Node_ComponentBoundEvent` ⇒ **player-gated** |
| `BP_Door`, `BP_Ramp` | ❓ | `TimelineComponent` + `PlayFromStart` + `ReceiveBeginPlay`, **no overlap on the actor**, but conventionally driven by a Button/PressurePlate reference. **Not established** |
| `BP_Lamp`, `BP_Cable`, `BP_Crate`, `CR_MovingPlatform` | ❌ | no motion machinery found |

⚠ **Two limits on the above, stated rather than buried.** (1) **Per-instance `Trigger` assignment is NOT
established** — `BP_Stomper` and `BP_MovingPlatform` move constantly *only when their Trigger reference
is empty*, and whether any given placed instance leaves it empty is a per-actor property this scout did
not read. (2) **Exact instance counts are NOT established** — the tally counted string occurrences
across external-actor files, which over-counts.

## 25. S-3 — THE OCCLUDER QUESTION

| shape | available **today** (cooked levels) | available **in MainWorld**, if it were cooked |
|---|---|---|
| **(i)** moving occluder crosses a static target | ❌ **none** | ✅ **`BP_MovingPlatform`** (PingPong translation, large) crossing any static `Environment/Modular` or `Props` mesh. **`BP_Stomper`** likewise on the vertical axis |
| **(ii)** moving target passes behind a static occluder | ❌ **none** | ✅ same actors as the **target** instead of the occluder — `BP_MovingPlatform` and `BP_Stomper` are themselves `StaticMeshComponent` carriers and so are **selectable** by the auto-pool |
| **(iii)** moving camera brings an occluder between itself and the target | ❌ **none** — CB_GateLevel's eye is invariant 844/844; MainMenu's 3 `CameraActor`s have nothing driving them | ❓ **not established** — would need the player pawn (no input in an unattended run) or a driven camera that does not exist |

**`BP_Fan` and `BP_EnergyOrb` rotate in place**, so they change silhouette but do not translate; they
are weak candidates for (i) and (ii) and no candidate at all for (iii).

## 26. S-4 — THE STREAMING QUESTION, SETTLED

**MainWorld is a WORLD PARTITION level with ONE-FILE-PER-ACTOR (OFPA), not a classic streaming shell.**
Settled from the asset, superseding PART THREE's flagged guess:

- `MainWorld.umap` contains `WorldPartition`, `WorldPartitionEditorSpatialHash`,
  `WorldPartitionRuntimeSpatialHash`, and a reference into
  `/Game/__ExternalActors__/StackOBot/Maps/MainWorld/…`.
- Its actors live in **419 external `.uasset` files**, which is why the `.umap` is 21 KB. **The size was
  the right clue and the wrong inference** — PART THREE guessed "streams the `Structures/` levels".
- `Struct_001..004` each reference `/Game/StackOBot/Maps/MainWorld` and have their own
  `__ExternalActors__` trees (`Struct_002`: 814 files, `Struct_003`: 389). They are **partitioned
  companions of MainWorld**, not sublevels it streams in the classic sense.

**Relevance to path (a), which is why the brief asked:** World Partition streams by **spatial distance
from the streaming source**, so geometry genuinely does appear and disappear during play — **streamed-in
geometry is itself a candidate occluder**, and a capture run in MainWorld must survive that load timing.
⛔ **Not evaluated** — cell size, runtime grid setup, and whether streaming is even active in a packaged
build were not read. Named because it is now a real design input, not an unknown.

## 27. Scout limitations, stated

- ⛔ **The MCP bridge was UNAVAILABLE** — `Connection refused (127.0.0.1:12029)`, no editor listening.
  **A59 is therefore satisfied vacuously: no measurement was taken over the bridge, so none is
  attributed to this project, and G97 has nothing to catch.** Everything above is **offline asset and
  container reading.**
- The Blueprint findings rest on **name-table strings and the developer comments embedded in them**.
  The comments are direct quotes and are strong; the machinery lists are **indicative of graph
  contents, not a graph read**. An editor session would settle per-instance `Trigger` assignment and
  exact instance counts, and neither is established here.
- ⛔ **No packaged run, no re-cook, no `GameDefaultMap` change, no production code, `CB_GateLevel`
  untouched (G99).**

## 28. State after PART FOUR

**No run. No plugin production code. No tag. `feature/stencil-capture` untouched and not rebased.**
Path (a) is **structurally open** and **environmentally blocked**: the only level with unattended
movers is **not in any build**, and putting it in one is the same re-cook that closes `G118` and retires
`101AFEA4`. ⛔ **The path (a) design is chat-side and is not written here.**

---
---

# PART FIVE — the pre-cook gate. G-1 PASSES. No cook performed.

**Owner ruling: Option A, the rebuild happens, carrying BOTH the MainWorld cook and G118's token
closure. THIS TURN IS THE GATE, NOT THE REBUILD.** Nothing was cooked, staged, or re-configured.

## 29. G-1 — THE TRIGGER QUESTION. ✅ **PASS.**

**Why this gate existed, restated:** the whole case for cooking MainWorld rested on a **developer
comment** and on machinery lists explicitly labelled *indicative of graph contents, not a graph read*.
Whether any **placed** instance leaves its `Trigger` empty was unestablished — and *"MainWorld gives us
unattended motion"* was therefore exactly the thing **G120** forbids: a scope decision resting on an
unverified mechanism.

**Method — offline, because the bridge was refused again.** `unreal-mcpython` returned
`Connection refused (127.0.0.1:12029)` on a fresh attempt, so **no editor measurement was taken and
A59 is satisfied vacuously — nothing is attributed, and G97 has nothing to catch.** Settled instead
from the World Partition **one-file-per-actor** tree: every placed actor is its own `.uasset`, and
**UE serialises only non-default property values**, so an instance that leaves `Trigger` at `None`
**does not serialise the property at all**.

⚠ **The first classification was wrong and was corrected before use.** Keying on *"which BP classes
does this file mention"* conflated *"is a Stomper"* with *"mentions a Stomper"* — two of the rows it
called Stompers were **PressurePlate** files. Re-keyed on the actor's **own** path
(`PersistentLevel.<Class>_C_UAID_…`), which names the actor the file *is*. All figures below are from
the corrected pass.

**Census — MainWorld's 419 external actor files, by own class:** `BP_EnergyOrb` 21 · `BP_PressurePlate`
11 · `BP_Spline` 8 · `BP_Elevator` 7 · **`BP_Stomper` 7** · `BP_Button` 7 · `BP_Crate` 7 ·
**`BP_MovingPlatform` 6** · `BP_Lamp` 6 · `BP_SpawnPad` 5 · **`BP_Fan` 4** · `BP_Door` 3 ·
`BPP_Struct_001-004` 1 each · 318 plain (non-Blueprint) actors.

### 29.1 Per-instance Trigger status

| class | placed | **Trigger EMPTY** | Trigger BOUND | motion |
|---|---|---|---|---|
| **`BP_Stomper`** | 7 | **5** | 2 (both → `BP_PressurePlate`) | **translating**, vertical |
| **`BP_MovingPlatform`** | 6 | **4** | 2 (both → `BP_PressurePlate`) | **translating**, `PingPong` |
| **`BP_Fan`** | 4 | **2** | 2 (both → `BP_PressurePlate`) | rotating in place |
| **`BP_EnergyOrb`** | 21 | **21** | 0 — *the class has no Trigger variable at all* | rotating in place, small |

**⇒ NINE TRANSLATING MOVERS WITH AN EMPTY TRIGGER. THE GATE PASSES WITH WIDE MARGIN.**

### 29.2 The discriminator was verified to fire in BOTH directions (G96)

A discriminator that has only ever returned one answer is not a discriminator. **16 files across
MainWorld carry a serialised `Trigger` property** — 11 `BP_PressurePlate`, 2 `BP_Stomper`,
2 `BP_MovingPlatform`, 1 plain actor. So a `False` reading is **a real reading, not blindness**. The
two independent signals — *property serialised* and *references a trigger class* — **agree on every
Stomper and every MovingPlatform**, with no disagreements.

⚠ **One correction, caught by that cross-check.** `BP_Fan` does **not** have a scalar `Trigger`; it has
**`Triggers` (an array)** plus `TriggersActive` / `TriggersNeeded`. Keyed on `Trigger` the Fans read
4-of-4 empty, which **contradicted** their reference signal (2 of 4 reference a PressurePlate). Re-keyed
on `Triggers`, the two signals agree: **2 of 4 bound, 2 of 4 empty.** PART FOUR's *"BP_Fan —
unattended, the overlap only pushes the Bot"* was **too strong**: the overlap is indeed not the gate,
but a `Triggers` array is, and it is populated on half the instances. Corrected here.

### 29.3 What is still NOT established

- **Which** unbound Stomper/MovingPlatform is positioned to occlude **what** — placement geometry was
  not analysed. Nine movers exist; whether any of them crosses a line of sight to a selectable target
  is a **path (a) design question**, not a gate question.
- The **`BP_Door` / `BP_Ramp`** ambiguity from PART FOUR is unchanged and was not needed.
- Everything remains **asset reading**, not a running-game observation.

## 30. G-2 — THE `101AFEA4` DEBT SWEEP

**Legs that ran on `101AFEA4`** (banked sessions newer than the exe's `2026-08-19 12:32:39`):
`S3B_S44_deliveryON_try2`, `S3B_S44_deliveryON_try3` (S4-4 delivery orthogonality) and all five H4
dirs (`H4_H4_CTRL_49` + `_try1` discard + `_try2`, `H4_H4_OCC_100` + `_try1`).

**Everything else in the m25 record ran on a DIFFERENT binary that is already preserved:** the ten
S4-1 matrix legs ran on `.s4-0-baseline` `834BB30A`, and the S4-2 flip gates on `.s4-2-baseline`
`259BF64F`. Those baselines sit beside the exe.

### 30.1 THE LIST — what is still owed against `101AFEA4`

| # | item | owed? |
|---|---|---|
| 1 | **m25 gate / limit / scope claims** | ❌ **No.** All reproduced and closed; *"Nothing in S4 is outstanding."* The matrix legs are on `834BB30A`, not this binary. |
| 2 | **H4** | ❌ **No measurement owed.** The mechanism claim is complete and its limits (SUPPORTED-not-CONFIRMED, no incidence claim, n=1 leg) are recorded as limits, not as pending work. A future second occluded leg is a **replication**, and a replication on a *new* binary is stronger evidence, not weaker. |
| 3 | **Path (a)** | ❌ **No — and it CANNOT be owed against this binary**, because it requires MainWorld, which this binary does not contain. It is owed against the *new* one. |
| 4 | **`P5`/`P7` blend-ladder** | ❌ **No.** Not started. Its comparison set is **banked legs**, and every leg carries its own in-run control. It can run on the new binary with a fresh baseline. |
| 5 | **`B2`, `B1`-NDC, the per-leg calibration bbox (G117)** | ❌ **No.** All are **analysis-side definition changes gated against BANKED data**, and banked data is unaffected by a re-cook. |
| 6 | **`A11`** | ❌ **No.** Open with no design and no lever; not a measurement on this binary. |
| 7 | **`P6`** | ❌ **No.** Its first observation is already recorded (from an H4 leg). Implementation is production work. |
| 8 | **Client-band thinness · journal 031 render half · A17/A19 audit** | ❌ **No.** Paper and banked-data work. |

✅ **THE LIST IS EMPTY. Nothing is owed against `101AFEA4`.** Stated explicitly, as the ruling
required: this is the answer, not an absence of one. **The binary can be retired**, provided it is
preserved as `.m25-baseline` and the caveats below are honoured.

### 30.2 🚨 TWO RISKS THE SWEEP SURFACED — both concern the COOK, not the debts

1. **THE BASELINE CHAIN LIVES INSIDE THE STAGED TREE AND A RE-STAGE CAN DESTROY IT.**
   `StackOBot.exe.m23-baseline` `85A39CFB`, `.m24-baseline` `3BA854FB`, `.s4-0-baseline` `834BB30A`,
   `.s4-2-baseline` `259BF64F` all sit in
   `Builds\BenchGate\Windows\StackOBot\Binaries\Win64\` — **the exact directory a re-stage writes
   into.** G92 already records that staging **wipes `Saved`**; nothing records what it does to
   `Binaries`. ⛔ **COPY ALL FOUR BASELINES OUT OF THE STAGED TREE BEFORE THE COOK**, alongside
   `101AFEA4` as the new `.m25-baseline`. Losing them would orphan every hash reference in the status
   block and in four journals.
2. **`CB_GateLevel` MUST BE IN THE COOK'S MAP SET, AND THE MAP SET MUST BE DECIDED BEFORE THE COOK
   RUNS.** Every m25 certification is expressed in `CB_GateLevel`; losing it orphans the whole matrix.
   **A cook that silently omitted a map is precisely what created this situation** — MainWorld's
   absence was never noticed because nothing checks the cooked map set. ⇒ **After the cook, read the
   new `StackOBot-Windows.utoc` map index back** and assert `CB_GateLevel`, `MainMenu` **and**
   `MainWorld` are all present, before any leg runs. That check is the same shape as `G119`'s rule and
   costs one command.

## 31. G-3 — ✅ RUN. THE G87 CORRECTION IS NOW **CONFIRMED**, NOT MERELY CORRECTED.

One launch of the **unchanged** `101AFEA4` at MainWorld, one log read. **No cook, no re-stage; the
binary was only run.** Cost: ~25 seconds.

```
LogNet:       Browse:  /Game/StackOBot/Maps/MainWorld?Name=Player
LogLoad:      LoadMap: /Game/StackOBot/Maps/MainWorld?Name=Player
LogStreaming: Warning: LoadPackage: SkipPackage: /Game/StackOBot/Maps/MainWorld
              - THE PACKAGE TO LOAD DOES NOT EXIST ON DISK OR IN THE LOADER
LogLoad:      Error:   Failed to enter /Game/StackOBot/Maps/MainWorld:
                       Failed to load package '/Game/StackOBot/Maps/MainWorld'.
LogExit:      Exiting.
```

**"The package to load does not exist on disk or in the loader."** The container read (PART FOUR §23)
predicted exactly this, and the engine states it in its own words. **G87's "active redirect" is
confirmed to be nothing of the kind.** Log banked at `_bench_sessions_bank/G3_MAINWORLD_BROWSE_PROBE/`
with a README naming the command and the decisive lines.

⚠ **One difference NOT claimed as a further correction:** under `-unattended` this build **exits**
rather than falling back to MainMenu, where G87 recorded a fallback. That is plausibly an
`-unattended` artifact and **was not isolated**. The **cause** is what G-3 settles, and the cause is
the missing package either way. *(Recording the difference rather than quietly absorbing it — the same
standard G120 exists to enforce.)*

## 32. G120's rule ELEVATED into the standing invariants

Moved out of `docs/gotchas.md` alone and into **`CLAUDE.md` → Invariants (do not violate)**, at the
top of the list:

> **An observation and its explanation are SEPARATE CLAIMS, recorded separately. Never derive a SCOPE
> decision — "X is impossible", "that approach is dead", "do not try Y" — from an unverified
> mechanism.**

with the reason a cold reader needs: a false positive (G116) and a false null (G114) are eventually
caught because someone re-runs the measurement; **a false FORECLOSURE is never re-run, by definition,
because foreclosing is the act of telling everyone not to.**

## 33. State after PART FIVE

| | |
|---|---|
| cook / re-stage / `GameDefaultMap` | ⛔ **NOT performed, NOT changed** |
| plugin production code | **ZERO lines, across all five parts** |
| tag | **none** |
| `feature/stencil-capture` | **untouched** |
| staged exe | **`101AFEA4`, unchanged** — G-3 only *ran* it |
| `CB_GateLevel` | **untouched** (G99) |
| bank | +1 dir: `G3_MAINWORLD_BROWSE_PROBE` (a log and a README; not a capture session) |

**G-1 PASS · G-2 LIST EMPTY · G-3 CONFIRMED.** The cook is justified and unblocked. ⛔ **The cook brief
is the owner's and is not written here. The path (a) design is chat-side and is not written here.**

---
---

# PART SIX — the cook. Both preconditions met, all gates pass, smoke green.

**Owner ruling: the cook is approved, carrying the MainWorld cook AND `G118`'s token closure.**
Executed. **NO production code changed. No tag.**

## 34. Precondition 1 — the baseline chain, rescued and verified

Copied out of the staged tree to **`D:\IntrusiveAnomalies\_binary_baselines\`** — a **sibling of the
bank, deliberately outside `Builds\`**, where a stage cannot reach it. **Verified BY HASH AT THE NEW
LOCATION before anything else proceeded** (A62 — a copy that ran is not a copy that landed):

| file at the new location | hash | expected | |
|---|---|---|---|
| `StackOBot.exe.m25-baseline` *(was `StackOBot.exe`)* | `101AFEA4` | `101AFEA4` | ✅ |
| `StackOBot.exe.s4-2-baseline` | `259BF64F` | `259BF64F` | ✅ |
| `StackOBot.exe.s4-0-baseline` | `834BB30A` | `834BB30A` | ✅ |
| `StackOBot.exe.m24-baseline` | `3BA854FB` | `3BA854FB` | ✅ |
| `StackOBot.exe.m23-baseline` | `85A39CFB` | `85A39CFB` | ✅ |

**5 of 5 verified, 0 failures.** `README.md` written beside them naming each hash, what it is, and
which journal or status line cites it.

### 34.1 The re-bank sweep found more than the ruling anticipated

**Bank 91 → 100 dirs, 10.13 GB.** Two categories were unbanked and would have been inside the blast
radius:

- **`Saved\M23B`** (108.6 MB) — the only `Saved` dir with no bank counterpart.
- **Eight leg outputs beside the exe** (G101 — output lands next to the binary, not under `Saved`),
  347.7 MB, matched **by session id** rather than by directory name (119 banked session ids checked):
  `S43_defaultA`, `S43_defaultB`, `S43_backbuffer`, `S44_deliveryON` — ⚠ **these four are the RAW
  EVIDENCE behind m25's S4-3 and S4-4 claims** — plus `H4_WSECHO`, `PRE`, `D3D12`, `S3A2_OFF`.

Every other exe-side leg dir already had its accepted session in the bank.

## 35. Precondition 2 — the map set, declared BEFORE and read back AFTER

**Declared in writing before the cook ran**, with `CB_GateLevel` marked non-negotiable. Cook command,
from `G91`'s recipe (**not** reconstructed):

```
RunUAT BuildCookRun -project=StackOBot.uproject -platform=Win64 -clientconfig=Development
  -cook -stage -pak -archive -archivedirectory=Builds\BenchGate -build -utf8output -nocompileeditor
  -map="/Game/CaptureBenchGate/CB_GateLevel+/Game/StackOBot/UI/MainMenu/MainMenu+/Game/StackOBot/Maps/MainWorld"
```

🚨 **A THIRD INDEPENDENT CONFIRMATION OF S-1, found in our own docs.** `G91`'s documented cook command
carries `-map="…CB_GateLevel+…MainMenu"` and **has never contained MainWorld**. The cause of the whole
episode was written down, in this repo, since 2026-08-06 — **on the same page as the sentence claiming
the title actively redirects MainWorld away.**

**Result: `BUILD SUCCESSFUL`, `ExitCode=0`, 2 m 27 s.** No production code changed.

### 35.1 THE GATE — ✅ PASS

`CaptureBench/tools/verify_cooked_maps.ps1` (new, committed) reads the **`.utoc` container index** —
the artifact — rather than trusting the `-map=` argument, which is an **input**. G119 applied to the
cook itself.

```
CB_GateLevel     PRESENT
MainMenu         PRESENT
MainWorld        PRESENT        <- the point of the cook
Entry            PRESENT
PASS - every required map is present in the cooked container.
```

⚠ **The tool scans BOTH encodings and reports which answered.** This container answered in **ASCII**
(`hits ascii: 4, utf16: 0`); the pre-cook containers answered in UTF-16. **The encoding is not stable
across containers**, so a single-encoding scan would have returned a clean-looking "no maps cooked" —
the exact false-negative shape G103 records for the A44 binary scan. The tool exits **2** on a
zero-in-both-encodings result rather than reporting a clean absence.

⚠ **The tool's first draft failed to parse.** Windows PowerShell 5.1 reads a `.ps1` as **ANSI unless
the file has a BOM**, so the em-dashes in it were a **parse error**, not a runtime one. Rewritten
7-bit ASCII, with that stated in its header. *(The existing harness scripts happen to carry BOMs, which
is why this had never bitten.)*

## 36. G118 CLOSED — the token, read back from the running build

```
source Config\DefaultGame.ini : 5b544cee3d97... (len 64)
ENFORCED by the staged binary : 5b544cee3d9780331b1fe1bed206bb4aaf74dbd8ebfb215b3f04b37f878d6c61 (len 64)
```

**No mismatch. No placeholder.** The build that enforced `TESTVALUE123` is gone. Read back from the
running build's own `Control server token:` log line per the amended `PRE-DELIVERY-CHECKLIST` §1 —
**not** from the source ini, which is the artifact that was never enforcing anything.

Same probe, unchanged on the new build: `viewportScoping: False`, `pollRadius: 1800`,
`minScreenCoverage: 6`, and the behavioural echo `blinking: matched 1 actor(s) for
'=StaticMeshActor_100'`.

## 37. A44 scan of the staged artifact, both encodings

| symbol | ascii | utf16 |
|---|---|---|
| `IsHideTypeAnomaly` | 0 | **1** |
| `selection_provenance` | 0 | **2** |
| `capture_path` | 0 | **1** |
| `IAI.Capture.SVE` | 0 | **10** |
| `AnomalyViewportOcclusion` | 0 | **1** |
| `AnomalyViewportProvenance` | 0 | **1** |
| `Control server token` | 0 | **1** |

**Non-zero throughout** — the scan is sound, not suspect tooling. Exactly the ASCII-0 / UTF-16-N shape
G103 predicts.

## 38. 🚨 WHAT THE STAGE ACTUALLY DID — and the finding it produced

**Nothing was wiped.** Leg dirs beside the exe **56 → 56**; `.baseline` exes **4 → 4, all present**;
`Saved\` **23 → 23 dirs, including `M23B`**. ⚠ **This is ONE cook with ONE flag set (no `-clean`,
archiving into an existing tree). The 2026-08-16 wipe is NOT retracted** — what is now known is that
the archive step is **not unconditionally destructive**, and *which* factor decides is **not
established**. **The precaution stays.** G92 annotated in place.

### 38.1 **G121 — the exe hash did NOT change, so it does not identify the build**

| | before | after |
|---|---|---|
| **exe SHA-256** | **`101AFEA4`** | **`101AFEA4`** — *identical* |
| exe mtime | 12:32:39 | **12:32:39** — *identical* |
| cooked maps | 3 | **4 (+ MainWorld)** |
| enforced token | **`TESTVALUE123`** | **64-char rotated** |
| `.utoc` | 194,996 B | **268,036 B** |
| `.ucas` | 125,071,408 B | **284,469,920 B** |

**Same exe hash. Different build, different maps, different secret.** No code changed, so nothing was
compiled and the archived exe kept its compile time.

⛔ **This reaches backwards: every A44 hash reference in this project identifies only HALF the
artifact.** *"Staged exe `101AFEA4` = m25"* was true, is still true, and is **no longer sufficient** —
**two builds now answer to that hash.** ⚠ **And the reverse is the dangerous case:** a **code-only
hot-swap (G103)** moves the exe hash and leaves the pak; the two halves move **independently**, so a
same-hash comparison is not a same-build comparison **in either direction**.

**RULE: build identity = exe hash + pak identity.** For the new build:

```
exe                    101AFEA4
StackOBot-Windows.utoc 939B9C9B    268,036 bytes   2026-08-19 17:00:27   (4 maps)
StackOBot-Windows.ucas 8A602D4D    284,469,920 bytes
StackOBot-Windows.pak  7CAE22DD     10,115,703 bytes
```

## 39. SMOKE — ✅ ALL ASSERTIONS PASS. The new build reproduces m25's certified behaviour.

One leg in `CB_GateLevel`, 1280×720 windowed, 100 % scale, `VideoFps` 30 pinned, SVE default **not
forced**, delivery OFF, target `StaticMeshActor_49` (on-calibration, so **B1 applies**).

| assertion | value | |
|---|---|---|
| `capture_path` | `sve` | ✅ |
| `content_clock` | `wall` | ✅ |
| `delivery_mode` | `false` | ✅ |
| `key_ring_missed` | `0` | ✅ |
| `published == consumed` | `121 == 121` | ✅ |
| `key_ring_corrupted` | `0` | ✅ |
| **B1 pose-match** | **YES** — modal bbox `(0.0, 485.6, 301.1, 234.4)` vs calib `(0.0, 485.2, 306.1, 234.8)`, tol 8.0 px | ✅ |
| A56 | modal **100.0 %** of 59 bbox rows, **1** distinct → CERTIFIABLE | ✅ |
| **A54** | **ALL-ALIGNED, 7/7, decidable 7/7**, median margin **0.103835** | ✅ |
| **positive control BOTH ways** | `+1` → **7/7 SHIFTED**, `−1` → **8/8 SHIFTED**, decidable 0/7 and 0/8 | ✅ |
| counted events | **7** (≥ 3) | ✅ |
| provenance | 8 events, **all `valid:true`, 9/9**, `poll_distance` 418.1 | ✅ |

⇒ **The new build becomes the measurement build for path (a).**

### 39.1 Two observations recorded, neither acted on

1. **A63 needed 3 attempts; two were banked bifurcation discards.** Attempts 1 and 2 settled non-modal
   — `modal_rot` `(359.46, 358.38)` and `(354.24, 353.71)`, settle windows starting at frames 37 and 51
   — i.e. **genuine A47 bifurcation**, matching the discriminator's own signature (non-uniform ratio +
   displaced `modal_rot`), *not* the resolution-scope pattern. **2-of-3 is a higher bifurcation rate
   than the record's ~2-in-5**, and the new pak is much larger, so startup/streaming timing plausibly
   differs. ⛔ **n = 3. Association only. No mechanism adopted, nothing changed.**
2. **The accepted leg's rest pose is `(0.0, 0.35, 0.0)`, not exactly `(0,0,0)`** as H4's legs were.
   Inside `SETTLE_TOL_DEG` 0.5 and inside `POSE_TOL_PX` 8.0, so it certifies — and it moves
   `coverage_pct` from `7.7977` to `7.6575` and the modal bbox width from `306.1` to `301.1`. **Recorded
   because a future comparison against an H4 leg should know the poses are near-identical, not
   identical.**

## 40. State after PART SIX

| | |
|---|---|
| cook | ✅ **done** — `BUILD SUCCESSFUL`, 2 m 27 s, ExitCode 0 |
| plugin production code | **ZERO lines, across all six parts** |
| tag | **none** — a re-cook is not a milestone |
| `feature/stencil-capture` | **untouched** |
| `GameDefaultMap` | **unchanged** (`…/MainMenu.MainMenu`) |
| `CB_GateLevel` | **untouched** (G99) — and **retained in the cook** |
| baselines | 5 evacuated + hash-verified at `_binary_baselines\`; 4 also still in the staged tree |
| bank | 91 → **104** dirs (9 rescued pre-cook, 4 smoke-leg dirs) |
| disk | 32.8 GB free |
| MainWorld reachable | **cooked in — not yet launched.** No path (a) leg was run. |

⛔ **MainWorld was NOT launched beyond what the map-set read-back required — which was a container
read, not a launch.** No occlusion measurement. **The path (a) design is chat-side and is not written
here.**

---
---

# PART SEVEN — MainWorld first launch. RECON, not path (a).

⛔ **THIS IS NOT A PATH (a) TEST.** It declares no hypothesis about occlusion and grades nothing.
**B1 IS NOT APPLICABLE and is DECLARED so, not skipped** — `TAU`, `CALIB_BBOX` and `POSE_TOL_PX` are
scoped to `CB_GateLevel` / `StaticMeshActor_49` / 1280×720 (G117), and MainWorld satisfies none of it.
**No A54 verdict was produced or attempted.**

**Why recon came first:** `G-1` established that `Trigger` is **empty in the ASSET FILES**. It did
**not** establish that anything **moves at runtime** with no player present. Separate claims (G120),
and only one had been observed.

## 41. Rulings 1–3, landed

**Ruling 1 — build identity is a quartet.** `_binary_baselines/README.md` rewritten: every exe entry
now states it is **EXE ONLY** and **does not reconstruct a build**, with `.m25-baseline` called out —
**the m25-era pak (3 maps, `TESTVALUE123`, `.utoc` 194,996 B) was overwritten by the cook and is
gone.** ⚠ **The loss is BOUNDED and the receipt is the G-2 sweep** (§30, run *before* the cook for
exactly this reason): the debt list was **empty**. ⛔ **No reconstruction attempted.** `G121` amended
with the same. **The PATH-(a) MEASUREMENT BUILD is now preserved COMPLETE** —
`pathA-measurement-build-paks/` beside the exe, **hash-verified at the new location** (A62), 282.9 MB,
5 of 5 match: `exe 101AFEA4 · utoc 939B9C9B · ucas 8A602D4D · pak 7CAE22DD` (+ `global.*`).

**Ruling 2 — the runbook gap is closed.** `setup-runbook.md` **§8.6 FULL COOK** added: re-bank first
(with the *match by session id, not directory name* trap that found 9 unbanked items), rescue the
**quartet**, declare the map set, the full `-map=` command, `verify_cooked_maps.ps1` **as a gate**, the
token read-back, the A44 scan, build identity, and the smoke leg.

**Ruling 3 — the two observations carried, not chased.** Bifurcation 2-of-3 is **association only, not
a lead**; it becomes worth a number only if it persists across MainWorld legs. And **any future
comparison against an H4 leg must treat the poses as NEAR-identical, not identical** — `(0.0, 0.35, 0)`
vs `(0,0,0)`, `coverage_pct` 7.7977 → 7.6575, modal bbox width 306.1 → 301.1.

## 42. R-1 — DOES IT BOOT, AND IS IT ACTUALLY MAINWORLD? ✅ **YES.**

```
LogNet:            Browse:  /Game/StackOBot/Maps/MainWorld?Name=Player
LogLoad:           LoadMap: /Game/StackOBot/Maps/MainWorld?Name=Player
LogWorldPartition: ULevel::OnLevelLoaded(MainWorld)(bIsOwningWorldGameWorld=1, bIsOwningWorldPartitioned=1 …)
LogWorldPartition: WorldPartition initialize took 441 us
LogWorld:          Bringing World /Game/StackOBot/Maps/MainWorld.MainWorld up for play
```

**By LEVEL NAME, not by the picture** (G87's headline rule, which survives its own correction):
`CaptureBench.Probe 1` emitted **1102 ticks, every one `level=MainWorld, actors=432`. Zero MainMenu
loads. Nothing redirects.** The subsystems initialise for world `'MainWorld'`. The process **stayed
alive** — where the pre-cook build **exited** on the missing package, this one runs.

## 43. R-2 — IS THERE A VIEW, AND WHAT IS IN IT? ✅ **A POPULATED GAMEPLAY VIEW.**

Not black, and not a default view from the origin. A **streaming source exists without any input** —
`New Streaming Source: PC_InGame_C_2147482444 -> X=3450.000 Y=4020.000 Z=1519.836`,
`CellsToActivate(1)` — so the prior that *"an unattended run may have no streaming source at all"* is
**refuted**.

Post-settle view: origin `(3073.76, 4335.69, 1634.48)`, rot `(0, −39.999, 0)`, 1280×720, with **6
renderable-visible actors**: `SM_Ramp2`, `BP_SplineSpawn`, `RoomBuilderSquare`, `BP_SpawnPad` and two
`InstancedFoliageActor`s.

⚠ **The camera is NOT static for the first ~25 frames** — the pawn settles from pitch −20 at
`(2982.1, 4412.6, 1742.1)` to pitch 0 at `(3073.8, 4335.6, 1634.5)`, then holds to ~2 cm. At **startup**
the visible set was a *different* 6 — `BP_Stomper` (dist 779), `BP_Elevator` (1010),
`RoomBuilderSquare`, `BP_SpawnPad`, 2 foliage — so **set membership changes during settle**, and that
turned out to be camera motion, not actor motion (§45).

## 44. R-3 — ARE THE MOVERS LOADED? ✅ **YES, ALL OF THEM.**

**Instrument: `IAI.ListActors` from `-ExecCmds`, i.e. at STARTUP.** ⚠ It is a **lower bound** on what
is loaded, never an upper one — and it reported **432 actors in world `MainWorld`** at frame 1, against
419 external actor files, so World Partition had **already streamed essentially everything**.

Named in that list, matching the asset census exactly: **7 `BP_Stomper_C`**, **7 `BP_MovingPlatform`**
(6 `_C` + 1 `BP_MovingPlatform2`), **4 `BP_Fan_C`**, plus `BP_EnergyOrb`, `BP_Elevator`, `BP_Door`.

**Second, independent instrument: targeted fire resolved them.**
`blinking: matched 1 actor(s) for '=BP_Stomper_C_UAID_…'` → `Auto.FireSpecific: applied`, on every
mover tried. ⚠ **Third instrument, `snapshot.visible` (the auto-pool's own enumeration), sees only 6** —
because it is renderable ∧ poll-radius ∧ frustum ∧ unoccluded ∧ ≥6 % coverage. **What the auto-pool can
SELECT is far narrower than what is LOADED**, and conflating the two would have read as "not loaded".

## 45. R-4 — DO THEY MOVE, UNATTENDED? ⚠ **SPLIT BY CLASS. AND THE FIRST ANSWER WAS WRONG.**

### 45.1 🚨 The first measurement targeted a **BOUND** Stomper — caught before it was reported

The first leg fired at `BP_Stomper_C_UAID_B42E9936F5429ADA00_2086831169`, which was on-screen at
startup. It measured **completely static**: over frames 3–16 the **camera was identical to one decimal**
at `(2982.1, 4412.6, 1742.1)` pitch −20, and the target's `bbox_px` was **identical** at
`(0.0, 0.0, 181.8, 720.0)` on all 10 of those frames.

That looked like the pre-declared **MOVERS LOADED BUT STATIC** halt. **It is not, and reporting it
would have been a false finding.** Joining runtime UAID names back to the asset files —
`PersistentLevel.<Class>_C_UAID_…` appears inside each external actor file, so the join is exact —
shows that instance is file `0E5JK19NZI4C74C00ZZ7N`: **one of the two BOUND Stompers.**

**A trigger-bound Stomper standing still with nobody on the pressure plate is the EXPECTED result and
tests nothing.** G-1's claim was about the **unbound** instances. ⚠ **The gap was mine: G-1 keyed on
FILES, this leg keyed on RUNTIME UAID NAMES, and the two had never been joined.** The join is now done
for all 13 Stomper/MovingPlatform instances and is in the record.

### 45.2 Re-measured on **unbound** instances, at TWO cadences to exclude aliasing

Anchors are 12 frames apart at the default config, so 8 identical samples could be a period that
divides 0.4 s. A second leg used `IAI.Capture.Config 3 2 5 2 0` → **13 events at 7-frame spacing**.

| leg | target | events | distinct positions | verdict |
|---|---|---|---|---|
| `MW_STOMP_FREE` | **unbound** Stomper `…F542F9D500_2012109606` | 8 (spacing 12) | **1** | **STATIC** |
| `MW_STOMP_FREE_C2` | same, cadence B | **13 (spacing 7)** | **1** | **STATIC** |
| `MW_PLAT_FREE` | **unbound** MovingPlatform `…F542EBDB00_1649270448` | 8 | **8** | ✅ **MOVES** |

**21 samples of the unbound Stomper at two incommensurate cadences, all `(8962.014, 8754.727,
2599.993)` to three decimals.** A period would have to divide both 12 and 7 frames — 84 frames = 2.8 s
against a 3 s leg. **Aliasing is excluded in practice.**

**The MovingPlatform translates on Z, monotonically:**

```
1666.889 -> 1734.444 -> 1802.000 -> 1869.556 -> 1937.111 -> 2004.667 -> 2072.223 -> 2139.779
+67.556 cm per 12 frames (0.4 s)  =>  ~168.9 cm/s   |   472.9 cm across the leg
```

X and Y constant. That is `InterpToMovementComponent` on its control-point path, **with no player and
no input**. ⚠ **`global_position` is a WORLD position and camera-independent** — this is actor motion,
not view motion.

**⇒ G-1's asset reading is CONFIRMED at runtime for `BP_MovingPlatform` and REFUTED at runtime for
`BP_Stomper`.** ⛔ **n = 1 instance of each. No mechanism proposed for why the unbound Stomper is
still.** The asset comment (*"when no trigger is referenced it move constantly"*) is an **asset fact**
that did not survive contact for that class — exactly the separation G120 demands, now with a
measurement on both sides.

## 46. R-5 — WHAT DOES THE CAPTURE PIPELINE DO HERE? **RECORDED, NOT GRADED.**

| leg | frames | positive | bursts | speed_ratio | sustained fps | ring p/c/missed | events | zero-match |
|---|---|---|---|---|---|---|---|---|
| `MW_STOMPER` (bound) | 90 | 59 | 7 | 1.0000 | 30.00 | 121/121/**0** | 8 | 0 |
| `MW_STOMP_FREE` | 90 | 59 | 7 | 1.0020 | 29.94 | 121/121/**0** | 8 | 0 |
| `MW_STOMP_FREE_C2` | 90 | 65 | 12 | 1.0000 | 30.00 | 166/166/**0** | 13 | 0 |
| `MW_PLAT_FREE` | 90 | 59 | 7 | 1.0000 | 30.00 | 121/121/**0** | 8 | 0 |

`capture_path` **`sve`**, `content_clock` **`wall`**, `delivery_mode` **false**, `non_manifested` **0**
on every leg. **Ratio is in band and no streaming hitch is visible** in frame counts, pacing or ring
counters. ⛔ **Not graded. An out-of-band ratio here would have been DATA, not a failure.**

## 47. 🔬 A free observation, recorded and NOT acted on

The MovingPlatform leg is fully instrumented and its **occlusion state is DYNAMIC**:

| anchor | valid | occlusion samples | coverage_pct | poll_distance | node Z |
|---|---|---|---|---|---|
| 3 | true | **6/9** | 0.5712 | 1370.7 | 1666.889 |
| 15 | true | 7/9 | 0.4118 | 1374.6 | 1734.444 |
| 27 | true | 7/9 | 0.8807 | 1380.6 | 1802.000 |
| 39 | true | **9/9** | 1.0817 | 1389.5 | 1869.556 |
| 51 | true | 7/9 | 1.2911 | 1401.0 | 1937.111 |
| 63 | true | 7/9 | 1.5043 | 1414.9 | 2004.667 |
| 75 | true | **9/9** | 1.7191 | 1431.4 | 2072.223 |
| 87 | true | **9/9** | 1.9345 | 1450.2 | 2139.779 |

`bbox_valid` **59/59** and `visible_positive` **59/59** — on screen throughout.

⚠ **Two things are visible here and NEITHER is claimed:** the occlusion sample count **varies within a
single window** (6 → 7 → 9 → 7 → 9), and the **6/9 and 7/9 rows are `valid:true`** — i.e. selected —
which is the `P-a1` band (*"1 of 9 clear ⇒ unoccluded"*) appearing in live data. ⛔ **This is RECON.
No path (a) hypothesis is declared, nothing is graded, and `P-a1`…`P-a5` remain UNTESTED
predictions.**

## 48. Harness faults found and fixed

- **`check_pose.py` CRASHED the harness** (`TypeError: 'NoneType' object is not iterable`) on a leg
  with **no bbox rows in the settle window** — which is normal off-calibration, where the camera
  settles looking elsewhere. ⚠ **The crash was inside the block whose own header says REPORTING
  ONLY**, and it killed the run *after* the artifacts were written. Fixed: a `bbox is None` branch that
  says plainly that **B1 has nothing to judge — not a pose reading and not a pose failure.**
- **`run_leg.ps1` gained `-Map`** (harness targeting parameter, same class as `-Target`), and
  `_leg_geometry.json` now records it.

## 49. THE BRANCH THAT OBTAINED

> **IT ALL WORKS.** Level boots and stays in MainWorld; the view is populated; the movers are loaded;
> **at least one class moves unattended**; capture produces frames and labels with clean pipeline
> numbers.

⚠ **With one pre-declared branch partially firing: `BP_Stomper` is LOADED BUT STATIC even when
unbound** — a complete finding in its own right, and G-1's runtime extrapolation is refuted **for that
class**. It does **not** send path (a) to a driven camera, because `BP_MovingPlatform` supplies the
unattended translating occluder the design needs.

⛔ **REPORTING AND STOPPING. No path (a) test designed, no path (a) test run.** That design is
chat-side and is not written here.

## 50. State after PART SEVEN

| | |
|---|---|
| plugin production code | **ZERO lines, across all seven parts** |
| tag | **none** |
| `feature/stencil-capture` | **untouched** |
| `GameDefaultMap` | **unchanged** · `CB_GateLevel` **untouched** (G99) |
| build | unchanged since the cook — `exe 101AFEA4` + `utoc 939B9C9B`, preserved complete in `_binary_baselines\` |
| bank | 104 → **111** dirs (4 MainWorld legs + attempts) |
| A63 | every leg accepted on attempt 1, focus at 1.4–1.5 s; **every attempt banked** |

---
---

# PART EIGHT — the geometry survey. NO hypothesis, NO test, NO P-a prediction touched.

⛔ **B1 NOT APPLICABLE, declared not skipped. Nothing graded. Shipping defaults, delivery OFF.**

## 51. ⚠ CORRECTION TO PART SEVEN — the platform's speed was wrong by 33 %

PART SEVEN reported the MovingPlatform at **"~168.9 cm/s"**. **It is 126.67 cm/s.** I divided a
12-anchor gap by 0.4 s assuming 12 captured frames = 0.4 s at 30 fps. **Game time between those
anchors is 0.5333 s, not 0.4 s** — captured frames are not the only ticks; the burst schedule's
settle frames tick without being captured.

Measured on two legs with different burst configs, normalised by `labels.jsonl`'s own `t`:

| leg | config | Δframes | ΔZ | Δt (game) | speed | per captured frame |
|---|---|---|---|---|---|---|
| `MW_PLAT_FREE` | `2 4 8 4 0` | 12 | 67.556 | 0.5333 s | **126.67 cm/s** | 5.630 cm |
| `MW_Q1_PLAT_LONG` | `3 2 5 2 0` | 7 | 54.889 | 0.4333 s | **126.67 cm/s** | 7.841 cm |

**Game-time speed is identical to 2 d.p.; cm-per-captured-frame differs by 39 %.**

🚨 **THIS IS A DESIGN CONSTRAINT, not just an arithmetic fix. THE OCCLUDER'S SPEED IN CAPTURED FRAMES
DEPENDS ON THE CAPTURE CONFIG.** Any path (a) design that says *"the occluder crosses at frame N"*
**must state the burst config**, or the number is meaningless. **Never convert frames to seconds by
dividing by `VideoFps` — read `t` from `labels.jsonl`.**

## 52. Ruling 1 — the 6/9 and 7/9 rows, held exactly where ruled

Recorded as **PATH (a) MOTIVATING DATA, not as a result**: the transition series
`6/9 → 7/9 → 7/9 → 9/9 → 7/9 → 7/9 → 9/9 → 9/9` with Z `1666.9 → 2139.8` and `coverage_pct`
`0.5712 → 1.9345`, all rows `valid:true`.

⛔ **This CORROBORATES `P-a1`'s premise that the band is occupied in practice. It does NOT test
`P-a1`**, which is about a target **passing pick time** at 8/9 blocked; these are **per-anchor
provenance readings on a target that was already selected**. **`P-a1`…`P-a5` remain UNTESTED and none
is marked touched.** No test was designed around it.

## 53. Ruling 2 — the two namespaces are joined, permanently → **G122**

`CaptureBench/tools/mainworld_instance_join.md` (the table) and `mainworld_join.ps1` (regenerates the
asset half) are committed. **18 instances** across `BP_Stomper` (7), `BP_MovingPlatform` (7), `BP_Fan`
(4), each carrying **class · asset file · runtime UAID name · trigger status · runtime motion
OBSERVED / REFUTED / UNTESTED**.

**The join key is exact:** every one-file-per-actor `.uasset` contains its own object path
`…PersistentLevel.<Class>_C_UAID_<HEX>_<N>`, and that substring **is** the runtime name. Nothing else
bridges them — file basenames are opaque GUIDs and **actor labels do not exist in a cooked build**.

✅ **Key sanity, both directions (G96): the two independent signals — *property serialised* and
*references a trigger actor* — AGREE on all 18 rows**, and the tool prints any disagreement. ⚠ **That
check is what caught `BP_Fan`'s ARRAY key** (`Triggers`, not `Trigger`): on the scalar, Fans read
4-of-4 empty while 2 of 4 referenced a PressurePlate. Census: **Stomper 5 EMPTY / 2 BOUND ·
MovingPlatform 4 EMPTY / 3 BOUND · Fan 2 EMPTY / 2 BOUND.**

⛔ **`UNTESTED` is not "presumed moving"** — the asset comment is already refuted at runtime for one
unbound Stomper.

## 54. Q-1 — THE PLATFORM'S TRAJECTORY, FULLY CHARACTERISED. It PingPongs and it REPEATS.

300-frame leg, 43 events at 7-frame anchors, 18.2 s of game time:

```
rise  f2   Z 1658.444  ...  f100 Z 2393.111   (+54.889 per anchor, constant)
fall  f100          ...  f198 Z 1675.333   (-54.889 per anchor, constant)
rise  f198          ...  f289 Z 2388.890   (+54.889, the cycle REPEATS)
```

| property | value |
|---|---|
| axis | **Z only** — X and Y fixed at `(3980, 2380)` across all 43 events |
| endpoints | **Z 1658.4 (bottom) ↔ 2393.1 (top)** |
| travel | **734.7 cm** |
| speed | **126.67 cm/s**, constant on both directions |
| **period** | **≈ 12.1 s** (bottom→top→bottom, f2→f198), and it repeats |
| turnaround | visible as the two short deltas `+21.110` and `−4.221` at the endpoints — **no dwell** |

⚠ **Q-1's BOUNDS QUESTION HAS A TRAP, AND IT IS LOAD-BEARING FOR THE SURVEY.**
`annotation.json`'s `nodes[].bounds` reports **origin (5845, 3445, 2490), extent (2115, 1315, 848)** —
a **42 m × 26 m × 17 m box centred ~2.2 km from the actor**. That is `ResolveNodeIdentity` calling
`Actor->GetComponentsBoundingBox(**true**)` — the **whole actor including non-colliding components**,
and `BP_MovingPlatform` carries spline / control-point components and a Control Rig.
**The projector and `IsUnoccluded` use only SM/SK `Component->Bounds`.** Confirmed empirically: the
platform's actual label rect is **240 × 20 px** with `coverage_pct` 0.57–1.93 %, which a 42 m box at
that range could not produce.
⛔ **`annotation.json`'s `bounds` is NOT the volume the label rect or the occlusion trace describe.**
Using it for this survey would have inverted the answer. *(Recorded as an observation of the shipped
contract. **`P6` DOES NOT MOVE** — no field added, removed, renamed or recomputed.)*
⚠ **Same family, second field: `global_position` is the ACTOR ORIGIN.** `BP_SplineSpawn`'s origin is
`(-278, 19104, 4946)` — **142.8° off-axis, i.e. behind the camera** — while its geometry projects to
`x 552..1280`. Fine for a plain `StaticMeshActor`, misleading for a Blueprint that offsets meshes.

## 55. Q-2 — ALL FOUR UNBOUND PLATFORMS MOVE, ON DIFFERENT AXES

| instance | axis | range | speed (game time) | position |
|---|---|---|---|---|
| `…F542EBDB00_1649270448` | **Z** | 734.7 cm | **126.67 cm/s** | `(3980, 2380)` |
| `…F542F9D500_2012115617` | **Z** | 476.0 cm | **210.00 cm/s** | `(9433, 7843)` |
| `…F542F9D500_2012117619` | **HORIZONTAL** (Y 752.5 + X 132.7) | 752.5 cm | ~110 cm/s | `(≈9270, ·, 3726)` |
| `…F542F9D500_2012105601` | **HORIZONTAL** (X 995.6 + Y 175.5) | 995.6 cm | ~145 cm/s | `(·, ≈9395, 3720)` |

**4 of 4 move. Two travel vertically, two horizontally** — and horizontal travel is the shape that
crosses a view rather than running along it.

## 56. Q-3 — WHAT THE AUTO-POOL CAN ACTUALLY SELECT HERE

Six renderable-visible actors at settle. Geometry obtained by **firing at each** (the only
engine-authoritative source of position; `DumpVisible`/`DumpCoverage` give neither position nor
bounds):

| actor | modal `bbox_px` | range | bearing | `poll_distance` |
|---|---|---|---|---|
| `SM_Ramp2` | `(142, 144, 286, 189)` | 798 cm | 28.0° | **389.9** |
| `RoomBuilderSquare_C` | `(0, 123, 1280, 597)` | 1185 cm | 20.1° | sentinel −1 |
| `BP_SpawnPad_C` | `(291, 440, 673, 236)` | 504 cm | 0.0° | sentinel −1 |
| `BP_SplineSpawn_C` | `(552, 0, 728, 290)` | *(origin 15502 cm, 142.8° — geometry offset)* | — | sentinel −1 |
| `InstancedFoliageActor_0_-1_0` | ~`(0,0,1060,720)` | **16740 cm** | — | — |
| `InstancedFoliageActor_0_0_0` | ~`(0,0,1280,713)` | **10831 cm** | — | — |

⚠ **Three targets report `poll_distance` as the −1 sentinel** — their provenance was `valid:false` at
their own legs' **anchor frames**, which fall in the first ~25 frames **while the camera is still
settling**. They are in the visible set *after* settle. **Anchor-time and settled-time visibility are
not the same set**, and this survey needed both readings to see that.

## 57. Q-4 — THE INTERSECTION. **NO CROSSING PAIR EXISTS for FULL occlusion.**

**Only ONE of the four platforms is in the frustum at all.** The other three sit at **68.9°, 75.5° and
75.0° off-axis**, against a 45° horizontal half-FOV — **outside it**, at 7492–9057 cm. They cannot
occlude anything in this view.

The near platform: range **2191 cm**, bearing **25.1°**, rect `(219, 137, 240, 20)`, `poll_distance`
**1370.7 → 1450.2**.

| target | rect overlap | platform nearer? | contains? | verdict |
|---|---|---|---|---|
| `SM_Ramp2` (389.9) | 209 × 13 px | **no** | no | **NO — the platform is FARTHER; the ramp occludes IT** |
| `RoomBuilderSquare` (1185 cm) | 240 × 20 px | **no** (2191 > 1185) | no | **NO — platform behind, and its rect is 1/160th the target's** |
| `BP_SpawnPad` | **0 px in y** | — | no | **NO — rects do not overlap** |
| `BP_SplineSpawn` | **0 px in x** | — | no | **NO — rects do not overlap** |
| `InstancedFoliageActor` ×2 (10831 / 16740 cm) | overlap | **YES** | **no** | ⚠ **PARTIAL AT MOST — 240×20 px against ~1280×713; containment impossible** |

**⇒ THE PRE-DECLARED HALT OBTAINS: no pair exists in which a moving platform can FULLY occlude a
selectable target from the settled camera.** Full occlusion — 9 of 9 rays blocked — requires the
occluder's rect to **contain** the target's, and no pair satisfies that.

⚠ **Stated precisely, because the weaker claim is true and matters:** the platform **is** in front of
both foliage actors with overlapping rects, so **PARTIAL occlusion pairs DO exist**. Partial occlusion
is a different condition from the one path (a) needs and is not offered as a substitute.

## 58. Q-5 — THE CAMERA HOLDS. Emphatically.

Over the 300-frame leg, after the ~25-frame settle from pitch −20 to 0:

```
LAST 200 FRAMES (6.7 s):  dX 0.0004  dY 0.0003  dZ 0.0000  dPitch 0.0000  dYaw 0.0000
```

**Sub-millimetre in position, exactly zero in rotation.** No drift, no fall, no push. ✅ **The
still-camera assumption every design rests on is measured and holds.** Pipeline across all survey
legs: `capture_path sve`, `clock wall`, `delivery false`, ring `556/556/0` on the 300-frame leg,
`speed_ratio` 1.0000, sustained 30.00 fps.

## 59. THE BRANCH THAT OBTAINED, and what is NOT concluded

> **NO CROSSING PAIR EXISTS** (for full occlusion), from the settled camera, with the four platforms
> and six selectable targets this level presents unattended.

⛔ **What this does NOT say.** It is scoped to **one camera pose** — the one an unattended run
settles into. It is **not** a statement about MainWorld in general: a different spawn, a driven
camera, or a different selectable set could change it entirely. ⛔ **No design is proposed.** The
ruling is explicit that a driven camera or a scripted occluder is the owner's call.

⛔ **`P-a1`…`P-a5` remain UNTESTED.** ⛔ **`feature/stencil-capture` untouched.** ⛔ **No path (a) test
designed or run.** ⛔ **Zero production code.**

## 60. 📋 A REQUEST, NOT A DECISION: journal 045 is getting unwieldy

This journal now runs to **eight parts and ~1,500 lines** covering the H4 pre-flight, the H4 run, the
claim ruling, the environment scout, the pre-cook gate, the cook, the MainWorld first launch and this
survey. It is still **one investigation**, so it has not been split — **the ruling is the owner's and
splitting is not done unilaterally.** Flagging it because a cold reader arriving at "H4 pre-flight
halt" will not guess that the cook and a MainWorld geometry survey are inside it. **A rename plus a
part index at the top would fix most of it without a split.**

## 61. State after PART EIGHT

| | |
|---|---|
| plugin production code | **ZERO lines, across all eight parts** |
| tag | **none** · `feature/stencil-capture` **untouched** |
| build | unchanged — `exe 101AFEA4` + `utoc 939B9C9B`, preserved complete |
| `GameDefaultMap` unchanged · `CB_GateLevel` untouched (G99) | |
| bank | 110 → **126** dirs (8 survey legs + attempts) |
| new gotchas | **G122** (namespace join), **G123** (a reporting path that can kill the run) |
| new instruments | `mainworld_instance_join.md`, `mainworld_join.ps1`, `mainworld_q4_geometry.py` |

---
---

# PART NINE — owner evidence redirects the lead. **`H5` is minted and class (ii) is SUPPORTED.**

## 62. The evidence, and its provenance label

🧾 **OWNER OBSERVATION — real evidence, EYEBALL-LEVEL, NOT MEASURED.** The owner inspected the
client's delivered output directly and reported:

1. **The invisible anomalies were NOT PARTIALLY HIDDEN. They could not be found at all.**
2. Two culprit classes named in the client's data: **`InstancedMeshActor`** and
   **`BP_LocalVolumetricFog`**. Both selected and hidden; for both, *"the blinking wasn't visible for
   obvious reasons"*.
3. The remaining invisible cases **could not be attributed**, because every actor is named
   `StaticMeshActor_xxx`.

⛔ **Not upgraded to measurement, not discarded.** It is the provenance label that makes it usable.

### 62.1 A chat-side ruling was WITHDRAWN, and the distinction matters

> **WITHDRAWN:** *"partial occlusion is the right target."* It rested on what `CB_GateLevel` and
> MainWorld can produce, **not on what the client's data shows.** The owner's evidence outranks that
> inference.

**What SURVIVES unchanged:** the Part Eight *"no crossing pair"* answer for full occlusion, and **H4
as SUPPORTED (path b)**.

⚠ **PATH (a) IS PARKED, NOT REFUTED — and this is a PRIORITY decision, not a SCOPE one.** G120 forbids
deriving a **scope** decision ("that approach is dead") from an unverified mechanism. Nothing here
refutes path (a); a better-evidenced lead simply arrived. **The distinction is in the record so that
re-opening path (a) needs no argument, only a decision.**

## 63. **`H5`** — minted. Number verified against the record first.

**Assigned: `H5`.** The record holds `H1` (GPU-load starvation, 49 mentions), **`H2` RETIRED-UNKNOWN**
(*"appears nowhere in this repo; history unrecoverable; never re-mint this number"* — journal 037
§1.2), `H3` (auto-exposure), `H4` (occlusion-blind labelling, 109 mentions). **No `H5`–`H9` anywhere.**
**Numbers are never reused**, so `H5` is the next free one.

> **`H5` — THE SELECTOR ADMITS OBJECTS THAT CANNOT MANIFEST A VISIBLE HIDE.**
> Selection requires a renderable component (`IsRenderableComponent`, SM-or-SK, G33). **That is a TYPE
> test. It is not a DRAWING test.** An actor can satisfy it while contributing no pixels, or while
> contributing pixels nowhere near where its label says. Hiding such an actor produces a label with no
> corresponding visual change, **in plain sight, with no occlusion involved.**
>
> **(i) NON-DRAWING MESH COMPONENT** — exists for editor visualisation and is not drawn in game.
> Owner-observed instance: `BP_LocalVolumetricFog`. **Not reproducible here** — the client runs her own
> game. **What IS ours and IS readable is the FILTER that admitted it.**
> **(ii) AGGREGATE / INSTANCED ACTOR** — ISM/HISM derive from `UStaticMeshComponent` and pass the type
> test trivially, while their `Bounds` cover the whole cluster. Owner-observed instance:
> `InstancedMeshActor`. **Reproducible here.**

⛔ **RELATIONSHIP TO H4, so neither absorbs the other: H4 is a target that WOULD draw but is BLOCKED.
H5 is a target that WOULD NOT DRAW ANYWAY. Same symptom, different mechanism, different cure. TWO
ITEMS.** ⛔ **`P-a1`…`P-a5` remain UNTESTED; none is marked touched.**

## 64. TASK 1 — THE FILTER, FROM SOURCE. **SOURCE READING, NOT MEASUREMENT.**

The whole of it, verbatim (`AnomalyViewport.cpp:493-510`):

```cpp
bool IsRenderableComponent(const UPrimitiveComponent* Component)
{
    if (!Component || !Component->IsVisible())        { return false; }
    if (const UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Component))
    {
        if (ISM->GetInstanceCount() <= 0)             { return false; }
    }
    return Component->IsA<UStaticMeshComponent>()
        || Component->IsA<USkinnedMeshComponent>();
}
```

| checked? | predicate | evidence |
|---|---|---|
| ✅ **PRESENT** | `bVisible` / `bHiddenInGame` | `Component->IsVisible()`. Engine source: `USceneComponent::IsVisible()` = `if (bHiddenInGame) return false; return GetVisibleFlag() && (!CachedLevelCollection \|\| CachedLevelCollection->IsVisible());` |
| ✅ **PRESENT** | instance count > 0 on ISM/HISM | `ISM->GetInstanceCount() <= 0` |
| ⛔ **ABSENT** | **owner actor's `bHidden`** | `IsVisible()` does **not** consult the owner. `UPrimitiveComponent::ShouldRender()` does — the filter does not use `ShouldRender()` |
| ⛔ **ABSENT** | `bRenderInMainPass` | the identifier appears nowhere in the plugin |
| ⛔ **ABSENT** | `GetStaticMesh() != nullptr` | never called on the selection path |
| ⛔ **ABSENT** | section / triangle count | never queried |
| ⛔ **ABSENT** | material presence, or a material that draws nothing | never queried |
| ⛔ **ABSENT** | `WasRecentlyRendered()` | never called |
| ⛔ **ABSENT** | **any distinction between ISM/HISM and a plain SMC** | `UInstancedStaticMeshComponent : public UStaticMeshComponent`; `UHierarchicalInstancedStaticMeshComponent : public UInstancedStaticMeshComponent`; `UFoliageInstancedStaticMeshComponent : public UHierarchicalInstancedStaticMeshComponent`. **All three pass `IsA<UStaticMeshComponent>()` trivially**, and after the instance-count guard nothing treats them differently |

**The companion predicates on the same path, and what each tests:**
`IsInFrustum` — bounds sphere ∧ box against the frustum. `IsUnoccluded` — 9 `ECC_Visibility` traces to
**bounds centre + 8 AABB corners**, `bTraceComplex=false`, **first clear ray wins**. Poll radius —
`dist(pollOrigin, B.Origin) − B.SphereRadius > R`. `PassesScreenCoverage` — projected **union bounds**
rect area ≥ 6 %. **Every one of them is computed on BOUNDS. Not one reads a pixel, a material or a
draw call.**

### 64.1 By what route could a `BP_LocalVolumetricFog`-shaped actor pass this filter?

⛔ **The contents of that Blueprint are NOT in this project and are NOT guessed.** Reasoning **from our
filter outward**, these are the routes the code *permits* — each is a property our filter never reads:

1. **A visible mesh component that is not drawn in the main pass.** `bRenderInMainPass = false` passes
   `IsVisible()` and is invisible on screen. **Nothing checks it.**
2. **A mesh component with a null `StaticMesh`** — or one whose mesh has no renderable sections.
   `IsVisible()` is about the component, not its contents. **Nothing checks it.**
3. **A material that renders nothing** (fully translucent, opacity 0, a null material slot).
   **Nothing checks it.**
4. **A component visible in the editor but hidden at runtime via the OWNER** — `SetActorHiddenInGame`
   sets the *actor's* `bHidden`, which `IsVisible()` **does not consult**. **Nothing checks it.**
5. **A volumetric / effect actor carrying an SMC purely as an editor gizmo or bounds proxy** — the type
   test cannot distinguish a gizmo from a rendered prop.

⚠ **Any one of these is sufficient. The filter's guarantee is "this component is of a renderable TYPE
and is flagged visible", and that is strictly weaker than "this component draws pixels."** That gap
**is** `H5`.

## 65. TASK 2 — CLASS (ii) REPRODUCED HERE. ✅ **SUPPORTED.**

Branches were **pre-registered as a file before the result was read.** Leg `H5_MW_H5_FOLIAGE`,
MainWorld, 1280×720 windowed, 100 %, `VideoFps` 30 pinned, SVE **not forced**, **delivery OFF**,
shipping defaults, A63 accepted on attempt 1 (focus 2.5 s), **B1 NOT APPLICABLE — declared**,
**no A54 verdict** (G117).

**Selection succeeded:** `blinking: matched 1 actor(s) for '=InstancedFoliageActor_0_0_0'` → `applied`.
8 events, 0 zero-match, 0 non-manifested.

### 65.1 The label as emitted

| field | value |
|---|---|
| `component_class` | **`FoliageInstancedStaticMeshComponent`** (a HISM subclass) |
| `asset_name` | `SM_Bush` |
| **`bbox_px`** | **`(0, 0, 1280, 720)` on 59/59 rows — THE ENTIRE FRAME (100 %)** |
| **`coverage_ratio`** | **`1.00000000`** |
| **`coverage_pct`** | **`100`** |
| `manifested` | **true**, on all 8 events |
| `node.global_position` | **`[12800, 12800, 12800]`** — the foliage cell corner, not any bush |
| `node.bounds` | origin `(11428.8, 9918.1, 3915.0)` extent `(12625.1, 10863.9, 3371.7)` — **a 252 m × 217 m × 67 m box** |
| provenance | `valid:true`, **samples `1/9`**, `poll_distance` **`−5396.0`** |

### 65.2 The pixel change — RAW SERIES, NOT A VERDICT

The bbox **is** the whole frame, so in-bbox and whole-frame are the same number by construction:

| ev | claimed / flank | Δ |
|---|---|---|
| 0 | 0.257862 / 0.254574 | +0.003288 |
| 1 | 0.274649 / 0.281038 | −0.006389 |
| 2 | 0.317203 / 0.310014 | +0.007189 |
| 3 | 0.320717 / 0.313521 | +0.007196 |
| 4 | 0.318330 / 0.311257 | +0.007073 |
| 5 | 0.320565 / 0.313397 | +0.007168 |
| 6 | 0.319117 / 0.312128 | +0.006989 |
| 7 | 0.320958 / 0.311261 | +0.009697 |

**Mean |Δ| ≈ 0.0069.** Scale references already measured on this build: the `CB_GateLevel` control hide
scores **0.1023–0.1116**; H4's fully-occluded target scored **≤ 2.0 × 10⁻⁴**. ⇒ **the frame-wide change
is ~6 % of a proper hide** — an order of magnitude below the control, two above nothing.

### 65.3 The grid — **where the change actually is**

Mean |claimed − flank| per cell, 8×8, averaged over 8 events (every cell is inside the bbox, because
the bbox is the frame):

```
 0.0018  0.1800  0.1510  0.0014  0.0048  0.0033  0.0065  0.0058
 0.0039  0.0175  0.0092  0.0028  0.0051  0.0077  0.0507  0.1228
 0.0062  0.0082  0.0129  0.0084  0.0339  0.0250  0.0860  0.0453
 0.0031  0.0071  0.0013  0.0176  0.0290  0.0051  0.0029  0.0103
 0.0062  0.0015  0.0062  0.0126  0.0107  0.0008  0.0011  0.0012
 0.0023  0.0027  0.0024  0.0028  0.0020  0.0004  0.0006  0.0008
 0.0004  0.0011  0.0013  0.0012  0.0009  0.0007  0.0003  0.0006
 0.0019  0.0040  0.0041  0.0017  0.0004  0.0003  0.0006  0.0004
```

**Four cells carry the change** (0.1800, 0.1510, 0.1228, 0.0860) against a whole-frame mean of 0.0149.
**The peak cell, 0.1800, is larger than the CB_GateLevel control's whole-bbox score** — the bushes that
*do* vanish vanish emphatically. **The other 60 cells are flat.** The bottom two rows are essentially
untouched (0.0003–0.0041).

**⇒ BRANCH: CLASS-(ii) SUPPORTED.** *"A label is emitted with a large bbox and non-trivial coverage,
AND the in-bbox pixel change is negligible against the control scale."* Both hold — and the grid shows
**why**: the label claims **100 % of the frame** while the change lives in **≈ 6 % of it**.

⚠ **The predicted "change concentrated OUTSIDE the bbox" test DEGENERATED and could not run** — a
full-frame bbox has no outside. **The spatial version of the same question answered it instead.**
Recorded because a bbox-only reading (A35's rule, correct when the label points at the object) would
have reported a real 0.0069 change and called it a manifest hide.

⛔ **MECHANISM ONLY. NO INCIDENCE CLAIM.** One instance, one level, one camera pose. The owner's
`InstancedMeshActor` is **not** this actor.

### 65.4 Two guards were defeated by the SAME property → **G124**

- **`poll_distance = −5396.0`, NEGATIVE.** `dist(pollOrigin, B.Origin) − B.SphereRadius`, and the
  cluster's bounds sphere (≈ 17,000 cm) **exceeds the distance to it**. ⇒ **the 1800 cm poll-radius
  cull can never reject this actor, from anywhere in the level.**
- **`coverage_pct = 100` against a 6 % floor.** The union-bounds rect fills the screen. ⇒ **the
  screen-coverage floor is vacuous for it.**

**Both distance guards and the coverage guard are computed on the aggregate's bounds, so the very
property that makes the label wrong also makes every guard that might have caught it pass.**

### 65.5 `samples 1/9` — corroboration at the exact boundary, and NOT a test

Provenance reports **1 of 9 rays clear**, i.e. **8 of 9 blocked** — the **exact minimum** that
`IsUnoccluded` accepts, since it returns on the first clear ray. `P-a1` predicted precisely this case.

⛔ **This CORROBORATES `P-a1`'s premise and does NOT test it**, on the same reasoning ruled for the
6/9 rows: `P-a1` is about a target **passing pick time** at 8/9 blocked, and provenance is computed at
the **anchor**, after selection. **`P-a1` remains UNTESTED.** Recorded because it is the strongest
corroboration available — the boundary itself, in a shipping-defaults run.

## 66. TASK 3 — `P6`'s SECOND OBSERVATION. **RECORD ONLY.**

Two `annotation.json` node fields are computed by a **different code path** from the one that produces
the label rect:

| field | produced by | for `BP_MovingPlatform` | for the foliage actor |
|---|---|---|---|
| `nodes[].bounds` | `ResolveNodeIdentity` → **`Actor->GetComponentsBoundingBox(true)`** — the **whole actor**, including non-colliding components | origin `(5845, 3445, 2490)`, extent `(2115, 1315, 848)` = **42 m × 26 m × 17 m, centred ~2.2 km from the actor**, whose label rect is **240 × 20 px** | extent `(12625, 10864, 3372)` = **252 m × 217 m × 67 m** |
| `nodes[].global_position` | the **actor origin** | — | **`[12800, 12800, 12800]`**, a cell corner |
| `bbox_norm` / `bbox_px` | **the projector** — `ProjectActorBoundsToScreenRect` over **SM/SK `Component->Bounds` only** | correct | correct |

Same family, second field: `BP_SplineSpawn`'s origin reads **142.8° off-axis — behind the camera** —
while its geometry projects on screen.

**CONSEQUENCE, without alarm: LABELS ARE UNAFFECTED.** `bbox_norm`/`bbox_px` come from the projector
and are right. **But any consumer using `node.bounds` or `node.global_position` for geometry is reading
something that can be wrong by kilometres.**

⛔ **`P6` DOES NOT MOVE. The render-relevant-bounds ruling is NOT implemented.**

🔗 **ADJACENCY NOTED, NOT ACTED ON:** the `P6` bounds ruling and `H5` class (ii) may share a root —
**both are about which components count as "the object."** ⛔ **NOT merged. H4/P5–P7 taught that
adjacency is not identity.**

## 67. TASK 4 — TRACEABILITY. **SCOPE NOTE ONLY, NO IMPLEMENTATION.**

### 67.1 🚨 The `B1` identifier collision — BOTH found, disambiguation PROPOSED not picked

| which | meaning | status |
|---|---|---|
| **`B1` (current, S3/journal 042)** | the **pose-match precondition on A56** — `CALIB_BBOX`, `POSE_TOL_PX 8.0` | **live**, ~18 mentions in `CLAUDE.md` alone, cited in journals 042/044/045 and in `run_leg.ps1` / `check_pose.py` |
| **`B1` (older, m22 plan — journals 028/029, `CHAT-HANDOFF-m22-and-sve-s1.md`)** | **"B1 traceability"** — *"`nodes[]` gains `asset_name`, `component_class`, and bounds"* | **SHIPPED at m22** (`03a51d5`) and closed |

⚠ **They are not in conflict today only because the older one is finished.** Any brief that says *"the
B1 work on the client's invisible-anomaly complaint"* is ambiguous **right now**.

**PROPOSED (not adopted — the owner picks):** keep **`B1` = the pose-match precondition** (live, widely
cited, in committed tool source where a rename costs real churn), and retire the older label to
**`m22-B1-traceability`** wherever it is referenced. ⛔ **Not renamed unilaterally.**

### 67.2 What the label path HAS at emission time

`ResolveNodeIdentity` runs at the anchor with the `AActor*` in hand. **Already emitted** in
`nodes[]`: `name`, `path`, `global_position`, **`asset_name`**, **`component_class`**, `bounds`.

⚠ **`asset_name` and `component_class` ALREADY EXIST — they are exactly the m22-B1 traceability
fields.** On this very leg they read `SM_Bush` / `FoliageInstancedStaticMeshComponent`, which **names
the culprit class outright.**

⛔ **WHY THEY WERE INSUFFICIENT FOR THE OWNER'S INSPECTION IS NOT ESTABLISHED FROM HERE, AND IS NOT
GUESSED.** Candidate explanations — the client build predating m22, a viewer that surfaces only
`name`, or the fields being present but not looked at — are **distinguishable only by asking which
fields her copy actually shows.** **That question is the cheapest next step and it is the owner's to
ask.**

**HAS but does not write:** the **component's own name** (only its class); **instance count** for an
ISM/HISM; **which selection predicate admitted it**; the per-component bounds the projector actually
used (as distinct from `node.bounds`); and **actual drawn-pixel contribution** — which is
`feature/stencil-capture`'s premise, **untouched**.

### 67.3 A minimal addition, with the contract flag raised LOUDLY

🚨 **ANY of these ADDS FIELDS TO `annotation.json`, which is the client-facing contract. That makes it
a MILESTONE CANDIDATE and `P6` TERRITORY — NOT an in-turn change.** Ordered cheapest-first:

1. **`nodes[].instance_count`** — an integer, only meaningful for ISM/HISM. **Would have named class
   (ii) outright.**
2. **`nodes[].component_name`** — disambiguates two components on one actor.
3. **`nodes[].render_bounds`** — the SM/SK union the projector used, beside the existing whole-actor
   `bounds`. **This is the P6 adjacency and must not be smuggled in under traceability.**

⛔ **Nothing implemented. No field added.**

## 68. State after PART NINE

| | |
|---|---|
| plugin production code | **ZERO lines, across all nine parts** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED** — *its premise is the cure for H4, and H5 may need a different one* |
| `GameDefaultMap` unchanged · `CB_GateLevel` untouched (G99) | |
| build | unchanged — `exe 101AFEA4` + `utoc 939B9C9B` |
| journal | **renamed** to `…-045-h4-cook-and-h5-mainworld-arc.md`, **part index added** |
| new | **`H5` minted** · **G124** · one new instrument `h5_pixel_change.py` |
| path (a) | **PARKED, NOT REFUTED** — a priority decision, not a scope one |

---
---

# PART TEN — traceability characterised, G124 generalises, and a marker contaminated my last numbers

📋 **NUMBERING, FLAGGED NOT RESOLVED SILENTLY: the brief called this PART ELEVEN. The record has parts
One–Nine and no PART TEN.** Numbered **TEN** to keep the sequence contiguous, because a hole with no
PART TEN is exactly what confuses a cold reader. **Say the word and it renumbers.**

## 69. ⚠ FIRST — A CORRECTION TO PART NINE. A HARNESS MARKER CONTAMINATED THE GRID.

PART NINE reported the foliage grid as *"FOUR cells carry the change (0.1800, 0.1510, 0.1228,
0.0860) — the peak EXCEEDS the control's whole-bbox score."*

**Two of those four cells were not the foliage. They were the CaptureBench frame-identity marker.**

Found by running a second MainWorld leg and noticing the two grids shared a signature:

| leg | level | target | top-row cells (0,1) and (0,2) |
|---|---|---|---|
| `H5_MW_H5_FOLIAGE` | MainWorld | foliage | **0.1800, 0.1510** |
| `H5i_SPAWNPAD` | MainWorld | `BP_SpawnPad_C` | **0.1797, 0.1528** |
| `PC_POSTCOOK_SMOKE` | **CB_GateLevel** | `StaticMeshActor_49` | **0.1808, 0.1570** |

**Three levels, three targets, the same two cells at the same magnitude.** `run_leg.ps1` passes
`CaptureBench.Marker 1` by default and `CaptureBench.Marker.Top` is `0.80` of the half-height — near
the top edge. **The marker encodes frame identity, so it changes every frame BY CONSTRUCTION**, which
is precisely what a claimed-vs-flank differencer measures.

**Corrected at source, not masked in analysis** — the leg was re-run with `-Marker 0`:

| | with marker (PART NINE) | **marker OFF (corrected)** |
|---|---|---|
| whole-frame mean \|Δ\| | 0.0069 | **0.0059** |
| grid peak | 0.1800 *(the marker)* | **0.1242** *(real)* |
| top row | 0.1800, 0.1510 | **0.0018 – 0.0072** |
| per-cell mean | 0.0149 | **0.0095** |

⛔ **WITHDRAWN: *"the peak exceeds the control's whole-bbox score."*** That sentence was about the
marker. **The honest comparison is grid-peak to grid-peak: foliage 0.1242 against the control leg's
0.5515.**

✅ **THE CLASS (ii) CONCLUSION IS UNCHANGED AND STRENGTHENED.** The label still claims **100 % of the
frame**; the real change is **smaller** than reported (0.0059 vs a proper hide's 0.1023–0.1116, ≈ 5 %)
and **more concentrated** — 3 cells above 0.04, ~55 of 64 below 0.01. **Removing a contaminant that
inflated my numbers made the finding stronger, not weaker.** → **G125**.

## 70. OWNER OBSERVATION 2, with its provenance

🧾 **OWNER OBSERVATION — real evidence, eyeball-level, NOT MEASURED:**
> *"`asset_name` and `component_class` obviously exist; they show properly when tested in the EDITOR,
> but in BUILDS it shows `StaticMeshActor_xxx` for most objects."*

⚠ **This contradicted a MEASURED point** — the H5 foliage leg ran on the **packaged** build and
reported `asset_name SM_Bush`, `component_class FoliageInstancedStaticMeshComponent`. **The
contradiction was the lead**, and it resolves cleanly.

## 71. TASK 1 — THE DEGRADATION, CHARACTERISED. **It is `node.name`, and ONLY `node.name`.**

### 71.1 From source: how each field is populated

| field | populated by | editor vs build |
|---|---|---|
| **`node.name`** | `Ev->NodeName = F.Target` → `FAutoLiveFire::TargetName` → **`AActor::GetName()`** — the **internal object name**, **not** `GetActorLabel()` | **IDENTICAL.** Not `WITH_EDITOR`-guarded |
| `node.path` | `FActor->GetPathName()` | **IDENTICAL** |
| `asset_name` | first **visible** `UMeshComponent`'s `GetStaticMesh()`/`GetSkinnedAsset()` → `GetName()`; **empty** if that pointer is null or no visible mesh component exists | **IDENTICAL** |
| `component_class` | that component's `GetClass()->GetName()`; set **even when the asset does not resolve** | **IDENTICAL** |

🚨 **NOTHING in `ResolveNodeIdentity` IS `WITH_EDITOR`-GUARDED. There is NO editor-versus-build branch
in the population code at all.** The only fallback is `continue` on `!Mesh->IsVisible()`, which would
leave **both** `asset_name` and `component_class` empty — and the sweep shows that never happens here.

### 71.2 From banked data — **1,267 node entries across 109 banked packaged legs**

**Every distinct node identity in the bank:**

| level | `node.name` | `asset_name` | `component_class` |
|---|---|---|---|
| CB_GateLevel | **`StaticMeshActor_49`** | `Cube` | `StaticMeshComponent` |
| CB_GateLevel | **`StaticMeshActor_73`** | `Cylinder` | `StaticMeshComponent` |
| CB_GateLevel | **`StaticMeshActor_85`** | `Cone` | `StaticMeshComponent` |
| CB_GateLevel | **`StaticMeshActor_100`** | `Cone` | `StaticMeshComponent` |
| MainWorld | `SM_Ramp2_UAID_…` | `SM_Ramp` | `StaticMeshComponent` |
| MainWorld | `BP_MovingPlatform_C_UAID_…` ×4 | `SM_Modules_Platform` | `StaticMeshComponent` |
| MainWorld | `BP_Stomper_C_UAID_…` | `SM_Fan_Frame` | `StaticMeshComponent` |
| MainWorld | `BP_SpawnPad_C_UAID_…` | **`Plane`** *and* **`SM_SpawnPad_Base`** | `StaticMeshComponent` |
| MainWorld | `BP_SplineSpawn_C_UAID_…` | `SM_GenericPlane` | **`InstancedStaticMeshComponent`** |
| MainWorld | `RoomBuilderSquare_C_UAID_…` | `SM_FloorBase` | **`InstancedStaticMeshComponent`** |
| MainWorld | `InstancedFoliageActor_0_0_0` | `SM_Bush` | `FoliageInstancedStaticMeshComponent` |

**`asset_name` populated 15 / 15. `component_class` populated 15 / 15. ZERO empty.**
⇒ ⛔ **THEY DO NOT DEGRADE IN BUILDS.**

⚠ **A CLASSIFIER ERROR OF MINE, CORRECTED BEFORE REPORTING.** My first sweep regex
`^[A-Za-z_][A-Za-z0-9_]*?_\d+$` classified `BP_MovingPlatform_C_UAID_B42E9936F542EBDB00_1649270448`
as **GENERIC**, because it ends in digits. It printed *"MEANINGFUL 0"* for every level, which is an
artifact of the classifier and **not a finding**. The table above is the raw data instead.

### 71.3 What actually degrades, and why

**`node.name` is `GetName()` — the internal object name — and its quality depends on HOW THE ACTOR WAS
AUTHORED, not on editor-versus-build:**

- **`CB_GateLevel`**: authored by `make_gate_level.py`, which calls `a.set_actor_label(name_hint)` and
  **never sets the object name**. So `GetName()` stays **`StaticMeshActor_<n>`** while the label reads
  `CB_Target_NN`. **In the editor you see the label. In a build labels do not exist.**
- **`MainWorld`**: editor-placed and Blueprint instances — `GetName()` is already meaningful.

✅ **MEASURED CONFIRMATION FROM THIS SESSION'S OWN LOG:** `IAI.ListActors` on the packaged build printed
**`(no-label)` for all 432 MainWorld actors.** Labels are gone in a cooked build — which is exactly why
`CLAUDE.md`'s invariant says **matching is label-free** and `ListActors` guards the label behind
`WITH_EDITOR`.

⇒ **THE OWNER'S OBSERVATION IS EXPLAINED AND REFINED.** She is comparing **the editor's LABEL** against
**the build's `GetName()`**. For script-spawned or never-renamed actors those differ completely. **It
is not a field-population defect; `asset_name` and `component_class` are intact in builds.**

### 71.4 CONSEQUENCE FOR THE CLIENT'S DATA — stated plainly

> **PARTIALLY YES.** The remaining invisible cases **CAN** be attributed to a culprit **class** from the
> fields as they ship today — **via `asset_name` and `component_class`, which are populated in builds** —
> **but NOT from `node.name` alone**, which for many actors carries nothing beyond the class.
>
> For the owner's two named culprits: `component_class` would read `InstancedStaticMeshComponent`-family
> for `InstancedMeshActor`, and for a `BP_LocalVolumetricFog` the **Blueprint instance's `GetName()` is
> itself meaningful** and `component_class` names the component.
>
> ⛔ **What CANNOT be done today: distinguish two instances of the same class**, or tell **which
> selection predicate admitted** a given actor.

⚠ **A SECOND FINDING FROM THE SWEEP, WORTH ITS OWN LINE: `BP_SpawnPad_C` reports TWO DIFFERENT
`asset_name`s across legs — `Plane` and `SM_SpawnPad_Base`.** `ResolveNodeIdentity` takes the **first
VISIBLE mesh component**, and that Blueprint toggles component visibility at runtime (`SetVisibility`
found in the asset). **So the identity fields are NON-DETERMINISTIC for actors with runtime-toggled
mesh components.** ⛔ **Recorded, not fixed. `P6` DOES NOT MOVE.**

## 72. OWNER RULING — foliage DROPPED. **And the scope of that ruling is narrow.**

> **APPLIES TO: the investigation.** No more foliage legs; foliage is not the study object.
> **DOES NOT APPLY TO: selection, or any cure. NOTHING is excluded from the selector, and a class
> blacklist is NOT adopted as a fix.**

🚨 **WHY THE DISTINCTION IS LOAD-BEARING, now with a measurement behind it: G124's mechanism is
AGGREGATE / OVERSIZED BOUNDS, not foliage-as-a-class.** §73 shows a **plain `StaticMeshComponent`**
with the same collapse. **A fix that blacklisted `InstancedFoliageActor` would leave the identical hole
open for every other actor whose bounds sphere exceeds the poll radius, while looking closed.** That is
the difference between fixing the mechanism and hiding the one instance we happened to find.

## 73. TASK 2 — **G124 GENERALISES. 3 of 13 non-foliage selectable actors.**

The deciding quantity is `selection_provenance`'s **`poll_distance`**, which **is**
`dist(pollOrigin, B.Origin) − B.SphereRadius` on the first renderable-visible component. **A NEGATIVE
value means the component's bounds sphere already contains the poll origin ⇒ the 1800 cm cull can never
fire, from anywhere in the level.**

⚠ **`node.bounds` cannot substitute** — it is `GetComponentsBoundingBox(true)`, the whole actor, a
different quantity. Measured: `BP_MovingPlatform`'s whole-actor box implies a ~2620 cm sphere while its
**measured** `poll_distance` is **+1370…+1544**. The whole-actor box does not drive the cull.

| actor | component_class | `poll_distance` | `coverage_pct` | occ | rect % |
|---|---|---|---|---|---|
| **`BP_SpawnPad_C`** | **`StaticMeshComponent`** | **−114.8 … −52.6** 🚨 | 11.94–19.79 | 4/9, 5/9 | 22.8 |
| **`BP_SplineSpawn_C`** | `InstancedStaticMeshComponent` | **−19405.5** 🚨 | 3.86–22.89 | 3/9, 4/9 | 22.9 |
| **`RoomBuilderSquare_C`** | `InstancedStaticMeshComponent` | **−1737.8** 🚨 | 21.67–23.52 | 3/9, 4/9 | 82.9 |
| `BP_MovingPlatform_C` (near) | `StaticMeshComponent` | +1370.5 … +1544.0 | 0.41–2.77 | 5/9, 6/9 | 2.8 |
| `SM_Ramp2` | `StaticMeshComponent` | +389.9 | 5.87–7.03 | 5/9 | 7.1 |
| `StaticMeshActor_49 / 73 / 85` | `StaticMeshComponent` | +418.1 / +346.0 / +444.9 | 0.49–8.84 | 9/9 | 6.6–9.3 |

**⇒ THE ANSWER: 3.** And the most important row is the first one:

🚨 **`BP_SpawnPad_C` IS A PLAIN `StaticMeshComponent`.** It is **not** instanced, **not** foliage, and
it still collapses the cull. **G124 is about OVERSIZED BOUNDS, and aggregation is only the most common
way to get them.** Any instanced-component blacklist would miss this actor entirely.

⚠ **Actors whose `poll_distance` is the −1 SENTINEL are NOT evidence either way** — provenance returned
`valid:false` at that anchor, so no distance was ever computed. **UNMEASURED, not small.** ⚠ Also
noted: a genuine `poll_distance` of exactly −1.0 would be indistinguishable from the sentinel. Not
observed; stated because the tool cannot tell them apart.

⚠ **Every one of the three negatives also sits in the `P-a1` band** — 3/9 and 4/9 rays clear, all
`valid:true`. ⛔ **Corroboration, not a test. `P-a1`…`P-a5` REMAIN UNTESTED.**

## 74. TASK 3 — CLASS (i). **BRANCH: SELECTED BUT MANIFESTS.**

**Candidates enumerated with reasons BEFORE firing** (pre-registered as a file — no fishing):

1. **`BP_SpawnPad_C` — strongest, two independent reasons.** (a) the Blueprint asset contains
   `SetVisibility` + `bVisible` ⇒ it toggles a mesh component's visibility at runtime; (b) banked data
   shows **the same actor reporting two different `asset_name`s** (`Plane`, `SM_SpawnPad_Base`), which
   can only happen if the *first visible* mesh component resolved differently. Also **negative
   `poll_distance`**.
2. `BP_Button_C` — same markers, no banked identity split. Second choice.
3. `BP_SplineSpawn_C` — **excluded and the exclusion recorded**: rect 22.9 % of frame, it clearly draws.

**Result on `BP_SpawnPad_C`:** selected via `Plane` / `StaticMeshComponent`, provenance `valid:true`
**9/9**, `poll_distance −123.4`, bbox 22.6 % of frame, `coverage_ratio 0.181`.

**It manifests, and the label points at it correctly.** In-bbox Δ reaches **−0.0355 … −0.0367** on the
settled events, and the **two brightest cells in the whole frame — 0.1861 and 0.1920 — are INSIDE the
bbox**, at the pad's location. *(The 0.1797/0.1528 cells outside it were the marker, §69.)*

> **BRANCH: SELECTED BUT MANIFESTS.** ⛔ **Why the prediction was wrong for it:** the visibility toggle
> is real, but the component that was live at pick time was a **drawn** one. The `Plane`/`SM_SpawnPad_Base`
> split proves *a* component is sometimes invisible — it does **not** show that the *selected* one
> draws nothing. **I read "has a toggled component" as "the toggled component is the selected one",
> and those are different claims.**

⇒ **`H5` class (i) remains ENUMERATED, NOT OBSERVED.** ⚠ **StackOBot is a polished sample project and
may simply not contain the pattern. That is a property of THIS PROJECT, not evidence against class (i)
in the client's game.**

## 75. TASK 4 — the ledger

**`docs/invisible-anomaly-mechanisms.md`**, referenced from `CLAUDE.md`. Five rows: `m23`/`P3`
(**FIXED**), `H4` (**SUPPORTED**, cure = `feature/stencil-capture`), `H5` (ii) (**SUPPORTED,
reproduced, generalises**), `H5` (i) (**ENUMERATED, NOT OBSERVED**), and **traceability degradation —
marked explicitly as NOT A CAUSE**, but as what prevents attributing the others. Each row states
**MEASURED vs SOURCE-READ**, its evidence pointer, its limits, and its cure if known. It opens with:
**these are DISTINCT mechanisms with potentially DISTINCT CURES, and no single fix is known to address
all of them.**

## 76. State after PART TEN

| | |
|---|---|
| plugin production code | **ZERO lines, across all ten parts** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED** |
| `P6` | **DOES NOT MOVE** — no field added, removed, renamed or recomputed |
| `GameDefaultMap` unchanged · `CB_GateLevel` untouched (G99) | |
| build | unchanged — `exe 101AFEA4` + `utoc 939B9C9B` |
| new | **G125** (the marker contaminant) · `docs/invisible-anomaly-mechanisms.md` · two sweep tools |
| corrected | PART NINE's foliage grid numbers, at source (`-Marker 0`), not masked |

---
---

# PART ELEVEN — the cure measurement. A distribution sketch, n small, nothing graded.

⛔ **NO CODE. NO CURE DESIGNED OR PROPOSED. NO A54 VERDICT. `P6` DOES NOT MOVE.**
*(Numbering: the owner confirmed PART TEN was correct and this is genuinely ELEVEN.)*

## 77. 🚨 A DEFECT I FOUND IN MY OWN HARNESS, AND THE ARTIFACT THAT EXPOSED IT

The table tool **silently swallowed a leg**. Chasing that rather than ignoring the gap found this:

`CM_SPLINE` produced **88 PNG files, 7 of them ZERO BYTES** (indices 80, 82–87) — while
`run_summary.total_frames` read **76**, `labels.jsonl` had **77 rows**, `key_ring` read **121/121/0/0**,
`speed_ratio` **1.0000**, and **nothing was logged.** Every health counter clean, the artifact
internally inconsistent, the disagreement undisclosed.

**CAUSE: MY HARNESS, NOT THE PRODUCT.** `run_leg.ps1` waited for `run_summary.json` to appear and then
killed the process **immediately** — but the summary is written at run *finish* while the **async frame
writer is still flushing**. The kill truncated the tail, which is exactly why the zero-byte frames are
the **highest indices**.

✅ **DEMONSTRATED BOTH WAYS.** A flush-wait was added — poll until the frame count *and* the zero-byte
count stop changing — and the leg re-run:

| | without the wait | **with the wait** |
|---|---|---|
| frames on disk | 88 | **90** |
| zero-byte | **7** | **0** |
| `run_summary.total_frames` | 76 | **90** |

⚠ **What survives as a product-side OBSERVATION, stated narrowly:** an interrupted run leaves an
artifact whose counters disagree with its files **with no disclosure**. The `GetDropped() > 0` warning
never fires because the writer never gets to count them. ⛔ **Not a defect claim** — a force-killed
process cannot be expected to reconcile. **Recorded because a delivered session could in principle be
interrupted the same way and would look clean.**

🔁 **This is m19's lesson recurring: gate on PIXELS, not on a counter.** Here the counters were
*self-consistent and wrong*.

**Two harness fixes landed** (`CaptureBench`, local-only): the flush-wait above, which also **warns
loudly if zero-byte frames remain**; and the table tool now **reports every skipped leg with its
reason** instead of `except: continue`. ⚠ **The silent skip is what hid seven corrupt frames** — and
the skip list immediately showed the other skips are **delivery-mode legs with no `labels.jsonl`**,
which is correct and expected.

## 78. THE TABLE — marker OFF, deduped, 7 distinct targets

⛔ **94 of 110 banked legs are MARKER-ON and are EXCLUDED from every pixel figure below** (G125), not
silently mixed.

| target | component | rect % | `cov_pct` | `poll_dist` | occ | whole \|Δ\| | peak IN | peak OUT |
|---|---|---|---|---|---|---|---|---|
| `RoomBuilderSquare_C` | `InstancedStaticMesh` | 82.9 | 21.67 | **−1737.8** | 5/9 | **0.04413** | 0.2030 | 0.0027 |
| `SM_Ramp2` | `StaticMesh` | 5.9 | 5.87 | +389.9 | 5/9 | 0.01105 | 0.1785 | **0.2955** |
| **`StaticMeshActor_49`** *(control)* | `StaticMesh` | 7.8 | 7.80 | +418.1 | 9/9 | 0.00750 | **0.5515** | 0.0086 |
| `InstancedFoliageActor_0_0_0` | `FoliageInstancedStaticMesh` | **100.0** | **100.00** | **−5396.0** | **1/9** | 0.00603 | 0.1242 | 0.0000 |
| `BP_SpawnPad_C` | `StaticMesh` | 17.0 | 13.24 | **−123.4** | 9/9 | 0.00515 | 0.1264 | 0.0383 |
| `BP_MovingPlatform_C` | `StaticMesh` | 0.5 | 0.57 | +1370.7 | 6/9 | 0.00249 | 0.0148 | 0.0295 |
| `BP_SplineSpawn_C` | `InstancedStaticMesh` | 22.9 | 3.86 | **−19405.5** | **3/9** | **0.00192** | **0.0175** | 0.0149 |

⛔ **(a) THE SELECTION BOUNDS EXTENT IS NOT RECOVERABLE FROM ANY ARTIFACT.** The guards use SM/SK
`Component->Bounds`; `node.bounds` is `GetComponentsBoundingBox(true)` (whole actor — `P6`); and
`poll_distance = dist − sphereRadius` is **one equation in two unknowns**. **The quantity every guard
is computed from is not recorded.** That is itself a finding about the cure.

### 78.1 ⚠ A SECOND CLASS-(ii) INSTANCE — and a call of mine it overturns

**`BP_SplineSpawn_C` claims 22.9 % of the frame and hiding it changes essentially nothing anywhere** —
peak IN **0.0175**, peak OUT 0.0149, whole-frame **0.00192**. Against the control's peak IN of
**0.5515**, that is **31× smaller**.

🚨 **LAST TURN I EXCLUDED IT FROM THE CLASS-(i) CANDIDATE LIST ON THE GROUNDS THAT "it clearly draws",
citing its 22.9 % rect. THAT WAS AN INFERENCE FROM RECT SIZE, NOT A MEASUREMENT, AND IT WAS WRONG.**
Rect size is *claimed* extent — the very quantity `H5` says is untrustworthy — so using it to conclude
"it draws" assumed away the hypothesis. **Same error family as the class (i) prediction: I substituted
a claim for a measurement.**

⇒ **`H5` class (ii) now has n = 2 measured instances**, and — importantly — **one is NOT foliage.**

### 78.2 `SM_Ramp2` — an A35 case, on the record

**peak OUT 0.2955 > peak IN 0.1785**, with the marker OFF. The largest change from hiding the ramp is
**outside its own bbox** — consistent with A35 (hiding an object removes its cast shadow). ⛔ Not
investigated; recorded because any in-bbox-only rule would score this legitimate target low.

## 79. Q1 — IS THERE A SEPARATION?

**GOOD/BAD assigned by OUTCOME**, then asked whether anything **available before the hide** predicts it.
**BAD = `InstancedFoliageActor`, `BP_SplineSpawn_C` (n=2). GOOD = the other five.**

**The best discriminator found is POST-hide: change per unit claimed area (`whole|Δ|` ÷ `rect_frac`):**

| | value |
|---|---|
| **BAD** `InstancedFoliageActor` | **0.00603** |
| **BAD** `BP_SplineSpawn_C` | **0.00838** |
| GOOD `BP_SpawnPad_C` | 0.0303 |
| GOOD `RoomBuilderSquare_C` | 0.0532 |
| GOOD `StaticMeshActor_49` | 0.0962 |
| GOOD `SM_Ramp2` | 0.187 |
| GOOD `BP_MovingPlatform_C` | 0.498 |

**Clean separation, no overlap, a 3.6× gap.** ⛔ **But it requires the pixel measurement, and a cure
must decide BEFORE hiding. A discriminator that needs the measurement is not a discriminator.**

**Among PRE-hide quantities:**

| quantity | separates? | overlap |
|---|---|---|
| `cov_pct` | ❌ **NO** | BAD {100.00, **3.86**} straddles GOOD {0.57 … 21.67} entirely |
| rect % | ❌ **NO** | BAD {100.0, **22.9**}; GOOD `RoomBuilderSquare` is **82.9** |
| `poll_distance < 0` | ⚠ partial | catches **2/2 BAD** but also **2/5 GOOD** |
| ISM/HISM/Foliage class | ⚠ partial | catches **2/2 BAD** but also **1/5 GOOD** |
| **occlusion sample count** | ✅ **yes, in this sample** | **BAD ≤ 3/9 · GOOD ≥ 5/9**, no overlap |

⚠ **THE OCCLUSION-COUNT SEPARATION IS REAL IN THIS SAMPLE AND VERY WEAK AS EVIDENCE.** n=2 versus n=5
on a 9-valued integer; `SM_Ramp2` and `RoomBuilderSquare_C` sit at 5/9, one step from the boundary; and
the mechanism I can offer for it — an aggregate's rays go to cluster-AABB corners so more of them hit
something — is **post-hoc**. ⛔ **Reported as an observation with its n, NOT proposed as a threshold.**
*(It is also the `P-a1` band again: the targets that barely scrape past `IsUnoccluded` are the ones
whose labels are worthless. `P-a1`…`P-a5` REMAIN UNTESTED.)*

## 80. Q2 — WHAT IS THE RATIO THAT MATTERS? **The owner's prior is CONFIRMED.**

> **Prior, stated to be refutable: the informative quantity is DRAWN EXTENT relative to CLAIMED
> EXTENT, and no field currently carries drawn extent.**

✅ **CONFIRMED, and the check that could have refuted it failed.** The one plausible proxy from
existing fields is `cov_pct` — the *selection* bounds projected — and §79 shows it **does not separate
at all** (BAD holds both the highest value in the table, 100.00, and nearly the lowest, 3.86). Bounds
volume ÷ label rect is **not computable**: the numerator is the un-recorded quantity of §78(a).

**Every field the artifact carries — `cov_pct`, `coverage_ratio`, `bbox_px`, `poll_distance`,
`node.bounds` — describes CLAIMED extent. None describes DRAWN extent.**

⇒ 🚨 **THE ANSWER IS "NO USABLE PROXY EXISTS", AND THAT MEANS THE CURE NEEDS A NEW MEASUREMENT RATHER
THAN A NEW THRESHOLD.** The brief called either result complete; this is the one that obtained.
⛔ **No cure proposed.** *(Noted without acting on it: that conclusion has the same shape as
`feature/stencil-capture`'s premise — report actual pixel contribution. **That branch is H4's cure and
is UNTOUCHED**; whether one measurement could serve both is not established and is not claimed.)*

## 81. Q3 — HOW MANY LEGITIMATE TARGETS WOULD A NAIVE FIX BREAK?

Over the 7 measured targets — **5 GOOD, 2 BAD**:

| candidate rule | BAD caught | **GOOD broken** | which GOOD |
|---|---|---|---|
| **reject if `poll_distance` < 0** | 2 / 2 | **2 / 5 (40 %)** | `RoomBuilderSquare_C` (−1737.8), `BP_SpawnPad_C` (−123.4) |
| **reject if `cov_pct` > 90** | 1 / 2 | 0 / 5 | — *(but misses `BP_SplineSpawn_C` entirely)* |
| **reject if `cov_pct` > 20** | 1 / 2 | 1 / 5 | `RoomBuilderSquare_C` (21.67) |
| **reject if `cov_pct` > 3** | 2 / 2 | **4 / 5 (80 %)** | all but `BP_MovingPlatform_C` |
| **reject bounds-volume ÷ rect > X** | — | — | ⛔ **NOT COMPUTABLE** — the numerator is not recorded (§78a) |
| **blacklist ISM / HISM / Foliage** | 2 / 2 | **1 / 5 (20 %)** | `RoomBuilderSquare_C` |

**`cov_pct` has NO value that catches both BAD without breaking GOOD** — the curve is in the three rows
above and it is monotone in the wrong way.

### 81.1 ⚠ THE STRAWMAN FAILED DIFFERENTLY THAN PREDICTED — reporting the table, not the argument

> The brief said: *"the blacklist … I expect it to miss `BP_SpawnPad_C` and I want that in the table
> rather than in my argument."*

**The table does not say that.** `BP_SpawnPad_C` **manifests correctly and is GOOD**, so a rule that
does not reject it is behaving *correctly*, not failing. In this sample the blacklist **catches both
BAD instances**, and its actual failure is **rejecting `RoomBuilderSquare_C`, a legitimate target that
produces a strong, correctly-located change** (peak IN 0.2030 against peak OUT 0.0027, a 75:1 ratio).

⛔ **The blacklist is still not a fix** — and now for a reason the measurement supports rather than one
argued from principle: **it is the second-most damaging rule in the table to legitimate targets, and
its apparent success at catching BAD rests on n=2 where both happen to be instanced.** `BP_SpawnPad_C`'s
role in the argument is different from the one predicted: it shows that **a plain `StaticMeshComponent`
can have a bounds sphere that defeats the poll cull** (§73) — which is why the *poll-distance* rule
breaks it, not why the blacklist would.

## 82. State after PART ELEVEN

| | |
|---|---|
| plugin production code | **ZERO lines, across all eleven parts** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED** |
| `P6` | **DOES NOT MOVE** — third observation recorded in the ledger, not fixed |
| build | unchanged — `exe 101AFEA4` + `utoc 939B9C9B` |
| new | `cure_measurement_table.py` · harness flush-wait · skip-reporting |
| corrected | my own exclusion of `BP_SplineSpawn_C`; the harness-induced zero-byte frames |
| ⛔ not done | **no cure designed or proposed** — chat-side, next brief |

---

# PART TWELVE — the cure OPTION SPACE, costed from source; and the disk prune

**NO RUN. NO IMPLEMENTATION. NO PROTOTYPE. NO BRANCH WORK. ZERO PRODUCTION CODE — now across TWELVE
parts. NO TAG. `P6` DOES NOT MOVE. `feature/stencil-capture` READ-ONLY, never checked out.**

Two tasks: cost the five candidates the owner named, and prune the disk with a verify-before-delete
pass.

---

## 83. Rulings recorded before anything else

**RULING 1 — the `H4`/`H5` shared cure is a HYPOTHESIS, not a finding.** Recorded verbatim in the
ledger **§3.3**. The resemblance between `feature/stencil-capture`'s premise and `H5`'s required
measurement is **attractive precisely because it would collapse two mechanisms into one fix, and that
is the reason to test it rather than assume it.**

**RULING 2 — `SM_Ramp2` is an A35 case and it CONSTRAINS the cure.** Recorded in the ledger **§3.2**.
peak-OUT **0.2955** > peak-IN **0.1785**, marker-off, on a legitimate target ⇒ **any candidate whose
measurement is bbox-scoped inherits this failure mode and must say so.**

**The `n=2` update and the unrecoverable-bounds finding were already in the ledger** (§3 `n` row,
§3.1) — verified present, not re-written.

---

## 84. The two architectural facts that price everything → `G127`

Before any candidate can be costed, two properties of the codebase decide what is even reachable.

**(1) THE FILTER AND THE PIXELS ARE IN DIFFERENT MODULES, AND ONE DOES NOT EXIST IN SHIPPING.**

| | module | deps | Shipping? |
|---|---|---|---|
| `IsRenderableComponent`, `IsUnoccluded`, selector, anomalies | **`AnomalyInjector`** | `Core`/`CoreUObject`/`Engine`/`InputCore` — **no render deps** | ✅ **builds** |
| SVE, readback, every pixel measurement | **`AnomalyCapture`** | `Renderer`/`RHI`/`RenderCore`/`Slate` **+ Renderer PRIVATE include path** | ⛔ **`ANOMALY_CAPTURE=0`** |

`AnomalyCapture.Build.cs:24-43`. ⇒ *"have the selector ask the mask"* is a **module-shape decision**.

**(2) A PIXEL MEASUREMENT CANNOT INFORM A SAME-FRAME PICK-TIME DECISION.** The SVE runs on the render
thread after post-processing; results return by **async GPU readback**
(`AnomalySveCapturer::Drain_RenderThread` polls `IsReady()` and skips when not ready). The stencil
branch budgets **12 frames** before abandoning a mask (`HeldAges[i] > 12`).

⇒ 🚨 **Any pixel-derived cure is a PRE-FLIGHT — arm, wait, decide — never a predicate the selector
calls inline.** A design that forgets this silently decides on last frame's answer.

---

## 85. C-1 — stencil / ID-buffer pixel count. **READ-ONLY inspection of `feature/stencil-capture`.**

Read with `git show`/`git log`/`git diff master...`. ⛔ **Never checked out, merged, rebased or
modified.** Tip **`76cac74`**; stencil work `468ed6b` → `b39c0d0` → `76cac74`; diff vs `master`
**21 files, +1283/−48**.

### 85.1 What it already implements — and it is a long way in

| piece | file | what it does |
|---|---|---|
| the test | `Shaders/Private/AnomalyVisibleMask.usf` | `customStencil ≥ ReservedBase` **∧** `customDeviceZ ≥ sceneDeviceZ − bias` (reversed-Z) → writes the tag, else 0 |
| the pass | `AnomalyStencilSceneViewExtension.cpp` | full-screen PS after **`EPostProcessingPass::Tonemap`**, R8_UINT RT at ViewRect size, async readback |
| **the number** | `AnomalyMaskTypes.h` | **`FAnomalyMaskAABB{MinX,MinY,MaxX,MaxY, Count}` per tag** |
| tagging | `AnomalyStencilTag.cpp` | saves/restores each primitive's prior `{bRenderCustomDepth, CustomDepthStencilValue}` **exactly**; refcounts `r.CustomDepth 3` |
| the join | `AnomalyCaptureSubsystem` | `LOCK-1`: hide-type ⇒ projected box; render-type ⇒ stencil box; mask armed only when ≥1 fire is rendering |

🚨 **`Count` IS the surviving-pixel count `C-1` needs, and it already exists.** The branch's premise
is not aspirational — the measurement is built.

### 85.2 Three things the costing must carry

1. 🚨 **Its last commit "excludes `InstancedFoliageActor`" — the class blacklist the owner has RULED
   IS NOT A FIX.** Worse, the code comment justifying it asserts *"standalone ISM/HISM crates have
   sane bounds + respect hide and stay eligible (G59)"* — **now MEASURED FALSE**: `BP_SplineSpawn_C`
   is a standalone ISM at **−19405.5**, and `BP_SpawnPad_C` is a **plain SMC** at **−114.8**.
   ⇒ **the branch's own scoping rests on a premise `H5` has refuted.**
2. 🚨 **A NARROWING DEFECT, failing in the dangerous direction.** `AnomalyStencilTag`'s
   `IsRenderableMesh` tests **`USkeletalMeshComponent`**; `master`'s `IsRenderableComponent` tests the
   **base `USkinnedMeshComponent`**. A `USkinnedMeshComponent` that is not a `USkeletalMeshComponent`
   (e.g. `UPoseableMeshComponent`) is **selectable but never tagged** ⇒ **mask `Count` 0 on a target
   that draws** — read as *"contributes nothing"*.
3. ⚠ **An unpriced cost:** `Drain_RenderThread` reduces the mask with a **single-threaded W×H CPU scan
   ON THE RENDER THREAD**, per armed frame — 921,600 byte reads at 1280×720. **Not measured by
   anyone; flagged.**

### 85.3 Costing

**Measures:** occlusion-correct surviving pixel **count** and pixel **AABB** per tagged target.
**Where:** render thread, post-Tonemap. **Cost:** per-armed-frame full-screen pass + R8 RT + readback
+ that CPU scan; mutates target render state; forces `r.CustomDepth 3` globally.
**Cannot see:** anything outside the tagged silhouette — **shadows, i.e. `SM_Ramp2` / A35 (Ruling
2)** — and anything not tagged (defect 2 above).

---

## 86. C-2 — depth comparison. **The parked S4 depth work is DESIGN ONLY; nothing is implemented.**

Searched the docs: depth is *"`SceneDepthTexture` in `PrePostProcessPass_RenderThread`, FP32, plus the
typed FP16/FP32 path"*, **PARKED and UNNUMBERED** (journal 037 §109-110; the S4 handoff §460 confirms
*"S4 = depth" → no*). **No code exists.**

✅ **The hook is available:** `PrePostProcessPass_RenderThread(FRDGBuilder&, const FSceneView&, const
FPostProcessingInputs&)` is a real `ISceneViewExtension` virtual in 5.1
(`SceneViewExtension.h:171`), on the same SVE object `master` already registers. ⚠ **It is a
*different* hook from the one the colour grab uses** (`SubscribeToPostProcessingPass`), so this is an
addition, not a re-use.

**Measures:** the target's expected depth against rendered scene depth at its projected rect.
**Cost:** cheaper than C-1 — one depth read, **no tagging and no per-target pass**.

⛔ **WHAT IT CANNOT DISTINGUISH, and it is disqualifying for `H5`: its reference depth is derived from
the SAME bounds `H5` says are untrustworthy.** For `InstancedFoliageActor_0_0_0` the bounds centre
sits in a **252 m × 217 m × 67 m** box at a cell corner — **kilometres from any actual bush**. There
is no "the target's depth" for an aggregate. It also cannot tell *"my target is at that depth"* from
*"something else is at that depth"*.

---

## 87. C-3 — `WasRecentlyRendered()`. Cheapest by far, and it answers the OTHER question. → `G126`

**It is not one signal.** `LastRenderTime` is written from three places with two meanings:

| writer | `bUpdateLastRenderTimeOnScreen` | meaning |
|---|---|---|
| `SceneVisibility.cpp:2493` | **true** | visible **and** `PrimitiveDefinitelyUnoccludedMap` set |
| `ShadowSetup.cpp:1672` | **false** | **shadow-casting pass** |
| `ShadowSetup.cpp:1909` | **false** | ditto |

`AActor::WasRecentlyRendered` (`Actor.cpp:1989-2000`) reads `LastRenderTime` — which **the shadow path
bumps**. ⇒ 🚨 **a shadow-only contributor reads "recently rendered"**, and `SM_Ramp2` is exactly that
shape. The finer field `LastRenderTimeOnScreen` exists, is bumped only by the `true` path, and
**`WasRecentlyRendered()` does not use it.**

**Latency, both stacked:** render thread *"up to a frame behind the game thread"*
(`PrimitiveComponent.h:810-814`) **plus** occlusion-query buffering
(`FOcclusionQueryHelpers::GetNumBufferedFrames`). *"Recently"* is a **tolerance**, never an instant.

**Does NOT account for:** how much was drawn (binary); LOD/HLOD substitution; **per-instance**
visibility — an ISM is one primitive, so 1 visible instance of 10,000 reads identically to all
10,000. Distance culling **is** reflected (a culled primitive stops being bumped).

✅ **What it IS good for:** the `true` path is gated on `PrimitiveDefinitelyUnoccludedMap`, so it is
**genuinely occlusion-aware** — making it a candidate for **`H4`'s** question, not `H5`'s.

---

## 88. C-4 — per-instance / per-section bounds

✅ **The engine exposes it cheaply and with NO render deps:**
`UInstancedStaticMeshComponent::GetInstanceTransform(int32, FTransform&, bWorldSpace)` is public
(`InstancedStaticMeshComponent.h:173`) and reads `PerInstanceSMData`; combined with
`GetStaticMesh()->GetBounds()` it gives per-instance world bounds in O(instances), **game thread,
pure CPU**. `GetNumRenderInstances()` and overlap queries by sphere/box are public too.

**Addresses the aggregate half of class (ii) with NO pixel measurement at all** — its main appeal.

⛔ **WHAT IT DOES NOT FIX: a PLAIN component whose bounds exceed its mesh.** That is `BP_SpawnPad_C`,
**poll_distance −114.8, a plain `StaticMeshComponent`** — the measured instance that made `G124`
generalise. C-4 leaves it untouched. **4 of the 7 banked targets are plain SMCs and C-4 does not
apply to any of them.**

⚠ **Cost caveat:** O(instances) is cheap for a spline spawner and **not** obviously cheap for a
foliage container with tens of thousands of instances, evaluated per candidate per poll.

---

## 89. C-5 — render-relevant bounds (the LOCKED `P6` ruling)

**Measures:** *which components* count as "the object" — the union over components that contribute
drawn pixels, reusing `IsRenderableComponent` (**G33**). **Cost:** negligible, game thread.

🆕 **A REAL DEFECT IT WOULD FIX, found in source this turn.** The **label** path
(`ProjectActorBoundsToScreenRect`, `AnomalyViewport.cpp:653-685`) unions components on a **TYPE-ONLY**
test — `IsA<UStaticMeshComponent>() || IsA<USkinnedMeshComponent>()`, **no `IsVisible()` gate** —
while **selection** (`IsRenderableComponent`, `:493`) **does** check `IsVisible()`. **The label rect
and the selection set already disagree about what "the object" is.** An invisible mesh component
enlarges the box but cannot be selected through.

⛔ **BUT IT IS A NO-OP ON `H5`, and this is DERIVABLE.** All 7 banked targets were selected *through* a
component that already passed `IsRenderableComponent` — so `IsVisible()` was already true for it.
**C-5 changes WHICH components are unioned; it never changes HOW BIG a renderable component's bounds
are. An ISM's `Bounds` stay the whole cluster under C-5.**

**Relationship to C-4:** orthogonal and composable — **C-5 picks the components, C-4 redefines one
component's extent.** Neither substitutes for the other, and **C-5 alone fixes neither `H5` class.**

---

## 90. What each would have returned on the 7 banked targets

⛔ **Every row is marked DERIVABLE or NOT DERIVABLE. Nothing is estimated.**

| candidate | derivable? | result |
|---|---|---|
| **C-1** | ⛔ **NOT DERIVABLE** | No artifact carries a per-target pixel count. The nearest banked quantity, whole-frame \|d\|, is **contrast-weighted change, not area** — substituting it is the named error family. **One bound IS derivable:** on the foliage leg the change lived in **4 of 64** cells against a **100 %** claim. That bounds *where change occurred*, not the count. |
| **C-2** | ⛔ **NOT DERIVABLE** | Neither the depths nor a reference depth are recorded. |
| **C-3** | ✅ **DERIVABLE** | **TRUE on all 7 ⇒ 0/2 BAD caught, 0/5 GOOD broken.** All 7 passed selection (`IsVisible()` ∧ frustum ∧ ≥1 clear ray) and every hide produced a **non-zero localized** change (min 0.00192) ⇒ all 7 were drawing. **It separates NOTHING here** — correctly: it targets class (i), and this set has **no** class (i) instance. |
| **C-4** | ⚠ **PARTIAL** | Applies to **3 of 7**: `RoomBuilderSquare_C` (GOOD), `InstancedFoliageActor` (BAD), `BP_SplineSpawn_C` (BAD). **The 4 plain SMCs — including `BP_SpawnPad_C` — are untouched.** Whether it **breaks `RoomBuilderSquare_C`** is ⛔ **NOT DERIVABLE**: it depends on the reduction rule (union / largest / nearest), which no candidate specifies. |
| **C-5** | ✅ **DERIVABLE** | **0/2 BAD, 0/5 GOOD broken — a NO-OP** (§89). |

---

## 91. Question A — class coverage

| candidate | class (i) *non-drawing* | class (ii) *over-claiming* |
|---|---|---|
| **C-1** | ✅ zero surviving pixels | ✅ count ≪ claimed area |
| **C-2** | ⚠ partial (a null mesh writes no depth) | ⛔ reference depth comes from the bad bounds |
| **C-3** | ✅ **only** | ⛔ binary |
| **C-4** | ⛔ | ⚠ **aggregate half only** |
| **C-5** | ⛔ | ⛔ |

⇒ **C-1 is the ONLY candidate addressing BOTH classes.**

---

## 92. Question B — which could ALSO answer `H4`? **HYPOTHESIS, per RULING 1**

| candidate | hypothesis | uncertainty |
|---|---|---|
| **C-1** | **Plausible — and it is the branch's own premise.** The shader's test *is* an occlusion test, so zero surviving pixels covers *blocked* and *draws nothing* alike | ⚠ **THE TIMING DIFFERS.** `LOCK-1` takes the mask **while the target renders**, falling back once hidden. `H4` path (b) fires at an **already-occluded** target ⇒ mask 0 **with nothing to fall back to**. One measurement, two different moments. **UNESTABLISHED** |
| **C-2** | **Plausible for `H4`** — depth comparison *is* an occlusion test | ⛔ and it is the candidate that does **not** serve `H5` — an `H4`-only cure |
| **C-3** | **Plausible, and by far the cheapest** — gated on `PrimitiveDefinitelyUnoccludedMap` | ⚠ **`G126`**: shadow-only reads TRUE. Plus two stacked latencies |
| **C-4 / C-5** | ⛔ **no** — neither reads occlusion | — |

⛔ **No claim is made that one measurement serves both. The resemblance is recorded as a hypothesis
with its uncertainty, and `feature/stencil-capture` stays untouched.**

---

## 93. Question C — DELIVERY MODE. ⚠ LOAD-BEARING → `G128`

🚨 **THE PREMISE NEEDED CORRECTING FIRST: DELIVERY MODE GATES *REPORTING*, NOT *MEASUREMENT*.**

| line | what | delivery-gated? |
|---|---|---|
| `AnomalyCaptureSubsystem.cpp:1599` | `EvaluateSelectionProvenance(...)` | ⛔ **NO — unconditional** |
| `:1691` | `Out.CoveragePct = Ev.Provenance.CoveragePct` | ⛔ **NO** |
| `AnomalyLabelWriter.cpp:404` | `coverage_pct` → **`annotation.json`** | ⛔ **NO — both modes** |
| `AnomalyCaptureSubsystem.cpp:1720` | `selection_provenance.json` sidecar | ✅ **YES** |

⇒ **the provenance measurement ALREADY RUNS in delivery mode and one of its outputs ALREADY REACHES
THE CLIENT.** Only the internal sidecar file is suppressed.

| candidate | runs in delivery? | real constraint |
|---|---|---|
| **C-1** | ✅ | ⚠ **SHIPPING**, not delivery (`G127`) — and capture is already compiled out of Shipping, so a client capturing at all is on a non-Shipping build |
| **C-2** | ✅ | same |
| **C-3** | ✅ **most delivery-safe** | none — game thread, engine-only, needs no artifact if used as a gate |
| **C-4** | ✅ | none |
| **C-5** | ✅ | none |

✅ **NO CANDIDATE IS BLOCKED BY DELIVERY MODE.** *"A cure that only works with delivery off is not a
cure"* — none of the five is one.

---

## 94. Does it move `P6`?

🆕 **`annotation.json` ALREADY CARRIES THE SLOTS.** Every event emits **`mask: {provided: false}`** and
**`depth: {provided: false}`**, hardcoded, in every delivered artifact today
(`AnomalyLabelWriter.cpp:452-459`).

| candidate | moves `P6`? |
|---|---|
| **C-1** | ⚠ **VALUE, not SHAPE** — `mask.provided` → `true` in a slot that already ships; `bbox_norm` changes value. **Sub-fields under `mask` WOULD be a shape change** |
| **C-2** | ⚠ same, via the `depth` slot |
| **C-3** | ✅ **NONE** if used purely as a pick-time gate |
| **C-4** | ⚠ **VALUE only** — `coverage_pct` / `coverage_ratio` / `bbox_*` |
| **C-5** | ⚠ **VALUE only** — this *is* the locked `node.bounds` ruling |

⛔ **`P6` HAS NOT MOVED. Nothing was added, removed, renamed or recomputed.**

---

## 95. TASK 2 — the disk prune, with a verify-before-delete pass

**Method = PART SIX's: match BY SESSION ID, never by directory name** (name matching missed nine last
time — and it would have again: exe-side `CM_CTRL49_try1` banks as `CM_CM_CTRL49_try1`).

**A match was only accepted as VERIFIED when the full per-file `{relative path → size}` manifest was
identical** — strictly stronger than the session-id match the brief required, and chosen deliberately
**because of PART ELEVEN's zero-byte frames**: a size-blind check would bless a truncated copy.

### 95.1 Result

| verdict | rows | bytes |
|---|---|---|
| **VERIFIED-DUPLICATE** | **79** | **6.21 GB** |
| UNVERIFIED — no bank match | 2 | 0.14 GB |
| NO-SESSION-DIR | 2 | 0.01 GB |

**Bank index: 195 `session_*` dirs, 155 distinct ids.**

⛔ **NOT VERIFIABLE ⇒ NOT DELETED. Listed and left alone:**

| leg dir | session | why it stays |
|---|---|---|
| `H4_WSECHO` | `session_20260819-170238` | **session id absent from the bank** (45 files, 24.7 MB) |
| `MW_STOMPER_try1` | `session_20260819-172148` | **session id absent from the bank** (95 files, 115.6 MB) |
| `D3D12` | — | no `session_*` dir (engine cache, 2 files) |
| `S3A2_OFF` | — | no `session_*` dir, **0 files** |

⚠ **Those two unbanked sessions are exactly what a name-based sweep would have deleted.** They are
**unbanked evidence**, the same shape PART SIX's re-bank sweep found — flagged, untouched, and **a
banking decision for the owner, not a deletion one for me.**

### 95.2 Safety

- Every one of the 79 leg dirs was confirmed to contain **only** files inside its `session_*` dir —
  **0 bytes of logs or anything else outside it** — so deleting the leg dir loses nothing beyond the
  verified duplicate.
- **Re-asserted immediately before deleting:** all 79 bank copies present and non-empty.
- **Prune scope was DIRECTORIES under `Binaries\Win64` only.** The staged `StackOBot.exe` and its four
  in-place `.baseline` files are **files**, never in scope, and verified intact afterwards.

### 95.3 Space, and the protected trees

| | before | after |
|---|---|---|
| **free on D:** | **12.89 GB** | **19.12 GB** |
| recovered | | **6.23 GB** (6,688,104,448 bytes) |

✅ **UNTOUCHED, verified after the prune:**

| tree | state |
|---|---|
| `_binary_baselines` | **11 files, 1,499,355,461 B** — five `.baseline` exes + README + the quartet |
| `pathA-measurement-build-paks` | **`.utoc` 268,036 · `.ucas` 284,469,920 · `.pak` 10,115,703** — **matches the recorded identity exactly** |
| `_bench_sessions_bank` | **148 top-level dirs, 20,139 files, 16.84 GB** |
| staged build | `StackOBot.exe` + 4 baselines, all present |

⇒ **6.23 GB ≫ the ~3 GB floor, so the prune stood on its own and the bank retention policy did not
need to be opened.**

---

## 96. State after PART TWELVE

| | |
|---|---|
| plugin production code | **ZERO lines, across all TWELVE parts** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED, UNREBASED, never checked out** |
| `P6` | **DOES NOT MOVE** |
| build | unchanged — `exe 101AFEA4` + `utoc 939B9C9B` |
| new gotchas | **`G126`** shadow-only reads "rendered" · **`G127`** filter ships, pixels don't · **`G128`** delivery gates reporting not measurement |
| ledger | **§3.2** Ruling 2 · **§3.3** Ruling 1 · **§6** the costed option space |
| disk | **12.89 → 19.12 GB free**; 79 verified duplicates deleted, **4 unverifiable dirs left alone** |
| ⛔ not done | **no candidate implemented, prototyped, or PICKED — the pick is the owner's, next brief** |

**THE ANSWER THIS PART PRODUCES, in one line:** **C-1 is the only candidate that addresses both `H5`
classes; C-3 is the only cheap one and it answers `H4`'s question rather than `H5`'s; C-5 is a `P6`
correctness fix and a measured no-op on `H5`; and NO candidate is blocked by delivery mode — the real
constraint is Shipping, which capture already lives outside.**

---

# PART THIRTEEN — the TIMING design: where a pixel answer can actually land

**SOURCE READING ONLY. NO CODE, NO PROTOTYPE, NO BRANCH, NO SHAPE PICKED. ZERO PRODUCTION CODE — now
across THIRTEEN parts. NO TAG. `P6` DOES NOT MOVE. `feature/stencil-capture` READ-ONLY.**

---

## 97. Rulings and the banking, recorded first

**RULING 1 — `C-1` IS THE CURE DIRECTION, AND IT IS NOT A REVIVAL.** Recorded in the ledger §3
(`cure direction` row). ⛔ **Mine `feature/stencil-capture`, do not resume it.** Its foliage blacklist
must not survive; its `USkeletalMeshComponent` narrowing **manufactures the exact defect the cure
exists to detect**; its reduction is unpriced (→ §104).

**RULING 2 — the label/selection disagreement is `P6`'s FOURTH OBSERVATION: RECORD, DO NOT FIX.**
Ledger **§7**. Three code paths already answer *"what is the object?"* differently. 🚨 **A mask would
be a FOURTH definition, and a cure that adds one is a defect generator.**

**BANKED, on owner ruling — bank 148 → 150.** Both verified by identical per-file path+size manifest
on copy:

| new bank dir | session | files |
|---|---|---|
| `RESCUE_P12_H4_WSECHO` | `session_20260819-170238` | 45 |
| `RESCUE_P12_MW_STOMPER_try1` | `session_20260819-172148` | 95 |

🚨 **AND THE NAME-BASED-SWEEP CLAIM IS NOW CONCRETE, NOT HYPOTHETICAL.** The bank **already
contained** `RESCUE_H4_WSECHO` — holding **`session_20260819-140533`**, a **different session**. A
sweep keying on directory names would have matched exe-side `H4_WSECHO` to it, called it banked, and
**destroyed the only copy of `…-170238` while reporting a clean duplicate.** **The name matched; the
evidence did not.** A bank `README.md` now records this (the bank had none).

---

## 98. `T-1` — THE LIFECYCLE, from source

### 98.1 The tick

`UAnomalyCaptureSubsystem::Tick` (`:318-438`), a `UTickableWorldSubsystem`, in order:

| # | call | note |
|---|---|---|
| 1 | `SampleDeferredHidden()` | fills `FireHidden` from the **previous** tick (the `m20` fix) |
| 2 | `ArmedPending` focus branch | early-returns until focus or the 30 s timeout |
| 3 | `PaceThisTick()` | real-time pacing sleep (`m11`) |
| 4 | `SampleViewThisTick()` | pushes the view into `ViewRing` |
| 5 | `ProcessCompletedFrames()` | drains readbacks → builds the label record → `AccumulateFrameEvents` → enqueues the writer job |
| 6 | frame-cap check | → `DrainTail` |
| 7 | **the phase switch** | below |
| 8 | `FinalizeArmedLabel()` | **last statement**; fills the armed snapshot POST-transition (the `m18` fix) |

### 98.2 The phase machine, and the defaults

Defaults from `AnomalyCaptureSubsystem.h:185-189`: **`SettleFrames K = 2` · `PreFrames = 4` ·
`PositiveFrames = 8` · `PostFrames = 4` · `BurstCount = 0`** (unlimited).

| phase | frames | captured? | exit |
|---|---|---|---|
| `LeadIn` | `Pre` **4** | ✅ | **`BeginFire()` IN THE SAME TICK as the last capture** |
| `SettleAfterFire` | `K` **2** | ⛔ | → `Positives` |
| `Positives` | **8** | ✅ | `BeginRevert()` |
| `SettleAfterRevert` | `K` **2** | ⛔ | → `PostGap` |
| `PostGap` | **4** | ✅ | **`BeginFire()` IN THE SAME TICK**, or → `DrainTail` |
| `DrainTail` | `max(10, ViewLag+4)` = **10** | ⛔ | `FinishRun` (early when `PendingSnapshots` empties) |

**One burst cycle = 20 ticks, 16 of them captured.** `LeadIn` runs **ONCE PER RUN**, not per burst.

### 98.3 🚨 THE DECIDING FACT: SELECTION → FIRE IS **ZERO FRAMES**

`BeginFire()` (`:1089-1103`) calls `TryFireOnce()`, and `TryFireOnce`
(`AnomalyAutoInjectorSubsystem.cpp:172-256`) does **all** of this in **one synchronous call, inside
one tick**:

`GetVisibleRenderableActors(World)` → sort by name → `Stream.RandHelper` picks the id →
build `Candidates` → `Stream.RandHelper` picks the target → `Stream.FRandRange` picks the hold →
**`Injector->ApplyAnomaly(Id, {"=" + TargetName})`** — which for `missing_object` **hides the actor
immediately**.

⇒ **There is no gap between selection and fire to put a measurement into. It is not a small gap. It
is zero.**

### 98.4 What is knowable at each step

| step | knowable | NOT knowable |
|---|---|---|
| `GetVisibleRenderableActors` | renderable **type**, `IsVisible()`, poll radius, frustum, `IsUnoccluded` (9 **CPU** traces), coverage from the **projected bounds union** | **anything about pixels.** The game thread runs *before* this frame is rendered — the frame containing the candidate **does not exist yet** |
| `ApplyAnomaly` | same | same |
| first captured positive (**3 ticks later**) | the frame's pixels | ⛔ **the target is already hidden, so its own pixels no longer exist to measure** |

### 98.5 ⇒ DOES A 12-FRAME PRE-FLIGHT FIT ANYWHERE? **NO.**

| candidate slot | frames | verdict |
|---|---|---|
| `LeadIn` | **4** | too short — and it exists **once per run**, not per burst |
| inter-burst (`SettleAfterRevert` + `PostGap`) | **6** | too short — and `PostGap` frames are **captured as negatives**, so they are not free |
| `ArmedPending` focus wait | up to **30 s** | ⛔ **not a design slot** — it is a focus gate, absent when the window is already focused |

⛔ **The longest existing gap is 6 frames against a 12-frame budget. A pre-flight cannot be fitted
into the existing sequence — it has to be CREATED, which changes when anomalies fire.**

### 98.6 🚨 A SECOND BLOCKER ON THE VETO SHAPE: THE SEEDED DRAW PROTOCOL

`TryFireOnce` consumes the seeded stream in a **fixed order**: id → target → hold, then
`AdvanceTime` draws the next interval. **`R-SEED` made that protocol independent of apply-result on
purpose**, and `m22` gated on **"seeded selection identity (seed 4242, 8 events, two runs
byte-identical)"**.

⇒ **A veto that rejects and re-picks makes the NUMBER OF DRAWS depend on a render-thread pixel
result. Seeded reproducibility is destroyed and a SHIPPED GATE BREAKS.** Recoverable only by
redesigning the protocol (e.g. draw a fixed-size candidate set up front and veto within it without
re-drawing) — **a change to `R-SEED`, not an addition to it.**

---

## 99. `T-2` — the three shapes, costed

### 99.1 (a) PRE-FLIGHT VETO — mask the candidate BEFORE arming

| | |
|---|---|
| **fixes** | **both `H5` classes at the source. No poisoned sample is ever generated.** |
| **does NOT fix** | a target that stops drawing *after* the pick (path (a)'s shape) — the answer is stale by construction (`P-a5`) |
| **cost 1** | ⛔ **no slot exists** (§98.5) — ≥12 frames must be inserted before every fire |
| **cost 2** | ⛔ **breaks seeded reproducibility** (§98.6) |
| **cost 3** | at 30 fps, 12 frames = **0.4 s of game time** per fire; against a 20-tick burst cycle that is a **~60 % longer cycle** ⇒ **fewer events per run at the same frame cap** |
| **cost 4** | the mask must be armed on a **CANDIDATE**, not a target ⇒ `bRenderCustomDepth` mutation and `r.CustomDepth 3` extend to **candidates**, widening the render-state mutation surface well beyond today's |
| **contract** | ✅ **NONE.** The veto precedes any event, so nothing is written. `mask{}` stays `provided:false`. **Zero `P6` movement.** |

### 99.2 (b) POST-HOC ANNOTATION — fire as now, measure during the window, record it

| | |
|---|---|
| **fixes** | every sample becomes **filterable downstream**; the contribution lands in the **already-shipping `mask{}` slot** |
| ⛔ **does NOT prevent** | **the poisoned frames are still generated and still labelled positive.** `manifested`, `frame_indices`, `coverage_pct`, `bbox` all still assert the claim. **A consumer that ignores `mask{}` receives exactly today's dataset.** |
| ⛔ also cannot fix | the per-frame label **rect** — `Job.Record` is built and enqueued at `ProcessCompletedFrames` (`:996-1013`) and is **not retroactively editable** |
| 🚨 **timing trap** | **for a HIDE-TYPE anomaly the target is hidden during the positive frames, so the mask is 0 BY CONSTRUCTION.** The measurement must come from a **negative** frame (`LeadIn`/`PostGap`) or a last-known value — **this is exactly what the branch's `LOCK-1` encodes**, and it is not optional |
| **cost** | **cheapest by far** — masks armed only on frames already being captured; no lifecycle change, no seed change, no timing change |
| **contract** | `mask.provided` → **true** = a **VALUE** change in a slot that already ships. ⚠ **Sub-fields under `mask` (a count, a rect) WOULD be a SHAPE change ⇒ `P6` MOVEMENT.** The slot exists; its contents do not. |

### 99.3 (c) DEFERRED VETO — fire as now; invalidate the event retroactively

**Q: is the artifact still open 12 frames in? — ANSWERED FROM `FinishRun`, not intuition.**

| artifact | when written | still open? |
|---|---|---|
| **`annotation.json`** | **once, in `FinishRun` (`:1472`)**, from the in-memory `Async->SessionEvents` accumulator | ✅ **YES — mutable until `FinishRun`** |
| `labels.jsonl` | per frame, `Job.Record` prebuilt and enqueued at `:1013` | ⛔ **NO** |
| the PNGs | per frame, worker pool | ⛔ **NO** |

✅ **And there is a real mechanism to force the readbacks home:** `FinishRun` calls
`DrainAsyncToCompletion()` **before** writing the annotation — **8 iterations, each with a blocking
`FlushRenderingCommands()`** (`:1035-1047`).

🚨 **AND THE DECISIVE ONE FOR THE CLIENT: `Job.bWriteLabels = !bDeliveryMode` (`:1012`).** In
**delivery mode `labels.jsonl` IS NOT WRITTEN AT ALL** ⇒ **in the mode the client actually uses,
`annotation.json` is the ONLY label artifact, and it is written entirely at `FinishRun` from a
still-open accumulator.** ⇒ **(c) is fully viable in exactly the mode that matters.**

| | |
|---|---|
| **fixes** | poisoned events can be **dropped or flagged before the client artifact exists** |
| **does NOT fix** | the **frames are already on disk**. A dropped event leaves images with no label — which is what a hard negative looks like, but it moves `total_frames` vs `positive_frames` accounting |
| ⚠ **a number that does not line up** | **`DrainTail` is `max(10, ViewLag+4)` = 10 frames; the mask budget is 12.** The existing tail is **SHORTER than the readback budget**. `DrainAsyncToCompletion`'s 8× flush would very likely force it home — **but 10 < 12 as read, and reconciling it is a MEASUREMENT, not a source read. Flagged, not resolved.** |
| **contract** | dropping an event changes `anomalies[]` **membership** — no field added, a **value/consistency** change. Flagging needs the `mask{}` slot (as in (b)). |

---

## 100. `T-3` — the module shape

### 100.1 The four options

| # | shape | game-agnostic invariant | when the provider is absent |
|---|---|---|---|
| 1 | **interface** declared in `AnomalyInjector`, implemented + registered by `AnomalyCapture` | ✅ preserved — the interface returns a **number**, needs no render types | pointer null |
| 2 | **callback/delegate** set by `AnomalyCapture` | ✅ preserved | unbound |
| 3 | **cached result** the selector reads (per-actor extent + frame stamp) | ✅ preserved — **and it matches §98.4: a pixel answer is ALWAYS stale, so a cache makes the staleness EXPLICIT instead of hidden** | cache empty / stale |
| 4 | **move the measurement into `AnomalyInjector`** | ⚠ CLAUDE.md contemplates `Renderer`/`RHI`/`RenderCore`/`Slate` as later deps, so **not forbidden** — but it puts the **Renderer PRIVATE include path (`G100`) into the module that ships in EVERY config**, where an engine bump breaks the shipped path | n/a — always present |

### 100.2 🚨 WHAT IS THE SHIPPING-BUILD BEHAVIOUR IF THE MEASUREMENT IS ABSENT? — answered explicitly

**In Shipping, `ANOMALY_CAPTURE=0`: THERE IS NO CAPTURE AT ALL.** The module loads (it is listed in
`AnomalyInjector.uplugin`) but its body is compiled out. The **selector and auto-injector DO exist**
in Shipping — they live in `AnomalyInjector` — so anomalies can still be injected. **But no frames,
no `labels.jsonl`, no `annotation.json` are produced.**

⇒ **`H5` IS A LABELLING DEFECT — a sample whose label claims more than the pixels support. WHERE
THERE IS NO LABEL, THERE IS NO `H5` DEFECT.** A Shipping build may still admit an over-claiming
actor, but it mislabels nothing and poisons no dataset.

⇒ ✅ **THE CURE CAN BE NON-SHIPPING-ONLY WITHOUT LEAVING A HOLE.** That is the answer that decides the
shape, and it removes option 4's only real motivation.

**⇒ THE FALLBACK ON ABSENT MUST THEREFORE BE *ADMIT* (behave exactly as today), NOT REJECT:**
- **Reject-on-absent** changes injection behaviour in the config the host game ships, for **zero**
  benefit — against the game-agnostic invariant in spirit.
- **Admit-on-absent** is byte-identical to today wherever the measurement cannot exist.

⚠ **BUT THE FALLBACK MUST NOT BE SILENT — and this is `G119`'s diagnostic pointed at the cure.** If
the provider is absent in a **non-Shipping** build (failed registration, wrong load order), a silent
admit **reproduces exactly today's defect while looking cured**. *"What would I observe if the thing I
added never reached the thing under test?"* — **today's output.** ⇒ **absence must be logged loudly
and recorded in the artifact**, or the cure is unfalsifiable. (`m19`: gate on pixels, not on a
counter.)

---

## 101. `T-4` — pricing the reduction

⚠ **ANALYTICAL, NOT MEASURED. Measuring needs production code ⇒ out of scope.** Stated as an estimate
with its assumptions, never as a reading.

**1280×720 = 921,600 px, R8 = 1 B/px ≈ 900 KB per armed frame, scanned single-threaded ON THE RENDER
THREAD.**

| case | work | estimate |
|---|---|---|
| all-zero mask | linear byte scan, ~900 KB (fits L2) | **~0.1–0.3 ms** |
| 10 % tagged (~92 k px) | + **one `TMap::FindOrAdd` per non-zero pixel** | **~1–3 ms** |
| 100 % tagged | ~921 k hash lookups | **~9–28 ms** |

🚨 **THE PATHOLOGICAL CASE IS EXACTLY THE `H5` CASE.** The target that claims the whole frame
(`InstancedFoliageActor`, `bbox_px (0,0,1280,720)`) is the one whose mask can cover the most pixels.
**The reduction is most expensive precisely where the defect lives**, and at 30 fps the 100 % case
approaches or exceeds the **33 ms** frame budget — **on the render thread.**

✅ **A CHEAP FIX INSIDE THE EXISTING SHAPE, worth naming before any alternative:** tags are a tiny
known set (`ReservedStencilBase = 200`, headroom under 256). **Replacing the `TMap` with a fixed
256-entry array indexed by tag value removes the hash from the inner loop entirely and makes the
worst case ≈ the best case.** That is a small change, not a new architecture, and it changes the cost
picture before any of the options below are needed.

### 101.1 Cheaper reductions, and what each gives up

| option | what it costs | ⛔ what it GIVES UP |
|---|---|---|
| **(a) GPU-side reduction** — compute shader, atomic add for count + atomic min/max for bbox into a tiny per-tag buffer | readback drops **~900 KB → tens of bytes**; work moves to the GPU | little in fidelity — a compute shader, atomic contention on a full-screen pass, more code. **Still latent.** |
| **(b) downsampled mask** (¼ → 320×180, **16× cheaper**) | cheapest of all | ⛔ **THE LOW END, WHICH IS THE SIGNAL.** A target drawing <16 px can reduce to **zero** — and *"draws far less than it claims"* is the whole `H5` measurement. **It systematically destroys the measurement exactly where the decision is made.** |
| **(c) count only inside the projected rect** | scan shrinks with the rect | ⛔ **TWO INDEPENDENT FAILURES.** (i) For `InstancedFoliageActor` **the rect IS the whole frame**, so it saves nothing **precisely in the worst case**. (ii) **RULING 2 / A35** — `SM_Ramp2`'s peak effect is **OUTSIDE** its rect (**0.2955** out vs **0.1785** in), so a rect-scoped reduction **under-counts a legitimate target**. |
| **(d) hardware occlusion query** (`RQT_Occlusion`) | engine doc: *"Result is the number of samples that are not culled (divide by MSAACount to get pixels)"* — **a hardware pixel counter, ~8-byte result** | ⛔ **no bbox — a COUNT ONLY.** Needs the target's geometry drawn in a query batch (a per-target draw); latent like all queries; MSAA scaling. |

### 101.2 🚩 What (d) exposes, and it is a design finding

**A VETO NEEDS ONLY A COUNT. ONLY FIXING THE LABEL *RECT* NEEDS THE MASK.**

If the chosen shape is a **veto or a flag** — (a) or (c) in `T-2` — an occlusion query may be
sufficient, and **the mask, the readback and the whole W×H scan disappear**. If the shape is *"correct
the box to the drawn silhouette"*, the mask is unavoidable. ⇒ **`T-2`'s shape choice decides `T-4`'s
instrument, not the other way round.**

---

## 102. State after PART THIRTEEN

| | |
|---|---|
| plugin production code | **ZERO lines, across all THIRTEEN parts** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED at `76cac74`, never checked out** |
| `P6` | **DOES NOT MOVE** — fourth observation recorded in the ledger §7, not fixed |
| bank | **148 → 150**, both recovered legs manifest-verified; bank `README.md` created |
| ledger | §3 `cure direction` = `C-1` (Ruling 1) · **§7** `P6`'s fourth observation (Ruling 2) |
| ⛔ not done | **no shape picked, nothing implemented, prototyped or branched** |

**WHAT THIS PART SETTLES:** **selection → fire is ZERO frames and the longest gap in the whole
lifecycle is SIX, so a 12-frame pre-flight does not fit and would additionally break seeded
reproducibility; `annotation.json` is still OPEN at `FinishRun` and in DELIVERY MODE it is the ONLY
label artifact, so a deferred veto is viable exactly where it matters; and because Shipping has no
capture at all, a non-Shipping-only cure leaves NO HOLE — provided its absence is never silent.**

---

# PART FOURTEEN — the shape is ruled, and the two gating measurements

🚨 **PRODUCTION CODE WAS WRITTEN THIS PART, FOR THE FIRST TIME IN THIRTEEN PARTS — narrowly, on owner
permission, for `M-1` ONLY.** It is **log-only instrumentation**: no behaviour change, no artifact
field. **`P6` VERIFIED UNCHANGED BY MEASUREMENT (48 fields, 0 added, 0 removed).** **NO TAG.**
`feature/stencil-capture` **still READ-ONLY at `76cac74`, never checked out.**

---

## 103. `RULING 1` — SHAPE (a) IS **REJECTED, PERMANENTLY**, WITH REASONS

⛔ **NOT DEFERRED. REJECTED.** Recorded so a future reader proposing *"just check before firing"* sees
both blockers without re-deriving them.

| # | blocker | source | independently sufficient? |
|---|---|---|---|
| **1** | **THE SELECTION-TO-FIRE GAP IS ZERO.** `BeginFire()` → `TryFireOnce()` runs `GetVisibleRenderableActors`, **both seeded picks**, and `ApplyAnomaly` — which **hides the actor immediately** — in **one synchronous call inside one tick.** The longest gap anywhere in the burst cycle is **6 frames**; `LeadIn` is **4** and runs **once per RUN**, not per burst | `AnomalyAutoInjectorSubsystem.cpp:172-256`, `AnomalyCaptureSubsystem.cpp:1089-1103, 375-434` | ✅ **YES** |
| **2** | **A RE-PICKING VETO DESTROYS THE SEEDED DRAW PROTOCOL.** `R-SEED` made the draw order independent of apply-result on purpose; `m22` gated on *"seed 4242, 8 events, two runs byte-identical"*. A veto that rejects and re-picks makes the **number of stream draws depend on a render-thread pixel result** | same, + the `m22` gate | ✅ **YES** |

**Recoverable only by redesigning `R-SEED`, which is not on the table.**

---

## 104. `RULING 2` — THE SHAPE IS **(c), WITH (b)'s REPORTING FOLDED IN**

**One mechanism, two outputs:**

1. **MEASURE** during the capture window (as (b) does) — same mask, **same `LOCK-1` timing
   constraint**: for a hide-type anomaly the target is hidden during the positives, so **the mask is
   0 BY CONSTRUCTION** and the measurement must come from a **negative** frame or a last-known value.
2. **RECORD** the contribution in the already-shipping **`mask{}`** slot — `provided: false → true`, a
   **VALUE** change.
3. **INVALIDATE** the event in the **in-memory accumulator** before `FinishRun` writes
   `annotation.json`, when the measured contribution says the target contributed nothing.

**WHY (c) AND NOT (b) ALONE — on the record:** *(b) is a disclosure, not a cure.* The poisoned frames
are still generated and still labelled positive; `manifested`, `frame_indices`, `coverage_pct` and
`bbox` all still assert the claim; and **a consumer that ignores `mask{}` receives exactly today's
dataset.**

**WHY NOT (c) ALONE:** (c) needs (b)'s measurement anyway.

### 104.1 ⚠ ACCEPTED LIMITS — recorded NOW, not discovered later

| # | limit |
|---|---|
| **L1** | **The frames are already on disk.** (c) removes the **EVENT** from `annotation.json`; **it does not un-write PNGs.** |
| **L2** | **Dropped events move total-vs-positive accounting**, and that must be **explicit and COUNTED, never silent** — a dropped event needs a counter exactly as `m23`'s `non_manifested_events` has one. ✅ **A `run_summary.json` counter does NOT move `P6`** — `P6` is the `annotation.json` contract, and `run_summary` already gained `capture_path` + five `key_ring_*` at `S4` without moving it. |
| **L3** | **`labels.jsonl` (delivery OFF) is prebuilt and cannot be corrected** (`Job.Record` at `:1013`), so **delivery OFF and delivery ON will DISAGREE.** ⛔ **Acceptable only because it is stated here. It would not be acceptable discovered.** |

---

## 105. `M-1` — PRE-DECLARED BRANCHES, RESTATED **VERBATIM** BEFORE THE RESULT

**Committed as a file BEFORE any leg ran: `CaptureBench/tools/p14_predeclared_branches.md`, commit
`ee9ecc1`.** Restated here without alteration:

> | # | branch | reading that selects it |
> |---|---|---|
> | **B1** | **DRAIN FORCES IT HOME, COMFORTABLY** | max observed latency **≤ 8** render frames, **0** frames dropped, and `DrainAsyncToCompletion` consumes **few** of its 8 iterations |
> | **B2** | **DRAIN FORCES IT HOME, BUT ONLY VIA THE FLUSH** | 0 dropped, **but** frames remain pending when `DrainTail` ends and are resolved only by the blocking flushes |
> | **B3** | **DRAIN DOES NOT FORCE IT HOME** | any frame dropped / unresolved at `WriteSessionAnnotationFile` ⇒ 🚨 needs an explicit wait at `FinishRun` — a **LIFECYCLE CHANGE, the OWNER'S RULING.** HALT |
> | **B4** | **IT DEPENDS ON N, WITH A THRESHOLD** | latency varies systematically with proximity to run end |
> | **B5** | **INVALID / NOT MEASURED** | instrument did not report; leg failed for a HOW-IT-RAN reason ⇒ discard, bank, re-run |

**And the reframe, also pre-declared:** ⚠ *"the **12** is a BUDGET THE BRANCH AUTHOR CHOSE, NOT A
MEASURED LATENCY. Nobody has measured what a GPU readback on this box actually costs."*

### 105.1 The instrument

Log-only, on the **existing** colour readback (`FAnomalySveCapturer`): record
`GFrameNumberRenderThread` at `SubmitInFlight_RenderThread`, again when `IsReady()` first returns
true in `Drain_RenderThread`; the difference is readback latency in **render frames**. Plus the
pending count at `DrainAsyncToCompletion` entry and how many of its 8 blocking-flush iterations are
consumed.

⚠ **Stated in advance:** the colour readback is armed in the same frame and the same graph a mask
would be, through the same `AddEnqueueCopyPass` → `FRHIGPUTextureReadback` → `IsReady()` mechanism.
**Its payload is LARGER (BGRA vs R8), so its latency is an UPPER BOUND on a mask's, not an equal.**

### 105.2 THE RESULT — **BRANCH `B1`, on both legs**

| leg | delivery | samples | min | max | mean | notReadyPolls | histogram | pendingAtDrainEntry | flushIterationsConsumed | dropped |
|---|---|---|---|---|---|---|---|---|---|---|
| `P14_M1_DELIVOFF` | **off** | 90 | **1** | **2** | **1.011** | 1 | `1:89 2:1` | **0** | **0 of 8** | **0** |
| `P14_M1_DELIVON` | **on** | 90 | **1** | **1** | **1.000** | 0 | `1:90` | **0** | **0 of 8** | **0** |

🚨 **THE ACTUAL READBACK LATENCY IS ONE RENDER FRAME.** 89 of 90 at 1, a single sample at 2, and on
the delivery leg **90 of 90 at exactly 1.**

✅ **`pendingAtDrainEntry = 0` on both legs — by the time `FinishRun`'s drain runs, NOTHING IS
PENDING. `DrainTail` alone had already resolved everything, and the 8 blocking
`FlushRenderingCommands()` iterations were NEVER ENTERED.**

⇒ **`B1` selected: the drain forces it home, comfortably.** Max latency **2** against the branch
budget **12** and the tail budget **10**.

🚨 **AND THE 10-vs-12 QUESTION DISSOLVES RATHER THAN RESOLVING.** Neither number is anywhere near the
measured latency — the real figure is **1**, with a **5× margin** to the smaller of the two. **The
conflict I flagged in PART THIRTEEN was between two BUDGETS, neither of which was ever a measurement.**
✅ **Shape (c) needs no lifecycle change.**

**Both legs A63-VALID on attempt 1.** Delivery-OFF leg: **B1 pose gate APPLICABLE and PASSED**
(`CB_GateLevel` / `StaticMeshActor_49` / 1280×720; `modal_rot (0,0,0)`, 59 rows, 1 distinct, modal
100 %, `pose_match=True`). Delivery-ON leg: **B1 declared NOT CHECKABLE** — `labels.jsonl` is
suppressed, so the harness reported the `coverage_ratio` pose *indicator* (`0.077977` × 8, invariant)
**and declared it rather than skipping silently** (G117 honoured). Key ring **121/121/0/0** on both.

### 105.3 A free confirmation of PART THIRTEEN, from the artifact rather than from source

The delivery-ON session's non-image file set is **`annotation.json` + `run_summary.json`, and nothing
else** — no `labels.jsonl`, no `run.json`. ⇒ **PART THIRTEEN's claim that in delivery mode
`annotation.json` is the ONLY label artifact is now MEASURED, not merely read from
`Job.bWriteLabels`.**

---

## 106. `M-2` — PRE-DECLARED BRANCHES, RESTATED **VERBATIM** BEFORE THE RESULT

> | # | branch | reading that selects it |
> |---|---|---|
> | **C1** | **COUNT SUFFICIENT, HARDWARE PATH USABLE** | `RQT_Occlusion` returns a mesh-accurate drawn-pixel count at acceptable latency/cost |
> | **C2** | **COUNT SUFFICIENT, HARDWARE PATH NOT USABLE** | a count answers the veto, **but** `RQT_Occlusion` cannot deliver a mesh-accurate one |
> | **C3** | **COUNT INSUFFICIENT** | the veto provably needs spatial information |
> | **C4** | **NOT DETERMINABLE WITHOUT MORE WORK** | source does not settle it |

> ⚠ *"if the hardware path is disqualified on **CORRECTNESS**, measuring its **COST** is theatre. A
> disqualified option is not made more disqualified by a millisecond number. **Report the
> disqualification and stop.**"*

### 106.1 THE RESULT — **BRANCH `C2`**, and it is settled from source

**What `RQT_Occlusion` returns** — engine comment, verbatim
(`RHIDefinitions.h:1077`): *"Result is the number of samples that are not culled (divide by MSAACount
to get pixels)"*. ✅ **It is a hardware pixel counter.** **Count is sufficient for a veto.**

🚨 **BUT WHAT IT COUNTS IS THE BOUNDING BOX, NOT THE MESH:**

| source | what it shows |
|---|---|
| `SceneOcclusion.cpp:485` | `FOcclusionQueryBatcher::BatchPrimitive(const FVector& **BoundsOrigin**, const FVector& **BoundsBoxExtent**, …)` |
| `:499-500` | `PrimitiveBoxMin = BoundsOrigin − BoundsBoxExtent` · `PrimitiveBoxMax = BoundsOrigin + BoundsBoxExtent` |
| `:680`, `:757` | `DrawIndexedPrimitive(**GCubeIndexBuffer**, …, 8 verts, **12 triangles**)` — **a CUBE** |
| `:694` | `const FBoxSphereBounds OcclusionBounds(SceneProxy->**WorldBounds**)` |

⇒ **UE's occlusion machinery rasterises the primitive's BOUNDING BOX. `RQT_Occlusion` via that path
returns the number of BOUNDING-BOX samples that pass the depth test.**

🚨 **THAT IS PRECISELY THE QUANTITY `H5` NAMES AS THE LIE.** For `InstancedFoliageActor_0_0_0` the
bounds are **252 m × 217 m × 67 m** covering the whole frame: a bounds-based occlusion query would
return a **huge** sample count while the foliage draws ~6 % of the frame. **It would ACTIVELY CONFIRM
THE FALSE CLAIM** — not merely fail to catch it.

**To be useful the query must bracket a draw of the target's actual MESH**, which means custom
per-target mesh draw commands — **more expensive than one full-screen mask pass**, and requiring
renderer-private mesh-batch machinery.

⇒ **`C2`: a COUNT suffices for the veto, but the cheap hardware path cannot supply a trustworthy one.
THE MASK STAYS — justified by a CORRECTNESS reason, not by cost.**

⛔ **AND THE COST WAS DELIBERATELY NOT MEASURED**, per the pre-declaration. A path disqualified on
correctness is not made more disqualified by a millisecond number, and measuring it would have looked
like diligence while adding nothing.

⇒ **`T-4`'s fixed 256-entry array was consequently NOT APPLIED** — its own precondition was *"IF you
touch the reduction at all for `M-2`"*, and `M-2` never needed the reduction touched. **Left for
whoever implements the mask.**

---

## 107. `TASK 2` — THE ONE DEFINITION, and the two states

### 107.1 The four definitions that exist today

| path | test | `IsVisible()`? |
|---|---|---|
| **selection** — `IsRenderableComponent` (`AnomalyViewport.cpp:493`) | `IsVisible()` ∧ (ISM ⇒ `GetInstanceCount() > 0`) ∧ (`UStaticMeshComponent` ∨ **`USkinnedMeshComponent`**) | ✅ |
| **label rect** — `ProjectActorBoundsToScreenRect` (`:653-685`) | **type only** | ⛔ |
| **`node.bounds`** — `GetComponentsBoundingBox(true)` | every `UPrimitiveComponent` | ⛔ |
| **branch tagging** — `AnomalyStencilTag::IsRenderableMesh` | `UStaticMeshComponent` ∨ **`USkeletalMeshComponent`** ← **NARROWER** | ⛔ |

### 107.2 THE ANSWER: the cure uses **`AnomalyViewport::IsRenderableComponent`**, and it is not a new choice

**It is the ALREADY-LOCKED ruling.** CLAUDE.md's `P6` contract decision states the definition *"must
reuse the existing renderable definition (`IsRenderableComponent`, static-or-skinned, **G33**) so
label geometry and selection geometry agree on what 'the object' is."* **Adopting it for masking
applies that ruling; it does not make a fourth.**

**Can all three paths share it?**

| path | can share? | note |
|---|---|---|
| **selection** | ✅ already does | — |
| **masking** | ✅ **immediately** — `IsRenderableComponent` is `ANOMALYINJECTOR_API` public (`AnomalyViewport.h:80`) and **`AnomalyCapture` already depends on `AnomalyInjector`** | **This alone removes the `UPoseableMeshComponent` narrowing defect** — by *calling the shared predicate* instead of hand-rolling a type test |
| **labelling** | ⚠ **NOT TODAY** — adopting it changes label-rect **values**, which is the locked-but-unimplemented `P6` bounds ruling. ⛔ **`P6` DOES NOT MOVE**, so labelling stays as-is and **the disagreement REMAINS, recorded** | must be stated in the design |

✅ **Why masking-with-`IsRenderableComponent` is SAFE against a type-only label set, argued rather
than assumed:** the label set is a superset of the mask set, and every component in the difference is
**invisible and therefore draws zero pixels**. **The mask can only omit components that contribute
nothing**, so it cannot under-report the drawn extent of what the label claims. ⚠ **The
`USkeletalMeshComponent` narrowing is NOT of that benign kind** — a `UPoseableMeshComponent` is
visible **and draws**, so excluding it under-reports a real contribution. That is the difference
between a safe omission and a false accusation.

### 107.3 REQUIRED — *"measured zero"* and *"never measured"* must be DIFFERENT STATES

🚨 **THE STAKE, concretely:** under (c), a target that is **selectable but never tagged** reads mask
**0** and would be **INVALIDATED DESPITE DRAWING PERFECTLY.** That is a false accusation, and under
(c) it **silently deletes a good event** — the cure manufacturing the defect it exists to detect.

✅ **THE REPRESENTATION, AND IT NEEDS NO NEW FIELD: `mask.provided` — the bool that already ships —
IS the two states.**

| `mask.provided` | meaning | the cure may invalidate? |
|---|---|---|
| **`false`** | **NEVER MEASURED** — no mask ran, the target could not be tagged, or the readback did not resolve | ⛔ **NO — ADMIT** |
| **`true`** | **MEASURED** | ✅ only then, and only if the count is zero |

**THE RULE: an event may be invalidated ONLY when `provided == true`. `provided == false` ⇒ ADMIT** —
the same fallback `T-3` established for an absent measurement, and for the same reason.

⇒ ✅ **`P6` DOES NOT MOVE.** `provided: false → true` is a **VALUE** change in a slot that already
ships. **No sub-field is required to carry the distinction.**

**Internally** the accumulator needs a **tri-state** per event — `NOT_MEASURED` / `MEASURED_ZERO` /
`MEASURED_NONZERO` — and **only `MEASURED_ZERO` invalidates.** ⚠ **A zero that means *"nothing was
measured"* and a zero that means *"nothing was drawn"* MUST NOT SHARE A REPRESENTATION** anywhere in
the chain. **⛔ NOT IMPLEMENTED — stated as required, per the brief.**

---

## 108. What changed on disk, and the build identity

**PRODUCTION CODE (log-only, `M-1` only):** `AnomalySveCapturer.{h,cpp}` — a latency-stats struct,
`SubmitRtFrame` on the in-flight item, accumulation in `Drain_RenderThread`, a `GetLatencyStats()`
accessor, reset in `Reset()`. `AnomalyCaptureSubsystem.cpp` — two `UE_LOG` lines in
`DrainAsyncToCompletion`. ⛔ **No behaviour change. No artifact field. `P6` verified unchanged by
measurement.**

🚨 **BUILD IDENTITY IS A QUARTET (`G121`), and a code-only hot-swap moved exactly one half:**

| half | before | after |
|---|---|---|
| **exe** | `101AFEA4` | 🆕 **`1EBA8944`** |
| `StackOBot-Windows.utoc` | `939B9C9B` | **`939B9C9B`** unchanged |
| `StackOBot-Windows.ucas` | `8A602D4D` | **`8A602D4D`** unchanged |
| `StackOBot-Windows.pak` | `7CAE22DD` | **`7CAE22DD`** unchanged |

✅ **`101AFEA4` PRESERVED** at `_binary_baselines\StackOBot.exe.m25-baseline`, **hash-verified against
the staged copy immediately before the swap.**
✅ **A44 on the STAGED artifact, BOTH encodings:** `M1 readbackLatencyFrames` and
`M1 pendingAtDrainEntry` **ascii=0 utf16=1**, alongside pre-existing `IsHideTypeAnomaly` and
`capture_path` also at utf16=1 ⇒ **the change reached the package and the scan is sound, not blind.**

---

## 109. State after PART FOURTEEN

| | |
|---|---|
| production code | 🆕 **log-only instrumentation, `M-1` only** — the first in fourteen parts, on owner permission |
| `P6` | ✅ **VERIFIED UNCHANGED BY MEASUREMENT** — 48 fields, 0 added, 0 removed |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED at `76cac74`** |
| build | 🆕 **exe `1EBA8944`** + pak quartet unchanged; `101AFEA4` preserved |
| bank | **150 → 154** — two legs, **four dirs**: A63 banks the `_try1` attempt alongside the accepted alias |
| shape | **(c) with (b)'s reporting** — (a) **REJECTED, permanently, two independent blockers** |
| ⛔ not done | the cure itself. No mask, no veto, no tri-state — **stated as required, not written** |

**WHAT THIS PART SETTLES:** **the readback takes ONE render frame, so the 10-vs-12 conflict was
between two budgets and never touched reality — shape (c) needs no lifecycle change; the cheap
hardware count is disqualified because UE's occlusion query measures the BOUNDING BOX, the very
quantity `H5` calls a lie, so the mask stays for a CORRECTNESS reason; and the cure's one definition
is `IsRenderableComponent`, which masking can adopt today and which makes *never measured* and
*measured zero* distinguishable through `mask.provided` alone — with no `P6` movement.**

---

# PART FIFTEEN — `m26`: the implementation PLAN

⛔ **PLAN ONLY. NOTHING WRITTEN. NO IMPLEMENTATION UNTIL APPROVED** (the project's standing
plan-before-code rule). **`P6` DOES NOT MOVE.** `feature/stencil-capture` **READ-ONLY at `76cac74`,
mined, never checked out.** **NO TAG.**

---

## 110. `RULING 1` — THE DESIGN IS CLOSED. Frozen statement.

| axis | ruled |
|---|---|
| **SHAPE** | **(c) deferred invalidation, with (b)'s reporting folded in** |
| **INSTRUMENT** | **the mask.** `RQT_Occlusion` **DISQUALIFIED ON CORRECTNESS** (bounds-rasterised), not on cost |
| **TIMING** | readback completes in **ONE render frame**; `pendingAtDrainEntry` **0** on both delivery modes; the 8 blocking flushes **never entered**. ⇒ **NO LIFECYCLE CHANGE** |
| **DEFINITION** | **`AnomalyViewport::IsRenderableComponent`** — the already-locked `G33`/`P6` predicate. **Masking ADOPTS it** (public `ANOMALYINJECTOR_API`, dependency already exists). **No fourth definition is created** |
| **SAFETY** | `mask.provided` **false = NEVER MEASURED = MUST ADMIT**; **true + count 0 = MEASURED ZERO = may invalidate**. 🚨 **The two zeros must never share a representation ANYWHERE in the chain** |
| **SCOPE** | capture configs only. `ANOMALY_CAPTURE=0` in Shipping ⇒ no labels ⇒ no `H5` defect. **Fallback on absent = ADMIT, never REJECT, and NEVER SILENT** |
| **LIMITS** | **`L1`** frames already on disk · **`L2`** dropped events MUST be counted · **`L3`** delivery OFF and ON will disagree (`labels.jsonl` is prebuilt) |

## 111. `RULING 2` — THE NEGATIVE BRANCH IS A **SHIP GATE**, not a test case

**This project's record is unambiguous: a guard that has never fired is not a guard (`G96`, three
instances), and only known-answer controls have ever exposed instrument blindness.**

| gate | target | required outcome |
|---|---|---|
| **N-1 IT FIRES** | `InstancedFoliageActor_0_0_0` **or** `BP_SplineSpawn_C` (both measured, both banked) | event **INVALIDATED** |
| **N-2 IT DOES NOT OVER-FIRE** | `StaticMeshActor_49` (control) **and** 🚨 **`SM_Ramp2`** | **NOT invalidated** |
| **N-3 IT ADMITS WHEN BLIND** | measurement absent/failing | `mask.provided` **false**, event **ADMITTED**, output **byte-identical to today** |
| **N-4 THE BLIND CASE IS LOUD** | same | absence **logged AND recorded in the artifact**. **`G119`'s diagnostic must not answer *"today's output"*** |

🚨 **`SM_Ramp2` IS THE SHARPEST AND IS NAMED EXPLICITLY: peak-OUT `0.2955` against peak-IN `0.1785`.
IF THE CURE'S MEASUREMENT IS RECT-SCOPED ANYWHERE, `SM_Ramp2` IS WHERE IT WILL WRONGLY FIRE.**
⇒ **the reduction is WHOLE-FRAME, never rect-scoped** (§114).

---

## 112. `P-1` — FILE BY FILE

**Legend — `MINE` = ported READ-ONLY from `feature/stencil-capture` (`git show`, branch untouched);
`NEW` = written here; `EDIT` = change to an existing master file.**

### 112.1 THE MASK — arm, render, read back

| file | kind | what | notes |
|---|---|---|---|
| `Shaders/Private/AnomalyVisibleMask.usf` | **MINE, verbatim** | `customStencil ≥ ReservedBase ∧ customDeviceZ ≥ sceneDeviceZ − bias` → tag-valued R8 | ✅ correct as written; reversed-Z handled |
| `Private/AnomalyMaskTypes.h` | **MINE** | `FAnomalyMaskAABB{Min/Max XY, **Count**}`, `FAnomalyMaskResult` | **`Count` is the measurement.** The AABB fields are carried but **NOT used by this cure** — the rect is out of scope |
| `Private/AnomalyStencilSceneViewExtension.{h,cpp}` | **MINE + EDIT** | the mask pass + async readback | ⚠ **EDIT: the reduction (§114).** Everything else ports as-is |
| `Private/AnomalyStencilTag.{h,cpp}` | **MINE + 🚨 EDIT** | tag/restore `{bRenderCustomDepth, CustomDepthStencilValue}`; refcount `r.CustomDepth 3` | 🚨 **ITS `IsRenderableMesh` IS DELETED AND REPLACED BY `AnomalyViewport::IsRenderableComponent`.** This is the `UPoseableMeshComponent` narrowing fix and it is **not optional** (§113.3) |
| `AnomalyCapture.Build.cs` | **EDIT** | already has `Renderer` + the Renderer private include path | ✅ **no dependency change needed** |
| `AnomalyCaptureModule.cpp` | **MINE** | shader-directory mapping for `/Plugin/AnomalyInjector/` | required for the `.usf` |

⛔ **WHAT DOES *NOT* COME ACROSS FROM THE BRANCH:**
1. 🚨 **`IsExcludedFoliageActor` and its call site in `ClassifyRenderableVisibleLive`** — the class
   blacklist the owner ruled is not a fix, resting on a comment `H5` measured false. **DELETED. The
   selector is not touched at all by this cure.**
2. 🚨 **`AnomalyStencilTag::IsRenderableMesh`** — the narrowing defect. Replaced, not ported.
3. **`StencilViz`** (`M_AnomalyStencilViz`, the post-process volume, `IAI.Capture.StencilViz`,
   `tools/create_stencil_viz_material.py`) — debug visualisation. ⚠ **Deliberately excluded: it is the
   only post-Slate path that can bake into a delivered frame** (the branch needed `LOCK-2` to force it
   off during a run). **Not shipping it removes that hazard entirely.**
4. **The branch's `bbox_norm` re-sourcing / `FResolvedFireBox` / `LOCK-1` box plumbing** — that is the
   **label RECT** fix. ⛔ **OUT OF SCOPE** (it is `P6` movement).

### 112.2 THE MEASUREMENT — reduce to a count, using the shared predicate

| file | kind | what |
|---|---|---|
| `Private/AnomalyMaskMeasure.{h,cpp}` | **NEW** | owns the per-event measurement state machine: which events still need a measurement, which tag each holds, when to arm, and the **MAX-across-frames** reduction (§113.4) |
| `Public/AnomalyViewport.h` / `Private/AnomalyViewport.cpp` | ⛔ **UNCHANGED** | `IsRenderableComponent` is **already** public `ANOMALYINJECTOR_API` (`:80`). **The cure CALLS it. Zero edits to the selector.** |

### 112.3 THE REPORTING — `mask{}` provided/value

| file | kind | what |
|---|---|---|
| `Private/AnomalyLabelWriter.h` | **EDIT** | `FSessionEvent` gains internal `MaskState` (tri-state) + `MaskCount` |
| `Private/AnomalyLabelWriter.cpp` | **EDIT** | `:452-459` — `mask.provided` becomes **the tri-state's bool** instead of a hardcoded `false`. 🚨 **`depth{provided:false}` is left exactly as-is.** ⛔ **NO SUB-FIELDS ADDED under `mask` — that would be `P6` SHAPE movement** |

### 112.4 THE INVALIDATION — the accumulator edit before `FinishRun`

| file | kind | what |
|---|---|---|
| `Private/AnomalyCaptureSubsystem.cpp` | **EDIT** | (i) `FSessionEventAccum` gains `TWeakObjectPtr<AActor> TargetActor`, `EMaskState MaskState`, `int32 MaskCount` — **internal struct, not an artifact field**; (ii) arm/collect hooks in `Tick`; (iii) **the veto pass in `FinishRun`, between `DrainAsyncToCompletion()` and `WriteSessionAnnotationFile()`**; (iv) untag on `FinishRun` |
| `Public/AnomalyCaptureSubsystem.h` | **EDIT** | the SVE + measure members, `VetoedEvents` counter |

**The event key already exists and is stable:** `(Id, StartFrame, Target)`
(`AccumulateFrameEvents`), and `StartFrame` is `GFrameCounter` at fire, unique per burst. **No new
identity scheme is required.**

### 112.5 THE COUNTER — dropped events

| file | kind | what |
|---|---|---|
| `Private/AnomalyLabelWriter.{h,cpp}` | **EDIT** | `WriteRunSummary` gains **`vetoed_events`**, beside `non_manifested_events` |

✅ **`run_summary.json` is NOT `P6`.** `P6` is the `annotation.json` contract; `run_summary` already
gained `capture_path` + five `key_ring_*` at `S4` **without moving `P6`**. Precedent is explicit.

---

## 113. `P-2` — THE `LOCK-1` TIMING. **The highest-risk item in the plan.**

🚨 **THE FAILURE MODE, STATED FIRST: for a hide-type anomaly the target is hidden during the
positives, so the mask is 0 BY CONSTRUCTION. A cure that measures a hidden target and reads zero
INVALIDATES EVERY HIDE-TYPE EVENT EVER RECORDED — which is every `blinking` and `missing_object`
event, i.e. the bulk of the dataset.**

### 113.1 What the code already gives us

`IsHideTypeAnomaly` is an explicit table (`m23`): **hide-type = {`blinking`, `missing_object`}**;
non-hide = {`missing_texture`, `lighting_mismatch`, `lod_corruption`, `lod_popping`,
`camera_clipping`, `time_dilation`}. And **`FSessionEventAccum::HiddenByIndex` already records, per
session index, whether the target was hidden** — the `m20` deferred sample.

| anomaly class | is the target drawing during the positives? |
|---|---|
| **non-hide-type** (6 ids) | ✅ **yes, throughout** |
| **`blinking`** | ⚠ **on SOME frames** — `HiddenByIndex[idx] == 0` marks them |
| **`missing_object`** | ⛔ **never, for the whole window** |

⚠ **And the last `LeadIn` frame is NOT a safe pre-fire sample:** `CaptureCurrentFrame()` runs and
**then** `BeginFire()` runs in the same tick, and per `m18` the async grab returns the render of the
**arm tick itself** — so that frame's pixels are already **post-fire**.

### 113.2 THE RULE — one uniform rule, no per-type special-casing

> **ARM THE MASK FOR AN EVENT'S TARGET ONLY ON A TICK WHERE THAT TARGET IS KNOWN NOT TO BE HIDDEN.**

- **non-hide-type** → any in-window tick.
- **`blinking`** → in-window ticks where the deferred hidden sample reads **not hidden**.
- **`missing_object`** → **no in-window tick qualifies** ⇒ fall through to the **post-revert window**:
  `SettleAfterRevert` (2 ticks) + `PostGap` (4 ticks), where `BeginRevert()` → `RevertAllLiveFires()`
  has restored the actor.

✅ **Post-revert measurement is sound here, and the camera is why:** `Q-5` measured the settled camera
holding **dX 0.0004 · dY 0.0003 · dZ 0.0000 · dPitch 0.0000 · dYaw 0.0000** over 200 frames, and
`CB_GateLevel`'s eye is invariant on **844/844**. **The target's drawn extent 6 ticks after the window
is the same quantity as during it.** ⚠ **This assumption is level-dependent and belongs in the tag's
scope statement** (§117) — it does **not** hold in a level with a moving camera or a moving target.

⚠ **`RevertAllLiveFires()` clears `LiveFires`**, so during the post-revert window the fire is gone
from the live set. **This is why `FSessionEventAccum` gains a `TWeakObjectPtr<AActor>`** — the
accumulator, not the live-fire list, is what carries the target into the post-revert window.

### 113.3 The measurement is per-EVENT, not per-FRAME — and it is much cheaper than the branch

The branch armed a mask **on every captured frame**. This cure arms **only until each event has a
usable measurement**, then stops. **Typical: a handful of arms per burst rather than 16.** That is
both a cost win (§114) and a smaller surface for the reduction to be pathological on.

### 113.4 The reduction across frames: **MAX, not FIRST**

**Take the MAXIMUM `Count` across all measured frames for an event.** ⚠ **Deliberate asymmetry: MAX
biases toward ADMITTING.** A frame where the target happens to be transiently occluded, mid-LOD-swap
or clipped yields a low count; **FIRST would let such a frame invalidate a good target. MAX cannot.**
Consistent with Ruling 2's `N-2`, which weighs over-firing as the worse failure.

### 113.5 🚨 THE SAFETY PROPERTY, RESTATED AS THE RULE THAT MAKES `P-2` SURVIVABLE

**If no qualifying tick is ever found for an event — for ANY reason: never un-hidden, target
destroyed, tagging produced no component, readback never resolved — then `MaskState` stays
`NOT_MEASURED`, `mask.provided` stays `false`, and THE EVENT IS ADMITTED.**

⇒ **The `P-2` failure mode cannot produce a systematic false invalidation, because the hide-type case
that has no measurable frame lands in `NOT_MEASURED`, not in `MEASURED_ZERO`.** **The two zeros are
different states, and this is the case that proves why that requirement exists.**

---

## 114. `P-3` — THE REDUCTION

**`T-4`'s precondition is now met, so the fix is IN the plan:** replace the per-pixel
`TMap::FindOrAdd` with a **fixed 256-entry array indexed by tag value** (`ReservedStencilBase = 200`,
headroom under 256), collapsed to a `TMap` once per frame after the scan.

| case | before (`TMap`) | **after (array)** |
|---|---|---|
| all-zero | ~0.1–0.3 ms | ~0.1–0.3 ms |
| 10 % tagged | ~1–3 ms | **~0.2–0.5 ms** |
| **100 % tagged** *(the `H5` case)* | **~9–28 ms** | **~1–3 ms** |

⚠ **ANALYTICAL, NOT MEASURED.** The `M-1` legs measured readback latency, not reduction cost. **A
real number needs a bench leg with the mask built, which is implementation, not planning.**

### Is a GPU-side reduction needed? — **NO. PREMATURE.**

Three reasons, in order of weight:
1. **The array fix alone takes the worst case from ~28 ms to ~1–3 ms**, against a 33 ms budget at
   30 fps.
2. **The mask is armed a few times per burst, not 16** (§113.3), so the *per-run* cost falls by
   roughly an order of magnitude on top of that.
3. **We have no measurement saying it is a problem.** Building a compute-shader reduction now would be
   optimising against an estimate — the same shape as designing around the 12-frame budget that
   `M-1` showed was never real.

⇒ **FILED, NOT BUILT.** Revisit only if a measured leg shows the reduction on the render thread
materially moving `speed_ratio`. ✅ **And `speed_ratio` is already the instrument that would show it**
— it is in every `run_summary.json`, so the trigger is a gate (§115 `G-6`), not a new tool.

---

## 115. `P-4` — GATES, with thresholds and what a FAIL looks like

| # | gate | pass threshold | **FAIL looks like** |
|---|---|---|---|
| **G-1** | **`P6` UNCHANGED, BY MEASUREMENT** | the 48-field key-set check (as run in PART FOURTEEN) — **0 added, 0 removed** against a pre-`m26` banked leg | any key delta ⇒ **`P6` MOVED ⇒ HALT** |
| **G-2** | **`annotation.json` field SET unchanged** | same check, both delivery modes | as above |
| **G-3** | **BYTE-IDENTICAL WHEN THE MEASUREMENT IS ABSENT** | with the cure's switch OFF, `annotation.json` + `labels.jsonl` + `run_summary` (minus run-unique fields, per the `m24` **control-pair** method) **identical** to the `m25` binary | any extra/changed field ⇒ the cure is not inert when off |
| **G-4 = `N-1`** | **IT FIRES** | `InstancedFoliageActor_0_0_0` **and** `BP_SplineSpawn_C`: event **absent** from `anomalies[]`, `vetoed_events ≥ 1`, `mask.provided true` | event still present ⇒ the guard never fired ⇒ **not a guard** |
| **G-5 = `N-2`** | **IT DOES NOT OVER-FIRE** | `StaticMeshActor_49` **and 🚨 `SM_Ramp2`**: event **present**, **NOT** vetoed, `mask.provided true`, count ≫ 0 | either vetoed ⇒ **the reduction is rect-scoped somewhere ⇒ HALT** |
| **G-6** | **PACING NOT MATERIALLY DISTURBED** | `speed_ratio` within the banked band for the same config (≈1.000–1.002 on the `M-1` legs); ring `missed == corrupted == 0` | ratio inflation ⇒ the reduction or `r.CustomDepth 3` is costing real time ⇒ revisit §114 |
| **G-7 = `N-3`** | **ADMITS WHEN BLIND** | measurement forced to fail: `mask.provided` **false**, event **ADMITTED**, `vetoed_events == 0` | a blind run that vetoes ⇒ 🚨 **the two zeros share a representation ⇒ HALT** |
| **G-8 = `N-4`** | **THE BLIND CASE IS LOUD** | a `Warning` naming the reason **and** `mask.provided:false` in the artifact | silent ⇒ **`G119`'s diagnostic answers "today's output" ⇒ HALT** |
| **G-9** | **DELIVERY ORTHOGONALITY (`m25`)** | a pose-matched delivery pair: event set, `mask.provided`, `vetoed_events` **identical** across modes | divergence ⇒ **`m25`'s certified property is broken ⇒ HALT** |
| **G-10** | **`A54` STILL CERTIFIES ON THE CONTROL** | `CB_GateLevel` / `StaticMeshActor_49` leg: ALL-ALIGNED, **≥3 counted events** | below 3 ⇒ the leg is **INVALID**, not evidence (§116 risk 4) |

⚠ **`G-3` and `G-7` must be run BOTH WAYS (`G96`).** A cure that is inert when off proves nothing
unless the same instrument shows it is *not* inert when on.

---

## 116. `P-5` — RISK

| # | risk | assessment |
|---|---|---|
| **1** | **SEEDED REPRODUCIBILITY** | ✅ **CONFIRMED UNAFFECTED, from source.** (c) touches nothing at or before selection: `TryFireOnce` is **unmodified**, the `Stream` draw order is **untouched**, and the veto acts on the accumulator at `FinishRun`. Under capture the interval timer is not even used — `BeginFire()` calls `TryFireOnce()` directly. ⚠ **One second-order path checked and cleared:** fixed timestep means GPU cost cannot change the number of game ticks, so tagging cannot perturb the stream either. **`G-3` re-proves it empirically.** |
| **2** | **`m23` ACCOUNTING** | ⚠ **REAL, needs a stated precedence rule.** `non_manifested_events` counts hide-type events with zero sampled-hidden frames; a veto is a **different** category. An event can be **both**. **RULE: evaluate `manifested` first (`m23` logic byte-unchanged), then the veto; a non-manifested event is ALREADY excluded from positives, so vetoing it adds nothing and MUST NOT be double-counted.** Proposed: **veto only events with `manifested == true`.** Keeps the two counters disjoint and `m23` untouched |
| **3** | **`m25` DELIVERY ORTHOGONALITY** | ⚠ **REAL.** `S4-4` certified **127 invariants identical** across delivery mode. The mask is delivery-independent **by construction** (the SVE does not consult `bDeliveryMode`), so the veto should be too — **but "should" is why `G-9` exists as a gate rather than an argument** |
| **4** | 🚨 **`A54` / THE ≥3-EVENT VALIDITY CONDITION** | 🚨 **THE SUBTLEST ONE. A veto REMOVES events, and `≥3 counted events per leg` is a VALIDITY condition (A31 / `m24`). A cure that vetoes 6 of 8 events on a leg silently turns that leg from EVIDENCE into INVALID** — and A54's oracle would be judging a shrunken set. **Mitigation: A54 gate legs must use the control target, which `G-5` proves is never vetoed. Stated so it is not discovered.** |
| **5** | **`run_summary.positive_frames` vs a vetoed event** | ⚠ `positive_frames` counts **fire-active** frames and is unchanged by design (`m23`: *fire-active ≠ manifested*). A vetoed event's frames stay counted ⇒ `run_summary` and `annotation.json` disagree. **Same family as `L2`; must be stated, not silently reconciled** |
| **6** | **`r.CustomDepth 3` on a HOST GAME that already uses custom stencil** | ⚠ **GAME-AGNOSTIC RISK.** The branch's tag code saves/restores per component and refcounts the cvar — good — but a host title using custom stencil for its own effects shares a 0–255 space with our 200+ tags. **`ReservedStencilBase = 200` is a convention, not a reservation.** Belongs in the scope statement |
| **7** | 🚨 **THE MASK MEASURES DRAWN SILHOUETTE, NOT VISUAL EFFECT** | 🚨 **THE DEEPEST LIMIT, and it is A35/`SM_Ramp2` in a new place.** A target whose contribution is largely **shadow, reflection or GI** has those pixels **outside its own silhouette by construction**, so the mask under-counts it. `SM_Ramp2` still draws real geometry (peak-in `0.1785`), so `G-5` should pass — **but a target that is ALMOST ENTIRELY shadow would be wrongly vetoed and we have never measured one.** ⛔ **MUST be in the tag's scope statement as a KNOWN, UNMEASURED limit** |
| **8** | **Tagging mutates render state on a live target** | `bRenderCustomDepth` is saved/restored per component. ⚠ **`StencilViz` is deliberately NOT ported** (§112.1), which removes the only path by which custom stencil could reach delivered pixels |

---

## 117. `P-6` — MILESTONE SHAPE: **`m26`**

**One milestone · one `feat(capture)` commit · annotated tag `m26` carrying its own scope statement,
per the `m24`/`m25` precedent.** Owner Play-gate smoke **before** the tag, as the standing rule
requires.

### The scope statement `m26` will carry — drafted now, to be corrected by what the gates actually show

> **`m26` — `H5` class (ii) mitigation: an event whose target is MEASURED to draw nothing is removed
> from `annotation.json` before it is written.**
>
> **WHAT IS CERTIFIED:** the cure fires on two measured `H5`-shaped targets
> (`InstancedFoliageActor_0_0_0`, `BP_SplineSpawn_C`), does not fire on two known-good targets
> (`StaticMeshActor_49`, `SM_Ramp2` — the A35 case), admits byte-identically when the measurement is
> absent, and is loud when blind. `annotation.json`'s field SET is UNCHANGED (`P6` not moved);
> `mask.provided` moves `false → true` in a slot that already shipped.
>
> 🆕 **AMENDED AGAIN (PART THIRTY-ONE — the veto ships; these paragraphs travel with the tag):**
> **THE VETO IS ZERO-ONLY.** An event is removed from `annotation.json` **if and only if** it
> manifested AND its target was **MEASURED at ZERO drawn pixels**. `NOT_MEASURED` is never vetoed;
> a measured NON-ZERO count is never vetoed **however small a fraction of its claimed extent it
> is**. **There is no ratio and no threshold.**
> 🚨 **THE ACCEPTED COST:** *"`m26` vetoes only targets measured at ZERO drawn pixels. A target
> that OVER-CLAIMS — measured non-zero but far below its claimed extent, such as the
> `InstancedFoliageActor` measured at 5,689–13,342 px against a claimed 921,600 px (the entire
> frame) — IS NOT VETOED and ships as a valid label. `m26` is a PARTIAL cure for `H5`: it removes
> the zero-contribution case and leaves the over-claim case. The over-claim rule requires a
> calibration campaign including complex-silhouette legitimate targets, which do not exist in the
> current measured set."*
> ⚠ **`A35`, as a RULING with its reason:** a zero-silhouette target can still have indirect visual
> effect — `BP_SplineSpawn_C`'s banked hide showed a small in-bbox luma change (`0.0175`) while the
> mask reads exactly zero. **`m26` vetoes it anyway, because the label points at the OBJECT and not
> at its shadow.**
> **`L1`–`L3`:** frames are already on disk and are **NOT un-written** · **a post-`m26` event count
> is NOT comparable with a pre-`m26` one; `vetoed_events` carries the delta** · `labels.jsonl`
> (delivery OFF) is prebuilt and uncorrectable, so **delivery OFF and ON WILL DISAGREE on event
> content**.
> **`vetoed_events` and `non_manifested_events` are DISJOINT by construction** — *"the target
> contributed no pixels to hide"* vs *"the hide never showed in pixels"*.
>
> 🆕 **AMENDED (PART THIRTY, Rulings 2–3 — these three paragraphs travel with the tag):**
> **(1) `run_summary.json` gains FOUR fields — `mask_probe_arms`, `mask_residual_discards`,
> `mask_nopass_discards`, `vetoed_events`.** `annotation.json`'s field SET is unchanged. *(The
> declared number is `+4`, and `+4` is what ships.)*
> **(2) `mask.provided` `false` NEVER means "the target drew nothing" — it means no measurement
> exists, and such an event carries exactly as much evidence as it did before `m26`: none from this
> measurement.**
> **(3) `framesNoPass` / `mask_nopass_discards` counts frames where the custom-depth pass did not
> produce for this target. Causes include Nanite geometry, frustum culling, and any other route by
> which the target is absent from the view's relevant set. It is NOT a Nanite counter. In all cases
> the frame is discarded and the event tends toward `NOT_MEASURED`, which ADMITS.**
> ⚠ **`SM_Ramp2` is NO LONGER the `N-2` control** — it is Nanite and therefore unmeasurable here;
> it serves as the KNOWN-NANITE control. `N-2` is `StaticMeshActor_73`. 🚨 **The A35 over-fire
> property is UNTESTED — no non-Nanite A35-shaped target exists in the measured set.**
>
> ⛔ **WHAT IS NOT:**
> - **NO INCIDENCE CLAIM.** `H5`'s incidence in client data is still unknown, and `m26` does not
>   measure it.
> - **`H5` class (i) is ENUMERATED, NOT OBSERVED** — `m26` may or may not catch it; that is untested
>   because no instance exists here.
> - **`H4` IS NOT CURED.** Its cure remains `feature/stencil-capture`, still untouched. **Whether one
>   measurement serves both is UNESTABLISHED** (Ruling, PART TWELVE §3.3).
> - 🚨 **THE MASK MEASURES DRAWN SILHOUETTE, NOT VISUAL EFFECT.** A target whose contribution is
>   mostly shadow/reflection/GI is under-counted. `SM_Ramp2` passes; **an almost-entirely-shadow
>   target has never been measured.**
> - 🚨 **THE POST-REVERT MEASUREMENT ASSUMES A SETTLED CAMERA AND A STATIONARY TARGET** (measured:
>   `dYaw 0.0000` over 200 frames; `CB_GateLevel` eye invariant 844/844). **It does NOT hold in a
>   level with a moving camera or a moving target**, and that is unmeasured.
> - **`ReservedStencilBase = 200` is a CONVENTION, not a reservation** — a host title using custom
>   stencil shares the space.
> - **`L1`–`L3` carried:** frames stay on disk; `vetoed_events` must be read alongside
>   `positive_frames`; **delivery OFF and ON disagree** because `labels.jsonl` is prebuilt.
> - Carried from `m25`: modal pose only, `VideoFps` 30 only, alignment certified at 1280×720 and
>   1281×721 only.

---

## 118. State after PART FIFTEEN

| | |
|---|---|
| plan | **written, NOT approved. NOTHING IMPLEMENTED.** |
| production code | unchanged since PART FOURTEEN's log-only `M-1` instrument |
| `P6` | **DOES NOT MOVE** — plan explicitly forbids sub-fields under `mask` |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED at `76cac74`** |
| ⛔ next | **owner approval of the plan.** No file is created until then |

**THE PLAN'S RISKIEST ITEM, NAMED:** **`P-2`. If the cure measures a hidden target and reads zero it
invalidates every hide-type event ever recorded. The design survives it ONLY because "no qualifying
frame" lands in `NOT_MEASURED` rather than `MEASURED_ZERO` — which is exactly why the two zeros were
required to be different states.**

---

# PART SIXTEEN — `m26` SLICE 1: written and compiling; **VALIDATION HALTED ON A COOK**

**SLICE 1 IS WRITTEN AND BUILDS CLEAN. It CANNOT be validated on the staged build, and the reason is
a fact nobody in this design had — including me.** ⛔ **NO TAG. `P6` NOT MOVED.**
`feature/stencil-capture` **READ-ONLY at `76cac74`, never checked out.**

---

## 119. The four amendments, recorded

| # | amendment | disposition |
|---|---|---|
| **A-1** | **Veto only `manifested == true` events** | ✅ **ADOPTED AS PROPOSED.** Definitions recorded side by side in §119.1 |
| **A-2** | **Risk 4 becomes gate `G-11`** | ✅ **ADOPTED.** Added to the gate table, plus the client-facing derivation note (§119.2) |
| **A-3** | **MEASURE the stencil collision, do not only disclaim it** | ✅ **ADOPTED, AND IT IS DETECTABLE — TWO WAYS** (§119.3) |
| **A-4** | **Risk 7 gets a control** | ✅ **ADOPTED.** `G-5` extended (§119.4) |

### 119.1 `A-1` — the two counters, side by side, in these words

> **`non_manifested_events`** — *"the hide never showed in pixels."* A hide-type event where **no
> captured frame sampled the target hidden**. Shipped at `m23`.
>
> **`vetoed_events`** — *"the target contributed no pixels to hide."* An event whose target was
> **MEASURED** to draw nothing, and which was therefore removed from `annotation.json`. New at `m26`.

**They answer different questions and they are DISJOINT: only `manifested == true` events are
eligible for a veto**, so an event is never counted in both. `m23`'s logic is byte-unchanged.

### 119.2 `A-2` — `G-11`, and the derivation note

**`G-11`: on every certifying leg, report COUNTED EVENTS BEFORE and AFTER the veto pass, as two
numbers. If the after-count drops below 3 on a leg that would otherwise certify, THAT LEG IS
INVALID** — reported as invalid, never silently graded on the reduced set.

🚨 **AND THE CLIENT-FACING HALF, which the gate alone does not cover:** *"a consumer computing event
counts from `annotation.json` after `m26` is counting **POST-VETO** events, and a **pre-`m26` session
is not comparable on that number**. `vetoed_events` in `run_summary.json` gives the delta."* **This
goes where a client-facing reader hits it — `docs/client-readme.md` and the `m26` tag scope — not only
in the gate table.**

### 119.3 `A-3` — the collision IS detectable, two ways, and both are implemented

⛔ **The disclaimer does NOT stand alone.** Slice 1 implements both detectors:

| # | detector | catches |
|---|---|---|
| **1** | **Game-thread property read-back** — `VerifyActorStillTagged()` re-reads `bRenderCustomDepth` and `CustomDepthStencilValue` off the component on a later tick and compares against what we wrote | a host game that **re-asserts its own stencil value** on the component — the most likely collision mode |
| **2** | **Unassigned-reserved-tag detection in the mask** — the reduction reports any tag in `[200,255]` that **this run never assigned** | a host game **writing into our reserved range** on its own actors |

**On either detection: the record's measurement is discarded, the state stays `NOT_MEASURED` (⇒
ADMIT), and a `Warning` fires naming the tag and the reason.**

⚠ **Honest limit, stated rather than glossed:** if a host game overwrote our 200 with a value **below**
`ReservedStencilBase`, detector 1 catches it on the component property, but a collision that happened
only in the GPU stencil buffer without touching the component property would be invisible. **Detector
1 covers the realistic path; full coverage would need a stencil read-back we are not doing.**

⚠ **The owner's framing is exactly right and is recorded:** this is the **environmental twin** of the
`USkeletalMeshComponent` narrowing — *a measurement that reads zero for a reason unrelated to the
target's contribution*.

### 119.4 `A-4` — `G-5` extended

**Report, for each negative-control target, its peak-IN and peak-OUT pixel change ALONGSIDE the veto
decision.** `SM_Ramp2` is the known case (peak-OUT **0.2955** > peak-IN **0.1785**) and **should be
ADMITTED** — giving a **measured** data point that a substantially-outside-the-rect target survives
the cure, rather than an argument that it should. 🚨 **If a target is ever vetoed while carrying a
large peak-OUT, that is the shadow limit firing in real data and it HALTS for a ruling.**
⛔ **No hunt for an almost-entirely-shadow target. It stays unmeasured and disclaimed.**

---

## 120. What SLICE 1 actually is, as written

**MEASURE ONLY. LOG OUTPUT ONLY. No artifact field, no veto, `mask{provided}` stays `false`.**
Switch `IAI.Capture.Mask <0|1>`, **default OFF**.

| file | kind | what |
|---|---|---|
| `Shaders/Private/AnomalyVisibleMask.usf` | **MINE** | the occlusion-correct silhouette test, verbatim |
| `Private/AnomalyMaskTypes.h` | **NEW** | per-tag result + **`EAnomalyMaskState` tri-state** |
| `Private/AnomalyStencilTag.{h,cpp}` | **MINE + EDIT** | 🚨 **`IsRenderableMesh` DELETED; tagging now calls `AnomalyViewport::IsRenderableComponent`** — the narrowing fix. Plus `VerifyActorStillTagged()` for `A-3` |
| `Private/AnomalyMaskSceneViewExtension.{h,cpp}` | **MINE + EDIT** | the mask pass; **`T-4`'s fixed 256-entry array replaces the per-pixel `TMap`**; unassigned-tag detection |
| `Private/AnomalyMaskMeasure.{h,cpp}` | **NEW** | the per-event state machine: `LOCK-1` arm rule, **MAX-across-frames**, ≤4 arms/event, tri-state |
| `Private/AnomalyCaptureSubsystem.{h,cpp}` | **EDIT** | switch, record registration, arm/collect in `Tick`, the slice-1 summary log |
| `Private/AnomalyCaptureModule.cpp` | **MINE** | shader-directory mapping |
| `AnomalyCapture.Build.cs` | **EDIT** | **+`Projects`** only — `Renderer` and the Renderer private include path were already there |

⛔ **NOT ported, as ruled:** the foliage blacklist · `IsRenderableMesh`'s narrowing · `StencilViz` ·
the `bbox_norm` re-sourcing. **`AnomalyViewport` is UNCHANGED — the cure calls the predicate.**

✅ **Compiles clean** (Development Win64, exit 0, 198 s).

---

## 121. 🚨 THE HALT — a new global shader CANNOT be delivered by a code-only hot-swap

**Staged the slice-1 exe (`15A87075`) by hot-swap, pak quartet untouched. A44 confirmed every new
symbol present in the staged binary, both encodings** (`IAI.Capture.Mask` utf16=6, `M26S1 EVENT`
utf16=1, `AnomalyVisibleMask` utf16=3, the `.usf` virtual path utf16=1, alongside pre-existing
`IsHideTypeAnomaly` utf16=1 — **so the scan is sound, not blind**).

**The leg produced NO ARTIFACT on 3 of 3 attempts. Cause read from the log, not inferred:**

```
Fatal error: [ShaderCompiler.cpp] [Line: 6931]
Missing global shader FAnomalyVisibleMaskPS's permutation 0, Please make sure cooking was successful.
```

### 121.1 Two properties of this failure that matter more than the failure

1. 🚨 **IT FIRES AT ENGINE INIT, BEFORE ANY CAPTURE STARTS.**
2. 🚨 **IT FIRES WITH THE MASK SWITCH *OFF*.** Global shader-map verification **does not consult a
   runtime cvar.**

⇒ **`IMPLEMENT_GLOBAL_SHADER` IS NOT GATED BY ANY RUNTIME SWITCH.** A default-OFF console variable
does **not** make a global shader optional — **the binary cannot boot without it.**

🚨 **THIS BREAKS AN ASSUMPTION THE PLAN INHERITED FROM `S3a`.** `G-3` ("byte-identical when the
measurement is absent") was written on the `S3a` precedent, where a default-OFF switch made new code
**structurally inert**. **That precedent does not extend to a global shader**: the shader's cost is
paid at load, unconditionally, and *"inert when off"* is **unobtainable by a switch** for this class
of change. **The only inert configuration is a build that does not contain the shader at all.**

### 121.2 Why this is a HALT and not something to just do

**Validating slice 1 requires a FULL COOK** (runbook §8.6), which:
- **retires the current build quartet** — the pak half (`utoc 939B9C9B` / `ucas 8A602D4D` /
  `pak 7CAE22DD`) is what every `H4`/`H5`/`M-1` measurement in Parts Two–Fourteen was taken on;
- is an operation this project has **repeatedly treated as owner-sequenced** — `G118`'s closure was
  explicitly sequenced by the owner *"AFTER the current measurement sequence and NEVER inside one"*;
- **`G92`** wipes `Saved` and the archive step's destructiveness is **not established** ("the
  precaution stays").

⇒ **REPORTED, NOT PERFORMED. The cook is the owner's call.**

### 121.3 Bench state — restored and verified

**Staged exe restored to `101AFEA4`** (the `m25` baseline, preserved and hash-verified in
`_binary_baselines`). ✅ **It boots and writes full sessions** — three restore-smoke attempts, **97
files each**, banked.

⚠ **Those three attempts FAILED THE B1 POSE GATE (3 of 3)** — `modal_rot (0, 2.27, 0)`, `distinct=10`,
`modal 72.9 %`, bbox width `69.0` against `CALIB_BBOX`'s `306.1`, ratio non-uniform
`(—, 0.9256, 0.2254, 0.7176)`. **By the harness's own discriminator that reads as genuine A47
bifurcation rather than resolution scope.** ⛔ **CAUSE NOT ESTABLISHED, and it is not attributed** —
3 consecutive is above the recorded ~2-in-5 rate, and the box was memory-pressured (~3.7 GB free)
during a session that also ran three engine-fatal launches. **Association only. The harness correctly
offered no verdict.** ⚠ **The point that matters for the halt: the bench BOOTS and PRODUCES
ARTIFACTS. The pose gate is a separate, pre-existing, known-flaky condition and is NOT evidence about
slice 1.**

⚠ **A gap in my own handling, recorded:** I overwrote the staged `1EBA8944` (the `M-1` instrument
build) without archiving it first, having archived `101AFEA4` before the previous swap. **The loss is
bounded — `1EBA8944` is reproducible from commit `0185c10`, and `M-1`'s results are already banked and
recorded — but the rule I followed once, I did not follow twice.**

---

## 122. What the owner has to rule on

| # | question |
|---|---|
| **1** | 🚨 **Cook or not?** Slice 1 cannot be validated without one, and a cook retires the quartet every prior measurement was taken on. |
| **2** | If cooking: **the map set must be declared IN WRITING before the cook** (runbook §8.6 step 3) — `CB_GateLevel` + `MainMenu` + `MainWorld` + `Entry`, i.e. the current set — because *"a cook that silently omitted a map is what created the `MainWorld` situation in the first place."* |
| **3** | **`G-3`'s wording needs amending** given §121.1: *"byte-identical when the measurement is absent"* is **unobtainable by a switch** for a global shader. The honest form is a **control pair against a build that does not contain the shader** — i.e. `m25` vs `m26`-switch-OFF, which is exactly the `m24` control-pair method already in the plan. |

---

## 123. State after PART SIXTEEN

| | |
|---|---|
| slice 1 | **WRITTEN, COMPILES CLEAN, NOT VALIDATED** |
| halt | **a new global shader needs a COOK; a hot-swap cannot deliver it** |
| staged build | **restored to `101AFEA4`** + unchanged pak quartet; boots and writes sessions |
| `P6` | **NOT MOVED** — slice 1 writes to the log only |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED at `76cac74`** |
| bank | **154 → 158** (3 restore-smoke attempts + the shader-halt log evidence) |
| ⛔ next | **the owner's ruling on the cook** |

**WHAT THIS PART SETTLES, beyond the halt:** **a default-OFF switch does NOT make a global shader
inert — the binary cannot boot without it. That retires, for this class of change, the `S3a`
precedent that a switch buys structural inertness, and it means `G-3` must be a control pair against
a build without the shader rather than a switch-OFF leg on the same binary.**

---

# PART SEVENTEEN — the cook preconditions: **one PASSED, one HALTED ON DISK**

**Owner ruled OPTION A (cook). Precondition 1 (preserve the quartet) is DONE and VERIFIED.
Precondition 2 (declare the map set) is RECORDED. ⛔ THE COOK DID NOT RUN — free space is
0.94 GB against the owner's own ~10 GB floor.** **NO PRODUCTION CODE CHANGED. NO TAG. `P6` NOT
MOVED.** `feature/stencil-capture` **READ-ONLY at `76cac74`.**

---

## 124. `RULING 1` — `G-3` IS AMENDED, and the reason travels with it

**`G-3` was:** *"byte-identical when the measurement is absent"*, run as a **switch-OFF leg on the
same binary**. That rested on `S3a`'s precedent — *"switch-OFF inertness is STRUCTURAL; there is no
way to reach the code."*

🚨 **`G129` MADE THAT UNOBTAINABLE, BY MEASUREMENT.** A global shader's cost is paid **at load,
unconditionally**; `IMPLEMENT_GLOBAL_SHADER` consults no cvar; the binary **cannot boot** without the
shader in the container. **There is no switch-OFF state to compare against.**

**`G-3` (AMENDED):**

> A **CONTROL PAIR** against a build that does **NOT** contain the shader — `m25`'s preserved binary
> versus `m26`-with-the-switch-OFF. **The run-unique field set is established empirically from a
> same-binary pair FIRST; the test pair's difference set must be a SUBSET of it. EXTRAS MUST BE 0.**
> Decided by a rule fixed in advance, never by judgement after seeing a diff.

**This is the `m24` C1-replacement method, which the plan already carried.** ✅ **It still runs BOTH
WAYS (`G96`): inert-when-off proves nothing unless the same instrument shows it is NOT inert when on.**

⚠ **RECORDED FOR A FUTURE READER: switch-OFF inertness was RETIRED BY MEASUREMENT, NOT WEAKENED BY
CONVENIENCE.** The gate did not get easier; it got *possible*. The old wording described a state that
does not exist for this class of change.

---

## 125. PRECONDITION 1 — the quartet is preserved and hash-verified. ✅ PASS

**Copied to `_binary_baselines\m25-h4h5m1-measurement-build\` — outside `Builds\`, where a stage
cannot reach — and verified BY HASH AT THE NEW LOCATION before anything else (A62: a copy that ran is
not a copy that landed).**

| file | bytes | source | destination | match |
|---|---|---|---|---|
| `StackOBot.exe` | 240,540,672 | `101AFEA4` | `101AFEA4` | ✅ |
| `StackOBot-Windows.utoc` | 268,036 | `939B9C9B` | `939B9C9B` | ✅ |
| `StackOBot-Windows.ucas` | 284,469,920 | `8A602D4D` | `8A602D4D` | ✅ |
| `StackOBot-Windows.pak` | 10,115,703 | `7CAE22DD` | `7CAE22DD` | ✅ |
| `global.utoc` | 539 | `C70ECDAA` | `C70ECDAA` | ✅ |
| `global.ucas` | 1,833,008 | `A16A18A8` | `A16A18A8` | ✅ |

**6 of 6 match.** `README.md` updated with what the quartet is, **what rests on it** (every result in
PARTS TWO–FOURTEEN, enumerated), and that it is **COMPLETE** unlike the exe-only `.m25-baseline`.

⚠ **Also recorded there rather than quietly fixed: `1EBA8944` (the `M-1` instrument exe) was NOT
archived before being overwritten** — bounded (rebuilds from `0185c10`, results banked), but an
instance of this file's own rule not being followed.

---

## 126. PRECONDITION 2 — the map set, declared IN WRITING before the cook

> **`CB_GateLevel` + `MainMenu` + `Entry` + `MainWorld`** (with its World Partition external actors).

**Identical to the last cook.** `CB_GateLevel` **RETAINED, non-negotiable** — every `m25`
certification, `B1`'s calibration and the A54 oracle are scoped to it. `MainWorld` **RETAINED** —
`H5`'s reproduction and **both** known `H5`-shaped targets live there, and `G-4`/`G-5` need them.

**Post-cook gate, before any leg:** `verify_cooked_maps.ps1`, which scans **both encodings** because
the container's encoding is not stable and a single-encoding scan returns a clean-looking *"no maps
cooked"*. `MainWorld` absent ⇒ HALT and report, do not re-cook blind. `CB_GateLevel` absent ⇒ HALT
immediately.

⛔ **NOT YET RUN — there is no new container to read.**

---

## 127. 🛑 THE HALT — DISK. 0.94 GB free against a ~10 GB floor.

**The owner's precondition was explicit:** *"If preserving it would take free space below ~10 GB,
HALT and say so — I will rule on bank retention rather than have you skip the preservation."*

| | |
|---|---|
| free after the PART TWELVE prune | **19.12 GB** |
| free at the start of this part | **1.37 GB** |
| free after preserving the quartet (~537 MB) | **0.94 GB** |

⚠ **The preservation was NOT skipped — it completed and verified first.** The floor was crossed
before this part began; preserving the quartet cost only ~537 MB of it.

**Where the ~17.7 GB went between PART TWELVE and here — this session's own work:** two full UBT
builds with UHT reflection, three engine-fatal launches, shader-compile attempts, and eight capture
legs. `StackOBot\Intermediate` alone is now **14.54 GB**.

### 127.1 The candidates, classified — ⛔ NOTHING WAS DELETED

| tree | size | class |
|---|---|---|
| `StackOBot\Intermediate` | **14.54 GB** | ✅ **REGENERABLE** — build intermediates. Deleting forces a full rebuild (~3 min measured) |
| `StackOBot\.vs` | **4.72 GB** | ✅ **REGENERABLE** — Visual Studio cache; the command-line build does not use it |
| `Builds\BenchGate\...\Saved\` | **5.66 GB** | ✅ **VERIFIED DUPLICATE — 75 sessions, ALL 75 present in the bank BY SESSION ID.** The PART TWELVE prune never covered this tree (it targeted `Binaries\Win64` leg dirs) |
| `Builds\MidRepro` | **6.57 GB** | ⚠ **NEEDS A RULING** — the `m17` repro harness, a documented validation asset, untouched since 2026-07-15 |
| `Builds\Windows` | **3.38 GB** | ⚠ **NEEDS A RULING** — the pre-cook 3-map build; historical evidence for `S-1`/`G87`'s correction |
| `StackOBot\Saved\AnomalyCaptures` | **3.89 GB** | 🚨 **DO NOT TOUCH — 21 sessions, ZERO banked.** Editor/PIE-era captures, entirely unbanked evidence |
| `_bench_sessions_bank` | **16.28 GB** | ⚠ **the owner's named lever** (bank retention) |

🚨 **THE ONE ACTIONABLE FINDING IN THAT TABLE: `Saved\AnomalyCaptures` holds 21 sessions and NOT ONE
is banked.** That is 3.89 GB of unbanked evidence sitting in the project tree — **the same shape as
PART TWELVE's `RESCUE_P12_*` near-miss, found by the same session-ID method, and it must be banked
before anything near it is deleted.**

### 127.2 What a cook needs

The last full cook produced a 284 MB `.ucas`, but the **working set** is far larger — `Saved\Cooked`
plus `StagedBuilds` plus the archive copy. **0.94 GB is not enough to start, and a cook that runs out
of disk mid-way is the worst possible outcome: a half-written container behind a build that still
boots.**

⇒ ⛔ **HALTED, AS INSTRUCTED. NOTHING DELETED. The ruling is the owner's.**

---

## 128. State after PART SEVENTEEN

| | |
|---|---|
| precondition 1 | ✅ **PASS** — quartet preserved, **6/6 hash-verified at the new location**, README updated |
| precondition 2 | ✅ **RECORDED** — map set declared; the read-back gate has nothing to read yet |
| the cook | ⛔ **DID NOT RUN** |
| slice 1 | unchanged — written, compiles clean, **still unvalidated** |
| staged build | `101AFEA4` + the unchanged quartet; boots and writes sessions |
| `P6` | **NOT MOVED** · production code **UNCHANGED this part** |
| tag | **none** · `feature/stencil-capture` **UNTOUCHED at `76cac74`** |
| ⛔ next | **the owner's disk ruling**, then the cook |

**WHAT THIS PART ADDS BEYOND THE HALT: `Saved\AnomalyCaptures` holds 21 sessions and ZERO are
banked — 3.89 GB of unbanked evidence found by the session-ID method while looking for something
else entirely.**

---

# PART EIGHTEEN — cleanup executed, evidence banked, **the cook is IN FLIGHT**

**Owner authorised Rulings 1–4. All cleanup is DONE and every figure below is measured. The cook is
RUNNING and will not finish inside this turn — the reason is measured too.** **NO PRODUCTION CODE
CHANGED. NO TAG. `P6` NOT MOVED.** `feature/stencil-capture` **READ-ONLY at `76cac74`.**

---

## 129. 🚨 THE ORDERING IN RULING 1 WAS IMPOSSIBLE, AND WHY — the first thing that happened

**Ruling 1 said: bank `Saved\AnomalyCaptures` FIRST, before anything is deleted.** I started there.

**It failed on the first session: `Copy-Item : There is not enough space on the disk.`**
**Banking 3.89 GB requires 3.89 GB, and 0.94 GB existed.** The ordering was written assuming room
that the previous part had already reported was gone.

⚠ **AND IT LEFT EXACTLY THE ARTIFACT WE HAD JUST FINISHED WARNING ABOUT: a PARTIAL bank directory —
652 files / 891 MB of a 21-session copy — indistinguishable by name from a complete one.** It was
**deleted immediately** and the source **re-verified intact (21 sessions, 2,967 files, 3.89 GB)**
before anything else ran.

### 129.1 The resolution, and the principle it rests on

**The INTENT of "bank first" is *never delete evidence before it is safe*. That intent is preserved by
a different order:**

> **FREE ONLY WHAT CONTAINS NO EVIDENCE AT ALL → THEN BANK → THEN FREE THE VERIFIED DUPLICATES.**

`Intermediate` and `.vs` are **build artifacts containing no evidence of anything**. Freeing them
cannot endanger a single session. Only the **third** tree (`Builds\BenchGate\...\Saved`) holds capture
data, and it was not touched until after the banking completed and was verified.

⇒ **Nothing that could be evidence was deleted before it was banked. The literal order changed; the
protection did not.** → **`G130`**

---

## 130. `RULING 1` — the 21 PIE-era sessions are banked ✅

| | |
|---|---|
| bank dirs before | **158** |
| bank dirs after | **159** *(one container, `PIE_UNBANKED_SWEEP_20260819/`, holding all 21 sessions — the `CAL`/`ANCH` precedent)* |
| distinct session ids | 162 → **183** |
| verification | **21/21 IDENTICAL** per-file `{relative path → size}` manifest on copy |

⚠ **`session_20260713-113639` has 3 files / 0 MB — incomplete. BANKED ANYWAY and marked**, per the
ruling: *a truncated artifact is still evidence of something; a deleted one is not.*

🚨 **ONE OF THE 21 IS CITED EVIDENCE: `session_20260817-132214` IS THE `m23` OWNER PLAY-GATE SMOKE** —
the run CLAUDE.md records as *"90 frames, 1068×604, fps 30, 8 blink events, gapped cadence byte-exact
to the historical shape, `manifested: true` 8/8"*, and **the first confirmation of the `m23` fix in
real gameplay content rather than synthetic `CB_GateLevel`.** It was sitting unbanked in the project
tree, and a disk sweep is what found it.

**Third instance of the session-ID method finding unbanked evidence** — PART SIX's nine dirs, PART
TWELVE's two rescues, these 21. **A name-based sweep found none of them.** Recorded in the bank
`README.md`.

⚠ **The originals were LEFT IN PLACE**, per the ruling, until the cook succeeds.

---

## 131. `RULING 2` — the three trees, freed, with per-tree figures

| step | tree | free after | recovered |
|---|---|---|---|
| — | *(start)* | **0.94 GB** | — |
| 1 | `StackOBot\.vs` | **5.59 GB** | **4.65 GB** |
| 2 | `StackOBot\Intermediate` | **20.15 GB** | **14.56 GB** |
| 3 | *(banking the 21 sessions)* | **16.26 GB** | −3.89 GB |
| 4 | `Builds\BenchGate\...\Saved` | **21.93 GB** | **5.67 GB** |

✅ **`G-3`'s condition on tree 3 was honoured exactly: the re-verification and the deletion ran in ONE
script, with no other work between them** — *"that gap is where a stale verification becomes a wrong
one."* Result: **75 of 75 duplicate by SESSION ID *and* per-file manifest**, zero failures, so nothing
had to be spared.

⛔ **UNTOUCHED as instructed:** `_binary_baselines` (both preserved quartets) · the session bank ·
`Builds\MidRepro` · `Builds\Windows` · `Saved\AnomalyCaptures` originals.

✅ **`G92` RE-BANK CHECK BEFORE COOKING: 0 unbanked sessions** in `Builds\BenchGate` (7 sessions) and
`StackOBot\Saved` (21 sessions). Matched **by session ID**.

---

## 132. `RULING 4` — disk is now an instrument

**`setup-runbook.md` §8.6 gains a STEP 0 — a go/no-go floor before the cook:** ≥15 GB **GO** ·
10–15 GB **marginal** · <10 GB **NO-GO, do not start**. It carries the measured working-set figures
and names the trees that are **not** free space (`_binary_baselines`, the bank, `MidRepro`, and
**`Builds\Windows` — the physical evidence behind `S-1`/`G87`**).

🆕 **`G130`** — generalised: *an operation's WORKING SET is not its OUTPUT SIZE, and running out of
room mid-way yields a half-written artifact behind a system that still starts.* It carries the
284 MB-output-vs-multi-GB-working-set figure, the partial-copy trap measured here, and the
preservation-vs-cleanup ordering resolution from §129.

---

## 133. THE COOK — running, and slow for a measured reason

**Launched with the declared map set, unchanged:**
`CB_GateLevel` + `MainMenu` + `MainWorld` *(`Entry` arrives by default)*.

⚠ **IT IS A FULL 761-ACTION REBUILD, NOT AN INCREMENTAL ONE — because `Intermediate` was deleted to
buy the disk space the cook needed.** That cost was known and accepted; the runbook records it as
*"forces a full rebuild, ~3 min"*, which was measured on a **10-process** build.

🚨 **IT IS RUNNING AT ONE PROCESS, AND THE REASON IS MEASURED, NOT GUESSED.** UBT's own line:

```
Requested 1.5 GB free memory per action, 2.36 GB available: limiting max parallel actions to 1
Building 761 actions with 1 process...
```

**A second `UnrealEditor` is resident at 3.19 GB working set** — **`G97`'s permanent environmental
fact** *(the operator runs a second UE editor on this box and is not asked to avoid the machine)*.
⇒ **the build is memory-bound, not CPU-bound: 10 physical cores idle behind a 1-process limit.**

**Observed rate: ~15 of 761 actions in ~5 minutes.** ⛔ **No completion estimate is offered as a
finding** — engine PCH actions and leaf compiles differ by more than an order of magnitude and a
linear extrapolation from the PCH-heavy head of the list would be a claim the data does not support.
**What IS established: it will not finish inside this turn.**

⛔ **NOT INTERRUPTED.** The build phase writes only `Intermediate`/`Binaries`; **no container is being
written yet**, so it is safe where it stands — but restarting would discard the work already done for
nothing.

**Consequently NOT YET RUN, and none of them can be:** the `verify_cooked_maps.ps1` map gate · the
A44 scan of the new staged artifact · the token read-back · the new quartet identity · **and the
entire slice-1 validation.**

✅ **What IS ready and committed so the next turn starts clean:** the slice-1 **pre-declared branch
table**, `CaptureBench/tools/p18_slice1_predeclared_branches.md`, commit **`972840d`** — **written
and committed BEFORE the cook finished and before any leg ran**, exactly as the practice requires.

---

## 134. State after PART EIGHTEEN

| | |
|---|---|
| Ruling 1 | ✅ **21/21 banked and verified**; bank **158 → 159** dirs, 183 session ids |
| Ruling 2 | ✅ **all three trees freed** — **0.94 GB → 21.93 GB**, per-tree figures recorded |
| Ruling 3 | ✅ **reserved trees untouched** |
| Ruling 4 | ✅ runbook §8.6 **step 0** + **`G130`** |
| the cook | 🚧 **IN FLIGHT** — full rebuild, memory-bound at 1 process |
| slice 1 | ⛔ **NOT VALIDATED** — blocked on the cook; branches pre-declared and committed |
| `P6` · tag · branch | **not moved · none · `76cac74` untouched** |

**WHAT THIS PART SETTLES: the preservation-vs-cleanup ordering. When banking and freeing contend for
the same disk, the resolution is not to skip either — it is to free ONLY what contains no evidence
first. The literal order in the ruling was impossible; the protection it existed for was kept
intact.**

---

## 135. ADDENDUM — SCOPE NOTE for the queued move to `E:`

⛔ **SCOPE ONLY. NOTHING CHANGED, NOTHING COPIED, NO MIGRATION PLAN PROPOSED.** Every path below was
read this turn and **left exactly as it is.** This is the input to the move brief, not the brief.

### 135.1 TRACKED — in git, visible in a diff, recoverable

| file | line(s) | value |
|---|---|---|
| `CaptureBench/tools/run_leg.ps1` | **82**, **84** | staged exe · `_bench_sessions_bank` |
| `CaptureBench/tools/verify_cooked_maps.ps1` | **37** | default `-Utoc` under `Builds\BenchGate` |
| `CaptureBench/tools/ws_scoping_echo.ps1` | **61**, **68** | `Config\DefaultGame.ini` · staged exe |
| `CaptureBench/tools/verify_lastrundir.ps1` | **38**, **53** | `DefaultGame.ini` · staged exe |
| `CaptureBench/tools/mainworld_recon.ps1` | **50**, **51** | staged exe · staged log |
| `CaptureBench/tools/mainworld_join.ps1` | **37** | `__ExternalActors__` root |
| `CaptureBench/tools/prune_verify.ps1` | **10**, **11** | bank root · exe root |
| `CaptureBench/tools/cure_measurement_table.py` | **40** | `BANK = r"D:\..."` |
| `CaptureBench/tools/guard_collapse_sweep.py` | **36** | `BANK` |
| `CaptureBench/tools/h4_recon.py` | **49** | `BANK` |
| `CaptureBench/tools/mainworld_q4_geometry.py` | **38** | `BANK` |
| `CaptureBench/tools/traceability_sweep.py` | **41** | `BANK` |
| `AnomalyInjector/docs/PRE-DELIVERY-CHECKLIST.md` | **24** | `Config\DefaultGame.ini` |
| `AnomalyInjector/docs/setup-runbook.md` | **7–9**, **30**, **42–43**, §8.1–8.6 | engine · host · clean · build/stage/cook |
| `AnomalyInjector/docs/architecture.md` | **200–201** | engine · host |
| `AnomalyInjector/docs/gotchas.md` | **96**, **201**, **207**, **216**, **1001**, **1020**, **1736**, **1821**, **2041** | engine, host, bank, baselines |
| `AnomalyInjector/docs/sessions/*` | many | historical prose |
| `anomaly-dashboard/host-tools/overlay_watcher.py` | **23**, **39**, **40** | `CAPTURES_ROOT` · `VERIFY_SCRIPT` |

⚠ **`CaptureBench` is TRACKED BUT LOCAL-ONLY — no remote.** Its 17 baked paths are in version control
and **nowhere else**. A drive move plus a disk loss takes the harness with it.

### 135.2 🚨 UNTRACKED — the hazard, and it is `G112`'s shape exactly

> *"a value that lives outside version control returns silently."*

| # | where | what |
|---|---|---|
| **1** | 🚨 **Windows registry** — `HKCU\Software\Epic Games\Unreal Engine\Builds` | **`{B34F356C-4AE7-256A-F0E1-318A632BB902}` → `D:/UESource/UnrealEngine`.** **This is how `StackOBot.uproject` resolves its engine at all** — the `.uproject` holds only the GUID. **It is in no file, in no repo, and in no backup.** The single most invisible path in the system |
| **2** | `D:\IntrusiveAnomalies\host-tools\` | **A SECOND, UNTRACKED COPY** of `encode_watcher.py`, `overlay_watcher.py`, `start_encode_watcher.bat`, `start_overlay_watcher.bat`. `D:\IntrusiveAnomalies` **is not a git repo.** The tracked copies live in `anomaly-dashboard/host-tools/` — **two copies, one tracked, one not** |
| **3** | `D:\IntrusiveAnomalies\_M2Smoke\host-tools\encode_watcher.py` | **a THIRD copy**, also untracked |
| **4** | `_binary_baselines\README.md` | untracked; carries the **hash→meaning provenance** for both quartets |
| **5** | `_bench_sessions_bank\README.md` | untracked; carries the **name-matched-but-evidence-differed** warning |
| **6** | `Builds\BenchGate\...` , `_bench_sessions_bank\`, `_binary_baselines\` | the trees themselves — build artifacts and evidence are deliberately not in git |

### 135.3 WHAT BREAKS IF A PATH IS MISSED — by failure mode

🚨 **Ranked by danger, because a path that SILENTLY FALLS BACK is worse than one that errors.**

| category | failure mode | danger |
|---|---|---|
| 🚨 **Python `BANK = r"D:\..."`** *(5 sweep tools)* | the bank path resolves to nothing ⇒ **the sweep reports an EMPTY CENSUS as a clean result.** `traceability_sweep.py` / `guard_collapse_sweep.py` would answer *"0 legs"* and look like a pass | 🚨 **SILENT WRONG ANSWER — the worst class.** *(`cure_measurement_table.py` is partly protected: PART ELEVEN taught it to report every skipped leg with a reason)* |
| 🚨 **`overlay_watcher.py CAPTURES_ROOT`** | polls a directory that does not exist, **never triggers, never errors** — frames simply never get annotated | 🚨 **SILENT NO-OP.** `G112`'s exact shape |
| ⚠ **`verify_cooked_maps.ps1 -Utoc` default** | a missing `.utoc` and a genuinely map-less container both land near *"found nothing"* | ⚠ **MISLEADING** — the tool's exit-2-on-zero-in-both design blunts it, but the two causes must not be conflated |
| ✅ **`run_leg.ps1` staged exe / bank** | path not found ⇒ **the leg fails loudly and immediately** | ✅ **SAFE — hard error** |
| ✅ **Registry engine mapping** | `.uproject` cannot resolve its engine ⇒ **build/open fails outright** | ✅ **SAFE — hard error**, though the message names a GUID, not a path |
| ⚠ **Docs / runbook / gotchas** | no runtime effect — but **a cold-start session follows them and lands nowhere**, which is a real cost given this project's cold-start contract | ⚠ **STALE PROSE** |
| 🚨 **The two untracked READMEs** | if lost in the move, **the hash→meaning provenance for both preserved quartets goes with them** — and `A44`/`G121` say a result whose build cannot be identified is not a result | 🚨 **UNRECOVERABLE CONTEXT LOSS** |

### 135.4 DOES `D:\UESource\UnrealEngine` HAVE TO MOVE? — **NO. It can stay. Stated plainly.**

**Recommendation: LEAVE THE ENGINE ON `D:`.** Three reasons, in order of weight:

1. **Nothing couples the project's correctness to the engine's drive letter.** `StackOBot.uproject`
   references the engine **by GUID**, resolved through the registry — so the project can live on `E:`
   and the engine on `D:` with **one registry value unchanged**.
2. **Our build files hardcode no engine path.** `AnomalyCapture.Build.cs` uses
   `GetModuleDirectory("Renderer")`, and the branch's variant uses `EngineDirectory` — **both
   engine-relative**. Verified this turn.
3. **It is ~230 GB and a source build.** Moving it is a larger, riskier operation than the project,
   with its own rebuild, and it buys nothing the move is for.

⚠ **ONE CONSEQUENCE THAT MUST BE IN THE MOVE BRIEF EITHER WAY: a FULL REBUILD IS REQUIRED AFTER THE
MOVE.** `Intermediate\**\*.dep.json` stores **absolute** source and PCH paths — measured this turn in
`CaptureBench\Intermediate\...\Module.CaptureBench.cpp.dep.json`, which names both
`d:\intrusiveanomalies\...` and `d:\uesource\...`. **Stale intermediates after a move are not a
correctness risk (the build regenerates them) but they are a time cost, and deleting `Intermediate`
is exactly what forced this part's 761-action rebuild.**

⛔ **NO MIGRATION PLAN IS PROPOSED. This is the scope, and it stops here.**

---

# PART NINETEEN — the cook SUCCEEDS, the build still cannot boot: **`LoadingPhase`. HALT.**

**Build dirs junctioned to `E:`, cook `BUILD SUCCESSFUL`, map gate PASS — and the packaged build
still dies at engine init. Two causes, in sequence, each reporting success at the step that caused
it. The second requires a PLUGIN-DESCRIPTOR change ⇒ HALT, per the brief.** **NO PRODUCTION CODE
CHANGED. NO TAG. `P6` NOT MOVED.**

---

## 136. `E:` junctions — the disk problem is solved, and the harness never noticed

**Owner directed the build to `E:` (609 GB free). Implemented as DIRECTORY JUNCTIONS so every path
stays literally `D:\IntrusiveAnomalies\StackOBot\...`** — `run_leg.ps1:82,84` and the other 15 baked
paths keep working **with no edits**, and the queued `E:` migration stays queued.

| dir | moved | verification |
|---|---|---|
| `Intermediate` | **9,106 files / 13,037,633,600 B** | MOVE VERIFIED byte-count identical; **9,106 files visible through the junction** |
| `Saved` | **5,443 files / 7,667,778,391 B** | MOVE VERIFIED; **5,443 files visible through the junction** |

**free D: 39.86 → 59.17 GB · free E: 609.33 → 590.06 GB.**

✅ **It worked exactly as intended: through the whole rebuild `freeD` stayed flat while `freeE`
absorbed the churn.** ⚠ **One residual, measured and NOT junctioned: `D:\UESource\UnrealEngine\
Engine\Intermediate` (59.78 GB) is engine-side and still on `D:`** — it oscillated D: between 42 and
59 GB during the build. **It always recovered, and it is the remaining `D:` exposure.**

⚠ **First attempt HALTED correctly** on *"the process cannot access the file because it is being used
by another process"* — the previous cook was still live. **Nothing was moved.** *(That also corrected
my earlier misread: at 00:07 I reported the cook had died, because I caught a gap between compile
actions and a stale log tail. **It had not** — it ran to `[761/761]` and failed only at the link.)*

## 137. Cook #1 — SUCCESSFUL, and the artifact is still wrong

**`BUILD SUCCESSFUL`, ExitCode=0, 39m 26s**, with the editor closed and ~10 build processes.

✅ **MAP GATE PASS**, read from the artifact, both encodings scanned:
`CB_GateLevel` · `Entry` · `MainMenu` · `MainWorld` — **all PRESENT**, exit 0.

**New quartet (`G121`):**

| | hash | bytes | mtime |
|---|---|---|---|
| exe | **`23EF6202`** *(was `101AFEA4`)* | 240,608,768 | 01:18:20 |
| `.utoc` | **`638A551E`** *(was `939B9C9B`)* | 268,036 | 01:18:49 |
| `.ucas` | **`4A816025`** *(was `8A602D4D`)* | 284,469,936 | 01:18:49 |
| `.pak` | `7CAE22DD` **UNCHANGED** | 10,115,703 | 01:18:45 |

**A44 on the staged exe: every new symbol present in UTF-16, controls non-zero ⇒ sound scan.**

🚨 **AND THE BUILD STILL DIED AT ENGINE INIT:**
`Missing global shader FAnomalyVisibleMaskPS's permutation 0, Please make sure cooking was successful.`

## 138. Cause 1 — `G47`: the cook runs on EDITOR binaries. **Measured.**

| symbol | stale `UnrealEditor-AnomalyCapture.dll` (18-08, 473,600 B) |
|---|---|
| `AnomalyVisibleMask` | **0** |
| `/Plugin/AnomalyInjector` | **0** |
| `IAI.Capture.Mask` | **0** |
| `IsHideTypeAnomaly` | **1** ⇒ **scan SOUND, not blind** |

**The cook commandlet is `UnrealEditor-Cmd` and it loaded a two-day-old dll, so
`AddShaderSourceDirectoryMapping` never ran and the shader was never compiled — while the cook
reported success.**

✅ **FIXED: `Build.bat StackOBotEditor` — 45 s, 22 actions, dll 473,600 → 590,336 B, all symbols
present.**

⚠ **`G47` has said this since `m8`. RUNBOOK §8.6 DID NOT** — its recipe builds only the game target.
**The knowledge existed; the recipe did not carry it, and that is how I missed it.** ✅ **§8.6 now has
step 3.5** with the editor rebuild *and* the A44 scan of the **editor dll**.

## 139. 🚨 Cause 2 — `LoadingPhase`. **THE HALT.**

Re-cook with fresh editor binaries **crashed at commandlet startup**, and the engine named the fix:

```
Assertion failed: !bInitializedSerializationHistory  [RenderCore/Private/Shader.cpp:246]
Shader type was loaded after engine init, use ELoadingPhase::PostConfigInit on your module
to cause it to load earlier.
```

*(The `EXCEPTION_ACCESS_VIOLATION` in `UClassRegisterAllCompiledInClasses()` below it is downstream
noise. The assertion is the cause.)*

**Global shader types must register BEFORE the shader serialization history is initialised.**
`AnomalyInjector.uplugin` declares **`AnomalyCapture` with `"LoadingPhase": "Default"`** — after
engine init.

⛔ **AND IT IS NOT A ONE-LINE FLIP: `AnomalyCapture` depends on `AnomalyInjector`, also `Default`, and
a module cannot load before its dependency.** All three modules are `Default` today.

⇒ 🛑 **THE COOK REQUIRES A PLUGIN-DESCRIPTOR CHANGE. The brief says: *"NO PRODUCTION CODE CHANGES. If
the cook requires any, HALT AND REPORT."* HALTED.**

### 139.1 The options, costed — ⛔ NONE IMPLEMENTED, the pick is the owner's

| # | option | cost / risk |
|---|---|---|
| **A** | Flip **`AnomalyCapture` + `AnomalyInjector`** to `PostConfigInit` | smallest edit — **but it changes module load order for the WHOLE plugin**, moving the injector and capture subsystems' module init before config is fully up. Broad blast radius for a shader problem |
| **B** ⭐ | **A new tiny module** (e.g. `AnomalyShaders`) at **`PostConfigInit`** declaring **only** the global shader + the shader-directory mapping | **the standard UE pattern.** `AnomalyCapture`/`AnomalyInjector` keep `Default` and their load order is **untouched**. Costs one `.Build.cs`, one module `.cpp`, a `.uplugin` entry, and moving two files |
| **C** | Avoid a global shader entirely | re-opens the `C-1` instrument question that `M-2` already settled on correctness grounds. **Not recommended** |

**Recommendation: B** — it confines the change to the thing that actually needs the early phase.

## 140. Bench restored and verified

⚠ **The staged build was left non-bootable by cook #1** (exe `23EF6202` + a container without the
shader). **Restored from `_binary_baselines\m25-h4h5m1-measurement-build\`:**

**All six files hash-verified against the preserved copy — `101AFEA4` · `939B9C9B` · `8A602D4D` ·
`7CAE22DD` · `C70ECDAA` · `A16A18A8`, ALL MATCH.**

✅ **Verified booting, not assumed:** control server responded over WS
(`viewportScoping False · pollRadius 1800 · minScreenCoverage 6`), behavioural A48 echo
`blinking: matched 1 actor(s) for '=StaticMeshActor_100'` ×4, and **no missing-shader fatal in the
log.** **The preservation precondition earned its keep within two hours of being taken.**

## 141. ⚠ `G115` FIRED ON ME, AND THE PRE-COMMIT DIFFSTAT IS WHAT CAUGHT IT

Adding the PART INDEX row for this part, I used a PowerShell `Get-Content -Raw` → `Set-Content`
round-trip. **The diffstat came back `2940 ++++----` on a ~130-line addition.**

**Diagnosed rather than committed:** BOM `EF BB BF` added · CRLF→LF · and, decisively, **`⚠` had
become `Ã¢Å¡Â ` — double-encoded. Every non-ASCII line in a 2,900-line journal was corrupted.**

⇒ **Reverted with `git checkout --` and re-applied through the editor tool.** ⚠ **Two encoding
"fixes" attempted before diagnosing (strip BOM, convert EOL) — both were wrong because I had not yet
established which of three candidate causes it was. `core.autocrlf=true` and the stored blob is
LF/no-BOM, so neither line endings nor the BOM was ever the real diff.**

**`G115` says exactly this and I did it anyway. The mechanical pre-commit diffstat check is the only
reason it did not land.**

## 142. State after PART NINETEEN

| | |
|---|---|
| disk | ✅ **solved** — `Intermediate` + `Saved` junctioned to `E:`, harness paths untouched |
| cook | ✅ runs to `BUILD SUCCESSFUL`; ✅ map gate PASS; ⛔ **artifact unbootable** |
| blocker | 🛑 **`LoadingPhase` — a plugin-descriptor change. HALT.** |
| slice 1 | ⛔ **still unvalidated**; branches pre-declared at `972840d` |
| bench | ✅ **restored to the preserved quartet, hash-verified, boots** |
| new | **`G131`** · runbook **§8.6 step 3.5** |
| ⛔ next | **the owner's pick of A / B / C** |

**WHAT THIS PART SETTLES: `BUILD SUCCESSFUL` is not evidence a shader reached the container — only
booting the packaged build is. Two separate failures each reported success at the step that caused
them, and the only instrument that caught either was running the artifact.**

---

# PART TWENTY — Option B ships, the build BOOTS, and the measurement is WRONG. **HALT.**

**The shader module works. The cook works. The build boots. And slice 1's measurement is not
trustworthy — for a reason my own `A-3` detector caught, and a second one it did not.**
**NO TAG. `P6` NOT MOVED.** `feature/stencil-capture` **READ-ONLY at `76cac74`.**

---

## 143. `RULING 1` — the `E:` migration is **SUSPENDED, NOT CANCELLED**

**The junction solved the problem the migration existed for**, with **zero path edits** and zero risk
to 25 milestones of tooling. **Recorded as SUSPENDED**: disk pressure is resolved and a full
migration would now be churn against no measured need.

**It reopens if:** `freeD` trends down again despite the junctions, **or** the unjunctioned
engine-side `D:\UESource\UnrealEngine\Engine\Intermediate` (~60 GB, swinging D: between 42 and 59 GB)
becomes a blocker. ⛔ **The path scope note (§135) is NOT needed now; if the migration reopens it is
asked for again.**

✅ **Topology recorded in `setup-runbook.md` §3.6** so nobody finds ~21 GB "missing" from `D:` and
concludes something is broken.

## 144. `G115` AMENDED — diagnose before fixing, and name what caught it

**Added to `G115`:** a large diffstat has **several** possible causes and **the wrong repair is
indistinguishable from the right one until the cause is named.** Recorded that **two repairs were
attempted before diagnosis and both were wrong** (the BOM was real but was not the diff; the LF→CRLF
conversion was backwards — `core.autocrlf=true`, blob is LF/no-BOM). **And that what caught it was
the MECHANICAL PRE-COMMIT DIFFSTAT CHECK, not vigilance** — the rule was written by the same hand and
violated anyway. **That is the argument for mechanical checks over remembered rules.**

## 145. `TASK 1` — the `AnomalyShaders` module

| file | contents |
|---|---|
| `Source/AnomalyShaders/AnomalyShaders.Build.cs` | deps below |
| `Public/AnomalyVisibleMaskShader.h` | `FAnomalyVisibleMaskPS` via **`DECLARE_EXPORTED_SHADER_TYPE(…, Global, ANOMALYSHADERS_API)`** + its `FParameters` |
| `Private/AnomalyVisibleMaskShader.cpp` | **`IMPLEMENT_GLOBAL_SHADER`** — and nothing else |
| `Private/AnomalyShadersModule.cpp` | **`AddShaderSourceDirectoryMapping`** — and nothing else |

**DEPENDENCIES, and why each:** `Core` (always) · `Engine` — **`FSceneTextureShaderParameters` is
`ENGINE_API` in `Engine/Public/SceneTexturesConfig.h`, NOT Renderer** · `RenderCore` (`FGlobalShader`,
the param macros, `AddShaderSourceDirectoryMapping`) · `RHI` · `CoreUObject`, `Projects`
(`IPluginManager`), private.

🚨 **THE POINT OF THE DEPENDENCY LIST: NO `Renderer`, AND NO RENDERER-PRIVATE INCLUDE PATH.** Those
stay in `AnomalyCapture` at `Default`. **A `PostConfigInit` module dragging the render deps early is
exactly how Option A's blast radius comes back in through the side door, and it does not.**

**GAME-AGNOSTIC INVARIANT: PRESERVED.** No host-game type is referenced; the deps are engine modules
already contemplated by the invariant.

✅ **`.uplugin` CONFIRMED — only the new entry moved:**

| module | LoadingPhase |
|---|---|
| **`AnomalyShaders`** | 🆕 **`PostConfigInit`** |
| `AnomalyInjector` | `Default` **unchanged** |
| `AnomalyCapture` | `Default` **unchanged** |
| `AnomalyControlServer` | `Default` **unchanged** |

✅ **A44 on the EDITOR dlls proves the split is clean:**

| dll | `AnomalyVisibleMask` | `/Plugin/AnomalyInjector` | `IAI.Capture.Mask` | `IsHideTypeAnomaly` |
|---|---|---|---|---|
| **`AnomalyShaders`** (58,880 B) | ✅ | ✅ mapping | ⛔ | ⛔ |
| **`AnomalyCapture`** (585,216 B) | ✅ *(uses)* | ⛔ *(moved out)* | ✅ | ✅ |

⚠ **One compile error worth recording:** `ANOMALYSHADERS_API` on **both** the class and
`DECLARE_EXPORTED_SHADER_TYPE` → `C2487: member of dll interface class may not be declared with dll
interface`. **The engine's own `BinkShaders.h` puts it only in the macro.** Fixed.

## 146. `TASK 2` — the cook, the gates, and the new quartet

✅ **COOK `BUILD SUCCESSFUL`, ExitCode=0, ~90 s** (incremental — 4 build actions).
✅ **MAP GATE PASS** — `CB_GateLevel` · `Entry` · `MainMenu` · `MainWorld`, read from the artifact.
✅ 🚨 **SHADER PRESENCE GATE — PASS.** Booted with `IAI.Capture.Mask 1`: **no `Missing global shader`,
`Game Engine Initialized`, `Bringing World … up for play`.** ⚠ **The gate is the BOOT, and §3.7 now
records WHY: string-scanning the container returns 0 for our shader AND 0 for known engine shaders,
so it is suspect tooling, not evidence.**
✅ **TOKEN READ-BACK PASS** — source `5b544cee3d97…` (64) **== enforced** `5b544cee3d97…` (64).

**NEW QUARTET — preserved at `_binary_baselines\m26-slice1-measurement-build\`, 6/6 verified at the
destination:**

| | hash | bytes | mtime |
|---|---|---|---|
| exe | **`998B1399`** | 240,610,304 | 01:40:12 |
| `.utoc` | **`9334496D`** | 268,174 | 01:40:42 |
| `.ucas` | **`62EB0072`** | 284,474,032 | 01:40:42 |
| `.pak` | **`78C977A5`** | 10,115,707 | 01:40:38 |

## 147. `SLICE 1` — pre-declared branches, restated **VERBATIM**

**Committed as `CaptureBench/tools/p18_slice1_predeclared_branches.md` at `972840d`, before any leg:**

> | # | branch | reading that selects it |
> |---|---|---|
> | **S1** | MEASUREMENT CORRECT ON ALL FOUR | non-zero on both controls, zero on both `H5` targets |
> | **S2** | CORRECT ON CONTROLS, NOT ON `H5` TARGETS | non-zero on controls, **non-zero** on foliage/splinespawn too |
> | **S3** | CORRECT ON `H5` TARGETS, NOT ON CONTROLS | zero on the `H5` pair **and** zero on a control ⇒ 🚨 broken in the dangerous direction. HALT |
> | **S4** | 🚨 `NOT_MEASURED` WHERE A QUALIFYING FRAME SHOULD EXIST | 🚨 **the `LOCK-1` rule is failing. HALT** |
> | **S5** | MIXED / PARTIAL | report exactly what was seen, attribute nothing, HALT |
> | **S6** | INVALID / NOT MEASURED (leg) | leg failed for a HOW-IT-RAN reason ⇒ discard, bank, re-run |

### 147.1 THE RESULT — **`S4`, with an `S3`-shaped observation inside it. HALT.**

**Two legs run, both A63-VALID on attempt 1, both banked.**

| leg | map | target | events | `NOT_MEASURED` |
|---|---|---|---|---|
| `P20_M26S1_CTRL49` | `CB_GateLevel` | `StaticMeshActor_49` — **B1 PASSED** (`modal_rot (0,0,0)`, 59 rows, 1 distinct, 100 % modal) | 8 | **8 of 8** |
| `P20_M26S1_RAMP` | `MainWorld` | `SM_Ramp2` — **B1 NOT APPLICABLE, declared** (G117) | 8 | **7 of 8** |

✅ **WHAT WORKS, and it is not nothing:** the `LOCK-1` arm rule behaves exactly as designed —
**`skippedHidden=3..4` on every event** shows it refusing to arm on hidden ticks; and
**`arms=4 resolved=4` on 15 of 16 events** shows tagging, the mask pass and the readback all
round-tripping.

🚨 **WHAT IS WRONG — TWO FAULTS:**

**(1) TAG 255 POLLUTES THE MASK.** `A-3`'s **unassigned-reserved-tag detector fired 25 times**, and
**always on 255 — never 254, never any other value, in BOTH levels.**

⚠ **AND THE DETECTOR'S MESSAGE NAMES A CAUSE IT HAS NOT ESTABLISHED** — it says *"a host title is
writing into the reserved custom-stencil range"*. **StackOBot is a sample project; a single constant
value in two unrelated levels is not a host-game signature.** 🚨 **This is S4-1's ruling recurring in
code I wrote this turn: a gate that fails safe still misleads if its LABEL names a cause it has not
established.** The wording must become *"an unassigned reserved tag was present — cause not
established"*.

⚠ **HYPOTHESIS, NOT ESTABLISHED: `ReservedStencilMax = 255` puts our range on top of the value an
unpopulated / default custom-stencil read returns.** 255 is the classic unbound-read constant.
**Testable cheaply by narrowing the reserved range below 255 — but that is production code.**

**(2) 🚨 THE ONE EVENT THAT DID MEASURE, MEASURED ZERO — ON A CONTROL.** `SM_Ramp2`
`startFrame=4`, `collisions=0`, `arms=4 resolved=4` ⇒ **`MEASURED_ZERO`, `maxCount=0`.**
**`SM_Ramp2` is a control that must be NON-ZERO.** ⇒ **the `S3` shape: zero on a target that draws,
which is the dangerous direction.**

**Both faults have one coherent candidate explanation: the shader is not reading the stencil values
we wrote — every read returns 255, so our tag never appears (count 0) and 255 appears instead.**
⛔ **NOT ESTABLISHED. Stated as the leading candidate, not a finding.**

### 147.2 ⛔ WHY I DID **NOT** RUN THE TWO `H5` LEGS

**Because a ZERO on `InstancedFoliageActor_0_0_0` and `BP_SplineSpawn_C` is exactly what the bug
produces.** With the measurement returning zero on a control, **an `H5` target reading zero would be
indistinguishable from the defect — and would look like the cure working.**

🚨 **That is the false-positive this project has named repeatedly (`G96`: a blind metric survives
review when its output looks like the answer you expected). Running those legs now would manufacture
evidence for a conclusion the instrument cannot support.**

### 147.3 The halt

**Fixing either fault is production code beyond Task 1's module.** The brief: *"NO FURTHER PRODUCTION
CODE beyond Task 1's module. If anything else is required, HALT AND REPORT."* ⇒ **HALTED.**

## 148. State after PART TWENTY

| | |
|---|---|
| Option B | ✅ **shipped and works** — module split clean, no `Renderer` dep, load order untouched |
| cook / gates | ✅ cook · ✅ map gate · ✅ **shader presence gate** · ✅ token read-back |
| new quartet | ✅ **preserved 6/6** at `m26-slice1-measurement-build` |
| slice 1 | 🛑 **`S4` + an `S3` observation. The measurement is NOT trustworthy.** |
| `H5` legs | ⛔ **deliberately NOT run** — their zero would be indistinguishable from the bug |
| `P6` · tag | **not moved · none** |
| ⛔ next | **the owner's call on the two faults** |

**WHAT THIS PART SETTLES: the plumbing is proven end to end — tag, mask, readback, and the `LOCK-1`
timing rule all work — and the VALUE coming back is wrong. Slice 1's whole purpose was to establish
that before the cure was trusted to act, and it did exactly that.**

---

# PART TWENTY-ONE — the cause is ESTABLISHED, and it is **NOT** the tag range

🚨 **THE LEADING CANDIDATE FROM PART TWENTY IS REFUTED, AND "FIXING" IT WOULD HAVE BEEN WORSE THAN
LEAVING IT BROKEN.** ⛔ **DIAGNOSIS ONLY — NO FIX. NO TAG. `P6` NOT MOVED.**
`feature/stencil-capture` **READ-ONLY at `76cac74`.**

**Branches pre-declared as a file BEFORE any measurement: `CaptureBench/tools/
p21_diagnosis_predeclared.md`, commit `6428282`, including the refuters `R1`–`R5`.**

---

## 149. `D-3` FIRST, because it answers everything else — **255 IS THE ENGINE'S FALLBACK**

**ESTABLISHED FROM ENGINE SOURCE, not inferred:**

```cpp
// Renderer/Private/SystemTextures.cpp:247-256  — "Create a dummy stencil SRV."
FRHITextureCreateDesc::Create2D(TEXT("StencilDummy"), 1, 1, PF_R8G8B8A8_UINT)
FTextureRHIRef Texture = RHICreateTexture(Desc);
SetDummyTextureData<FColor>(Texture, FColor::White);      // <<< WHITE = 255
```

**And the binding that selects it (`SceneTextures.cpp:959`):**

```cpp
SceneTextureParameters.CustomStencilTexture =
    bCustomDepthProduced ? CustomDepthTextures.Stencil : SystemTextures.StencilDummySRV;
```

⇒ **When custom depth is NOT produced for the frame, `CustomStencilTexture` is a 1×1 texture filled
with 255, and `CalcSceneCustomStencil` returns 255 AT EVERY PIXEL.**

**That is the observation exactly: always 255, never 254, uniform, in both levels, 25 times.**
⇒ **`D-3` = candidate (a), the unpopulated-buffer fallback. Candidates (b) format/swizzle and (c)
something-writes-255 are both EXCLUDED** — a 1×1 white dummy explains the constant, the uniformity
and the level-independence together.

## 150. `D-1` — the shader IS reading, but it is reading the WRONG TEXTURE

`CalcSceneCustomStencil` (`SceneTexturesCommon.ush:134`) does
`SceneTexturesStruct.CustomStencilTexture.Load(...) STENCIL_COMPONENT_SWIZZLE`. **The binding is
live and the read succeeds** — it simply resolves to the dummy. ⇒ **the pass point is not wrong and
the read is not broken; the buffer it names was never produced.**

## 151. 🚨 `R1` FIRES — THE `ReservedStencilMax` CANDIDATE IS **REFUTED**, AND THE REPAIR WOULD HAVE BEEN ACTIVELY HARMFUL

**Pre-declared refuter `R1`/`R3`: if the read path is live and the value is a bound fallback, the
range is not the cause.** It is.

🚨 **AND THE TRAP THE OWNER NAMED IS REAL AND WORSE THAN "CHANGES NOTHING": moving the reserved range
to, say, 100–155 would leave every read still returning 255 — but 255 would then be OUTSIDE our
range, so `CalcSceneCustomStencil` would fail the `Stencil < ReservedBase` test, the shader would
write 0, and THE UNASSIGNED-TAG DETECTOR WOULD GO SILENT.**

⇒ **Every event would come back `MEASURED_ZERO` with `collisions=0` — a confident, clean-looking
answer that is entirely wrong, and under slice 3 IT WOULD VETO EVERY EVENT.** **The "fix" would have
converted a loud fault into a silent one.** *(`G118`'s rule: a guard that passes the unsafe case is
worse than no guard.)*

## 152. `D-4` — why custom depth was not produced. **LEADING CANDIDATE, source-grounded, NOT measured**

Two requirements, and the second is the suspect:

| requirement | state |
|---|---|
| **`r.CustomDepth` must be 3** (`EnabledWithStencil`) | ⚠ engine **default is 1** — *enabled, but stencil writes OFF*. We set 3 in `FAnomalyMaskMeasure::BeginRun()` at `StartRun`, **several frames before the first arm**, and the cvar is `ECVF_RenderThreadSafe`. **Plausibly fine, NOT read back — see the limit below.** |
| **a primitive must be marked `bRenderCustomDepth` for that view** | 🚨 **`SetRenderCustomDepth` → `MarkRenderStateDirty()` — a DEFERRED render-state recreate.** The proxy flag is **not live for the frame in which it is set.** |

🚨 **AND WE TAG AND ARM IN THE SAME TICK.** `ArmIfMeasurable` calls `TagActor(...)` and then
`Sve->ArmMask(RequestId)` **on the same tick**, so the mask pass for frame N runs against a proxy
that has not yet received the flag ⇒ **custom depth not produced ⇒ dummy bound ⇒ 255.**

⇒ **THE FAULT IS A ONE-FRAME ORDERING BUG, NOT A RANGE BUG.** The tag needs at least one frame to
reach the render thread before the mask can see it.

⛔ **STATED AS THE LEADING CANDIDATE, NOT ESTABLISHED.** What would settle it is arming N frames
after tagging and watching 255 disappear — **that is the repair, and it is not this turn's work.**
⚠ **Honest limit: `r.CustomDepth`'s EFFECTIVE value at pass time was NOT read back.** `-ExecCmds` is
startup-only and would report the default, not the in-run value; a real read-back needs
instrumentation. **So "the cvar is fine" is assumed, not measured, and that assumption is recorded
rather than buried.**

## 153. `D-2` — the write side is **EXONERATED**, and the odd event now makes sense

**Pre-declared `R5`: an event with `collisions=0` that still returns count 0 exonerates the write
side.** That event exists — `P20_M26S1_RAMP`, `startFrame=4`: **`collisions=0`, `arms=4 resolved=4`,
`MEASURED_ZERO`.** `collisions=0` means `VerifyActorStillTagged` found `bRenderCustomDepth` **true**
and `CustomDepthStencilValue` **== our tag** on every renderable component. ⇒ **the property was
written and held; the mask still never saw it.** ⇒ **READ-SIDE FAULT. The tagging code is not at
fault.**

✅ **And it explains the one event that did NOT see 255:** by then earlier tags had propagated, so
custom depth **was** produced and the real stencil was bound — no dummy, no 255 — while *that
event's own* target had only just been tagged, so its tag was absent ⇒ **count 0 with no collision.**
**Same single cause, two different-looking symptoms.**

## 154. `D-5` — the 16th event: **benign, fully explained**

The outlier is the **last** event of each leg (`startFrame=116`, `arms=1 resolved=1`). Events are
spaced **16 ticks** apart (one burst = `K2 + P8 + K2 + Post4`), and the run ends at the **90-frame
cap**, entering `DrainTail` before that event could issue its remaining arms. ⇒ **truncation by the
frame cap — the pre-declared benign branch. NOT a second fault.**

## 155. `TASK 2` — the detector no longer names a cause it has not established

**Both detector messages rewritten to state the OBSERVATION and hand the reader the
DISCRIMINATOR:**

- **was:** *"A host title is writing into the reserved custom-stencil range"* — an unestablished cause
  that **would have sent a future reader hunting a host game in a sample project** (`G120`'s false
  foreclosure in the making).
- **now:** *"OBSERVED — the mask carried reserved-range tag N, which this run never assigned. CAUSE
  NOT ESTABLISHED … DISCRIMINATORS: 255 uniformly across the frame is the engine's StencilDummy
  fallback, bound when custom depth was NOT produced — i.e. our stencil was never read; a
  geometry-shaped region, or any value other than 255, is something genuinely writing into the
  reserved range."*
- The tag read-back warning likewise now separates **write-side** from **read-side** and says which
  observation distinguishes them.

⛔ **Message only — no logic changed.** Compiles clean.

## 156. State after PART TWENTY-ONE

| | |
|---|---|
| `D-3` | ✅ **ESTABLISHED** — 255 is `StencilDummy` = `FColor::White`, bound when custom depth is not produced |
| `D-1` | ✅ **ESTABLISHED** — the read is live; it resolves to the dummy |
| `D-2` | ✅ **write side EXONERATED** — read-side fault |
| `D-4` | ⚠ **LEADING CANDIDATE: tag-and-arm-in-the-same-tick.** `SetRenderCustomDepth` is a DEFERRED render-state recreate. **`r.CustomDepth` effective value NOT read back — recorded as assumed** |
| `D-5` | ✅ **benign** — frame-cap truncation |
| range candidate | 🚨 **REFUTED — and the repair would have SILENCED the detector and vetoed everything** |
| Task 2 | ✅ detector reports observation + discriminator, not cause |
| still standing | **`LOCK-1` PROVEN · plumbing round-trips · module · gates · quartet** |
| ⛔ next | **the owner's ruling on the fix** |

**WHAT THIS PART SETTLES: the fault is a ONE-FRAME ORDERING BUG on the read side, not the tag range —
and the pre-declared refuters are what caught it. Repairing the leading candidate would have produced
`MEASURED_ZERO` with `collisions=0` on every event: a clean-looking answer that vetoes the entire
dataset.**

⚠ **SUPERSEDED BY PART TWENTY-TWO: `F-1` REFUTES the one-frame-ordering candidate from source. The
`255 = StencilDummy` finding (§149-§151) STANDS; the `D-4` explanation for *why* custom depth was not
produced does NOT.**

---

# PART TWENTY-TWO — `F-1` REFUTES THE FIX DIRECTION. **The design cannot be written yet.**

🚨 **I WAS ASKED TO DESIGN THE TIMING FIX. `F-1`'s FIRST QUESTION — "establish from source WHEN the
proxy actually has the flag" — ANSWERS IT, AND THE ANSWER REMOVES THE FIX'S REASON TO EXIST.**
⛔ **DESIGN ONLY, NO CODE. NO TAG. `P6` NOT MOVED.** `feature/stencil-capture` **READ-ONLY.**

---

## 157. Rulings recorded

**RULING 1 — the fix is approved IN PRINCIPLE, design first.** ⇒ **§158 reports why the design cannot
be written on the approved premise.**
**RULING 2 — the `H5` legs stay BLOCKED** until **both** controls read NON-ZERO. **Unchanged, and it
is not affected by anything below.**

## 158. 🚨 `F-1` — THE PROXY IS **ALREADY UP TO DATE**. ONE TICK IS NOT NEEDED; ZERO ARE.

**Chain established from source, each link read, not inferred:**

| # | fact | source |
|---|---|---|
| 1 | `SetRenderCustomDepth` → `MarkRenderStateDirty()` → **`MarkForNeededEndOfFrameRecreate()`** | `ActorComponent.cpp` |
| 2 | 🚨 **the end-of-frame recreate is flushed INSIDE the render entry point, in the SAME frame** | `SceneRendering.cpp:4528`, in **`FRendererModule::BeginRenderingViewFamilies`** |
| 3 | and the engine's own comment says why | *"Guarantee that all render proxies are up to date before kicking off a `BeginRenderViewFamily`."* |

⇒ **BY THE TIME OUR MASK PASS RUNS, THE PROXY HAS THE FLAG. The tag-and-arm-in-the-same-tick
ordering is NOT the fault.** ⛔ **Separating tag from arm would change nothing — it is the
`ReservedStencilMax` repair again, in a new place: a change that cannot fix the symptom it targets.**

⇒ **`F-1`'s answer is not "one tick" or "usually one". It is ZERO — the guarantee already exists,
and it is a guarantee, not a typical case.**

### 158.1 Two further candidates that source ALSO exonerates

**(a) THE PASS POINT IS FINE.** Post-processing does not get a custom-depth-less buffer by design —
when the custom depth pass runs, the uniform buffer is **rebuilt to include it**:

```cpp
// DeferredShadingRenderer.cpp:2981 and :3306
if (RenderCustomDepthPass(GraphBuilder, SceneTextures.CustomDepth, ...))
{
    SceneTextures.SetupMode |= ESceneTextureSetupMode::CustomDepth;
    SceneTextures.UniformBuffer = CreateSceneTextureUniformBuffer(..., SceneTextures.SetupMode);
}
```

⇒ **post-Tonemap is a legitimate place to read custom stencil — PROVIDED the pass ran.**

**(b) THE CVAR PRIORITY IS FINE.** `ECVF_SetByCode = 0x09000000` is **second-highest** (only
`SetByConsole` outranks it), and **no `Config\*.ini` in this project sets `r.CustomDepth` at all** —
so nothing outranks our `Set(3, ECVF_SetByCode)`. ⛔ **This is about PRIORITY only. It says the write
would not be rejected; it does NOT say the value was 3 at pass time.**

## 159. Where that leaves the diagnosis

**Every link in the chain, read from source, says it should work:**

`r.CustomDepth` set to 3 at `StartRun` (high priority, unopposed) → tag written and verified on the
component (`D-2`, `collisions=0`) → proxy recreate flushed before `BeginRenderViewFamily` (`F-1`) →
custom depth pass runs → uniform buffer rebuilt with `CustomDepth` (§158.1a) → our post-Tonemap read
gets the real stencil.

**And the measurement says it does not.** ⇒ 🚨 **SOURCE-ONLY DIAGNOSIS IS EXHAUSTED. The next step is
MEASUREMENT, not design.**

⚠ **AND THE THING TO MEASURE IS THE ONE I ALREADY LABELLED ASSUMED-NOT-MEASURED, exactly as the owner
warned it must not quietly become settled:**

> **`r.CustomDepth`'s EFFECTIVE value at pass time.** `-ExecCmds` is startup-only and reports the
> default (**1**, *enabled but stencil writes OFF*), not the in-run value. **`GetCustomDepthMode()`
> maps 1→`Enabled` and only 3→`EnabledWithStencil`, and `FCustomDepthTextures::Create` returns an
> INVALID texture set when the mode is not enabled — after which `RenderCustomDepthPass` returns
> false and no custom depth is produced.** **A value of 1 at pass time produces EXACTLY the observed
> symptom.**

⛔ **NOT a claim that the cvar is the cause — a claim that it is the ONLY link still unmeasured, and
that it has a mechanism reaching the observed symptom.**

## 160. `F-2`..`F-6` — what survives, and what is contingent

### `F-2` LOCK-1 vs a tag/arm split — ⚠ **CONTINGENT, but the RULE is recorded now**

⛔ **Not answerable as a design while the fix that creates the window is refuted.** **But the hazard
the owner named is real for ANY future change that separates tag from arm, so the rule is fixed
here rather than re-derived under time pressure:**

> **IF tag and arm are ever separated, the hidden-state test must be applied at EVERY point in the
> window — tag time, arm time, and the frame the mask resolves — and a target hidden at ANY of them
> yields `NOT_MEASURED`, never `MEASURED_ZERO`.**

🚨 **The window is exactly `blinking`'s toggle period (half-period 3 frames), so a not-hidden→hidden
transition inside it is not a rare race — it is the common case.** **The admit bias is the safety
argument and it is not negotiable.**

### `F-3` budget — ⚠ **CONTINGENT.** Numbers depend on the separation `N`, which is now not needed

**Recorded for whoever needs it:** events are **16 ticks** apart; arms are capped at **4**;
`missing_object` has only the **6-tick post-revert window** (`SettleAfterRevert` 2 + `PostGap` 4).
🚨 **A separation of `N` ticks divides that window: at `N=2` `missing_object` retains ~2 arms, and any
larger `N` risks ZERO — which would make it permanently `NOT_MEASURED`, a cure that never fires while
looking like a clean pass.**

### `F-4` UNTAGGING — ✅ **ANSWERABLE NOW, and it is cause-independent**

Today tags are applied in `ArmIfMeasurable` and released **only** at `EndRun` via
`AnomalyStencilTag::RestoreAll()`, so **a tag persists for the rest of the run.**

| question | answer |
|---|---|
| when is a tag cleared? | **only at `FinishRun`** — never per event |
| can a retagged target collide with its own stale state? | **No today** — tag values are allocated per event and never reused within a run (`NextTagOffset` increments; the range is 56 wide against ≤8 events). ⚠ **A longer run could wrap and reuse a tag while the old one is still applied.** |
| does the release defer the same way? | ✅ **Yes — `SetRenderCustomDepth(false)` takes the same `MarkRenderStateDirty` path**, so a release is also flushed before the next `BeginRenderViewFamily`. **Symmetric, and per `F-1` that is a guarantee.** |

⚠ **Persistent tags are why the one clean-measuring event saw no 255** (§153): earlier tags had
already made custom depth render. **That is a real coupling between events and it should not be
relied on.**

### `F-5` WHAT A RESIDUAL 255 WOULD MEAN — ✅ **ANSWERABLE NOW**

| after a working fix | meaning |
|---|---|
| **no 255 at all** | custom depth is produced; the dummy is never bound |
| **255 uniform across the frame, on some frames** | ⇒ **custom depth was not produced for THOSE frames** — a per-frame gating problem, not a per-target one |
| **255 in a geometry-shaped region** | ⇒ 🚨 **genuinely written by something** — the only reading under which the original "host title" wording would have been right |
| **any reserved value other than 255** | ⇒ something writing into the range; the dummy cannot produce it |

✅ **The detector's discriminator (as rewritten in PART TWENTY-ONE) separates all four, and needs no
further change.**

### `F-6` THE GATE — ✅ **ANSWERABLE NOW, and it is the most important item here**

🚨 **`R1`'s standing warning applies to the fix itself: a silent instrument and a working one look
identical from outside.** The gate must therefore be **positive evidence**, not absence of noise:

| # | required observation |
|---|---|
| **1** | **`StaticMeshActor_49` (CB_GateLevel) NON-ZERO** — `MEASURED_NONZERO`, `collisions=0` |
| **2** | **`SM_Ramp2` (MainWorld) NON-ZERO** — `collisions=0`; **`A-4`: peak-IN/peak-OUT reported beside it** (peak-OUT **0.2955** > peak-IN **0.1785**) |
| **3** | **arm counts match the `F-3` prediction** — not merely non-zero, the predicted number |
| **4** | **`pctOfFrame` is PLAUSIBLE for the target's on-screen size** — a count that is non-zero but absurd is still wrong |
| **5** | 🚨 **THE 255 DETECTOR IS PROVEN STILL LIVE (`G96`, both ways)** — demonstrate it can still fire, so its silence means ABSENCE and not BLINDNESS. **Without this, items 1-4 could all pass on an instrument that has simply stopped looking.** |

⛔ **Only after 1–5 do the `H5` legs unblock (Ruling 2).**

## 161. State after PART TWENTY-TWO

| | |
|---|---|
| `F-1` | 🚨 **ANSWERED — and it REFUTES the approved fix direction.** The proxy is already up to date; **zero** ticks are needed |
| also exonerated | the **post-Tonemap pass point** · the **cvar priority** |
| diagnosis | ⛔ **source-only is EXHAUSTED** — every link reads as correct and the measurement disagrees |
| next | **MEASURE `r.CustomDepth`'s effective value at pass time** — the only unmeasured link, and it has a mechanism reaching the exact symptom |
| `F-2`/`F-3` | ⚠ **CONTINGENT** — rules and numbers recorded, design deferred |
| `F-4`/`F-5`/`F-6` | ✅ **answered, cause-independent** |
| unchanged | **`LOCK-1` PROVEN · plumbing · module · gates · quartet · write side exonerated · range stays 200/255** |

**WHAT THIS PART SETTLES: the approved fix would not have worked, and the reason is the same shape as
the repair it replaced — a change targeting a symptom whose mechanism source refutes. `F-1` was the
right first question, and asking it before writing the design is the only reason that was caught.**

---

# PART TWENTY-THREE — **THE MASK WORKS.** It has been working on half the frames all along.

🚨 **BRANCH: "THEY DISAGREE" — mode 3 at pass time, custom depth NOT produced on half the armed
frames. The pre-declared "more interesting one".** **AND A SECOND FINDING THAT IS ENTIRELY MINE: the
mask has been returning a CORRECT, STABLE measurement on every frame where custom depth was produced,
and my own collision handling threw it away.**
⛔ **MEASUREMENT ONLY. NO FIX APPLIED. NO TAG. `P6` NOT MOVED.**

**Branches pre-declared at `CaptureBench 2537a2d`, before any measurement.**

---

## 162. Recorded first: the withdrawal, the banked rule, the adopted gate

- **THE TAG/ARM SEPARATION IS WITHDRAWN, NOT DEFERRED** — `F-1` refutes it at the source (§158).
- **`F-2`'s RULE IS BANKED** though contingent and now probably moot: *if tag and arm are ever
  separated, the hidden-state test applies at **TAG**, **ARM** and **RESOLVE** time, and hidden at
  **ANY** of them yields `NOT_MEASURED`.* **Derived under no pressure, which is when rules are worth
  writing.**
- **`F-6` IS ADOPTED IN FULL as the fix gate**, including item 5 — **the 255 detector proven still
  live, both ways (`G96`)** — *"without it, items 1-4 can all pass on an instrument that has stopped
  looking."*
- **`F-4`'s coupling travels:** tags persist to `FinishRun`, so a later event measures under
  conditions an earlier one created. ⇒ **the instrument reports PER ARMED FRAME and nothing is
  averaged.** §164 shows why that mattered.

## 163. `M-1` and `M-3` — **THE CVAR IS EXONERATED**

| measurement | result |
|---|---|
| **`M-3`** game thread, over time | `beginRun rCustomDepth before=1 after=3` · `finishRun before restore=3` ⇒ **set correctly and never reset during the run** |
| **`M-1`** render thread, at the pass point, per armed frame | **`rCustomDepth_renderThread=3` on ALL 30 armed frames** |

⇒ ✅ **The last unmeasured link is measured, and it is clean.** The engine default is 1 (*enabled
WITHOUT stencil writes*) and our `Set(3, ECVF_SetByCode)` reaches the renderer. **It is not the
cause.**

## 164. 🚨 `M-2` — CUSTOM DEPTH IS PRODUCED ON **EXACTLY HALF** THE ARMED FRAMES, IN A REGULAR PATTERN

**Direct discriminator, decided in advance: `StencilDummy` is `1×1`; the real texture is view-sized.**

**30 armed frames: 15 dummy, 15 real.** And the pattern is **not random** — it is identical in every
burst:

| arm within event | 1st | 2nd | 3rd | 4th |
|---|---|---|---|---|
| `customStencilExtent` | **`1x1` DUMMY** | **`1280x720` REAL** | **`1280x720` REAL** | **`1x1` DUMMY** |

*(ids 6,10,11,12 · 22,26,27,28 · 38,42,43,44 · 54,58,59,60 · 70,74,75,76 · 86,90,91,92 ·
102,106,107,108 · 118,122 — **seven full events, all identical**.)*

⛔ **THE MECHANISM FOR THE PATTERN IS NOT ESTABLISHED AND IS NOT GUESSED HERE.** What is established:
it correlates with **position within the burst**, not with anything stochastic, and **the cvar is 3
throughout**, so it is not a mode problem. *(`blinking`'s half-period is 3 frames and the deferred
hidden sample is one tick stale (`m20`) — an obvious place to look, and NOT a claim.)*

## 165. 🚨 **THE MASK WORKS.** The numbers on the real frames are correct and stable.

**On every one of the 14 reduced real frames:**

| | |
|---|---|
| `totalMasked` | **66,635 – 66,862 px** |
| `pctOfFrame` | **7.23 – 7.25 %** |
| spread across 14 frames | **< 0.03 %** |

✅ **AND IT MATCHES THE BANKED GROUND TRUTH.** `StaticMeshActor_49`'s banked figures are **rect 7.8 %
/ `cov_pct` 7.80**. **The mask reads 7.25 % — the right magnitude and slightly UNDER the bounding
rect, which is exactly what an occlusion-correct silhouette should be against a rectangle.**

🚨 **THIS IS THE POSITIVE EVIDENCE `F-6` ITEM 1 ASKS FOR, ON THE FRAMES THAT WERE MEASURED. The
instrument is fundamentally sound. It was never broken — it was being discarded.**

## 166. 🚨 THE SECOND FINDING, AND IT IS MINE: **ONE BAD FRAME DISCARDS A WHOLE EVENT**

**Every event still reports `NOT_MEASURED`, `maxCount=0`, `collisions=2`** — while two of its four
frames carried a correct ~66,800-pixel measurement.

**Cause, in code I wrote:** `FAnomalyMaskMeasure::CollectResults` does

```
if (R.CollisionHits > 0) { ArmedRequestToRecord.Remove(RequestId); continue; }
```

⇒ **the collision flag is EVENT-SCOPED where the observation is FRAME-SCOPED.** A single dummy frame
sets `CollisionHits`, and from then on **every subsequent frame of that event is skipped before its
count is ever read** — including the good ones. **The `MAX`-across-frames design (§113.4) was intended
to take the best frame; the collision check throws the event away before `MAX` ever runs.**

⚠ **THE TWO FAULTS ARE INDEPENDENT.** Even with the alternation unexplained, **a frame-scoped discard
would have produced `MEASURED_NONZERO` at ~7.25 % on this control.** ⛔ **NOT FIXED THIS TURN — no
same-turn fix to a validity instrument.**

## 167. Where the pre-declared branches land

| branch | selected? |
|---|---|
| mode 3 **and** produced ⇒ cvar exonerated, fault elsewhere | ⛔ no |
| mode **not** 3, or **not** produced ⇒ mechanism established | ⛔ not as stated |
| 🚨 **THEY DISAGREE — mode 3 but not produced** | ✅ **SELECTED, and it is the more interesting one, as pre-declared** |
| `B4` instrument did not report | ⛔ no — 30 armed frames, all reported |

**Plus one outcome no branch anticipated: the instrument's numbers were RIGHT on the frames it
measured, and a bug in the result-collection discarded them.** ⚠ **Recorded as unanticipated rather
than folded into a branch.**

## 168. State after PART TWENTY-THREE

| | |
|---|---|
| `r.CustomDepth` | ✅ **EXONERATED** — 3 at set, 3 at every pass, 3 at finish |
| custom depth produced | ⚠ **on exactly half the armed frames, in a fixed per-burst pattern.** Mechanism **NOT established** |
| the mask itself | ✅ **CORRECT — 7.23–7.25 % against a 7.80 % banked rect, spread < 0.03 % over 14 frames** |
| why slice 1 reported nothing | 🚨 **event-scoped collision discard throwing away good frames — my bug** |
| build | exe **`722266A7`** (hot-swap; container unchanged from the `m26` cook, boot re-verified) |
| leg | `P23_M23_CVAR_CTRL49`, A63 attempt 2 (attempt 1 banked as a pose discard), **B1 PASSED** |
| unchanged | LOCK-1 · plumbing · module · gates · quartet · range 200/255 · **H5 legs BLOCKED** |
| ⛔ next | **the owner's ruling — two independent faults, neither fixed** |

**WHAT THIS PART SETTLES: the mask measures correctly. Two separate defects were hiding that — an
unexplained per-burst alternation in whether custom depth is produced, and an event-scoped discard in
my own collection code that threw away the frames which did measure. The cvar, the last thing anyone
could reason about from source, is clean.**

---

# PART TWENTY-FOUR — fault (i) FIXED, fault (ii) MECHANISM ESTABLISHED. The mask now measures.

**m26 slice 1, fresh session 2026-08-20. The owner's brief: fix fault (i), diagnose fault (ii)
WITHOUT fixing it, nothing else.** ⛔ **NO TAG. `P6` NOT MOVED** (measured below).
`feature/stencil-capture` **READ-ONLY at `76cac74`, never checked out.**

**Branches pre-declared BEFORE any measurement: `CaptureBench/tools/p24_fault2_predeclared.md`,
commit `cb299aa` — including what would REFUTE the blinking-cadence lead.**

---

## 169. `TASK 1` — the discard is now FRAME-SCOPED. Commit `795f2a4`.

**The fix, in one sentence: a polluted READ discards THAT FRAME; clean frames still feed the
MAX-across-frames reduction; an event with no clean frame stays `NOT_MEASURED`.**

| what changed | where |
|---|---|
| per-frame pollution replaces `if (R.CollisionHits > 0) continue` | `CollectResults` — `bFramePolluted` = this frame's mask carried an unassigned reserved tag, OR this request was in flight when a verify collision fired |
| verify collisions poison IN-FLIGHT requests, not the event | `VerifyPendingTags` marks the armed-unresolved requests of that record into `PollutedRequests` |
| new per-event counters | `FramesDiscarded` / `FramesContributed`, in the `M26S1 EVENT` summary line |

🚨 **THE ADMIT BIAS IS UNCHANGED, AND THE CODE PATH THAT PRODUCES `NOT_MEASURED` IS:**
`FAnomalyMaskRecord::State` initialises to `EAnomalyMaskState::NotMeasured`
(`AnomalyMaskMeasure.h`), and `CollectResults` writes `State` **only on the clean path, after a
resolved, unpolluted read** — polluted frames `continue` out at `bFramePolluted`, unresolved
frames never enter. **So an event whose every frame is discarded or unresolved is never written
to and stays `NOT_MEASURED`; `MEASURED_ZERO` is reachable ONLY from a clean resolved read of
zero.** The FinishRun warning ("Slice 3 MUST ADMIT this event") fires on exactly that state —
**and it fired live this session, on the truncated final event (below), which is the admit path
demonstrated in an artifact rather than asserted.**

### 169.1 The counts, accepted leg `P24_M26S1F1_CTRL49` (blinking, `StaticMeshActor_49`, B1 PASSED — `modal_rot (0,0,0)`, bbox == `CALIB_BBOX` exactly, 59 rows, 100 % modal, attempt 1)

| startFrame | state | maxCount | pctOfFrame | arms | resolved | discarded | contributed | skippedHidden | collisions |
|---|---|---|---|---|---|---|---|---|---|
| 4 | **MEASURED_NONZERO** | 66862 | **7.2550** | 4 | 4 | 2 | 2 | 3 | 2 |
| 20 | **MEASURED_NONZERO** | 66832 | **7.2517** | 4 | 4 | 2 | 2 | 3 | 2 |
| 36 | **MEASURED_NONZERO** | 66862 | **7.2550** | 4 | 4 | 2 | 2 | 3 | 2 |
| 52 | **MEASURED_NONZERO** | 66843 | **7.2529** | 4 | 4 | 2 | 2 | 3 | 2 |
| 68 | **MEASURED_NONZERO** | 66862 | **7.2550** | 4 | 4 | 2 | 2 | 3 | 2 |
| 84 | **MEASURED_NONZERO** | 66843 | **7.2529** | 4 | 4 | 2 | 2 | 3 | 2 |
| 100 | **MEASURED_NONZERO** | 66852 | **7.2539** | 4 | 4 | 2 | 2 | 3 | 2 |
| 116 *(frame-cap truncated, D-5)* | **NOT_MEASURED** | 0 | — | 1 | 1 | 1 | 0 | 3 | 1 |

**Every full event now measures at 7.25 % against the banked 7.80 % rect — the exact numbers
PART TWENTY-THREE proved were being thrown away.** The truncated event's only resolved frame was
a position-1 dummy; all its frames discarded ⇒ **`NOT_MEASURED`, not zero.** Exactly the
pre-declared expectations, including that one.

⛔ **THIS IS NOT `F-6` VALIDATION AND IS NOT CLAIMED AS ANY GATE.** Fault (ii) still discards
half the frames (collisions=2 per event above IS fault (ii) being observed per frame); `F-6`
item 5 and the `SM_Ramp2` control belong to the fault-(ii) FIX, which is not this turn's work.

### 169.2 And the `missing_texture` companion leg measures clean end to end

`P24_M26S1F1_MTEX49` (same target, B1 PASSED, attempt 1): **8 of 8 events `MEASURED_NONZERO`
at 7.2550–7.2567 %, `arms=4 resolved=4 discarded=0 contributed=4 collisions=0` on every one.**
A non-hide anomaly never hides its target, so no frame is polluted and nothing is discarded.
*(Its role as the R-C refuter is §171; the 255 detector being SILENT here while FIRING on the
blinking leg is also the two-sided behaviour `G96` asks of a detector, observed in passing —
recorded as an observation, not claimed as the formal item-5 gate.)*

## 170. The instrument shakedown FAILED FIRST — branch `B4`, and the pre-declared table caught it

The first `M-4` build produced **0 `M24 ENDFRAME` lines against 30 `M23 PASS` lines** on the
shakedown attempts — the pre-declared **B4: instrument did not report**.

🚨 **CAUSE, READ FROM ENGINE SOURCE, THEN FIXED: `GFrameCounter` INCREMENTS BEFORE `OnEndFrame`
BROADCASTS.** `LaunchEngineLoop.cpp:5568` (`GFrameCounter++`, after the world tick) vs `:5623`
(`FCoreDelegates::OnEndFrame.Broadcast()`). At sample time the counter is already N+1, so a
sampler filtering `RequestId == GFrameCounter` matches nothing, every frame. The sampler now
consumes an explicit armed-this-tick id list (`9f91472`). → **`G132`.**

⚠ Three B1 pose-gate discards preceded the accepted leg (3 consecutive A47-bifurcated settles,
banked as `P24_M26S1F1_CTRL49_try1..3` of the first cycle; the re-run accepted on attempt 1 at
the exact calibration pose). **A63 discipline: discarded for how they ran, all banked.** The
discarded attempts also served as the instrument shakedown above — their PASS/DISCARDED lines
were read for instrument LIVENESS only, never for a verdict.

## 171. `TASK 2` — the mechanism, ESTABLISHED by the pre-declared discriminator. Branch `B1`.

**M-4 measured, per armed frame, the target's hidden state at `OnEndFrame` — after every
subsystem tick, which is the state the frame rendered** (all togglers run in world tick; the
render commands are enqueued before `OnEndFrame`).

### 171.1 The join, blinking leg — 29 of 30 armed frames joined, ZERO violations

| | |
|---|---|
| DUMMY frames (extent 1×1) with `hiddenAtEndOfFrame=1` | **15 of 15** |
| REAL frames (1280×720) with `hiddenAtEndOfFrame=0` | **14 of 14 joined** |
| **R-A violations** (dummy but visible at render) | **0** |
| **R-B violations** (real but hidden at render) | **0** |
| per-event pattern, all seven full events | kind **D-R-R-D** = hiddenEof **1-0-0-1**, identical |
| the 30th frame (id 122) | unjoined — §172, fully accounted for |

### 171.2 The R-C refuter did NOT fire

`missing_texture` (never hides its target): **32 of 32 armed frames REAL, hiddenEof 0 on every
one, ZERO dummies.** The alternation is not a property of burst position or of the mask — **it
tracks the target's rendered hidden state and nothing else.**

### 171.3 ⇒ THE MECHANISM, in one chain, every link either measured here or read from source

1. **The arm gate's hidden read is STALE BY ONE TICK relative to what the armed frame renders.**
   `ArmIfMeasurable` reads `Actor->IsHidden()` inside `UAnomalyCaptureSubsystem::Tick`;
   `blinking` toggles hidden inside `UAnomalyInjectorSubsystem::Tick`, which runs LATER in the
   same engine frame. **Measured:** on all 15 dummy frames the gate read not-hidden and the
   end-of-frame state was hidden — the state changed between the arm decision and the render,
   every time, and only on those frames.
2. The toggle reaches the SAME frame's render — `F-1`'s guarantee (`SendAllEndOfFrameUpdates`
   inside `BeginRenderingViewFamilies`). **The guarantee that exonerated the tag path is what
   makes the hide path bite.**
3. A hidden actor's primitives are not in the view's visible set, and
   `bHasCustomDepthPrimitives` is set only during relevance over VISIBLE primitives
   (`SceneVisibility.cpp:2470-2474`).
4. No view has custom-depth primitives ⇒ `RenderCustomDepthPass` returns false
   (`CustomDepthRendering.cpp:148`) ⇒ the dummy is bound (`SceneTextures.cpp:959`, D-3) ⇒ 255.

**The fixed D-R-R-D pattern decodes exactly:** with C(t)=H(t−1) and blinking's half-period 3
locked to the fire tick, arms land at ticks {6,10,11,12} of the 16-tick cycle; the rendered
state at those ticks is hidden-visible-visible-hidden. Deterministic because both the blinking
phase and the arm cadence are locked to the burst — nothing stochastic was ever involved.

**The other two candidates:** C-A (no primitive carries the flag) — REFUTED: zero write-side
verify collisions on both legs, on top of D-2's exoneration. C-C (pass skipped for an unrelated
per-burst reason) — REFUTED by R-C: 32/32 armed frames on a never-hidden target produced the
pass.

### 171.4 🚨 Why this fault is WORSE than the 255 it wears here — for the fix design, not for now

On StackOBot nothing else writes custom depth, so a stale arm lands on the DUMMY and announces
itself as 255. **On a host title whose own effects keep custom depth produced, the same stale
arm reads the REAL stencil, finds no tagged pixels, and returns a CLEAN `MEASURED_ZERO` on a
target that draws — no 255, no collision, the dangerous direction, silently.** The MAX
reduction saves any event that also gets one clean visible frame, but an event whose qualifying
frames all land on toggle boundaries would be wrongly vetoed under slice 3. **The fault-(ii)
fix must close the stale read itself, not rely on the dummy's loudness.**

⛔ **NO FIX ATTEMPTED OR DESIGNED THIS TURN — the mask is `m26`'s validity instrument.** The
obvious shapes (sample the post-toggle state; arm from a point after the injector tick;
consume the same deferred sample `m20` built) each have `LOCK-1`/`F-2` interactions, and the
design is the owner's call under the `F-6` gate.

## 172. FILED, NOT FIXED: one stray arm fires AFTER `FinishRun`, every leg

The unjoined id 122 exposed a pre-existing defect, visible in the P23 leg too (its id-122 PASS
row): **the mask block in `Tick` runs below the phase switch and checks only
`bMaskMeasure && Async` — when the switch calls `FinishRun` (which runs `EndRun`: summary
printed, tags restored, cvar restored), the SAME tick then re-tags the target and issues one
more arm.** Its pass executes after the cvar restore (`mode=1` in its PASS record), its
readback is never collected, its `M24 ENDFRAME` never logs (`bRunning` false), **and the
re-tag survives `RestoreAll`, leaving `bRenderCustomDepth` asserted on the target after the
run.** Harmless in a bench leg (process exits); real in a long-lived session. **Filed for the
fault-(ii) fix turn — it is one guard (`bRunning`) and it is NOT this turn's work.**

## 173. `P6` — measured unchanged, both ways

48-field key-set comparison, post-fix `P24_M26S1F1_CTRL49` vs pre-fix banked
`P23_M23_CVAR_CTRL49`: **`annotation.json` 0 added / 0 removed (48 = 48); `run_summary.json`
0 added / 0 removed.** The fix writes logs and in-memory state only; no writer code touched.

## 174. Environment and identity after this part

| | |
|---|---|
| plugin commits | `795f2a4` (frame-scoped discard + M-4) · `9f91472` (sampler off-by-one) — **pushed with this part** |
| bench commits (local-only) | `cb299aa` (pre-declaration) · `3419ea5` (`p24_join.py`) |
| staged exe | **`444D4812`** (hot-swap; built==staged hash-verified; container UNCHANGED from the `m26` cook: `utoc 9334496D · ucas 62EB0072 · pak 78C977A5`, 4 maps — `G121` identity stated in full) |
| predecessor exes | `722266A7` archived to `_binary_baselines\StackOBot.exe.m26-slice1-m1m3-instrument-722266A7` BEFORE the swap; interim `1648027C` (dead sampler) produced only discarded attempts and was not preserved |
| legs banked | `P24_M26S1F1_CTRL49` (+ `_try1`), `P24_M26S1F1_MTEX49` (+ `_try1`), 3 pose-discarded first-cycle tries |
| A44 | new strings (`M24 ENDFRAME`, `framesDiscarded`, `M24 FRAME DISCARDED`, `hiddenAtEndOfFrame`) present in the STAGED exe, UTF-16, controls present |

**WHAT THIS PART SETTLES: slice 1's collection now keeps what the instrument measures, and the
alternation has a mechanism — the arm gate trusts a hidden state that is one tick older than
the frame it arms. What it deliberately does NOT do: fix fault (ii), run `SM_Ramp2`, claim
`F-6`, unblock `H5`, start slice 2 or 3, or tag.**

---

# PART TWENTY-FIVE — the fault-(ii) fix is DESIGNED. Ruling 1 governs it. **NO CODE.**

**PART TWENTY-FOUR ACCEPTED; the fix is approved IN PRINCIPLE, design first.** ⛔ **DESIGN ONLY —
nothing implemented this turn. NO TAG. `P6` NOT MOVED. Stencil range stays 200/255.
`feature/stencil-capture` READ-ONLY. `H5` legs BLOCKED. Slices 2/3 not started.**

---

## 175. `RULING 1` — the hazard IS the specification. Recorded verbatim; it governs every choice below.

> **"The 255 dummy is a PROPERTY OF THIS BENCH, not of the defect. On a host title where custom
> depth is produced by other primitives, a stale arm yields a clean MEASURED_ZERO with no tell —
> and under slice 3 that SILENTLY DELETES A GOOD EVENT. The fix must close the stale read itself.
> Any fix that relies on detecting the dummy is a fix that works only where we happen to be
> looking."**

*(G124's shape in a new place: the loud symptom and the mechanism are not the same thing, and the
loudness is environmental.)* ⇒ **Two consequences used below: (1) the fix must relocate the READ,
not improve the dummy detector; (2) the correctness test must measure the actual property — "the
gate read the state the frame rendered" — never the symptom's absence.**

## 176. `H-1` — THE OPTIONS, COSTED. **Option B is the design.**

**The engine facts every option is priced against (read this session, 5.1 source):**

| # | fact | source |
|---|---|---|
| 1 | both subsystems are `FTickableGameObject`s, ticked together inside `UWorld::Tick` — their mutual order is REGISTRATION ORDER, engine-internal, with no public priority API | `LevelTick.cpp:1606` (`FTickableGameObject::TickObjects`) |
| 2 | **`FWorldDelegates::OnWorldTickEnd` broadcasts as the LAST line of `UWorld::Tick`** — after every tick group, timer, and tickable | `LevelTick.cpp:1814` |
| 3 | the draw — where `F-1`'s proxy flush lands — runs AFTER the world tick, same frame | `GameEngine.cpp:1775` (world tick) → `:1891` (`RedrawViewports`) |
| 4 | `GFrameCounter` is still N at `OnWorldTickEnd` (it increments later, `LaunchEngineLoop.cpp:5568`) — request-id semantics unchanged; `G132` does not bite there | PART TWENTY-FOUR |

### 176.1 Option A — reorder the tick dependency. ⛔ **REJECTED — it is a BEHAVIOUR change, twice over, and there is no stable lever.**

Two sub-forms, both bad:
- **Injector earlier:** 🚨 **changes WHEN anomalies apply. The injector is the SHIPPING SELECTOR** —
  toggle timing relative to captured frames is what the dataset records; this is a behaviour change
  to the product, not a measurement change.
- **Capture later:** leaves the injector alone but **moves `SampleDeferredHidden` and the
  `FireHidden` label sampling with it** — the one-tick-stale hidden sample is an `m20`-CHARACTERISED
  property of every shipped `annotation.json`; making it fresh silently changes `HiddenByIndex`,
  `manifested`, and positive-frame derivations on every future leg. **A label-content change wearing
  a measurement fix's clothes.**
- And mechanically: tickable order is registration order (fact 1) — any "reorder" rests on
  engine-internal iteration, exactly the fragility that produced this fault.

### 176.2 🎯 Option B — arm from `OnWorldTickEnd`: post-toggle by POSITION, not by luck. **CHOSEN.**

**Move the mask block — `VerifyPendingTags` → `EnqueueDrain` → `CollectResults` →
`ArmIfMeasurable`, as a unit, internal order preserved — out of `UAnomalyCaptureSubsystem::Tick`
(`:441-447`) into a `FWorldDelegates::OnWorldTickEnd` handler** (registered at `Initialize`,
removed at `Deinitialize`, mirroring PART TWENTY-FOUR's `OnEndFrame` pattern; guards: world match,
`bMaskMeasure`, **`bRunning`** (= `H-5`), `Async` valid).

| property | why it holds |
|---|---|
| the arm read is POST-TOGGLE | fact 2 — every tickable (the injector included) has ticked before the broadcast |
| the tag and the arm still make the SAME frame | fact 3 — the draw (and `F-1`'s flush inside it) has not happened yet |
| robust to registration order | the anchor is a POSITION in the frame, not a place in the tickable array |
| request ids unchanged | fact 4 |
| **tick ordering: NOTHING moves** | the phase machine, `CaptureCurrentFrame`, pacing, `SampleDeferredHidden`, labels — all stay in `Tick`, byte-for-byte semantics |
| **behaviour outside `AnomalyCapture`: NONE** | the injector is untouched; no module gains or loses an API; captured frames, labels, seeds identical |

⚠ **Honest residual, stated:** `OnWorldTickEnd` covers everything that ticks. Code that toggles
hidden state AFTER the world tick (inside the draw itself, or another `OnWorldTickEnd` handler
registered later) is outside the anchor. **That residual is what `H-2`'s enforcing confirmation
exists for — it is caught and discarded, never admitted.**

### 176.3 Option C — the injector publishes the state the frame will render. ⛔ **REJECTED ON RULING 1.**

It fixes **blinking**, not the class. `IsHidden()` can be toggled by ANY code — on a host title,
by the host's own logic, which will never publish through our API. *"Have the toggler tell us"* is
a fix that works only where we happen to be looking — Ruling 1's exact wording. It also adds a
capture-serving public surface to the SHIPPING module (`ANOMALYINJECTOR_API`) for the benefit of a
non-Shipping measurement — the `G127` module boundary crossed in the wrong direction.

### 176.4 Option D — keep the arm point, discard at resolve using the `m20` deferred sample. ⛔ **REJECTED as the fix; its IDEA survives in `H-2`/`H-3`.**

Post-hoc discard makes the numbers safe but (1) **the premise is itself order-fragile** — the
deferred sample means "the state frame t rendered" only under the SAME unguaranteed tick order
that produced this fault; (2) the blinking arm budget stays half-wasted; (3) it treats the symptom
class Ruling 1 forbids treating. **What survives: resolve-time render-state checking — anchored at
`OnEndFrame` (order-robust), as the enforcing confirmation below.**

## 177. `H-2` — THE CORRECTNESS TEST. The property, not the symptom.

**The property to prove: EVERY FRAME THAT CONTRIBUTES TO A MEASUREMENT WAS VERIFIED VISIBLE AT
BOTH BRACKETS OF ITS RENDER.** "The dummies stop" is environmental (Ruling 1) and is NOT the test.

**The M-4 sampler BECOMES PRODUCT, and it becomes ENFORCING — whitelist polarity:**
- At `OnEndFrame` (after the draw is enqueued, before anything else can tick), each armed-this-tick
  frame's target is sampled. **A frame CONTRIBUTES only if its sample RAN and read VISIBLE.**
  Hidden, or sample missing ⇒ the frame is fed into the existing `PollutedRequests` /
  `bFramePolluted` path ⇒ **frame-scoped discard**.
- **Polarity rationale:** a missing check must never read as a passed check (`G119`'s family).
  Blacklist ("pollute if seen hidden") admits the unchecked frame; whitelist does not.
- The `M24 ENDFRAME` log line stays, so the offline join (`p24_join.py`) remains auditable.
- **It lives inside `ANOMALY_CAPTURE`** — permanent instrument code, zero Shipping footprint
  (`G127`), not dev-scratch.

**So the render is BRACKETED:** visible at `OnWorldTickEnd` (the arm) ∧ visible at `OnEndFrame`
(the confirmation), with the draw between them. Anything that changed the state before the frame
was drawn lands on one bracket or the other ⇒ discarded ⇒ **`MEASURED_ZERO` is unreachable from a
stale read even in the residual cases Option B cannot anchor.**

**Gate legs and their pre-declarable predictions** *(to be pre-declared as a file before the fix
leg runs, per standing practice)*:

| leg | prediction |
|---|---|
| blinking control (`StaticMeshActor_49`) | `arms=4 resolved=4 framesDiscarded=0 framesContributed=4` per full event · join: every armed frame REAL ∧ `hiddenEof=0` · `ENDFRAME` line count == armed count |
| `missing_texture` control | unchanged from P24: 4/4, 0 discards |
| **`missing_object` leg (new to the gate)** | **ZERO in-window arms** (the gate now refuses the whole hidden window) · **4 post-revert arms, all contributing** — this exercises `P-2`'s riskiest path BEFORE slice 3 exists |
| item-5 probe leg (below) | the probe frame fires BOTH detectors and is discarded; the event still measures from its clean frames |

**`F-6` item 5 (the 255 detector proven still live, BOTH ways) — the mechanism, designed now:**
post-fix, a healthy bench NEVER produces a dummy naturally (that is the point), so the detector's
silence needs a live-fire demonstration on the NEW binary, not only the control-pair argument from
the banked pre-fix P24 leg. **THE PROBE: a default-OFF flag (`IAI.Capture.MaskProbe`) that, on a
gate leg only, issues ONE deliberate arm on a KNOWN-hidden tick (`missing_object` in-window),
logged loudly as PROBE.** Expected: dummy bound ⇒ 255 detector FIRES; end-of-frame confirmation
reads hidden ⇒ frame DISCARDED; the event still resolves from its post-revert frames. **One probe
frame demonstrates the 255 detector, the enforcing confirmation, and the frame-scoped discard all
live on the shipped binary — and the admit bias disposes of it safely.** The probe bypasses
`LOCK-1` for exactly that one arm, by design, only under the flag.

## 178. `H-3` — WHAT IT DOES TO `LOCK-1`. Preserved, and strengthened — stated point by point.

**The arm gate stays `LOCK-1`'s enforcement point** — the refusal stays in `ArmIfMeasurable`,
same code, same `skippedHidden` accounting. What moves is WHEN it runs: its `IsHidden()` read now
happens after every toggler, so **the rule finally refuses on the state the frame actually
renders** rather than a one-tick-stale proxy of it.

**`F-2`'s banked rule** — *hidden at ANY sampled point ⇒ `NOT_MEASURED`, never `MEASURED_ZERO`* —
**maps onto the design at three points:**

| sampled point | where | on hidden |
|---|---|---|
| **ARM** | `ArmIfMeasurable` at `OnWorldTickEnd` | no arm at all (`skippedHidden`) |
| **VERIFY** (write side) | `VerifyPendingTags`, per tick, unchanged | in-flight frames polluted (frame-scoped, P24) |
| **RESOLVE** (render state) | the enforcing `OnEndFrame` confirmation (§177) | frame discarded before its count is read |

⇒ **`MEASURED_ZERO` now requires: visible at arm ∧ visible at confirmation ∧ tag verified held ∧
a clean read.** An event with no such frame stays `NOT_MEASURED` by the P24-proven initialisation
path. **The two zeros still never share a representation anywhere.**

## 179. `H-4` — THE ARM BUDGET AFTER THE FIX. No type loses its window; blinking's waste becomes yield.

The gate arms at most one record per tick (code: `ArmIfMeasurable` returns after one arm) and a
record keeps arming on later eligible ticks until its 4-arm cap — unchanged.

| anomaly class | qualifying ticks post-fix | budget |
|---|---|---|
| **non-hide (6 ids)** | unchanged — any in-window tick | **4 issued / 4 usable** (was 4/4) |
| **`blinking`** | rendered-VISIBLE ticks: ~3 per positives window (half-period 3 in 8) **plus post-revert ticks backfill to the cap** (the record arms until 4; post-revert the target is restored) | **4 issued / 4 usable** (was 4 issued / 2 usable) — predicted `framesDiscarded=0` |
| **`missing_object`** | in-window: **ZERO** (correct — the target renders hidden all window; pre-fix the stale read wasted an arm at window start) · post-revert: the 6-tick window (`SettleAfterRevert` 2 + `PostGap` 4), one arm per tick | **4 issued / 4 usable in a 6-tick window — room confirmed, 6 ≥ 4** |

**Zero-qualifying-arms risk: NONE.** Every type retains ≥ 3 qualifying ticks per event; the fix
does not SKIP ticks the instrument previously used — it stops spending budget on ticks that
measured nothing. ⚠ *Blinking taking some arms post-revert measures the same quantity under the
same settled-camera scope note as `missing_object` (§113.2) — already in the tag's scope statement.*

## 180. `H-5` — the stray post-`FinishRun` arm, folded in. One guard, and Option B absorbs it.

The defect (§172): the Tick mask block ran below the phase switch with no `bRunning` check, so the
`FinishRun` tick re-tagged the target after `RestoreAll` and issued one stray arm whose pass ran
after the cvar restore (`mode=1`, the id-122 row). **In this design the block lives in the
`OnWorldTickEnd` handler, whose guard set INCLUDES `bRunning`** — `FinishRun` (inside `Tick`, i.e.
inside `UWorld::Tick`) has already cleared it by the time the handler fires that same tick. The
stray arm, the post-`RestoreAll` re-tag, and the post-restore pass all die with that one guard.
**Verification on the gate legs: `M23 PASS` line count == total arms issued in the `M26S1`
summaries, and no PASS record with `mode != 3`.**

## 181. State after PART TWENTY-FIVE

| | |
|---|---|
| the design | **written (§175–§180), NOT implemented — owner's ruling next** |
| Ruling 1 | recorded verbatim (§175) and used to reject Options C and D |
| production code | **unchanged this turn** — HEAD still `475147e`'s tree for `Source/` |
| gate owed after the fix | **`F-6` all five items (item 5 via the §177 probe) → `SM_Ramp2` with peak-IN/OUT (`A-4`) → only then the `H5` legs** — adopted verbatim from the ruling |
| unchanged | `P6` · stencil range 200/255 · `feature/stencil-capture` at `76cac74` · slices 2/3 · **NO TAG** |

**WHAT THIS PART SETTLES: the fix has one shape that changes nothing outside the measurement —
relocate the read to a position in the frame that is post-toggle by construction, bracket the
render with an enforcing confirmation so no stale read can ever contribute, and let the same
guard retire the stray arm. What it deliberately leaves to the owner: whether to build it.**

---

# PART TWENTY-SIX — the fix SHIPS and PROVES ITSELF on four legs; **the `SM_Ramp2` control then FAILS `F-6` item 2 and exposes TWO NEW instrument limits. HALT.**

**The Part-25 design implemented as approved, with amendments A-1 and A-2. Gate run in the adopted
order, predictions pre-declared first. STOPPED AT THE FIRST FAILURE — item 2.**
⛔ **`H5` LEGS NOT RUN. NO TAG. `P6` NOT MOVED (measured again below). Stencil range untouched.**

**Predictions pre-declared BEFORE the fixed build existed:
`CaptureBench/tools/p26_fix2_gate_predeclared.md`, commit `84106bd`.**

---

## 182. A-1 and A-2 recorded, and the implementation

**A-1 (the probe is a gate artefact):** default OFF; **inert in delivery mode by a GUARD at the
fire site** (`!bDeliveryMode` in the arm condition, not a convention); `PROBE` in every log line;
the fire is **artifact-attributable** — `run_summary.mask_probe_arms`; and
`PRE-DELIVERY-CHECKLIST.md` §1 gains the OFF check (same class as the token read-back). **A48
echo implemented:** every mask run logs `probe EFFECTIVE=<n> (flag=<n>, deliveryMode=<n> …)`.
**A-2 (the residual gets a name and a counter):** a frame the ARM gate accepted whose end-of-frame
confirmation read HIDDEN is counted in **`framesResidual`** — its own bucket, never shared with
pollution (`framesDiscarded`), never with `framesUnconfirmed` (sample never ran), never with
`probeArms`. **Reported per event in the summary line AND per leg in
`run_summary.mask_residual_discards` — artifact chosen over log-only because the residual on a
host title would be discovered from delivered artifacts, and A-1 already required the probe field
in `run_summary`; the two ride together** (`capture_path`/`key_ring_*` precedent; run_summary is
not `P6`).

**The implementation (commit `4a9631a`):** the mask block moved from `Tick` to a
`FWorldDelegates::OnWorldTickEnd` handler (guards: world match · `bMaskMeasure` · **`bRunning`** ·
`Async`); the enforcing confirmation is WHITELIST — a frame contributes only if its `OnEndFrame`
sample ran and read visible; `IAI.Capture.MaskProbe` issues at most ONE known-hidden arm per run,
only when the normal arm did not fire that tick; the final mask collect moved BEFORE
`WriteRunSummary` so the artifact counters are complete.
**Identity:** staged exe **`DBA2D8EC`** (built==staged hash-verified; predecessor `444D4812` — the
P24 measurement binary — archived FIRST at `_binary_baselines\StackOBot.exe.m26-p24-fault1-fix-444D4812`);
container UNCHANGED (`utoc 9334496D · ucas 62EB0072 · pak 78C977A5`, 4 maps). **A44:** all seven
new strings present in the STAGED exe, UTF-16, controls present.

## 183. The pre-declared predictions, restated verbatim before any result

> **L1** "Every full event: `arms=4 resolved=4 framesContributed=4 framesDiscarded=0
> framesResidual=0 framesUnconfirmed=0 probeArms=0 collisions=0 state=MEASURED_NONZERO`,
> `pctOfFrame` 7.2–7.3 … every armed frame REAL … `M24 ENDFRAME` line count == armed-frame count;
> `M23 PASS` line count == total arms issued; no PASS record at mode != 3."
> **L2** "Unchanged from P24: 8/8 events 4/4 contributed, 0 discarded/residual/unconfirmed,
> collisions=0, MEASURED_NONZERO ~7.25 %."
> **L3** "ZERO in-window arms … 4 post-revert arms … all 4 contributing … trailing event(s) …
> possibly zero ⇒ NOT_MEASURED, never MEASURED_ZERO."
> **L4** "Exactly ONE probe arm … the 255 detector FIRES … the sample reads HIDDEN … bucketed
> PROBE … probed event `arms=4 resolved=4 probeArms=1 framesContributed=3` …
> `mask_probe_arms=1`."
> **L5** "Every full event `MEASURED_NONZERO`, `collisions=0` … A-4 beside the result: peak-OUT
> `0.2955` > peak-IN `0.1785` … `SM_Ramp2` MUST read non-zero."

## 184. L1–L4: **every prediction met.** `F-6` items 1, 3, 4, 5 PASS.

| leg | accepted | result vs prediction |
|---|---|---|
| **L1** `P26_FIX2_CTRL49` (blinking; B1 PASS attempt 3; 2 pose discards banked) | ✅ | 7 full events `4/4 contributed, 0/0/0 discard/residual/unconfirmed, collisions=0, MEASURED_NONZERO 7.1963–7.2174 %`; truncated final event contributed its single clean arm; **29 PASS == 29 ENDFRAME == arms issued; ZERO mode≠3 records — §172's stray arm is GONE (H-5 verified)**; arms moved to rendered-visible ticks ({9,10,11,14}, `skippedHidden` 3→5) — exactly H-4's shape |
| **L2** `P26_FIX2_MTEX49` (B1 PASS attempt 2) | ✅ | 8/8 events 4/4, byte-matching P24's maxCounts (66862/66878); 32/32 REAL·visible |
| **L3** `P26_FIX2_MOBJ49` (B1 PASS; one 3-discard cycle re-run per A63/F-H) | ✅ | **0 in-window arms (`skippedHidden=8` — the whole hidden window refused) · 4 post-revert arms per full event, all contributing, 7.2529–7.2567 %**; the final event's post-revert window fell beyond the cap ⇒ `arms=0` ⇒ **`NOT_MEASURED` with the MUST-ADMIT warning — the admit path live again, in `P-2`'s purest shape** |
| **L4** `P26_FIX2_PROBE49` (B1 PASS; one cycle re-run) | ✅ | **exactly ONE probe arm** (id 6, in-window hidden tick); `PROBE RESULT detector255Fired=1 confirmationReadHidden=1`, frame bucketed PROBE; probed event `probeArms=1 framesContributed=3 MEASURED_NONZERO`; **`run_summary.mask_probe_arms=1`** — 🚨 **`F-6` item 5: the 255 detector, the confirmation and the frame-scoped discard demonstrated LIVE on the shipped binary; `G96` both ways (silent L1–L3, fires on demand L4)** |

`mask_residual_discards` = **0 on every leg** (the bench expectation). Artifact check (F-F):
`annotation.json` **48/48, 0 added 0 removed** (`P6` not moved); `run_summary` **exactly
{`mask_probe_arms`, `mask_residual_discards`} added** — the declared +2, nothing else. Probe A48
echo on probe-free legs: `probe EFFECTIVE=0 (flag=0, …)`.

⇒ **THE PART-24 MECHANISM IS CLOSED: with the arm read post-toggle and the render bracketed, a
blinking target yields 4/4 usable arms and zero dummies on the calibration control.**

## 185. 🛑 L5 — `P26_FIX2_RAMP` (accepted attempt 1; B1 NOT APPLICABLE, G117): **`F-6` ITEM 2 FAILS**

**Observed, before any attribution:**

| # | observation |
|---|---|
| 1 | **29/29 armed frames: `customStencilExtent=1x1`** — the custom-depth pass was produced on NO armed frame |
| 2 | **29/29: `hiddenAtEndOfFrame=0`** — the target was NOT hidden at render, ever. `framesResidual=0`. The Part-24 fault is not what this is |
| 3 | tag applied and held (`taggedComponents=1`, zero write-side verify collisions), `mode=3` on all 29 |
| 4 | the ramp's projected bbox is VALID and in-frustum (286×189 px) on in-window label rows; camera steady at (−20, −40) |
| 5 | events 2–8: the 255 detector fired per frame (`unassignedCount=1` each) ⇒ all frames discarded ⇒ `NOT_MEASURED` + MUST-ADMIT — **fails safe** |
| 6 | 🚨 **event 1 (`startFrame=4`): NO 255 on its four frames (`unassignedCount=0`) ⇒ 4 frames CONTRIBUTED count 0 ⇒ `MEASURED_ZERO` — a clean-looking zero on the A35 control, through every guard built this turn** |

**A-4 beside the result, as required: banked peak-OUT `0.2955` > peak-IN `0.1785`. And the
decisive artifact fact: `CM_CM_RAMP` — the PART ELEVEN hide-measurement — ran at the IDENTICAL
camera (−20, −39.9989) and hiding the ramp changed in-bbox pixels by 0.1785 ⇒ THE RAMP DRAWS in
the base pass from this exact view.** A drawing, tagged, un-hidden, in-frustum target whose
custom-depth pass never runs is not the Part-24 fault and not occlusion culling.

### 185.1 FINDING 1 — **the instrument is structurally blind to NANITE geometry in this engine. ESTABLISHED.**

| # | link | source |
|---|---|---|
| 1 | `SM_Ramp` (the ramp's `asset_name`) serializes a non-default `NaniteSettings` **with `bEnabled`** — the Nanite-ENABLED signature (properties serialize only when non-default; the default is false) | `Content\StackOBot\Environment\Modular\SM_Ramp.uasset` (name-table scan; ⚠ editor confirmation is one query away and was NOT taken — `G97`) |
| 2 | **`Nanite::FSceneProxy::GetViewRelevance` NEVER sets `bRenderCustomDepth`** — read in full, both branches | `NaniteResources.cpp:941-1010` |
| 3 | `bHasCustomDepthPrimitives` is raised ONLY from `ViewRelevance.bRenderCustomDepth` | `SceneVisibility.cpp:2470` |
| 4 | the 5.1 custom-depth pass has **no Nanite path at all** (zero matches in the file); it rasterises classic mesh draw commands | `CustomDepthRendering.cpp` |

⇒ **a Nanite primitive cannot write custom depth in UE 5.1, with any flag, any cvar, any timing.**
The component property write succeeds and verifies (D-2 re-confirmed) — and can never reach
pixels. The bench control that "worked" all milestone (`StaticMeshActor_49`) lives in the
script-built gate level on non-Nanite geometry, **so the bench could never have shown this** —
`G124`'s shape again: the loudness of a fault is environmental. → **`G134`.**

### 185.2 FINDING 2 — **a CLEAN `MEASURED_ZERO` from an unproduced pass: the contribution path lacks a "pass ran" precondition, and the 255 detector cannot supply it. MEASURED.**

The REDUCE data re-reads the whole milestone's detector history: **every 255 fire ever recorded
was `unassignedCount=1` — ONE pixel.** The dummy is a **1×1** texture; `.Load` at any other pixel
returns **0** out-of-bounds, so 255 can only ever appear at texel (0,0), and only when the
depth-dummy comparison passes there. **The detector is a single-pixel, view-content-contingent
signal: event 1's frames simply had a non-far pixel at (0,0), the detector stayed silent, and
four zeros from a pass that never ran were CONTRIBUTED.** Under slice 3 that vetoes a control.
🚨 **This is Ruling 1's exact hazard shape realised ON OUR OWN BENCH, through the detector's
environmental dependence rather than a host title.** The reliable pass-ran discriminator —
`CustomStencilExtent` 1×1 vs view-sized — **is already collected per frame and logged since M-2,
and is NOT yet a contribution precondition. NOT FIXED THIS TURN** — the gate has failed and a
same-turn change to the validity instrument is exactly what the standing rule forbids. → **`G133`.**

## 186. The halt, and what it does and does not mean

- **`F-6`: items 1, 3, 4, 5 PASS. Item 2 FAILS. STOPPED at the first failure, as ruled. The
  `SM_Ramp2` control did not over-fire — it exposed the instrument. That is what `N-2`/`G-5`
  exist for.**
- ⛔ **`H5` LEGS NOT RUN, STILL BLOCKED** — an `H5` zero is now DOUBLY uninterpretable: the
  extent precondition does not exist yet, and Nanite scope is unresolved. *(For the record:
  `InstancedFoliageActor` foliage on this title may itself be Nanite-affected — its measurability
  is now an open question of the same kind.)*
- **The Part-24/Part-25 fix itself is NOT the failure** — it passed every gate it was designed
  for (L1–L4) and the ramp leg's `framesResidual=0`/`hiddenAtEndOfFrame=0` show the stale-read
  class is closed.
- **What this means for the cure is the OWNER'S territory, stated but not designed:** (1) the
  extent precondition (contribute only view-sized frames) closes the false-zero hole and lands
  every all-dummy event in `NOT_MEASURED` ⇒ ADMIT — safe, but it makes Nanite targets permanently
  unmeasured by C-1 on this engine; (2) `m26`'s scope statement must carry "the mask cannot see
  Nanite geometry in UE 5.1" as a KNOWN limit, and a Nanite-heavy host title makes that limit the
  common case, not the corner; (3) whether `SM_Ramp2` can remain the `N-2` control for a
  custom-depth instrument is now a real question — a control the instrument cannot ever see
  cannot certify over-fire.

## 187. State after PART TWENTY-SIX

| | |
|---|---|
| the fix | ✅ shipped (`4a9631a`), proven on L1–L4; A-1 and A-2 implemented |
| `F-6` | 🛑 **items 1/3/4/5 PASS · item 2 FAIL — HALTED at first failure** |
| new instrument limits | **`G133`** (single-pixel, view-contingent 255 detector; extent is the pass-ran datum) · **`G134`** (Nanite blindness, UE 5.1 structural) |
| legs banked | `P26_FIX2_CTRL49/MTEX49/MOBJ49/PROBE49/RAMP` + every discarded attempt (A63) |
| `P6` | measured unchanged again — 48/48; `run_summary` +2 exactly as declared |
| `H5` | ⛔ **BLOCKED** · slices 2/3 NOT STARTED · **NO TAG** · `feature/stencil-capture` untouched |
| ⛔ next | **the owner's ruling** — the extent precondition, the Nanite scope question, and what replaces or reinterprets the `SM_Ramp2` control |

**WHAT THIS PART SETTLES: the stale-read fault is fixed and proven, the probe demonstrates the
detectors live on demand, and the ship-gate control did its job by failing — the instrument
cannot see Nanite geometry in this engine, and until a pass-ran precondition exists a target it
cannot see can read as a clean zero. Both limits were invisible from the non-Nanite bench level,
which is why the gate runs on `SM_Ramp2` at all.**

---

# PART TWENTY-SEVEN — **the `H5` instances are NON-NANITE: the cure reaches what it was built for.** The extent precondition ships and closes the false zero.

**T-1 answered first, as ruled — it outranked the three rulings and it lands on the good branch.**
Then Ruling 1 built and gated, Ruling 2's control search, Ruling 3's scope draft and the depth
question. ⛔ **`H5` LEGS STILL NOT RUN. NO TAG. `P6` NOT MOVED. Stencil range 200/255.
`feature/stencil-capture` READ-ONLY.**

**Pre-declared BEFORE the first asset was read: `CaptureBench/tools/p27_nanite_status_predeclared.md`,
commit `3fdea29`** — including T-2's predictions and the refuters `F-T2-A`/`F-T2-B`.

---

## 188. `T-1` — the branches, restated verbatim, then the answer

> **BOTH H5 TARGETS NON-NANITE** => the cure applies to the instances that motivated it. `m26`
> proceeds with a stated Nanite limit. Continue to T-2.
> **EITHER OR BOTH NANITE** => the cure cannot measure the `H5` instances on this bench. That is a
> SCOPE-LEVEL finding, not a defect. Report it, complete T-2, and HALT before T-3 and T-4.
> **MIXED** => report which, and treat the Nanite one as the second branch.

### 188.1 Method, and its confidence — stated before the result

**M-A (primary, offline):** the `.uasset` name-table signature. UE tagged-property serialisation
writes a property NAME only when the value differs from the class default, and
`FMeshNaniteSettings::bEnabled` defaults to **false** — so `NaniteSettings` **+** `bEnabled`
present is evidence the struct was written with a non-default sub-field. Tool committed as
`CaptureBench/tools/nanite_signature_scan.py` (`G106`: an instrument that grades a result is a
committed artifact), **with its two weaknesses printed beside every run** — the struct can
serialise for a different sub-field, and `bEnabled` is a generic name.
**M-B (corroboration): ABANDONED, NOT WORKED AROUND.** The editor bridge answered
`Connection refused (127.0.0.1:12029)`. Per the pre-declaration and `A59`/`G97`, no bridge reading
is attributed without a project-identity read-back, and there was no bridge. **M-A stands alone,
at EVIDENCE strength, never claimed as a measurement.**

### 188.2 🎯 THE RESULT — and the discriminator closes the Part-26 explanation

| actor | mesh | signature | measured behaviour |
|---|---|---|---|
| **`InstancedFoliageActor_0_0_0`** *(`H5` #1)* | `SM_Bush` | ✅ **plain — NON-NANITE** | — |
| **`BP_SplineSpawn_C`** *(`H5` #2)* | `SM_GenericPlane` | ✅ **plain — NON-NANITE** | — |
| 🚨 **`StaticMeshActor_49`** *(THE DISCRIMINATOR)* | **`Cube`** *(`/Engine/BasicShapes/`)* | ✅ **plain — NON-NANITE** | **MEASURED SUCCESSFULLY all milestone** |
| **`SM_Ramp2`** | `SM_Ramp` | 🚨 **NANITE** | **29/29 armed frames dummy (Part 26)** |
| **CB_GateLevel's whole target set** | `Cube`·`Sphere`·`Cylinder`·`Cone` — `make_gate_level.py:54-58` builds the level **entirely from `/Engine/BasicShapes/`** | ✅ **all plain** | the calibration bench, always green |

**Pre-declared reading `N-1` FIRES and `N-2` DOES NOT:** the discriminator is non-Nanite, so
**that is why it measured, and `G134`'s explanation CLOSES.** Every target that ever measured is
non-Nanite; the one that never did is Nanite. The signature predicts measurability on 5 for 5.

⇒ 🎯 **BRANCH ONE: BOTH `H5` TARGETS ARE NON-NANITE. THE CURE CAN SEE THE VERY INSTANCES THAT
MOTIVATED IT.** `m26` proceeds with a stated Nanite limit. Continue to T-2.

### 188.3 ⚠ The scope finding that travels with it — and it CONFIRMS "common case"

Sweeping all StackOBot content: **46 assets carry the Nanite signature.** *(A percentage is not
offered — the sweep denominator includes non-mesh assets, so it would mislead.)* **The PATTERN is
the finding: the authored structural geometry is overwhelmingly Nanite** — walls, floors,
platforms, pillars, roofs, pipes, fences, crates, buttons, doors, the ramp — **while foliage and
a few simple planes are not.** ⇒ **On this very title the Nanite limit would hit most of the level
and miss the foliage. `G134`'s "common case, not corner case" is not a projection about some
hypothetical host title; it is measured here.** ⚠ **The two `H5` instances are non-Nanite by luck
of what they are made of, not by any property of `H5`.**

## 189. `RULING 1` / `T-2` — the extent precondition SHIPS (`3beb3ba`) and closes the false zero

`customStencilExtent` (1×1 dummy vs view-sized real) is now a **CONTRIBUTION PRECONDITION**: a
frame contributes only on **positive evidence the pass ran**. New disjoint bucket `framesNoPass`;
the probe branch precedes it by design, so a deliberate dummy is attributed to PROBE, not to
no-pass. **The 255 detector is DEMOTED TO A SECONDARY SIGNAL, and the record says so.**

### 189.1 The decisive gate — `SM_Ramp2`, same target, same leg design

| | Part 26 (pre-precondition) | **Part 27 (post)** |
|---|---|---|
| event 1 (`startFrame=4`) | 🚨 **`MEASURED_ZERO`**, 4 frames contributed | ✅ **`NOT_MEASURED`**, `framesNoPass=4`, contributed **0** |
| events 2–8 | `NOT_MEASURED` via pollution | ✅ `NOT_MEASURED` via **`framesNoPass`** — the honest reason |
| leg summary | `notMeasured=7` of 8 | ✅ **`notMeasured=8` of 8** |
| artifact | — | **`mask_nopass_discards=29`** |

🚨 **THE FALSE ACCUSATION IS GONE. `F-T2-B` does not fire.** And the reason recorded in the
artifact is now the true one — "the pass never ran" — rather than an incidental 255.

### 189.2 L1–L4 unchanged — and `F-T2-A`'s LITERAL firing, investigated rather than waved away

**L2 `missing_texture`** `66878/66862…` and **L3 `missing_object`** `66878/66862/66843/66862` are
**byte-identical to Part 26**. **L4 probe**: exactly one probe arm, `detector255Fired=1
confirmationReadHidden=1`, probed event `probeArms=1 framesContributed=3 MEASURED_NONZERO`,
`mask_probe_arms=1`, `framesNoPass=0`. All dispositions unchanged; `framesNoPass=0` on L1–L4.

⚠ **BUT L1's `maxCount` MOVED — 66321-66516 (P26) → 66843-66878 (P27) — and `F-T2-A` as I wrote it
says any movement ⇒ HALT. It was investigated before proceeding, and the cause is POSE, not the
precondition:**

| leg | build | settled pose | modal bbox | maxCount range |
|---|---|---|---|---|
| `P24_M26S1F1_CTRL49` | pre-precondition | **(0,0,0) exact** | **`(0.0, 485.2, 306.1, 234.8)` = `CALIB_BBOX`** | 66832–66862 |
| `P26_FIX2_CTRL49` | pre-precondition | (359.83, 0.35) | `(0.0, 483.6, 301.6, 236.4)` — 4.5 px narrower | **66321–66516** |
| `P27_EXT_CTRL49` | **post-precondition** | **(0,0,0) exact** | **`= CALIB_BBOX`** | 66843–66878 |

**Four independent facts settle it.** (1) **The counts group by POSE, not by BUILD** — P24 and P27
straddle the change and agree to ~0.05 %; P26 sits apart with a 0.8 % smaller bbox area. (2) **The
armed tick ids are byte-identical between P26 and P27** (`9,10,11,14,25,…,121`) — the same frames
were armed. (3) **The bucket dispositions are identical** (`4/4 contributed, framesNoPass=0`) — no
frame was removed from contributing. (4) **Structural:** the precondition is a `continue` placed
BEFORE contribution; it can only ever REMOVE a frame, never alter a contributing frame's `Count`.
✅ **And L2 confirms it from the other direction: a DIFFERENT pose whose bbox AREA happens to match
(302.0×238.4 vs 306.1×234.8) returns BYTE-IDENTICAL counts across builds.** The count tracks
projected silhouette area, which tracks the settle pose within `B1`'s 8 px tolerance.
📌 **CRITERION CORRECTED IN THE RECORD, not quietly:** `F-T2-A` should have read *"any maxCount
that moves AT A MATCHED POSE"*. As written it cannot distinguish the change under test from a
confound this project characterised long ago (`A47`/`B1`). **A refuter that fires on a known
confound is a badly drawn refuter, and the fix is to say so, not to grant an exception.**

### 189.3 ⚠ `run_summary` gained a THIRD field — declared, not slipped in

`mask_nopass_discards` is **beyond the +2 that A-1/A-2 declared.** **I added it deliberately and
report it for veto:** Ruling 2 makes `SM_Ramp2` a POSITIVE test for the Nanite limit, and a test
whose evidence lives only in a log cannot be run from a delivered session. Measured key-set check:
**`annotation.json` 48/48, 0 added 0 removed (`P6` NOT MOVED)** · **`run_summary` +3**
(`mask_probe_arms`, `mask_residual_discards`, `mask_nopass_discards`), 0 removed.

## 190. `RULING 2` / `T-3` — the replacement `N-2` control: **no non-Nanite A35-shaped target exists. Saying so.**

**`SM_Ramp2` is retired as `N-2` and becomes the KNOWN-NANITE CONTROL**, as ruled: post-Ruling-1 it
must return `NOT_MEASURED` every time — a **positive test that the instrument recognises what it
cannot see**, and the place where a future engine bump adding Nanite custom-depth support would
show up first. §189.1 is its first passing run.

**The search, and its honest result:**

| requirement | available? |
|---|---|
| non-Nanite **and** drawing **and** selectable | ✅ **yes** — CB_GateLevel is `/Engine/BasicShapes/` throughout: `StaticMeshActor_73` (`Cylinder`) and `StaticMeshActor_85` (`Cone`) both appear in banked label rows with valid bboxes. ⛔ **`StaticMeshActor_100` is excluded — it is `H4`'s deliberately-occluded target** |
| …**and A35-shaped** | ⛔ **NOT AVAILABLE ANYWHERE** |

**Why not, precisely:** the peak-IN/peak-OUT split is banked for **exactly one target —
`SM_Ramp2`** (OUT `0.2955` > IN `0.1785`). For the other measured targets the split was never
banked, so their A35 status is **UNKNOWN, not "not A35"** — and it does not matter, because
**every measured legitimate target except the `Cube` control is NANITE** (`SM_Modules_Platform`,
`SM_FloorBase`, `SM_SpawnPad_Base`, `SM_Ramp`). Even if one proved A35-shaped it would be
unusable as a control for this instrument. ⇒ **On this project's content, "A35-shaped legitimate
target" and "Nanite" are entangled, and the entanglement is not a coincidence — A35 shapes come
from large authored structural geometry, which is exactly what is Nanite here.**

⇒ **RECOMMENDATION (the ruling's own fallback): `N-2` is satisfied by a plain non-Nanite drawing
target — `StaticMeshActor_73` (`Cylinder`) is the cleanest candidate — and 🚨 THE A35 PROPERTY
GOES INTO THE TAG AS *UNTESTED*, stated, not quietly dropped.**
⚠ **Two limitations of that recommendation, stated:** (1) a second gate-level primitive shares the
level, lighting and trivial geometry of the item-1 control, so it adds **little independence** —
it demonstrates non-over-firing, not robustness; (2) a stronger `N-2` would be a non-Nanite
**MainWorld** actor, and **whether any exists as a selectable drawing target is UNESTABLISHED** —
the plain meshes there (`SM_Elevator`, `SM_Pipes_250`, `SM_GratIng`, `SM_Cube1M`, the foliage) are
asset-side facts, and joining them to placed, selectable actors needs a runtime census leg
(`G122`'s rule), **which was not run this turn.** ⛔ **No asset was modified to manufacture a
control.**

## 191. `RULING 3` / `T-4` — the Nanite scope statement, drafted; and the depth question, ANSWERED

### 191.1 Draft for the `m26` tag and `client-delivery.md` — not softened

> 🚨 **`m26` CANNOT SEE NANITE GEOMETRY.** The measurement is a custom-depth/stencil mask, and on
> **UE 5.1 a Nanite primitive cannot write custom depth at all** — `Nanite::FSceneProxy::
> GetViewRelevance` never sets `bRenderCustomDepth`, and the custom-depth pass has no Nanite path.
> Setting the flag on a Nanite component **succeeds and verifies, and never reaches a pixel.**
>
> **CONSEQUENCE FOR THE CURE:** a Nanite target is **measured as `NOT_MEASURED` and therefore
> ALWAYS ADMITTED, never vetoed.** That is safe — it can never delete a good event — **and it is
> also the cure not working on that target.** `m26` neither detects nor mitigates `H5` on Nanite
> geometry.
>
> 🚨 **THIS IS THE COMMON CASE, NOT A CORNER CASE, AND IT IS MEASURED RATHER THAN PROJECTED.** On
> StackOBot itself the authored structural geometry — walls, floors, platforms, pillars, roofs,
> pipes, fences, crates, doors, ramps — is overwhelmingly Nanite, while foliage and simple planes
> are not. **On a Nanite-heavy title, most of the level is outside this cure's reach.** The two
> `H5` instances it does cover are non-Nanite because of what they are made of, not because `H5`
> favours non-Nanite geometry.
>
> **HOW TO TELL FROM A DELIVERED SESSION:** `run_summary.json` → `mask_nopass_discards` counts
> frames where the pass was never produced. A target whose every armed frame lands there is
> structurally unmeasurable by the mask; the per-event log names the case.
>
> ⚠ Scoped to **UE 5.1**. A later engine that supports Nanite custom depth changes this, and the
> `SM_Ramp2` control is the first place that would show.

### 191.2 THE DEPTH QUESTION — **ANSWERED: YES, SCENE DEPTH IS NANITE-INCLUSIVE ON 5.1**

**Answer only, from source, as instructed — no design, no costing, nothing revived.**

`Nanite::EmitDepthTargets` (`NaniteMaterials.cpp:745`) takes `FRDGTextureRef SceneDepth` and writes
into it: `PassParameters->RenderTargets.DepthStencil = FDepthStencilBinding(SceneDepth,
ERenderTargetLoadAction::ELoad, FExclusiveDepthStencil::DepthWrite_StencilWrite)` at **`:896`**
(`FEmitSceneDepthStencilPS`) and **`:930`** (`FEmitSceneDepthPS`), with a compute path
`FDepthExportCS` (**`:856`**, `RDG_EVENT_NAME("DepthExport")`) and an HTile resummarize on
`SceneDepth` at **`:1024`**.

⇒ **Nanite geometry IS present in scene depth. A path exists where `C-1` has none.** ⛔ **Stated as
the answer to the question asked and nothing more — no hybrid designed, no cost estimated, the
depth work stays parked, and whether `C-2` could serve `H5`'s question remains what PART TWELVE
§6.3 already recorded: `C-2` addresses class (i) partially and class (ii) NOT AT ALL, because its
reference depth comes from the same bounds `H5` calls untrustworthy.**

## 192. State after PART TWENTY-SEVEN

| | |
|---|---|
| `T-1` | ✅ **BRANCH ONE — both `H5` targets NON-NANITE.** Discriminator closes `G134`'s explanation (5/5) |
| Ruling 1 / `T-2` | ✅ **BUILT (`3beb3ba`) AND GATED** — `SM_Ramp2` `MEASURED_ZERO` → `NOT_MEASURED` ×8; L1–L4 unchanged; `F-T2-A`'s literal firing traced to pose and the criterion corrected |
| Ruling 2 / `T-3` | ✅ `SM_Ramp2` repurposed as the known-Nanite control (first pass banked). **No non-Nanite A35-shaped control exists — said so.** `N-2` → a plain non-Nanite target; **A35 → tag as UNTESTED** |
| Ruling 3 / `T-4` | ✅ scope statement drafted, unsoftened; **depth question answered: scene depth IS Nanite-inclusive** |
| artifacts | `annotation.json` **48/48 — `P6` NOT MOVED**; `run_summary` **+3**, the third flagged for veto |
| build | staged **`F93AEF71`** (`DBA2D8EC` archived first); container unchanged; A44 green |
| ⛔ unchanged | `H5` legs **NOT RUN** · slices 2/3 **NOT STARTED** · **NO TAG** · stencil range 200/255 · `feature/stencil-capture` untouched |

**WHAT THIS PART SETTLES: the cure can see the two instances that motivated it — that was the
question that decided whether `m26` has value on its own bench, and the answer is yes. The extent
precondition closes the false accusation the `N-2` control caught. And the same asset scan that
brought the good news brought the honest bad news with it: on this title most of the authored
level is Nanite and outside the cure's reach, and the A35 over-fire risk cannot be tested here at
all, because every A35-shaped legitimate target we have is made of exactly the geometry the
instrument cannot see.**

---

# PART TWENTY-EIGHT — 🎯 **`F-6` COMPLETE, AND THE CURE IDENTIFIES BOTH `H5` TARGETS.** The measurement the milestone was blocked on.

**The last control, then the two legs.** Branches pre-declared as a file **before either ran** —
`CaptureBench/tools/p28_n2_and_h5_predeclared.md`, commit `2c2e60a`. Binary under test: staged
**`F93AEF71`**, container unchanged. ⛔ **NO SLICE 2. NO SLICE 3. NO TAG. `P6` NOT MOVED.
CB_GateLevel untouched (`G99`). Stencil range 200/255. `feature/stencil-capture` READ-ONLY. The
depth work stays parked — the P27 answer was scoping, not a licence.**

---

## 193. Rulings recorded

**RULING 1 — the third field is APPROVED.** `mask_nopass_discards` stays, for the reason given: the
known-Nanite control must be auditable **from a delivered session**, and a limit visible only on our
bench is not a limit a client can act on. ✅ **`run_summary` is `+3`, declared — and the `m26` scope
statement's field list now SAYS `+3`, not `+2`** (§196.1). *"A tag saying +2 while the artifact
carries +3 is exactly the kind of small untruth that costs a reader their trust in the rest."*
**RULING 2 — the Nanite entanglement is HEADLINE, not footnote.** Written into `client-delivery.md`
as its own **`⛔ KNOWN LIMITATION`** section (not a footnote under the good news) and into the scope
statement, in the ruling's own terms, **with no percentage** — the pattern is the finding.
**RULING 3 — the calibration-environment lesson is generalised as `G135`**, including the tension
stated rather than resolved: **the property that makes CB_GateLevel a good instrument is the same
property that makes it unrepresentative.** ⛔ **No change to CB_GateLevel proposed — it is frozen,
`m25`'s certifications are expressed in it, and `G99` guards it. The correct response is knowing
what it cannot show.**

## 194. `LEG A` — the replacement `N-2` control. **`F-6` ITEM 2 IS SATISFIED.**

`StaticMeshActor_73` (`Cylinder`, `/Engine/BasicShapes/`, non-Nanite), CB_GateLevel, `blinking`.
**Accepted on attempt 1.** `B1` **NOT APPLICABLE, declared in advance** (`G117`) — and the harness
printed exactly that, then reported the pose read-only.

| prediction | observed |
|---|---|
| `MEASURED_NONZERO` every full event | ✅ **8 of 8, `notMeasured=0`** |
| `framesContributed=4`, all other buckets 0 | ✅ **`contributed=4 discarded=0 residual=0 unconfirmed=0 noPass=0 probeArms=0`** on every event |
| `collisions=0` | ✅ **0**, `tagFailed=0` |
| non-zero and **plausible for a cylinder of its on-screen size** | ✅ **48,591–48,597 px = 5.27 % of frame**, against the `Cube` control's 7.25 % — **smaller, as a cylinder should be**, and its own claimed `coverage_pct` is **6.87 %**, so it **draws ~77 % of what it claims** |
| — | ✅ **29/29 armed frames view-sized; ZERO dummies**; `mask_nopass_discards=0`; `speed_ratio` 1.0000 |

⚠ **BOTH WEAKNESSES CARRIED, NOT DISCOVERED LATER:** it **shares CB_GateLevel with the item-1
control**, so it demonstrates **non-over-firing, not robustness**; and whether a stronger MainWorld
candidate exists is **UNESTABLISHED** (needs a `G122` census leg — not run, not now). 🚨 **The A35
property remains UNTESTED and no control for it exists anywhere (P27 §190).**

### 194.1 ✅ **`F-6` IS NOW COMPLETE — ALL FIVE ITEMS**

| item | satisfied by |
|---|---|
| **1** `StaticMeshActor_49` NON-ZERO, `collisions=0` | P27 `P27_EXT_CTRL49` — 8/8, 7.25 % |
| **2** a legitimate drawing control NOT over-fired | ✅ **THIS LEG** — 8/8 non-zero, 5.27 %, all buckets clean |
| **3** arm counts match prediction | all legs: `arms=4 resolved=4`, capped final events as predicted |
| **4** `pctOfFrame` plausible | 7.25 % cube · 5.27 % cylinder · both under their claims |
| **5** the 255 detector proven live BOTH ways | P27 `P27_EXT_PROBE49` — fires on demand; silent everywhere else |

⛔ **`SM_Ramp2` is retired from `N-2` and now serves as the KNOWN-NANITE control** (must read
`NOT_MEASURED` every time; first pass banked at P27 §189.1).

## 195. 🎯 `LEG B` and `LEG C` — **THE `H5` LEGS.** Branches restated verbatim, then the results.

> **Z1** `MEASURED_ZERO` with ALL buckets clean ⇒ 🎯 **THE AIMED-AT RESULT: the cure identifies an
> `H5`-shaped target.**
> **Z2** small non-zero, buckets clean ⇒ ✅ **also the cure working**: drawn ≪ claimed is exactly
> `H5`'s claim. Report the RATIO; ⛔ **do NOT convert it into a veto threshold.**
> **Z3** count comparable to the controls ⇒ a finding **about the target**, not a failure of the cure.
> **N1** `NOT_MEASURED` via `framesNoPass` ⇒ ⛔ **the instrument could not see it — NOT evidence of `H5`.**
> **N2** `NOT_MEASURED` via `framesUnconfirmed`/`framesResidual`/`tagFailed` ⇒ ⛔ instrument fault.
> **N3** `NOT_MEASURED` via `skippedHidden` only ⇒ ⛔ no qualifying tick ⇒ ADMIT.
> **X** 🚨 **ANY zero or near-zero arriving with ANY bucket non-zero ⇒ NOT YET INTERPRETABLE.**

### 195.1 `LEG B` — `InstancedFoliageActor_0_0_0` (`SM_Bush`, non-Nanite). **BRANCH `Z2`.**

**Accepted attempt 1. `B1` NOT APPLICABLE (`G117`), declared.**

| | |
|---|---|
| state | **`MEASURED_NONZERO` on 8 of 8**, `notMeasured=0` |
| **the label's CLAIM** | 🚨 **`coverage_pct = 100` on every event, `bbox_px = (0,0,1280,720)` — THE ENTIRE FRAME** |
| **the mask's MEASUREMENT** | **5,689 px (0.62 %) · 13,342 · 12,514 · 12,564 · 12,682 · 12,754 · 12,696 · 12,646 — i.e. 0.62–1.45 % of frame** |
| **claimed vs drawn** | 🎯 **the target draws ≈ 1.4 % of what its label claims — an over-claim of roughly 70×** |
| buckets | ✅ **`contributed=4 discarded=0 residual=0 unconfirmed=0 noPass=0 probeArms=0 collisions=0 tagFailed=0`** on every event |
| extents | ✅ **29/29 view-sized, ZERO dummies** · `mask_nopass_discards=0` |

⇒ **`Z2`: the cure working.** The pixel reality it reports agrees with the independent banked
measurement (whole-frame change **0.0069**, marker-off **0.0059**, living in **4 of 64** grid cells)
— but now as an **area the cure itself can act on**, rather than a differencer's contrast score.

### 195.2 🎯 `LEG C` — `BP_SplineSpawn_C` (`SM_GenericPlane`, non-Nanite). **BRANCH `Z1`.**

**Accepted attempt 1. `B1` NOT APPLICABLE (`G117`), declared.**

| | |
|---|---|
| state | 🎯 **`MEASURED_ZERO` on ALL 8 EVENTS**, `maxCount = 0`, `pctOfFrame = 0.0000` |
| **the label's CLAIM** | **`coverage_pct` 3.86 % (first two events) then 22.89 %** |
| **the mask's MEASUREMENT** | **ZERO surviving pixels** |
| buckets — **the part that makes the zero interpretable** | ✅ **`framesContributed = arms` (4/4, and 1/1 on the capped event) · `framesDiscarded=0` · `framesResidual=0` · `framesUnconfirmed=0` · `framesNoPass=0` · `probeArms=0` · `collisions=0` · `tagFailed=0`** |
| extents | ✅ **29/29 armed frames view-sized, ZERO dummies** · `mask_nopass_discards=0` |

🚨 **BRANCH `X` DOES NOT FIRE. Every bucket is clean, so this zero is the interpretable kind:** the
target was **tagged** (`tagFailed=0`), the tag was **verified still held** (`collisions=0`), the
custom-depth pass **was produced on every armed frame** (29/29 view-sized), and the target was
**confirmed visible at both brackets** of each render (`framesResidual=0`, `framesUnconfirmed=0`) —
**and the mask still found not one pixel carrying its tag.** That is the difference between this
zero and `P26` event 1's, and it is why the bucket reporting was made mandatory in advance.

⚠ **ONE NUANCE RECORDED RATHER THAN SMOOTHED:** the banked hide-measurement for this target showed
a **small but non-zero** peak in-bbox change (**0.0175**, against a control hide's 0.5515). A mask
of exactly zero alongside a non-zero luma change is **consistent with, and a reminder of, plan risk
7 / A35: the mask measures DRAWN SILHOUETTE, not VISUAL EFFECT** — indirect contribution
(shadow, GI, reflection) is outside the tagged silhouette by construction. ⛔ **Not a contradiction
and not re-opened here; it is the already-recorded limit showing up exactly where the plan said it
would.**

### 195.3 The four targets side by side — reported, and deliberately NOT thresholded

| target | non-Nanite? | claimed `coverage_pct` | mask `pctOfFrame` | drawn ÷ claimed |
|---|---|---|---|---|
| `StaticMeshActor_49` (item 1) | ✅ | ~7.80 (rect) | **7.25** | **~0.93** |
| `StaticMeshActor_73` (`N-2`) | ✅ | 6.87 | **5.27** | **~0.77** |
| `InstancedFoliageActor_0_0_0` | ✅ | **100** | **~1.38** | 🚨 **~0.014** |
| `BP_SplineSpawn_C` | ✅ | **22.89** | **0.00** | 🚨 **0.000** |

⛔ **NO THRESHOLD IS PROPOSED OR IMPLIED.** Slice 3's veto rule is a separate decision on a separate
turn; four targets on one title is a distribution sketch, not a calibration. **What this table
establishes is that the cure's measurement SEPARATES the two classes on the instances it was built
from — nothing about where a line should sit.**

### 195.4 What these legs do NOT establish — declared in advance, restated after

⛔ **NO INCIDENCE CLAIM** — two instances on one title says nothing about how often `H5` occurs in
client data. ⛔ **`H5` class (i) remains ENUMERATED, NOT OBSERVED.** ⛔ **NOT a veto test** — slice 3
does not exist; nothing was removed from any `annotation.json`; **these legs measure, they do not
act.** ⛔ **NOT a Nanite result** — both targets are non-Nanite, so they say nothing about `G134`
either way.

## 196. Scope statement and artifacts

### 196.1 The `m26` scope statement's field list — corrected to `+3`
*(⚠ superseded in part by PART THIRTY §205, which adds the client-facing `mask.provided` sentence
and the `framesNoPass` definition to the same statement. The `+3` correction below stands.)*

> `run_summary.json` gains **THREE** fields at `m26`: **`mask_probe_arms`**,
> **`mask_residual_discards`**, **`mask_nopass_discards`**. `annotation.json`'s field SET is
> **UNCHANGED** — `P6` is not moved. *(Measured again this turn on all three legs: annotation
> **48/48, 0 added 0 removed**; `run_summary` **+3**, 0 removed.)*

Plus **Ruling 2's paragraph verbatim** into the scope statement and `client-delivery.md`'s new
`⛔ KNOWN LIMITATION` section, and `mask_probe_arms` must read `0` on any delivered session
(checklisted at `m26`).

### 196.2 `G135` — the new gotcha (Ruling 3)

**A calibration environment built from a restricted asset set cannot exhibit defect classes that
depend on asset features outside that set, and the resulting blindness presents as a CLEAN PASS,
not as a gap.** CB_GateLevel is `/Engine/BasicShapes/` throughout — which is exactly why it is a
stable instrument **and** exactly why it could not have surfaced `G134`. **The tension is stated,
not resolved.**

## 197. State after PART TWENTY-EIGHT

| | |
|---|---|
| **`F-6`** | ✅ **COMPLETE — ALL FIVE ITEMS PASS** (item 2 by the replacement control) |
| **`H5` legs** | 🎯 **RUN. `BP_SplineSpawn_C` → `MEASURED_ZERO` ×8 with every bucket clean (`Z1`); `InstancedFoliageActor_0_0_0` → 1.4 % drawn against a 100 % claim (`Z2`). The cure identifies both instances that motivated it.** |
| rulings | 1 (third field, +3 declared) · 2 (Nanite entanglement in the headline) · 3 (`G135`) — all recorded |
| `P6` | measured unchanged on all three legs — **48/48** |
| build | staged **`F93AEF71`**, container unchanged, A44 green |
| banked | `P28_N2_CYL73`, `P28_H5_FOLIAGE`, `P28_H5_SPLINE` (+ attempts; all three accepted on attempt 1) |
| ⛔ unchanged | slices 2/3 **NOT STARTED** · **NO TAG** · `P6` does not move · CB_GateLevel untouched · `feature/stencil-capture` READ-ONLY · depth work parked |

**WHAT THIS PART SETTLES: the instrument is certified by its own gate on all five items, and then
it did the thing it was built to do — it looked at a label claiming the entire frame and measured
1.4 % of a frame, and at a label claiming 22.89 % and measured nothing at all, both with every
integrity bucket clean. `H5` class (ii) is no longer only a diagnosis; it is a quantity the cure
can act on. What remains is the owner's: whether to act on it.**

---

# PART TWENTY-NINE — **SLICE 2 SHIPS.** `mask.provided` carries the tri-state, and both zeros stay distinguishable.

**Slice 2 cleared with scope frozen. Predictions pre-declared as a file BEFORE implementation and
before any leg — `CaptureBench/tools/p29_slice2_predeclared.md`, commit `e36beb3`.**
⛔ **SLICE 3 NOT STARTED — its veto rule is NOT designed, NOT proposed, and NOT assumed by anything
here. NO TAG. `P6` NOT MOVED. Stencil range 200/255. CB_GateLevel untouched (`G99`).
`feature/stencil-capture` READ-ONLY.**

---

## 198. 📌 RECORDED FOR A COLD READER: the clean result at the end was not a clean path

**Eleven parts ago this instrument returned a constant 255.** It now separates two known-good
targets from two known-bad ones with every integrity bucket clean and its guards demonstrated live
on demand. **The path from there to here ran through two repairs that source refuted before they
were built (the stencil range, the tag/arm separation), one fault in our own collection code (the
event-scoped discard), one stale read (the arm gate's one-tick-old hidden state), one structurally
blind detector (the single-pixel 255 signal), and a calibration level that could not have shown the
blindness (BasicShapes throughout). EVERY ONE WAS CAUGHT BEFORE IT SHIPPED.** ⚠ **A reader
arriving at PART TWENTY-EIGHT's numbers should not mistake them for a straight line.**

## 199. Slice 2 — what changed, and the code path that guarantees the safety property

**`mask.provided` stops being a hardcoded `false` and becomes the measurement tri-state's bool.**

| internal state | `mask.provided` |
|---|---|
| `NotMeasured` | **`false`** — never measured ⇒ MUST ADMIT |
| `MeasuredZero` | **`true`** — measured |
| `MeasuredNonZero` | **`true`** — measured |

🚨 **THE GUARANTEE, NAMED AS THE RULING ASKED:** `provided` is produced by **one function,
`MaskStateProvidesMeasurement`, which switches on `State` ALONE.** Its only `true` branches are
`MeasuredZero` and `MeasuredNonZero`; everything else — including the "no mask record for this
event" path — falls to `false`. **`MaxCount` is never consulted and never emitted**, so there is
no code path on which a magnitude can turn `MEASURED_ZERO` into `NOT_MEASURED` or the reverse.
**The two zeros cannot collapse: one is `true`, the other is `false`, structurally.**

**Scope held:** no sub-fields under `mask` (verified per leg: sub-keys are exactly `['provided']`)
· `depth{provided:false}` untouched · field SET unchanged · **nothing vetoed.**
⚠ **One ordering change, stated because it is real:** the final mask collect moved to **before**
`WriteSessionAnnotationFile()` — it previously ran after it and fed only `run_summary`. Nothing
else in `FinishRun` moved.
⚠ **And two strings were corrected: the `IAI.Capture.Mask` help text and the `StartRun` banner both
claimed "log output only … mask{provided} stays false". They no longer do.** A binary must not
assert something it has stopped doing.

## 200. THE KNOWN-ANSWER SET — all five rows correct, in both directions

| # | target | measured state | `mask.provided` | predicted | |
|---|---|---|---|---|---|
| 1 | `StaticMeshActor_49` (`Cube`) | `MEASURED_NONZERO` ×8 | **true** ×8 | true | ✅ |
| 2 | `StaticMeshActor_73` (`Cylinder`) | `MEASURED_NONZERO` ×4, **`NOT_MEASURED` ×4** | **true ×4, false ×4** | both | ✅ 🎯 **both directions in ONE leg** |
| 3 | `InstancedFoliageActor_0_0_0` | `MEASURED_NONZERO` ×8 | **true** ×8 | true | ✅ |
| 4 | 🚨 `BP_SplineSpawn_C` | **`MEASURED_ZERO` ×8** | **true** ×8 | true | ✅ **`F-2` does NOT fire** |
| 5 | 🚨 **`SM_Ramp2`** (known-Nanite control) | **`NOT_MEASURED` ×8** | **false** ×8 | false | ✅ **`F-1` does NOT fire** |

**Mapping mismatches: 0 on every leg.** Sub-keys under `mask`: exactly `['provided']` everywhere.
`depth.provided`: `false` everywhere.

🚨 **ROWS 4 AND 5 ARE THE MILESTONE'S SAFETY PROPERTY, TESTED IN BOTH DIRECTIONS ON BANKED KNOWN
ANSWERS.** Row 5 reading `true` would mean a slice-3 veto **deletes a target that draws real
geometry and the instrument merely cannot see**. Row 4 reading `false` would mean `MEASURED_ZERO`
had collapsed into `NOT_MEASURED` and **the cure would never fire at all**. Neither happened.

### 200.1 🆕 An unplanned corroboration from row 2 — and it re-derives `G134`'s mechanism from a different cause

The `Cylinder` leg produced **both** values in one run, and the reason is visible in the artifact:

| event | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|---|
| `coverage_ratio` | 0.0685 | 0.0680 | 0.0678 | 0.0379 | **0** | **0** | **0** | **0** |
| `framesNoPass` | 0 | 0 | 0 | **3** | **4** | **4** | **4** | **4** |
| `mask.provided` | true | true | true | true | **false** | **false** | **false** | **false** |

**The camera drifted until the cylinder left the frustum, and the moment it did, the custom-depth
pass stopped being produced.** ⇒ **`framesNoPass` is not a Nanite-specific signal — it is
"the target is not in the view's relevant set", and FRUSTUM CULLING reaches it exactly as Nanite
does** (`SceneVisibility.cpp:2470`, relevance runs over the VISIBLE set). ✅ **And the admit bias
handles it correctly: a target that is off-screen is NOT MEASURED, reports `provided:false`, and
must be ADMITTED.** *(Observed, not engineered; recorded because it is a second, independent route
to the same mechanism.)*

## 201. `G-9` — delivery orthogonality. ⚠ **INVARIANT CORE PASSES POSITIVELY; the path-level subset test is INCONCLUSIVE and I am not calling it a pass.**

### 201.1 🚨 First: `A64` justified itself, and it stopped me writing a false finding

**The FIRST delivery leg (`P29_S2_CTRL49_DEL`) came back `mask.provided:false` on every event, with
`framesNoPass=4` on all eight** — the mask armed and passed normally (29 arms / 29 passes), but the
custom-depth pass was never produced. **Read alone, that says "delivery mode breaks the mask" — a
G-9 failure and a serious finding.**

**It is not what happened.** That leg's `coverage_ratio` was **0.0513 and 0** against the OFF legs'
**0.0780** — a bifurcated pose, with the target barely or never projecting. **Two independent
things refute the delivery explanation:**
1. **The `Cylinder` leg reproduces the identical signature with delivery OFF** (§200.1) — pose
   alone is sufficient to produce it.
2. **A POSE-MATCHED delivery leg (`P29_S2_CTRL49_DEL2`, `coverage_ratio` 0.0729–0.0780 against the
   OFF leg's 0.0780) reads `mask.provided:true` on every event.** A positive test, not an absence.

⇒ **Delivery mode does NOT suppress the mask.** 🚨 **And this is precisely why the ruling said
"B1 per leg is not sufficient": a delivery leg cannot be pose-gated from its artifact at all (no
`labels.jsonl`), the harness accepted it on attempt 1, and without pose-matching I would have
reported a delivery-orthogonality break caused by a camera pointed elsewhere.**

### 201.2 The two halves of the gate, reported separately

**✅ INVARIANT CORE — ASSERTED POSITIVELY, not inferred from absence in a diff list:**

| asserted | result |
|---|---|
| `mask.provided`, all 8 events, OFF vs ON | ✅ **IDENTICAL** — `true` in both modes, 0 mismatches |
| `depth.provided` | ✅ identical, `false` |
| `mask_probe_arms` · `mask_residual_discards` · `mask_nopass_discards` | ✅ **identical (0/0/0 in both modes)** — the three `m26` fields are delivery-invariant |
| event COUNT | ✅ identical (8 vs 8) |
| full key SET, both artifacts | ✅ **identical — 0 only-OFF, 0 only-ON** |

**⚠ SUBSET TEST — path level: `EXTRAS = 6`. Kind level: `EXTRAS = 0`.**
The six are `anomalies[0]/camera/rotation[0]`, `[0]/camera/rotation[1]`, `[0]/coverage_pct`, and
`camera/rotation[0]` on events 5–7 — **every one a camera-pose-derived field**, and every one of a
KIND the control pair already established as run-unique (`camera/rotation[*]`, `coverage_pct`,
`coverage_ratio`). At kind level the test pair's difference set is **exactly** the control pair's
kinds **plus `delivery_mode`**.

⛔ **I AM NOT CALLING THAT A PASS, AND THE REASON IS A DEFECT IN MY CONTROL PAIR, NOT IN THE
BUILD.** The two OFF legs happened to settle at poses differing in **yaw but not pitch**, so
`camera/rotation[0]` never entered the run-unique set — **the control pair UNDER-SAMPLED the pose
variation it exists to characterise.** And the OFF/ON pair is pose-matched only *approximately*,
because a delivery leg's pose can only be matched by re-running against the `coverage_ratio`
indicator. ⇒ **A definitive path-level `G-9` needs either an exactly-pose-matched OFF/ON pair or a
control pair that spans pitch. Neither was achieved this turn, and the gate is recorded as
INCONCLUSIVE at path level rather than quietly passed at kind level.** ✅ **What IS established is
the part slice 2 actually changes: the invariant core, positively, on every element.**

## 202. The remaining gates

| gate | result |
|---|---|
| **G-2** field SET unchanged, **BOTH delivery modes** | ✅ **48/48, 0 added 0 removed, on all seven legs including the delivery-ON leg** |
| **P6** | ✅ **annotation 48/48 unchanged; `run_summary` +3 and nothing more**, every leg |
| **MAP** | ✅ **all five known-answer rows correct; 0 mismatches** |
| **INERT** | ✅ **mask OFF ⇒ `provided:false` on all 8 events, 0 `M26S2 MAP` lines, all three counters 0** — the reporting change is inert when the switch is off |
| **F-1 … F-7** | ✅ **none fired** |

## 203. State after PART TWENTY-NINE

| | |
|---|---|
| **slice 2** | ✅ **SHIPPED (`ece343f`) and gated.** `mask.provided` carries the tri-state; both zeros stay distinguishable, tested both ways on banked known answers |
| **slice 3** | ⛔ **NOT STARTED.** Its veto rule is not designed, not proposed, and not assumed — **and no threshold exists anywhere in the code or the docs** |
| `G-9` | ⚠ **invariant core PASSES positively; path-level subset INCONCLUSIVE (my control pair under-sampled pitch)** — owner's to weigh |
| build | staged **`047FA489`** (`F93AEF71` archived first); container unchanged; A44 green |
| banked | `P29_S2_{CTRL49_A,CTRL49_B,CTRL49_DEL,CTRL49_DEL2,CYL73,FOLIAGE,SPLINE,RAMP,INERT}` + attempts |
| ⛔ unchanged | **NO TAG** · `P6` does not move · CB_GateLevel untouched · stencil range 200/255 · `feature/stencil-capture` READ-ONLY |

**WHAT THIS PART SETTLES: the measurement now reaches the artifact, and the one property everything
downstream depends on — that "never measured" and "measured zero" are different states — is
enforced by a single function that cannot see a magnitude, and was tested in both directions on
targets whose correct answers were already banked. What it deliberately leaves open: whether a
veto should ever act on it.**

---

# PART THIRTY — **`G-9` CLOSES: `EXTRAS = 0`.** Route (a), and the run-unique set got NARROWER instead of wider.

**Ruling 1: `G-9` closes BEFORE slice 3, because slice 3 deletes events from `annotation.json` and
in delivery mode that is the ONLY label artifact the client receives.** ⛔ **SLICE 3 NOT STARTED.
NO TAG. `P6` NOT MOVED. Stencil range 200/255. CB_GateLevel untouched (`G99`).
`feature/stencil-capture` READ-ONLY.**

---

## 204. `G-9` — route **(a)**, chosen, with route (b)'s hazard restated before running

**CHOSEN: (a), an exactly pose-matched OFF/ON pair. WHY: it makes the difference VANISH rather
than EXCUSED.** If the two legs settle at the same pose, the pose-derived fields are identical and
there is nothing to subtract — **the run-unique set is not widened by a single member, so there is
no laundering surface at all.**

**(b)'s constraint, restated because the ruling asked for it to be stated before running — and it
is the reason (b) was NOT taken:** widening a run-unique set is how a real difference gets
laundered into an expected one. Under (b) the pitch variation would have to arise from the **same
mechanism that produces it naturally** (`A47` settle), never anything introduced to make the axis
appear, and the pair would have to be **two OFF legs establishing the baseline, never the test pair
itself**. ⚠ **Even honouring all of that, (b) means re-running legs UNTIL THE BASELINE WIDENS
ENOUGH TO EXCUSE THE DIFFERENCE BEING CLEARED — the laundering shape, even when every individual
step is legitimate.** (a) has no such shape.

### 204.1 🎯 THE RESULT — and the gate got HARDER, not easier

**All three legs landed at the same settled pose — camera rotation `0/0/0`, `coverage_ratio`
`0.077977`, `CALIB_BBOX` exactly — on the first attempt for each.** *(The delivery leg cannot be
pose-gated from its artifact at all, so the match was verified from `camera.rotation` and
`coverage_ratio`, which are camera-derived and outcome-independent.)*

| | P29 (inconclusive) | **P30 (this part)** |
|---|---|---|
| control pair | poses DIFFERED (yaw only) | **pose-IDENTICAL** |
| **run-unique set size** | **26** — incl. `camera/rotation[*]`, `coverage_pct`, `coverage_ratio` | 🎯 **4** — `/session_id`, `/speed_ratio`, `/sustained_wall_fps`, `/video/path` |
| test-pair difference set | 33 | **5** — the four above **+ `/delivery_mode`** |
| **path-level `EXTRAS`** | **6** (all pose-derived) | ✅ **0** |

🚨 **THE RUN-UNIQUE SET SHRANK FROM 26 TO 4, AND THAT IS THE POINT: NO POSE FIELD IS IN IT AT ALL,
so no pose field could be excused even if one had differed.** The subset test is now **maximally
strict** — every member was exercised by the control pair by construction, and the only field the
test pair adds is the variable under test. ✅ **PASS on the ruling's terms: `EXTRAS = 0` against a
run-unique set whose every member the control pair exercised.**

### 204.2 The invariant core — carried forward, not re-run, and it holds on this pair too

The ruling said the P29 invariant core stands and needs no re-running. **It was re-asserted anyway
on this pair, because it costs one command:** `mask.provided` **identical across modes on all 8
events** (`true` both sides, 0 mismatches) · `depth.provided` identical · **`mask_probe_arms`,
`mask_residual_discards`, `mask_nopass_discards` all identical (0/0/0)** · event **COUNT**
identical (8 vs 8) · full **key SET** identical, 0 only-OFF and 0 only-ON.

⇒ **`G-9` IS CLOSED. `m25`'s certified delivery orthogonality survives slice 2.**
📦 Gate run on staged **`F4EBEAD7`** (`047FA489` archived first), container unchanged, A44 green.

## 205. `RULING 2` — `framesNoPass` gets its definition fixed WHERE A READER HITS IT

**The definition, now written at every point of contact:**

> **`framesNoPass` counts frames where the custom-depth pass did not produce for this target.
> Causes include Nanite geometry (`G134`), frustum culling, and any other route by which the target
> is absent from the view's relevant set. It is NOT a Nanite counter. In all cases the frame is
> discarded and the event tends toward `NOT_MEASURED`, which ADMITS.**

| where | was | now |
|---|---|---|
| the `M26S1 NO-PASS` log line | *"e.g. NANITE geometry on UE 5.1"* — named Nanite as if it were the cause | **"THIS IS NOT A NANITE-SPECIFIC COUNTER"** + Nanite · frustum culling · any absence from the relevant set |
| the `M26S1 NOT_MEASURED` warning | *"NANITE on UE 5.1 is the known case"* | **"framesNoPass is NOT a Nanite counter"** + the three causes + how to distinguish them |
| **`G134`** (gotchas) | implied the counter was Nanite-specific | 🚨 **corrected**, with the `Cylinder` measurement quoted and the consequence named: *"had that gone unrecorded, `G134` would have inherited a wrong denominator"* |
| **`client-delivery.md`** | *"A target whose every armed frame lands there is structurally unmeasurable"* | **"IT IS NOT A NANITE COUNTER"** + the causes + *"a high count means the measurement could not see the target on those frames, not that the target is Nanite and not that it drew nothing"* |

*(Shipped in `7ea8ce9`; A44-verified in the staged binary. The wording change was made BEFORE the
`G-9` legs so the gate certifies the binary that carries it.)*

## 206. `RULING 3` — the client-facing sentence, kept verbatim, into the tag scope statement

Added to the `m26` scope statement (§117) alongside `client-delivery.md`:

> **`mask.provided` `false` NEVER means "the target drew nothing" — it means no measurement exists,
> and such an event carries exactly as much evidence as it did before `m26`: none from this
> measurement.**

## 207. REPORTING ONLY — the two raw quantities behind each slice-1 target

**Measured pixel count and claimed coverage as an AREA IN PIXELS, same units, viewport 921,600 px.**
⛔ **No ratio, no threshold, no comparison operator. Tool: `CaptureBench/tools/p30_raw_quantities.py`,
which computes none of those by construction.**

| target | state | **measured px** | **claimed px** |
|---|---|---|---|
| **`StaticMeshActor_49`** (`Cube`, control) | `MEASURED_NONZERO` ×8 | **66,843 – 66,878** | **71,864** (all 8 events) |
| **`StaticMeshActor_73`** (`Cylinder`, `N-2`) | `MEASURED_NONZERO` ×8 | **48,590 – 48,597** | **63,296** (all 8 events) |
| **`InstancedFoliageActor_0_0_0`** (`H5`) | `MEASURED_NONZERO` ×8 | **5,689** · then **12,514 – 13,342** | **921,600** (all 8 events — the entire viewport) |
| **`BP_SplineSpawn_C`** (`H5`) | `MEASURED_ZERO` ×8 | **0** (all 8 events) | **35,535** ×2, then **210,921 – 210,942** ×6 |

*(Per-event detail is in the tool's output; the ranges above span the eight events of each leg.)*

## 208. State after PART THIRTY

| | |
|---|---|
| **`G-9`** | ✅ **CLOSED — `EXTRAS = 0`** against a 4-member run-unique set, on a pose-matched pair; invariant core identical |
| slice 2 | ✅ shipped and fully gated |
| `framesNoPass` | ✅ **definition fixed at four points of contact** (two in the binary, two in docs) |
| the client sentence | ✅ in `client-delivery.md` **and** the tag scope statement |
| raw quantities | ✅ reported, **numbers only** |
| **slice 3** | ⛔ **NOT STARTED. Its rule is the owner's decision. NO THRESHOLD EXISTS in the code or the docs, and nothing has begun to assert one.** |
| build | staged **`F4EBEAD7`**; container unchanged; A44 green |
| banked | `P30_G9_OFF_A`, `P30_G9_OFF_B`, `P30_G9_ON_1` (+ attempts) |
| ⛔ unchanged | **NO TAG** · `P6` does not move · CB_GateLevel untouched · `feature/stencil-capture` READ-ONLY |

**WHAT THIS PART SETTLES: the delivery gate that was inconclusive is now closed by making the
confound absent rather than expected — the run-unique set went from 26 members to 4, and the
difference between a delivery-ON and a delivery-OFF session is exactly one field plus the four that
differ between any two runs. And the counter that was one report away from being read as a Nanite
gauge forever now carries its own definition everywhere a reader meets it.**

---

# PART THIRTY-ONE — **SLICE 3 SHIPS. THE VETO IS ZERO-ONLY, AND IT FIRES.** `m26` is functionally complete.

**Predictions pre-declared BEFORE implementation and before any leg —
`CaptureBench/tools/p31_slice3_predeclared.md`, commit `043b110`.**
⛔ **NO TAG — the owner smoke gate comes first. `P6` NOT MOVED. Stencil range 200/255.
CB_GateLevel untouched (`G99`). `feature/stencil-capture` READ-ONLY.**

---

## 209. `THE VETO RULING` — recorded VERBATIM, because a cold reader must see why the obvious rule was refused

> **VETO IF AND ONLY IF: `State == MEASURED_ZERO`.** That is: `mask.provided == true` AND the
> measured pixel count == 0.
>
> **NOT VETOED, under any circumstance:** `NOT_MEASURED` (`provided:false`) — never measured, MUST
> ADMIT, unchanged and non-negotiable · `MEASURED_NONZERO` — any count ≥ 1 pixel, regardless of how
> small a fraction of the claimed area it is · `manifested == false` events — `A-1`'s precedence
> rule stands, veto only `manifested == true`.
>
> **NO RATIO. NO THRESHOLD. NO COMPARISON AGAINST claimed area.** Do not implement one, do not
> leave a constant that could become one, and do not name a variable as though a ratio is coming.

**THE OWNER'S REASONING, VERBATIM:**

> **"The four measured targets separate cleanly (good 0.77–0.93 drawn/claimed; bad 0.014 and
> 0.000), and a ratio threshold would catch both known `H5` instances. IT IS REFUSED ANYWAY. Both
> GOOD targets are convex primitives viewed head-on — `/Engine/BasicShapes` Cube and Cylinder —
> which is why they score so high. A legitimate target with a COMPLEX SILHOUETTE (fence, railing,
> ladder, grate, sparse foliage) can draw a small fraction of its bounding rect while being fully
> visible and fully valid. NO SUCH TARGET EXISTS IN OUR MEASURED SET. Calibrating a threshold on
> four points, all simple convex shapes, is `G135`'s exact failure: a calibration set that cannot
> exhibit the case that would break the rule, with the blindness presenting as a clean pass.
> A count of ZERO needs no calibration. A target contributing not one pixel cannot be the visible
> anomaly its label claims, whatever its silhouette."**

## 210. The implementation (`65deadc`)

**The veto pass runs in `FinishRun`, after the final mask collect and BEFORE
`WriteSessionAnnotationFile`, on the in-memory accumulator.** It tests **the enum state only** —
`MaskStateVetoes(State) { return State == EAnomalyMaskState::MeasuredZero; }`. ⛔ **`MaxCount` is
never read, no percentage is computed, no constant is introduced that could become a threshold, and
no identifier is named as though a ratio is coming.**
**`AccumEventManifested` shares ONE definition of "manifested" with the writer** rather than
duplicating `m23`'s logic, and it is evaluated **FIRST** — so `vetoed_events` and
`non_manifested_events` are **DISJOINT BY CONSTRUCTION**, in the agreed words:

| counter | means |
|---|---|
| `non_manifested_events` | **"the hide never showed in pixels"** |
| `vetoed_events` | **"the target contributed no pixels to hide"** |

`run_summary` gains **`vetoed_events`** — **`+4` since `m25`**, and the tag's field list says `+4`.
The cvar help and the `StartRun` banner now describe the veto truthfully, including that it is
zero-only and that frames are not un-written.

## 211. GATE RESULTS — every prediction met, no failure branch fired

| leg | target | measured | vetoed | events kept | `vetoed_events` | gate |
|---|---|---|---|---|---|---|
| **A** `P31_S3_SPLINE` | `BP_SplineSpawn_C` | `MEASURED_ZERO` ×8 | 🎯 **8** | **0** | **8** | ✅ **`G-4` IT FIRES** |
| **B** `P31_S3_CTRL49` | `StaticMeshActor_49` | `MEASURED_NONZERO` ×8 | 0 | 8 | 0 | ✅ `G-5`, `G-10` |
| **C** `P31_S3_CYL73` | `StaticMeshActor_73` | `MEASURED_NONZERO` ×8 | 0 | 8 | 0 | ✅ `G-5` |
| **D** 🚨 `P31_S3_FOLIAGE` | `InstancedFoliageActor_0_0_0` | `MEASURED_NONZERO` ×8 | **0** | **8** | **0** | ✅ 🚨 **THE RULE'S OWN GUARD** |
| **E** `P31_S3_RAMP` | `SM_Ramp2` (known-Nanite) | `NOT_MEASURED` ×8 | **0** | 8 | 0 | ✅ **`G-7` admits when blind** |
| **F–H** `P31_S3_G9_{OFF_A,OFF_B,ON}` | `StaticMeshActor_49` | `MEASURED_NONZERO` | 0 | 8 | 0 | ✅ **`G-9`** |
| **I** `P31_S3_INERT` | mask OFF | — | **0**, zero `VETO` lines | 8 | 0 | ✅ **`F-7` inert** |

🚨 **LEG D IS THE RESULT THAT MATTERS MOST AFTER LEG A.** The foliage draws **~1.4 % of what it
claims** and **is NOT vetoed** — because the rule is zero-only. **Had any veto appeared there, a
ratio would have crept in and the gate would have failed.** It did not.
🚨 **LEG E IS THE DATA-DESTROYING DIRECTION, AND IT IS CLOSED.** Eight `NOT_MEASURED` events on a
target that draws real geometry and the instrument merely cannot see — **all eight KEPT.**

**`G-8` (the blind case is loud) — in the ARTIFACT, not just the log:** leg E ships
`mask.provided:false` on every event **and** `mask_nopass_discards=30` in `run_summary`.
**`G-11` — two numbers, every leg:** `countedEventsBefore` / `countedEventsAfter` logged, with
`before − vetoed == after` verified and `vetoed + nonManifested ≤ before` (no double-count).
**`P6`** — `annotation.json` **48/48, 0 added 0 removed** on every leg; `run_summary` **+4 exactly**.

### 211.1 `G-9` re-run at slice 3 — because the veto CHANGES `annotation.json` content

All three legs settled at the same pose (`0/0/0`, `coverage_ratio 0.077977`), route (a) again.
**Path-level `EXTRAS = 0`** against a **4-member** run-unique set. **Invariant core asserted
positively and extended for slice 3:** `mask.provided` identical ×8 · `depth.provided` identical ·
all three mask counters identical · **`vetoed_events` identical (0/0)** · **the EVENT SET itself
identical across modes (8 vs 8, same types and start frames)** · event count identical · full key
set identical.

### 211.2 ⚠ The consequence that was pre-declared rather than discovered

**Leg A vetoes all 8 of its events, so its `annotation.json` contains ZERO anomalies** — which is
**the veto working** and simultaneously **below `G-11`'s 3-event floor**. Reconciled by the leg's
ROLE, fixed before it ran: **leg A is a DEMONSTRATION leg (`G-4`), not a certifying one; it is not
graded by `A54` and certifies nothing about alignment.** ✅ **`F-8` checked explicitly: the empty
artifact parses, `anomalies` is a well-formed empty array, `video.total_frames` is still 90, and
all 90 PNGs remain on disk** — `L1` demonstrated rather than asserted.

## 212. The accepted cost and the `A35` ruling — into the docs, not buried

Both written into `client-delivery.md` and the `m26` tag scope statement:

> **`m26` vetoes only targets measured at ZERO drawn pixels. A target that OVER-CLAIMS — measured
> non-zero but far below its claimed extent, such as the `InstancedFoliageActor` measured at
> 5,689–13,342 px against a claimed 921,600 px (the entire frame) — IS NOT VETOED and ships as a
> valid label. `m26` is a PARTIAL cure for `H5`: it removes the zero-contribution case and leaves
> the over-claim case. The over-claim rule requires a calibration campaign including
> complex-silhouette legitimate targets, which do not exist in the current measured set.**

> ⚠ **`A35` — and it now bites a VETOED case, as a RULING with its reason, not an oversight:**
> `BP_SplineSpawn_C`'s banked hide showed a small in-bbox luma change (**0.0175**) while the mask
> reads exactly zero. **A zero-silhouette target can still have indirect visual effect (shadow,
> GI). `m26` vetoes it anyway, because the label points at the OBJECT and not at its shadow.**

Plus **`L1`–`L3`** stated: frames are on disk and are NOT un-written · **a post-`m26` event count is
NOT comparable with a pre-`m26` one and `vetoed_events` carries the delta** · `labels.jsonl`
(delivery OFF) is prebuilt and uncorrectable, so **delivery OFF and ON WILL DISAGREE on event
content**.

## 213. State after PART THIRTY-ONE

| | |
|---|---|
| **slice 3** | ✅ **SHIPPED (`65deadc`) AND GATED. The veto fires on `MEASURED_ZERO` and on nothing else.** |
| **`m26`** | ✅ **FUNCTIONALLY COMPLETE — slices 1, 2 and 3 all shipped and gated** |
| gates | `G-4` ✅ · `G-5` ✅ (incl. the foliage guard) · `G-7` ✅ · `G-8` ✅ · `G-9` ✅ EXTRAS 0 · `G-10` ✅ · `G-11` ✅ · `P6` ✅ 48/48 · `A-1` disjoint ✅ · `F-1`…`F-8` **none fired** |
| build | staged **`5EA6AB92`** (`F4EBEAD7` archived first); container unchanged; A44 green |
| banked | `P31_S3_{SPLINE,CTRL49,CYL73,FOLIAGE,RAMP,G9_OFF_A,G9_OFF_B,G9_ON,INERT}` |
| ⛔ **NEXT** | **THE OWNER'S PLAY-GATE SMOKE, WHICH THE OWNER WILL BRIEF. NO TAG UNTIL THEN.** |

**WHAT THIS PART SETTLES: the cure acts. An event whose target was measured to draw nothing is
removed before `annotation.json` is written, and everything else is kept — including a target that
draws 1.4 % of what it claims, because the rule is zero-only and a threshold was refused on
evidence about what our control set cannot contain. The two failure directions that would have
mattered — vetoing a target the instrument merely cannot see, and vetoing on a ratio — were each
tested on a banked known answer, and neither occurred.**

---

# 🧭 HANDOFF — READ THIS FIRST. A COLD SESSION NEEDS NOTHING ELSE FROM PARTS 1–31.

**Session closed 2026-08-20 at the end of PART THIRTY-ONE. `m26` is FUNCTIONALLY COMPLETE and
AWAITING THE OWNER'S PLAY-GATE SMOKE. NO TAG.**
**NO TAG since `m25`. `P6` HAS NEVER MOVED. `feature/stencil-capture` is READ-ONLY at `76cac74` —
mine it, never check it out.**

## H.1 Where `m26` stands, in one read

**`m26` is the `H5` class-(ii) cure: an event whose target is MEASURED to draw nothing is removed
from `annotation.json` before it is written.** Shape ruled **(c) deferred veto with (b)'s
reporting**.

| slice | state |
|---|---|
| **1 — MEASURE ONLY** *(log-only, `IAI.Capture.Mask`, default OFF)* | ✅ **DONE AND CERTIFIED. Both faults FIXED (`795f2a4`, `4a9631a`), the EXTENT PRECONDITION shipped (`3beb3ba`), and `F-6` is COMPLETE — ALL FIVE ITEMS PASS** (item 2 by the replacement control `StaticMeshActor_73`) |
| **2 — REPORTING** (`mask.provided` → the tri-state's bool) | ✅ **SHIPPED AND GATED (`ece343f`, PART TWENTY-NINE).** All five known-answer rows correct; **`SM_Ramp2` → `false`, `BP_SplineSpawn_C` (MEASURED_ZERO) → `true`** — the two zeros stay distinguishable, tested both ways. Field SET 48/48 in BOTH delivery modes; inert when the switch is off |
| **3 — THE VETO** + `vetoed_events` + gate `G-11` | ✅ **SHIPPED AND GATED (`65deadc`, PART THIRTY-ONE). THE RULE IS ZERO-ONLY:** veto iff manifested AND `MEASURED_ZERO`. **`NOT_MEASURED` never vetoed; a measured non-zero count never vetoed however small — NO RATIO, NO THRESHOLD.** Fires on `BP_SplineSpawn_C` (8/8 removed); does NOT fire on the foliage at ~1.4 % of its claim, the two controls, or the known-Nanite target |

🎯 **THE `H5` LEGS ARE RUN, AND THE CURE IDENTIFIES BOTH INSTANCES THAT MOTIVATED IT
(PART TWENTY-EIGHT §195):**

| target | label CLAIMS | mask MEASURES | branch |
|---|---|---|---|
| **`BP_SplineSpawn_C`** | `coverage_pct` **22.89 %** | 🎯 **`MEASURED_ZERO` ×8 — ZERO pixels**, every bucket clean | **`Z1`** — the aimed-at result |
| **`InstancedFoliageActor_0_0_0`** | 🚨 **`coverage_pct` 100 %, `bbox_px (0,0,1280,720)` — the whole frame** | **0.62–1.45 % of frame** (≈ **1.4 % of what it claims**) | **`Z2`** — also the cure working |

**Both zeros/near-zeros arrived INTERPRETABLE:** `framesContributed = arms`, and
`framesDiscarded / framesResidual / framesUnconfirmed / framesNoPass / probeArms / collisions /
tagFailed` **all 0**, with **29/29 armed frames view-sized (zero dummies)** on every leg. *(Branch
`X` — "any zero with any bucket non-zero is NOT YET INTERPRETABLE" — did not fire.)*
⛔ **NO INCIDENCE CLAIM · class (i) still ENUMERATED-NOT-OBSERVED · NOT a veto test (slice 3 does
not exist; nothing was removed from any artifact) · NOT a Nanite result (both targets non-Nanite)
· NO THRESHOLD proposed or implied.**

## H.2 🚨 THE TWO FAULTS — one FIXED, one DIAGNOSED-NOT-FIXED (PART TWENTY-FOUR)

### FAULT (i) — event-scoped discard — ✅ **FIXED at `795f2a4`, measured**

The discard is now **FRAME-scoped**: a polluted read discards that frame; clean frames feed the
MAX. **`MEASURED_ZERO` is reachable only from a clean resolved read; an event with no clean frame
stays `NOT_MEASURED`** (`State` initialises to `NotMeasured` and is written only on the clean
path) — **demonstrated live by the frame-cap-truncated final event** (§169.1). Control counts per
full event: `arms=4 resolved=4 framesDiscarded=2 framesContributed=2`, **7.2517–7.2550 %** vs the
banked 7.80 % rect.

### FAULT (ii) — the stale arm read — ✅ **FIXED (`4a9631a`, the Part-25 design + A-1/A-2) and PROVEN (PART TWENTY-SIX §184)**

The mask block runs from **`FWorldDelegates::OnWorldTickEnd`** (post-toggle by position, pre-draw
same frame); the render is **BRACKETED** by an enforcing whitelist confirmation (a frame
contributes only if its `OnEndFrame` sample ran and read visible); the **`bRunning` guard**
retired §172's stray post-`FinishRun` arm (verified: zero mode≠3 pass records on all five legs).
Amendments implemented: **A-1** (probe = gate artefact: default OFF, delivery-inert by GUARD at
the fire site, `PROBE`-marked, `run_summary.mask_probe_arms`, checklisted in
`PRE-DELIVERY-CHECKLIST.md`) · **A-2** (the residual has its own counter, per event and in
`run_summary.mask_residual_discards`; 0 on every bench leg).
**Gate legs L1–L4 met every pre-declared prediction** (`p26_fix2_gate_predeclared.md`, bench
`84106bd`): blinking 4/4 contributed 7.20–7.22 %, `missing_texture` 4/4 byte-matching P24,
`missing_object` 0 in-window + 4 post-revert with the capped final event landing `NOT_MEASURED`
(the admit path, twice demonstrated live), the probe firing all three detectors on demand —
**`F-6` items 1/3/4/5 PASS.**

### H.2b ⚠ **THE TWO INSTRUMENT LIMITS — `G133` CLOSED BY THE EXTENT PRECONDITION, `G134` PERMANENT AND SCOPED**

- **`G134` — THE INSTRUMENT IS STRUCTURALLY BLIND TO NANITE GEOMETRY IN UE 5.1. ESTABLISHED, AND
  IT IS PERMANENT ON THIS ENGINE:** `Nanite::FSceneProxy::GetViewRelevance` never sets
  `bRenderCustomDepth` (`NaniteResources.cpp:941-1010`, both branches); `bHasCustomDepthPrimitives`
  rises only from that flag (`SceneVisibility.cpp:2470`); the 5.1 custom-depth pass has NO Nanite
  path. ✅ **Scoped in PART TWENTY-SEVEN: the two `H5` targets are NON-Nanite, so the cure reaches
  them — but on StackOBot the authored structural geometry is overwhelmingly Nanite, so the limit
  is the COMMON CASE, measured not projected.** Nanite targets land `NOT_MEASURED` ⇒ **always
  ADMITTED, never vetoed** — safe, and also the cure not working there. Scope statement drafted
  (§191.1).
- ✅ **`G133` — CLOSED (`3beb3ba`).** The 255 detector fires on AT MOST ONE PIXEL and its silence
  could never certify the pass ran; **`customStencilExtent` is now a CONTRIBUTION PRECONDITION**,
  so a frame contributes only on positive evidence. The Part-26 false `MEASURED_ZERO` on
  `SM_Ramp2` is **gone — all 8 events now `NOT_MEASURED`** (§189.1), and the 255 detector is
  **demoted to a SECONDARY signal**.
- ✅ **`F-6` IS COMPLETE (§194.1), ALL FIVE ITEMS.** Item 2 is satisfied by the replacement control
  **`StaticMeshActor_73`** (`Cylinder`, non-Nanite): 8/8 `MEASURED_NONZERO` at **5.27 %** against
  its own claimed 6.87 %, every bucket clean. **`SM_Ramp2` now serves as the KNOWN-NANITE
  control** — it must read `NOT_MEASURED` every time, a positive test for the `G134` limit and the
  first place a future engine bump would show.
  ⚠ **Two weaknesses travel with the `N-2` control:** it shares CB_GateLevel with the item-1
  control, so it shows **non-over-firing, not robustness**; and a stronger MainWorld candidate's
  existence is **UNESTABLISHED** (needs a `G122` census leg). 🚨 **The A35 over-fire property is
  UNTESTED and no control for it exists anywhere (§190) — it is in the tag as UNTESTED.**
- 🆕 **`G135` — a calibration environment built from a RESTRICTED ASSET SET cannot exhibit defect
  classes outside that set, and the blindness presents as a CLEAN PASS.** CB_GateLevel is
  `/Engine/BasicShapes/` throughout — why it is a stable instrument AND why it could not surface
  `G134`. ⛔ **The tension is stated, not resolved; CB_GateLevel is NOT to be changed (`G99`).**

## H.3 PROVEN — do **not** re-prove any of these

| | |
|---|---|
| **`LOCK-1`'s hidden-tick refusal** | `skippedHidden=3..4` on every event — the riskiest item in the whole plan, validated. ⚠ **It refuses on the state it READS; PART TWENTY-FOUR proved that read is one tick stale for `blinking`** |
| **the plumbing round-trip** | tag → mask pass → readback, `arms=N resolved=N` |
| **`AnomalyShaders`** | `PostConfigInit`, **no `Renderer` dep**, other modules' load order untouched |
| **the four gates** | cook · map set · **shader presence (the BOOT)** · token read-back |
| **the `m26-slice1` quartet** | preserved 6/6 at `_binary_baselines\m26-slice1-measurement-build\` |
| **the write side** | exonerated — property verified intact while the mask still saw nothing |
| **the cvar** | **`r.CustomDepth` = 3 on ALL 30 armed frames, read at the pass point** |
| **`255` = `StencilDummy`** | `FColor::White`, bound when custom depth is not produced |
| **the mask's number** | **7.23–7.25 % vs a 7.80 % banked rect, spread < 0.03 % over 14 frames** |
| 🆕 **the frame-scoped collection** | full events `discarded=2 contributed=2 MEASURED_NONZERO 7.25 %`; the all-discarded event lands `NOT_MEASURED` — measured on `P24_M26S1F1_CTRL49` |
| 🆕 **fault (ii)'s mechanism** | stale-by-one-tick arm read, established by the P24 join (0 refuter violations, R-C control clean) |
| 🆕 **the fault-(ii) FIX** | `OnWorldTickEnd` arm + bracketed render: blinking 4/4 usable, zero dummies, `LOCK-1` refuses the whole `missing_object` window (`skippedHidden=8`), stray arm gone — P26 legs L1–L4 |
| 🆕 **the probe / item 5** | one deliberate hidden arm fires the 255 detector + confirmation + discard on the shipped binary; admit bias disposes of it (`P26_FIX2_PROBE49`) |
| 🆕 **the ramp draws from the leg camera** | `CM_CM_RAMP`, identical pose, in-bbox change 0.1785 — which is what makes `G134` a finding about the INSTRUMENT, not the target |
| 🆕 **both `H5` targets are NON-NANITE** | `SM_Bush`, `SM_GenericPlane` — the cure reaches the instances that motivated it (P27 §188.2) |
| 🆕 **the discriminator** | `StaticMeshActor_49` = `/Engine/BasicShapes/Cube`, non-Nanite; CB_GateLevel is BasicShapes throughout — signature predicts measurability **5 for 5** |
| 🆕 **the extent precondition works** | `SM_Ramp2` `MEASURED_ZERO` → `NOT_MEASURED` ×8, `framesNoPass=29`, L1–L4 unchanged at matched pose |
| 🆕 **mask count tracks projected bbox AREA** | matched-area poses give byte-identical counts across builds; 0.8 % smaller area ⇒ 0.5 % fewer pixels (P27 §189.2) — **use a MATCHED POSE before comparing counts across legs** |
| 🆕 **scene depth IS Nanite-inclusive on 5.1** | `Nanite::EmitDepthTargets` writes `SceneDepth` (`NaniteMaterials.cpp:896,930`; compute path `:856`) — answered, nothing designed |
| 🆕 **`F-6` COMPLETE, all five items** | item 2 by `StaticMeshActor_73` (`Cylinder`) — 8/8 non-zero at 5.27 % vs its claimed 6.87 %, buckets clean |
| 🆕 🎯 **the cure identifies both `H5` targets** | `BP_SplineSpawn_C` **`MEASURED_ZERO` ×8** vs a 22.89 % claim · foliage **1.4 % drawn** vs a **100 %** claim — all buckets clean, 29/29 frames view-sized on every leg |
| 🆕 **slice 2's mapping** | five known-answer rows correct both ways; `provided` derives from `State` ALONE through one function that never sees a magnitude |
| 🆕 **`framesNoPass` is "not in the visible set", not "Nanite"** | the `Cylinder` leg reached it by FRUSTUM CULLING with delivery OFF (`coverage_ratio` → 0 in lockstep) — same mechanism, different cause |
| 🆕 **delivery does NOT suppress the mask** | a POSE-MATCHED delivery-ON leg reads `provided:true` on every event; the first delivery leg's zeros were its bifurcated pose |

## H.4 Rulings that travel

- ⛔ **WITHDRAWN, NOT DEFERRED: the tag/arm separation.** `F-1` refutes it at source — the proxy is
  already up to date (`SendAllEndOfFrameUpdates` runs inside `BeginRenderingViewFamilies`, same
  frame). **Zero ticks are needed, and it is a guarantee.** Nothing to revisit.
- 📌 **BANKED for any future tag/arm split:** the hidden-state test applies at **TAG**, **ARM** and
  **RESOLVE** time; hidden at **ANY** ⇒ **`NOT_MEASURED`**, never `MEASURED_ZERO`.
- ✅ **ADOPTED — `F-6` IS THE FIX GATE, all five items** (§160): both controls NON-ZERO with
  `collisions=0` · arm counts matching prediction · plausible `pctOfFrame` · **and item 5 — 🚨 THE
  255 DETECTOR PROVEN STILL LIVE, BOTH WAYS (`G96`)**, without which items 1–4 can pass on an
  instrument that has stopped looking.
- ⛔ **The stencil range stays `200`/`255`** — refuted as the cause; **not to be changed as a side
  effect of anything.**
- ✅ **`CollectResults` is FRAME-scoped as of `795f2a4`** (PART TWENTY-FOUR). The admit bias is
  unchanged and was demonstrated live.
- 🆕 **`RULING 1` (PART TWENTY-FIVE §175) travels with the fix:** the fix must close the stale
  read itself; any fix that relies on detecting the dummy works only where we happen to be
  looking. **Options C (injector publishes state) and D (post-hoc discard as the fix) are
  REJECTED on this ruling — do not re-propose them.**

## H.5 Environment a cold session inherits

| | |
|---|---|
| plugin | `AnomalyInjector`, `master`, pushed, **no tag since `m25`** |
| bench | `CaptureBench`, **local-only, no remote** |
| staged exe | **`5EA6AB92`** (slice 3, the veto; code-only hot-swap over the `m26` cook; predecessors archived first: `F4EBEAD7`, `047FA489`, `F93AEF71`, `DBA2D8EC`, `444D4812`) |
| container | `m26` cook — `utoc 9334496D` · `ucas 62EB0072` · `pak 78C977A5`; **4 maps** |
| preserved quartets | `m25-h4h5m1-measurement-build` (Parts 2–14) · `m26-slice1-measurement-build` |
| 🗺 **disk topology** | ⚠ **`Intermediate` and `Saved` are JUNCTIONS to `E:\IA_BuildCache\...`** — every path stays `D:\...` and **no tool needed editing**. Do not "fix" the missing ~21 GB on `D:`. Runbook §3.6 |
| ⚠ cook recipe | **runbook §8.6 STEP 0** (disk floor) and **STEP 3.5** (rebuild the EDITOR target — `G47`/`G131`) are **not optional** |
| bank | `_bench_sessions_bank`, latest **`P28_N2_CYL73` · `P28_H5_FOLIAGE` · `P28_H5_SPLINE`** (all accepted attempt 1), then the five `P27_EXT_*` and five `P26_FIX2_*` + every discarded attempt |

## H.6 What the next session should do first

1. **Read `docs/invisible-anomaly-mechanisms.md`** — the ledger — then this HANDOFF. **Nothing else
   from Parts 1–31 is required.**
2. 🎯 **`m26` IS FUNCTIONALLY COMPLETE — slices 1, 2 and 3 all shipped and gated.** ⛔ **THE NEXT
   STEP IS THE OWNER'S PLAY-GATE SMOKE, WHICH THE OWNER WILL BRIEF. DO NOT TAG UNTIL THEN**, and do
   not start anything new unprompted.
3. ⛔ **THE VETO RULE IS ZERO-ONLY AND IT IS RULED. NO THRESHOLD EXISTS ANYWHERE — not in code, not
   in docs.** §209 records the owner's reasoning verbatim, including why a ratio was refused
   despite the four targets separating cleanly: **every GOOD target measured is a convex primitive,
   and a complex-silhouette legitimate target — which would break a ratio rule — does not exist in
   the measured set.** ⚠ **Do not re-open this by proposing one.**
4. ⚠ **Limits that travel with `m26` and must be quoted, not softened:** the ACCEPTED COST (the
   over-claim case is NOT cured — §212) · `A35` (a zero-silhouette target may still cast shadow;
   vetoed anyway, by ruling) · `L1`–`L3` · risk 4 (a veto can turn a certifying leg INVALID below
   3 counted events — `G-11` reports before/after) · 🚨 **`G134` — a Nanite target reports
   `provided:false` and is therefore ALWAYS ADMITTED** · **the A35 over-fire property is UNTESTED
   because no non-Nanite complex-silhouette control exists (§190).**
5. ✅ **`G-9` IS CLOSED (PART THIRTY §204): `EXTRAS = 0` on a pose-matched OFF/ON pair against a
   4-member run-unique set — `/session_id`, `/speed_ratio`, `/sustained_wall_fps`, `/video/path`.
   The delivery-ON vs delivery-OFF difference is exactly those four plus `/delivery_mode`.**
   *(P29's inconclusive result is superseded; the method that closed it was route (a) — make the
   confound absent rather than expected.)*
6. `P6` does not move. CB_GateLevel untouched (`G99`). Stencil range 200/255. **No tag.**
