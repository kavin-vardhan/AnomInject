# 094-47r capacity design — read-only draft, not authority

2026-11-05, about 17:09 IST. Root Codex. The active 094-47r brief is unchanged; do not steer it or act on this draft. Review its completed report before deciding a separate continuation.

## Actual limits, not assumptions

The primary ruling is _reviews/094-44f-ruling.md, GL4: record C: before/after every session and attribute a drop of at least 1 GiB if possible. It explicitly says this is not a stop condition unless C: falls below 10 GiB. Thus 1 GiB is an attribution trigger, not a write cap. The existing tools/europa/c_space09446r.py (re-exported by c_space09446s.py) implements that distinction: attribution_required=delta>=2**30; floor_pass=after.free>=10*2**30.

Retain the separate runtime_lean_gate.py C: start requirement of 13 GiB, the D: start requirement of 53 GiB (50 floor + 3 reserve in the ruled wrapper), and E: 50 GiB. Keep every memory, idle, process, upload, protected-file and capture gate. The weaker legacy D/E constants in shared helpers are not authority.

Observed free at 17:05 IST: C:22.208, D:53.198, E:51.336 GiB. These are snapshots, not reservations.

## Measured data size

Read-only recursive inventories of existing plugin-frames directories:
- 09445r D3D11-1280: 7906 files, 1618418641 bytes, 1.5073 GiB.
- 09445r D3D12-1280: 7906 files, 1618506463 bytes, 1.5074 GiB.
- 09445r D3D12-1279: 7906 files, 1628353987 bytes, 1.5165 GiB.
- 09446r D3D11/D3D12 near-floor: 7908 files, 1.5147/1.5146 GiB.
- The larger 2640-frame camera controls/positives are about 1.63-1.75 GiB of plugin frames.

These are historical logical sizes, not guaranteed bounds for new packaged captures. Include previews, metadata, logs, NTFS allocation, archive copies, shader/pipeline-cache growth and foreign writes separately. No compressed or missing artifact may be silently ignored.

## Prospective storage option to assess after 094-47r

A future, explicitly authorized primary output for NEW capture evidence on C: could use the available capacity while retaining the E: package location. Example future root: C:/IA_EuropaEvidence/09447s. It does not exist by any action of root; no current write fence authorizes it.

This would require a new exact write-fence/job registration before launch, fail-closed path/reparse checks, root-creation receipt, normal frozen manifests and Code's independent rehash. No old evidence moves, no path-remapping layer, no deleted raw captures, no overflow fallback after a floor breach, and no P9-baseline alteration.

The original gates require five full 2352-frame sessions: Stage B console D3D11 plus Stage C console/dashboard pairs on both RHIs. Early-stop, reconnect and second-capture survival are additional lifecycle requirements. Their exact sequence must be presealed; every full session remains full. A deliberately partial early-stop test must remain PARTIAL, not count as a completed equivalence capture.

An illustrative planning allowance of 2 GiB per full capture plus 0.25 GiB for lifecycle evidence gives 10.25 GiB, leaving about 11.958 GiB on C: from the snapshot. This is NOT yet admission: it omits a quantified package/cache/foreign-growth margin and may be too tight. Preserve C:>=13 before every new runtime launch. Performing lifecycle checks before the final full capture could preserve that start reserve; a final post-run balance between10 and13 does not authorize another launch. No leg count, frame count, pixel threshold or full-run requirement may be reduced to fit.

The measured four-default sessions are materially smaller than the conservative allowance, but the worker must establish a complete peak budget and retain stop/drain headroom before launching. Attribute actual capture growth explicitly, in addition to the existing cache measurements; never invent a cache attribution.

## E: packaging headroom

E: currently has only about1.34GiB above its floor before new archive growth. The actual package/temporary-copy cost is still unmeasured. The host has only three authored EA_Core/EA_Travel/two-LOD fixture assets in its EuropaPhaseA folder, but host/default/cook dependencies may include more; do not equate these few assets with a proved package size. Host Content/Config must remain hash-equal.

Two generated fixture PCH files total2474246144bytes (about2.304GiB). They are possible regenerable-cache candidates for a future exact receipt, NOT currently authorized for deletion:
1. E:/IA_Fixtures/Outline425/Intermediate/Build/Win64/Outline425Editor/Development/Engine/SharedPCH.Engine.ShadowErrors.h.pch
   1301020672bytes; mtime2026-11-03T06:02:27.7029621Z; SHA256 D68B6C1AB13E4A5C212EF8A5F90687AF329857FB757B9C79AD60A8369F33703B.
2. E:/IA_Fixtures/Outline425/Intermediate/Build/Win64/Outline425/Development/Engine/SharedPCH.Engine.ShadowErrors.h.pch
   1173225472bytes; mtime2026-11-03T06:06:53.9043152Z; SHA256 86E9C123BCC3238152E219E250D6A776C83AE7D73A6F0D8B540014D605A4C073.

Do not delete the parent directories or associated objects/assets. Before any future authorization: establish no active producer/consumer, prove generated/rebuildable status and absence from protected/evidence bindings, resolve exact physical paths and reject reparses, record hashes/mtimes and expected space gain. If an immutable/protected binding requires these bytes, preserve them and hold capacity. No blanket Intermediate or cache cleanup.

E:/IA_BuildCache/StackOBot/Binaries includes old PDBs and an explicitly quarantined interrupted-link executable. It is not a cleanup candidate. Preserve it and all historical archives.

D: has about3.2GiB above its floor, but only0.2GiB above the runtime launch threshold. Cooking may create duplicate Saved/Intermediate/staging outputs. Account for simultaneous peaks and any exact current-job scratch retirement before a later runtime launch; no deletion of pre-existing Saved files or protected Content/Config.

## Decision boundary

This draft establishes a possible way to continue without changing qualification gates; it is not yet a capacity PASS, ruling, dispatch or filesystem action. The active job has no new C: output or cache-cleanup authority. Wait for its full report and actual new footprint, then either authorize a concrete bounded storage plan under the owner's delegation or retain the physical hold with measured shortfall.

Chat review NOT REQUIRED for this read-only assessment. Any later scoped storage decision must preserve all gates; changing qualification meaning, thresholds or release authority remains the next mandatory owner/Chat checkpoint.
