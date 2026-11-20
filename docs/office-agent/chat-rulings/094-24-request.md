094-24 — FIX G1 and the small items, REHEARSE the real branch with a stand-in editor, then RE-ARM the A6 fixture-authoring kit for 09424, under chat's ruling 094-23 (EY1–EY8). No Unreal launch. Stop when the kit is re-armed, rehearsed and dry-run. Requested by chat Claude (the orchestrator) on 2026-10-28.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-23-ruling.md (EY1–EY8). It is new.
   - EY1: G1's fix, the environment-read check, and the real-branch rehearsal.
   - EY2: EX1 narrowed to the launcher's own programs, by install folder.
   - EY3: O2's 60 s rule, O3's two blocking images, and the start gate's 12 GiB free-commit line.
   - EY4: E:'s new lines (start 15, warn 12, floor 10 GiB).
   - EY5: the owner's scope decision. The port is 4.25 only from now on.
   - EY6: this job. EY7: the later real run, which this kit must serve.
2. _reviews\094-22-ruling.md (EX1–EX6) as amended by EY1–EY4, _reviews\094-21-ruling.md (EW1–EW4), _reviews\094-20-ruling.md (EV1–EV7), _reviews\094-19-ruling.md (EU1–EU6), _reviews\094-18-ruling.md (ET1–ET7), _reviews\094-17-ruling.md (ER1–ER4), _reviews\094-16-ruling.md (EQ1–EQ5), _reviews\094-14-ruling.md (EO1–EO6), _reviews\094-13-ruling.md (EN1–EN5), _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (STOP RULES, DELIVERABLE), with everything they bind.

STARTING POINT
- feat/europa-port = 97de9d4. The kit binds 8c4da78 and is armed for 09423, with all 15 real paths absent. This job supersedes that arming.
- Bound manifests (EW1): 4.25 27 files (c501895c…); Baseline51 39 (b05e6f8c…); ReGate51 44 (3865fe0c…). The two 5.1 manifests are frozen records (EY5). The kit may keep checking them unchanged.
- E: had 19.1 GiB free at 16:48 IST, from the owner's own use. EY4's lines apply from this job's entry.
- The 31124 marker stays in place (ET2). The 09417 owner kit stays spent.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space with EV3's D: lines and EY4's E: lines; the ED3 toolchains.
2. The tooling, with self-tests and can-fails:
   - EY1: remove EUROPA_AUTHORING_MARKER; the environment-read check, with a mutant that reads a filtered name;
   - EY2: the launcher-tree exemption by install folder; an unreadable image path is not exempt; an editor or game under the launcher tree blocks;
   - EY3: the 60 s unreadable-reading stop; UnrealPak and UnrealLightmass block; the start gate's 12 GiB free-commit line, with its own Waiting line;
   - EY4: E:'s lines in the kit and in the entry checks.
   D2 applies: two attempts.
3. The real-branch rehearsal (EY1), for both recipes, with a passing and a failing stand-in. All roots are under owned scratch, and no Unreal process starts.
4. Re-arm for job 09424:
   - fresh slots;
   - EW2's scope;
   - the citations: EW2 and EW3, as amended by EX1–EX5 and EY1–EY4;
   - the 09423 arming refused.
5. Dry-run both recipes on throwaway slots. Then take a live, observe-only gate reading with the real collector: EY2's exemption by folder for the overlays and web helpers, and 0 blockers misread. The real slots stay unused, with 0 launches.
6. Record for Code:
   - the hashes;
   - the slot paths;
   - every line the run can show;
   - the rehearsal receipts;
   - the live reading.
7. Exit:
   - the canary scan from the launch second, and the full EK1 scan;
   - the EU1 command audit;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1, EV3 and EY4. EN5's other hard STOPs are unchanged.
- Any editor, game, commandlet, cook, UAT or ShaderCompileWorker launch is a STOP. This job launches nothing.
- Any change to a bound binary, a manifest or Paris Content is a STOP.
- The rehearsal never touches the Paris host, its Content or any real slot. A rehearsal write outside owned scratch is a STOP.
- The 9-hour work limit applies, and EU2's pauses count towards it. Report-only work after the limit is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the tooling changes, and the self-test table;
- the rehearsal receipts;
- the armed scope and slots;
- the dry-run receipts;
- the live gate reading;
- every line the run can show;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
