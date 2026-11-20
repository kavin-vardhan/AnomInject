# REPORT for 081-26-m55-predicate-correction-and-audit.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-26-m55-predicate-correction-and-audit.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-25 11:52:38 | finished: 2026-09-25 12:25:09 | duration: 33 min | exit code: 0
context at last turn: 629511 tokens (63% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-26-m55-predicate-correction-and-audit.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

The G8 check is fixed and proven both ways. `C1_G8_N` now passes when re-checked from its saved evidence (it was not re-run), the boundary file is re-issued, and the dry run passes 134 of 134. But the audit found two more checks whose **predictions** don't match what the build writes by design. The ruling says I must not change those, and together they would stop c1 again at `C1_G2_N` and c2 at `C2_G7_N`. So the GREEN condition is not met.

## 2. Plain summary for the owner
- The faulty test check from last run is fixed. The test that stopped now counts as passed, using the saved results. Nothing was re-run.
- I checked every other test check the same way. Two more are wrong for a few special tests: they expect "zero" or "one" where the program correctly writes "one or more" or "two".
- I did not change those two. Chat has to decide them, otherwise the next PC session would stop again at its 5th test.
- Nothing ran on the PC. No game, no editor, no build.

## 3. Corrected clause and two-way proof
- **Clause** (`081-23-lib.py:113`, was `:111`; helper `receipt_without_delivery`, `:90-91`): SI8's receipt has no delivery side, meaning `t_drain_ms == -1` and `view_family_id == 0` and `serving_token == 0` and `format == 0` and `rect == []` and `extent == []`. The message keeps its `build-3 D:` prefix.
- **Proof:** `_reviews\081-26-evidence\g8-proof.json`, 38 of 38 checks as expected.
- **PASS on:**
  - `C1_G8_N_A1` SI8.
  - Stage-1 SI8 rows: Lyra V6 G15 N/S, Lyra V5 G4 N/S, StackOBot V6 G15 N/S, StackOBot V6 G4 N/S.
  - The old clause fails every one of these rows.
- **FAIL on:**
  - Six mutated copies of SI8's receipt, each with exactly one delivery field copied from SI7's real receipt.
  - The real SI7 and SI9 receipts, an absent receipt, and an empty receipt.
  - The full G8 evaluator on each mutated session copy fails on this clause only.
- **Whole bank:** 374 sessions, 74,257 receipts. All 801 receipts with `t_drain_ms -1` carry the complete stub. All 73,456 delivered receipts fail the clause. No other shape exists.

## 4. Audit table
Full version, including the `081-08-audit.py` invariants, is in `_reviews\081-26-evidence\audit.md`. Source files are on the branch: ACS = AnomalyChangeStage.cpp, ACM = AnomalyChangeMeasurement.cpp, SVE = AnomalySceneViewExtension.cpp, MSVE = AnomalyMaskSceneViewExtension.cpp, SUB = AnomalyCaptureSubsystem.cpp.

| predicate (lib line) | prediction | serialised form | written at | banked stand-in | verdict |
|---|---|---|---|---|---|
| :96 all gates | every issued index finalised | one Verbose `PAIR si= valid= reason=` line per index; `change_issued` | ACS:583, :120 | C1_G8_N_A1 | OK |
| :98-100 G1 | two-ready drain, real receipts, reverse order | counters; delivered `t_drain_ms` ≥ 0 (default -1 = stub) | SVC:382, ACS:501 | V6 G1 | OK |
| :102-103 G2 | arm dropped; SI8 OOT, SI9 predecessor_missing | counters, trace, F5 row 1 | SUB:4498, ACS:469/534 | V6 G2 (pre-build-3 shape) | OK |
| :105-107 G3–G6 | current_undelivered / predecessor_undelivered / mask_payload_missing | trace, F5 rows 3/5 | ACS:321-338/535/542 | V6 | OK |
| :108 G7 | old-generation drains rejected, SI9 epoch_reset | `change_epoch_rejected_colour_drain` / `_mask_drain` | ACS:147, SVC:372, MSVE:608 | V6 G7 | OK |
| :110-112 G8 | view_rejected ≥ 1, throwaway 1, SI8/SI9 reasons, PNG absent | counters, trace, missing file | SVE:89/110/125, SUB:4471 | C1_G8_N_A1 | OK |
| **:113 G8** | **SI8 no receipt** | **stub with sentinels; the key is always present** | **ACS:493-524, :563** | **C1_G8_N_A1, Stage-1 G15/G4, whole bank** | **ENCODING-DEFECT, fixed** |
| :114-120 G9–G13 | extent_mismatch / budget_exceeded / late+frozen / prev 6 / unregistered | trace, counters, LATE-FROZEN line | ACS:541/336/154/554/107, SUB:5652 | V6 | OK |
| :122-123 G15 | cancel/defer counters, closure_timeout, epoch_reset, 89 labels | counters | SUB:4488, SVE:95 | V6 G15 | OK |
| :141-180 case predicates | delay / depth / empty / motion / 1080 / count / wall | row fields, CLOSURE-TIMEOUT and MISSING-COMPLETION lines | ACM:246-273, ACS:466/315/693 | 081-11/12/15/17/22 legs | OK |
| :186-203 LOW, memory, cap, refusal census | refusals only where forced; census closed | `BUDGET-EXCEEDED … census=`, row `budget_census` | ACS:258-265/576-581/680 | synthetic (never run at runtime) | OK |
| :52-62 peak census | **exactly one** HIGH-WATER-PEAK line equal to the summary | **one line per closure** | ACS:655/664/669 | build-3 single-closure legs pass; none with two closures | OK, except **PQ-B** on G7/G15 |
| :205-210, :212 | EFFECTIVE line, no REFUSED/DISCARDED, multi-view 0, unsupported 0, no late_results | log lines, counters | ACS:84-90/404, SVE:76 | build-3 legs | OK |
| **:211** | **view_rejected 0 except G8** | counter | SVE:89/179, **MSVE:160, MSVE:184** | **V6 G2 N/S = 1, V6 G15 N/S = 1** | **PQ-A** on G2/G15 |
| :214-221 oracle | exit 0, compared = measured | oracle summary | frozen `verify_capture.py` | build-3 legs | OK |
| nohold / Lyra / legacy / fixture evaluators | as in 081-23 | label and event fields, change keys, OFF echo | ACM:164, etc. | 081-14, c0 L1/L4 (build 3), 081-22 D, 081-12 | OK |

Note (not a question): `lyra_g270_eval` stops on an honest `empty_region` refusal, as 081-14 §3.4 is written. It would have fired on the 1800-frame c0 L1 leg (14 rows). It did not fire on c0 L4 or on 081-17.

## 5. C1_G8_N re-evaluation and resume proof
- **Re-evaluation:** `PASS-REEVALUATED`, with no errors.
  - The oracle was re-run: 17 compared, 17 matched, 0 mismatched.
  - Trace: SI8 `unsupported_delivery`, SI9 `predecessor_undelivered`.
  - A new ledger row was appended after the original `FIXTURE-VALID-FAILURE` row, which is kept identical.
  - Re-evaluation files are in `c1\C1_G8_N_A1\reevaluation-081-26\`. The first invocation's summary, postflight and manifests are byte-copied to `c1\invocation-1-081-25\`.
- **Resume proof** (`resume-proof.json`, 7 of 7 as expected, c1 files unchanged): I drove the unchanged `081-23-chunk1.py` and `chunk3.py` with every launch stubbed.
  - With the new ledger, `C1_G8_N` is skipped as decided and the first launch would be **`C1_G8_S_A1`**.
  - With the 081-25 ledger alone, it stops at `C1_G8_N` with code 1 and launches nothing.
  - Invalid attempts still retry. A recorded NOT-RUN stops again with code 2; a NoHold FAIL stops again with code 1.
- 🔻 **Correction to the 081-25 record:** that journal said an unchanged re-run would retry `C1_G8_N`. It would not. The old code filed the failure under "done" and would have launched `C1_G8_S`, running past the failure (gotcha G290). All four runners now re-raise a recorded failure instead.

## 6. Boundary diff and dry-run results
- **Boundary** (`boundary-diff.json`):
  - `feature_head` `a745ee5…` → `148050f…` (docs commits 081-24, 081-25, 081-26).
  - `081-23-lib.py` `7e41fa89…` → `ce8184d4…`.
  - The other 33 pinned hashes and all other keys are unchanged; a `reissued` provenance block was added.
  - An offline replay of the preflight passes for c1, c2 and c3 (`boundary-verify.json`). Only the live process check was stubbed.
- **Dry run** (`081-26-evidence\dryrun\`): the unchanged `081-23-dryrun.py`, with its output folders redirected to `081-26-evidence\dryrun\`.
  - **134 of 134 as expected:** the 118 original checks with identical outcomes, plus 16 new G8 checks.
  - No 081-23 evidence file changed. The c0/c1 manifests confirm it, and the bank has 13,987 of 13,987 files unchanged.

## 7. Commits pushed; every harness file changed
- **Pushed** `feat/m55-change-evidence` `1ca1ab3..148050f` (docs only): predictions AMENDMENT 1 (post-hoc, counted), journal 081-26, gotchas G290 and G291, CLAUDE.md status. No tag. The scratch worktree is removed.
- **Checkout state:** main checkout is `m51 53bf725` with no tracked edits, master is `031a103`, staged exe is `002805CF`. CaptureBench and `ToCodex` untouched.
- **Harness files changed:**
  - `_reviews\081-23-lib.py` (+15/−5, CRLF kept).
  - `_reviews\081-23-evidence\prep-boundary.json`.
  - c1 state: `c1\stack-results.json` (row appended); new `c1\invocation-1-081-25\*` and `c1\C1_G8_N_A1\reevaluation-081-26\*`.
- Everything else new is under `_reviews\081-26-evidence\`.

## 8. NEEDS-DECISION items
1. **PQ-A:** the rule "`change_view_rejected` 0 except G8" (predictions §2.1.4, `lib:211`) is wrong by construction for two gates.
   - G2: the mask for the dropped SI8 frame is still served, finds no m55 family data, and counts `view_rejected` (MSVE:184).
   - G15: the bench mask hold counts every held pass (MSVE:160).
   - Stage-1 had already recorded 1 on both gates.
   - **It blocks `C1_G2_N/S` and `C1_G15_N/S`.** Recommended wording: "0 except G8, G2 and G15 (≥ 1 on each)".
2. **PQ-B:** the rule "exactly one `HIGH-WATER-PEAK` line" (§2.1.3, `lib:60`) is wrong for gates 7 and 15.
   - Both reset the epoch at SI 9 and close twice, and the 081-17 field reference already says one line per closure.
   - **It blocks `C1_G15_N/S` and `C2_G7_N/S`.** Recommended wording: "one line per closure (1 + `change_epoch_resets`), the last equal to the summary, every census closed".
3. **Declared deviation to accept or reject:** I hardened the resume rule in all four runners, not only the c1 runner. I also ran the dry run through a redirecting wrapper, because running it as-is overwrites existing 081-23 evidence.

## 9. Wall-clock (IST, approximate)

| step | time |
|---|---|
| Reading (ruling, report, harness, source) | 11:53–12:00 |
| Source audit and build-3 stand-in scan | 12:00–12:05 |
| Lib fix and G8 proof | 12:05–12:07 |
| Re-evaluation and resume proof | 12:07–12:09 |
| Dry run (177 s) | 12:10–12:14 |
| Audit stand-ins and table | 12:14–12:17 |
| Docs, commit, push | 12:17–12:20 |
| Boundary re-issue and preflight replay | 12:20–12:22 |
| Final checks, worktree removal, memory | 12:22–12:24 |
| **Total** | **~31 min** |

The claude.ai connectors (Gmail, Calendar and others) still need authorizing in claude.ai settings, and the PDF viewer failed to connect; none were needed here.
api-equivalent cost (not billed on Max): $15.27 | turns: 95 | running total across all briefs: $2,165.94
