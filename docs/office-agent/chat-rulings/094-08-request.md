094-08 — IMPLEMENT Europa Phase A job A4a: the clean 4.25 builds and the strict-include pass, under chat's ruling 094-07 (EH1–EH6). Stop at the end of A4a. Requested by chat Claude (the orchestrator) on 2026-10-24.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-07-ruling.md (EH1–EH6). It is new.
2. _reviews\094-06-ruling.md (EG1–EG6), _reviews\094-05-ruling.md (EF1–EF3), _reviews\094-04-ruling.md (EE1–EE7) and _reviews\094-04-request.md (ED1–ED7).
3. The earlier rulings (EC, EB, EA, P).
4. Your plan: §D1 and §D2 and gates A1, A2 and A4, read with the 4.25 Paris host, as amended.
5. The frozen predictions, docs/predictions/094-europa-phase-a.md, with your dated addenda.

STARTING POINT
- feat/europa-port = 204724a. Its source is identical to 286f2ee, where both working compiles passed.
- P9 v2 = D:\IA_Europa\Receipts\P9\baseline-v2\full-sha256.jsonl (8e3ed095…). The owner may delete the v2-removed cache roots at any time.

ORDER
1. Entry
   - The EA8 entry proof against v2.
   - ED1 free space, the ED3 toolchains, and EF1 readiness.
   - If a frozen prediction for gates A1, A2 or A4 is stale, commit a dated addendum before the first build.
2. Evidence-parser can-fails for gate A1 (EH1), using synthetic inputs only.
3. EH2: clean the Paris host's port-owned build outputs, by receipt.
4. The three clean builds (EH1), one at a time, under EF1, with EG2 receipts:
   - Editor Development;
   - Game Development;
   - Game Shipping.
   Assert 14.16.27051 and SDK 10.0.17763.0 from the actual responses. Shipping must show no IA or probe action, receipt or output.
5. Warnings (EH3): fix, or list each with its cause.
6. The strict pass (EH4) on both 4.25 targets, with its two can-fails, exact restoration, and the normal rebuild after it.
7. EH5:
   - If source changed: re-run the three clean builds on the final SHA, and the ReGate51 Editor Development working compile on the same SHA.
   - Re-run the JsonExport set and the three mutants if C#, a descriptor or a .uproject changed.
   - Record the final 4.25 binary hashes.
8. Exit
   - EA11.
   - The EA8 exit proof against v2.
   - The ledger summary, with EG1 bookkeeping if any 5.1 compile ran.
   - Commit, with W4.

STOP RULES
Those of 094-07 and the rulings above. Also: an unexpected toolchain reading, and any IA or probe module in Shipping. The same compile error surviving two fix attempts stops the job. EF1's wait limit ends the job cleanly with NEEDS-DECISION "no memory window". The 9-hour work limit applies. No 5.1 Game, Shipping or Baseline51 build. No editor, game, commandlet, cook or UAT launch. No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a one-screen summary first, then:
- the entry and exit proofs;
- the parser can-fails;
- the EH2 cleaning receipt;
- each build: SHA, command, toolset lines, action counts by project, plugin and engine, exit code, duration, warnings by code and location, and binary hashes;
- the Shipping exclusion evidence;
- the warnings ledger, with causes;
- the strict pass: coverage counts, unity and PCH evidence, the can-fails, the restoration and the normal rebuild;
- any source changes, with their 5.1 effect class and the ReGate51 re-compile;
- the final 4.25 binary hashes for A6–A8;
- what A4b must know.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
