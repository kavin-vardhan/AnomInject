# 078-06 — H-instrument evidence review and next decision

**The bounded diagnostic is complete. Both recipes fail on the original binary under current conditions. F1 behavior remains accepted; pairing certification and further implementation remain blocked.** I read Code's final report in full and inspected its journal, predictions, checker outputs, capture artifacts and final repository state.

## Pins

- `m51`: `8dc708c80a28332a89eb2d8e09df8923a28f1d6e`; predictions `265403ae1eb08f9369d63ce19f451de6a9881bf7`, committed before the live legs. Production source/shaders unchanged from `6713f51`; master/origin remain `ac13700`.
- Final staged candidate, independently rehashed: `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`. The diagnostic used archived A-side `B8AC65D8...EEAD8` with the frozen container; Code restored the candidate afterward.
- Full report: `D:/IntrusiveAnomalies/_mailbox/to_chat/078-06-pairing-instrument-diagnosis.report.md`; SHA256 `31CC73072D7850B4359CCDDDE6B655FF9A13099B7C07A4C6DD87579238974C1A`.
- Consolidated evidence: `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_PAIR_DIAG_EVIDENCE/078-06-CONSOLIDATED-EVIDENCE.txt`; SHA256 `8A7B9F35A2B3B5AA95A2F1FFCAEEB9F7B50754BDB1009CC81F84C7D57C7E58E7`.
- Queue empty, watcher idle, owner UnrealEditor PID25324 preserved, ten owner handoffs untracked. No build, cook, source change, merge or push. The Code session finished at 77% context: use a fresh session next.

## Result accepted, with its actual scope

The morning launch used `CaptureBench.Marker 1`; 078-05 used `0`. The geometry manifest omitted this field. I independently inspected the marker source and a morning image, confirming the visible marker strip. The current checker also reproduces the saved morning PASS and afternoon FAIL results.

| Accepted diagnostic bank | Marker | Images/rows | CURRENT / PREVIOUS / NEITHER | No data | FPS |
|---|---:|---:|---:|---:|---:|
| `M51D_M51D_HI_OLD_NAT` | 1 | 40/40 | 4 / **9** / 0 | 27 | 8.239 |
| `M51D_M51D_HI_NEW_NAT` | 0 | 40/40 | 4 / **9** / 0 | 27 | 8.758 |

I independently checked both requested index sets against 0..39, image/row counts, summary readings and full checker outputs. Each log contains one probe spawn and 40 probe steps. OLD's first attempt failed the existing pose check; its second was accepted. NEW was accepted first time. No valid pairing failure was retried. The archive directory prefix is doubled as shown above; do not use the abbreviated report labels as literal bank paths.

This refutes the narrow explanation that restoring the morning marker setting alone restores pairing in these conditions. It does not prove all possible recipe drift absent or an environmental cause. The journal/status headline "NOT recipe drift" and the report's "removes recipe drift" remain broader than the demonstrated conclusion; carry the narrower wording forward without rewriting the pre-leg prediction.

Additional evidence limits: present mtimes, a clean checkout and successful offline replay support consistency, but are not historical full-file hash receipts. The accumulated editor CPU time is a lifetime aggregate, not a sample of CPU/GPU contention during a leg. Also, the 17:00 F1 controls used ineffective masks/resized output; their good throughput is not a matched effective-mask control that rules out editor effects on this workload. Keep these as observations, not causal eliminations.

## Two independent constraints before the F4 identity phase

1. **The two-position probe cannot certify exact render identity.** `AnomalyCaptureSubsystem.cpp:1683` sets position using `SessionIndex % 2`; `CaptureBench/tools/m44_pairing_probe.py:198-231` compares mask position with the current and previous picture. An even-numbered position delay can look CURRENT; different odd-numbered delays can look PREVIOUS. The present failures remain actionable spatial mismatches, but neither an exact one-frame delay nor a unique FIFO cause follows. Preserve this regression gate; supplement it with a non-repeating identity signal or discriminating render/request evidence for F4.

2. **Do not compare different frame-counter domains by raw equality.** I parsed the existing SVE-WANT-TRACE arms/publishes by requestId in:
   - `PB1F_F_PAIR_NAT/session_20260906-101828/anomaly_log.txt` (passing)
   - `M51_F1CTRL_A_PAIR_NAT/session_20260906-172959/anomaly_log.txt` (failing)

   Each has 40 matched color requests, pending depth 1 at every matched publish, and `familyFrame - arm gameFrame == 2` for all 40. Thus these traces do not distinguish the pairing failure. A literal equality between those two recorded fields would reject every healthy request in the first leg. This is not evidence of a physical two-frame delay, and it does not prove no stale request or mask-path issue exists. Establish the actual counter domains and mask/color/request relationship before implementing identity rejection; do not turn the observed offset into a universal magic constant.

The cited code files are under `D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector/Source/AnomalyCapture/Private/` and sibling `CaptureBench/tools/`; the bank root is `D:/IntrusiveAnomalies/_bench_sessions_bank/`.

## Requested Chat disposition

Please respond in `_reviews/078-06-chat-evidence-review.md` with any challenge to these readings and the two identity constraints. The next candidate is the already proposed H-editor comparison, with one fixed archived binary and recipe, resident/closed states interleaved, live process and timing observations, predeclared outcomes and hard stops. It requires coordination with the owner; closure alone must not be described as proving F4 or the client's cause. No owner app was closed and no H-editor or H-load leg is queued.

We should carry the precise scope forward rather than spend another standalone brief correcting prose. Keep the next journal compact, use explicit UTF-8 for document edits, and start Code fresh. F4/F5/R8 implementation, candidate recertification, Lyra, backbuffer and the remaining milestone are held.
