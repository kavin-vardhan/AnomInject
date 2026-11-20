<!-- Collected by Claude Code from runs\2026-10-16-094-01-europa-phase-a-plan\report.md (thread 01a14334, danger-full-access resume, exit 0, duration_s=998, 17:02:47-17:19:25 IST). Body below is byte-identical to report.md, sha256 BA6E8FB6D4B05AA7BF6D0D11F890860E8E7EA8285CDD3FC1388DC4AFC6CDDEC5. last-message.txt sha256 F8F81B68B59BC2AA8DEE101E3B553666C171A27E77F4CC4E80086F96C09A7F34 (9644 B). -->
094-47r stopped at the full P9 entry check, before source edits or Stage A builds. The stop is an unhandled receipt prerequisite: 1,021 inherited 5.1 build-metadata files do not match P9's recorded admission state. No code, audit rule, threshold or baseline was changed to clear it.

The fast entry and exit checks pass. The full `owner_p9.main()` invocation returned exit 1 after 160.278 seconds. Its inventory failure prevents the later full binary/config hash phase from running; this is not a full P9 pass.

| Component | This attempt |
|---|---|
| Stage A: OD7 include correction and OD8 predicate/audit correction | UNRUN after entry stop; 0 repository files changed |
| New 4.25 Editor/Game and ReGate51 changed-module builds | UNRUN; 0 build attempts |
| Focused tests, audit can-fails, full strict-include obligation | UNRUN; obligations remain open |
| Editor `-game` protocol smoke | UNRUN; neither warmup-only nor completed-capture integration was exercised |
| Stage B: minimal dashboard component | UNRUN because Stage A did not pass; no branch, tests or desktop build |
| Stage B: package and packaged console C5 | UNRUN; no package folder, cook or capture |
| Stage C: equivalence, early stop, reconnect, second capture and real desktop connection | UNRUN |
| Plugin/dashboard commits and pushes | None |

The P9 diagnosis found:

- 653 `LiveCodingInfo.json` files and 368 UHT `Timestamp` files; all 1,021 are classified `engine51_metadata` by the unchanged EG1 classifier.
- 1,167,215 bytes in both the baseline and current inventory. File IDs are unchanged for all 1,021; 878 contents match the baseline and 143 contents differ.
- Every modification time is between 2026-11-05 09:45:03.838478 and 09:45:11.218056 UTC, inside the prior ReGate51 build window of 09:44:49.148018–09:48:22.454588 UTC.
- Zero per-operation `classified-changes.jsonl.gz` receipts under `D:/IA_Europa/Receipts/09447/ledger`. P9 therefore has no recorded admission for this state. The observations strongly associate it with that build; they do not reconstruct a missing per-operation receipt.

EG1 permits these bookkeeping classes but requires per-operation path/size/mtime/hash receipts. OD8 requires unhandled prerequisite defects to be reported. I retained the failing result and wrote a diagnostic only; I did not synthesize an acceptance ledger, alter P9 or rebase its inventory. The exact prerequisite for continuation is an authorized reconciliation of these 1,021 inherited metadata records with P9 under the unchanged EG1 rule.

Evidence: [P9 result](D:/IA_Europa/Receipts/09447r/owner-p9-result.json), [inventory comparison](D:/IA_Europa/Receipts/09447r/P9/entry/inventory-comparison.json), [diagnosis](D:/IA_Europa/Receipts/09447r/p9-entry-diagnosis.json), [all 1,021 path/hash readings](D:/IA_Europa/Receipts/09447r/p9-entry-difference-hashes.json), [stop receipt](D:/IA_Europa/Receipts/09447r/stop.json).

Before the stop, the independent OD8 reproduction confirmed all 32 existing B2 registration mismatches: all 32 source registrations are inactive under the evidenced 4.25 Development profile and active under the 5.1 profile. It used the unchanged evaluator and includes hashes of the engine, capture-feature and module declarations. This diagnoses the audit input; it is not a passing audit. [Reproduction and profiles](D:/IA_Europa/Receipts/09447r/audit-profile-reproduction.json).

No correction files were changed. The 14 first-header defects and 12 raw checks remain. No new bench predicate exists in this attempt, so its required truth table—none=false, IAIBench=true, fixture-only=false, both=true, Shipping=false—has not been tested. R1–R6/B1–B3 were not altered; profile binding, nonempty inventories, active mutants, wrong-profile rejection and full strict-include checks remain required. Shipping exclusion and the four default routes, including LOD corruption, remain unchanged. Camera stays opt-in and unqualified.

The attested prior 4.25 Editor/Game builds at `a231a577905b3ea6163500b5a5982f108c8c2183` remain prior results, not fresh 09447r passes. There are no new build logs to scan. The previous ReGate51 compile remains failed on 14 first-header diagnostics despite exit 0; the previous lever audit remains failed. No helper result is presented as live integration.

ReGate51 source stayed clean and detached at `40a8eaf983518fe1992e172434aa8caf347654f1` throughout. Its binaries remain the inherited mixed, unqualified set; the Capture and ControlServer DLLs are from the failed `a231a57` candidate. No temporary checkout, rebuild, restore or 5.1 launch occurred, and the tree is not claimed as a qualified `40a8eaf` baseline.

All 38 live/archive artifact pairs were rehashed: 0 missing files, 0 hash mismatches and 0 mtime mismatches. OD9 allowed the existing copies to be referenced; 0 new binary copies were made. [Complete inventory with live paths, archive paths and hashes](D:/IA_Europa/Receipts/09447r/existing-binary-archive-rehash.json). The following EXE/DLL paths are relative to `E:/IA_BuildCache/09447-evidence/binary-archive/`; hashes are SHA-256. Inclusion of an existing Shipping artifact is an inventory reading, not a Shipping build or launch.

| Archive-relative EXE/DLL | SHA-256 |
|---|---|
| `425/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe` | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| `425/Binaries/Win64/StylizedParisStreet.exe` | `b5c1a94fb79d292af07a1354957b1252be5cc47d599546e5a33381af36a13ea2` |
| `425/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll` | `2b19de1d4f1628d5dac9b8968bed9ea62cfef6326d6d4f7edac7e932bb682595` |
| `425/Binaries/Win64/UE4Editor-StylizedParisStreet.dll` | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| `425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll` | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| `425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll` | `8036d3cda8b4a91fab5ef101eac938b892f5284e0478c64ff57d6dee42a3bf4f` |
| `425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll` | `ddf6af9816e8a75b22b642438bf90077030392468c3d8a2173656bb79a25aa8e` |
| `425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll` | `00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d` |
| `425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll` | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| `ReGate51/Binaries/Win64/UnrealEditor-AnomalyBench.dll` | `128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c` |
| `ReGate51/Binaries/Win64/UnrealEditor-AnomalyCapture.dll` | `dc3195fdb4cb212fef0a2e3e28813aaf2b8d0a8ae52fa3e955ccfc441df344d2` |
| `ReGate51/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll` | `4e83f6cac88f6e99fc4dec598f574f455020d390d16c02a94e2e6cd234726aeb` |
| `ReGate51/Binaries/Win64/UnrealEditor-AnomalyInjector.dll` | `88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5` |
| `ReGate51/Binaries/Win64/UnrealEditor-AnomalyShaders.dll` | `f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c` |

| Capacity | Entry GiB | Exit GiB | Floor GiB |
|---|---:|---:|---:|
| C: | 22.207603 | 22.208447 | 10 |
| D: | 53.198719 | 53.108704 | 50 |
| E: | 51.336967 | 51.318455 | 50 |

No build, runtime session or other physical stage started, so there are no per-stage before/after readings or C: session-growth measurement. All floors remained satisfied. The 3 GiB D: runtime reserve was not reduced. E: exit headroom was 1.318455 GiB; D: headroom above its floor and runtime reserve was 0.108704 GiB. A full OD9 package/Saved/Intermediate/DDC/archive/session/review peak budget was not reached after the entry stop, so capacity is not approved. No package size, cooked-resource proof or capture result exists.

Safety: 0 runtime crashes, upload signs, tokens read/generated/copied, deletions, compression operations or owned editor/game/build children. All invoked read-only helper processes finished. Foreign Python/Node processes were recorded and left untouched. Host Content/Config/descriptors, plugin assets and binaries, fixture assets, protected m51 state, refs/tags and drive-root names match entry. The inherited engine-metadata discrepancy is explicitly reported above; no new protected-file change was observed. The new evidence and package folders on E: were not created.

Writes were confined to the fresh receipt/scratch locations and new run-folder helper files, plus the required report replacement. The previous report was preserved hash-equal as `D:/IA_Europa/Receipts/09447r/prior-09447-report.md` before replacement. [Replacement receipt](D:/IA_Europa/Receipts/09447r/report-replacement-intent.json), [entry snapshot](D:/IA_Europa/Receipts/09447r/entry.json), [exit snapshot](D:/IA_Europa/Receipts/09447r/exit.json), [process readings](D:/IA_Europa/Receipts/09447r/processes-closeout.json). No source/runtime setting or bench lever changed. There was no scope or gate relaxation; the deviation from the planned sequence is the documented earlier entry stop.

Final plugin tip is unchanged: `feat/europa-port` = `a2250fda7a9b2f3754fd13c6061fe632f45b9d6e`; origin remains `c5d0919e5074404d26da8c16ccf845b360bde383`. Dashboard remains clean on master `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`; `feat/europa-425` was not created. No commit subjects or pushed commits exist for this attempt. Camera D4/G3 failures, unrun GO3 and the plugin-publication hold remain open. LOCAL VALIDATION / NOT OFFICE-READY; the shipped 094-45r card is unchanged.

VERDICT: NEEDS-DECISION
