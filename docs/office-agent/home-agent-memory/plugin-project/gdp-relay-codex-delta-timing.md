---
name: gdp-relay-codex-delta-timing
description: "GDP Relay Codex jobs are fast (a full m53 delta check took 556 s at max effort); poll with condition loops, bare Start-Sleep is blocked"
metadata:
  node_type: memory
  type: reference
  originSessionId: <uuid>
  modified: 2026-10-01T01:48:36.527Z
---

First real GDP Relay use (brief 082-03, 2026-09-27): a Codex delta review of the m53 plan ran
`gpt-6-astra` at effort `max` in **556 s** (read ~1 M input tokens, wrote an 8 KB review.md, exit 0).
Budget minutes, not hours, for a delta pass; a full design review may be longer. Second data point
(082-04 delta #2): **518 s**, 5.6 KB review. Third/fourth (082-06 / 082-06b): an S1 source review took **1,526 s**;
its fix-delta **649 s**, 5.1 KB; a two-item mini-delta (082-06c) **290 s**, 2.7 KB; a ten-item mini-delta with a
runnability question (082-13) **545 s**, 5.3 KB (Codex re-hashed the staged set and re-ran Code's scratch scripts). The runner's queue check needs no other job folder with events.jsonl but no exit-code.txt.

**Pinned since 082-04 (owner):** the runner's defaults are `-Model gpt-6-astra -Effort max`, always passed
as `-m`/`-c` (empty refused, exit 5); `run-meta.txt` has a `requested_model/effort` line beside the
rollout's `model=`/`effort=` — check the two agree. A stray `apply_patch verification failed` line in
`stderr.txt` is Codex retrying its own write, not a failure, if `review.md` exists.

**How to wait:** launch the runner with `run_in_background`, then poll with a bounded *condition* loop
(`while (-not (Test-Path exit-code.txt) -and (Get-Date) -lt $end) { Start-Sleep 20 }`). A bare
`Start-Sleep N; <check>` is blocked by the harness. `exit-code.txt` appearing is the completion signal
(README). The runner's `WaitHours` is only the queue wait, not a job timeout.

Collecting: prepend one header line and copy the bytes; verify the body is byte-identical. **Read 
eview.md as UTF-8 when building the ledger row** ([IO.File]::ReadAllLines(p,[Text.Encoding]::UTF8)): PS 5.1 Get-Content defaults to ANSI and turned the verdict's em-dash into mojibake in 084-05b (fixed by rewriting the row). 084-05b source review of two diffs (~320 KB): **1,465 s**, 36 KB review. 084-07b seven-item re-check of a 110 KB source diff (frozen `git archive` extract placed in the run folder so a concurrently edited worktree could not leak in): **1,233 s**, 37 KB review; review.md appeared ~1.5 min before exit-code.txt. 090-08 six-question source re-check of two ~250 KB diffs: **1,702 s**, 43 KB review.
**Memory pressure (090-08, 2026-10-01):** an owner's overnight UnrealEditor render held ~60 GB private of a 97.6 GB commit limit; PowerShell/bash then failed to start (CLR HRESULT 80004005, "error creating the pipeline", bash temp-cwd error) and the first relay launch never reached codex. Check commit free (Win32_OperatingSystem FreeVirtualMemory) before launching; wait bounded for headroom rather than risk starving the owner's process. Related:
[[bash-tool-stalls-use-powershell]].
