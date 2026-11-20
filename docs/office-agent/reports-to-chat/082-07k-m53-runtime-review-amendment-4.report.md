# REPORT for 082-07k-m53-runtime-review-amendment-4.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07k-m53-runtime-review-amendment-4.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 05:24:55 | finished: 2026-09-28 06:40:47 | duration: 76 min | exit code: 0
context at last turn: 770959 tokens (77% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07k-m53-runtime-review-amendment-4.live.txt

---- FINAL MESSAGE (Claude Code) ----
I've finished 082-07k, and **the last 35 S1 legs (082-07l) are cleared to run**. No stop condition fired, all changes are committed and pushed, the boundary is re-issued, and the preflight replay passed. Nothing ran on the bench.

VERDICT: GREEN

**2. Plain summary for the owner**
- The "cancel before focus" test is now a "stop before the first fire" test, and both of its legs run again fresh. The real cancel-before-focus check moves to S2, which needs a small bench switch in the build.
- Checking every remaining test against how the bench actually behaves caught three more that would have failed for reasons unrelated to the feature:
  - the MainWorld tests would all have been voided, because its camera keeps drifting slightly for ~85 frames;
  - the garbage-collection test could never see garbage collection in the log;
  - one test would have stopped if an early second injection was refused.

  All three are now handled and proven.

**3. Runtime-conditions review.** The full table with evidence and citations is AMENDMENT 4 §A4.3. In summary:

| unrun legs | runtime conditions checked | disposition |
|---|---|---|
| CAN-UV, CAN-NM | Focus at the frame-1 Start: the runner forces it by 0.9–1.3 s. The 0-frame session has no `labels.jsonl`. | **Fixed:** the stop-before-fire rule (A4.1); TRIP now reads `run_summary.json` (A4.2) |
| FIN-NM | Frame cap with a live event, fixed step (FIN-UV passed for real in 082-07j) | Unaffected |
| DES ×2, DES2 (P2-6) | The destroy switch counts active ticks, NoApply 2 included. APPLIED lands in the decision frame on 1,069 of 1,069 real fires. The switch is in the exe. | Unaffected |
| END ×2 (P2-6) | Log read live (proven on 4 census legs). A real window-close exit is banked (M40_TEARDOWN_GRACEFUL, exit 0). Engine source routes the Quit end-of-play to the target watch and revert; live bytes drop on the game thread. | Unaffected |
| GC ×2 | GC does pass about once a second with the cvar, but `LogGarbage` defaults to Warning while the "Collecting garbage" line is at Log. Three real 65 s logs contain 0 such lines. | **Fixed:** `Log LogGarbage Log` switch, plus a "detector blind" reason (A4.3.2) |
| FR ×2, FS ×12 (P2-5) | Switches present in the exe. All 1,069 real ledger lines read `off=2 live=0 pending=0 balanced`. | Unaffected |
| TWO | The frame-1 census reads TC_NN1 as APPLY, but the frame-1 apply transaction has never run on the bench. | **Fixed:** a named refusal reads FIXTURE-CANNOT-EXERCISE, not FAIL (A4.3.3) |
| COLL F-SYN (P2-2) | On-screen set depends on camera pose: 156 of 156 real F-SYN sessions sit at one pose | Unaffected |
| FMW_CTL_A/B, FMW_ID | The MainWorld camera origin drifts for ~85+ frames, so under the old "settled camera" rule every attempt would fail 6 times and stop the run. 146 of 161 banked launches follow one camera path exactly (held-out check: 73 of 73). | **Fixed:** the pixel legs must match a pinned camera path (A4.3.1) |
| S3O, A2, COLL F-MW, S8 | Same camera rule applied, but these rows read only logs. The Bot's name is stable across builds; CB_GateLevel is in the cooked build. | **Fixed:** no camera check on these legs |
| All legs | Asset-resolution check and TRIP | Unaffected |

No row among the 35 needs a build or fixture change, beyond the cancel-before-focus check the ruling already moved to S2.

**4. Person evidence.**
- **10 of 13 flagged attempts are the runner's own keystroke.** Their last input sits within −0.010 to +0.033 s of `sampling_started`, which is the runner's final startup `Force` (a synthetic Alt key).
- **The three CAN attempts are different:**
  - A1 and A2 each show an input 51.7 s after launch, 47 s after the runner's last action.
  - The runner does nothing at that point and the game log is silent.
  - It is not the runner's input; its cause is not established.
  - A3's second event could not be timed.
- **No unrun leg can be voided.** Person evidence never adds a validity error; it only turns a focus-only failure into ENV-INTERRUPTED and costs at most a 60 s wait. The longest real attempt is 11.1 s.
- **Detector unchanged**, since the ruling's condition for fixing it isn't met. For S2: save the input timestamps and excuse the runner's final startup keystroke.

**5. AMENDMENT 4 (§A4.1–A4.8),** in commit `7a5b67b`:
- the stop-before-fire rule for both CAN legs;
- TRIP reading `run_summary.json`;
- the review table and its three fixes (camera-check scope, GC detector, TWO refusal);
- the person-evidence finding;
- supersede entry 5;
- the harness changes: legs `c552c587`, lib `96831d8f`, new locked file `082-07-mw-trajectory.json` `1ce9d988`;
- predictions for 082-07l.

**6. Proofs, no-flip and dry run** (final lib, all suites run from copies in `082-07k-evidence/prep`):

| suite | result |
|---|---|
| `proof-082-07k` (new, both ways, real banked data) | **46/46**, before and after the supersede act |
| `proof-evaluators` | **207/210**: the 3 failures are the old checks this amendment replaces (the old cancel test, and two MainWorld checks written for the old camera rule). The replacements pass in `proof-082-07k`. |
| `proof-082-07d` | **37/37**: its first run exposed a real crash (`trajectory_errors` on a row with no `frame_index`), now fixed; one synthetic MainWorld leg was given a valid camera path |
| `proof-sequencer` / `proof-disk` | **20/20** / **15/15** |
| `proof-082-07g` | **32/33**: P5a, the known A3.7 finding (same 10 of 991) |
| `proof-082-07i` | **35/35** |
| No-flip, before / after supersede | **511/511 and 511/511, 0 flips**, live files unchanged |
| Dry run, before / after lock | **193 legs, 548 rows, 0 problems** both times |
| End-to-end window (stubbed launch) | **8/8** |
| Preflight negatives | **14/14 refused**; control passes; both stub refusals hold |

**7. Supersede record, entry 5**
- **Record:** `e67f0ec9`, replacing `ff7f884f`.
- **Superseded:** `M53S1_G4_CAN_UV_A1/A2/A3`, plus their label-less NOT-RUN record. That needed a new record-level supersede key: the old lib filtered by label only, so the NOT-RUN would have stopped every window.
- **Bank:** the three attempt dirs are renamed `…__SUPERSEDED_082-07k` (12 files each, contents verified). Nothing deleted.
- **After:** no halt; 158 legs accepted, 35 left starting with `G4_CAN_UV`. Its fresh attempt will be labelled A4, because attempts are now counted from labelled rows only; no other leg's numbering changes.

**8. Boundary and preflight replay**
- **Boundary:** `1c3a348f`, replacing `456f4146` (kept as `boundary-082-07i.json`).
- **It locks:**
  - head `7a5b67b`;
  - predictions `8acb75d4` (AMENDMENTS 1–4);
  - lib, legs, supersede record and the trajectory file;
  - the unchanged staged build `2FCDF059` / `20DA6F98` / `534C5863`.
- **Preflight replay** right after the lock: **0 problems, bench quiet**. Still clean after all the post-lock checks.

**9. The 082-07l invocation**
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07l 540
```
- Don't set `IAI_R53_ROOT` or `IAI_R53_STUB_LAUNCH`. Use `082-07l-2` if the 45-minute wait budget runs out.
- **What it runs:** it resumes at `G4_CAN_UV` (fresh, A4) and runs only the 35 unaccepted legs, `G4_CAN_NM` included. It re-launches no decided leg (dry run and end-to-end test).
- **Expected time:** about 24 min by the estimator, plus waits.

**10. Commits pushed:** `7a5b67b` on `feat/m53-uv-normal-corruption` (`848651e..7a5b67b`, local = origin). It carries the amendment, the journal, the status block and gotchas G333–G335. No tag; `m51` is still `53bf725` and `master` `b5f15a3`.

**11. NEEDS-DECISION:** none blocking 082-07l. For your evidence review:
- the 3 old `proof-evaluators` checks superseded by design;
- the unexplained 51.7 s input in the CAN attempts (S2);
- the declared risk that the MainWorld camera path under this build differs from the banked one. If so, the F-MW pixel legs stop as NOT-RUN with the reason printed, not a wrong verdict.
api-equivalent cost (not billed on Max): $28.48 | turns: 145 | running total across all briefs: $2,826.04
