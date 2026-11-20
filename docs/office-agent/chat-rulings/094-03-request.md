094-03 — RESUME Europa Phase A jobs A1 and A2, under chat's ruling 094-02 (EB1–EB5). Stop at the end of A2. Requested by chat Claude (the orchestrator) on 2026-10-16.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-02-ruling.md (EB1–EB5). It is new and amends the write proof.
2. _reviews\094-01-ruling.md (EA1–EA15).
3. _reviews\094-02-request.md: the A1 and A2 work list and the deliverable. It still applies, except where EB1–EB5 change it.
4. Your plan, as amended by those rulings.

WHAT CHANGED SINCE YOUR LAST TURN
- Your stop was correct, and the P9 baseline stands: full-sha256.jsonl db884f1e…, 707,775 files.
- The write proof is now EB1's targeted check:
  - per-operation inventories of the exception paths, with budgets;
  - in-window modified-file scans of the declared locations;
  - command-line receipts showing every redirect;
  - free space used only as a floor monitor;
  - no ETW, no USN, no elevation, and no attempt at any of them.
  Writes in scanned locations by other processes (the owner's apps, UE 5.7, Windows) are recorded as foreign, not port-attributed.
- Measure the orchestration tools' folders each operation and report them as tooling overhead, outside the port budget: <USERPROFILE>\.claude, <USERPROFILE>\.codex, C:\ClaudeTemp.
- C: was at 10.28 GiB at your exit. Claude Code confirms C: is at 13 GiB or more and E: at 52 GiB or more before launching you (EB2). If C: falls below its warning margin (11 GiB) during your work, finish the current operation, measure, and continue only while C: stays at 10 GiB or more. The floor breach rule is unchanged.

ORDER
1. Entry: the EA8 per-job entry proof against your full baseline. That means git state, protected inventories (path, size, mtime), and full hashes of protected binaries and configs. Do not re-run the full hash.
2. A1, from where you stopped:
   - the three hosts and their worktrees (EA3), with copy manifests and source before/after inventories;
   - the routing (DDC graphs, wrappers, ledger tool) under EB1;
   - the idle gate moved from draft into port tooling;
   - commit the reviewed predictions on feat/europa-port before the first UBT invocation.
3. A2:
   - review your 22-file draft against EA1–EA15, then apply it;
   - the EA11 checker in the real worktree, plus its mutant;
   - both UBTs compile the shared C# rules;
   - the toolset selection shown from the UBT logs: 5.1 at 14.38.33130, 4.25 at 14.16.27051;
   - the JsonExport matrices on both engines, plus the removed-blacklist mutant (EA13).
4. Exit: the EA8 per-job exit proof, and the write-ledger summary.

STOP RULES
EA15 as amended by EB1–EB2. D2 applies: at most 2 attempts per harness problem. No editor or game launch. No full Editor or Game build; that is A4a/A4b. No push. Commits go on feat/europa-port only, with W4.

DELIVERABLE
As in 094-02-request.md. Also add:
- the EB1 ledger results per operation, including foreign writes and tooling overhead;
- C:, D: and E: free space at each operation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
