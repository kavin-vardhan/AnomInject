---
name: europa-owner-kit-review-facts
description: "Europa A6 owner kit: G1/G2 fixed in 094-17 (09417 arming), new G3 (all refusals print the generic 'unexpected error' line); how to run main()/self-tests without writing outside CodeReview folders"
metadata:
  node_type: memory
  type: project
  originSessionId: <uuid>
  modified: 2026-10-26T02:08:24.720Z
---

Europa A6 owner kit (`D:\IA_Europa\Launch\Europa425-Editor-OwnerLeg.bat` + `Europa425-OwnerLeg.json`), tools in the Paris worktree `tools\europa`.

- **094-16 (303d02d):** G1 live sampler failed ~4 %/poll uncaught; G2 post-Popen lock hole. **094-17 (b20e66e, arming 09417, config 5d1d1ab5) fixed both**: per-reading `try` in `idle_gate.collect()` (errors -> `<name>:unknown` wait); `run_child(before_create, on_started first, on_creation_failed)`.
- **G3 (found 094-17, not fixed as of b20e66e):** `owner_leg.main()` catches `BaseException` -> every refusal (all slots used, lock held, already started, superseded, unarmed) prints only `Stopped: unexpected error; details saved to <file>. No editor was started.`; the plain reason is in the details file.
- **Write-scope trap:** a refusal before a slot claim writes `D:\IA_Europa\Receipts\<ARMING_JOB>\refused-attempts\<uuid>\...`. When driving `main()` in a review, patch `owner_leg.ROOT` to a CodeReview path first (see `code-receipts-094-17\review\main_case17.py`).
- **Re-running Codex's suites** needs `EUROPA_JOB_CONFIG` (write_ledger imports SCOPE at import) -> make a job-config.json under `Receipts\CodeReview<job>\<name>` with `temp_root` under `Temp\CodeReview<job>`, and patch each suite's fixture TEMP base (`review\rerun_codex_selftests17.py`, `review_env17.py`).
- Live collector: GPU `Get-Counter` failures cluster when the foreign <other-home-project> UE 5.7 workload runs (5/200 then, 0/120 when quiet); calls ~4-10 s. The kit's P9 preflight takes ~13 min (768-806 s) before the 45-min gate.
- Owner profile blocks any image matching `unreal|ue4` except UnrealTraceServer/UnrealVersionSelector, so the Epic launcher's UnrealCEFSubProcess.exe holds it; the measured profile never blocked those two helpers or a bare UnrealBuildTool.exe.

**Why:** the next kit job (fix G3 / 094-18 evidence) and its review reuse this.
**How to apply:** reuse `code-receipts-094-17\review\*.py` (eq1/eq2/eq3 can-fails, main_case17, collector_live17, bat_dry_run17, console_lines17). Related: [[gdp-impl-relay-orchestrator-guard]], [[bash-tool-stalls-use-powershell]], [[foreign-editors-block-bench]].
