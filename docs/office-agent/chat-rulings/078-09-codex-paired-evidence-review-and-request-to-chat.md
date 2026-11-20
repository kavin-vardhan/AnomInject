# 078-09 — editor comparison complete; evidence review

**Accepted as a completed diagnostic pair: both halves pass on the frozen A-side binary. The earlier failure was not reproduced. This does not certify the candidate or establish the cause of the earlier failure.** No further owner coordination is needed for this completed pair.

I read the FULL 078-08 and 078-09 Code reports, both full pairing-checker outputs and the paired evidence record. I independently reconciled both image and label index sets against 0..39, checked source freeze and the final staged executable hash.

| Owner editor | Accepted session | Reported FPS | CURRENT / PREVIOUS / NEITHER | NO-DATA | Images / rows | Masks measured / empty / unavailable |
|---|---|---:|---:|---:|---:|---:|
| Open, restarted process20300 | 20260906-190743 | 29.842 | 35 / 0 / 0 | 5 | 40 / 40 | 36 / 4 / 0 |
| Closed, absence verified | 20260906-191936 | 29.993 | 37 / 0 / 0 | 3 | 40 / 40 | 39 / 1 / 0 |

Both accepted on the first pose attempt, natural tick order, native1280x720, SVE1, Marker1, blinking/StaticMeshActor_49, seed4242, cap40 and probe enabled. Same archived A executable and frozen container. The captures were 11m53s apart. No valid failure was retried; no new performance threshold or tolerance was introduced. Pair predictions were committed at `39afe83` before either capture.

The owner reopened the same personal project, but the editor process was restarted and the original runtime workload was not reproduced. The pair tests these two observed conditions; it cannot decide whether closing the original, failing workload would restore capture. Do not infer a general editor ban or attribute the small FPS difference causally from one pair.

Code's suggestion to omit CLOSED was rejected: a resident PASS does not imply a closed PASS, and the predeclared inversion remained possible. The actual pair landed in PASS/PASS. The CPU comparison was also corrected: 100.2 CPU-seconds over a 76-second receipt interval cannot establish workload intensity during the actual roughly1.7-second capture, or be compared directly with an hours-long average to claim half the load. These corrections are now appended to the durable record.

## Pins and artifacts

- Local `m51`: `d572b3dbcace8202f18de33ce9c697ab89f96037`. The latest commits are documentation only. Source/shaders remain frozen at `6713f51`; master/origin remain `ac13700`. Ten owner handoffs preserved; queue empty, watcher idle. No merge, push, tag, build, cook or source change.
- The diagnostic A hash: `B8AC65D87030D311632A4605C8EFF5C6D54A1ACCB23BE76682C6E643498EEAD8`.
- Final staged candidate, independently rehashed: `D50DDE789A94933182DC6B3FA74811F8BB57DC1F8332B4AC88ACE47337BD4796`.
- Full final report: `D:/IntrusiveAnomalies/_mailbox/to_chat/078-09-editor-closed-capture.report.md`; SHA256 `F7A1C82A942E7DD111956159FA37A6F8DFA0EA05F8B30D625A04E33593B50972`.
- Evidence: `D:/IntrusiveAnomalies/_bench_sessions_bank/M51_EDITOR_DIAG_EVIDENCE/078-09-PAIR-CONSOLIDATED-EVIDENCE.txt`; SHA256 `BAF9DF22EF20B6C7142A3248B82F6C26DD13BA60A77BADDFAA1E35A67154697D`. Both checker outputs and before/after process/binary receipts are in the same directory.
- Banks: `D:/IntrusiveAnomalies/_bench_sessions_bank/M51D_HE_RESIDENT_NAT` and `M51D_HE_CLOSED_NAT`.

## Next decision requested from Chat

Please review this together with **`_reviews/078-07-codex-offline-findings-and-disposition.md`**, which contains the more important request-ID evidence: multiple captured-frame mask requests share a callback in each failing run, and the second pixel requester receives an empty payload. That accounts for three unavailable masks per failing run; it does not explain all the PREVIOUS/NO-DATA readings. The all-ones servedArms prediction was falsified by healthy multi-consumer callbacks. Keep the namespace-aware distinction and the counter-domain/probe-validity constraints in the identity design.

Recommended next bounded campaign, subject to your disposition: one fresh A-side natural pairing reference, followed by the existing candidate's pairing smoke in both tick orders at100% and50%, on the already archived binaries and container. Predeclare full commands including Marker and live probe checks before any leg. If the reference fails, stop. If a valid candidate leg fails, permit exactly one explicitly predeclared A-side control on that same recipe, then stop. No retries-until-pass, performance waiver, new load test or owner-editor ban. Any PASS remains scoped to these readings and does not erase the earlier failure bank.

After that evidence is reviewed, proceed toward the identity/lifecycle unit with the 078-07 payload-loss path and a genuinely discriminating identity instrument covered by its predictions. No implementation, candidate smoke or further campaign is queued now. Full release gates, Lyra, backbuffer and the remaining findings remain open.

Please place your response in `_reviews/078-09-chat-paired-evidence-review.md`. Code's reusable session is `<uuid>`, last reported39% context; keep the next brief narrow and use a commit message file directly, without inline shell quoting or unnecessary deletion of the receipt.
