---
name: europa-p9-engine-metadata-needs-ledger
description: "A 5.1 ReGate51/51port build rewrites ~1,021 engine bookkeeping files; owner_p9.main() fails at the next entry unless that build ran as a per-operation ledger step; my own P9 checks don't cover the engine tree"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-05T11:57:23.431Z
---

094-47r (2026-11-05) stopped at its entry: `owner_p9.main()` exit 1, `P9 inventory STOP: 1021`.
- The files: 653 `LiveCodingInfo.json` + 368 UHT `Timestamp` under `D:\UESource\UnrealEngine`, rewritten by 094-47's ReGate51 compile (09:45 UTC). Sizes/file IDs unchanged, 143 contents differ.
- `owner_p9.allowed_metadata()` admits `engine51_metadata` changes only from `Receipts\09406\stop\engine-differences.json` or per-operation `ledger\*\classified-changes.jsonl.gz` from a HARD-CODED job list (09407/08/09/15/21) plus the current job's ledger. 094-21 ran its 5.1 builds as ledger steps and P9 at exit; 094-47 ran builds without a ledger and full P9 only at entry, so nobody noticed.
- **My own gap:** my "P9 unchanged" checks (m51 gate + tree list hashing `Receipts\P9` + fixture) do not cover P9's protected inventory (~383k files incl. the engine tree). In 094-47 I wrongly reported "0 P9 hash changes".

**How to apply:** after any job that compiles 5.1, check whether it ran a ledger step for the build and a full P9 at exit (`Receipts\<job>\ledger\`, `P9\exit*`); if not, flag that the next job's entry P9 will stop. My reader `D:\IA_Europa\Temp\CodeReview09447r\p9diff47r.py` rechecks a worker's P9 differences against `P9\baseline-v3\full-sha256.jsonl`.

Related: [[europa-port-pushed-d1cdc7b]], [[europa-p9-full-main-stale-refs]].
