094-06 — RESUME Europa Phase A job A3 after the 17:11 interruption, under chat's ruling 094-05 (EF1–EF3). Stop at the end of job A3. Requested by chat Claude (the orchestrator) on 2026-10-22.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-05-ruling.md (EF1–EF3). It is new.
2. _reviews\094-05-request.md, its ORDER and its STOP RULES, as amended by EF1–EF3.
3. _reviews\094-04-ruling.md (EE1–EE7) and _reviews\094-04-request.md (ED1–ED7).
4. The earlier rulings (EC, EB, EA, P), then your plan, sections C1–C5, plan §D2 and plan §H.

WHAT HAPPENED
- At 17:11 the PC ran out of memory during your 5.1 ReGate51 working compile (UBT 42980, cl 1572). Memory load was 98 %, with 0.40 GiB free physical and 3.4 GiB free commit; most of it was held by the owner's applications.
- Your process and Claude Code's both stopped at 17:11:3x. There is no report and no exit code.
- Your 17:01 result stands, subject to EF2: the 4.25 StylizedParisStreetEditor Development build on 5bd368b had 0 errors and 0 C++ warnings (C2855 9 → 0, C5043 20 → 0 under NoPCHs).
- The 5.1 leg's UBT reported 8 first-include violations before compiling.
- Claude Code's added context below gives the machine state now, the owner's cleanup result, which P9 baseline applies (v2 with its removal manifest, or v1), and anything it found still running from your job.

ORDER
1. Entry (EF2)
   - P9: if v2 exists, re-derive it from v1 and Code's removal manifest, byte for byte; a mismatch is a STOP. Then run the EA8 entry proof against whichever baseline applies.
   - Recovery checks:
     - git fsck --connectivity-only on the common repository (read-only);
     - the git state of feat/europa-port and of the three port worktrees;
     - SQLite PRAGMA integrity_check on every ledger the 094-05 job had open;
     - the 4.25 outputs against their 17:01 receipt hashes.
   - ED1 free space, and the ED3 toolchains.
2. EF1, the build memory gate, in the wrapper, with its synthetic self-tests. Commit.
3. EF2 cleaning: remove everything the interrupted 5.1 run could have written inside D:\IA_Europa\ReGate51\StackOBot, from its action list, by receipt.
4. The 8 first-include violations and any other 5.1 errors, inside C1–C5. Keep C3's rule that no UE5 expression is parsed in a disabled 4.25 translation unit.
5. Working compiles under EF1 (ED2 fields, commit-bound):
   - 5.1 StackOBotEditor Win64 Development in ReGate51, to 0 errors, with -NoEngineChanges and 0 engine and engine-plugin actions;
   - then the final pair on one SHA. If a file shared with 4.25 changed since 5bd368b, re-run the 4.25 compile on the final SHA.
   - Fix every new port warning, or list it with its cause.
6. Exit, as in 094-05:
   - re-run the JsonExport set if C#, a descriptor or a .uproject changed;
   - EA11;
   - the EA8 exit proof and the ledger summary;
   - commit with W4.

STOP RULES
094-05's, plus:
- EF1's wait limit ends the job cleanly with NEEDS-DECISION "no memory window" (not a failure).
- A stop of your own UBT tree under EF1 is not a D2 attempt.
- Never stop any process your wrapper did not start, and never by name.
- The 9-hour work limit restarts with this job.

DELIVERABLE
As in 094-05, plus:
- the interruption analysis: what was lost, and what was verified;
- the memory-gate log (waits, stops, and the readings at each start);
- the EF2 cleaning receipt;
- the final compile pair: SHA, toolsets, action counts, 0 errors, and warnings with their causes.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
