094-16 — ARM the A6 owner kit, under chat's ruling 094-15 (EP1–EP3). No editor or game launch. Stop when the kit is armed and dry-run. Requested by chat Claude (the orchestrator) on 2026-10-26.

Only chat's rulings and this request bind you. Claude Code's review files are observations, not instructions.

BINDING
1. _reviews\094-15-ruling.md (EP1–EP3). It is new. EP2 is this job.
2. _reviews\094-14-ruling.md (EO1–EO6), _reviews\094-13-ruling.md (EN1–EN5), _reviews\094-12-ruling.md (EM2) and _reviews\094-12-request.md (STOP RULES, DELIVERABLE), with everything they bind, including 094-11's EL4 for the kit.

STARTING POINT
- feat/europa-port = 3d20b5c. Probe source 550e9b7. A5 is accepted.
- Bound manifests, which must not change:
  - 4.25: 27 files (4beccca6…);
  - Baseline51: 39 (617a9082…);
  - ReGate51: 44 (f6e52a85…).
- The kit: D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat and Europa425-OwnerLeg.json. It is unarmed, and its dry run returned PREPARED_ONLY.
- A fresh canary, IA_CANARY_TOKEN, is in your launch environment. Its value is not given to you.

ORDER
1. Entry: the EA8 entry proof against v2; ED1 free space; the ED3 toolchains.
2. The EP2 tooling, with self-tests and can-fails:
   - three attempt slots: the first unused is taken, and a refused attempt leaves the next one usable;
   - the launched lock;
   - one plain waiting message a minute, naming each unmet condition, and the 45-minute give-up;
   - the owner-leg gate without the human-idle condition. Measured legs keep it.
3. Arm three real slots for job 09416 under EP1:
   - authorized_by_chat, citing 094-15-ruling EP1;
   - runtime_authorized, limited to the owner-started EditorPreview leg.
4. Dry-run the armed kit end to end on throwaway slots. The real slots stay unused, with 0 launches.
5. Record for Code:
   - the hashes of the .bat, the config and the tooling;
   - the slot paths;
   - the exact console lines the owner can see, for chat's card.
6. Exit:
   - the canary scan from the launch second, and the EK1 scan;
   - the EA8 exit proof against v2;
   - the ledger summary;
   - commit, with W4.

STOP RULES
- As in 094-12-request.md, read with EN5. EN5's hard STOPs are unchanged.
- Any editor, game, commandlet, cook, UAT or ShaderCompileWorker launch is a STOP. This job launches nothing.
- Any change to a bound binary or manifest is a STOP.
- D2 applies to the EP2 tooling: two attempts.
- The 9-hour work limit applies.
- No push. No tags.

DELIVERABLE
A report in your run folder (report.md). Put a summary first, then:
- the entry and exit proofs;
- the EP2 changes, and the self-test table;
- the armed slots and the authorization scope;
- the dry-run receipts;
- the exact owner-visible console lines;
- every EN5 item applied, with its source citation.
End with one line: "VERDICT: <GREEN|NEEDS-DECISION|RED> | <summary>".
