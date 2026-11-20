# 081-20 — independent review of the Codex-authored m55 source (Stages 1–2 + completion clock)

Reviewer: fresh Claude Code session (Opus 5.5), headless, read-only. 2026-09-24, 03:08–03:45 IST.
Scope: `git diff 031a103 5649a6d -- Source/` (25 files, +1,979/−30). Out of scope: `9c75abe` (build 2),
Python tools, harness scripts. Contract: `docs/predictions/2026-09-20-m55-change-evidence.md` at `5649a6d`
(amendments first), `docs/change-evidence-fields.md`, chat rulings 081-04 … 081-19.
Evidence folder: `_reviews/081-20-evidence/` (source snapshot `src-5649a6d/`, the diff, every script and output).
Nothing was built, launched, edited or committed. No worktree was created; the snapshot is a `git archive`.

## VERDICT: GREEN — review complete, no P1 found

No path was found that publishes a wrong number or a wrong pair as measured, crashes, hangs, or leaks
reserved bytes. Two P2 findings (legacy side-effects of the stage's lifecycle and owner checks) and eleven
P3 items are listed for chat's disposition.

## Findings (file:line on 5649a6d)

| id | sev | file:line | defect | failure scenario | conf. | evidence |
|---|---|---|---|---|---|---|
| F1 | P2 | `AnomalyCaptureSubsystem.cpp:5629` (inside `if (bWroteSession)`, block ends :5717) + `:5722` + `:3020` / `:595`; `AnomalyChangeStage.cpp:461-481`, `:450-455`; engine `FileManagerGeneric.cpp:110-116` | A run cancelled before focus gets its folder deleted, then re-created with an empty `change_evidence.jsonl` | Dashboard Start → Stop before the game window is focused (m16's armed-pending cancel). `FinishRun` calls `BeginClosure` (:5221) but skips `CloseAndPersist` (it is inside `bWroteSession`), then deletes `RunDir` (:5722). At the next `StartRun` (:3020) or at `Deinitialize` (:595) the stale stage persists: `SaveStringToFile` → `CreateFileWriterInternal` re-creates the missing directory. Result: an orphan `session_<ts>/change_evidence.jsonl` (0 bytes) in the captures folder, in both delivery modes, breaking m16's "cancel writes nothing and deletes the empty session dir". | high (both halves read in source) | not exercised (every banked leg began) |
| F2 | P2 | `AnomalySceneViewExtension.cpp:76-85`; `AnomalyMaskSceneViewExtension.cpp:151-161` | With m55 on (default), a family with `Views.Num() != 1` (or whose render target/scene is not the capture's viewport/world) is rejected **before it consumes the arm and without publishing a key**; the mask pass for any family without m55 family data is skipped entirely | On a split-screen/multi-view host: no colour arm is ever consumed, so no key is published, `AfterPass` logs "NO KEY … frame DROPPED" per frame, and **zero legacy PNGs** are written; mask arms (including m26/census arms) are never served; `PendingWanted`/`PendingIssues`/`PendingArms` grow by one entry per captured frame for the whole run. The contract says multi-view is `unsupported_delivery` (an evidence refusal), not a loss of the legacy capture. ⚠ Pre-m55 multi-view was already unsound (the non-consuming key lookup submitted one readback per view under one RequestId, so the delivered picture could be view 1 while the labels describe view 0); m55 turns silent mispairing into a loud no-data. | high on code, reach unknown | unexercised (all fixtures single-view) |
| F3 | P3 | `AnomalySceneViewExtension.cpp:164-171`; `AnomalyChangeStage.cpp:487`, `:91` | `AfterPass` drops the legacy colour submission whenever a stage exists but the family carries no m55 data | Owner change mid-run → `ResetEpoch` (:88) → persist fails (disk/permissions) → `return` at :487 leaves the stage closed → `Issue()` returns null for the rest of the run → arms carry no issue → no family data → every later legacy frame is dropped. Double fault; contradicts `AnomalyChangeFamilyData.cpp:15-16` ("cannot drop a legacy PNG"). | high on code, double fault | unexercised |
| F4 | P3 | `AnomalyChangeStage.cpp:461-481` (GT spin), `:88`, `:482-486`; `AnomalySceneViewExtension.cpp:148-200` (RT drops without `Fail`) | `CloseAndPersist` spins on the game thread for up to 5 s, while mask service and writer enqueue are game-thread driven | (a) Production `ResetEpoch` from `Issue()` on an owner change freezes the GT until the 5 s wall; nothing GT-dependent can complete meanwhile, so all pending items end `closure_timeout`. (b) At run end/teardown, a frame dropped on the render thread (extent clamp, NO KEY, empty rect, post-consume `view_rejected`) never notifies the stage; with no later frames the closure waits the full 5 s wall on the GT. Bounded; no hang. | high | a) not exercised outside gates 7/15; b) gate 17 shows the 5 s wall path |
| F5 | P3 | `AnomalyChangeStage.cpp:379`, `:380`, `:387` | Reason labels disagree with the field reference / plan | A predecessor that timed out before its colour arrived reads `predecessor_undelivered` (field reference: "a skipped or timed-out predecessor is missing") — the only 2 banked `predecessor_undelivered` rows are gate-16 timeouts. A delivered-but-budget-refused predecessor reads `predecessor_missing`. A mask receipt from another family reads `view_mismatch` (plan §2.3: `mask_payload_missing`). All are honest refusals; only the names mislead. | high | banked (gate 16) |
| F6 | P3 | `AnomalyChangeMeasurement.cpp:163` | Event record `late_results` is the constant 0 | Plan §4.3 defines it as results rejected after the event was finalised; the field reference calls it the "frozen event-time value", which is structurally always 0. Real late rejects exist (197 across banked runs) but never reach the per-event field. | high | banked |
| F7 | P3 | `AnomalyChangeStage.cpp:109`, `:190` | `duplicate_completion` increments on every unsupported frame | Unsupported issues get a synthetic `bColourDone` at issue, so the real writer completion is counted as a duplicate — a fault-sounding counter on every JPEG / OutputHeight / backbuffer run. | high | not in bank (all banked runs supported) |
| F8 | P3 | 14 files (count in `count_comments.py` output) | 66 added comment lines against the no-comments convention | Largest: `AnomalyChangeStage.cpp` 21, `AnomalyBenchModule.cpp` 8, `AnomalyChangeMeasurement.cpp` 6, `AnomalyChangeStage.h` 6, `AnomalyCaptureSubsystem.cpp` 5. Already a merge-checklist item (081-17R). | high | — |
| F9 | P3 | `AnomalyChangeStage.cpp:19-20`; `AnomalyAsyncWriter.cpp` gate 3/5; `AnomalySveCapturer.cpp:377-381`; `AnomalyCaptureSubsystem.cpp` gate 2, teardown device path; `AnomalyInjector.uplugin` AnomalyBench entry | Bench surfaces are only partly fixture-gated; some carry host literals | `IAI.Bench.ChangeGate` 2/3/4/5 drop the legacy arm / colour PNG / readback / mask PNG at SI 8 and are **not** gated by `-IAIBenchFixture` (echoed in the summary). `AnomalyBench` ships, dormant, in any non-Shipping client Game build (gated by flag + map-name substring). Host literals in plugin modules: `/Game/StackOBot/Maps/MainWorld`, `CB_GateLevel`/`L_ShooterGym`, `StaticMeshActor_49`, StackOBot/Lyra camera coordinates. No host *types* — the letter of the game-agnostic invariant holds. | high | — |
| F10 | P3 | `AnomalyChangeMeasurement.cpp:62`; `AnomalyChangeStage.cpp:454` | Row buffer cap is a row count, not a memory bound | 100,000 rows × median 1,173 chars (UTF-16) ≈ 0.25 GB held, plus a same-size `FString::Join` and UTF-8 copy at persist on the GT. Outside the byte budget (declared for rows by 081-16). | medium | banked row sizes |
| F11 | P3 | `AnomalyChangeMeasurement.cpp:221-239`; `AnomalyChangeStage.cpp:327-333` | Hot path is a single serial pool worker | ≈45 ms per scanned pair at 1080p (13.5 ms at 720p) → ≈22 scans/s ceiling; bounded by K=4 windows per phase and co-labelled events share one scan, so ordinary recipes stay under it. During closure the worker holds a pool thread in a 1 ms sleep loop for up to 5 s, in the pool the PNG writer uses (081-18). | medium | banked `change_worker_ms_total` |
| F12 | P3 | `AnomalyChangeMeasurement.cpp:18` | Mean rounding rule is undocumented | `floor(x+0.5)` at 4 dp; a consumer using half-to-even disagrees on exact midpoints (5 in 200k random pairs). | high | port test |
| F13 | P3 | `AnomalyChangeStage.cpp:482-492` | `ResetEpoch` does not clear `DeferredEndCause`/`DeferredEndAt` | Not reachable with today's tick ordering (labels are observed at the end of the capture tick, before any later `Issue`); a robustness note only. | medium | — |

Not re-reported: the 64 MiB default in `5649a6d` refusing at 1080p (ruled a chat spec defect, fixed in build 2).

## Tested claims

1. **Invariant replay over all 365 banked sidecars** (`replay_invariants.py`): **0 violations**. 1,450 measured rows: 8-bin
   histogram sums to n, `gt8` = bins 4–7, sum inside histogram bounds, mean = half-up 4 dp, predecessor = N−1, colour/mask/prev
   receipts agree on rect/extent/format/epoch/cut, serving token = capture token, mask family = colour family, mask owner = arm,
   view index 0, onset `ref` = `chg`. 433 event records: phase `measured` ⇔ measured onset row (F1 rule), `pairs_measured`
   matches rows, running maxima correct. `change_bytes_retained` = 0 at the end of every run (no reservation leak on any exercised
   path, including epoch, budget, late, teardown and can-fail gates).
2. **Timeout contradicted by a late colour** (`late_after_timeout.py`, 425 logs): on the `5649a6d` clock (081-15) and build 2
   (081-17) there are **0** `closure_timeout`s outside the gate-16/17 can-fails and **0** timeouts later contradicted by an arriving
   colour. The 19 late rejects are all on the pre-fix 081-14 clock.
3. **Pixel arithmetic port** (`port_pixel_arithmetic.py`): the C++ loop ported line for line matches independent contract formulas
   on 400 random trials (0 mismatches); `d=8` not counted / `d=9` counted; the 0,3,6,9,12 walk gives adjacent `[0,0,0,0]` vs
   reference `[0,0,48,48]` (matches 081-19).
4. **Fuzzed model of the ordered stage and completion clock** (`clock_model.py`, 1,200 runs, jitter up to 1 s, 5% lost
   completions, mask holds): no hang, closure always ≤ 5 s after finish, bytes return to 0, never negative, no measured pair against
   a non-N−1 or unretained predecessor, every phase finalised. The 8-completion rule refuses labelled frames whose colour would
   have arrived only under heavy completion disorder (uniform 0–1 s: 1,824 over 200 runs); none at Stack-like (0.12 s) or
   Lyra-like (0.45 s) spreads.
5. **Worker cost** (`worker_cost.py`): median ≈45.5 ms per scanned pair at 1080p (10 runs), 13.5 ms at 720p (61 runs) —
   upper bounds, since total worker time includes non-scan work.
6. **Engine source checks**: extension data is held by thread-safe shared refs (`SceneView.h:1968-2040`), so the GT family and the
   renderer's copy share one object (no double free); `CreateFileWriterInternal` re-creates a missing directory (F1).

## Areas reviewed and found sound

- **Identity.** Token/epoch/cut minted under the lock at GT issue; stale generation rejected at every admission (`AcceptLocked`);
  a foreign family cannot consume an arm; the colour receipt is bound to the family that consumed the FIFO arm, and `AfterPass`
  refuses submission if that family's issue is not the key-ring entry; the mask receipt carries the family's colour token and
  family id, so a coalesced non-owner reads `mask_payload_missing` and a wrong-family owner reads `view_mismatch`; predecessor must
  be N−1, delivered, retained and same generation. The stage's session index and the delivered PNG index share one source
  (issue `RequestId` → snapshot).
- **Numbers.** The canonical buffer is frozen `const` before encoding and the encoder reads that same buffer; the mask is frozen
  after all filters (the MASK-TIE loop reads `Gray` before the move); sizes are checked before any indexed read.
- **Lifetime and threading.** The worker is single (guarded by `bWorkerActive`); the pending item is moved out before the lock is
  dropped for the pixel scan; no event/phase reference is held across the unlock; `Previous` is touched only by the worker and by
  `ResetEpoch` after the worker has stopped; the render thread and writers call the stage without holding their own locks (no
  lock-order inversion); every `FindChecked`/`check` is guarded.
- **Completion clock (`5649a6d`).** Matches 081-14 exactly: completions (delivered or failed) beyond `Phase.Last` / watermark,
  never issues; 5 s wall; gap rule of 4 suppressed while a closing phase covers the head; `Pulse` every tick wakes the wall check;
  the closing worker self-loops. The run-closure completion count is structurally 0 (nothing is issued past the watermark), so run
  end always resolves by readiness or the wall.
- **Bench isolation.** Twins compiled out of Shipping, absent from every pool/selector list, and `Apply` refuses loudly without the
  cvar, `-IAIBenchFixture` and a bench map; `AnomalyBench` is excluded from Shipping and from Client/Server targets.

## Open questions (could not be made concrete)

1. **Simulate-in-Editor:** if `World->GetGameViewport()` is null there, the owner check (F2) would reject every family and m55-on
   capture would write nothing. Not verified from source within the bound.
2. Is split-screen / multi-view in product scope? If yes, F2 is data loss on that host class and should be re-rated.
3. Downstream effect of F1's orphan folder on `encode_watcher` / the dashboard session list (no `run_summary.json` ever arrives).
4. Is 8 later completions an adequate closure margin on hosts with high writer-completion *disorder* (not latency)? The model says it
   refuses early under ±0.5 s disorder; banked Lyra 1080p shows none.

## Coverage

- **Read fully:** `AnomalyChangeStage.h/.cpp`, `AnomalyChangeMeasurement.cpp`, `AnomalyChangeFamilyData.h/.cpp`,
  `AnomalyBenchModule.cpp`, `Anomaly_ChangeCase.h/.cpp`, `AnomalyBench.Build.cs`, the descriptor; every hunk of the subsystem,
  writer, label-writer, SVE capturer, colour SVE, mask SVE, control-server and injector diffs, plus the surrounding functions
  (`ServiceTargetMask`, `ArmTargetMaskOwn`, `OnWorldTickEndMask`, `CaptureCurrentFrame`, `Tick` phase switch, `FinishRun`
  (m55 parts, drains, cancel path), `Deinitialize`, `DrainAsyncToCompletion`, `Drain_RenderThread`, `BeginRenderViewFamily`,
  `AfterPass_RenderThread`, mask arm serving and result distribution); the completion-clock commit on its own.
- **Skimmed:** the pre-existing mask reduce body in `AnomalyMaskSceneViewExtension::Drain_RenderThread`; `WriteRunSummary` merge.
- **Not reached:** runtime behaviour in Simulate-in-Editor; auto-pool/selector lists were checked only by their absence from the diff.
