# REPORT for 095-06b-review-fixes-integration.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-06b-review-fixes-integration.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 06:47:40 | finished: 2026-11-14 07:43:37 | duration: 56 min | exit code: 0
context at last turn: 826163 tokens (83% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-06b-review-fixes-integration.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**1. For the owner (3 lines, plain words)**
- Every ruled fix is written and tested offline, and the combined test branch `integ/m56-m58` is built. All of it is saved locally only.
- I did not run the Editor compile and pushed nothing: E: had 20.7 GiB free against the 50 GiB floor this brief checks first. Your own files, not the build, fill E:.
- To finish: free E:, or have chat say the floor does not apply to an Editor-only compile (that compile writes to D:; see decisions). Then a short brief compiles the three branches and pushes them.

**2. Per item.** Commit hashes are local. `cl /Zs` means a syntax-only compile using the real build's own settings; it writes no output, and a deliberate error placed at the end of each file group proves the check read every file to the end.

m56 (`fix/m56-capture-continuity`, local `e12180c`, origin still `b4017ea`):

| Item | Commit | What changed | What proves it |
|---|---|---|---|
| S1 | `d200851` | A vetoed event no longer comes back into `annotation.json`. `run_summary` gains `vetoed_event_ids`. | `cl /Zs` clean |
| S2 | `8f9ebd7` | The readers drop the count-based tolerance. An orphan transition event passes only on schema 2.2, only if its id is listed, only if its sole reason is `settle`. Schema 2.1 is judged as before. On 2.2, LABELLED-EXTRA (the reader's check for labelled frames an event does not list) is also keyed to the ids; that goes slightly beyond the ruling. | label-rule selftest 88 cases, incl. the reviewer's probe session as a must-fail case and one case the old rule passed; annotation tests 11 |
| N10 | `1da2720` | `annotation.json` gets a root `transition_frames` list. The 2.2 reader requires it. | annotation tests (3 new failing cases); office kit 102 |
| S3 | `dacc017` | In held-global mode the settle ticks are captured as normal frames: same schedule, no flag. | new held-global `camera_clipping` case passes; the old flagged version fails |
| S4 + N2 | `66f56c6` | 120 s with no writer job finishing means stalled: the run stops, unwritten frames count `writer_timeout`, `writer_stalled` is set. This covers back-pressure and both flushes. Writer jobs carry a run generation. | `writer_progress_selftest` 26/0; two deliberately broken copies fail 4 and 5 checks |
| N1 | `e01b9ac` | Sync-fallback frames and positives are added at both places that overwrote the count. | `cl /Zs` only; the engine proof needs a backbuffer leg |
| S5 | `24a837b` | Code fix, not the documentation fallback: the windows skip settle frames when they count. No membership rule or label bit changed. | `settle_window_selftest` 36/0: for K = 2 and 0, N = 1, 3, 40, the fixed build flags exactly the release frames on every non-settle tick; the as-built version lost frames at K = 2, N = 3. `m52_window` 316/0 unchanged |
| N7 | `c0c5ab3` | The zero-match excuse covers at most 2K rows per burst, as the burst's two K-tick runs at their schedule positions. | label rule 94 cases |
| N3, N4, N5 | `e12180c` | Readme and journal; N5 left as ruled. | n/a |

m58 (`feat/m58-terrain-blink-subtype`, local `5e1bc03`): M1 (`clear` returns to the ini list, `none` means no patterns), M2 (the all-terrain warning) and the M3 doc note are one commit. Terrain selftest 56/0; five broken copies all fail; `cl /Zs` clean. A quoting error first put this commit under a docs-only subject; it was local, so I amended the message.

CaptureBench `m56-gates` (local, now `1978cd8`; checkout left on `master`):
- **B1** (`4c9b1ae`): `run_leg.ps1` never stops a process by name. It stops only its own PID after rechecking start time and path, and refuses to start while another StackOBot process runs, naming it. Tested 12/12 with a renamed copy of `ping.exe` as a stand-in, never the game.
- **S6, N6, N9, CB-N1** (`1978cd8`):
  - The G5 comparator lists `exposure_dip` differences separately and never fails on them; any other new difference still fails. It now states it reads `labels.jsonl` only. Selftest 10 cases.
  - New `m56_settle_content_check.py` (N6): 8 cases, proven both ways.
  - The continuity check fails zero-byte frames (N9): 10 cases.
  - The frozen predictions file gained a dated AMENDMENT 1 (P11 and P19 amended, P21–P23 new). Hashes before and after are in the receipts.

**3. The integration branch** (`integ/m56-m58` = `9bf6c7e`, local)
- Built from `master` with true merges: m58 first (`c8a60d1`), then m56 (`e2117d3`). M4 (the terrain row moved to the 2.1 → 2.2 table) is `4d7fbe5`, then docs.
- `WriteRunSummary` keeps m58's two counts, then m56's `Continuity`, at the declaration, the definition and the one call. The run-start resets keep both sides. `CLAUDE.md` and the gotchas file keep both (G574, then G575–G578).
- Vetoed terrain blink: no fix was needed. The vetoed-id skip runs before the terrain count, so a vetoed terrain blink does not count in `blink_terrain_events`.
- No UBT compile (E: floor). `cl /Zs` is clean on every changed unit. All C++ selftests pass. The Python sweep is green: label rule 94, pixel gate 101, change oracle 35, kit 102, 55 unit tests, exclusion gate, offset selftest, lever audit PASS (57 levers) with its 12-case selftest. The pre-push scan passes (scrub clean, no token, no comments). Not pushed, no tag.

**4. Binaries and context**
- The checkout is on `integ/m56-m58`, clean. The host's Editor binaries still match `b4017ea`'s source (the 095-05 build), not any new tip.
- About 40–50 % context used.

**5. What 095-07 must run** (also in the receipts as `095-07-run-list.md`)
- **First:** compile the three branches on the Editor target and push them.
- **Exes**, from one cook by code-only swap:
  - the release exe from the archive;
  - an m58-only exe from `5e1bc03`;
  - the integration exe from the `integ/m56-m58` tip.
- **Cook maps:** `CB_GateLevel`, `MainMenu`, `MainWorld`, `Entry`, plus the m58 landscape fixture if C-G2 needs it. Read the map set back after the cook.
- **Legs:**
  - every m56 gate and the settle-exactness table, plus m58 C-G2, C-G3 and C-G6, on the integration exe;
  - the release exe as the can-fail side and as G5's comparison side, with a release control pair;
  - m58 C-G4 and C-G5 on the m58-only exe against release.
- **Readers:** `verify_capture --label-rule`, the continuity check, the G5 comparator and the settle-content check. Do not use the continuity check's settle-placement clause (C7) on a targeted held `camera_clipping` leg; the label rule judges that leg.
- **Frozen predictions:** `code-receipts-095-05\predictions-095-05.txt` with AMENDMENT 1, and `m56_gate_predictions.md` amendment 1.

**6. Safety**
- A failed command of mine wrote an empty stray `tools/run_leg.ps1` into the plugin repo. I deleted it; it was never staged.
- The owner's processes were not touched.

**Decisions chat must make**
1. **The E: floor.** It blocked the compile and every push. I measured that the plugin's Editor compile writes its objects and DLLs to the plugin folder on D:; only the target makefile goes to E:. Chat should either waive the floor for an Editor-only compile or wait for the owner to free E:.
2. **S3 wording.** The ruling says only "the first onset and the final revert are flagged". In the code the held global is applied before the first captured frame and removed after the last, so nothing is flagged at all. If chat wants the first K frames of such a run flagged, the readers must change too.
3. **The S2 extension.** LABELLED-EXTRA is also keyed to the vetoed ids, on 2.2 sessions only. Chat should ratify or revert it.

Receipts: `D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-06b\`
api-equivalent cost (not billed on Max): $42.60 | turns: 313 | running total across all briefs: $6,174.71
