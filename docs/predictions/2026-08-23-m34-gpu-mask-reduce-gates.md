# 2026-08-23 — m34 pre-declared gates: the GPU mask reduction (G-R1..G-R7 + A1)

**PRE-DECLARED BEFORE ANY IMPLEMENTATION LEG RUNS** (amendment A2: this file is the branch's first
commit; gates → milestone commit → push, branch `feature/mask-gpu-reduce` only). The approved plan
is `docs/sessions/2026-08-23-057-m34-gpu-mask-reduce-approved-plan-handoff.md`; this file restates
nothing it doesn't have to, and every reading below is written BEFORE the code exists.

**Read this file verbatim before reading any result.** Line anchors below are at `4046aff`/`1a3b1eb`
(identical for these files).

## §0 Scope restated in one line

The m26/m27 mask PIXEL SHADER and its R8 `MaskRT` are NOT touched. m34 adds a compute reduction
(`FAnomalyMaskReduceCS`) of `MaskRT` to a 256-entry `{Count, MinX, MinY, MaxX, MaxY}` R32_UINT
table (5,120 B), read back via `FRHIGPUBufferReadback`, replacing ONLY the render-thread W×H CPU
scan (`AnomalyMaskSceneViewExtension.cpp:188-224`). The result-building loop (`:226-275`) survives
VERBATIM. Lever `IAI.Capture.MaskReduce <gpu|cpu|both>`, compiled default `gpu`; `cpu` is the named
bisect; `both` is the equivalence instrument (fork ruling 2).

## §1 Design constants (fixed here so a deviation is visible)

- CS: 8×8 thread groups over the view rect, one pixel per thread; grid = ceil(W/8) × ceil(H/8).
- Per-group GROUPSHARED histogram + bounds first (256 × 5 × uint32 = 5,120 B groupshared), then ONE
  global atomic merge per group per present tag — the atomic-contention mitigation named in the plan.
- Output buffer: 1,280 uint32 (tag-major: `T[t*5+0]=Count, +1=MinX~, +2=MinY~, +3=MaxX, +4=MaxY`),
  cleared to 0 before dispatch.
- **Mins stored BIT-INVERTED** (`InterlockedMax(dst, ~x)`) so all four bounds use `InterlockedMax`
  against the zero clear; CPU decode: `MinX = ~T[t*5+1]` etc., gated on `Count > 0`; `Count == 0`
  ⇒ tag absent, bounds never read (skip value 0).
- **INTEGER ATOMICS ONLY ⇒ bit-exact equal to the CPU scan, order-independent.** The equivalence
  gate is an identity test. No float, no tolerance, no ratio, no threshold.
- The mask values are 0 or ≥ `ReservedStencilBase` by construction (the pixel shader writes 0 below
  the base), so a full 0..255 histogram is a superset of what the result loop consumes — the GPU
  table and the CPU scan count the SAME coordinate space (the view-rect-local 0-based `MaskRT`).
- `AddEnqueueCopyPass(buffer)` → `FRHIGPUBufferReadback`, polled in the existing drain — poll,
  never wait, unchanged discipline.
- Mode is latched PER IN-FLIGHT ITEM at pass time, so a mode change between runs can never make the
  drain decode an item with the wrong path.
- In `both` mode the RESULT fed to `Results` comes from the GPU table (the path under test); the
  CPU scan rides as the reference. This choice only matters on a failing leg — any difference FAILS
  the leg regardless of which side fed the result.
- Compare line, `both` mode only, one per armed frame:
  `MASK-REDUCE COMPARE id=<req> IDENTICAL` or
  `MASK-REDUCE COMPARE id=<req> FIRST-DIFF tag=<t> field=<count|minx|miny|maxx|maxy> cpu=<v> gpu=<v>`.
  Comparison spans all 256 tags: counts first; equal-and-zero ⇒ equal by definition; equal-and-
  nonzero ⇒ all four bounds compared.

## §2 A1 — FINISHRUN SEMANTICS FOR STILL-IN-FLIGHT BUFFER READBACKS (investigated, then declared)

**What the code does today (read from source at `1a3b1eb` before writing this):** `FinishRun`'s
mask drain is ONE bounded attempt — `EnqueueDrain()` → `FlushRenderingCommands()` →
`CollectResults()` (`AnomalyCaptureSubsystem.cpp:2283-2291` on the written-session path,
`:2487-2494` on the cancelled path). `Drain_RenderThread` polls each in-flight readback with
`IsReady()` and SKIPS any that is not ready (`AnomalyMaskSceneViewExtension.cpp:183-186`) — it
never blocks on the GPU, and `FlushRenderingCommands` guarantees only that the drain RAN, not that
the GPU copy completed. An entry still not ready after that final drain stays in `InFlight`: its
result never reaches `Results`, `CollectResults` never resolves it, and the arm is LOST to the run
— today visible only as `resolved < arms` on the `M26S1 EVENT` line, with no dedicated count and no
warning. `Reset()` deliberately does NOT clear `InFlight` (`:54-65`): the `TUniquePtr` ownership
keeps a possibly-GPU-pending readback alive until a later drain consumes it or the extension is
torn down, which is what makes the skip safe.

**m34 DECLARES (the bounded-wait branch, with the loss made LOUD):** the buffer readback inherits
the identical bounded-wait semantics — one final `EnqueueDrain` + `FlushRenderingCommands` +
`CollectResults` at `FinishRun`, no waiting loop, no spin, poll-never-wait unchanged, and the same
`FMaskInFlight`/`TUniquePtr` lifetime rule (an entry is destroyed only after its readbacks are
consumed or at extension teardown — never while the GPU may still write it). Any measurement still
in flight after that final drain — buffer, or texture in `cpu`/`both` mode — is a LOST MEASUREMENT
and **the loss is LOUD: the final drain counts the survivors and emits one WARNING naming the count
and their request ids** (the B′ miss-is-loud shape). LOG-ONLY — `run_summary` stays at +0 fields;
the `M26S1 EVENT` line continues to expose the same fact as `resolved < arms`. **The veto consumes
only COMPLETE results, BY CONSTRUCTION and unchanged:** `MEASURED_ZERO` is reachable only from a
fully decoded table on a clean contributing frame; an event whose armed frames were all lost stays
`NOT_MEASURED` ⇒ ADMIT. Expected loss count on this bench: **0 on every leg** — readback latency
was measured at one render frame (m26 M-1) and the `DrainTail` phase (≥10 frames) precedes
`FinishRun`. Per m33's G-B lesson, no G-R gate reads this teardown-adjacent warning as its
instrument; every gate below reads mid-run lines or on-disk artifacts.

## §3 The gates, with predicted readings

### G-R1 — build + token scan
Build BOTH targets exit 0 (⚠ G47/G131: the EDITOR target too — the cook runs on editor binaries).
A44 both-encodings scan of the STAGED exe for the new tokens: the shader name
(`AnomalyMaskReduce`), the cvar name (`IAI.Capture.MaskReduce`), the compare token
(`MASK-REDUCE COMPARE`). ⚠ Runbook §8.2's example control `IsHideTypeAnomaly` is STALE (renamed at
session 053) — the live control is `TICKPIN` or `wanted_matches`.
**Predicted:** all three new tokens present (utf16 non-zero), control token non-zero, scan sound.

### G-R2 — THE BOOT GATE (full cook; G129 — a new global shader cannot hot-swap)
Full cook per runbook §8.6 (STEP 0 disk floor and STEP 3.5 editor rebuild NOT optional); map gate
exit 0 on the expected set; new quartet recorded (G121: identity = exe hash + pak identity);
**the session-051 quartet preserved and hash-verified at the new location FIRST (A62)**; packaged
build boots clean.
**Predicted:** no `Missing global shader` fatal at engine init — for EITHER `FAnomalyVisibleMaskPS`
or the new `FAnomalyMaskReduceCS`; boot reaches the control server; map set = CB_GateLevel +
Entry + MainMenu + MainWorld.
**Failure branch:** a missing-shader fatal or map-gate failure HALTS the milestone — no leg runs on
a build that did not pass this gate.

### G-R3 — EQUIVALENCE (decisive)
`IAI.Capture.MaskReduce both` on targeted legs over the known-answer set. Every armed frame:
IDENTICAL counts AND bounds for every tag. **Bit-exact is the spec; ANY `FIRST-DIFF` line FAILS
the leg, no tolerance, and a failed G-R3 is a STOP-AND-REPORT — no same-turn fix to the
instrument.**
**Predicted per leg (known answers from the m26–m30 record):**
- `StaticMeshActor_49`: MEASURED_NONZERO, maxCount in 66,843–66,878 px / 7.23–7.25 % of frame.
- `StaticMeshActor_73` (Cylinder, non-Nanite control): MEASURED_NONZERO, 48,590–48,597 px.
- `BP_SplineSpawn_C`: MEASURED_ZERO, 0 px on every event (×8).
- `SM_Ramp2` (known-Nanite control): NOT_MEASURED on every event via the extent discriminator
  (`framesNoPass` = resolved arms), never MEASURED_ZERO.
- One `IAI.Capture.MaskProbe` leg: see G-R4(a).
- Every armed frame on every leg: `MASK-REDUCE COMPARE ... IDENTICAL`; zero FIRST-DIFF lines.
⚠ Known-answer bands assume the B1 pose gate passes (A47/A63 discipline as always); a pose-failed
attempt is INVALID and re-run, banked per A63 — invalidity is declared by the pre-fixed rule, never
by the outcome.

### G-R4 — GUARDS PROVEN BOTH WAYS (G96)
(a) The probe fires the 255 detector + end-of-frame confirmation + frame discard **on the NEW
path** (`MaskReduce gpu` or `both` with the GPU table feeding the result): `M26S1 PROBE RESULT ...
detector255Fired=1 confirmationReadHidden=1`, frame bucketed PROBE, contributes nothing.
(b) The COMPARE line proven able to FAIL on a local build: a deliberate mismatch injected (one
value perturbed on one side), seen to produce `FIRST-DIFF tag=... field=...` naming the field,
then REVERTED — the m27 guard-proven-by-breaking-it discipline; the perturbation never ships and
never rides any other leg.
**Predicted:** both directions demonstrated; (b)'s perturbed build produces FIRST-DIFF on the first
armed frame.

### G-R5 — REGRESSION
Same-seed TARGETED pre/post legs (G150: targeted, never auto-pool, for cross-commit comparison):
identical event sets, identical veto outcomes (m27 Gate-3 shape); INERT leg `IAI.Capture.Mask 0`
unchanged (zero mask lines, zero vetoes); one delivery-ON leg — veto outcome identical to the
matching delivery-OFF leg (G-9 orthogonality shape).
**Predicted:** event sets keyed by (target, anomaly_type, start_frame) identical pre/post; vetoed
counts identical; the INERT leg emits no mask machinery at all.

### G-R6 — ARTIFACT
`annotation.json` keyset 48/48; `run_summary` FIELD-SET diff EMPTY against a same-binary control
pair (`subset_gate.py`; A63/A64 apply; the m33 `M33_CTRL_*` shape is the worked example — the
control pair establishes the run-unique set empirically, then the diff must be a subset with ZERO
extras).
**Predicted:** 48/48 and +0 run_summary fields. m34 adds NO artifact field anywhere (the A1 loss
count is log-only).

### G-R7 — PERF, two halves with different roles (A2)
**(i) STACKOBOT HALF (gates the branch commit):** packaged, full pool, Mask ON, delivery OFF
(labels.jsonl needed), `IAI.Capture.Pace 0` so per-frame cost is not hidden under the pacer sleep,
same seed both legs, A/B on the SAME binary: `MaskReduce cpu` vs `MaskReduce gpu`, at the largest
achievable windowed resolution on this box (recorded from the leg's own log, identical for both
legs). Instrument: per-frame `t_wall` deltas from `labels.jsonl` (m33's stamps), fire-active
window rows vs outside.
**Predicted:** on the gpu leg the armed/in-window delta distribution is indistinguishable from the
unarmed baseline; on the cpu leg it is elevated. **Honest limit, declared now:** at bench
resolution (~0.9–3.7 MP vs the client's 6.4 MP) the cpu-leg elevation may be too small to resolve
against frame-time noise; if the cpu leg shows NO measurable elevation, the StackOBot half degrades
to a NO-REGRESSION reading (gpu armed deltas ≤ cpu armed deltas within noise) and the collapse
demonstration falls entirely to the Concorde half — stated before the leg runs, not after.
**(ii) CONCORDE HALF (gates the MERGE to master, owner-run, post-delivery cook):** hitch A/B by eye
with Mask ON + the 120-frame wall span by stopwatch/`t_wall` — expected back toward ~4 s + lead-in.
🚨 Never read `speed_ratio` as the perf verdict on a PRE-m33 binary on that host (blind under the
pin); post-m33 it is meaningful, and `game_clock_speed_ratio` beside it shows the regime.

## §4 Item-7 no-change list — GATED, NOT ASSUMED

Declared unchanged and checked at gate time (diffstat per G115 before every commit):
`AnomalyMaskMeasure.cpp` (zero lines) · the veto rule (enum-state only; NO RATIO, NO THRESHOLD —
journal §209 stands) · `FAnomalyMaskResult` shape (`AnomalyMaskTypes.h` untouched) ·
`annotation.json` (P6 DOES NOT MOVE) · `labels.jsonl` · `run_summary` field set (+0) · both
`Build.cs` (AnomalyShaders already carries `RenderCore`/`RHI` non-Shipping; AnomalyCapture already
has Renderer private access — any surprise dep is REPORTED, not slipped in).

OUT OF SCOPE, NAMED: the whole-run custom-depth baseline cost (054 §7.1's second cost) · anything
watcher/ratio-side (m33's business; `encode_watcher.py` is not touched on this branch).

## §5 Standing constraints carried into every leg

- PREEMPTION IS ABSOLUTE: any Concorde-delivery item (cook, G-C, dry-run fallout) parks m34
  mid-step. Branch-only commits guarantee no m34 state blocks a master-side fix.
- `feature/stencil-capture` untouched at `76cac74`. `master` receives nothing from this branch
  until the post-delivery merge gated on G-R7(ii).
- No ratio, no threshold, anywhere (journal §209).
- Teardown-printed telemetry never survives a harness kill (m33 G-B): every gate above reads
  mid-run lines or on-disk artifacts, never an exit-time counter.
