# 084-02b — m52 label-sync fix: F1–F7, Shipping, resource replacement, purity scope and the two-sided gate

Date: 2026-09-28 (IST) · Claude Code (Opus 5.5, xhigh), headless, fresh session · Branch `fix/m52-label-timing`
Spec: `_reviews/084-02-chat-ruling-codex-fix-review.md` (decisions 1–3), against Codex's
`_reviews/084-02-codex-m52-fix-review.md`. Governing rule: `085-00` ADDENDUM (labels and masks match the picture on
both edges, 0 frames off, from pixels).

**Compiling only. Nothing was launched, staged or swapped; no game, no editor, nothing that takes focus.** The main
checkout stayed on `m51` `53bf725`; `master` untouched. Work happened in the branch worktree on the warm scratch host
`E:\IA_BuildCache\_r84_host`.

## 1. What changed (commit `d798900`, 14 files, +2,217/−201)

The pure logic is `Source/AnomalyInjector/Public/AnomalyStuckMipWindow.h`. It is standalone C++, so the selftest
checks exactly the predicates production calls.

| item | the correction | where |
|---|---|---|
| **F1** | A trail confirmation counts only for a baseline record whose frame was armed with **no stream operation pending and no held texture still restoring**. That settled flag is frozen per fire in `FinalizeRenderTruthArm`, the same tick the request is armed, before its family renders. An unsettled baseline is still labelled `Out` (the picture is baseline), but it breaks the confirmation run. The 084-01 "back but still streaming" race keeps its anomaly-side tracking (`Revert` tracks a busy texture), and that restoring list is exactly what the settled flag reads. | `FTrail::Process`; `FinalizeRenderTruthArm`; `Anomaly_StuckLowMip.cpp` `Revert`/`TickAlways` |
| **F2** | `FTrail` is now an **ordered per-event cursor**: `NoteArmed(si)` at arm, `Receive(si, verdict, settled)` at drain, a 256-slot ring advanced only through contiguous session indices. At the drain, a frame whose membership is not final yet is held (`ORDER HOLD`), together with the rest of its batch, until the earlier requests report. A request whose snapshot is gone is marked **Missing**, as is one still pending after 16 ticks while later frames arrived. Missing means unknown: labelled if the frame exists, and it breaks the run, so a gap is never a clean confirmation. Closure is **final only at a fence**: the last SI armed when the second confirmation arrived. A held, unknown or missing receipt at or after closure **reopens** the event. `HandleTrailReopens` re-attaches it, re-labels the not-yet-written frames armed after the closure as unknown (labelled), and counts the written ones. | `AnomalyStuckMipWindow.h` `FTrail`; `ComputeRenderMembership`; `ResolvePendingMembership`; `ResolveTrailGaps`; `HandleTrailReopens`; `ProcessCompletedFrames` |
| **F3** | `Combine` with no watched texture returns **Unknown** (labelled), never before-apply. The actual gap: `CaptureCurrentFrame` froze the watch before `BeginFire` in the same tick. The watch is now **re-frozen at `FinalizeArmedLabel`** through `FAnomalySveCapturer::ExtendRenderWatch`, which adds only while the request is still in `PendingWanted`, i.e. before `BeginRenderViewFamily` consumes it. So an Apply-tick frame is labelled from its own record: baseline if its family rendered before the drop, held if a later family served it. If the request was already served, the frame is unknown and labelled. Event texture sets are cached per event, with the Restoring list as fallback, so an anomaly that reverted itself (target lost or contamination) keeps its watch. | `FinalizeRenderTruthArm`; `ResolveEventTextures`; `AnomalySveCapturer.cpp` |
| **F4 / purity (decision 3)** | **At Apply:** users are counted across **all loaded levels, active or not** (`World->GetLevels()` plus every streaming level's loaded level), over every owned component of every actor in them, **registered or not**, every primitive type plus decals. The scope predicate is `InPurityScope` in the pure header. **During the hold:** a hold monitor. See §2 for why it is not a delegate. A new non-target user of a held texture **reverts the hold immediately**, and every labelled frame of that event from the contamination point carries `stuck_mip.contaminated = 1`. | `Anomaly_StuckLowMip.cpp` `BuildWorldTextureUsers`, `StartHoldMonitor`, `ScanHoldForNewUsers`, `ConsiderHoldUser`; capture `ServiceStuckMipTrails` (polls `ConsumeAnomalyContamination`), `ApplyRenderTruthToSnapshot` |
| **F5** | `FinishRun` opens a trail for any m52 event still live when the run is cut (frame cap, stop or teardown), counted in `stuck_mip_live_window_cut`. Trails still attached at run end are **carried** in `CarriedTrails`, which the per-run reset does not clear. Their target reservation and an m52 refusal (`restore_carried`) are re-applied after the run-end clear. `StartRun` **adopts** them: rebased into the new run's session-index space, re-reserved and refused, then labelled per frame from the record until settled baselines close them. Holds reverted outside a render-truth run, such as `StartRun`'s own clean-slate revert of a manual hold, are adopted as orphan inherited trails. Nothing is carried on world teardown. | `CarryAttachedTrails`, `AdoptCarriedTrails`, `ApplyCarriedTrailOwnership`, `FinishRun`, `StartRun` |
| **F6** | m55 never receives a provisional "not labelled". An unready queue entry **waits** until one of three things: a final (non-order-pending) record, which gives exact membership; a terminal request, meaning its snapshot is gone; or the 64-tick hard bound. The last two submit it as **unknown, i.e. labelled**. `BeginClosure` (which ends m55 events) is **deferred** until the drain and the forced flush when m52 observations are queued, so final entries are not discarded as late. | `ResolveObserve`; `FlushObserveQueue`; `FinishRun` |
| **F7** | `ServiceTargetMask` carries the deferred mask's age when it re-defers, so `DeferredMaskExpired` (> 16 ticks) actually fires; the mask payload is released and the row reads `mask_state` unmeasured. | `ServiceTargetMask`; `ServiceStuckMipTrails` |
| **Shipping** | `GStuckMipLegacyPurity`, `SetLegacyPurityLever`/`IsLegacyPurityLeverOn` (definitions and exported declarations) and the legacy branch in `Apply` are under `#if !UE_BUILD_SHIPPING`; in Shipping `bLegacyPurity` is a constant `false`. The capture-side lever registration was already inside `ANOMALY_CAPTURE` (0 in Shipping). **Not proven by a Shipping build** (none was built); it is a source guard. | `Anomaly_StuckLowMip.cpp`; `AnomalyStuckMipStats.h` |
| **Resource replacement** | The anomaly records each held texture's `GetResource()` identity at Apply. `ClassifyTextureSample` returns **Unknown** (labelled) when a render-thread sample's resource differs from the bound one (bound from the first valid sample if Apply saw none), logs `RENDER RESOURCE REPLACED` once per texture, marks the texture entry `resource_replaced: 1` and counts the frame. | `ClassifyTextureSample`; `ComputeRenderMembership` |

`run_summary` gains 15 keys, emitted only on runs that fired m52 or inherited/carried a trail (the 084-02 condition, widened
by the carry counters). The keys cover contamination, resource replacement, reopens, missing frames, frames unwatched after
closure, re-frozen and missing watches, order-held frames, the live-window cut, carried and inherited restores,
unresolved m55 observations, and the purity scan's inactive-level and unregistered users. Per frame, `stuck_mip.contaminated`
and `stuck_mip.watch_missing` are emitted only when set. `stuck_mip.render_state` no longer has a `before_apply` value.

## 2. Decision 3's "component-registered" subscription: what UE 5.1 offers (→ G346)

UE 5.1 has **no global component-registered delegate**. `UActorComponent` exposes only the physics-state delegates and
`MarkRenderStateDirtyEvent`, and the latter is broadcast from `MarkRenderStateDirty()` only for a component that is already
registered with its render state created (`ActorComponent.cpp:1843`). The one global hook that sees every new component,
`FUObjectArray::AddUObjectCreateListener`, is iterated **unlocked** in `AllocateUObjectIndex` (`UObjectArray.cpp:241`),
including from the async-loading thread. The engine registers such listeners only once (cooker, DDC commandlet). Adding
one mid-game races the loader, so it was rejected.

The hold monitor therefore uses three routes, with no engine change and no threading risk:
- **(i) per-tick new-component diff:** each tick of the hold, a `TObjectIterator<UPrimitiveComponent>` /
  `<UDecalComponent>` (class hash) pass against an `FObjectKey` set recorded at Apply. Objects still loading, and objects
  whose level is not yet a loaded level of the world, are judged when they settle. This covers spawned, streamed-in,
  newly created and newly registered components alike;
- **(ii)** `FWorldDelegates::LevelAddedToWorld`, the subscription the ruling names, scanned immediately;
- **(iii)** `MarkRenderStateDirtyEvent`, for a material change on an existing component.

The per-tick cost is **not measured**. It is bounded to the hold (about 8–30 frames per event).

## 3. Unit tests (`tools/m52_window_selftest.cpp`, MSVC `/W4 /WX`, standalone)

**93 checks, 0 failures** (41 at 084-02). Every correction is proven **both ways**: a verbatim replica of the 084-02 logic
(`namespace Legacy08402`) reproduces the defect, and the corrected logic passes the same case.

| case | 084-02 replica | corrected |
|---|---|---|
| F1 pending stream-out | closes on two baselines while a stream-out is pending; the late low-mip frames 4, 5 are drawn after detach, unlabelled | unsettled baselines do not confirm; 4, 5 labelled; closes at 7 |
| F2 cross-batch | baseline 100, baseline 102 (before 101), held 101 → closed early, never reopens | 102 held by the cursor; in order baseline/held/baseline = 1 confirmation; closes at 103 |
| F2 late older baseline | enters SettleTail with a zero tail (over-label) | Out |
| F2 gap | two baselines across a dropped request close the trail | the head ages out; missing = unknown, breaks the run; only the post-gap baselines confirm |
| F2 fence / reopen | — | closing with in-flight frames is not final; a held in-flight frame aborts it; a held receipt after final closure reopens it and is labelled |
| F3 unknown fire | empty watch = before-apply = clean | unknown = labelled; a re-frozen watch reads held when a later family served the arm, baseline when the earlier one did |
| F5 stop/start | the trail list is reset, so no event labels the next run's blurred frame | carried, rebased, labelled, closes on settled baselines; unresolved stays unresolved |
| F6 | provisional negative after 16 ticks | waits; terminal / hard bound / forced = labelled |
| F7 | age reset every service, never dropped in 60 ticks | dropped at tick 17 |
| resource replacement | a replaced resource reading the old baseline count is clean | unknown, flagged replaced |
| purity: inactive-level user | outside the census, so the texture reads pure | counted, refused |
| purity: unregistered user | skipped, reads pure | counted, refused |
| mid-hold user | nothing watched the hold | revert now, frames flagged `contaminated` from that point |

Shipping cannot be unit-tested standalone; it is a source guard (§1).

## 4. Harness (`_reviews/084-03-*`)

- **Gate (i) two-sided** (`gate_sync`): the labelled window must start at `first_vis` and end at `last_vis` (the pixel window
  from a frozen oracle built on the independent null), with no unlabelled visible frame inside it and no labelled frame
  outside it. Censored edges (no 3-frame clean suffix, or a frame-index gap at the edge) are unmeasured, not passed.
  Isolated visible frames outside the window are reported, not required. **The first dry run showed why (→ G345):** the
  banked event 577 has an isolated below-threshold frame 497, ten frames before its onset (507). "Every visible frame
  labelled" plus "labels start at `first_vis`" made the pixel-repaired window unpassable.
- **Four can-fail cases on real banked pixels.** Each is a real session **copy on disk**: `labels.jsonl` rewritten, mask
  PNGs written, frames being the banked pixels by absolute path. Each is judged against the oracle measured once on the
  original. Dry-run result on the baseline stand-in `B3_BASE` event 577 (pixel window 507–521):

  | variant | label | two-sided (i) | record check | window (ii) |
  |---|---|---|---|---|
  | repaired | 507–521 | PASS | PASS | PASS |
  | onset +2 | 509–521 | **FAIL** | FAIL | FAIL |
  | offset −5 | 507–516 | **FAIL** | FAIL | FAIL |
  | early-onset over-label −2 | 505–521 | **FAIL** | FAIL | FAIL |
  | late-offset over-label +4 (record extended as `settle_tail`, Codex's counterexample) | 507–525 | **FAIL** | FAIL | PASS |

  The last row is the counterexample reproduced on real data: the window gate alone passes it and the two-sided gate
  catches it. `proven = True`. The verbatim 084-02 one-sided gate also fails the repaired window on this event, because
  of isolated frame 497, so it could not have certified a correct build here either.
- **Residual measurement:** per judged event on the fix build:
  - `residual_after_record` = last pixel-visible frame − last record-member frame, signed;
  - `residual_before_record` = first record-held frame − first pixel-visible frame.

  These are reported for B0_FIX and B3_FIX (AA as delivered) and B9_FIX (AA off, **now a required leg**). The
  `closure_rule` names the constant settle tail if the residual is one constant; otherwise it states that no constant gives
  0 and an in-engine settling detector is needed. The onset gets its own flag.
- **Gate (iii):** the `TEXUSERS` line must carry `scope=all_loaded_levels`, else `FAIL-SCOPE`. The enumerated textures must
  **join per fire** to that fire's `HOLD` lines, else `FAIL-JOIN`. Inactive-level and unregistered users are counted.
  `HOLD CONTAMINATED` lines are listed. The legacy-purity lever still has to be caught (`P1_FIX`).
- **`084-03-gateselftest.py`:** 15 offline known-answer checks, 0 failures, including:
  - Codex's counterexample;
  - early-onset, onset +2 and nothing-labelled;
  - the isolated-frame case;
  - the closure rule;
  - the purity PASS / FAIL-SCOPE / legacy-caught / FAIL-JOIN / PASS-REFUSED lines.
- **Dry run** (stubbed launch, banked stand-ins, `dry2_20260928_164959`): exit 0.
  - B5_BASE null PASS (rock worst drop 2.73 %).
  - B3_BASE (i) **FAIL** as predeclared: label onset 1 frame after the first visible frame, 7 unlabelled visible frames
    after it. That is the 084-01 diagnosis.
  - Doctored proof proven.
  - Fix legs have no stand-ins in a dry run.
- **Fix archive pinned by name** (`FIX_ARCHIVE_NAME='m52fix-FD621B27'` in `084-03-common.py`). The old "exactly one
  `m52fix-*`" rule would have refused the window, because `m52fix-562EE7A4` stays on disk.
- **`B9_FIX` is required.** The expected bench time rises to 50–65 min (up to ~95 with retries), about 20 GB of bank.

## 5. Builds

- Game `StackOBot Win64 Development` on `_r84_host`:
  - the first attempt failed at exit 6: C2121, a `#if` inside `UE_LOG` arguments (→ G346 side note);
  - fixed in source; the rebuild was **exit 0**, 3 actions, 45.7 s. `AnomalyCapture` had compiled clean in the first attempt.
- Editor `StackOBotEditor Win64 Development`: **exit 0**, 13 actions, 137.8 s.
- **Warnings: 0 in plugin code** (neither log has a compiler warning).
- Exe **`FD621B27`** (`FD621B27E92DA23A6325532FDF0581C65374EAF7E9F14DDE3ACBD2DBB9C9A3CB`, 241,923,584 B).
  - Code-only; pairs with container `67EA1FE0`.
  - Archived at `_binary_baselines\m52fix-FD621B27\` with both logs, the failed first log and a README.
  - **Not staged:** the staged exe still reads `2FCDF059` (m53).
- A44 (UTF-16): every new token present (`HOLD MONITOR ON`, `HOLD CONTAMINATED` 2, `scope=all_loaded_levels`,
  `WATCH RE-FROZEN`, `ORDER HOLD`, `TRAIL GAP` 2, `RESTORE TRAIL REOPENED`, `RESTORE CARRIED`, `RESTORE INHERITED` 2,
  `RENDER RESOURCE REPLACED`, `m55 OBSERVATION UNRESOLVED`, `LIVE WINDOW CUT`, `stuck_mip.contaminated` 3, …). Controls:
  `IAI.Bench.StuckMipNoHold` 5 and `IAI.Capture.TargetMask` 6.
- Comment strip: 0 of 116 files changed. Diffstat checked: no encoding rewrite.
- `m52fix-562EE7A4` is superseded for the bench and **kept** until chat rules.

## 6. Codex mini-delta (Relay): CHANGES-REQUIRED, collected and NOT acted on

- **Run:** `runs\2026-09-28-084-02b-m52-minidelta`, Astra, max, 1,105 s, exit 0. `run-meta` agrees with the request. It
  reviewed `749a3bd..d798900` and verified all six harness hashes and the archived exe hash.
- **Collected, unedited, to `_reviews/084-02b-codex-m52-minidelta.md`**, with a ledger row.
- **Its fitness verdict:** **not fit for tonight's bench under decision 1's "correct first".** It says explicitly that the
  permitted zero-tail residual is **not** its reason.

| item | Codex's disposition | its finding, in one line |
|---|---|---|
| F1 | PARTIAL | Settled-at-arm fences an already-issued `PendingUpdate`, but not streamer intent computed under the hold and issued later (background mip calculation, deferred copy queue); an `AlreadyBack` texture gets no restoring entry. Source counterexample, not measured. |
| F2 | PARTIAL | The cursor is right, but reopen after a serviced closure does not restore masks (appends mask value 0) or m55 continuity, never resets `bEndEventsIssued`, and does not own the `restore_reopened` refusal, so a later reclose leaves the target reserved and m52 refused for the run. |
| F3 | RESOLVED | — |
| F4 | Apply scope RESOLVED; lifetime NOT | The known-set diff misses (1) an unregistered, known component that is given the held texture by `SetMaterial` and then registers (no dirty broadcast before registration); (2) a MID texture-parameter change (no component dirty). The monitor stops at `Revert` while the texture may still be low; startup seeds every global component as judged; cost unmeasured. |
| F5 | PARTIAL | Inherited trails have no mask record at the new run's start (records are made only for Auto's live fires), so the first inherited frames are labelled with unmeasured masks (P1). An intervening non-render-truth run's `FinishRun` drops the carried reservation and refusal (P2). The orphan synthetic `start_frame` needs explicit provenance. |
| F6 | PARTIAL | The 64-tick fallback is final only inside the m55 queue. A live (non-trailing) frame whose baseline receipt arrives after it gets label and mask out while m55 got labelled. |
| F7 | RESOLVED | — |
| Shipping | RESOLVED (source) | Not Shipping binary qualification. |
| Resource replacement | RESOLVED | — |
| Two-sided gate | comparator RESOLVED; gate NOT | Synthetic false passes: (a) a missing SI right after the window, or an engine-frame jump inside the clean suffix, is not censored; (b) a later visible **run** (20–24) is called "isolated"; (c) a measurable event with no masks passes the window gate; (d) purity PASS ignores `HOLD CONTAMINATED`, so it is an Apply-census PASS, not lifetime purity. |

- **Its answers to Code's challenges:**
  - (a) no permanent ORDER HOLD stall with normal ticking, but mixed batches are delayed and reordered;
  - (c) no new race in `ExtendRenderWatch`;
  - (f) no intended change for a run with no m52 activity and no inherited restore.

These are **information for chat's ruling**. Code did not change source, harness or build in response.

## 7. Deviations, stated

- **The component-registered subscription is a per-tick diff, not a delegate.** UE 5.1 has none, and the create listener is
  unsafe (§2). The diff covers the registration route and more; its cost is unmeasured.
- **The ORDER HOLD holds the rest of a batch**, non-m52 frames included, behind an order-pending m52 frame, to keep write
  order. Rows are unordered by contract (G162), so only timing changes. It is bounded by the 16-tick gap rule and is
  force-resolved at run end.
- **The F3 "proven before Apply by frame ordering" exception is not used.** Every Apply-tick frame gets a re-frozen watch
  and is judged by its own record, which is stricter than an ordering proof. With no record it is unknown (labelled).
- **Orphan inherited trails use a synthetic `StartFrame`** (`GFrameCounter + k·10⁹`) as a collision-free event key.
- **Nothing is carried across world teardown.** A world ending takes its targets with it; the anomaly's own
  "unverified at teardown" report stands.
- **The first commit message carried a BOM** (PowerShell `-Encoding utf8`, G141 again). It was amended **before** the push;
  nothing published carried it.

## 8. State and hand-off

- `fix/m52-label-timing`: `d798900` (source) plus this docs commit, pushed. No tag, no merge. `master` `b5f15a3`; `m51`
  `53bf725`.
- ⛔ **Codex's mini-delta is CHANGES-REQUIRED and calls the build NOT fit for tonight under decision 1 (§6). Whether 084-03
  runs tonight on `FD621B27`, or after further correction, is CHAT'S decision (NEEDS-DECISION).**
- **084-03 (tonight, only on chat's ruling and the owner's go):**
  `C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\084-03-window.py live`. It stages BASELINE `E0BE6F0A`, then FIX
  `FD621B27`, and always restores M53. 12 required legs.
  - Expect **50–65 min**, up to ~95 with pose or focus retries, and about 20 GB of bank on D:.
  - The fix legs may fail gate (i) by exactly the residual; that is expected and is the measurement.
- **Open, and not decided here:**
  - the closure rule, after tonight (chat);
  - the per-tick monitor cost;
  - Shipping, never built;
  - runtime evidence of every correction, which tonight's legs exercise only on the ordinary path. Contamination, reopen,
    carry, resource replacement and the order hold have **no bench lever** and are proven only by the unit test.
