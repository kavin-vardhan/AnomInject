# REPORT for 081-20-m55-independent-review-of-codex-stages-1-2.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 081-20-m55-independent-review-of-codex-stages-1-2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-24 03:08:01 | finished: 2026-09-24 03:38:17 | duration: 30 min | exit code: 0
context at last turn: 674885 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\081-20-m55-independent-review-of-codex-stages-1-2.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner**
- I checked every line of Codex's m55 source against the contract. I found nothing that publishes a wrong number or pairs the wrong frames, and nothing that crashes, hangs or leaks memory.
- I re-checked all 365 saved evidence files from the bench, covering 1,450 measured rows: none break a rule, and every run ends with no memory still held.
- A simulation of the waiting and timeout logic, run 1,200 times, never hung or joined the wrong frames.
- Two real problems for chat to decide on. If you start a capture and stop it before the game window has focus, it leaves an empty folder behind. With m55 switched on, a game that draws split-screen would record no frames at all.
- Everything else is tidy-up: labels that don't match the docs, code comments that must be stripped, and bench-only switches.

**Findings** (file:line on `5649a6d`; the full table with every scenario is in `_reviews\081-20-code-review-of-codex-m55.md`)

| id | sev | where | defect | scenario | conf. | evidence |
|---|---|---|---|---|---|---|
| F1 | P2 | `AnomalyCaptureSubsystem.cpp:5629` (inside `if (bWroteSession)`), `:5722`, `:3020`/`:595`; engine `FileManagerGeneric.cpp:110-116` | A run cancelled before focus gets its folder deleted, then re-created | Start then Stop before focus: the folder is deleted. At the next `StartRun` or at teardown, the leftover stage saves an empty `change_evidence.jsonl` and the file write re-creates the folder. This breaks m16's "cancel writes nothing" rule, in both delivery modes. | high | not exercised by any banked run |
| F2 | P2 | `AnomalySceneViewExtension.cpp:76-85`; `AnomalyMaskSceneViewExtension.cpp:151-161` | A family with more than one view is rejected before it consumes the capture arm or publishes a key | On a split-screen host, zero frames are written and every frame logs a "NO KEY" drop. Mask arms are never served, and pending arms grow by one per frame for the whole run. The contract says such frames should be refused as `unsupported_delivery`, not lost. Before m55 this case was already unsound (the picture could come from view 1 while the labels described view 0). | high on code; whether such hosts matter is unknown | not exercised (every fixture has one view) |
| F3 | P3 | `AnomalySceneViewExtension.cpp:164-171`; `AnomalyChangeStage.cpp:487` | Legacy frames are dropped when a stage exists but the frame carries no m55 data | Needs two faults at once: the camera owner changes mid-run, then the sidecar write fails. The stage stays closed and every later legacy frame is dropped. This contradicts the design comment at `AnomalyChangeFamilyData.cpp:15-16`. | high | not exercised |
| F4 | P3 | `AnomalyChangeStage.cpp:461-481`, `:88` | Closing the stage makes the game thread wait up to 5 s | This happens when the camera owner changes mid-run, or at run end when a frame was dropped on the render thread, because those drops never notify the stage. The wait is bounded; nothing hangs. | high | the 5 s timeout path is shown by the gate-17 can-fail legs |
| F5 | P3 | `AnomalyChangeStage.cpp:379`, `:380`, `:387` | Refusal names disagree with the field reference and the plan | A predecessor that timed out reads `predecessor_undelivered` (docs say `predecessor_missing`). A predecessor refused on budget reads `predecessor_missing`. A mask from the wrong family reads `view_mismatch` (plan says `mask_payload_missing`). All are still honest refusals. | high | banked: the only 2 `predecessor_undelivered` rows are gate-16 timeouts |
| F6 | P3 | `AnomalyChangeMeasurement.cpp:163` | The event record's `late_results` is always 0 | 197 real late rejects exist in the bank, but none can reach this field. | high | banked |
| F7 | P3 | `AnomalyChangeStage.cpp:109`, `:190` | `duplicate_completion` counts every unsupported frame | JPEG or resampled runs show a fault-sounding counter on every frame. | high | none in the bank |
| F8 | P3 | 14 files | 66 comment lines added, against the no-comments rule | Already on the merge checklist (081-17R). | high | `count_comments.py` output |
| F9 | P3 | `AnomalyChangeStage.cpp:19-20`, the descriptor, the bench paths | Bench surfaces are only partly gated | `IAI.Bench.ChangeGate` values 2, 3, 4 and 5 drop legacy output at SI 8 without the `-IAIBenchFixture` flag. `AnomalyBench` is present but dormant in any non-Shipping client build. Plugin code contains StackOBot and Lyra names, a map path and coordinates, but no host types. | high | — |
| F10 | P3 | `AnomalyChangeMeasurement.cpp:62`; `AnomalyChangeStage.cpp:454` | The row cap is a row count, not a memory bound | 100,000 rows is about 0.25 GB, plus a copy of the same size when the file is written. | medium | row sizes from the bank |
| F11 | P3 | `AnomalyChangeMeasurement.cpp:221-239`; `AnomalyChangeStage.cpp:327-333` | One serial worker does the pixel scans | About 45 ms per scanned pair at 1080p, so roughly 22 scans per second at most. During closure it busy-waits on a thread the PNG writer also uses (relevant to 081-18). | medium | worker times from the bank |
| F12 | P3 | `AnomalyChangeMeasurement.cpp:18` | The mean's rounding rule (round half up) is not documented | A consumer that rounds half-to-even disagrees on exact midpoints: 5 in 200,000 random pairs. | high | port test |
| F13 | P3 | `AnomalyChangeStage.cpp:482-492` | `ResetEpoch` does not clear a pending deferred end | Not reachable with today's tick order; a robustness note only. | medium | — |

I did not re-report the 64 MiB default in `5649a6d` refusing at 1080p; chat already ruled that a spec defect, fixed in build 2.

**Tested claims**
1. **Replay of all 365 banked sidecars: 0 violations.** The histogram, sum and mean arithmetic, the N−1 predecessor, token, family and geometry matches, and the onset check all hold. Each phase is "measured" exactly when its onset row is. Retained bytes are 0 at the end of every run.
2. **Timeouts contradicted by a late frame:** on this build's clock (081-15) and on build 2, there are no timeouts outside the two can-fail gates. No timeout was later contradicted by an arriving frame.
3. **Pixel arithmetic, ported to Python:** 0 mismatches against the contract's formulas. A difference of exactly 8 is not counted and 9 is. The slow 0, 3, 6, 9, 12 walk gives `[0,0,0,0]` against `[0,0,48,48]` from the reference frame.
4. **Simulated stage and timeout logic, 1,200 runs:** no hang, closure always within 5 s, bytes back to 0, never a wrong predecessor. Early refusals of frames that would have arrived appear only under heavy completion disorder (0–1 s spread), not at StackOBot-like or Lyra-like spreads.
5. **Engine source:** the per-family m55 data is shared, not copied, so it cannot be freed twice. The file writer re-creates a missing folder, which is what causes F1.

**Open questions**
1. In Simulate-in-Editor, does `GetGameViewport()` return null? If so, F2's check would reject every frame.
2. Is split-screen in product scope? If it is, F2 should be re-rated.
3. How do `encode_watcher` and the dashboard treat F1's orphan folder, which never gets a `run_summary.json`?
4. Is 8 later completions enough margin on hosts where frames finish badly out of order?

**Coverage**
- Read fully: all new m55 files, every hunk of the diff with the surrounding functions, and the completion-clock commit on its own.
- Skimmed: the older mask-reduce body and the run-summary merge.
- Not reached: Simulate-in-Editor at runtime. I checked the auto-pool and selector lists only by their absence from the diff.
- Nothing was built or launched and nothing was committed. The live checkout is untouched on `m51` and no worktree was created. I only wrote new files: the report and the scripts under `_reviews\081-20-evidence\`.

**Wall clock** (03:08:40–03:38 IST, about 30 minutes; splits are approximate)
- Setup and reading the contract and rulings: about 5 min
- Review plan: about 1 min
- Core stage code: about 4 min
- Integration code (capture, writer, SVE, mask, bench, control server): about 8 min
- Checks against banked data (replay, logs, cost, reasons): about 7 min
- Deeper reads, engine checks, port test and simulation: about 4 min
- Writing the report: about 2 min

Files are in `D:\IntrusiveAnomalies\_reviews\`:
- `081-20-code-review-of-codex-m55.md` — the full report
- `081-20-evidence\review-plan.md`
- `081-20-evidence\replay_invariants.py` — banked-sidecar replay
- `081-20-evidence\late_after_timeout.py` — timeout vs late-frame log scan
- `081-20-evidence\port_pixel_arithmetic.py` — arithmetic port test
- `081-20-evidence\clock_model.py` — stage and timeout simulation
- `081-20-evidence\worker_cost.py` — pixel-scan cost from the bank
- `081-20-evidence\reason_origins.py` — where the refusal names come from
- `081-20-evidence\count_comments.py` — comment count per file
api-equivalent cost (not billed on Max): $15.28 | turns: 83 | running total across all briefs: $2,071.21
