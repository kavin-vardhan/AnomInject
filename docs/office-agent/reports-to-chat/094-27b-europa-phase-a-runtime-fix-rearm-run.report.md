# REPORT for 094-27b-europa-phase-a-runtime-fix-rearm-run.md
verdict: RED (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-27b-europa-phase-a-runtime-fix-rearm-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-30 10:34:25 | finished: 2026-10-30 15:40:08 | duration: 306 min | exit code: 0
context at last turn: 657894 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-27b-europa-phase-a-runtime-fix-rearm-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: RED. Codex fixed, tested and re-armed the runtime kit, and the smoke session passed. Then our editor crashed while closing at the end of the A3 Load leg, so the run stopped as the rules require. No other test ran and there are no pictures. Nothing protected changed and nothing was uploaded.

**Plain summary for the owner**
1. Unreal ran twice this afternoon, at 14:41 for 33 s and at 14:43 for 11 s. The first session was clean. The second crashed while closing.
2. Two bugs of ours showed up first. Our script loaded the test map three times at once, and the test map's camera looks like it was saved tilted sideways instead of looking down. No anomaly tests ran, so there are no pictures.
3. Nothing is left running and nothing needs closing.

**Start state (10:38 IST)**
- `m51` matched P9: `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries. Checked again by the launch wrapper and at the end.
- 094-27's report and review showed no protected change, no crash and no upload.
- The protected-state check passed. All 110 bound files matched, and P9 v2 was unchanged.
- The feature branch was at `ccc861b`, clean. The three test-map files matched 094-26d byte for byte.
- Only the `31124` marker was in EditorRuns. The Epic launcher was running, with no Unreal process.
- Free space: D: 57.9 GiB, E: 289.7, C: 28.2. Memory was low: 6.4 GiB free commit and 5.3 GiB free physical.

**The Codex job**
- Thread `01a14334` in run folder `…\runs\2026-10-16-094-01-europa-phase-a-plan`, resumed on the first try.
- It ran 10:43:25–15:25:53 (16,949 s) and exited 0, with no capacity retries and no foreign drive-root entries.
- From 11:43 to 14:39 it waited for an idle PC. Physical memory was the main blocker, held by your four `kz_s3_post.py` render jobs and DaVinci Resolve.
- At 12:14, D: briefly dropped to 28.5 GiB because Windows grew the page file by 19.5 GB. It recovered by 12:24, while no session was running.
- Codex's verdict: `RED | 2 real sessions; A3 FAIL, A5 NOT_RUN, A6 NOT_RUN, A7 NOT_RUN, A8 NOT_RUN; evidence retained, no retry or push.`

**The changes and the run-up**
- **The cleanup fix (FH2):** the fixed reader now passes a copy of the exact 094-27 smoke files the old reader failed. I checked all 26 reader requirements against the probe's source lines, and every one is emitted where cited.
- **Real-file tests:** the reader passes all five kept real probe files and rejects 115 damaged copies. My own damaged copies failed too.
- **My can-fails: 82 of 82 behaved as expected.**
  - A failing leg is followed by a passing one, and the second still runs.
  - An ensure folder after a normal exit fails only its leg; exit code 3 ends the run.
  - A `-MONITOR` stand-in started outside the owned tree is adopted and stopped before its root, and the decoys are left alone.
  - The memory lines work as ruled: 8 and 6 GiB to start, a stop below 4 GiB.
- **Suites I re-ran:** 35, 121 and 22 checks, plus the 115 mutations, all pass. The engine's Python 2.7.14 harness passed when I ran it myself.
- **Codex's own catch:** before any launch, it found that a hard stop could be lost at closeout. It fixed that once (`ecfb3ce`) and re-armed.
- **Rehearsal and dry runs:** all 8 rehearsal cases passed, and both 18-leg dry runs passed with 0 launches.
- **Live reading:** my own 3-poll compare matched the kit's reading.

**The real run, per leg**

| Leg | Gate wait | Start–end (IST) | Duration | Exit | Reader |
|---|---|---|---|---|---|
| Smoke (PID 14924) | pre-check passed 14:39 (1,811 s idle); gate 82 s | 14:41:06–14:41:40 | 33 s | 0 | PASS: 31/31 bindings, clean `probe_stop` |
| Load / A3 (PID 48368) | pre-check passed 14:42 (1,979 s idle); gate 71 s | 14:43:40–14:43:52 | 10.6 s | 3 | FAIL; the camera check had already failed (`DRIVER_FAIL`); run STOP |
| The other 16 legs | — | — | — | — | not run |

**The gates**

| Gate | Reader | Mine |
|---|---|---|
| A3 | FAIL | Agree. The compile-out half alone would pass on the Load files. |
| A5 | NOT_RUN | Agree |
| A6 | NOT_RUN | Agree |
| A7 (valid / applied / reverted) | NOT_RUN: 0/0/0 for blinking, lighting_mismatch, lod_corruption and missing_object | Agree; no A7 probe rows exist |
| A8 | NOT_RUN | Agree |

**The pictures:** none. `D:\IA_Europa\Receipts\09427b\contact-sheet-index.json` lists 0 contact sheets, so there are 0 of the 4 expected.

**My review** (`_reviews\094-27b-code-review.md`, 45 rows): 38 OK, 1 GAP, 1 not run, 1 OK ending in the crash stop, 1 carried. The fixed kit met every rule it was given; the run is RED because of the crash.
- **G1, blocking:** our editor script re-entered itself. Three `MAP LOAD` commands went out in one frame (Runtime.log:711, 713, 714), followed by three loads and three `QUIT_EDITOR` (:732, :751, :770).
- **G2, GAP:** the camera read-back failed. In 4.25, the Python `unreal.Rotator` takes roll first, then pitch, then yaw (NoExportTypes.h:454, KismetMathLibrary.h:2142-2146). So the recipe's `unreal.Rotator(-12,0,0)` (author_425_fixture.py:80) most likely saved a −12° roll instead of a −12° pitch. This is a strong lead from source, not an observation: no receipt kept the saved angles.
- **G3, OPEN:** the crash itself was an access violation in `UE4Editor-Niagara.dll` during shutdown. No crash report folder was written, and the cause is not established.
- **O1:** no UnrealBuildTool process started in either session (094-27's smoke had one), so the build guard (FD2) was not tested live.
- **O2:** Codex's final secret-token scan could not read three locked Windows Update files. My own scan of our files found 0 in 3,201, and my planted test token was found 3 of 3 times.
- **O3:** the crash left a new marker, `EditorRuns\48368`. It is in ET2's allowed class, but Codex's receipt lacks its times and process; I recorded them.
- **O4:** three comment lines were added to the tools' Python, against the repository's no-comments rule.
- **Other observations:** the `-game` privacy switches are still untested in a real session, and my 10-minute polls missed both sessions, so I checked the monitors against the crash-client logs instead.
- **My other independent checks:**
  - Content, Config, Saved\Config, the descriptor and the `.uproject` are unchanged.
  - Both sessions' sealed evidence still matches its hashes, and the kept real files are unchanged.
  - No upload or POST line appears in either session's logs, and no new crash folder appeared.
  - The command audit shows 314 commands: 313 through the wrapper, 1 direct read-only, 0 direct writes.
  - Exactly 2 editor launches. Charged growth was 53 KB on C:, 0 on E: and 0 on the 5.1 engine.
  - `m51` matched P9 four times. No push and no tags.

**Owner actions**
- **Eye check:** none; there are no pictures.
- **On screen:** nothing Unreal is open.

**NEEDS-DECISION**
1. **The crash (RED):** how to proceed. One option: a single Load session with a re-entrancy guard on the editor script (G1), whose camera check logs the angles before failing. That would show whether the Niagara shutdown crash (G3) happens on a normal single load.
2. **The camera (G2):** if the saved camera roll is confirmed, EA_Core (an A6 output, FG1) needs re-authoring with the pitch set by name, and the A7 picture regions were predicted from the intended view.
3. **The locked Windows Update files (O2):** whether to add them to FE2's operating-system class.
4. **The build guard (O1):** whether a later leg must exercise it live.

Separately: several connectors (Google Calendar and the plugin servers) need authorizing in claude.ai settings or `/mcp`, and the PDF viewer server failed to connect. This job didn't use any of them.
api-equivalent cost (not billed on Max): $22.96 | turns: 164 | running total across all briefs: $5,239.92
