094-23 — FIX the launcher gate and the in-session rules, then RE-ARM the automated A6 fixture-authoring kit, under chat's ruling 094-22 (EX1–EX6). No editor or game launch. Stop when the kit is re-armed and dry-run. Requested by chat Claude (the orchestrator) on 2026-10-28.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-22-ruling.md (EX1–EX6). It is new.
   - EX1: exclude the launcher's whole process tree, and add a live gate reading.
   - EX2: the in-session rules.
   - EX3: quarantine of partial outputs.
   - EX5: the small items.
2. _reviews\094-21-ruling.md (EW1–EW4), _reviews\094-20-ruling.md (EV1–EV7), _reviews\094-19-ruling.md (EU1–EU6), _reviews\094-18-ruling.md (ET1–ET7), _reviews\094-17-ruling.md (ER1–ER4), _reviews\094-16-ruling.md (EQ1–EQ5), _reviews\094-14-ruling.md (EO1–EO6), _reviews\094-13-ruling.md (EN1–EN5), _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (STOP RULES, DELIVERABLE), with everything they bind.

STARTING POINT
- feat/europa-port = 1a9afc1. The kit binds 8abb79e.
- The kit is armed for 09422, with all 15 real paths absent. This job supersedes that arming.
- Bound manifests (EW1):
  - 4.25: 27 files (c501895c…);
  - Baseline51: 39 (b05e6f8c…);
  - ReGate51: 44 (3865fe0c…).
- The owner may have restarted the PC and the launcher. If so, record it.
- The 31124 marker stays in place (ET2).
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space (EV3's D: thresholds); the ED3 toolchains.
2. The tooling, with self-tests and can-fails:
   - EX1: the launcher-tree exclusion;
   - EX2: the in-session rules;
   - EX3: the quarantine of partial outputs;
   - EX5: the job-template wording.
   D2 applies: two attempts.
3. Re-arm for job 09423:
   - fresh slots;
   - EW2's scope;
   - the citations: EW2 and EW3, as amended by EX1–EX5;
   - the 09422 arming refused.
4. Dry-run both recipes on throwaway slots, then take a live, observe-only gate reading with the real collector (EX1). The real slots stay unused, with 0 launches.
5. Record for Code:
   - the hashes;
   - the slot paths;
   - every line the run can show;
   - the live reading.
6. Exit:
   - the canary scan from the launch second, and the full EK1 scan;
   - the EU1 command audit;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5, ET2, EU1, EU2, EV1 and EV3. EN5's other hard STOPs are unchanged.
- Any editor, game, commandlet, cook, UAT or ShaderCompileWorker launch is a STOP. This job launches nothing.
- Any change to a bound binary, a manifest or Paris Content is a STOP.
- The 9-hour work limit applies, and EU2's pauses count towards it. Report-only work after the limit is allowed.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the tooling changes, and the self-test table;
- the armed scope and slots;
- the dry-run receipts;
- the live gate reading;
- every line the run can show;
- the command audit;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
