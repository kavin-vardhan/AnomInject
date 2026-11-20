# REPORT for 094-15-europa-a5-reader-builds.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-15-europa-a5-reader-builds.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-25 22:40:00 | finished: 2026-10-26 02:08:41 | duration: 209 min | exit code: 0
context at last turn: 703061 tokens (70% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-15-europa-a5-reader-builds.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN — A5 is complete, the canary was absent, the owner kit was built and dry-run only, nothing launched, there is no blocking GAP, and P9 v2 is unchanged.

**For the owner**
- Codex finished the Europa A5 work: all six builds of the three test hosts pass cleanly, and the double-click starter for your hands-on step (A6) is built and tested without starting anything.
- I re-checked the main claims myself and they hold: the build-log reader, the build-environment check, the protected files, and the leak check on the harmless test value.
- Nothing is needed from you now. A6 waits for chat; when it comes, close Blender and other heavy jobs first.

**Start state**
- **`m51`:** head `53bf725`, index `526aba7f`, 0 tracked changes, 15 untracked, equal to P9's record at 22:42, 23:03 and again by the launch wrapper. The gate's can-fail set passed 5/5.
- **Protected state:** PASS. P9 v2 is unchanged (`8E3ED095…`, 472,552 lines).
- **Resources at 22:42:** free space C 18.95, D 82.41, E 232.55 GiB. Toolchains are byte-equal to 094-14's reading. The mailbox watcher was running.
- **EK1 pre-scan:** 36,716 files, 0 secret values. The 19 new hits are name-only.
- **Foreign writes:** E: fell to 137 GiB at 22:51–23:03 through a foreign 95 GB write to `E:\<personal-folder>`. It stayed far above the floor.
- **Launch inputs:** fresh canary (SHA-256 only, `26911a26…`), my before-snapshots, and 094-14's outputs preserved hash-equal. The resume prompt was built from the request verbatim plus my context and the implementer block, UTF-8 with no BOM.

**The Codex job**
- **Run:** `_relay\runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334`, ledger row `…-resume13` (appended once).
- **Timing:** 23:05:55 → 01:43:49 IST, 9,475 s, exit 0, one try. A new turn was confirmed at launch.
- **Its verdict:** `VERDICT: GREEN | A5 readiness complete; six rebuilds and protected-state proofs pass; no runtime launched`

**A5**
- **EO6:** `resume_self_tests.py` was repaired. It passes 17 cases, and a raw value is rejected.
- **EO1:** the reader passed attempt 1 offline before any build (both known answers, 794 5.1 lines, 3 can-fails). The first failed 5.1 run exposed "cancelled" status lines. Attempt 2 handles them and re-passed everything (800 lines). Both attempts are now spent.
- **EO2:** the fallback was not used on any build.
- **EO3:** 9 receipts. Four makefiles were removed by receipt, five were already absent, and none was an engine or protected file.
- **EO4:** one Baseline51 compile error (a private header the release plugin includes, 2 files). One probe-only fix, guarded by `ENGINE_MAJOR_VERSION`, cleared it. The five probe files are identical in all three hosts, and no production file changed.
- **Builds:** 4.25 Editor and Game, Baseline51 Editor and Game (against `d7eee97`), ReGate51 Editor and Game. All exit 0 with 0 C++ errors and 0 C++ warnings. 4.25 shows only the two accepted user-XML warnings.
- **EI4:** 0 outdated actions.
- **Manifests:** re-bound at 27, 39 and 44 files.
- **EN5 items (3):** the toolchain environment (`VCEnvironment.cs` 4.25:144–183, 5.1:152–180), the stale-output cleanup (`BuildMode.cs:321/568`), and the makefile refresh (`BuildMode.cs:530–534/788–792`).
- **Commits:** 5 local commits on `feat/europa-port`, head `3d20b5c`, all W4-correct. No push, no tag.

**My review** (`_reviews\094-15-code-review.md`): 52 OK · 0 GAP · 6 OPEN · 0 STOP violations · 0 conflicts.
- **OPEN 1:** the failed first Baseline51 run's makefile was removed (EO3) before anyone proved its environment. The regenerated one passed.
- **OPEN 2:** the reader's EO1 budget is used up; any future failure falls to EO2.
- **OPEN 3:** on 5.1 the probe reads the control subsystem through its own code path, with an assert if the class is missing. This matters at A9.
- **OPEN 4–6:** EJ1, EJ3 and EJ5 are carried over; nothing in this job exercised them.

What I checked myself:
- **EO1 reader:**
  - It returns exactly 5 and 3 executed lines on the two retained 094-14 logs.
  - The 094-14 reader still fails with my original reproduction, verbatim.
  - 9/9 of my own can-fails fire, 800/800 5.1 lines parse, and all 9 legs join, matching Codex's table.
  - Both engines' executor citations are exact in source.
- **EO3:** 9/9 receipts are correct. The 4.25 removal matches the makefile hash I recorded before launch.
- **EN1:** all 6 makefiles on disk pass my own checker, pinned to MSVC 14.16.27023 + SDK 10.0.17763.0 for 4.25 and MSVC 14.38.33130 + SDK 10.0.22621.0 for 5.1. The checker self-tests 17/17.
- **EL2 canary:** 0 value hits in 2,566 files changed since the launch second; 0 canary names in 157 child records.
- **Child environments:** 154/154 boundaries pass.
- **P9 v2:** 18,264 of 18,264.
- **Protected state:** PASS three times.
- **`m51`:** equal to P9 throughout.
- **Engine trees:** only allowed bookkeeping files changed (1,022 of them); 0 binaries; the 4.25 install is unchanged.
- **Self-tests:** all pass when I re-run them. That includes EO6's raw-value can-fail.
- **Launches:** none. 0 forbidden commands in 189.

Foreign activity during and after the job, none of it ours: a <other-home-project> job, Blender, and a UE 5.7 editor for another project (<other-home-project>) writing to LocalAppData after Codex exited.

**Owner kit**
- `D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` with `Europa425-OwnerLeg.json`.
- `_reviews\094-15-owner-card-draft.md`.

The `.bat` is six lines and goes through the approved wrapper. It refuses to start until chat arms a fresh job config, checks 27 bound binaries, 30 source and config files, the D: routing and P9 first, and sends logs, crash logs, shader files, user dir and temp to D:. The dry run reported "prepared only" with 0 launches.

The card covers everything required, including "No" to "rebuild modules?" and closing Blender. Notes for chat's polish:
- The file has one Windows-1252 dash (byte `0x97`), so it isn't valid UTF-8 and the title shows "�".
- Tell the owner to keep hands off after double-clicking: the P9 preflight takes several minutes before the five-minute quiet check.
- Name the "Don't Save" button for the exit.
- Say the kit will refuse until chat arms it.

**Owner actions:** none now. For A6, close Blender, other editors and heavy jobs first.

Several connectors are not authorized in this session (Google Calendar, GitHub, Slack and others), and the PDF viewer failed to connect. Nothing in this work depended on them.

**NEEDS-DECISION:** none.
api-equivalent cost (not billed on Max): $25.43 | turns: 189 | running total across all briefs: $4,864.91
