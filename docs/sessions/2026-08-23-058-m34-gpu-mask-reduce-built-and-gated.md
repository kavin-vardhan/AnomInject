# 2026-08-23 — session 058 — m34 BUILT AND HOME-GATED on `feature/mask-gpu-reduce`

**Status at close: the GPU mask reduction is implemented, cooked, and every branch-commit gate
(G-R1..G-R6 + G-R7's StackOBot half) is GREEN. Milestone commit `0fc00ef` on
`feature/mask-gpu-reduce`, predictions-first at `3e20032`, branch cut from master `1a3b1eb`.
⛔ NO TAG. NO MERGE. m34 closes only after G-R7's Concorde half passes on the owner's
post-delivery pass — that leg gates the MERGE to master.** `master` untouched;
`feature/stencil-capture` untouched at `76cac74`; `encode_watcher.py` untouched.

Cold-start contract executed:
`docs/sessions/2026-08-23-057-m34-gpu-mask-reduce-approved-plan-handoff.md`.
Predictions (pre-declared BEFORE implementation, per A2):
`docs/predictions/2026-08-23-m34-gpu-mask-reduce-gates.md`.

## §1 Cold start (§7 item 1 of the handoff)

- `m30^{commit}` = `1d9dc2e`, `m27^{commit}` = `4a92962` (G143 form — ⚠ under PowerShell the
  `^{commit}` suffix must be QUOTED or git sees a mangled rev and errors `Needed a single revision`).
- master HEAD = `1a3b1eb`, exactly the handoff cut point; tree clean except the untracked
  delivery-line doc `docs/CHAT-HANDOFF-crisis-weekend-delivery.md`, deliberately not touched.
- Staged exe `757A5DD4` ✓, container = the session-051 quartet ✓ (all five hashes re-verified).
- Branch `feature/mask-gpu-reduce` cut from `1a3b1eb`, pushed -u.

## §2 The A1 investigation (read first, then declared — verbatim in the predictions file §2)

FinishRun's mask drain is ONE bounded attempt (`EnqueueDrain` → `FlushRenderingCommands` →
`CollectResults`; the drain polls `IsReady()` and SKIPS not-ready items; `Reset()` deliberately
does not clear `InFlight`, because the `TUniquePtr` must keep a possibly-GPU-pending readback
alive). m34 keeps those exact semantics for the buffer readback and makes the loss LOUD: the final
drain warns `M34 LOST-IN-FLIGHT lostInFlight=N requestIds=[...]` (log-only, `run_summary` +0).
The veto consumes only complete results by construction. Measured loss on every leg this session: 0.

## §3 What was built (the approved file list, items 1–6; +411/−52 across exactly 6 files)

1. NEW `Shaders/Private/AnomalyMaskReduce.usf` — the reduce CS. 8×8 groups, one pixel per thread;
   groupshared 256×5 uint histogram+bounds; one global atomic merge per group per present tag;
   mins bit-inverted (`InterlockedMax(dst, ~x)` against a zero clear, CPU decodes with `~` gated
   on `Count > 0`). Integer atomics only ⇒ bit-exact vs the CPU scan.
2. NEW `Source/AnomalyShaders/Public/AnomalyMaskReduceShader.h` — `FAnomalyMaskReduceCS`
   (`DECLARE_EXPORTED_SHADER_TYPE(..., Global, ANOMALYSHADERS_API)`, the `FAnomalyVisibleMaskPS`
   mirror).
3. NEW `Source/AnomalyShaders/Private/AnomalyMaskReduceShader.cpp` — `IMPLEMENT_GLOBAL_SHADER`
   bound to `/Plugin/AnomalyInjector/Private/AnomalyMaskReduce.usf`, `MainCS`, `SF_Compute`.
4. EDIT `AnomalyMaskSceneViewExtension.h` — `EAnomalyMaskReduceMode {Gpu,Cpu,Both}` + Lex helper;
   `FMaskInFlight` gains `BufferReadback` + a per-item latched `Mode` (a between-runs mode change
   can never decode an in-flight item with the wrong path); texture-readback member retained
   (fork ruling 2); `SetReduceMode`; `EnqueueDrain(bool bFinal)`.
5. EDIT `AnomalyMaskSceneViewExtension.cpp` — `AfterTonemap`: after the (byte-untouched) mask
   pass, mode-gated: CS dispatch + 5,120 B buffer copy (gpu/both), full-surface texture copy
   (cpu/both). `Drain_RenderThread`: readiness = all present readbacks ready; buffer decode
   and/or the verbatim scan fill twin 256-entry array sets; in `both`, all 256 tags compared
   (count first; bounds only when count > 0) and ONE line per armed frame —
   `MASK-REDUCE COMPARE id=N IDENTICAL` / `FIRST-DIFF tag= field= cpu= gpu=` (FIRST-DIFF logs at
   Error). The RESULT is fed from the GPU table in gpu/both, from the scan in cpu; **the
   result-building loop survives VERBATIM** (tag-range filter, unassigned-255 detection order,
   `TotalMaskedPixels`, discriminator copy). The `M23 REDUCE` line gains a trailing `reduce=<mode>`
   field. Final-drain loss warning per A1.
6. EDIT `AnomalyCaptureSubsystem.cpp` — file-scope mode state + parser + provenance describer
   (inside `#if ANOMALY_CAPTURE`); Initialize reads `[AnomalyCapture] MaskReduceDefault`
   (unrecognised value REFUSED loudly, G144; console survives a world re-init); m34 init banner;
   StartRun mask echo extended with `maskReduce=<mode>(from <source>)` (G139); mode threaded to
   the SVE at extension creation and at StartRun reset; both FinishRun drains become
   `EnqueueDrain(true)`; console lever `IAI.Capture.MaskReduce <gpu|cpu|both>` with mid-run guard
   and EFFECTIVE READ-BACK line.

Item 7's no-change list HELD by diffstat: `AnomalyMaskMeasure.cpp` zero lines ·
`AnomalyMaskTypes.h` untouched · both `Build.cs` untouched (no new dep needed — AnomalyShaders
already carried `RenderCore`/`RHI`; the RDG buffer utilities came through the existing includes) ·
veto rule untouched · P6 verified (§6 G-R6).

## §4 The cook (runbook §8.6, all protective steps)

- **STEP 0 was a NO-GO as found: D: free 6.12 GB.** Cleared by the sanctioned route: the
  `prune_verify.ps1` sweep (session-ID + per-file path+size manifest vs the bank) found
  **163 exe-side sessions VERIFIED-DUPLICATE (16.82 GB) and 17 sessions + `D3D12` UNBANKED**.
  The unbanked 16 dirs were MOVED into the bank first (`RESCUE_M34PRECOOK_*`, manifest-verified
  after move — includes `H4_WSECHO`, `CAMCLIP` ×3, the M29 calibration legs, `POSTCOOK_AP`);
  then the 163 verified session payloads were deleted **verify-and-delete in the same script**,
  leg logs KEPT (the log can be the only copy of a result). 0 unverified deletions.
  D: 6.12 → 22.97 GB ⇒ GO. Also banked BY COPY (originals left until the cook proved clean):
  three unbanked sessions from earlier today in the package `Saved\AnomalyCaptures`
  (`RESCUE_M34PRECOOK_SAVEDCAP\session_20260823-{121158,123005,125514}`, ~1.3 GB — plausibly
  delivery-day runs; preserved, not judged).
- **A62** — `_binary_baselines\m34-precook-m33exe-s051container\`: the m33 exe `757A5DD4` + the
  full session-051 container (utoc `E4FE9B35` · ucas `D9929F6F` · pak `BFB95333` · global
  `C70ECDAA`/`A16A18A8`), **6/6 hash-verified at the new location**, README updated.
- STEP 3.5 editor rebuild: exit 0, 62 s; A44 on `UnrealEditor-AnomalyCapture.dll` — all new
  tokens utf16 non-zero, `TICKPIN` control non-zero; `UnrealEditor-AnomalyShaders.dll` carries
  `AnomalyMaskReduce`.
- Cook: `BUILD SUCCESSFUL`, exit 0, 51 s wall (warm DDC). The cook log itself names
  `FAnomalyMaskReduceCS (compiled 2 times)`. No Error/Fatal lines.
- 📦 **NEW QUARTET (G121): exe `17DEAA74` · utoc `2A66CA57` · ucas `A7EF9B12` · pak `D8009AD7`**
  (global.* byte-identical — the shader bytecode landed in the project container, ucas +4,096 B).
- Map gate exit 0: `CB_GateLevel`, `Entry`, `MainMenu`, `MainWorld` — exact expected set.
- Boot gate (G-R2/§3.7): zero `Missing global shader` / `Fatal error`; `Game Engine Initialized`;
  world up; init banner `maskReduce=gpu, from COMPILED DEFAULT (gpu)`.
- Token read-back (STEP 6): enforced token 64-char from the cooked ini, exit 0. The probe run
  incidentally exercised the gpu path end-to-end (3 vetoes on the occluded `_100`, SVE handshake
  40/40/40/40).

## §5 Gate results (read the predictions file §3 first — every reading below is against it)

- **G-R1 PASS.** Both targets exit 0. A44 both-encodings on the STAGED exe: `AnomalyMaskReduce`
  ascii=1/utf16=5 · `IAI.Capture.MaskReduce` utf16=7 · `MASK-REDUCE COMPARE` utf16=4 ·
  `M34 LOST-IN-FLIGHT` utf16=1 · `MaskReduceDefault` utf16=4; controls `TICKPIN` utf16=4,
  `wanted_matches` utf16=1 (live scan, not blind).
- **G-R2 PASS.** §4 above, in the required order (A62 FIRST).
- **G-R3 PASS — the identity test is unbroken everywhere: 145/145 per-frame COMPARE lines
  IDENTICAL, zero FIRST-DIFF, across all five accepted legs.**
  - `M34_R3_CTRL49B` (`_49`): 8 events MEASURED_NONZERO, maxCount **66,843–66,878 px /
    7.2529–7.2567 %** — the banked band EXACTLY, at the exact modal pose (B1 pass, distinct=1,
    modal=100 %).
  - `M34_R3_CYL73` (`_73`): 8 events MEASURED_NONZERO, 29/29 IDENTICAL. ⚠ Absolute counts read
    **48,403–48,429 vs the banked 48,590–48,597** (−0.34 %). Attributed, not smoothed: the CPU
    reference read the SAME values as the GPU path on today's frames, and the cylinder's projected
    bbox differs from the P31 leg at identical rotation (Y 469.0 vs 450.4, H 251.0 vs 257.8; `_73`
    is off the B1 calibration target, so no pose gate pins its leg). A view-dependent count
    (m26 FINDING 3) on a differently-settled admissible pose — a LEG property. The instrument
    identity (cpu==gpu, bit-exact) held.
  - `M34_R3_SPLINEB` (MainWorld): 8 events MEASURED_ZERO, 0 px, 29/29 IDENTICAL.
  - `M34_R3_RAMPB` (MainWorld, known-Nanite control): 8 events NOT_MEASURED via the extent
    discriminator (`framesNoPass` == resolved arms), never MEASURED_ZERO, 29/29 IDENTICAL.
  - `M34_R3_PROBE49`: band-exact events + the probe (below).
  - ⚠ Two first-run configuration errors, VOIDED FOR HOW THEY RAN and re-run: my first
    SPLINE/RAMP legs omitted `-Map /Game/StackOBot/Maps/MainWorld` (the targets do not exist on
    the gate level ⇒ zero events; banked as `M34_R3_SPLINE_try1`/`M34_R3_RAMP_try1`, not graded).
  - ⚠ The first CTRL49 label hit **B1 3/3** (`M34_R3_CTRL49_try1..3` banked: one genuine-A47
    signature — modal_rot 357.12/10.69, width 147.7 — one marginal bbox drift, one
    self-consistency miss at pose_match=True). Elevated bifurcation immediately after a cook
    matches the session-051 record; the relabelled `R3_CTRL49B` accepted on attempt 1 at the
    exact modal pose. Discarded for HOW THEY RAN (A63); the discarded try1 already showed
    29 IDENTICAL / 0 FIRST-DIFF.
- **G-R4 PASS, both ways (G96).**
  (a) `M26S1 PROBE RESULT ... detector255Fired=1 confirmationReadHidden=1`, frame bucketed PROBE,
  on the new path (both mode, GPU-fed result).
  (b) A deliberate one-sided perturbation (`GpuCounts[ReservedStencilBase] += 1`, both-mode only)
  on a LOCAL build (staged exe hot-swapped to `8EA34B34` for one leg) produced
  `FIRST-DIFF tag=200 field=count cpu=37190 gpu=37191` on every armed frame — the line fails and
  names the field. The perturbation was REVERTED, the clean game target rebuilt, and the staged
  exe RESTORED from backup and hash-verified back to `17DEAA74`
  (`_binary_baselines\StackOBot.exe.m34-candidate-17DEAA74`). Never committed, never shipped.
  ⚠ Note: the project-Binaries exe rebuilt after the revert hashes `F8F343F8` (link
  nondeterminism); the STAGED exe `17DEAA74` is the build identity every gate leg ran on.
- **G-R5 PASS.** Same-seed targeted pairs on the same binary, cpu vs gpu:
  kept-events pair (`_49`) — 8 events IDENTICAL including `frame_indices`, `manifested`,
  `mask.provided`; 0 vetoed both. Veto pair (spline, MainWorld) — 8 vetoed both,
  `anomalies: []` both. Delivery-ON gpu spline leg — veto outcome identical (8), file set exactly
  `Actual_Frames + annotation.json + labels.jsonl + run_summary.json` (G-9 shape). TRUE INERT
  (`IAI.Capture.Mask 0`): **zero** mask machinery lines, 8 events kept, `provided=false`
  everywhere, 0 vetoed.
  ⚠ My first "INERT" leg (`M34_R5_INERT49`) issued no mask command and therefore ran mask-ON —
  **the cooked bench ini carries `bMaskMeasureDefault=True`, so no-command is NOT inert.** Voided
  as an inert reading and re-run with the explicit `Mask 0`; kept as a BONUS reading: it is the
  ini-defaults leg, proving the INI route drives the mask with the gpu default end-to-end
  (8 events, all provided=true, states correct) — the config-matched G-R6 test leg.
- **G-R6 PASS.** `annotation.json` flat keyset **48/48, diff 0** vs m33-era. `run_summary` keyset
  **36 vs 36, diff 0** vs the m33-binary leg `M33_GA` ⇒ **m34 adds +0 artifact fields**.
  `subset_gate.py` vs the pre-m33 control pair (`M33_CTRL_A/B`, 35 keys) exits 1 with exactly ONE
  extra — `run_summary/game_clock_speed_ratio`, **m33's own documented +1** — invariant core ALL
  IDENTICAL. Reported verbatim, not relabelled a pass; the m34-specific claim (+0) rests on the
  m33-binary keyset comparison.
- **G-R7(i) — the pre-declared honest-limit branch FIRED; resolves as NO-REGRESSION PASS.**
  1920×1080 windowed (the box's ceiling; the client captures at 3200×2000 = 3.1× the pixels),
  auto-pool full pool, Mask ON, delivery OFF, `Pace 0`, seed 777, same binary, A/B cpu vs gpu:
  identical 20-event schedules, 80 arms each, 0 vetoed both. Armed-adjacent `t_wall` deltas
  (median/p90/max): **cpu 11.50/13.42/20.53 ms vs gpu 11.56/12.81/19.57 ms** — no measurable
  cpu-side elevation at 2.1 MP on a box that is not render-bound, exactly the limit the
  predictions file declared before the leg ran. Reading: **gpu ≤ cpu within noise; nothing
  regressed.** The COLLAPSE demonstration is structurally the Concorde half (6.4 MP, the host
  where the hitch is owner-visible), which is the MERGE gate.
- **A1 loss counter: 0 on every leg** (as predicted).

## §6 Environment at close

- Staged: exe **`17DEAA74`** + container **`2A66CA57` / `A7EF9B12` / `D8009AD7`** (the m34
  quartet). Predecessor preserved at `_binary_baselines\m34-precook-m33exe-s051container\` (6/6)
  and the m34 candidate exe at `_binary_baselines\StackOBot.exe.m34-candidate-17DEAA74`.
- Bank: +16 `RESCUE_M34PRECOOK_*` dirs, +3 `RESCUE_M34PRECOOK_SAVEDCAP` sessions, + the
  `M34_R3_*`/`M34_R4_*`/`M34_R5_*`/`M34_R7_*` legs (every attempt, discards included).
- D: free ≈ 20 GB post-cook. Junctions intact.
- ⚠ The pre-m34 exe-side leg-dir SESSION PAYLOADS are deleted (163, all verified banked); their
  leg dirs and logs remain beside the exe.

## §7 NOT done, named

- **G-R7(ii) — the Concorde half. Owner-run, post-delivery cook. It gates the MERGE to master;
  no merge-now-validate-later.** Until it passes: NO TAG, NO MERGE, branch-only.
- The m33 watcher-estimator ratify-or-revert (journal 056 §3) — not this branch's business.
- cpu/both deletion — a decision item at the NEXT cook after m34's (fork ruling 2).
- No CLAUDE.md status-block edit — that is a master-side (delivery-line) doc and this branch
  commits nothing to master. The status refresh lands with the merge.
- The whole-run custom-depth baseline cost (054 §7.1) — out of scope, unchanged.
- No ratio, no threshold, anywhere.

## §8 Handoff

Preemption never fired this session (no delivery item arrived). If the Monday pipeline lands
anything: master is clean and this branch blocks nothing. Next actions in order: (1) owner's
post-delivery Concorde pass runs G-R7(ii) per the predictions file (hitch A/B by eye with Mask ON
+ the 120-frame wall span; `speed_ratio` only meaningful post-m33 there, with
`game_clock_speed_ratio` beside it); (2) on PASS: merge `feature/mask-gpu-reduce` → master, tag
m34, refresh the CLAUDE.md status block; (3) the next cook after m34's decides the cpu/both
lever's fate.
