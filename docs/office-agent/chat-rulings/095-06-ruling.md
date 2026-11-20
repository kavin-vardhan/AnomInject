# 095-06 — Chat ruling: the independent review is ACCEPTED; every finding and note is ruled

**Date:** 2026-11-14, about 06:55 IST · **Author:** chat Claude (orchestrator) · **Supersedes:** `claude/095-06-ruling.DRAFT.md`
**Input:** mailbox report 095-06 (GREEN, 27 min, $20.18 API-equivalent; fresh side session, read-only, changed nothing) and the full review file `_reviews\095-06-independent-review.md`, read by chat.

## The review's verdicts
| Unit | Verdict | BLOCKER | SHOULD-FIX | NOTE |
|---|---|---|---|---|
| m56 capture continuity (`fix/m56-capture-continuity` `b4017ea`) | READY AFTER FIXES | 0 | 5 | 11 |
| m58 terrain blink subtype (`feat/m58-terrain-blink-subtype` `a948046`) | READY FOR GATES | 0 | 0 | 7 |
| CaptureBench `m56-gates` (`1d1e119`, local) | READY AFTER FIXES | 1 | 1 | 1 |

None of the findings makes a normal training label wrong. They concern event bookkeeping, one targeted capture mode, check-tool loopholes, a possible long freeze, accounting on rare paths, and an optional setting.

## BLOCKER and SHOULD-FIX: all fixed
- **B1 (bench).** `run_leg.ps1` (lines 230 and 354) never kills by name. It stops only the PID it started itself, after checking that PID's start time and image path, and it refuses to start a leg while any other StackOBot process is running. The owner's standing rule; it lands before any leg runs.
- **S1 (m56).** A vetoed event leaves no event in `annotation.json`. The transition-only loop skips vetoed events.
- **S2 (m56 readers).** The count-based veto tolerance goes. `run_summary` gains the exact list of vetoed event ids. A reader accepts an orphan transition entry only when its event id is in that list, its only reason is `settle`, and the schema minor is 2 or later. Schema-2.1 sessions are judged exactly as before. The reviewer's probe session becomes a permanent must-fail test.
  - **Why:** a tolerance keyed to a count can always be spent on the wrong event; one keyed to ids cannot.
- **S3 (m56).** A frame is `settle` only when it follows a real fire or a real revert. In held-global mode (targeted `camera_clipping`) the settle-phase ticks are captured as normal frames; only the first onset and the final revert are flagged.
- **S4 (m56).** The writer's wait is bounded by **progress**: if no writer job completes for 120 s, the writer is declared stalled, the run stops, the unfinished frames are counted as `writer_timeout`, and the log says so loudly. The same rule covers the back-pressure wait and the end-of-run and teardown flushes.
  - **Why not a plain total deadline:** a slow but working disk must be allowed to finish; a disk that has stopped must never freeze the game thread for hours.
- **S5 (m56): code fix, with a fallback.** The optional temporal transition windows start after the settle frames, so every frame that carried `temporal_aa` in the release build under the same settings still carries it.
  - **Why chat overrules the reviewer's "document it" advice:** the other choice silently flags fewer frames for any client who uses the overrides, and under-flagging is the dangerous direction for label sync. A client should not have to change a setting to keep the coverage he had.
  - **Fallback:** if this cannot be done inside the window arithmetic without touching a membership rule or a label path, stop, apply the documentation fix instead ("windows now count from the edge; add K"), and report.
- **S6 (bench).** Amend the predictions before any leg runs, as a dated amendment with the reason: the G5 comparator allows the intended `exposure_dip` change on non-settle frames and nothing else new; P19 lists every accepted `run_summary` key.

## Notes: m56
| Note | Ruling |
|---|---|
| N1 sync-fallback frames undercount `FramesWritten` | **Apply.** Count them. The new accounting reads this number, and the never-run backbuffer leg can reach the path. |
| N2 a timed-out writer can corrupt the next run's counts | **Apply.** Tag writer jobs with a run generation; late jobs of an old run never touch a new run's lists. S4 makes this path reachable. |
| N3 a `writer_timeout` frame can still land on disk after the summary | **Document.** The summary marks the run as stalled; files that arrive later are not part of it. |
| N4 mask counts can exceed the frame count on a lossy run | **Document:** "they add up to the frame count on a healthy run". |
| N5 offset-settle geometry frozen at `BeginRevert`, not tick end | **Leave.** Transition-only entries carry no label box. Record it in the journal. |
| N6 no reader checks what a settle frame's label says | **Apply.** A scripted check: labelled-and-flagged counts per event, and a measured mask on every onset-settle frame. Proven both ways offline. |
| N7 the label rule excuses every stray settle row once one zero-match burst exists | **Apply.** Excuse at most 2K rows per zero-match burst, at schedule positions. Same lesson as S2. |
| N8 the oracle's margins on onset-settle frames | **Leave for 095-07** (re-gate margins on real legs). |
| N9 the continuity check accepts zero-byte frames | **Apply** (trivial). |
| N10 `annotation.json` has only a count of row-only settle frames | **Apply.** Their indices go into the session-level transition list, so a client reading only `annotation.json` can drop them too. |
| N11 blink hidden sets in tick space not re-derived | **Leave for 095-07.** |

## Notes: m58 and bench
| Note | Ruling |
|---|---|
| M1 `IAI.SetTerrainPatterns clear` returns to the cooked ini list | **Apply.** `clear` means "back to the ini list" and says so; a new `none` means "no patterns". |
| M2 no guard against an over-broad pattern | **Apply.** One loud run-end warning when every blink event of a run (five or more) was terrain. |
| M3 sync path never freezes anchors | **Leave.** Unsupported for delivery; note it. |
| M4 readme row sits in the v1→v2 table | **Apply** on the integration branch: it belongs with 2.2. |
| M5, M6, M7 | **Leave.** C-G2 proves M7. |
| CB-N1 the comparator reads `labels.jsonl` only | **Apply:** state that scope in the G5 reading. |

## The build plan for 095-07 (revised from the draft)
The reviewer wants each unit gated on its own exe. The draft ruling wanted one merged exe. Both points are right, so:
- **`integ/m56-m58`** = `master` + m58 + m56 (true merges). **This is the build that ships,** and every m56 gate plus m58's C-G2, C-G3 and C-G6 run on it.
- **An m58-only exe** runs m58's C-G4 and C-G5 against the release exe. Those two gates say "nothing else changed", and m56 changes the frames per cycle, so they can only be read cleanly without m56.
- **The release exe** (archived) is the can-fail and the comparison side.
- One cook serves all three by code-only swap. No m56-only exe is needed.
- After green gates, `master` fast-forwards to the integration tip.

## Still open, for after the gates
- The K default (settle flag), from the settle-exactness table.
- The oracle keeps skipping only `settle`-flagged flank frames.

## Next
1. **095-06b** (light): the fixes and notes above, the bench script, the integration branch.
2. **On the owner's word that the PC is free: 095-07** (heavy): Game builds, one cook, every gate, the settle-exactness table.
