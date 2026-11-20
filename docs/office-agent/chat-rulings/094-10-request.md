094-10 — IMPLEMENT Europa Phase A job A5: probe, readers and fixture readiness, with no editor or game launch, under chat's ruling 094-09 (EJ1–EJ5 and its "Next" list). Stop at the end of A5. Requested by chat Claude (the orchestrator) on 2026-10-25.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING, in this order
1. _reviews\094-09-ruling.md: EJ1–EJ5, and its "Next" items 1–8. It is new.
2. _reviews\094-08-ruling.md (EI1–EI7; EI2's memory gate applies to every build).
3. _reviews\094-07-ruling.md (EH5's final-binary binding) and _reviews\094-06-ruling.md (EG1–EG6).
4. The earlier rulings (EF, EE, ED, EC, EB, EA, P).
5. Your plan:
   - §C4, the probe and fixture rows;
   - §E1–E5: evidence, gates, pictures, delivered routes;
   - §H, the A5 row;
   - §C5, the content boundary.
   All as amended.
6. The frozen predictions, with your dated addenda.

STARTING POINT
- feat/europa-port = d30cbe0. The production source equals 489c828 and 1c4cce7.
- Manifests to keep bound:
  - 4.25: A4a's 27 files;
  - 5.1: Baseline51's 39 and ReGate51's 44.
  - A probe or host change rebuilds and re-binds only what it touches.
- P9 v2 (8e3ed095…). The build cache is gone (EI7); v2 already excluded it.

ORDER
1. Entry
   - The EA8 entry proof against v2.
   - ED1 free space. D: is now about 85 GiB; mind the 70 GiB start and 55 GiB warning.
   - The ED3 toolchains.
2. EJ4:
   - the allowlisted child environment, and effective-environment receipts (names, with masked values);
   - find, redact and log credential-like values in port-owned receipts;
   - delete regenerable makefiles holding one, by receipt.
   Never print a value.
3. Runtime readiness of the tooling ("Next" item 4):
   - the ledger's runtime attribution classes;
   - the idle gate exempting the descendants of the job's own processes;
   - the real idle collector in observe-only mode for at least 15 minutes, with its readings reported.
   All with self-tests.
4. The probe's behaviour (item 1) in all three hosts: identical in Baseline51 and ReGate51, and compiling against d7eee97. Then the screenshot driver (E3).
5. The reader can-fails (item 2) for gates A3–A8 and R3–R9, on synthetic inputs only.
6. Fixture recipes and manifests (item 3). Recipes only; no editor authoring.
7. The A6 content-admission analysis (item 6):
   - from 4.25 source and the package headers, with no launch;
   - a prediction table;
   - a content-isolation proposal if it is needed;
   - the draft A6 owner card.
8. Normal rebuilds (item 7), under EI2, with EG2 receipts. Only the targets the probe or host changes touch, in all three hosts, Editor and Game. Then the EI4 clean-engine query.
9. The dated prediction revision (item 8), committed before any authoring leg.
10. Exit
    - Re-run the JsonExport set and the three mutants if C#, a descriptor or a .uproject changed.
    - EA11.
    - The EA8 exit proof against v2, and the ledger summary.
    - Commit, with W4.

STOP RULES
Those of 094-09 and the rulings above. Also:
- any editor, game, commandlet, cook, UAT or ShaderCompileWorker launch. A5 has none;
- any credential value appearing in a new file.
The same compile error surviving two fix attempts stops the job. The 9-hour work limit applies; at a clean point, end NEEDS-DECISION "A5 continues". No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a one-screen summary first, then:
- the entry and exit proofs;
- EJ4: variable names only, file counts and the redaction log;
- the runtime-readiness changes and the observe-only collector readings;
- the probe and screenshot-driver design, mapped to gates A3–A8 and R3–R9;
- the reader can-fail table;
- the fixture recipes and manifests;
- the A6 content-admission prediction table, any isolation proposal, and the draft owner card;
- the rebuilds, with binary hashes and the updated manifests;
- the prediction revision;
- what A6 needs: idle windows, the owner leg, and stop risks.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
