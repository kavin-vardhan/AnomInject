---
name: europa-p9-full-main-stale-refs
description: "P9's ref check (owner_p9.py) once failed on 4 recorded refs; GE1 (d2f481d, 094-43r) replaced it with owner_ref_policy.py: 71 baseline refs exact, 4 named extras by rule; a new ref made by a job must be added there or full main() fails"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-11-03T23:24:07.369Z
---

094-43 (2026-11-04) stopped at Stage A entry because `owner_p9.py:72` required every ref except local feat to equal the 71 baseline refs.
- **Chat's GE1 fix:** Codex implemented it in 094-43r as `tools/europa/owner_ref_policy.py` (commit `d2f481d`).
  - The 71 baseline refs must be exact.
  - Only four named extras are admitted:
    - `refs/backup/europa-pre-fv6-09440` at exactly `08fe652`;
    - `origin/feat/europa-port` at an ancestor of, or equal to, local feat;
    - `spike/europa-09442c-mesh-shader` at exactly `740220c`;
    - `spike/europa-09443a-depthonly-byname` at `c1193bd` or a descendant (now `42ea37e`).
  - Any other extra raises; the baseline is never regenerated.
- **Tests:** `owner_ref_policy_tests.py` has 8 cases. It runs read-only from the Paris worktree with `python -B` and no argument; with an argument it writes a receipt.
- **Full `main()`:** passes (Codex 354 s at entry). Mine passed unmodified in 560 s at `93613ae`, using `D:\IA_Europa\Temp\CodeReview09443r\p9main43r.py`.
  - `job_scope.load` needs the config inside a job root under `D:\IA_Europa\Receipts`, the temp root under `D:\IA_Europa\Temp`, and a memory tuple ({6,12,1,4} works).
  - `main()`'s argv[1] is the phase name; a name starting with `entry` uses the start disk floors.

**Why:** P9 protects the baseline by naming refs instead of re-baselining.
**How to apply:** if a job creates a new ref (a spike, a backup, a remote branch), P9's full `main()` fails at entry until chat rules and `owner_ref_policy.py` names it. Check the ref list before launching a job whose entry runs P9. Related: [[europa-port-pushed-d1cdc7b]], [[gdp-impl-relay-orchestrator-guard]], [[europa-phaseb-texture-clip-facts]].
