# Europa 4.25 / AnomDash: work since the Claude takeover

Snapshot: 6 November 2026, 09:10 IST. Prepared for the owner. This is a progress record, not release approval. The active validation worker is still closing its evidence; Codex retains ownership.

The packaged 4.25 Development game and the built AnomDash desktop application now connect, authenticate, list anomalies including LOD corruption, and pass the tested start/stop/reconnect/restart lifecycle cases. Full console and dashboard-protocol captures have both completed 2,352 frames with zero drops and passed their individual image/label checks. However, the new exact D3D11 comparison has just reported FAIL: 1,893 differences, described below. Consequently, the overall delivery is still **LOCAL VALIDATION / NOT OFFICE-READY**, not “everything complete.”

## What the takeover meant

At your request, Codex took over Claude Code's launcher, orchestration, monitoring, evidence-review and reporting duties during its limit interruption. One separate existing Codex CLI continued as the implementer/build/capture worker. That is what the “repair worker” referred to; it was not another Claude session. Root Codex remained the independent reviewer and did not operate the game or edit plugin source directly. There has been one worker/brief in flight and one idle mailbox watcher.

The recorded authority is [OD14 takeover ruling](094-47s-owner-takeover-ruling.md). Later instructions changed the work order to 4.25 plus dashboard first and authorized overnight continuation without a RAM admission wait. They did not waive correctness gates or permit plugin release.

## Work completed after takeover

| Area | What was done and what it established |
|---|---|
| Initial reconciliation, 094-47s | Reconciled 1,021 metadata records, checked 368 timestamp and 5,147 header proofs, and rejected ten synthetic faults. This first run stopped on the then-required D: storage floor after 3,974 of 22,628 protected-file hashes. It made no source/build/package/dashboard changes. The stopped result was preserved. |
| 4.25 compilation and provenance | Corrected narrow header/include/predicate and verification-tool issues, completed normal Editor and Game builds, checked binary inventories and archives, and verified no engine or engine-plugin compile actions. Later native repairs were rebuilt and checked again. Current normal and strict Editor/Game checks pass; strict coverage includes nine direct plugin translation-unit actions per target plus retained build coverage. |
| Protocol qualification | Reviewed transport/protocol checks and rejecting controls. An earlier stage recorded 543 transport checks and four rejected mutants. The current native candidate also passes its 16-check packaged protocol smoke test. These are separate from the real desktop and full-capture tests. |
| Dashboard | Kept `lod_corruption` visible, added its display metadata, and removed it from the hidden-ID set. The feature branch changes only `src/anomalies.ts` and `src/store.ts`. All 69 existing tests, TypeScript and frontend build pass. A native packaging/embedding problem in the first desktop attempt was corrected; the second executable is the qualified one. |
| Development package | Produced the 4.25 Development package, verified cooked resources and native/source provenance, and subsequently staged the repaired native executable without another cook. The current package has 34 files totaling 2,369,785,987 bytes. Its pak is preserved; the later repair changed the game executable. |
| Listener repair | Fixed the observed occupied-port behavior by requiring an actual IPv4 loopback listener owned by the game process. Failed starts destroy the attempted context, preventing a latent listener from taking the port later. Random-token restart and configured-token retention cases pass. |
| Stop/drain repair | Changed normal early stop to stop admitting new frames while allowing accepted readbacks/writes to drain. The server's terminal response waits for the bound capture session to finish. Warmup cancellation remains distinct. Partial receipts now honestly report the frames written and paired, with zero drops, and preserve the planned 2,352-frame count. |
| C5 route ordering | Made the selected default routes use the historical console order consistently, with extra routes ordered separately. Seed, route membership, schedule and requested frame count were retained. The new auxiliary comparison now passes, but the main comparison still fails. |
| Live observation repair | Replaced the ineffective late depth query with a separately owned observer thread that obtains actual replies at startup and after frames 80 and 2,300, then joins before closeout. Synthetic rejection tests and actual full captures verify it. A sent command is never counted as a reply. |

Current native repair commit: `d10bbe859e81ef3a0a0e15dfc5224181cc7911b8`. Current local plugin feature tip: `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6`; the later tip has no native-input difference from the built commit. The last native repair touched nine files across the 4.25 capture/session/labels adapters and control-server transport/subsystem/build rules. Its source review is [here](../_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47u/source-d10bbe8-independent-review.json).

## Runtime validation completed

Four accepted lifecycle cases use the current package and current desktop executable: random token and configured token on both D3D11 and D3D12. They verify desktop discovery/authentication/catalogue/snapshots/LOD display and normal closing, listener exclusivity, restart behavior, warmup cancellation, a second capture, reconnect, stable terminal artifacts and restoration. Their early-stop captures contain 38 or 40 paired frames, zero drops, and correctly remain PARTIAL rather than pretending to be complete 2,352-frame captures. One earlier configured-D3D11 observer STOP is retained.

The corrected full reference console capture in 094-47v independently passes:

- 2,352 requested, written and paired frames; zero drops; 624 clean-prepass and 1,728 scored frames.
- Full H4/image/label checks, 443 strict texture measurements at IoU 1.000 with zero spill, and 140 clean timing-edge/restoration checks.
- Three actual depth replies of 1; the late reply arrived 3.011 seconds before completion.
- All 8,323 frozen raw files independently rehashed, totaling 1,702,964,743 bytes.

The earlier 094-47u reference capture remains failed because it did not obtain the third actual reply. Its otherwise-passing frame checks did not erase that failure. The four comparison legs were not launched in 094-47v because the complete remaining runtime/readers/drain margin did not fit its bound; that time refusal is preserved.

The separately authorized 094-47w unit began at 08:32:26 IST and reuses the same package, dashboard, accepted reference capture and lifecycle evidence. It performs no native build, cook, package stage, dashboard rebuild or further 5.1 work.

| Current 094-47w test | State at this snapshot |
|---|---|
| Full C console D3D11 | Worker operation and protection ledger PASS. Root independently recomputed the full image/label/strict-texture checks: PASS. Root rehashed all 8,323 sealed files: PASS. All three live depth replies independently bound to the game/epoch; late reply 4.250 seconds before completion. |
| Full C dashboard-protocol D3D11 | Worker operation, image/label/strict-texture checks and protection ledger PASS. 2,352 frames, zero drops. Root independently verified all three actual depth replies, with the late reply 2.331 seconds before completion. Root verified terminal status, session identity, seed and reconnect against actual capture artifacts. Full independent image/raw reprocessing remains pending at this snapshot. |
| Follow-up after full dashboard capture | Same surviving listener accepted another start; it was cancelled during warmup without frames or a delayed physical start. This is separate from the full capture's completion. |
| Exact main D3D11 comparison | Worker reports **FAIL, 1,893 differences**: 1,639 census-cycle differences, 172 in-flight-batch differences, 76 private-pass verified-count differences, and six label-schema comparison entries. Root reproduction and diagnosis are next. These fields have not been dropped or given new tolerances. |
| Auxiliary D3D11 comparison | Worker reports **PASS, zero differences**, covering annotation/protocol/run-summary/private-resource/host-depth/routes artifacts. Root reproduction remains pending. |
| Full C D3D12 console/dashboard pair | UNRUN. Exact D3D11 equivalence is its prerequisite and has not passed. Earlier D3D12 lifecycle success is not full-pair qualification. |

The actual desktop connection/lifecycle tests and scripted dashboard-protocol full captures are distinct evidence. This record does not claim that the full capture was driven by clicking Start in the desktop UI.

## Current files and source state

- **AnomDash executable:** `D:\IA_Europa\Receipts\09447t\dashboard-build-r4\AnomalyDashboard.exe`
  - 11,190,784 bytes; SHA-256 `c8120946ff5bb4daa713286510f091b75760ae41f0895480adea1a14e481be28`.
  - Dashboard feature `feat/europa-425`: `b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5`.
- **4.25 Development package:** `E:\IA_BuildCache\09447u-package\WindowsNoEditor`
  - Game EXE SHA-256 `e8166cda4ac636f10d70ffa28db3e33da8997a284694b1554b9b79612c416110`.
  - Pak SHA-256 `d926673c1e26886f69292162a87bdfcfbfaade283ef83b21e71e48c8dc940f56`.
  - [Complete package manifest](D:/IA_Europa/Receipts/09447u/package-manifest.json).
- **Plugin local/remote:** local `bd767ca8ac7cd1ca18abe34d9cc66764fee100a6`; origin remains `c5d0919e5074404d26da8c16ccf845b360bde383` at the last verified check. No plugin push, merge or tag.
- **Dashboard publication:** not pushed yet. The independently reviewed two-file component is eligible for feature-only publication under OD4 after the active worker's protection checks and exact closure. Its master remains `7ea2e51bf55e19c2949641bbff2ec72afb4ae493`. No refs are changed during active protection checks.

The endpoint is `ws://127.0.0.1:8077`. The configured-token location is the selected UserDir's `Saved/Config/WindowsNoEditor/Game.ini`, `[AnomalyControlServer]`, `Token`; otherwise a token is generated. No token values are copied into this writeup. The retained 4.25 preset uses the four defaults including LOD, seed 4242, native PNG and 2,352 frames. Live preview and wider capture options are not newly claimed. Shipping remains excluded.

## Resources, protection and the 5.1 question

The earlier 5.1 compile was run for the then-required compatibility check. It completed with 17 plugin actions and no engine/engine-plugin actions. After your explicit 4.25/dashboard priority instruction, its owned operation was safely closed and further 5.1 qualification was deferred. No 5.1 editor/game runtime qualification is claimed, and the overnight continuation has not started another 5.1 operation.

After the overnight instruction, physical operations use immediate memory sampling with no RAM admission wait. Severe-pressure stop/drain protection remains. No owner applications were closed, and no paging, power, security or global settings were changed.

No IA_BuildCache deletion, evidence relocation or new blanket compression has been needed. The most recent sample near 09:10 IST was approximately C:18.3 GiB, D:35.7 GiB and E:22.8 GiB free, above the applicable floors. Current package/evidence/baselines are preserved and are not treated as disposable caches.

Completed units have full protection, artifact and process-closure reviews. Canonical entry/exit checks cover 382,970 inventory entries and hash 22,628 protected binary/config files totaling 100,696,938,006 bytes. Root parses and binds the complete worker proof and independently refreshes selected source/artifact/raw evidence; it does not falsely claim a second full 100 GB hash on every review. Owner settings and the 2,589 inventoried dashboard library-file entries remain unchanged. The active unit's final exit proof is still pending.

## What remains and what is not cleared

The immediate work is to independently reproduce the new exact-comparison failure, finish the dashboard's independent image/raw review, inspect the full worker report and exit protection/accounting, and verify every owned process is closed. Then the independently passing dashboard component can be published within OD4. A corrective native task, if justified by the actual differences, requires its own concrete scoped disposition; a failed comparison is not relabelled as passing.

Camera clipping was already unqualified when the implementation takeover began. The 094-46s D3D12 1279x719 case failed D4 and strict texture G3; GO3 is unrun. Those inherited failures remain open. Camera is opt-in, plugin publication/release remains held, and the shipped 094-45r office card is unchanged. The 4.25/dashboard work was explicitly decoupled from those camera holds for local development; it did not clear them.

**Chat review NOT REQUIRED** for the presently authorized validation, source-backed diagnosis, independent evidence review and qualified dashboard-only publication. **Chat review REQUIRED** at any proposed change to capture/label meaning, correctness tolerances or plugin release authority; the existing plugin merge/release checkpoint remains mandatory.

## Where to find the full trail

- [Current orchestrator state/read order](../ToCodex/CURRENT.md).
- [4.25/dashboard local-validation card](094-47-office-card-europa-dashboard.md).
- [Initial takeover disposition](094-47s-takeover-codex-disposition.md).
- [Reviewed build/5.1 safe checkpoint](094-47t-r2-codex-checkpoint-review.md).
- [Overnight resource authority](094-47t-owner-overnight-resources.md).
- [Current comparison-unit scope](094-47w-root-comparison-disposition.md).
- Worker evidence: `D:\IA_Europa\Receipts\09447t`, `09447u`, `09447v`, `09447w`.
- Independent review evidence: `D:\IntrusiveAnomalies\_relay\runs\2026-10-16-094-01-europa-phase-a-plan\root-receipts-094-47t-r4`, `root-receipts-094-47u`, `root-receipts-094-47v`, `root-receipts-094-47w`.

This snapshot preserves the distinction between a working local connection/capture path, passing individual captures, failing exact equivalence, and release readiness. Work is still active; no successful overall delivery is asserted.


## 2026-11-06 09:53 IST addendum — Chat ruling applied

The current authority is `_reviews/094-47w-chat-ruling.md` plus `_reviews/094-47w-gr1-clarification.md`. The earlier release/camera holds above are historical. GR1 now PASSES by explicit owner-relayed Chat ruling. Console/console cycles differ on1339 rows (max8,mean2.72); console/dashboard on1639 (max26,mean11.00), both recorded as session nondeterminism. Private verification and in-flight differences match the baseline. Six schema entries are only the allowed timestamp t=320 number representation. All780 masks match exactly after existing actor-ID binding;1572 clean frames match; labels/events/schedule/routes/seed/counts/restoration agree. No new baseline capture ran.

GR2 drops the D3D12 full pair. GR3 qualifies opt-in camera_clipping at1280x720 on bothRHIs and records distant-geometry depth-precision speckle atsome resolutions:1279x719 farfloor rows373–375,max117;plainfloor<=2; onepixel(375,239)acceptedonlyinsideheldcamera sessions. Default strictG3 staysIoU1/zero spill. Further5.1 qualification remainsdeferred.

09447w closed at09:30:52IST,exit0. Complete report,59,151file sealedunion,final protection and exactnative/helper/CLIclosure independently reviewed. Root result is C_COMPONENTS_PASS_GR1_EQUIVALENCE_PASS_CLOSED. Historicalstrict1893differenceFAIL ispreserved,notrewritten.

One09447xfinalgateworker launched09:40:43IST to finish the retainedGO3default-only1279x719D12compatibilityrun andonefinalFJ1/fullP9exit proof. No rebuild,newpackage,5.1work,fullD12pairorcleanup. The4GiBDceiling covers a measured3,128,529,168byteforecast. The newgateisunderGR3/GR4,notanotherconsolebaselineorimplicitretry.

Plugin prepushcontent scanPASS:c5d0919..bd767ca,25commits118files11954addedlines,no secret/codename/root-listing/binary/build-output finding; all flagged contextsreviewed. Remote plugin stillc5d0919, dashboardfeatureabsent. BothpushesawaitfinalGO3/protection/closure; no publication claimed yet. Root ispreparingtheofficecardandClaudeprojectcopy. Chat review NOT REQUIRED fortheseGR4actions; a newcorrectnessfailureorproposedmeaning/tolerance/release-scopechangerequiresreview.


## 2026-11-06 10:42 IST — GO3 completed; final proof running

The owner explicitly waived idle checks/waits and approved stopping the running read-only protection scan and worker after the interruption was explained. The exact owned x process tree was closed; no game/build existed in x, and its partial P9 has no PASS credit. All partial receipts remain preserved. This resolved the automatic review's initial rejection of inferred stop authority; the actual stop used the subsequent explicit approval.

The single y continuation launched the unchanged4.25 Development package at10:24:15IST and the game exited normally at10:27:24IST. Exactly one physical GO3 attempt ran, default-onlyD3D12 at1279×719, seed4242, camera disabled. The actual admission receipt records idle_checked=false and wait_seconds=0. No RAM admission wait, build, new package, new baseline or D3D12 full comparison pair was run.

GO3 passes:2352requested/written/paired,0drops,780mask frames,69events, restoration intact;443 strict texture measurements atIoU1.000 with0spill and140clean edges. Actual startup/live80/live2300 depth replies all returned1, and the observer joined. Root independently reviewed the actual replies, counts, dimensions, strict measurement/edge rows and restoration without repeating the pixel decode.

The established14-file key/type comparison reports no removal or type change, seven added paths and one new file. All24 retained card commands have recorded outputs; a literal-percent batch wrapper issue was corrected without rerunning the capture or schema gate. The updated handover's PowerShell reads also pass. Root corrected its draft's blinking route / blink annotation alias before finalizing the card; the initial draft/output remain preserved.

FinalFJ1 passes, including unchanged protected source/ref/index/package bindings. The one complete canonical exitP9 is now running AFTER the successful capture. Publication has not occurred yet. Prepared root publication guards require final proof, exact worker closure and fresh remote checks before plugin then dashboard pushes. The final card, Claude-project copy and final report remain the next authorized actions.

Evidence: `_relay/runs/2026-10-16-094-01-europa-phase-a-plan/root-receipts-094-47y/` and `D:\IA_Europa\Receipts\09447y`. Chat review NOT REQUIRED for the explicit operational waiver and existingGR3/GR4 scope. Next mandatory Chat checkpoint: a new correctness failure or a proposed meaning/tolerance/release-scope change. No correctness gate was loosened.

## 2026-11-06 final addendum — published and handed over

The final GO3 capture, FJ1, one complete canonical exit P9 and exact worker/child closure all passed independent review. The worker exited 0. P9 checked 382,970 inventory entries and 22,628 protected binary/config hashes totaling 100,696,938,006 bytes, with no differences. The interrupted x partial proof retains no PASS credit; the complete y proof followed the capture. No correctness gate was loosened.

After fresh pre-push scans, the plugin feat/europa-port was fast-forwarded to bd767ca8ac7cd1ca18abe34d9cc66764fee100a6 at 10:58:05 IST, then dashboard feat/europa-425 was published at b8ffaca1d88a2c89d1ed69adaac85501cc2fc0b5 at 10:58:21 IST. Fresh ls-remote checks verified both exact heads and unchanged other remote heads/tags. No force, tags, master changes or release merge. The protected plugin worktree/index stayed unchanged.

GR1 passes under the explicit Chat ruling: console/console cycles 1,339 rows / max 8 / mean 2.72; console/dashboard 1,639 / 26 / 11.00, accepted as session nondeterminism with both magnitudes retained. Likely cause is cumulative readback timing plus dashboard snapshot game-thread work. GR3's 1279×719 held-camera side effect remains documented and does not relax the strict default-pool gate. GO3 passes on the final package with 2,352 paired frames, zero drops and no removed schema key or changed type. The D12 full pair is dropped under GR2; further 5.1 and office-fork qualification remain separate.

The [final report](094-47-final-report.md) and [handover card](094-47-office-card-europa-dashboard.md) are complete. Card SHA-256: 7a2d1646e5f1efa4571e00031df42fa7a9a5f8c5c86efb4de866365094539028. The owner said “I'll hand it over myself” and now handles the Claude-project copy. Codex stopped UI control without uploading a file or sending a message. All Europa physical work is closed; the single mailbox watcher is idle and its queue is empty. The owner can use the PC normally.

Final evidence is in root-receipts-094-47y, including final-independent-closeout-review.json, both publication receipts, project-copy-receipt.json and final-handover-receipt.json. CURRENT.md now contains a clean final handoff; its prior chronological version is preserved. Historical holds and pending states above are retained as dated history, superseded by this final disposition.

**Chat review NOT REQUIRED** for completed GR4 publication/handover and owner project copy. Next mandatory Chat checkpoint: a new correctness failure or a proposed change to meaning, tolerance or release scope.
