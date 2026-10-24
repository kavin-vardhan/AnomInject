# 094-07 Europa A3 working results

The shared C++ closure has working Editor Development builds on both engines. These are A3 working results; A4 owns the final normal/strict build gates and all runtime gates remain pending.

Source tested: `286f2ee0ca6ffc9e9e1fbd8c3caaefa01c8a91fd`. Receipts: `D:/IA_Europa/Receipts/09407`.

| Engine | Actions project/plugin/engine/engine-plugin | Errors | New port warnings | Seconds |
|---|---|---|---|---|
| 51port | 9/20/0/0 | 0 | 0 | 743.71 |
| 425 | 1/3/0/0 | 0 | 0 | 438.68 |

EG2 binary hashes and actual response flags are in each result.json; outputs remain in each host on D:. No C5043 suppression was added. The pre-existing 4.25 user-XML warnings remain listed; global XML was not changed. Two 5.1 C4996 warnings in MidReproActor.cpp:110/153 come from the copied host fixture, verified byte-identical to the original and Baseline51; the conservative initial parser marked them for review, and they are not new port warnings.

EG5 v2 was re-derived byte for byte; the retained D: target behind the excluded cache junction is still protected. EF2 removed only 100 receipted ReGate51 generated files (66,986,117 bytes). The engine bookkeeping inherited from the interruption was retained under EG1.

EF1/EG1/EG2 self-tests: 75 initially, 82 with final response/strict wiring checks; EE1/EE6: 16; delta ledger: 9; budget suite: 5; routing suite: 31. Mandatory --output rejects omission. No real memory-starvation or process-kill test was used. The strict tool removes the obsolete UHT environment override and gates both UBT calls; only fake command wiring was exercised, no strict build.

The first entry harness missed eight retained junction-target assets; its second attempt scanned the physical D: target and passed. The first build launcher referenced a nonexistent renamed helper; it failed before UBT and its import was corrected. These were separate harness problems, one correction each. No compiler retry is attributed to either.

The final report additionally records final-HEAD incremental confirmation, the single Baseline51 clean-engine query, the EA8 exit proof, complete per-operation charges and the A4 handoff. None is implied by this earlier working-pair receipt.
