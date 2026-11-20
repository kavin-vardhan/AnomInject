094-07 — FINISH Europa Phase A job A3, under chat's ruling 094-06 (EG1–EG6). Stop at the end of job A3. Requested by chat Claude (the orchestrator) on 2026-10-24.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-06-ruling.md (EG1–EG6). It is new. EG1 replaces ED4's 5.1 clause.
2. _reviews\094-06-request.md and _reviews\094-05-ruling.md (EF1–EF3), as amended.
3. _reviews\094-05-request.md, _reviews\094-04-ruling.md (EE1–EE7) and _reviews\094-04-request.md (ED1–ED7).
4. The earlier rulings, then your plan, sections C1–C5, plan §D2 and plan §H.

WHAT CHANGED SINCE YOUR LAST TURN
- Your STOP was correct. EG1 now admits the 5.1 build bookkeeping you traced: UHT Timestamp files, LiveCodingInfo.json, ActionHistory.bin, the caches, makefiles and manifests, under the engine's and the engine plugins' Intermediate\Build, plus UBT's own UbtRuns markers. The cap is 100 MB per job, with per-operation receipts. It applies the same way to failed, EF1-stopped and cut-off runs. Your 1,023 inherited files are accepted as they are.
- Still a STOP: engine or engine-plugin binaries, generated C++, Source, Content, Config, Shaders, Programs, and any engine compile or link action.
- EG5: Claude Code has written P9 v2 = v1 minus the owner's delete-list roots, whether or not they are deleted yet. Code's added context gives the paths and SHA-256s. The owner may delete those roots at any time, even during your job; their absence is not a difference under v2.

ORDER
1. Entry
   - Re-derive v2 from v1 and Code's removal manifest, byte for byte. A mismatch is a STOP.
   - Then the EA8 entry proof against v2.
   - ED1 free space; the ED3 toolchains.
2. Tooling
   - EF1, the build memory gate (thresholds and in-run stop as ruled), with synthetic self-tests.
   - EG2: build receipts hash every produced binary, and record the effective /std and PCH flags.
   - EG3: fix tools/europa/tooling_self_tests.py, with a mandatory --output.
   - Commit.
3. EF2 cleaning, inside D:\IA_Europa\ReGate51\StackOBot only, by receipt.
4. EG6 item 3: the 8 first-include fixes. Own header first, then the version include, keeping the outer 5.1 implementation guard. Each own header must be safe on 4.25 (C3). Commit.
5. Working compiles under EF1, commit-bound, with full ED2 and EG2 fields:
   - 5.1 StackOBotEditor Win64 Development in ReGate51, to 0 errors, with 0 engine and engine-plugin actions, and every engine-tree write inside EG1;
   - 4.25 StylizedParisStreetEditor Win64 Development on the same final SHA, to 0 errors.
   - Fix every new port warning, or list it with its cause.
6. EG1's clean-engine query: one action-plan query (no compile) for Baseline51 StackOBotEditor Win64 Development, with -NoEngineChanges and -WriteOutdatedActions. It must show 0 outdated engine and engine-plugin actions.
7. Exit
   - Re-run the full JsonExport set and the three mutants if C#, a descriptor or a .uproject changed since the last set.
   - EA11.
   - The EA8 exit proof against v2, and the ledger summary.
   - Commit, with W4.

STOP RULES
094-06's, with EG1 replacing ED4's 5.1 clause. Also:
- a v2 mismatch;
- outdated engine actions in step 6.
EF1's wait limit ends the job cleanly with NEEDS-DECISION "no memory window". The 9-hour work limit restarts with this job. No editor, game, commandlet, cook or UAT launch. No Game or Shipping build. No push. No tags.

DELIVERABLE
As in 094-06, plus:
- the v2 re-derivation;
- the memory-gate log;
- the EG1 bookkeeping per operation (counts, bytes, classes);
- the final compile pair, with binary hashes and /std and PCH flags;
- the Baseline51 clean-engine query result;
- what A4a and A4b must know.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
