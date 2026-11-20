---
name: ia-europa-cleaned-094-49
description: "2026-11-06 094-49 deleted every >1 MiB file under D:\\IA_Europa\\Receipts and Temp plus Evidence\\09447u..y (74.15 GiB); small receipts, P9 kit inputs and tool-referenced fixtures were kept"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-06T15:56:35.979Z
---

094-49 (2026-11-06, Claude Code, no Codex) cleaned `D:\IA_Europa` by manifest: 61,263 files / 74.15 GiB deleted, 0 skipped; D: free 26.4 -> 99.9 GiB.
- **Gone:** every file >1 MiB under `Receipts\` and `Temp\` (frames, masks, `.u32`/`.zlib`, ledger `.sqlite`/`.gz` snapshots, `P9\*\protected-inventory.jsonl` copies inside job folders, strict-include `.obj`, superseded dashboard exes) and all of `Evidence\09447u..09447y`.
- **Kept on purpose:** all files <=1 MiB; `Receipts\P9\` (P9 baselines); `Receipts\09405\ledger\`; owner_p9 inputs (09406 engine-differences, 09407/08/09/15/21 classified-changes.jsonl.gz, 09403 source-before, 09415 actions.json); 7 tool-referenced fixtures (e.g. 09415 51base-editor native-graph.json, 09435 evidence-000050 zlib, 09439 C5/02 labels.jsonl, 09447t eg1-verified-r2.json); `Receipts\09447t\dashboard-build-r4\`; 3 "kept, unsure" items.
- The current package is on E: (`E:\IA_BuildCache\09448c-package\flat-01\WindowsNoEditor`), never hardlinked into D:.
- Receipts: `_relay\runs\2026-10-16-094-01-europa-phase-a-plan\code-receipts-094-49\` (inventory.csv, delete/keep manifests, delete-results.csv).

**How to apply:** a reader or test that opens an old job's large frame/ledger/inventory file will now find it missing; that is this cleanup, not corruption. Check `delete-manifest.csv` before diagnosing. Related: [[europa-p9-full-main-stale-refs]], [[europa-port-pushed-d1cdc7b]], [[delete-guard-refuses-command-text]].
