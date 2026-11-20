094-19 — FIX the probe's exit crash and REBUILD, then RESEARCH the launcher spawn, the uploads and the texture ensure, under chat's ruling 094-18 (ET1–ET7). No editor or game launch. Requested by chat Claude (the orchestrator) on 2026-10-27.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-18-ruling.md (ET1–ET7). It is new.
2. _reviews\094-17-owner-leg-ruling.md (ES1–ES5), _reviews\094-18-request.md (what remains of it), _reviews\094-17-ruling.md (ER1–ER4), _reviews\094-14-ruling.md (EO1–EO6), _reviews\094-13-ruling.md (EN1–EN5), _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (STOP RULES, DELIVERABLE), with everything they bind.

STARTING POINT
- feat/europa-port = b20e66e. Probe source 550e9b7, still identical in all three hosts.
- Bound manifests, until ES4 re-binds them:
  - 4.25: 27 files (4beccca6…);
  - Baseline51: 39 (617a9082…);
  - ReGate51: 44 (f6e52a85…).
- E:\EpicGames\UE_4.25\Engine\Intermediate\EditorRuns\31124 stays in place (ET2).
- The Epic Games Launcher may still be running. If it is, its writes are foreign under ET3.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains.
2. ET2: record the 31124 marker's receipt and disposition.
3. ET7: scan the two owner-slot temp files if they have been released. Otherwise record that they are still locked.
4. The probe fix (ES2 and ET5), in the probe only, with one source for all hosts:
   - no actor iterator in any cleanup, teardown or garbage-collection-time path;
   - no FindObject, StaticFindObject or LoadObject in those paths either;
   - a recorded "skipped" row when the world is torn down at exit;
   - the static check, covering both pattern families, with a mutant for each.
5. ES4:
   - the six rebuilds, with EO3, EI2, EG2 and the EN1 makefile proofs;
   - EI4, after the Baseline51 rebuild;
   - re-bind the three manifests, with source labels.
6. Research, read-only. Findings and proposals only; change nothing.
   - ET3: what in 4.25 starts the Epic Games Launcher from the editor, and how to prevent it without any setting change.
   - ET4: the exact paths for ensure and crash report uploads, and the least-invasive way to keep both on the PC.
   - ET6: which texture triggered TextureDerivedDataTask.cpp:252, and whether it can affect pictures.
7. Exit, as in EO5:
   - EA11;
   - the canary scan from the launch second, and the EK1 scan;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5 and ET2. EN5's hard STOPs are unchanged.
- Any editor, game, commandlet, cook, UAT or ShaderCompileWorker launch is a STOP. Never run the kit.
- The same compile error surviving two fix attempts stops the job, counted per host.
- D2 applies to the static check: two attempts.
- The 9-hour work limit applies.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the ET2 and ET7 records;
- the fix, the static check and its mutants;
- the builds, the binary hashes and the re-bound manifests;
- the ET3, ET4 and ET6 findings, each with source citations and a proposal;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
