# `P9` FIXTURE-V2 — a letterboxed `CB_GateLevel`: predictions, controls and gates

**Written 2026-09-02/03, session 067, BEFORE ANY LEG RUNS ON THE NEW FIXTURE.** Committed as its own
`docs:` commit before the first leg.

> ⛔ **`docs/predictions/2026-09-02-p9-blinking-boundary-repro.md` (v1) IS NEVER AMENDED.** This is a
> new file because the fixture changed. v1's measurements stand as taken; its discriminators are
> carried here **verbatim**.

> ⛔ **NO MECHANISM IS PROPOSED ANYWHERE IN THIS FILE.** If `P9`-SHAPE appears, the raw tables are
> reported and work stops. Design happens chat-side.

**Why v2 exists.** v1's five legs returned **UNDECIDABLE on every event**, and the cause was a
**fixture conjunction failure**, not `P9`: a non-zero view-rect origin forced `MainWorld` (the
letterbox lever refuses `CB_GateLevel`'s `SpectatorPawn`), and `MainWorld`'s intro camera **moves
during capture** — 32 distinct origins over 90 frames — so the per-event bbox changed every frame
and `A56` collapsed to modal 1-in-8. Journal 067 §11.

---

## 1. THE FIXTURE — ✅ **ROUTE R3, THE ZERO-COOK ROUTE. NO SOURCE CHANGE, NO COOK, NO NEW EXE.**

**`CB_GateLevel`, letterboxed by two engine console commands**, issued in the leg's `ExecCmds`:

```
Set PlayerCameraManager bDefaultConstrainAspectRatio true
Set PlayerCameraManager DefaultAspectRatio 2.39
```

⇒ **`READBACK-LAYOUT sourceExtent=1280x720 rect=(0,92)-(1280,628) picture=1280x536`** — the
**identical rect** the lever produced on `MainWorld`, now on the settled-camera fixture. **Measured
on the fixture probe, not predicted.**

📌 **This supersedes Route A (teaching the lever a fallback) and Route B (a copied level).** Neither
is built. **The lever is untouched, `AnomalyCaptureLetterbox.cpp` is unmodified, no plugin module
recompiles, the exe stays `D2BB25A5`** — which also means fixture-v2 legs sit on the **same binary as
v1**, so the fixture is the only variable that moved.

### 1.1 `R1` — where the lever lives, and its exact refusal condition

| | |
|---|---|
| file | `Source/AnomalyCapture/Private/AnomalyCaptureLetterbox.cpp` |
| module | **`AnomalyCapture`** (Runtime), in the `AnomalyInjector` plugin — **not** CaptureBench |
| callers | **NONE.** Every reference to `AnomalyLetterbox::` is inside that one file; it exists only as the console command registered at `:165-172`. It is a self-contained bench artifact living in a production module. |
| **refusal condition** | `ResolveViewTargetCamera` (`:22-55`) requires `ViewTarget->FindComponentByClass<UCameraComponent>()` (`:46`); a null return refuses at `:49-51` with *"view target '…' has no UCameraComponent"*. **A `SpectatorPawn` has none** — that is all of `G193`. |

### 1.2 `R2` — the view-target-agnostic constraint path in UE 5.1.1 (`++UE5+Release-5.1`)

Engine at `D:\UESource\UnrealEngine`, `Engine/Build/Build.version` → 5.1.1.

1. **The defaults are applied unconditionally, BEFORE any view-target-specific work** —
   `APlayerCameraManager::UpdateViewTarget`, `PlayerCameraManager.cpp:340`:
   ```
   352:  OutVT.POV.FOV                   = DefaultFOV;
   354:  OutVT.POV.AspectRatio           = DefaultAspectRatio;
   355:  OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
   ```
   Properties declared at `PlayerCameraManager.h:221` and `:400`; engine defaults `1.33333f` / `false`
   at `PlayerCameraManager.cpp:44-45`. They are **re-applied on every view-target assignment** too
   (`:216-219`), so the setting is sticky across a view-target change.
2. **The default `CameraStyle` reaches the pawn path.** `CameraStyle = NAME_Default` (`:58`), so the
   `else` chain at `:369-435` falls through to `UpdateViewTargetInternal` (`:434`) →
   `OutVT.Target->CalcCamera(...)` (`:335`).
3. **`CalcCamera` on a camera-less actor sets LOCATION AND ROTATION ONLY.** `AActor::CalcCamera`,
   `Actor.cpp:3067-3086`: with no active `UCameraComponent` it calls
   `GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation)` (`:3085`) and nothing else.
   ⇒ **`bConstrainAspectRatio` and `AspectRatio` SURVIVE for a `SpectatorPawn`.**
4. 🎯 **THE EQUIVALENCE IS EXACT, AND THIS IS THE LOAD-BEARING FACT.**
   `UCameraComponent::GetCameraView` (`Camera/CameraComponent.cpp`) sets
   `DesiredView.AspectRatio` (`:392`) and `DesiredView.bConstrainAspectRatio` (`:393`) — **the same
   two `FMinimalViewInfo` fields** the camera-manager defaults set. The component path and the
   manager path converge on one POV, so **everything downstream — the local player's view-rect
   computation, `READBACK-LAYOUT`, and the census's rect-relative `frame_px` — receives an
   identical input and cannot tell the two apart.** Confirmed by measurement: the same
   `rect=(0,92)-(1280,628)` came out of both.

### 1.3 `R3` — the zero-cook route, and why it is preferred

`Set` is a **`UObject` exec command**, `Obj.cpp:3937-3941` → `PerformSetCommand` (`:3435`) →
`GlobalSetProperty`, which applies the property to **live instances** of the class, not only a CDO.
🚨 **It is NOT shipping-gated** — the `#if !UE_BUILD_SHIPPING` block begins at `:3947`, *after* `SET`
and `SETNOPEC` — so it is available in this Development-configuration package.

✅ **VERIFIED BY RUNNING IT, not by reading it**: fixture probe `P9V2_PROBE`, banked, produced the
rect above. ⇒ **preferred over Route A exactly as the ruling directed.**

⚠ **Honest limits of this route, stated up front.** It is a **bench** mechanism: it depends on a
non-Shipping build, and it sets a property on every `PlayerCameraManager` instance in the process.
⛔ **It is a FIXTURE device and must never appear in a client-facing or delivery payload.**

### 1.4 The fixture's measured properties — why it is the right one

From the probe (`P9V2_P9V2_PROBE_try1`):

| property | measured |
|---|---|
| view rect | `(0,92)-(1280,628)`, picture `1280x536`, **`minY = 92 > 0`** |
| camera settle | `settle_start=0  dropped=0  modal_rot=(0,0,0)` |
| bbox stability | 🎯 **`distinct=1`, `modal=100.0%` over 59 rows** |
| census candidates | 77 · zero 12 · below-floor 46 · **nanite 0** |
| blinking events | 8 — seven complete plus one truncated by the cap |
| key ring | `121/121`, missed 0 · `wanted_matches` 90 · `fires_fallback_all` 0 |

**One distinct bbox at 100 % modal is precisely what `MainWorld` could not give** (14–17 % modal,
28–29 distinct). This fixture satisfies **both** halves of the conjunction that defeated v1.

---

## 2. Configuration — every value by `StartRun` read-back (`A48`), never the value issued

| axis | value |
|---|---|
| map | **`CB_GateLevel`** (harness default) |
| exe / container | **`D2BB25A5`** on the unchanged m34 quartet — **same binary as v1** |
| letterbox | the two `Set` commands; **`minY > 0` asserted per leg** from `readback_layout`, and the harness **invalidates** any leg declaring `-LetterboxedFixture` without it |
| census | **ON**, floor **0.5** from **console**, `maxVerdictAgeTicks` 12, `excludeTranslucent` 1, `reservation` 1 |
| mask | **ON** |
| blinking half-period | **3** (`F-BLINK` compiled default) |
| burst schedule | `IAI.Capture.Config 2 4 8 4 0`, cap **90** |
| VideoFps / clock | **30** / **wall** |
| delivery | **OFF on every leg** |
| pose gates | **`B1` DECLARED NOT APPLICABLE** (§1 of the harness note); **`A47` via `-RequireModalRotZero` IS the gate** — it reads the camera only and a letterbox does not move the camera |

📌 **Delivery is OFF and the reason is v1's own measurement, not an assumption:** v1 leg **C** ran
`delivery_mode = True` and produced an identical event structure and near-identical separations to
leg A (0.011701 against 0.011758). **Delivery is excluded as a `P9` factor**, so **leg C does not
return** in v2.

**Seeds:** primary **4242**, fallback **777**.

**Event count:** the probe's cadence gives **7 counted blinking events + 1 TRUNCATED** at cap 90
(claimed sets `{4,5,9,10}`, `{16,17,21,22}` … `{76,77,81,82}`, then `{88,89}` truncated). ⇒
**N = 7** for the targeted legs, **fixed before running**. A departure from 7 is reportable, not
corrected.

---

## 3. Discriminators — CARRIED FROM v1 VERBATIM

⛔ **Unchanged. May be tightened before measurement, never loosened, never edited after.**

- **`P9` REPRODUCED** = ≥1 blinking event, any leg, whose observed hidden set differs from its
  `frame_indices` in **BOTH directions** within the window — at least one claimed frame observed
  visible **AND** at least one unclaimed frame observed hidden — **and not expressible as a constant
  shift**. Report the gap between the two differing frames as data (Bates: 3).
- **NOT `P9`** = a uniform per-event displacement → `SHIFTED(k)`, **`P1` class, do not conflate**.
- **ONE-DIRECTIONAL** = a difference in one direction only → its own observation class; ⛔ **no new
  `P`-number without chat**.
- **NOT REPRODUCED** = **every** blinking event `ALIGNED` on **both** legs with margin ≥ TAU **and**
  the reader returning **empty differences on every event**. ⛔ **Both conditions.**
- **UNDECIDABLE** = margins below TAU, or the reader's split below its separation floor. ⛔ **No
  re-thresholding after the fact.**

**`SEP_RATIO` STAYS `5.0`, FROZEN.** ⛔ **Never retuned after any leg.** A leg event landing near 5.0
gets its margins **reported and annotated**, never reclassified (`A55` / `A57`). `FLANK` 2,
`MARGIN_FLOOR` 0.5, `KMAX` 6 likewise unchanged.

---

## 4. IN-REGIME INSTRUMENT CONTROLS — the `A57` gate for this regime

🚨 **Nothing is graded as `P9` evidence until these pass.** The reader's four banked controls were
gated on **un-letterboxed** `CB_GateLevel` pixels; letterboxing crops and rescales the picture, so
the instrument must be shown to work **in the new regime, on the new regime's own data.**

**FIXTURE-PROOF LEG:** one targeted `blinking` leg, **paced 30**, on the fixture, target
**`StaticMeshActor_49`** — the certified gate target, which the probe confirms draws and fires.
Then, **on its own banked data**:

| # | control | required |
|---|---|---|
| **(i)** | the reader on the leg as-is | **it must COUNT its events** — per-event `A56` passes with the settled camera and separations land decidable. ⛔ **All-UNDECIDABLE here = THE FIXTURE FAILED → HARD STOP, report.** |
| **(ii)** | `--synth-move` against **its own** annotation | **`P9`-SHAPE, exactly one-in / one-out, on every readable event.** This is the in-regime proof the reader still sees the shape **on letterboxed pixels**. ⛔ Any miss → **HARD STOP**. |
| **(iii)** | `--synth-shift 1` against **its own** annotation | **ZERO `P9`-SHAPE, required regardless.** Anchor refusals naming the hidden flank are **expected IF this cadence is flush-boundary**, which is **stated from the annotation, not assumed** — see below. `A54`'s banked `R30 --shift 1` positive control (12/12 `SHIFTED`, margins 0.105 → 0.050) is **CITED as the uniform-shift companion and is NOT re-run.** |

**Flush-boundary statement, from the probe's own annotation rather than assumed:** the claimed sets
are `{4,5,9,10}`, `{16,17,21,22}`, `{28,29,33,34}`, … — the **same 2-hidden / 3-visible / 2-hidden
cadence as `R30`**, with the first claimed frame at the window's leading edge. ⇒ **this cadence IS
flush-boundary**, so `--synth-shift 1` is **expected** to slide a genuinely hidden frame under the
leading flank and produce anchor refusals. **That is the guard working**, and it is why `SHIFTED(k)`
is unreachable here (`SHIFTED(k)` needs window slack ≥ |k|).

**GATE: (i) and (ii) pass, and (iii) shows zero `P9`-SHAPE → PROCEED. Otherwise STOP.**

---

## 5. The legs

| leg | targeting | pacing | note |
|---|---|---|---|
| **A** | targeted `blinking` on `StaticMeshActor_49` | **paced 30** | 📌 **the FIXTURE-PROOF LEG *IS* leg A** if its controls pass — **grade it, do not re-run it** |
| **A′** | same target | **unpaced** | |
| **B** | auto-pool | paced 30 | ⚠ **ONLY IF** a settled census cycle shows **≥1 non-target blinking-capable candidate** |
| **B′** | auto-pool | unpaced | same condition |
| ~~C~~ | — | — | ⛔ **DOES NOT RETURN** — delivery excluded by v1's leg-C measurement |

**If the auto-pool is empty or target-only, B/B′ are recorded as structurally N/A — never "failed".**

Same seed / map / exe / container across all fixture-v2 legs. Every value by `StartRun` read-back
(`A48`). **`_leg_geometry.json` verified per leg** — the artifact that convicts a mis-invoked leg.
Key ring and marker series clean (`A45`) or the leg is **VOID**. `census_fires_fallback_all`
non-zero → **report, never re-run to a green.** Zero-byte frames surviving the flush wait → **VOID
for pixel work.**

**READ:** the reader's per-event table (raw sets, both differences, best `k`, residual, margins) ·
the **`A54` companion demoted to the `P1` question only** · each leg's `ticks_per_captured_frame`
**recorded as a config echo, no longer a question** (closed as a non-finding in journal 067 §11.4).

---

## 6. STOP conditions

| condition | consequence |
|---|---|
| any in-regime control (i)/(ii)/(iii) fails | ⛔ **HARD STOP.** No legs graded. |
| `readback_layout` origin is zero on a leg declaring `-LetterboxedFixture` | ⛔ **The harness INVALIDATES the attempt.** The switch must show its receipt. |
| config read-back disagrees with §2 | ⛔ **STOP — the leg does not run.** |
| key ring or marker series not clean (`A45`) | ⛔ **That leg is VOID. Say so.** |
| `census_fires_fallback_all` non-zero | ⛔ **Report. Do not re-run to a green.** |
| zero-byte frames survive the flush wait | ⛔ **VOID for pixel work.** |
| `_leg_geometry.json` disagrees with the intended config on any axis | ⛔ **VOID** — the mis-invocation gotcha's own guard |

---

## 7. What is deliberately NOT here

- ⛔ **No production source change of any kind.** The lever is untouched; no plugin module
  recompiles; the ceiling knob (floor 0.5 / ceiling 25) stays **QUEUED** and does **not** ride this
  work; no census change; no `a54_oracle.py` edit; no `SEP_RATIO` retune; no `CB_GateLevel` content
  edit; `Build.cs` untouched; no tags; `feature/stencil-capture` untouched.
- ⛔ **No cook.** R3 removed the need for one.
- ⛔ **No mechanism, lead or likely cause for `P9`.**
