# 2026-08-19 — 045 — H4, the re-cook, MainWorld, and H5

**Plugin:** `AnomalyInjector` — **docs only, in every part. ZERO production code. NO tag.**
**Bench:** `CaptureBench` — read-only instruments throughout. Local-only.
**Base:** `caaac09` (tag `m25` → `ebf1f16` → `d8482a2`).

⚠ **ONE INVESTIGATION, NINE PARTS.** It began as a one-run H4 test and became the arc that unblocked
path (a)'s environment and then found a different lead entirely. It is not split because it is not
separable — each part exists because the one before it produced something unexpected. **Renamed from
`…-045-h4-preflight-halt.md` on 2026-08-19; that title described only Part One.**

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
| **Nine** | 62– | **Owner evidence redirects the lead → `H5`** | ✅ **H5 class (ii) SUPPORTED**, reproduced here. **G124** |

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
