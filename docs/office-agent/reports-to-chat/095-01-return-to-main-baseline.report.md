# REPORT for 095-01-return-to-main-baseline.md
verdict: NEEDS-DECISION (derived: NEEDS-DECISION section) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-01-return-to-main-baseline.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-11-13 12:09:41 | finished: 2026-11-13 12:16:07 | duration: 6 min | exit code: 0
context at last turn: 359464 tokens (36% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-01-return-to-main-baseline.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
095-01 REPORT TO CHAT - return to main + baseline cadence
Date 2026-11-13. Claude Code, Opus 5.5 high. No Codex. Stopped at Part 1; nothing changed.

1. VERDICT: NEEDS-DECISION
   Disk floor on E:. The brief's Part 1 stop rule fired: "the rebuild or the capture legs would
   take a volume below its floor". E: is already below its floor, and the rebuild would take it
   lower. Parts 2-6 were not started.

2. FOR THE OWNER (plain words)
   - The main branch has NOT been moved yet, and the main folder is still on m51. Nothing changed.
   - Nothing was built. E: (where this project's build files and captures live) has 22.7 GB free.
     The standing rule needs 50 GB, and this rebuild would use about 14.5 GB more.
   - No capture was run, so there are no baseline frame-skip numbers yet.

3. GIT (before = after; nothing moved)
   ref                    local     origin
   master                 b5f15a3   b5f15a3
   release/2026-10-05     d7eee97   d7eee97
   m51                    53bf725   (local only)
   fix/followup-capture   7754fd2   1bf7054
   feat/europa-port       2828f6a   2828f6a
   - Primary checkout: m51 @ 53bf725. 15 untracked files, 0 modified, 0 staged.
     Index SHA-256 526ABA7F...4C9EA35E, the same at the start and the end.
   - git fetch origin printed nothing, so no remote-tracking ref moved. No push.
   - master is a strict ancestor of d7eee97, and no worktree has master checked out. Part 2 can go
     ahead once disk is resolved.
     Side note: release/2026-10-05 is still registered to the prunable worktree
     E:/IA_BuildCache/_r917. That does not block anything, and I did not prune it.
   - The 15 untracked files are untouched: not copied, not moved. Part 3 never started.
   - No Unreal, build or shader process is running. Free physical memory is 7.45 GiB.

4. BUILD
   Not run. Why:
   - E:\IA_BuildCache\StackOBot\Intermediate is EMPTY (0 files) and Saved is empty. Binaries holds
     2.50 GiB.
   - So Part 4 would be a full rebuild from nothing. The last recorded full-rebuild Intermediate
     was 14.54 GB (runbook 8.6 / G130).
   - Expected E: after the build: about 8 GiB, plus about 1-2 GiB for the B1/B2 sessions.

   Disk now and floors:
   drive  free        floor  status
   C:     20.84 GiB   10     ok
   D:     51.36 GiB   50     ok, only 1.36 above the floor
   E:     22.69 GiB   50     already below
   - Intermediate, Binaries, Saved, the bench bank and _binary_baselines are all junctions onto E:.
   - Where the floor comes from: runbook 8.6 step 0b ("E: FLOOR - 50 GB, NO-GO for a cook, a build
     or a bench round"). The text is the same on d7eee97, on fix/followup-capture and on
     feat/europa-port.
   - 094-47r (owner-delegated) kept the D:/E: 50 GiB floors and said not to use weaker constants as
     authority.
   - The lower Europa schedules each covered one named continuation only: 094-47t used E: stop 10,
     094-48 used 10/25+3/10.
   - The projected ~8 GiB would break even that 10 GiB stop floor.

5. BASELINE
   No legs ran and predictions-095-01.txt was not written. Both are Part 5, after the stop point.
   B1 and B2 have no readings.

6. HOST AND FIXTURE vs THE RELEASE PROOF (read-only findings; nothing aligned)
   - The release proof (091-07 report.md) ran on "the named detached host" under
     E:\IA_BuildCache\_r917. 091-03's evidence was under _r913.
   - Both folders have since been deleted. The _r917 plugin worktree survives only as a prunable
     registration.
   - Proof settings: -IAIBench, fixture CB_ProxyDetail, StaticMeshActor_77, seed 777, config
     0 192 48 192 8, 30 fps, height 1024, CaptureBench.Marker 0. A temporary device profile was
     used and removed afterwards.
   - Primary host today:
     - Plugins = AnomalyInjector, CaptureBench (git, master @ e64d6f9, 8 untracked tools/*
       files), RoomGenerator (not git), unreal-mcp (not git).
     - Config: DefaultEngine.ini dated 2026-07-13, DefaultGame.ini dated 2026-08-21.
   - Release archive still present: _binary_baselines\release-2026-10-05-23098fa-218BA4C7.
   - Which CaptureBench revision the _r917 host carried is not recorded in report.md. Still to be
     found when Part 3.5 runs.

7. NEXT FREE NUMBERS (checked across every local and origin ref, docs/ + CLAUDE.md)
   - Milestones: m56, m57 and m58 appear nowhere; the highest mNN token is m55. Next free: m56.
   - Gotchas: the highest is G573 (feat/europa-port). Next free: G574.

8. SAFETY: none. Nothing deleted, nothing killed, no token printed, no write inside any repo.
   Receipt: D:\IntrusiveAnomalies\_relay\runs\2026-11-13-095-client-feedback\code-receipts-095-01\preflight.txt

   DECISIONS CHAT MUST MAKE
   (a) How E: gets space, or which floor applies. The obvious source is
       E:\IA_BuildCache\0944*-evidence / -package (Europa 094-44e..094-48c receipts), 52.77 GiB in
       total:
       folder              GiB
       09445r-evidence     13.32
       09446r-evidence      9.10
       09446-evidence       5.92
       09448-package        4.41
       09446s-evidence      3.52
       09445-evidence       2.59
       four packages        2.21 each
       These are Europa evidence (frozen under M4), so clearing or moving them is an owner decision.
       Briefs delete nothing. A one-brief lower E: floor alone is not enough: the full rebuild
       projects to about 8 GiB.
   (b) Whether to re-issue 095-01 unchanged once E: is at or above 50 GiB. The pre-flight is
       otherwise clean: refs, worktrees, processes and the checkout all match expectations. Or
       whether to split it, so M1/M2 (master fast-forward and the checkout move, no disk cost) run
       before the build and legs.
```
api-equivalent cost (not billed on Max): $4.50 | turns: 23 | running total across all briefs: $5,911.78
