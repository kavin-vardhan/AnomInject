# 094-47s complete relay report, collected by root under OD14
verdict: NEEDS-DECISION

Collected 2026-11-05 19:00:56 IST. Worker ran18:25:18-18:53:38IST; wrapper finished18:53:39IST.
Process exit0 is successful report delivery, not gate acceptance.
Worker body SHA256 f815c87c8f45226e4612b24ab866728aef8a127a781204141e13e6d43d478d0b.
The complete worker report follows verbatim. Its passing reconciliation is narrower than full P9 or qualification.

---

094-47s stopped at the mandatory full P9 entry gate, before source repair or Stage A. OD11's inherited-metadata reconciliation passed. Canonical P9 then reported `OD12 floor: D` during binary/config hashing and exited 1 after 257.571 seconds. A later reading confirmed D: at 45,789,945,856 bytes free (42.645210 GiB), below its 50 GiB floor. No retry, threshold change or cleanup was used to continue.

Packaged dashboard capture is **unrun and unqualified**. No plugin or dashboard source changed, no feature commit was made, and nothing was pushed. This is the first 09447s worker execution; the earlier consumed launcher attempt and the failed 09447r P9 evidence remain preserved.

| Component | 09447s result |
|---|---|
| OD11 current-time reconciliation | PASS: 1,021 exact records, 1,167,215 bytes, 368 Timestamp proofs, 5,147 protected-header proofs |
| Reconciliation can-fails | PASS: positive control accepted; 10 synthetic invalid receipts rejected; 0 protected-file mutations |
| Full canonical entry P9 | STOP on D: floor; inventory passed; binary/config hashing incomplete |
| Forward EG1 operation support | Helper implemented; no build/query used it, so forward operation integration and can-fails remain unproved |
| OD12 capture-root registration and scope can-fails | UNRUN; no C: capture root created |
| Stage A source corrections and builds | UNRUN: 0 source edits, 0 build attempts |
| Stage A focused tests, lever audit and full strict includes | UNRUN; prior failures remain open |
| Stage A editor `-game` smoke | UNRUN: neither warmup-only integration nor completed capture exercised |
| EG1 clean-engine query and full exit P9 | UNRUN; fast closeout does not replace either obligation |
| Stage B minimal dashboard component | UNRUN because A did not pass; no branch, tests or desktop build |
| Stage B package and packaged console C5 | UNRUN; no package, cook or cooked-resource proof |
| Stage C equivalence and lifecycle/desktop gates | UNRUN; 0 of the five required full captures performed |

The reconciliation was recorded honestly at 2026-11-05T13:01:36.420663+00:00. It binds the independently pinned root receipts and three diagnostic proofs, then rechecks each current path, physical identity, size, mtime and SHA-256. All 653 LiveCodingInfo files and 368 Timestamp files remain associated with the prior ReGate51 build window. The 368 Timestamp contents match the retained candidate UHT manifest byte for byte; all 5,147 referenced engine headers match P9's identity, size, mtime and hash. The charged inherited metadata is 1,167,215 bytes against the unchanged 100,000,000-byte EG1 cap.

This accepts a verified inherited state under OD11; it does not manufacture a prior per-operation receipt, backdate evidence, restore engine mtimes, or clear 09447's missing full exit P9 and clean-engine query. P9 v1/v2/v3 and canonical `owner_p9.py` were not edited. The canonical source hash is `c82536a937d69cb70a7ddfe8de995b42f4eb39f6a2976d3a66fb1ab290551eca`.

Synthetic receipt mutations rejected wrong SHA, size, mtime, file identity, a missing record, an unapproved path, generated header, generated C++, engine DLL class and an over-cap inventory. The loader accepts only the exact independently pinned inherited records; its proposed forward chain checks exact job paths, parent hashes and cumulative charging. The forward context manager has before/after metadata receipts around the existing ledger, but it was not exercised or integrated with a new physical operation after this stop. It is not a completed forward-ledger qualification.

Evidence: [reconciliation](D:/IA_Europa/Receipts/09447s/eg1-inherited-reconciliation.json) (SHA-256 `81ca04f963c888aed985e11adab8b8440f26d60521eb25e0e5121c06669836d4`), [pin](D:/IA_Europa/Receipts/09447s/eg1-inherited-pin.json), [can-fails](D:/IA_Europa/Receipts/09447s/eg1-reconciliation-can-fails.json), [canonical P9 result](D:/IA_Europa/Receipts/09447s/owner-p9-result.json), [complete P9 log](D:/IA_Europa/Receipts/09447s/owner-p9-main.log) and [stop receipt](D:/IA_Europa/Receipts/09447s/stop.json).

Canonical P9 rederived the baseline partition and passed its git and full inventory checks: 382,970 files, 0 inventory differences, and 1,023 exact metadata admissions (the 1,021 reconciled records plus two previously admitted records). It selected 22,628 binary/config files and completed 3,974 matching hashes totaling 48,939,528,265 bytes. The remaining 18,654 hashes and the later original-source, compiler-identity, connectivity and interrupted-ledger checks did not complete. There is no full-P9 summary or pass. [Inventory comparison](D:/IA_Europa/Receipts/09447s/P9/entry/inventory-comparison.json); [partial binary/config hashes](D:/IA_Europa/Receipts/09447s/P9/entry/binary-config-sha256.jsonl).

The wrapper's failure message did not retain the exact free-byte sample at 13:05:54 UTC. That is a measurement limitation. Later readings briefly showed D: above 50 GiB, then independently recorded it below the floor again; they do not replace the missing original sample or authorize a retry. The later pagefile observations showed its allocated size increasing from 24,196 to 25,220 MiB. This does not explain the entire D: change; the remaining volume growth is unattributed. No paging or machine setting was changed.

Only new worker helpers and evidence were written. [bootstrap09447s.py](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/bootstrap09447s.py) invokes canonical P9 with the bounded OD11 loader and stricter disk-floor checks. [eg1_receipts09447s.py](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/eg1_receipts09447s.py) implements reconciliation, synthetic tests and the unexercised forward receipt helper. [compress_text09447s.py](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/compress_text09447s.py) was prepared but never run. [closeout09447s.py](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/closeout09447s.py) performs read-only exit checks; [report09447s.py](D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan/report09447s.py) writes this report and preservation receipts. The entry configuration permits no builds, queries or runtime. No launcher, wrapper, orchestration ledger, root receipt, ruling, authorized brief or protected baseline was edited.

OD7's 14 first-header corrections and OD8's 12-check centralization were not reached. No bench predicate or audit rule changed. The required predicate truth table—no flag=false, IAIBench=true, fixture-only=false, both=true, Shipping=false—remains untested. The supported audit profiles, ANOMALY_BENCH binding, active rule mutants, missing/extra/echo/wrong-profile rejection and full strict-include obligation remain open. R1–R6/B1–B3 were not weakened. The prior 32-location inactive-registration diagnosis is not a passing audit.

The attested prior 4.25 Editor/Game builds at `a231a577905b3ea6163500b5a5982f108c8c2183` remain prior results; current build inputs match that commit. There are no new compiler logs or build results. The inherited ReGate51 compile still fails on 14 first-header diagnostics despite UBT exit 0. ReGate51 stayed clean and detached at `40a8eaf983518fe1992e172434aa8caf347654f1`; its mixed binaries, including the failed candidate's Capture and ControlServer DLLs, remain unqualified. No checkout, rebuild, source restore or 5.1 launch occurred.

All 38 live/archive pairs were rehashed during closeout: 0 missing files, hash mismatches, size mismatches or mtime mismatches. Their 974,343,342 bytes remain in the existing immutable archive; 0 new archive copies were created. [Complete live/archive inventory and hashes](D:/IA_Europa/Receipts/09447s/existing-binary-archive-rehash.json). Each link below is the archive path; hashes are SHA-256. The existing Shipping EXE is inventory only, not a build or launch.

| Archived artifact | SHA-256 |
|---|---|
| [4.25 host Shipping EXE](E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/StylizedParisStreet-Win64-Shipping.exe) | `6f669b6337eebf97d9b6343557b596f7ab25fa7c03273abebb5621599a24c861` |
| [4.25 host Game EXE](E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/StylizedParisStreet.exe) | `b5c1a94fb79d292af07a1354957b1252be5cc47d599546e5a33381af36a13ea2` |
| [4.25 probe DLL](E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/UE4Editor-EuropaPhaseAProbe.dll) | `2b19de1d4f1628d5dac9b8968bed9ea62cfef6326d6d4f7edac7e932bb682595` |
| [4.25 host Editor DLL](E:/IA_BuildCache/09447-evidence/binary-archive/425/Binaries/Win64/UE4Editor-StylizedParisStreet.dll) | `8c553e7fc999ef9ca995a933d8d573a7c67791d116f328842f9f3c95d91a6f2c` |
| [4.25 AnomalyBench.dll](E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyBench.dll) | `2bf4268777f3c459b6812bdf56f24d6624fe0b6d282b0e7ae2f31a53f55ad940` |
| [4.25 AnomalyCapture.dll](E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyCapture.dll) | `8036d3cda8b4a91fab5ef101eac938b892f5284e0478c64ff57d6dee42a3bf4f` |
| [4.25 AnomalyControlServer.dll](E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyControlServer.dll) | `ddf6af9816e8a75b22b642438bf90077030392468c3d8a2173656bb79a25aa8e` |
| [4.25 AnomalyInjector.dll](E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyInjector.dll) | `00b2e8f49f0f5394eab3b8c027ec908d7a0af9efbc574e625ffb905b2e6a256d` |
| [4.25 AnomalyShaders.dll](E:/IA_BuildCache/09447-evidence/binary-archive/425/Plugins/AnomalyInjector/Binaries/Win64/UE4Editor-AnomalyShaders.dll) | `3d0ff3bdd2a721f48573124a0ceaa79870fdae460bd0a01c90df2220cc7cb0fb` |
| [ReGate51 AnomalyBench.dll](E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyBench.dll) | `128e48761dfc075662200648667725e426f8e978d8400a8aedbab9c66612c33c` |
| [ReGate51 AnomalyCapture.dll](E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyCapture.dll) | `dc3195fdb4cb212fef0a2e3e28813aaf2b8d0a8ae52fa3e955ccfc441df344d2` |
| [ReGate51 AnomalyControlServer.dll](E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyControlServer.dll) | `4e83f6cac88f6e99fc4dec598f574f455020d390d16c02a94e2e6cd234726aeb` |
| [ReGate51 AnomalyInjector.dll](E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyInjector.dll) | `88f288eed3763402f91fc5e38fe8612451df8cc3f54db95a6f391f0aefa7e4b5` |
| [ReGate51 AnomalyShaders.dll](E:/IA_BuildCache/09447-evidence/binary-archive/ReGate51/Binaries/Win64/UnrealEditor-AnomalyShaders.dll) | `f8736404b31745068a78638807114d25a4f42a3df093f4f2e8c8e28aa7c93c9c` |

Capacity readings below are at entry and final process closeout (2026-11-05T13:17:48.8601046Z). These are read-only proof/report operations, not build or runtime sessions.

| Volume | Entry GiB | Closeout GiB | Closeout free bytes | Start / floor GiB |
|---|---:|---:|---:|---|
| C: | 21.847759 | 21.593700 | 23,186,059,264 | 13 / 10 |
| D: | 52.842915 | 42.645210 | 45,789,945,856 | 53 runtime / 50 |
| E: | 51.293983 | 51.280979 | 55,062,532,096 | 50 / 50 |

D: is 7,897,145,344 bytes (7.354790 GiB) short of its floor, and 11,118,370,816 bytes (10.354790 GiB) short of runtime admission. E: has only 1.280979 GiB above its floor. The full package/cook/staging/DDC/archive/five-session/review peak budget was not reached or approved. Physical continuation is HELD FOR CAPACITY as well as the failed entry gate. No capacity improvement, reserve reduction, shorter run or alternative output root was assumed. GA5's wait was not used to override an observed floor failure.

No C: capture evidence, E: evidence archive or package folder was created. No runtime session occurred, so no per-session C: growth attribution or physical-stage before/after proof exists. C: observations remain above the start/floor limits. OD12's 11 GiB evidence maximum and the unchanged incidental caps remain unexercised. No budget justified OD13 cleanup: both PCHs were left intact; all seven text compression candidates were left intact. There were 0 deletions, compression operations or binary replacements. No fixture compile occurred. [Capacity and process observations](D:/IA_Europa/Receipts/09447s/processes-closeout.json); [earlier post-stop readings](D:/IA_Europa/Receipts/09447s/post-stop-capacity-processes.json).

Safety closeout: fast checks match entry for protected git state, all feature/protected refs and drive-root names, host Content/Config/descriptors/assets/binaries, fixture assets/config/source/binaries/descriptor and both material sets. Reconciled metadata was rehashed again and matched. Full P9 remains incomplete, so these targeted matches are not a claim that every protected binary received a complete exit proof. No new protected-file change was observed within the completed checks. [Fast exit snapshot](D:/IA_Europa/Receipts/09447s/exit.json); [fixture/material hashes](D:/IA_Europa/Receipts/09447s/fixture-and-materials-closeout.json).

There were 0 runtime crashes, upload signs, control-server tokens read/generated/copied, editor/game/build children or owner-process interventions. The ignored dashboard configuration was not read. Foreign processes were recorded and left untouched; their presence was not the stop reason. All invoked worker helper processes finished, including P9's hash workers. No owned child was abandoned. The read-only closeout found no UE editor, UBT, compiler or linker process. No source/runtime setting, bench lever, security, power or machine setting changed.

Writes stayed in the fresh D: receipt/scratch roots and new worker helpers/report in the run folder. The prior report was preserved byte-equal as [prior-09447r-report.md](D:/IA_Europa/Receipts/09447s/prior-09447r-report.md) before replacement. [Report replacement intent](D:/IA_Europa/Receipts/09447s/report-replacement-intent.json) and [worker-created artifact inventory](D:/IA_Europa/Receipts/09447s/worker-created-artifacts.json) record the only report replacement and created files. No earlier failed attempt was discarded. The departure from the planned sequence is the documented mandatory entry stop; forward EG1 integration, OD12 registration/can-fails and all later qualifications remain unfinished.

Final plugin tip is unchanged: `feat/europa-port` = `a2250fda7a9b2f3754fd13c6061fe632f45b9d6e`; read-only `ls-remote` confirmed origin = `c5d0919e5074404d26da8c16ccf845b360bde383`. Dashboard remains clean on master `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`; `feat/europa-425` was not created. There are no new commit hashes/subjects or pushed commits for either repository. Four defaults, including LOD corruption, remain unchanged; camera remains opt-in and unqualified. Camera D4/G3, GO3 and plugin-publication holds remain open. LOCAL VALIDATION / NOT OFFICE-READY; the shipped 094-45r card is unchanged.

VERDICT: NEEDS-DECISION
