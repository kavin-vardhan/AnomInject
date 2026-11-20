# REPORT for 084-04-m52-edge-fixes.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-04-m52-edge-fixes.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 19:09:19 | finished: 2026-09-28 19:43:29 | duration: 34 min | exit code: 0
context at last turn: 701566 tokens (70% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-04-m52-edge-fixes.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Plain summary for the owner**
All the edge-path fixes from Sunday's ruling are in, plus a timer that measures what the new mid-hold checker costs per frame. It's rebuilt as exe `B725678B` and every unit test passes. Nothing was launched, staged or focused, so tonight's bench runs on `FD621B27` exactly as planned.

**Each item**
- **F1 (stream-outs queued but not started):** UE 5.1 has no public way to see these (G348). So one tick after each revert, the plugin runs a synchronous full streaming update, the same step the engine's own `StreamAllResources` uses. That replaces any plan made under the hold before it can run. Textures that were already back now also count as restoring until this has run. If a project turns on amortized streaming copies, a counted two-cycle wait is used instead; that bound is a heuristic and is documented in the client readme. It lives in `Anomaly_StuckLowMip.cpp` (`RunStreamerFence`, `Revert`, `TickAlways`).
- **F2 (reopen continuity, reservation and refusal):**
  - Closing an event still releases the next burst, but the event keeps its watch, mask and m55 label until every frame it tracked has come back, or until the next fire begins. A late held frame therefore reopens it with nothing missing.
  - Each trail now owns its reservation and refusal, including those set by a reopen or by the unresolved timeout, and releases them once when it lets go.
  - If a reopen does come after it let go, those frames are flagged `stuck_mip.products_unrecoverable = 1` and counted.
  - Lives in `AnomalyCaptureSubsystem.cpp` (`ServiceTrailDetach`, `DetachClosedTrailsForNextFire`, `HandleTrailReopens`).
- **F5 (inherited frames):** an event carried into a new run gets its mask record before that run's first mask is armed (`EnsureMaskRecordsForCapturedFrame`). The carried reservation and refusal now survive a run that has no render record (`FinishRun`).
- **F6 (64-tick fallback):** the fallback now writes a final "unknown" (labelled) result into the frame's single record. A late receipt can no longer turn the row or mask to "not labelled" (`FlushObserveQueue`, `ComputeRenderMembership`).
- **Material set on an unregistered component that later registers:** included, and cheap. Components that were unregistered when checked are re-checked the tick they register; the cost is one pointer check each (`StartHoldMonitor`, `ScanHoldForNewUsers`).
- **Texture parameter changed on a runtime material instance:** a documented delivery limitation, as ruled (client readme §8.3a).
- **Monitor cost timer:** `stuck_mip_hold_monitor_scans` and `_ms_mean`, `_ms_p95`, `_ms_max`, plus component counts, go into the run summary, with a `HOLD MONITOR COST` log line. The streamer fence has its own `_fences`, `_fence_ms_max` and `_fence_incomplete` keys.

**Unit tests:** 130 checks, 0 failures (93 existing, 37 new). Each item is tested both ways against a byte-for-byte copy of the `90dfa6f` header (`tools/m52_window_legacy_90dfa6f.h`, namespace renamed): the old logic reproduces the defect, and the new logic passes the same case.

**Build:**
- Game: exit 0, 6 actions, 112 s. Editor: exit 0, 13 actions, 92 s. 0 warnings in both.
- Exe `B725678B1AA8C881311A7D1E5C0450AA5E0D019A2628F4873AF1A547B66107DF`, 241,950,720 bytes.
- Archived at `_binary_baselines\m52fix-B725678B\` with both build logs and a README; hash re-checked at the archive.
- String scan finds all 16 new tokens; controls present.
- Not staged: the staged exe still reads `2FCDF059`. The `FD621B27` archive and the `084-03-*` harness are untouched.

**Tuesday's confirmation bench**
- **Legs:** the FIX set changes: `B0_FIX`, `B5_FIX`, `B3_FIX`, `B9_FIX`, `B0L_FIX`, `P0_FIX`, `P1_FIX`, and optionally `B7_FIX`. The harness has no FIX-only mode, so `live` re-runs all 12 required legs (50–65 min).
- **Harness:** yes, changing the pinned hash alone is enough. Set `FIX_ARCHIVE_NAME='m52fix-B725678B'` in `084-03-common.py`. Every log line it parses keeps its prefix, and it already banks every `stuck_mip_*` key per leg.
- **Three caveats in how the report reads (no verdict changes):**
  - The monitor section still prints "not measured"; the real numbers are in the same record's `run_summary_monitor_keys`.
  - The new counters aren't in the edge-path table; they're in each leg's run summary.
  - Rows after a closure now carry "not labelled" entries for the old event up to the next fire, with `mask_state: empty`. The delivered label window is unchanged.
- **Read first:** both edges of gate (i) against tonight's run, then `stuck_mip_reopens_after_detach`, `stuck_mip_streamer_fence_ms_max`, `stuck_mip_hold_monitor_ms_p95` and `_max`, and `stuck_mip_late_receipt_after_force`.

**Commits pushed** to `fix/m52-label-timing` (`b1eb088..c71f826`, head equals origin): `8773ddb` (source and tests) and `c71f826` (journal, status block, client-readme limitations, G348). No tag, no merge. The main checkout is still `m51` `53bf725`, and `master` is `b5f15a3`.

**NEEDS-DECISION:** none. Two things Tuesday's numbers will settle:
- The streamer fence costs one synchronous streaming pass per revert. It's timed, but that cost on a heavy host is unmeasured.
- A reopen after the event has let go is still possible if the next fire begins while roughly one readback latency of frames is in flight. It's flagged and counted, not silent.

Several MCP connectors (Gmail, Calendar, GitHub and others) need authorising in their connector settings, and the PDF viewer server failed to connect. This task didn't use any of them.
api-equivalent cost (not billed on Max): $23.97 | turns: 175 | running total across all briefs: $3,119.30
