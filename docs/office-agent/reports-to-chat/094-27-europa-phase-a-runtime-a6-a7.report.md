# REPORT for 094-27-europa-phase-a-runtime-a6-a7.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-27-europa-phase-a-runtime-a6-a7.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-30 07:00:22 | finished: 2026-10-30 08:53:57 | duration: 114 min | exit code: 0
context at last turn: 624752 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-27-europa-phase-a-runtime-a6-a7.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. Codex built, rehearsed, armed and dry-ran the 4.25 runtime kit. The first real editor session (the smoke check) closed cleanly, but the kit's own reader wrongly failed it, so the run stopped before any of the 17 test legs. There was no crash, no upload, and nothing protected changed.

**Plain summary for the owner**
1. One short Unreal session ran this morning at 08:08 (15 seconds) and closed normally.
2. A bug in our checking script failed that session's otherwise good result, so no anomaly tests ran and there are no pictures yet.
3. Nothing is left running, and nothing needs closing.

**Start state (07:02 IST)**
- m51 matched P9: `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries.
- The protected-state check passed. 110 of 110 bound files matched, and P9 v2 is unchanged.
- The feature branch was at `1686c4d`, clean. The three A6 packages matched 094-26d byte for byte.
- EditorRuns held only `31124`. The Epic launcher was running, with no Unreal process.
- Free space: D: 70.5 GiB, E: 291.6, C: 31.0. Free commit 14.8 GiB, free physical 8.7 GiB.
- 094-26d's report and review show no blocking GAP.

**The Codex job**
- Run folder `…\runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334` (resumed on the first try).
- Ran 07:07:10–08:38:55 (5,505 s), exit 0. No pauses, no capacity retries, no foreign drive-root entries.
- Codex's verdict: `NEEDS-DECISION | Runtime kit prepared; real Smoke stopped on an incorrect cleanup reader; all runtime gates and pictures remain unproved.`

**FB2–FB4**
- **Kit:** commit `599ef70`, with 18 single-use legs: Smoke, Load, two PIE legs, GC, 12 A7 legs and Admission. Limits are 300, 600 or 900 s per leg. It binds only the 27-file 4.25 manifest.
- **Design note:** I checked 16 of its 4.25 source citations and all are correct.
- **Tests:** Codex's 12 suites passed.
- **My can-fails:** 56 of 56 behaved as expected:
  - out-of-scope legs and older armings are refused;
  - Content, config and build changes stop the run, between legs and during a session;
  - input- or GPU-contaminated events don't count;
  - the session limit stops the owned tree and spares an unrelated process.
- **Rehearsal:** 5 stand-in cases passed. All 18 dry runs passed with 0 launches.
- **Live reading:** clean, and my own 3-poll compare matched it.

**The real run, per leg**

| Leg | Gate wait (IST) | Start–end (IST) | Duration | Exit |
|---|---|---|---|---|
| Smoke (PID 2344) | 08:07:20–08:08:19, 3 quiet polls | 08:08:27–08:08:43 | 15.3 s | 0; all 31 Python bindings found; reader FAIL |
| The other 17 legs | — | — | — | Not run |

**The gates**

| Gate | Reader | Mine |
|---|---|---|
| A3 | NOT_RUN | agree |
| A5 | NOT_RUN | agree |
| A6 | NOT_RUN | agree |
| A7 (valid / applied / reverted) | NOT_RUN: 0/0/0 for all four anomalies | agree |
| A8 | NOT_RUN | agree |

**The pictures:** none exist. `D:\IA_Europa\Receipts\09427\contact-sheet-index.json` lists 0 contact sheets and says so.

**My review** (`_reviews\094-27-code-review.md`, 49 rows): 40 OK, 1 not run, 1 stopped, 2 N/A, 4 carried, 1 GAP. No ruling row is a FAIL.
- **G1 (blocking, confirmed):** the reader looks for a `world_snapshot_skipped` row labelled `cleanup` (`runtime_readers.py:26`). The probe actually writes `world_cleanup` with `actor_snapshot=skipped` (`EuropaPhaseAWorldProbe.cpp:211-225`), so every real leg would have failed. The stand-in and the reader test both copied the reader's own wrong format, which is why the rehearsal didn't catch it. On a copy of the real smoke files, every other check passes.
- **G2:** in 4.25 the crash reporter restarts itself outside the editor's process group (`WindowsPlatformCrashContext.cpp:493-536`). The kit never saw it, so its time limit can't stop it and its command line and environment aren't recorded.
- **G3:** every runtime command line carries `-unattended`, but EX4 ruled it off. The design note doesn't mention it.
- **Observations:**
  - O1: any failed leg ends the whole run, which is stricter than FB5 (only a crash stops later legs);
  - O2: each A7 leg tries exactly 6 events, with no extra events if some are contaminated;
  - O3: the game legs' privacy switches are present but untested in a real session.
- **My own checks:**
  - **Before/after lists:** nothing in Content, config, the descriptor or the `.uproject` changed. 33 Saved/Intermediate changes, all engine bookkeeping we record rather than stop on.
  - **Command audit:** 176 commands, 175 through the filtered wrapper, 1 direct read-only, 0 direct writes.
  - **Canary:** 0 matches in 2,011 files; my planted control was found 3 of 3 times.
  - **Launches:** exactly 1 editor launch.
  - **Crash folders:** none new.
  - **Ledger:** 0/0/0 bytes charged on C:, E: and the 5.1 engine.
  - **Python 2.7 harness:** passed when I ran it myself.
  - **Proofs:** Codex's EA8 entry and exit proofs passed; m51 matched P9 four times.
  - **No push, no tags.**

**Owner actions:** none. There are no pictures to check, and nothing Unreal is open.

**NEEDS-DECISION:**
1. Fix the cleanup reader (G1) and test it against real probe files (094-26d's two sessions and this smoke). Then re-arm fresh: the 09427 kit is spent.
2. G2: how to cover the crash reporter. One option is to adopt any process whose command line names an owned editor's `-MONITOR=<pid>`; or accept it as a recorded limitation.
3. G3: may runtime legs carry `-unattended`, given EX4?
4. O1 and O2: should a non-crash gate failure let later legs run, and may A7 legs take extra events?

Separately: several connectors (Google Calendar and the plugin servers) need authorizing in claude.ai settings or `/mcp`, and the PDF viewer server failed to connect. This job didn't use any of them.
api-equivalent cost (not billed on Max): $17.72 | turns: 117 | running total across all briefs: $5,216.96
