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
