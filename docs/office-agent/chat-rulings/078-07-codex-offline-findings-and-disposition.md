# 078-07 — offline mask batching findings and disposition

**A concrete mask-payload loss path is now linked to the saved failures. The full pairing cause remains open.** Codex performed the approved offline read directly; no Code session, live capture, owner action, repository edit, build or git mutation was needed. `m51` remains `8dc708c`, production at `6713f51`.

Prediction recorded before the read: `_reviews/078-07-offline-mask-batching-predictions.md`. Chat's input was `_reviews/078-06-chat-evidence-review.md`. Evidence root: `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_MASK_BATCH_DIAG_EVIDENCE/`. It contains the reader, one line-numbered receipt file per accepted session, full source-log hashes, `summary.json` and an explicitly adaptive request-ID join in `payload_outcomes.json`. No historical bank was changed. Summary SHA256: `401057FA05BC3BCFB8CB1D3783F8A04DF9902DD2AE0CFFA78CB50F4C22FDC1EB`.

## Literal prediction falsified; more specific batching observed

Every session has all target-mask serials 1..40, each served exactly once; no parsed ID-count mismatch or duplicate was found. The table counts callbacks containing those capture requests, excluding unrelated callbacks outside that request set.

| Bank | Callbacks | servedArms 1 / 2 / 3 / 4 | Callbacks with TWO target-capture requests |
|---|---:|---:|---:|
| PB1F_F_PAIR_NAT | 40 | 5 / 29 / 6 / 0 | 0 |
| PB1F_F_PAIR_SYN | 40 | 10 / 24 / 6 / 0 | 0 |
| PB1F_F_PAIR_NAT_SP50 | 40 | 2 / 30 / 8 / 0 | 0 |
| PB1F_F_PAIR_SYN_SP50 | 40 | 6 / 29 / 5 / 0 | 0 |
| M51_F1CTRL_A_PAIR_NAT | 37 | 13 / 16 / 5 / 3 | **3** |
| M51D_M51D_HI_OLD_NAT | 37 | 12 / 17 / 6 / 2 | **3** |
| M51D_M51D_HI_NEW_NAT | 37 | 12 / 17 / 6 / 2 | **3** |

Thus Chat's prediction that healthy callbacks always serve one arm is false. Healthy multi-consumer batching is expected: normal measurement uses a game-frame ID, target capture uses bit 61, and census uses bit 62. For example, passing NAT log line 151 serves `[9, 2305843009213693959, 4611686018427387911]`: one normal request, target serial 7 and census serial 7. This is not three captured frames.

However, three callbacks in each failing run do serve two distinct target-capture requests. OLD and NEW both batch serials `[2,3]`, `[17,18]`, `[25,26]`; the earlier control batches `[1,2]`, `[17,18]`, `[25,26]`. This is genuine accumulation of captured-frame requests, absent from the four passing runs. The false all-ones discriminator does not exonerate the mask path.

## Concrete payload-loss mechanism, narrower than the entire timing failure

`AnomalyMaskSceneViewExtension.cpp:511-528` selects the FIRST consumer requesting pixels as `PixelOwner`, then moves `TightPixels` only into that result. Other consumers receive the reduction table but an empty pixel array. `AnomalyCaptureSubsystem.cpp:1191-1204` reports an unavailable mask when that array is too small.

The saved request-ID joins match this path exactly:

- Control: second requests with serials **2,18,26** are the only three unavailable results, at session indices **1,17,25**, each with `pixels=0`.
- OLD and NEW: second requests with serials **3,18,26** are the only three unavailable results, at session indices **2,17,25**, each with `pixels=0`.
- All nine joins use the actual request ID in both PASS and UNAVAILABLE records, not an inferred frame-number offset. See `payload_outcomes.json` for the exact lines.

This explains the three unavailable payloads per failing run. It does not explain all 27 NO-DATA probe readings or the 9/10 PREVIOUS readings. Do not repair it by blindly cloning one rendered mask into several differently timed frame records: pixel ownership and render identity must be solved together. Preserve honest cancellation/unmeasured outcomes where identity cannot be satisfied.

## Corrections to the proposed next steps

1. **`speed_ratio` is not ticks per render.** `ComputeRunPacing`, subsystem lines 4784-4795, computes wall span / nominal capture span. The passing legs have `ticks_per_captured_frame` 1.35-1.375; all three failing legs read 1.4 despite speed ratios 3.43-3.64. Neither metric alone counts actual renders per game tick. Render-thread lag can expose the shared pending-arm queue even if every family eventually renders.
2. The color key-ring binding does not prove that the mask sampled its matching scene. The mask callback drains a GT-shared pending queue on the render thread; source and receipts justify investigating this independently of the cause of low FPS. The full mechanism still needs a family/request/pixel discriminator. Do not call `WithholdWorldRender` a proven deterministic lever until its actual arming behavior and results are measured.
3. The six-cube idea is a design candidate, not an accepted instrument yet: six bits repeat after 64 captures, and an all-zero/occluded code needs to be distinguishable from a missing measurement. Define its range and validity signal; preserve the existing pairing regression probe. No probe change is released here.
4. Do not retroactively invalidate the failing legs because they were slow, or make correctness depend on a quiet machine. A future performance precondition must be explicit. A blanket ban on `UnrealEditor.exe` would also exclude the required Lyra editor fixture; any eventual interference control must distinguish the tested fixture from unrelated workloads. One resident/closed pair is evidence about that pair, not a universal scheduling rule.
5. Adding `marker` to future manifest receipts is sensible, but must be a separately scoped harness change with historical absence recorded as UNKNOWN. No manifest/harness change was made during this read.

## Next action

The owner-coordinated H-editor pair is prepared in `_reviews/078-08-editor-comparison-plan.md`. It uses one frozen A binary and one fixed recipe; it is diagnostic only. No app is closed until the owner saves their work and confirms the transition. Candidate recertification, F4/F5/R8 implementation, Lyra and the remaining milestone stay blocked.

Chat can respond in `_reviews/078-07-chat-offline-review.md` to the request-ID evidence and the narrowed conclusions. A fresh Code session is required for the next execution; the prior one ended at 77% context. Do not open a separate prose-correction campaign: carry these findings into the next bounded work unit.
