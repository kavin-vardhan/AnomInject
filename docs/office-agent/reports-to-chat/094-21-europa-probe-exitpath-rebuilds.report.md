# REPORT for 094-21-europa-probe-exitpath-rebuilds.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-21-europa-probe-exitpath-rebuilds.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-28 01:30:16 | finished: 2026-10-28 04:11:51 | duration: 162 min | exit code: 0
context at last turn: 645475 tokens (65% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-21-europa-probe-exitpath-rebuilds.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
- Overnight, Codex closed the last known crash-at-exit path in the test probe and tightened the automatic check that guards it. It then rebuilt all six test programs, and every build passed on the first try.
- I checked everything independently and all of it agrees. Nothing protected was touched, no game or editor was launched, and no secret or canary value leaked.
- One small finding goes to chat (G1, not blocking): Codex's own environment receipt now ends up inside the build caches. The PC still needs your restart.

**Start state (01:32–01:48 IST)**
- **m51 = P9:** HEAD `53bf725`, index `526aba7f`, 0 tracked changes, the same 15 untracked entries. Its can-fail check caught all 5 of 5 cases.
- **Free space and memory:** C 20.4, D 74.3, E 33.5 GiB; commit free 22.7 GiB, physical free 10.1 GiB; no Blender running.
- **Protected state PASS:** release baseline 8/8, BankOverflow nothing missing or changed, StackOBot Content and the cache's StackOBot Binaries equal to their earlier snapshots. P9 v2 file `8E3ED095…` unchanged.
- **Feature branch and probe:** `feat/europa-port` at `1722348`. The probe source was identical in all three hosts and equal to the 1722348 fix.
- **Bound manifests:** 27/27 (`4beccca6…`), 39/39 (`617a9082…`), 44/44 (`f6e52a85…`).
- **Marker and launcher:** the 31124 marker is unchanged. The launcher is still the old instance (pid 26116); the old kit console is gone.
- **EK1 pre-scan:** 42,188 files, 0 secret values.
- **Launch:** held until D: read ≥ 40 GiB on two polls (it read 74.26).

**The Codex job**
- **Run:** run folder `runs\2026-10-16-094-01-europa-phase-a-plan`, thread `01a14334`, resumed at 01:49:07 and exited at 03:49:29 IST (7,223 s), exit 0. Ledger row `…-resume19`.
- **Pauses:** 0. D: never dropped below 73.54 GiB across 439 readings.
- **Codex's own verdict:** "VERDICT: GREEN | EV5 fixed and compiled in six targets; static mutants, EI4, 110-artifact bindings, P9 and scans pass; no runtime launch."
- **Commits:** `40a8eaf` (fix) and `7039df7` (documentation only). Both are local; nothing pushed, no tags.

**EV5 and the rebuilds**
- **Root changes:** they now happen only in a guarded `ChangeRoot`. `Stop()` makes no UObject access after the engine's exit purge; I walked every path from the source.
- **Skips at purge:** `ReleaseGC` and `ScreenshotDone` skip and leave a receipt.
- **World receipts:** they are keyed by address, so nothing builds a weak pointer from a dying world.
- **Static check:** it now also refuses `LoadClass`, `StaticLoadClass`, `TObjectIterator`/`TObjectRange`, `FindFirstObject`, and root changes outside the helper. It passes on all four sources with 25/25 self-tests each.
- **Source and production:** one probe source in all three hosts, byte-equal to the committed templates; `ENGINE_MAJOR_VERSION` still handles engine differences; no production file changed.
- **Builds:** all six exited 0 with `-NoEngineChanges -NoBuildUHT -MaxParallelActions=4` and 0 engine actions. Every EO3 receipt matches my own before-snapshot. The EI4 query after the Baseline51 rebuild found 0 actions.

New probe-related hashes:

| Host | Probe DLL | Game exe |
|---|---|---|
| 4.25 (Paris) | `9458cb95…` | `StylizedParisStreet.exe` `a2db7a02…` |
| Baseline51 | `ac3b1ca4…` | `StackOBot.exe` `b6def734…` |
| ReGate51 | `f8cab87d…` | `StackOBot.exe` `5b0fb171…` |

- **Re-bound manifests** are under `Receipts\09421\qualification`: `c501895c…` (27), `b05e6f8c…` (39), `3865fe0c…` (44). I re-hashed all 110 files and they match. Only probe-linked outputs changed; every plugin DLL is byte-identical.

**My review** (`_reviews\094-21-code-review.md`)
- **Counts:** 53 rows: 43 OK, 8 N/A, 2 OPEN (both carried items), 0 STOP, 0 stop violations.
- **GAP G1:** `EUROPA_BOOTSTRAP_PROOF`, a variable set by Codex's wrapper, now lands in all six makefiles. It holds the canary's *name*, the canary's hash, and SHA-256s of 84 variable values, including `USERNAME` and `COMPUTERNAME`. No value of any secret and no canary value is in it.
  - Because of it, my strict EN1 byte test fails on all six makefiles. Scoped to variable names, EN1 passes 6/6 with both engines' MSVC and SDK pinned.
- **O1:** the exit fix is proven from source only; no runtime leg has exercised it yet.
- **O2:** 8 related calls outside EV5's list still pass the check (for example `StaticFindFirstObject`, `FObjectIterator`, a plain dereference in `Stop`). None of them is in the probe today.
- **O3:** physical free memory fell to 0.08 and 0.48 GiB during the two 5.1 game links; commit free stayed at 14.3 GiB or more.
- **O4:** script, picture and dormant commands now refuse a null or tearing-down world, and probe receipts read "unavailable" for object identities once exit is requested.
- **Independent results:**
  - My 21 mutants of the named families were all caught.
  - My four disk-pause can-fails pass under the new thresholds, and the mutant is caught. The one red row, D's poll count, is my harness's old wrong expectation, already noted in 094-20.
  - My command audit: 117 wrapped commands, 1 direct read-only, 0 direct write-capable. No pause opened, so no edit fell inside one.
  - Engine-tree changes are only the 5.1 build bookkeeping already allowed, the same counts as in 094-15.
  - Canary by value: 0 hits in 2,858 files changed since launch.
  - Full EK1 after exit: 43,037 files, 0 secret values.
  - P9 v2 18,264/18,264; m51 = P9 at the end; protected state PASS at the end; 0 launches.
  - Self-tests: 159/159, 62/62 plus 48 measured, 48/48, disk pause 12/12, EA11 Paris and ReGate51 PASS (the Baseline51 control fails as expected).

**Owner actions**
- Between jobs: restart the PC (this clears `wlanext` at 9.4 GB), then run `start_watcher.bat`, then start the Epic Games Launcher normally.
- Separately: these MCP servers need authorizing (claude.ai connector settings, or `/mcp` interactively): Google Calendar and the design, engineering, marketing and productivity plugin connectors. The pdf-viewer server failed to connect. This job did not need them.

**NEEDS-DECISION:** nothing blocks. One call for chat, not blocking: G1. Either accept the proof variable as wrapper metadata, or keep it out of build and runtime children before the arming job. Also: the owner kit is still bound to the old 09415 manifest. It refuses the new binaries for now, so 094-22 must re-bind it.
api-equivalent cost (not billed on Max): $20.48 | turns: 152 | running total across all briefs: $4,999.50
