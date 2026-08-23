# 2026-08-23 — session 057 — m34 HANDOFF: the GPU mask reduction, plan APPROVED, ready to start

**THIS FILE IS THE COLD-START CONTRACT FOR m34.** It is self-contained: the defect, the approved
design, the file list, the gates, chat's verdict with its three amendments, the branch mechanics,
and every source anchor and gotcha the work depends on. A fresh session should be able to execute
m34 from this file plus the repo, without the chat transcript.

## §0 STATE OF THE WORLD AT HANDOFF (verify at cold start, per the standing tag/HEAD discipline)

- Plugin `master` = `4046aff`, clean, pushed. AnomDash `master` = `132d27d`, pushed.
- **Highest tag `m30`. `m31` OPEN (awaits Concorde V-3/V-4). `m32` BURNED (never re-mint). `m33`
  BUILT + HOME-GATED + PUSHED (`03b0b7a`), closes only after its G-C on Concorde. NO TAG on any of
  these.** 🔴 m33 carries one PENDING chat item (watcher estimator ratify-or-revert, journal 056
  §3) — **NOT m34's business; do not touch `encode_watcher.py` on the m34 branch.**
- Staged bench exe **`757A5DD4`**; container = the session-051 quartet (utoc `E4FE9B35` · ucas
  `D9929F6F` · pak `BFB95333`). Baselines in `D:\IntrusiveAnomalies\_binary_baselines\`.
- ⚠ **THE MONDAY DELIVERY PIPELINE PREEMPTS m34 BRANCH WORK AT ANY MOMENT, NO CEREMONY** (chat
  A3). The delivery cook carries m33 + m31-validation; m34 is deliberately NOT in it.

## §1 WHAT m34 IS, AND WHY

**The capture hitch = the m26/m27 mask measure pass** (owner-bisected, journal 054 §7: empty pool
no hitch; full pool hitches DURING windows; `Mask 0` removes it). ⚖ **OWNER RULING: THE MASK STAYS
ON** — turning it off re-admits invisible-object labels (the client's complaint #1). The hitch's
mechanism: per armed frame the mask path reads back a **full view-rect R8 surface** (6.4 MB at the
client's 3200×2000) and then runs a **W×H CPU scan ON THE RENDER THREAD**
(`AnomalyMaskSceneViewExtension.cpp:188-224`) — 6.4 M iterations per measured frame.

**m34 moves the counting to the GPU:** a compute shader reduces the mask to a ~5 KB per-tag table;
the CPU reads back the table, not the surface. Latency is free — the veto consumes results only at
`FinishRun`. **The pattern is proven on the client's own engine:** the team's ExportTextures plugin
(compute-pack → tiny buffer readback → poll) extracts far more data per frame on Concorde without
hitching; our SVE path was originally derived from it, and this is a return to its shape. Filed as
the post-delivery candidate at journal 054 §7.2; the owner's bisect is its motivating measurement.

## §2 THE APPROVED DESIGN (chat verdict: "Design, file list, gates G-R1..G-R7, risks: approved as
written, subject to A1–A3")

The m26/m27 mask PIXEL SHADER and its R8 `MaskRT` are **NOT touched** — the certified instrument
(tag values in `[ReservedBase..255]`, EClear-to-0, the 255/StencilDummy signature, the
extent/mode discriminators captured at pass time `:126-140`) stays byte-identical. After the mask
pass, a new **`FAnomalyMaskReduceCS`** reduces `MaskRT` to a 256-entry table
`{Count, MinX, MinY, MaxX, MaxY}` in one R32_UINT RDG buffer (256 × 5 × 4 B = 5,120 B):

- 8×8 thread groups over the view rect; **groupshared per-group histogram + bounds first**, one
  global atomic merge per group (keeps worst case flat under heavy tag coverage — the atomic-
  contention risk named in the plan);
- buffer cleared to 0; **mins stored bit-inverted** so all four bounds use `InterlockedMax`
  against a zero clear; decoded on the CPU; skip value 0;
- **INTEGER ATOMICS ONLY ⇒ the result is BIT-EXACT equal to the CPU scan, order-independent** —
  the equivalence gate is an identity test. No float, no tolerance, no ratio, no threshold.
- `AddEnqueueCopyPass(buffer)` → `FRHIGPUBufferReadback`, polled in the existing drain (poll,
  never wait — unchanged discipline).
- `Drain_RenderThread` replaces **ONLY** `:188-224` (the texture lock + scan) with a 5 KB lock +
  decode into the same `Counts/MinXs/MinYs/MaxXs/MaxYs` arrays; **the result-building loop
  `:226-275` survives VERBATIM** — tag-range filter, unassigned-255 detection order,
  `TotalMaskedPixels`, discriminator copy — so `FAnomalyMaskResult` semantics cannot drift.

### File-by-file (approved)

1. NEW `Shaders/Private/AnomalyMaskReduce.usf` — the reduce CS.
2. NEW `Source/AnomalyShaders/Public/AnomalyMaskReduceShader.h` — `FAnomalyMaskReduceCS`
   (`DECLARE_EXPORTED_SHADER_TYPE(..., Global, ANOMALYSHADERS_API)` — mirror of
   `FAnomalyVisibleMaskPS`, `AnomalyVisibleMaskShader.h:11-26`).
3. NEW `Source/AnomalyShaders/Private/AnomalyMaskReduceShader.cpp` — `IMPLEMENT_GLOBAL_SHADER`
   bound to `/Plugin/AnomalyInjector/Private/AnomalyMaskReduce.usf`.
4. EDIT `Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.h` — `FMaskInFlight` gains
   `TUniquePtr<FRHIGPUBufferReadback> BufferReadback`; the texture-readback member is retained
   while the compare lever exists (§4 ruling 2).
5. EDIT `Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.cpp` — `AfterTonemap`
   (`:77-156`): dispatch the CS after the mask pass, enqueue the BUFFER copy (texture copy only
   in compare mode); `Drain_RenderThread`: buffer decode replacing the scan; one greppable
   per-frame line in compare mode only
   (`MASK-REDUCE COMPARE id=... IDENTICAL | FIRST-DIFF tag=... field=...`).
6. EDIT `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp` — console lever
   `IAI.Capture.MaskReduce <gpu|cpu|both>`, default `gpu` (G88: console beats ini; `cpu` is the
   NAMED BISECT, the `IAI.Capture.SVE 0`/TickPin precedent), threaded to the SVE; echoed with
   provenance on the existing StartRun config line (G139).
7. NO CHANGE, declared and gated as unchanged: `AnomalyMaskMeasure.cpp` (game-thread arm/collect —
   zero lines) · the veto rule (enum-state only; **NO RATIO, NO THRESHOLD — journal §209 stands,
   never re-open it**) · `FAnomalyMaskResult` shape · `annotation.json` (**P6 DOES NOT MOVE**) ·
   `labels.jsonl` · `run_summary` field set (+0 fields) · both `Build.cs` (AnomalyShaders already
   carries `RenderCore`/`RHI` non-Shipping — `AnomalyShaders.Build.cs`; AnomalyCapture already has
   Renderer private access — any surprise dep is REPORTED, not slipped in).

OUT OF SCOPE, NAMED: the whole-run custom-depth baseline cost (054 §7.1's second cost — a constant
tax, not the spike) · anything watcher/ratio-side (that is m33).

## §3 GATES G-R1..G-R7 (approved; pre-declare as a predictions file BEFORE any leg, house style)

- **G-R1** build BOTH targets exit 0 (⚠ G47/G131: the EDITOR target too — the cook runs on editor
  binaries); A44 both-encodings scan of the STAGED exe for the new tokens (shader name, cvar name,
  COMPARE token). ⚠ Runbook §8.2's example control `IsHideTypeAnomaly` is STALE (renamed at
  session 053) — use a live control, e.g. `TICKPIN` or `wanted_matches`.
- **G-R2** THE BOOT GATE — 🚨 **a new global shader CANNOT ride the code-only hot-swap (G129;
  m26's exact failure: `Missing global shader ... permutation 0` fatal at engine init, with the
  switch OFF, before anything runs).** Full cook per runbook §8.6 (STEP 0 disk floor and STEP 3.5
  editor rebuild are NOT optional); map gate exit 0; new quartet recorded (G121: identity = exe
  hash + pak identity); **the session-051 quartet preserved and hash-verified at the new location
  FIRST (A62)**; packaged build boots clean.
- **G-R3** EQUIVALENCE (decisive): `MaskReduce both` on targeted legs over the known-answer set —
  `StaticMeshActor_49` (MEASURED_NONZERO, ~7.25 % band / 66.8 k px) · `StaticMeshActor_73`
  (Cylinder, non-Nanite control) · `BP_SplineSpawn_C` (MEASURED_ZERO) · `SM_Ramp2` (known-Nanite:
  NOT_MEASURED via the extent discriminator) · a `IAI.Capture.MaskProbe` leg. Every armed frame:
  IDENTICAL counts AND bounds for every tag. **Bit-exact is the spec; any mismatch FAILS, no
  tolerance.**
- **G-R4** GUARDS PROVEN BOTH WAYS (G96): (a) F-6 item 5 — the probe fires the 255 detector +
  confirmation + frame discard on the NEW path; (b) the COMPARE line itself proven able to FAIL on
  a local build (deliberate mismatch injected, seen to FIRST-DIFF, reverted — the m27
  guard-proven-by-breaking-it discipline; never shipped).
- **G-R5** REGRESSION: same-seed TARGETED pre/post legs — identical event sets, identical veto
  outcomes (m27 Gate-3 shape); INERT leg `Mask 0` unchanged; one delivery-ON leg — veto outcome
  identical (G-9 orthogonality shape).
- **G-R6** ARTIFACT: annotation keyset 48/48; run_summary field-set diff EMPTY against a
  same-binary control pair (`subset_gate.py`; A63/A64 apply; the m33 `M33_CTRL_*` shape is the
  worked example).
- **G-R7** PERF — THE POINT, in two halves with different roles (A2):
  (i) STACKOBOT HALF (gates the branch commit): packaged, full pool, Mask ON — per-frame `t_wall`
  deltas from `labels.jsonl`: the armed-frame spike must collapse to the unarmed baseline.
  (ii) CONCORDE HALF (gates the MERGE, owner-run, post-delivery cook): hitch A/B by eye with Mask
  ON + the 120-frame wall span by stopwatch/`t_wall` — expected back toward ~4 s + lead-in.
  🚨 **Never read `speed_ratio` as the perf verdict on a PRE-m33 binary on that host (blind under
  the pin); post-m33 it is meaningful, and `game_clock_speed_ratio` beside it shows the regime.**

RISKS (approved as named): atomic contention at 3200×2000 (mitigated by the groupshared pre-pass;
measured at G-R7) · buffer-readback lifetime (same `InFlight` TUniquePtr ownership as today's
texture readback and as ExportTextures) · the cook retires the current container (mitigated by
A62 + G-R2).

## §4 CHAT'S VERDICT — AMENDMENTS AND STANDING RULINGS (closed; do not relitigate)

- **NUMBER: m34.** (m32 burned on the bench legs; m33 is the ratio re-key.)
- **RULING 2 — CPU-PATH LIFETIME:** `cpu`/`both` stay behind `IAI.Capture.MaskReduce` through m34
  as the named bisect and equivalence instrument; **deletion is a decision item at the NEXT cook
  after m34's** — not m34's call.
- **RULING 3 — SEQUENCING:** fold into the delivery cook REJECTED. **m34 does NOT enter the build
  the client captures with on receipt; the hitch ships as the documented limitation** (standing
  owner ruling). m34 gets its OWN post-delivery cook and office pass.
- **A1 — FINISHRUN SEMANTICS, STATED IN THE PREDICTIONS FILE BEFORE ANY LEG:** what happens to a
  buffer readback still in flight at `FinishRun` — bounded wait or loss. **If a measurement can be
  lost, the loss is LOUD (counted + warned, the B′ miss-is-loud shape), and the veto consumes only
  COMPLETE results.** One paragraph, written after reading the current drain behaviour
  (`AnomalyMaskMeasure.cpp` CollectResults + the FinishRun drain path) — investigate, then
  declare; do not guess.
- **A2 — GATE/COMMIT ORDER:** predictions file first; then G-R1..G-R6 plus G-R7's StackOBot half
  must PASS before the m34 milestone commit finalizes on the branch (**gates → commit → push**).
  G-R7's Concorde leg gates the MERGE, not the branch commit.
- **A3 — QUEUE:** the precondition (m33 committed with G-A/G-B green) **IS NOW MET** (`03b0b7a`).
  The Monday pipeline still preempts branch work at any moment.

## §5 BRANCH MECHANICS (owner instruction, verbatim intent)

- ALL m34 work on **`feature/mask-gpu-reduce`**, cut from `master` HEAD. **Commits AND pushes go
  to that branch ONLY.** `master` is reserved for the delivery line (m33 + authorized docs).
  ⚠ Interpretation on record: cut from master HEAD **at cut time** (now `4046aff`, which includes
  m33) — the branch needs m33's capture edits under it; cutting from the pre-m33 head would
  manufacture merge pain for nothing.
- **Merge m34 → master happens POST-DELIVERY and is gated on G-R7's Concorde leg passing** — no
  merge-now-validate-later.
- `feature/stencil-capture` remains untouched at `76cac74`.

## §6 SOURCE MAP — where everything lives (verified this week; line numbers at `4046aff`)

- Mask SVE: `Source/AnomalyCapture/Private/AnomalyMaskSceneViewExtension.{h,cpp}` — subscribe at
  `EPostProcessingPass::Tonemap` (`:70-75`) · mask PS dispatch + R8 `MaskRT` (`:107-121`) ·
  texture readback (`:123-124`) · discriminators captured at pass time (`:126-140`) · drain poll
  (`:183-186`) · **the CPU scan to replace (`:188-224`)** · result build, KEEP VERBATIM
  (`:226-275`) · `TakeMaskResult` (`:278-288`).
- Shader module: `Source/AnomalyShaders/` — PostConfigInit (the m26 LoadingPhase saga's Option B);
  `AnomalyShadersModule.cpp` maps `/Plugin/AnomalyInjector` → `<Plugin>/Shaders`; existing .usf
  under `Shaders/Private/`; `AnomalyShaders.Build.cs` already carries `Engine/RenderCore/RHI`
  non-Shipping (`ANOMALY_SHADERS=1`), Shipping compiles it out.
- Game-thread side (DO NOT TOUCH): `AnomalyMaskMeasure.cpp` — arm/collect/tri-state/veto.
- Console block: `AnomalyCaptureSubsystem.cpp` (the `IAI.Capture.*` cluster; StartRun echo line
  for provenance).
- Reference implementation: the WhiteMoon ExportTextures plugin (owner-supplied, not in-repo) —
  `ConvertAndExtractTexture` = compute-pack → `FRHIGPUBufferReadback` → poll in
  `PostRenderViewFamily`; the m34 design is that pattern applied to our mask.
- Harness: `CaptureBench/tools/run_leg.ps1` (A63 self-enforcing; `-Label -BankPrefix -Seed
  -MaxFrames -StallGameMs -Anomaly -Target`) · `subset_gate.py` · runbook §8 (hot-swap) and §8.6
  (FULL COOK — required here, G129).
- Known-answer expectations for G-R3 live in the m26–m30 record: `_49` 66,843–66,878 px /
  7.23–7.25 % · `_73` 48,590–48,597 px · spline 0 px ×8 · `SM_Ramp2` NOT_MEASURED every time.

## §7 FIRST ACTIONS FOR THE m34 SESSION, IN ORDER

1. Cold-start checks: `git rev-parse --short m30^{commit}` (G143 form), master == `4046aff`+,
   clean tree, staged exe `757A5DD4`, this file re-read.
2. `git checkout -b feature/mask-gpu-reduce` from master HEAD; push -u the branch.
3. Read the current FinishRun/mask drain behaviour and write the **A1 paragraph**, then the m34
   predictions file (G-R1..G-R7 verbatim from §3 + A1) — **committed to the BRANCH before any
   implementation leg runs.**
4. Implement §2's file list. Comment-free source (`_strip_comments.py` both targets of the
   stripper's reach); diffstat check (G115) before every commit.
5. Editor target build + game target build (G-R1) → A62 preservation → FULL COOK (§8.6) → boot
   gate (G-R2) → legs (G-R3..G-R6, G-R7 StackOBot half) → gates → commit → push (branch only).
6. Report to chat in a copy block; the Concorde half of G-R7 and the merge wait for the owner's
   post-delivery pass.
