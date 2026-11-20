# CURRENT — 094-47 completed; both feature branches published

Completed 2026-11-06. Codex is the sole orchestrator and independent reviewer. The bounded Europa 4.25 / AnomDash delivery is closed: final GO3, FJ1, one full canonical exit P9, scans, exact owned-process closure and both publications passed. No Europa worker, build or capture remains in flight. The existing mailbox watcher PID 29140 is idle, with no queued brief; it is not an active implementation worker. Latest mailbox state was refreshed at final handover. Exact worker/child closure is recorded in the review below.

The owner asked to use the PC and was told normal use is fine. The owner then said **“I'll hand it over myself.”** The finalized local handover card is ready; the owner now owns its Claude-project copy. Codex stopped UI control without uploading a file or sending a chat message. Do not resume UI use or start another engineering unit for this completed delivery.

## Read order

1. D:/IntrusiveAnomalies/_reviews/094-47-final-report.md — final owner report, outcomes and scope.
2. D:/IntrusiveAnomalies/_reviews/094-47-office-card-europa-dashboard.md — published references, office build/package instructions, connection/preset and tested numbers-only reads.
3. D:/IntrusiveAnomalies/_reviews/094-47w-chat-ruling.md and 094-47w-gr1-clarification.md — GR1–GR5 and explicit GR1 interpretation.
4. RCY/final-independent-closeout-review.json, publication-readiness.json, plugin-publication.json, dashboard-publication.json, owned-worker-closure-independent-review.json, project-copy-receipt.json and final-handover-receipt.json.
5. RCW/gr1-console-baseline.json, gr1-required-content-review.json and gr1-final-disposition.json. Read RCY/closed-y-report.md for the complete final worker report, not just its verdict. The takeover progress history is _reviews/094-47-codex-takeover-progress-2026-11-06.md.

RUN = D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan
RCY = RUN/root-receipts-094-47y; RCW = RUN/root-receipts-094-47w.
Previous CURRENT chronology is preserved in RCY/CURRENT-before-final-handover.md; earlier x/y authorities and partial receipts remain unchanged.

## Published product and immutable artifacts

- Plugin feat/europa-port: **bd767ca8ac7cd1ca18abe34d9cc66764fee100a6**, fast-forward from c5d0919e5074404d26da8c16ccf845b360bde383. Native inputs bind to **d10bbe859e81ef3a0a0e15dfc5224181cc7911b8**; subsequent tip changes are documentation.
- Dashboard feat/europa-425: **b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5**, newly published feature branch. Master remains **7ea2e51bf55e19c2949641bbff2ec72afb4ae493**.
- Both exact remote heads verified using ls-remote after publication; all other remote heads/tags unchanged. No force/tags/release merge. Protected plugin m51 HEAD **53bf725bd406f8ce3e38d6ed153b7ff77e3a3e67** and index SHA-256 **526aba7fbf38d9936fcca0551362002e6332ac42b2e1ffcb1c5b322d4c9ea35e** were preserved.
- Package **E:/IA_BuildCache/09447u-package/WindowsNoEditor**, 34 files / 2,369,785,987 bytes. Manifest D:/IA_Europa/Receipts/09447u/package-manifest.json, SHA-256 **c71067f78882ad0c26c5dd90b4a62774bce22b528cededc135034a67c98e41c1**. EXE **e8166cda4ac636f10d70ffa28db3e33da8997a284694b1554b9b79612c416110**, pak **d926673c1e26886f69292162a87bdfcfbfaade283ef83b21e71e48c8dc940f56**.
- Desktop **D:/IA_Europa/Receipts/09447t/dashboard-build-r4/AnomalyDashboard.exe**, 11,190,784 bytes, SHA-256 **c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28**.
- Final handover card: 13,737 bytes, SHA-256 **7a2d1646e5f1efa4571e00031df42fa7a9a5f8c5c86efb4de866365094539028**. The card's embedded PowerShell reads were actually run and corrected for blinking-route / blink-annotation naming before finalization.

## Decisions and evidence that must survive

GR1 PASS under explicit Chat ruling: census cycles are session nondeterminism. Console/console differs on **1,339 rows, max 8, mean 2.72**; console/dashboard on **1,639 rows, max 26, mean 11.00**. Record both. Likely mechanism: cumulative readback-timing counter plus game-thread snapshot work; causal inference only. Private-verification differences 77/76 and in-flight 163/172, all deltas 1, match the baseline. Six schema entries are only the t=320 JSON integer representation and sorted-list shifts. FUTURE: serialize time fields as floats. Original strict-difference reports stay preserved.

Labels, all 780 actor-bound mask images, 1,572 clean frames, events/schedule/routes/seed/counts and restoration agree. Both full D11 captures: 2,352 requested/written/paired, zero drops, 443 strict texture measurements IoU 1.000 / zero spill, 140 clean edges. Full equivalence used the dashboard protocol; separate actual desktop lifecycle tests cover UI start/stop/reconnect/second capture on D11/D12. Dashboard has 69 passing tests and passing frontend/native builds. LOD remains in the four defaults; Shipping remains excluded.

GR2 drops the D12 full comparison pair. GR3 qualifies opt-in camera_clipping at 1280×720 on D11/D12. At 1279×719 held near 100, distant textured-floor rows 373–375 show speckle up to 117 levels; plain floor ≤2; no artifact in either 1280 leg. One-pixel miss (375,239) accepted only during held camera sessions. Defaults retain strict IoU 1.000 and zero spill. Historical binary compression accepted only with unchanged hash/mtime; future compression receipts/evidence only.

GO3 PASS: one final-package default-only D12 capture at 1279×719, seed 4242, camera disabled; 2,352 paired frames, zero drops, 780 mask frames, 69 events, 443 strict measurements and 140 clean edges. Startup/live80/live2300 depth replies all 1. Schema comparison: 14 existing files, zero removed keys/type changes, seven added paths plus run_summary.json. Both retained and current card reads passed.

The owner explicitly approved stopping the x scan/worker and waived idle checks. x launched no game; partial P9 retains NO PASS credit. y performed the capture with idle_checked=false and wait_seconds=0, then final FJ1 plus ONE complete canonical exit P9. FJ1: 3,120 protected entries identical. P9: 382,970 inventory entries, 22,628 hashes / 100,696,938,006 bytes, all match; no full entry P9. y worker exited 0 at 05:20:22 UTC. Root verified exact process birth/parent lifetimes; nine unrelated prior-day processes appeared under a reused parent PID and were correctly excluded and never stopped. Worker report SHA-256 d428ec2a88aca7b03ca1a0c8f9b2da36b1fdb30fa32f4604eb2469840f9a2dcb.

## Limits and next ownership

The owner handles project copy and office deployment. Home stock UE 4.25.4 qualification does not qualify Europa's office fork. Office data stays at the office; numbers-only readback, no captures/logs/screenshots/actor names/paths/tokens transferred. No further 5.1 runtime qualification or release merge is claimed. No live preview, arbitrary frame counts, JPEG/resizing or targeted capture support on 4.25; Library video/overlay remains unqualified. Capture refusals other than authentication may require local log inspection.

Continue using codenames only. Root plugin source reads remain Git-object-only with GIT_OPTIONAL_LOCKS=0 from D:/IntrusiveAnomalies/StackOBot/Plugins/AnomalyInjector; do not touch the protected main worktree/index. The operating contract remains authoritative, including dated corrections. No new source/build/capture work is authorized by this handoff alone.

**Chat review NOT REQUIRED** for the completed GR4 publication/handover and the owner's project copy. Next mandatory Chat checkpoint: a new correctness failure or any proposed change to capture/label meaning, tolerance or release scope. No fresh Codex session is recommended merely because this round closed; keep the coherent workstream together if follow-up is needed.
