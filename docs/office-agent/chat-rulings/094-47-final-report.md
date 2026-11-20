# 094-47 final report — Europa 4.25 and AnomDash

**Completed 2026-11-06.** The UE 4.25 packaged Development control server and AnomDash are qualified within the scope below. Both feature branches are published and verified remotely. LOD corruption remains in the dashboard; Shipping excludes the control server. Final protection and process-closure checks passed. No Europa build or capture remains in flight.

The [handover card](094-47-office-card-europa-dashboard.md) is finalized. The owner said “I'll hand it over myself” and assumes the Claude-project copy. Codex stopped UI control without uploading a file or sending a chat message. The local card is 13,737 bytes, SHA-256 **7a2d1646e5f1efa4571e00031df42fa7a9a5f8c5c86efb4de866365094539028**.

## Published delivery

| Repository / branch | Verified published commit | Publication |
|---|---|---|
| Plugin / feat/europa-port | bd767ca8ac7cd1ca18abe34d9cc66764fee100a6 | Fast-forward from c5d0919, 10:58:05 IST |
| Dashboard / feat/europa-425 | b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5 | New feature branch, 10:58:21 IST |

Fresh pre-push scans passed. Before/after ls-remote checks confirmed both exact heads and unchanged other remote branches and tags. No force push, tags or release-branch merge occurred. The protected plugin worktree/index stayed unchanged. Dashboard master remains 7ea2e51bf55e19c2949641bbff2ec72afb4ae493.

- Development package: **E:/IA_BuildCache/09447u-package/WindowsNoEditor** — 34 files, 2,369,785,987 bytes.
- Desktop dashboard: **D:/IA_Europa/Receipts/09447t/dashboard-build-r4/AnomalyDashboard.exe** — SHA-256 c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28.
- The card provides office-fork build/package commands, loopback listener and local-token setup, discovery, the four-default PNG/native/2352-frame preset, and tested numbers-only readback. The native package binds to d10bbe859e81ef3a0a0e15dfc5224181cc7911b8; the later plugin tip adds documentation.

## Capture qualification and rulings

The packaged listener supports discovery/authentication, anomaly listing including LOD, and the shared C5 capture path. Actual desktop lifecycle tests passed on D3D11 and D3D12, including start, early stop, reconnect, a second capture and normal close. Early stop reports PARTIAL, drains accepted frames and restores state. Dashboard tests and frontend/native builds passed (69 tests).

The full D3D11 console/dashboard comparison used the dashboard protocol; the separate desktop tests qualify the UI actions. Both full captures requested, wrote and paired **2,352 frames with zero drops**. Labels, actor-bound mask pixels, events, schedule, routes, seed, counts and restoration agree. Each passed **443 strict texture measurements at IoU 1.000 with zero spill** and 140 clean edges.

**GR1 PASS.** Existing 094-47v/094-47w console captures supplied the console A/B baseline; no new baseline launch was needed.

| Difference | Console / console | Console / dashboard |
|---|---:|---:|
| Census-cycle differing rows | 1,339 | 1,639 |
| Maximum absolute cycle delta | 8 | 26 |
| Mean absolute cycle delta | 2.72 | 11.00 |
| Private-verification differing rows; all deltas 1 | 77 | 76 |
| In-flight differing rows; all deltas 1 | 163 | 172 |

Chat explicitly classifies the cycle differences as session nondeterminism. The likely cause is a cumulative readback-timing-driven counter plus game-thread work serving dashboard snapshots; this is an inference, not an isolated causal experiment. Private-verification and in-flight differences match the baseline. The six label-schema entries are one JSON timestamp representation, t=320 written as an integer, including sorted-list shifts. Only representation was normalized. **Future work: serialize time fields as floats.** Original strict-difference reports remain preserved.

**GR2:** the D3D12 full console/dashboard pair is dropped. Current-package lifecycle and prior console evidence remain. **GO3 PASS:** one final-package, default-only D3D12 capture at 1279×719 produced 2,352 paired frames, zero drops, 780 mask frames and 69 events. All 443 strict measurements had IoU 1.000 and zero spill. The 14-file compatibility comparison found no removed key or changed type; seven paths and one file were added. Retained card commands and the corrected current PowerShell readback passed. Startup and both live depth replies were 1; restoration passed.

**GR3:** camera_clipping is opt-in and qualified at 1280×720 on D3D11/D3D12. Held near 100 at 1279×719 caused distant textured-floor depth-precision speckle in rows 373–375, maximum 117 levels; plain floor stayed within 2, and neither 1280×720 leg showed it. The one-pixel miss at (375,239) is accepted only inside held camera-clipping sessions. Defaults retain IoU 1.000 and zero spill. Prior archived-binary compression is accepted with unchanged hashes/mtimes; future compression is limited to receipts/evidence.

## Final protection, ownership and remaining scope

**GR4 complete:** final FJ1 passed; 3,120 protected entries matched. One complete canonical exit P9 ran after the capture: 382,970 inventory entries, 22,628 binary/config hashes covering 100,696,938,006 bytes, zero inventory differences and matching hashes. Root reviewed the complete report, evidence bindings and exact process closure. The worker exited 0. The single mailbox watcher remains idle with an empty queue.

At the owner's explicit instruction, the earlier read-only scan/worker was stopped, its partial proof preserved without PASS credit, and idle admission was waived. The subsequent capture recorded no idle check and zero wait; final protection then completed. No correctness gate was loosened. No new build, package, 5.1 runtime, cache deletion or binary compression occurred in this final continuation.

Home qualification uses stock UE 4.25.4. The office fork must build/package this source and return its own numbers-only observations; office data stays there. This delivery does not qualify 5.1 runtime or a release merge. The four-default dashboard preset does not enable camera clipping. Live preview, arbitrary frame counts, JPEG/resizing and targeted captures are unsupported on 4.25; Library video/overlay generation remains unqualified. Non-authentication capture refusals require local log inspection.

## Evidence

- [Final independent review](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47y/final-independent-closeout-review.json), [plugin publication](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47y/plugin-publication.json), [dashboard publication](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47y/dashboard-publication.json).
- [GR1 disposition](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47w/gr1-final-disposition.json), [owner handover record](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47y/project-copy-receipt.json).
- Full worker evidence: D:/IA_Europa/Receipts/09447y. [Takeover progress history](094-47-codex-takeover-progress-2026-11-06.md).

**Chat review NOT REQUIRED** for the completed GR4 publication and handover. The next mandatory Chat checkpoint is a new correctness failure or a proposed change to capture/label meaning, tolerance or release scope.
