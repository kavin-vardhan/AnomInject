# 094-47s owner-delegated ruling — reconcile EG1 receipts and provide bounded storage

2026-11-05. Root Codex, today's owner-authorized orchestrator, has read the completed 094-47r worker report, Code review, mailbox final, scans and card in full. The job closed at17:27:52IST, with no owned child remaining. This is an owner-delegated ruling under "You take the best decisions for all of these yourself", not a Chat ruling. No gate relaxation is authorized. OD1-OD10 stand except the precise scope/storage changes below.

## OD11. complete the existing EG1 receipt contract

The existing 094-06-ruling.md EG1 already permits bounded 5.1 build bookkeeping with per-operation path/size/mtime/hash receipts and a 100 MB per-job cap. Engine binaries, generated C++, source, Content, Config, Shaders, Programs and engine/plugin compile or link actions remain forbidden. Its Baseline51 clean-engine action-plan query remains mandatory: no compile, NoEngineChanges, WriteOutdatedActions, zero engine and engine-plugin actions.

094-47's ReGate51 build omitted the receipt P9 needs. The full 094-47r P9 check correctly stopped; preserve it. Root's independent evidence:
- 1021 exact paths: 653 LiveCodingInfo.json, 368 UHT Timestamp; same identities and sizes; 1167215bytes total. All modification times lie inside the recorded prior ReGate51 build.
- Against 094-47's admitted entry state, 892 files are byte-equal and129 differ. Against original P9, the worker reports878/143. These are different comparators, not conflicting counts.
- All368 current Timestamp files are byte-exact products of the retained candidate UHT manifest.
- All5147 referenced engine headers match P9's hashes, sizes and mtimes.
- The existing UBT writer and reader are ExternalExecution.cs:855-879 and779-800: it writes the listed headers and checks their count/set on the next run. Manifest construction is1012-1015.
- Root proof hashes: diagnosis651e28febac6f90875ef3dcaa58b7dd6f55fb23c332406b268f1ced29df36682; prior comparison82aa8e9274b87b241519d9046761959bd6d7e1eab3a310171f048903b3b8172c; UHT/header validation2759841754d12052d4a72334e8c0c83f77e8d40ca790e98bf06fca64bdb73ea5.

Authorize the relay implementer to record an honest current-time reconciliation of exactly those inherited1021 records after Code independently reproduces the hashes, identities, build-time association and generated-header proof. Do not fabricate a receipt as if collected during094-47, backdate anything, restore engine mtimes, discard the failed P9 result or edit P9v1/v2/v3 baselines.

Allow a tightly bound inherited-receipt loader/bootstrap wrapper for the full P9 check, plus the minimal tooling correction that captures EG1 metadata before/after each newly authorized5.1 build/query and carries only explicitly pinned, reviewed parent receipts forward. The canonical full P9 checks must still execute. No broad timestamp ignore, whole-directory exemption, automatic trust of arbitrary old receipts or swallowed failures.

Require fail-closed path/class/cap checks, exact SHA/size/mtime binding, receipt provenance and full recheck. Synthetic can-fails must reject a wrong byte/hash, wrong size/time, missing record, unapproved path/class and generated C++ or engine binary change. Never mutate actual protected files for a can-fail. Current-operation verified receipts take precedence only for their exact paths; every other protected item stays checked.

This is an added tooling scope under the owner's delegation. After reconciliation, require full owner_p9.main() through its binary/config phase, not only the inventory or a hash summary. Only then resume OD7/OD8 corrections. Future5.1 operations must not repeat the missing-receipt defect; capture all allowed metadata and require EG1's clean-engine query/exit proof. No5.1 runtime.

## OD12. deliberately place NEW capture evidence on C:

Authorize only the exact primary root C:/IA_EuropaEvidence/09447s, and required creation of its otherwise absent parent, before physical work and after physical-path/reparse/ownership preflight. This is a prospective write-fence change, not an automatic overflow fallback. No old data is moved or deleted and no raw evidence is compressed.

The new C: root is a declared primary capture-output scope only, with a maximum11GiB of allocated new evidence and a smaller presealed working budget when required by actual headroom. Keep the existing incidental-write caps unchanged: C:250000000bytes, E:50000000bytes, engine51 bookkeeping100000000bytes. Do not reclassify any existing C: cache/system write into the new evidence scope. Add only exact09447s path/job registrations and synthetic wrong-root/sibling/reparse/over-budget can-fails. Do not disable the ledger or weaken any other classification.

Retain the E: package and E: changed-binary archive under new09447s paths. Job configs/receipts/scratch remain under the approved D: roots. Minimal exact job/root registration in the kit is allowed; never relax general path validation or root attribution. Receipt the new owned root and reject siblings/out-of-scope writes. P9 protected baselines stay unchanged.

C: start13GiB and stop10GiB remain. GL4's >=1GiB trigger requires attribution, not refusal; measure actual new evidence growth and existing caches separately and report unattributed residual honestly. D: launch53GiB/floor50 and E: floor50 remain. All memory/idle/owned-process gates remain.

Before any large write, prove the full per-volume peak budget: five required full2352-frame captures plus all lifecycle evidence, package/cook/staging/DDC/archive/temp/review growth, simultaneous copies and drain/safety margin. Historical four-default plugin-frames occupy about1.51GiB per full run; this is not a guaranteed upper bound. No fewer/shorter qualification runs, silent reuse of a StageB leg as a new StageC leg, or invented safety margin.

Preseal lifecycle sequencing and every leg. A partial early-stop test may also demonstrate second-capture survival when actually performed in the same game/listener, but remains PARTIAL and never counts as a full equivalence result. All five full sessions remain. Run admission checks before every launch; if any required continuation cannot fit, hold it rather than truncate a gate.

No capacity PASS is implied by this ruling. No new physical leg after20:30IST absent extension; finish an active child safely and checkpoint. Existing source, camera, Shipping, push and release bounds remain.

## OD13. two exact regenerable PCH files only, if the proved package budget needs them

Under the owner's delegated technical decisions, authorize an exception to OD9's no-old-cache-cleanup rule for ONLY these two generated Outline425 PCH files, total2474246144bytes:
- E:/IA_Fixtures/Outline425/Intermediate/Build/Win64/Outline425Editor/Development/Engine/SharedPCH.Engine.ShadowErrors.h.pch — 1301020672bytes, mtime2026-11-03T06:02:27.7029621Z, SHA256 D68B6C1AB13E4A5C212EF8A5F90687AF329857FB757B9C79AD60A8369F33703B.
- E:/IA_Fixtures/Outline425/Intermediate/Build/Win64/Outline425/Development/Engine/SharedPCH.Engine.ShadowErrors.h.pch — 1173225472bytes, mtime2026-11-03T06:06:53.9043152Z, SHA256 86E9C123BCC3238152E219E250D6A776C83AE7D73A6F0D8B540014D605A4C073.

Before worker launch, Code must independently verify exact physical absolute paths, hash/size/mtime, generated/rebuildable status, no active producer/consumer, no P9/protected/immutable-evidence binding requiring the bytes, and report the exact expected gain. The worker must then prove the complete capacity budget before deciding this cleanup is needed. Recheck immediately before deletion. If any condition fails, do not delete and retain the capacity hold.

Only the relay implementer may remove these two files using literal native filesystem paths, after an intent receipt listing every identity and hash. Record actual deletion/space gain, and independently verify fixture Content, Config, Source, binaries and descriptor unchanged. No recursive removal, parent deletion, objects, debug symbols, archives, owner caches or any additional file. Do not recreate/compile the fixture in this job; a later fixture build can regenerate its PCHs under its own gates.

If the complete package budget fits without this cleanup, leave these files intact. This is not a general cleanup permission, not a floor reduction and not evidence retirement.

OD5's lossless compression of completed own text receipts remains available. For a concrete D: reserve recovery, authorize the relay to compress only the seven completed per-job P9 entry protected-inventory.jsonl files in _reviews/094-47s-codex-text-receipt-candidates.json (SHA256 f1204109af7c09b9d0f80fc061a581c270978cb34388a48398894a07ec0bcf17), after validating each recorded SHA/size/mtime. These are the entry inventories under Receipts/09445,09445r,09446,09446r,09446s,09447,09447r/P9, totaling648385976logicalbytes. Record actual allocated-space gain and prove unchanged file hashes and mtimes. Do not compress the P9 baseline itself, binaries, raw captures, packages or any other file through this exception. No recursive command or deletion; stop if identity/content cannot be preserved. Code reviews the receipts independently.

The reconciliation is an explicitly dated acceptance of this verified inherited metadata state, not a claim that094-47 had the missing per-operation receipts or passed its unrun full exit P9/clean-engine query. Keep those omissions visible. The new full P9, query and metadata accounting must pass.

Overall NEEDS-DECISION remains while camera D4/G3 and GO3/plugin-publication holds remain. Only an independently passing dashboard branch may be pushed by Code under OD4. No plugin push, release merge or tag.

Chat review NOT REQUIRED for these tightly bounded engineering reconciliation/storage decisions under the owner's delegation. Next mandatory owner/Chat checkpoint remains a qualification-meaning, threshold or release-authority change. Existing release/merge checks remain.
