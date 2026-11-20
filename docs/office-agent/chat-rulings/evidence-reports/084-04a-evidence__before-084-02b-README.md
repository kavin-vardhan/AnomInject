# 084-03 — part B harness (m52 label-sync, baseline vs fix)

Written by 084-02 before any leg ran; corrected by 084-02b (two-sided gate (i), four on-disk can-fail cases, the residual measurement, the scoped and joined purity gate, fix archive pinned to `m52fix-FD621B27`). Files: `084-03-common.py`, `084-03-lib.py`, `084-03-window.py`, `084-03-run.ps1`, `084-03-gateselftest.py` (offline known-answer checks of the gates)
(the S1 launcher `082-07-run.ps1` byte-for-byte except one added `-ExtraArgs` parameter, used for `-IAIBench`).

## Invocation (night window, PC idle)
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live --with-optional   # adds B7_BASE, B7_FIX, B9_FIX
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py gates                   # re-evaluate accepted legs only
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py dry                     # stubbed launch, banked stand-ins
```
Evidence: `_reviews\084-03-evidence\run\` (ledger, receipts, `report.json`, `report.md`). Bank: `_bench_sessions_bank\M52FIX_<leg>_A<n>`,
plus a hardlink alias `M52FIX_<leg>` for each accepted attempt.

## Sequence and safety
1. Preflight:
   - the pinned fix archive `_binary_baselines\m52fix-FD621B27` (084-02b corrected build) exists and its name matches its hash;
   - `fix/m52-label-timing` == origin;
   - the main checkout is on `m51` `53bf725`;
   - the staged set is M53, BASELINE or FIX;
   - E: ≥ 50 GB and D: ≥ 15 GB;
   - no engine or game process of ours is running.
2. Back up the staged m53 set to `E:\IA_BuildCache\_r84_m53_staged_backup` and verify all six hashes.
3. Stage BASELINE (exe `E0BE6F0A` plus container `67EA1FE0`, all six files) with a hash receipt, then run the baseline legs.
4. Stage FIX (exe `FD621B27`, same container) with a receipt, then run the fix legs.
5. **Always** restore M53 in a `finally` block and verify all six hashes (receipt `99-restore-m53`).
6. Evaluate the gates and write the report.

Before each attempt: the person-present gate (60 s idle) and the quiet check. The runner kills only its own process id.
An attempt is invalid on protocol, focus, frame-count or PNG problems, with at most 3 invalid attempts per leg (NOT-RUN
after that). A foreign editor or a busy bench stops the window.

## Legs (MainWorld, `IAI.Capture.Config 2 4 8 30 0`, auto-exposure pinned off, AA and Lumen as delivered)
| leg | build | what | frames |
|---|---|---|---|
| B0_BASE / B0_FIX | both | targeted rock `StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803` | 1200 |
| B5_BASE / B5_FIX | both | B0 plus `IAI.Bench.StuckMipNoHold 1`: the null that calibrates the threshold | 1200 |
| B3_BASE / B3_FIX | both | auto-pool with only `stuck_low_mip` enabled, seed 4242 | 1200 |
| B9_BASE | baseline | B0 plus `r.AntiAliasingMethod 0`: is the slow recovery temporal history? | 1200 |
| B0L_FIX | fix | B0 plus `IAI.Bench.StuckMipLegacyTiming 1`: **can-fail, must FAIL sync** | 1200 |
| P0_FIX | fix | targeted `BP_SpawnPad_C_UAID_B42E9936F54253D500_1492231360`: the purity rule | 300 |
| P1_FIX | fix | P0 plus `IAI.Bench.StuckMipLegacyPurity 1`: **can-fail, must FAIL purity** | 300 |
| B9_FIX | fix | B0 with AA off: the residual WITHOUT anti-aliasing on the fix build (required since 084-02b) | 1200 |
| B7_* (optional) | both | B0 plus `IAI.Bench.SynthTickOrder 1` | 1200 |

Fix legs launch with `-IAIBench`.

## Gates
- **(i) Sync on measurable content.** The oracle, per event:
  - ROI: the target's projected bbox, shrunk 15 % per side, taken from the null leg when the null has that target;
  - S: mean horizontal plus vertical grey gradient in the ROI;
  - reference: the flat pre-event level, the median of the 8 frames around the event's first row;
  - threshold: `max(5·σ, 2 %·pre, null worst drop)`. σ is the robust MAD of the null's high-frequency residual after a
    9-frame rolling median. The null worst drop is the null's own largest relative drop below its pre-event level under
    the same procedure.

  A frame below `pre − thr` is pixel-visible. Measurable means depth ≥ 3·thr. An event is excluded if the camera moved
  more than 0.5 cm or 0.05° from 6 frames before the event to 10 frames after it. The pixel window is [first_vis ..
  last_vis]: first_vis = the first of 2 consecutive visible frames, last_vis = the last visible frame before a clean
  suffix of 3 frames. **TWO-SIDED (084-02b, ruling decision 2): PASS iff the labelled window starts AT first_vis and ends
  AT last_vis, with no unlabelled visible frame inside it and no labelled frame outside it** (over-labelling after the
  last visible frame fails, as does labelling before the first). Isolated visible frames outside the pixel window are
  reported (`isolated_visible_outside_window`), not required. On the fix build, no labelled frame may precede the first
  render-held frame either. An edge without the 3-frame clean suffix, or with a frame-index gap at the edge, is
  **censored (unmeasured), not passed**.
- **(ii) Window (fix only):** label window == render-record window (`stuck_mip.render_state` ∈ held/unknown/settle_tail),
  no mask outside the label, and every labelled frame with `target_pixels > 0` has the mask.
- **(iii) Purity (fix only):** every m52 fire's independent `TEXUSERS` line reads `users=1 target_users=1` for every held
  texture, with `scope=all_loaded_levels` (all loaded levels, active or not, registered or not; else `FAIL-SCOPE`), and
  the enumerated textures JOIN per fire to that fire's `HOLD` lines (else `FAIL-JOIN`). `PASS-REFUSED` means the rule
  refused every shared texture and nothing was held. `HOLD CONTAMINATED` lines (a new user mid-hold) are listed.
- **Null:** 0 labelled or render-held rows, with at least one settled null event available to calibrate from.

## Can-fail proofs
- **Doctored labels (offline, on real banked pixels):** each variant is a real session COPY on disk (labels.jsonl rewritten,
  mask PNGs written, frames = the banked pixels), judged against a FROZEN pixel oracle measured once on the original. The
  repaired window [first_vis..last_vis] must PASS (i) and (ii); **onset +2, offset −5, early-onset over-label (−2) and
  late-offset over-label (+4, with the record extended as settle_tail = Codex's counterexample, which (ii) alone passes)
  must all FAIL the two-sided pixel gate (i).**
- **B0L_FIX:** today's timing restored. (i) must FAIL.
- **P1_FIX:** today's purity rule restored. (iii) must FAIL.

## Predeclared expectations (written before any leg)
- B0_BASE and B3_BASE: (i) FAIL. Blur 1 frame before the label, and 5–14 unlabelled visible frames after it (084-01 §5).
- B5_*: null PASS.
- B0_FIX and B3_FIX: (ii) PASS and (iii) PASS. **(i) is the open question and MAY FAIL tonight by exactly the residual
  (ruling decision 2: expected and reported, not hidden).** The residual is measured per event as
  `residual_after_record` = last pixel-visible frame − last record-member frame (signed; positive = pixels still visible
  after the record is back at baseline) and `residual_before_record` = first record-held frame − first pixel-visible
  frame, on B0_FIX/B3_FIX (AA as delivered) and B9_FIX (AA off). The report states the **closure rule** that would give
  0 on both edges: a constant settle tail if the residual is one constant, otherwise that no constant works and an
  in-engine settling detector is needed. `IAI.StuckMip.SettleTailFrames` stays 0 in the build.
- B0L_FIX: (i) FAIL. P1_FIX: (iii) FAIL. P0_FIX: (iii) PASS or PASS-REFUSED.

## Premise audit (per leg, in the report)
- focus time and whole-leg foreground;
- camera movement per event;
- streamer echoes from the log (`r.TextureStreaming`, `UseAllMips`, `FramesForFullUpdate`, `OverlapAssetAndLevelTicks`,
  `r.AntiAliasingMethod`, `sg.TextureQuality`, `IAI.StuckMip.SettleTailFrames`), and the anomaly's own precondition
  read-back;
- the run log present;
- the label-source echo;
- `-IAIBench` registration;
- the dither phase (`frame_index % 8`) at both edges;
- censored edges.

## Expected bench time
About 12 required legs (B9_FIX is required since 084-02b) of 60–120 s capture plus startup, with banking at about 1.6 GB per 1200-frame leg (≈ 20 GB on D:).
Two staging swaps and the restore take about 1 minute each. Gate analysis is about 1–2 minutes per leg. **Estimate: 50–65 minutes; up to about 95 with pose or focus retries.**
