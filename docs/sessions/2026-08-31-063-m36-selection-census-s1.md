# 2026-08-31 — session 063 — m36 selection census: S1 (census core) built and home-gated

**Status at close: m36 S1 is BUILT on branch `feature/selection-census` (cut from
`feature/mask-gpu-reduce` tip `784d31f`, R1) and GREEN on every S1 gate: clean compile both
targets, P-C7 (census-OFF control pair artifact-identical to the parent-tip binary), P-C6
(tag hygiene, cycle + final, plus the G96 leak-probe proof both ways). ⛔ S2 IS BLOCKED BY ITS
PRE-DECLARED PRECONDITION: m35 is NOT CLOSED on the parent branch (G-M5 ×2, G-M6, G-M7/G-M8 PIE,
G-M9 unbuilt, office pass, no tag) — S2 does not run the readback in anger until it is. NO TAG.
Selection is NOT wired: census OFF by default; ON, it MEASURES and LOGS only.**

Predictions were committed FIRST as their own docs commit (`e55c7b8`,
`docs/predictions/2026-08-31-m36-selection-census-gates.md`) — before any code existed to
measure, per the chat ruling. m36 confirmed unused before adoption (tags, tracked files, both
untracked CHAT-HANDOFF docs, CaptureBench) — R0.

## §1 What S1 is

The m26 mask pass + m34 per-tag GPU reduce, reused at SELECTION time as a rolling multi-target
CENSUS upstream of `TryFireOnce` (wiring is S2). New driver `AnomalyCensus.{h,cpp}` beside
`FAnomalyMaskMeasure`; ZERO lines changed in `AnomalyMaskSceneViewExtension.*` — the SVE already
runs on any frame with a pending arm and knows nothing about capture events (scoping answer (b),
confirmed in the build). The armed-frame measurement and the zero-only veto are UNCHANGED and
remain the backstop. `annotation.json` unmoved (P6); `run_summary` unmoved in S1 (census keys are
S2, pre-declared).

Per-candidate measurability (constraint 4 — never the H6 view-level shape):
- NANITE: per-component `!bDisallowNanite && UseNanite(ShaderPlatform) && HasValidNaniteData()` —
  the engine's own `ShouldCreateNaniteProxy()` logic REIMPLEMENTED because that method is
  `protected` on 5.1 (`StaticMeshComponent.h:769`; body at `StaticMeshComponent.cpp:1719-1736`).
  Reads the HOST's Nanite state, so a Support-Nanite-off host measures its Nanite-built meshes
  through the classic proxy — the Concorde case, and Bates (no Nanite at all) classifies nothing
  nanite.
- TRANSLUCENT (R2, EXCLUDED not fallback): every slot of every renderable component translucent
  without `IsTranslucencyWritingCustomDepth()` — the exact condition
  `FCustomDepthPassMeshProcessor::UseDefaultMaterial` skips (`CustomDepthRendering.cpp:310-338`).
  Knob `IAI.Capture.CensusTranslucent`, default ON.
- TAG_FAILED / HIDDEN: existing shapes, bounds fallback (S2).
- View-level no-pass (extent 1x1) discards the FRAME and re-queues the batch — it never
  classifies a candidate.

Tag discipline (R4 + R10, closed BY CONSTRUCTION):
- `FAnomalyStencilTagLedger` (AnomalyStencilTag.h): HostReserved / EventClaimed / CensusClaimed,
  assignable range 200..254. 255 is NEVER mintable by either allocator (R7 — the m26 event
  allocator's span-56 latent is FIXED: it now skips reserved/claimed values, `check()`s the
  range, and logs the pool at BeginRun).
- R10 reservation: `SnapshotHostReservedValues` at StartRun; reserved values echoed
  (`M36 STENCIL RESERVATION` line) and never assigned by either allocator. `IAI.Capture.
  CensusReservation 0` exists only as the P-C12 companion lever.
- A census batch's tags stay ON until COLLECTED or LOST (8-tick bound); ≤2 batches in flight,
  ledger-exclusive claims (a batch waits rather than reuses); batch cap = half the free pool so
  the second batch always has values. A fired-on/overtaken actor is never restored by the census
  (TAG-OVERTAKEN, run-end RestoreAll covers the prior).

Knobs, all G139-echoed at StartRun with provenance: `IAI.Capture.Census` (+ ini
`bSelectionCensusDefault`), `IAI.Capture.CensusFloor` (R3 — NEW `CensusMinDrawnCoveragePct`,
6.0 in v1, deliberately NOT `GMinScreenCoveragePct`), `IAI.Capture.CensusMaxAge` (R5, default
12), `IAI.Capture.CensusTranslucent`, plus gate levers `CensusReservation` / `CensusLeakProbe` /
`CensusCoArm` / `CensusHostTag` (all default-off, MUST be off in any shipping build).

## §2 The S1 gate results

- **Compile:** editor + game targets exit 0 (three builds; the only compile failure was
  `ShouldCreateNaniteProxy` being protected — fixed by reimplementation, above).
- **A44 (both encodings, on the STAGED artifact):** 12/12 new census tokens present at utf16
  (ascii=0/utf16>0, the known encoding) — scanned on the first S1 exe `FB3BE03C` while staged,
  and re-confirmed on the final S1 exe `5B33E689`. The runbook's old example control
  `IsHideTypeAnomaly` reads 0 — it is STALE (renamed at session 053, already flagged there); the
  scan's soundness is carried by the 12 non-zero new tokens, not by that control.
  Exe chain this session: `733FE83C` (m35 Build B, the parent A-side of P-C7, archived) →
  `FB3BE03C` (S1 pre-fix; NOT archived — rebuilds from the branch, its legs are banked with
  logs) → `5B33E689` (S1 final; NOT archived, same reason). Staged bench exe RESTORED to
  `733FE83C` and hash-verified at close.
- **P-C7 PASS, twice** — control pair `M36_S1CTRLA` (parent-tip `733FE83C`) vs `M36_S1CTRLB`
  (first S1 exe `FB3BE03C`), then RE-RUN against the FINAL S1 exe (`M36_S1CTRLB2`, `5B33E689`,
  post-§3-fix) so the gate is anchored to the binary that ships forward. Identical harness
  payload (diff EMPTY by construction — same invocation, only the exe differs, G184 reported):
  annotation keyset 48/48, event set identical (8 blinking events by (target, type, start/end,
  indices, manifested)), run_summary keyset 48/48 with NO census_* keys, values identical
  outside the declared run-unique set, frames 90/90. All legs A63-valid on attempt 1,
  pose-matched to each other (identical modal pose and bbox — A64). Checker
  `CaptureBench/tools/m36_s1_pc7_check.py` proven against a known answer BEFORE any verdict was
  read: A-vs-A must pass (did), A-vs-perturbed-A must fail (did), first on a banked m34 leg and
  again on the fresh pairs.
- **P-C6 PASS, both directions** — on the FINAL S1 binary, census-ON leg `M36_S1HYG2`: 24 census
  cycles, hygiene `identical=1` after every cycle (23 counted) and at the final full-diff, zero
  DIFF lines, `framesPolluted=0`, `tagOvertaken=0`, `batchesLost=0`, event-side frames discarded
  0. Companion (G96): leak-probe leg `M36_S1PROBE` — `LEAK PROBE fired` naming
  `StaticMeshActor_28/StaticMeshComponent0` tag 227, and the final hygiene check reports exactly
  `DIFF n=1 first=StaticMeshActor_28/StaticMeshComponent0 gained bRenderCustomDepth (value 227)`
  — the instrument proven able to fail. (The pre-fix leg `M36_S1HYG` also read hygiene identical
  17/17 — the §3 defect never touched hygiene.)
- Census bring-up numbers from `M36_S1HYG` (reported, not gated): 77 prefiltered candidates on
  CB_GateLevel at the settled pose, cycle length 5 ticks, 75 census frames across a 90-frame run;
  `StaticMeshActor_49` measured 66,832 px = 7.252 % drawn (the banked event-mask band is
  66,841–66,878 at this pose — the census reads the same quantity through the same instrument);
  `StaticMeshActor_73` 48,568 px = 5.270 %; 13 candidates MEASURED_ZERO; 62 of 64 NONZERO sit
  below the 6.0 floor (the drawn-coverage histogram line exists per cycle for the future floor
  decision). Event mask on the same leg: 8/8 MEASURED_NONZERO inside the banked band, 0 frames
  discarded — the census did not disturb the backstop.
- Cost sighting (P-C9 is S3; this is a sighting, not the measurement): census game-thread block
  8.2 ms TOTAL across the pre-fix run (~0.11 ms/tick, 1,594 flag flips) and 10.1 ms across the
  post-fix run (77 census frames, 24 cycles, 1,821 flag flips). Render-side cost is NOT sighted
  here — that is exactly what S3's A/B exists for.
  ⚠ **THE TAG-BLOCK MS EXCLUDES THE DEFERRED PROXY RECREATES BY CONSTRUCTION** (chat ruling,
  2026-08-31): `SetRenderCustomDepth` only QUEUES the recreate (`MarkForNeededEndOfFrameRecreate`);
  the actual destroy/create runs later inside `SendAllEndOfFrameUpdates`, outside the timed block.
  **The ~0.13 ms/tick figure is the QUEUING cost only and MUST NOT be quoted as the census's cost**
  — the recreate cost is exactly what S3's A/B with pacing OFF exists to capture. The flag-flip
  counter is the size of the excluded work, not its price.

## §3 A bring-up defect, found by the leg and fixed IN S1 (not a failed gate — no S1 gate binds it)

**⚖ CHAT RULING (2026-08-31, S1 verdict): the fix below is ACCEPTED as a design correction
(frame-honest allowed set), and its event-drain half is recorded as a DECLARED DEVIATION from
constraint 6 ("the armed-frame mask + zero-only veto: unchanged"): the event drain's assigned-tag
set now includes the census's recently-released grace set. Measured INERT with census OFF (P-C7 —
the grace set is empty when the census is inactive, and the control pair is artifact-identical);
its live half is EXERCISED IN S2 (P-C8 and its back-to-back-events companion).**

First census-ON leg read `framesPolluted=16`: the census flagged tags of its own SIBLING batch as
"assigned to no batch". Cause, from the log's own tag values (222/250 = exactly the other
batch's values): the pollution check consulted the allowed-tag set at COLLECT time, but a batch's
FRAME renders 1-2 ticks earlier — a sibling batch collected in between had already released its
tags. The frame was measured while those tags were legitimately ON. Fix: each batch snapshots
`CensusClaimed` at arm; the allowed set at collect = event tags ∪ snapshot ∪ current claims ∪
an 8-tick `RecentlyReleased` grace set ∪ host-reserved; the same grace set now feeds the event
path's `SetExtraAssignedTags`, closing the mirror-image latent (an event frame drained just after
a census release; measured 0 occurrences this leg, closed anyway). The 16 polluted frames were
DISCARDED AND RE-QUEUED by design — no verdict was ever wrong; the cost was 16 wasted frames.
Both leg attempts banked (`M36_S1HYG` pre-fix, `M36_S1HYG2` post-fix).

## §4 NOT done, named

- S2 (TryFireOnce provider, WaitCensus, run_summary census keys) — BLOCKED on m35 closing on the
  parent, per the verdict's precondition. S3 (cost A/B) after S2.
- The parent branch is UNTOUCHED; if it gains commits and m36 needs a rebase: STOP and report
  (R1). No merge, no tag, no push beyond this branch.
- Dashboard picker stays on the bounds path (R8). `GetVisibleRenderableActors/-Infos` untouched.
- The staged bench exe was RESTORED to `733FE83C` (m35 Build B) after the S1 legs and
  hash-verified — the bench remains the m35 gate environment while m35 is open. The m36-S1 exe
  is NOT archived (disk floor ~5 GB; it rebuilds from this branch's commit; its legs are banked
  with logs) — the m26 archive-gap shape, recorded not hidden.
- Determinism note (constraint 7, G140 shape) — FOR S2, stated now: wiring the census changes
  the candidate set, so the same seed will pick different targets across the S2 commit; every
  banked auto-pool run becomes non-comparable across that boundary. The S1 build changes NO
  selection behaviour (P-C7 is the receipt).

## §5 Gotcha candidates (filed at milestone end per the verdict; listed here so they survive)

1. Tag-reuse/attribution hazard: a readback consumer's "allowed set" must be the set AT THE
   MEASURED FRAME, not at collect — a sibling's release in between manufactures a false
   pollution flag that looks exactly like a host writer (§3).
2. The event allocator's span-56 latent (could mint 255 at 56 records) — fixed here; the
   residual detector was one long run away from being blinded by its own allocator.
3. Pre-work (a) split: `SetCustomDepthStencilValue` updates the proxy IN PLACE;
   `SetRenderCustomDepth` is a FULL PROXY RECREATE deferred to end-of-frame and flushed the same
   frame (`SceneRendering.cpp:4528`) — tag cost is CPU, not latency, and only on flag FLIPS.

## §6 m35 HOME GATES RUN THIS SESSION (chat order, 2026-08-31 — S2's re-specified precondition)

**Run on the PARENT's binaries with NO code change anywhere: G-M5 legs on staged `733FE83C`;
G-M6 by hash-verified swaps between the archived `7F37A4AC` (A) and `733FE83C` (B). The parent
branch itself is untouched — its gate file's results ledger is NOT edited from this branch; this
section and the report are the record until chat settles the branch strategy.**

- **G-M5 leg 4 (`M35_GM5_CYL73`) PASS** — payload derived from the banked `M34_R3_CYL73` config,
  G184 diff EMPTY on every axis. 8/8 `MEASURED_NONZERO`, maxCounts **48,590–48,597 — the banked
  band exactly, both endpoints present**, 5.2724–5.2731 %, every discard bucket zero,
  `MASK-REDUCE COMPARE` **29 IDENTICAL / 0 FIRST-DIFF**, guard/clamp 0, 90 frames.
- **G-M5 leg 5 (`M35_GM5_PROBE49`) PASS** — ONE deliberate known-hidden probe arm;
  `detector255Fired=1`, `confirmationReadHidden=1`, frame bucketed PROBE,
  `run_summary.mask_probe_arms=1`; the other 8 events 8/8 `MEASURED_NONZERO`, COMPARE 29/0,
  guard/clamp 0. **G-M5 is now 5/5.**
- **G-M7 (`M35_GM7_MW_LB` + `M35_GM7_MW_PLAIN`) PASS as written — via the §13 "untested and
  cheap" route: PACKAGED MainWorld supplies a camera-bearing view target and
  `IAI.Bench.Letterbox` APPLIED** (`bConstrainAspectRatio 0->1, aspect 1.7778->2.3900` on the
  spawn-pad intro camera) — the PIE-only constraint is RETIRED BY MEASUREMENT for this lever.
  Both legs: `capture_path=backbuffer`, 90 frames, 8 events, guard/clamp silent,
  `bufferHeight == picture height` (720), frames are the picture's size, and the frames were
  LOOKED AT (bars top/bottom baked into the letterboxed frames — 157–173 near-black rows vs 0 on
  the plain leg — game UI present, no pitch/smear artifacts). ⚠ **SCOPE, stated:** the bench
  backbuffer grab is WINDOW-scoped, so its rect stays (0,0)–(1280,720) with the bars INSIDE the
  picture — a non-zero-origin backbuffer case is not producible on this bench; the non-zero
  origins are exercised on the SVE path (G-M1's Y, G-M8's X below). Bates' non-zero-origin
  condition arises host-side.
- **G-M8 (`M35_GM8_MW_PILLAR`) PASS, with its shape stated** — SVE path, `Letterbox 1.0`:
  `READBACK-LAYOUT rect=(280,0)-(1000,720) picture=720x720` — **`rect_min_x=280`, the first
  non-zero X origin ever exercised.** Column checker (`m35_gm8_column_check.py`) proven BOTH ways
  first (known frame: 0 near-black cols, no collapsed band, left col alive; synthetic 40-px black
  pillar: caught) then read: PASS on the pillarboxed frames (frame 0: 0 near-black cols, left col
  50.01, no collapsed band); frame looked at — bar-free coherent picture. ⚠ **The leg is
  HETEROGENEOUS:** MainWorld's intro-camera→Bot switch drops the constraint mid-run — 16 frames
  at 720×720, then the view blend walks the aspect through EIGHT more widths (782..1218, i.e.
  eight further distinct non-zero X origins), then 66 frames at 1280×720 — with guard/clamp
  SILENT and every written frame matching its own rect throughout. That is a DYNAMIC-rect
  robustness result beyond the pre-declared static leg; the uniform static pillarbox leg remains
  obtainable in PIE (console re-issue after possession) if chat wants it — flagged, not run.
  ⚠ Artifact note: `annotation.video.resolution` reads 720×720 (m28: first written frame) while
  the session is mixed-size — the m28 contract meeting a mid-run rect change; bench-lever
  induced; labels carry per-frame sizes.
  ⚠ Checker datum note: the owner's POINT values (~50 left / ~87 right) did not reproduce on the
  CB un-letterboxed frame by any channel method tried (reads 85.75/141.67); the BINDING
  categorical conditions (0 near-black cols, min 5.0, no collapsed band) reproduced exactly, and
  left≈50 appears on the MainWorld pillar frames — the datum's venue was likely a
  MainWorld-shaped frame. Reported, not smoothed.
- **G-M6 PRIOR TAKEN** (instrument, not pass/fail) — declared discard leg first, then
  **A,B,B,A**, hashes verified at every swap, pacing OFF, mask ON, same map/seed/config
  (blinking `_49`, seed 777, CB_GateLevel, 90 frames): per-captured-frame
  **A₁ 12.0365 · B₁ 11.6327 · B₂ 11.5437 · A₂ 11.5744 ms** (per-MP 13.06/12.62/12.53/12.56);
  A mean 11.8055 (spread 0.4621), B mean 11.5882 (spread 0.0890); **B−A = −0.2172 ms/frame
  (−0.2357 ms/MP) — NOT larger than the within-build spread ⇒ BELOW THE RESOLUTION OF THIS
  INSTRUMENT** (G169; never "no cost"). The warm-up gradient landed inside A's own spread (A₁
  first and slowest) — the A,B,B,A design doing its job (G186). Concorde 6.4 MP figure would be
  −1.51 ms/frame and is an EXTRAPOLATION under a linear-scaling assumption. NUMBERS ONLY.
- **G-M9 — NOT BUILT, and it CANNOT be harness-only:** the second old-form readback + byte
  compare live inside the SVE capture path (`AnomalySveCapturer`/`AnomalySceneViewExtension`,
  both files the m35 WIP itself edits) plus a cvar in `AnomalyCaptureSubsystem` — ALL PARENT
  (AnomalyCapture module) code. Per the chat ruling's branch: **STOPPED; chat rules
  twin-vs-rebase.** m35 is therefore NOT yet home-closed (everything runnable is green; G-M9 is
  the only open home item) and S2 remains blocked on that ruling.

Evidence: `M35_GM5_CYL73` · `M35_GM5_PROBE49` · `M35_GM7_MW_LB` · `M35_GM7_MW_PLAIN` ·
`M35_GM8_MW_PILLAR` · `M35_GM6_DISCARD` (declared discard) · `M35_GM6_A1/B1/B2/A2` — all banked
with their process logs matched by session id. New harness tools:
`CaptureBench/tools/m35_gm8_column_check.py`, `CaptureBench/tools/m35_gm6_prior.py`.

S2 prep recorded now: bench floor for S2 legs = `IAI.Capture.CensusFloor 0.5` (from the S1HYG2
histogram: ~14 candidates ≥0.5 % vs 9–10 at 1.0 and 2 at 6.0 — the ≥10-pool rule), default stays
6.0; run_summary census keys will also carry `census_batches_lost` and `census_frames_polluted`;
the census needs a per-batch COLLECT log line so the grace-window latency stat (worst
tag→collect ticks vs the 8-tick constant) can be READ rather than inferred — S1's logs cannot
supply it and it is not reconstructed (A60).

## §7 Evidence bank (S1)

`M36_S1CTRLA` · `M36_S1CTRLB` (+ leg log) · `M36_S1HYG` (pre-fix, + leg log) · `M36_S1HYG2`
(post-fix, + leg log) · `M36_S1PROBE` (leak-probe, + leg log) — all under
`_bench_sessions_bank\`, every attempt A63-logged by the harness. Checker:
`CaptureBench/tools/m36_s1_pc7_check.py`; A44 scanner: `CaptureBench/tools/m36_a44_scan.ps1`.
