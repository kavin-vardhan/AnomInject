# 2026-08-19 — 045 — H4 pre-flight: the run is BLOCKED, and the block was found before it ran

**Plugin:** `AnomalyInjector` — **docs only. NO production code touched. NO tag.**
**Bench:** `CaptureBench` — three new read-only recon instruments. Local-only.
**Base:** `caaac09` (tag `m25` → `ebf1f16` → `d8482a2`).

**NO RUN HAPPENED. No branch obtained. This journal records a pre-flight that reached the
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
