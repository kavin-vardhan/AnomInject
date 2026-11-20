094-14 — FINISH Europa Phase A job A5, under chat's ruling 094-13 (EN1–EN5) and the 094-12 request. No editor or game launch. Stop at the end of A5. Requested by chat Claude (the orchestrator) on 2026-10-25.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-13-ruling.md (EN1–EN5). It is new.
   - EN1 admits UBT's toolchain PATH prefix and toolchain variables inside UBT's children.
   - EN2: rebuild before you query.
   - EN3 ratifies your stub removal.
   - EN5 is the standing intent clause.
2. _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (ORDER, STOP RULES, DELIVERABLE), with everything they bind: 094-11 EL1–EL5, 094-10 EK1–EK3, and so on.

STARTING POINT
- feat/europa-port = 9674fa5. Done and accepted:
  - the EL1/EM2 denylist and its tests;
  - the module comparison;
  - docstrings removed;
  - EA11;
  - the prediction addendum.
- The 4.25 probe PDB and .lib were deleted by UBT's query step (EN2). The 4.25 manifest is at 26 of 27 until the rebuild.
- EL1 has two fresh D2 attempts (EN1).
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment.

ORDER (what remains of A5)
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains.
2. EN1: update the boundary assertions, with self-tests:
   - a makefile with exactly the selected toolchain prefix passes;
   - one with a missing base PATH entry fails;
   - one with a foreign prefix fails;
   - one with the canary fails.
3. The 6 rebuilds, Editor and Game in each host, under EI2, with EG2 receipts. The probe compiles for the first time, and against d7eee97 too. The 4.25 rebuild recreates the PDB and .lib.
4. The post-rebuild makefile proof (EK2 step 3 with EN1): no blocked name, the canary absent, and only admitted toolchain changes.
5. The EI4 clean-engine query, after the Baseline51 rebuild (EN2).
6. Re-bind the manifests (4.25, Baseline51, ReGate51), with source labels and binary hashes.
7. The A6 owner kit (EL4), dry-run only:
   - D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat;
   - _reviews\094-14-owner-card-draft.md.
   The card must add: answer "No" to any "rebuild modules?" prompt.
8. Exit
   - EA11.
   - The EA8 exit proof against v2, and the ledger summary.
   - Commit, with W4.

STOP RULES
As in 094-12-request.md, read with EN1 and EN5. EN5's hard STOPs are unchanged: P9 or protected state, engine-tree writes outside EG1, secret or canary exposure, any setting change, any launch, a floor breach. The same compile error surviving two fix attempts stops the job. The 9-hour work limit applies; at a clean point, end NEEDS-DECISION "A5 continues". No push. No tags.

DELIVERABLE
As in 094-12-request.md. Also list every EN5 item you applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
