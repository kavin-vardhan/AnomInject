# REPORT for 094-47-europa-dashboard-packaged.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 094-47-europa-dashboard-packaged.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-05 14:27:01 | finished: 2026-11-05 16:38:00 | duration: 131 min | exit code: 0
context at last turn: 656836 tokens (66% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\094-47-europa-dashboard-packaged.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**In short:** The dashboard can't connect to a packaged 4.25 build yet: Codex never built the package. The 4.25 connection code is written and compiles on 4.25, but the job stopped at the required 5.1 compile check. Nothing was pushed, and no dashboard change exists. Keep using `_reviews\094-45r-office-card-europa.md`; the new 094-47 card is marked LOCAL VALIDATION / NOT OFFICE-READY.

**Two new problems need your decision.** First, the 5.1 compile check rejects 14 include-order errors in code that is already on origin. Second, even with that fixed, the remaining stages won't fit in the free disk space under the current rules.

## Stages
**Before launch:** I independently confirmed the compiled starting point (OD2):
- The 09446r Editor and Game builds both exited 0 and passed, at commit cd2d60c.
- All 27 live binaries match the 09446r archive.
- cd2d60c..f3fbba7 changes only tools and docs (35 files).

Codex then resumed thread 01a14334 at 14:36 IST and exited 0 at 16:26 (6,590 s) with NEEDS-DECISION. The PC was in modern standby from 15:30:45 to 16:13:50; Codex had already stopped at 15:23, so nothing was interrupted.

**Stage A:**
- **Code (by reading the diff):** it matches the plan.
  - The 4.25 transport binds to 127.0.0.1 only, sends raw binary with no length prefix, reassembles fragments, has bounds (1 MiB in, 16 MiB out, 8 connections), dispatches on the game thread after service, and tears down once.
  - Console and dashboard use one C5 start path. The warmup timer can be cancelled, refused starts return an error, and a dashboard start keeps the game running.
  - `capture_start` accepts only the preset: PNG, native size, seed 4242 by default, 2352 frames. The include fix is in.
- **Tests:** the wire selftest passes 543 checks, and all 4 deliberately broken versions fail. These cover the helper code only, not a live connection.
- **4.25 builds:** Editor and Game both pass at a231a57 (22.6 s and 33.1 s, exit 0, no compiler errors).
- **5.1 compile (ReGate51):** fails the gate. UBT exits 0 and links, but prints 14 "Expected X.h to be first header included" errors.
  - **New finding:** the cause is an include order added by a2cc30d on 2026-10-31, which is already on origin.
  - The last 5.1 compile of the port (094-21, at 40a8eaf) had 0 of these errors, so this job's compile is the first to see them.
- **Protocol smoke:** not run.

**Stage B:** not run. No package, no packaged C5 capture, no dashboard branch, tests or desktop build.

**Stage C:** not run.

## Pushes and card
- **Plugin:** not pushed (OD4). Local is a2250fd; origin is c5d0919 (confirmed with ls-remote). The scan of c5d0919..a2250fd is clean apart from the profile path already public on origin: `_reviews\094-47-prepush-scan.md`, marked PUSH HELD.
- **Dashboard:** nothing pushed, because there's nothing to push. master is still 7ea2e51 and `feat/europa-425` doesn't exist (`_reviews\094-47-dashboard-prepush-scan.md`).
- **Card:** `_reviews\094-47-office-card-europa-dashboard.md`. Full review: `_reviews\094-47-code-review.md`.

## Safety
No safety problems:
- No crash, no upload sign, and no runtime launch at all.
- No token was generated, and none appears in commits or reports.
- The Europa host's own Content, Config, descriptor and .uproject are hash-equal before and after. Both 4.25 and 5.1 material `.uasset`s are unchanged.
- No files were removed, P9 is unchanged, and m51 passes the P9 git check at start and exit.
- Host and plugin binaries changed only while builds were running.

One side effect to know about: ReGate51's source is restored to 40a8eaf, but two of its plugin DLLs are now from this job's unqualified a231a57 build. Rebuild it before relying on it.

## Decisions for root Codex / the owner
All new; OD1–OD6 stand.
1. **The 5.1 first-header errors.** Choose one:
   - **(a)** Allow fixing the include order in the 14 AnomalyCapture files on feat. That goes beyond GQ2's transport-only scope.
   - **(b)** Accept "links with exit 0, only these 14 pre-existing errors" for GQ2, logged as a 5.1 debt to fix before any 5.1 merge. That changes what the gate accepts.
   - **(c)** First run a 5.1 compile of f3fbba7 to show this job adds no new 5.1 errors. That is evidence only; it doesn't clear GQ2 on its own.
2. **Disk space (my estimate).** E: has about 1.3 GiB above its 50 GiB floor; D: has about 3.2 GiB, before the kit's 3 GiB launch reserve. Each C5 capture needs about 1.6–1.8 GiB on E:. The smoke, the package and Stage C need several times what's left, and OD3/OD5 forbid deleting or compressing to make room. This needs more space, a different evidence location, or fewer full captures.
3. **Inherited holds, unchanged:** camera_clipping still fails D4 and strict G3 at D3D12 1279×719, and GO3 is unrun. The plugin push hold stays. The 5.1 runtime regression is still required before any 5.1 release merge.
api-equivalent cost (not billed on Max): $20.98 | turns: 152 | running total across all briefs: $5,840.65
