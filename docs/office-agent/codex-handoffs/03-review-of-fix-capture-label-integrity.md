# Chat review of branch `fix/capture-label-integrity` (`be16977`, 43 files, +1415/−421) — 2026-09-06

**Base:** `21c1e0d` (m50) — NOT Phase B (`m49-phase-b` / `9b27b05`). **Verdict: DO NOT MERGE AS A UNIT. Adopt as the m51 source material, re-based onto master after Phase B merges, with the rulings in §3 applied and the full gate discipline run.** The branch's own validation (one 90-frame 640×360 smoke per class, 30-frame levers, 7 C++ + 12 Python unit tests) is far below this repo's gate bar (both tick orders, both fixtures, G-EDGE ×8, MASK-TIE, P-C7 v3, byte-identity anchors, throughput gate). It was checked out on the bench when reviewed — the checkout must return to `master` before 077R-02.

## 1. What it gets right (KEEP — these are the m51 Tier-1 fixes, done)

| review finding | branch change | note |
|---|---|---|
| F1 disabled-mask drain | `Snap.bAwaitingTargetMask = bTargetMaskEffective`; per-snapshot `MaskWaitTicks`; held frames no longer block the batch | correct fix; `TargetMaskHoldTicks` is now vestigial |
| F2 late bbox / identity | `FTargetGeometry` frozen at `SampleDeferredActiveState` (path, asset, component class, bounds, provenance, screen rect); label writer and event accumulation read the frozen copy, never the live actor | correct; the rect is re-projected once more when the real render view arrives (see F11) |
| F3 tag exhaustion | `RetireInactiveRecords` releases `EventClaimed` when the fire ends; retired records skipped by arm/verify/base-set; history kept | correct; test proves 200 sequential events |
| F4 view binding | both extensions filter on `CaptureWorld` + `CaptureViewport` + single-view family + no stereo; mask arms published per `Family.FrameNumber` in `BeginRenderViewFamily`, consumed by that frame's tonemap pass; color and mask carry `RenderFrame`; identity mismatch ⇒ mask unmeasured | correct design; stale arms (tick without a render) are dropped instead of mis-paired |
| F5 session leak | `Generation` on the mask extension, results from an old generation discarded; serials monotonic across runs (`TargetMaskOwnSerial`, `CensusIdSerial`); `BlockUntilGPUIdle` on final drain | correct |
| F6 event veto | the M26 veto is deleted; events are derived from committed per-frame evidence | correct direction — but see ruling R1 on the fallback |
| F7/F8 committed writes | writer returns commit status, `CommittedFrames`, mask PNG saved BEFORE the label row, label append result checked, per-session writer object, `FlushPending(-1)` joins before annotation, atomic JSON via tmp+rename, `capture_complete` / `requested_frames` / `written_frame_indices` | correct accounting — but see R6/R7 on the backpressure design |
| F9 shared validity | `ServiceTargetMask` now rejects `CustomDepthModeAtPass != 3`, bad stencil extent, unassigned reserved tags, and frames in `InvalidTargetMaskFrames` (owner violations, host-reserved collisions, lost leases) | correct |
| F10 global transitions | fires + globals + geometry all sampled in `SampleDeferredActiveState`; `ComputeFireActive` = `IsFireLabelledThisFrame` | correct; `SessionGlobalPositive/NegativeFrames` counters are no longer incremented anywhere (stale keys — fix) |
| F11 projection | actual `ProjectionMatrix`/`ViewProjectionMatrix` from the rendered `FSceneView` (and `GetProjectionData` at sample time); near-plane edge clipping in `ProjectBoxToNormalizedRect` | correct; test covers ortho + near-plane |
| F12 GC of originals | `TStrongObjectPtr<UMaterialInterface>` in both texture anomalies; no-op swap skipped | correct |
| F13 preview freeze | unknown-request guard + 2 s cancel | correct |
| F14 sync parity | sync path uses `IsLogicallyHidden`, includes session globals via `FireOverride` | correct |
| F15 ownership | WebSocket mutations refused with `capture_active`; console `IAI.Apply/Revert/RevertAll` no-op during capture | see R4 — the console half breaks the bench levers |
| F16 pixel formats | `IsSupportedPixelLayout` gate in decode, luma and encode | correct |
| extras | LOD popping no-op skip + `IsVisualConditionHeld`; `IsHideConditionHeld`; exposure baseline from injection-free frames only, batch sorted by session index; `FilterPlugin.ini`; auditor `verify_integrity.py` + unit tests | keep |

## 2. Why it cannot merge as-is

**2.1 Wrong base.** It edits the same regions Phase B rewrote (drain loop, mask extension, label writer, mask measure, subsystem finish). Phase B is gate-proven on 21 legs; this is not. Order is fixed: Phase B merges first (077R-02), then this work is re-based onto it.

**2.2 Bundled semantic changes that reverse locked, measured rulings — without a measurement:**
- **Unmeasured events lose their positive window.** `affected_frames` = observable subset ONLY; when nothing is measured the window is empty, `manifested:false`, and every row on a mask-blind host (Nanite target, mask off, sync path, `OutputHeight≠0`) reads `anomaly_present:false`. m49/m50 ruled the opposite ("unmeasured falls back to `injected_frames` with `observability_measured:false`; honest labels without mask evidence beat no labels") and Lyra `L_Convolution_Blockout` went 0 → 6 events on that ruling. Under this branch such a host ships an all-negative dataset.
- **`anomaly_present` changes meaning** (v2: an injection is present → v3: observable) one day after the v2 field reference and v1→v2 changelog shipped to the client. Schema churn is a trust cost we cannot spend twice.
- **Nanite: "any Nanite component ⇒ unmeasurable"** replaces m50's measured "all-Nanite ⇒ unmeasurable" (`NaniteOnly == Renderable` → `> 0`). Changes the Lyra LG-9 count with no leg run.
- **Exact targeting no longer matches class names** and refuses ambiguous names. The client's M2-era `inject` by class name silently stops matching. The fog-card name exception on Bates is on this path.
- **`AnomalyMaterialsIncomplete > 0` makes EVERY fire on the frame unmeasured**, not the fire whose swap material is pending. On Bates the first frames of a session routinely carry a pending material (m47); a co-active hide would lose its label for no reason.

**2.3 Engineering risks with no measurement behind them:**
- **Writer throughput.** One serialized worker (was one task per job on the pool) plus a **10-second sleep loop on the game thread** when 8 jobs / 256 MiB are queued. At 1080p native (the delivery config) PNG encode is tens of ms per frame; a stall on the game thread changes pacing, tick/render ratio, and therefore the frames that get captured. Validated only at 640×360.
- **`PendingSnapshots >= 8` ⇒ the frame is dropped and the session marked incomplete.** GPU readback latency plus mask hold ticks can exceed 8 on a hitching host; G-R7(ii) exists because Bates hitches. A drop cap this tight can make every Bates session `capture_complete:false`, which the branch's own auditor then fails.
- **Stale-arm rejection on decoupled hosts.** A tick that gets no render is now never captured (correct — it is the F1/F3 pairing-slip candidate), but the session index was already consumed, so the snapshot dies at finalize and leaves a hole ⇒ `capture_complete:false`. With the tick pin (`tickpin_applied`) ticks == renders and this never fires; without it the session honestly fails. Must be measured on the synth/decoupled-order legs, and the interplay written into the client README.
- **A mask-PNG encode failure drops the colour frame** (`Dropped.Increment(); return;`). A valid image should still commit with `mask_state:"unmeasured"`.
- `check(Pending == 0)` in `ResetCounters` is a crash in Development builds; `ensure` + log.
- ~~Held frames are copied (`MoveTemp` on a const ref) once per hold tick~~ — WITHDRAWN, see §5: the reference is mutable and the move is real.
- `Published` map hard-reset at 64 entries silently discards live batches.
- Console-command blocking (`IAI.Apply/Revert/RevertAll` no-op while capturing) breaks the bench's own lever legs (m45 omit-depth, forced collision, F2 lever all inject during capture).

## 3. Rulings (chat, 2026-09-06)

- **R1 Schema:** `label_schema` 3 = schema 2 **plus additive keys only**, no meaning changes. `anomaly_present` keeps the v2 meaning (injection present); `visible_positive` stays observable-only; `injection_present` may ship as an explicit alias. `affected_frames` keeps the v2 meaning (observable subset) **with the m49 fallback intact**: when an event has zero measured frames, `affected_frames` = `injected_frames` and `observability_measured:false`. New keys accepted: `event_id`, `injection_id`, `observability_basis`, `frame_observations`, `capture_complete`, `requested_frames`, `written_frame_indices`, `condition_requested`, `schema_version` per row, `start/end = -1` on empty windows. `observability_measured` = every injected frame measured (branch's stricter definition) is accepted, since the fallback now covers the partial case explicitly.
- **R2 Nanite:** keep m50's `NaniteOnly == Renderable`. "Any Nanite component" is a new claim; it gets its own leg on Lyra later, not a silent flip.
- **R3 Targeting:** keep name-OR-class exact matching; ADD full-path matching; on ambiguity log a warning and keep today's behaviour (first match). No client-visible refusal this milestone.
- **R4 Ownership:** WebSocket refusal with `capture_active` — KEEP. Console commands during capture — ALLOWED, but every one increments `external_mutations` in `run_summary` and is logged with the session index; the bench levers depend on this path. The auditor does not fail on it.
- **R5 Shader-incomplete:** unmeasured per FIRE (the fire whose swap material is incomplete), not per frame. If per-fire attribution is not available from `Snap.AnomalyMaterialsIncomplete`, add it; do not ship the per-frame blanket.
- **R6 Writer:** keep commit accounting, per-session writer, mask-before-row, atomic JSON, join-before-annotation. Change: bounded pool of N workers (N = 2 default, ini knob), queue cap by bytes; backpressure waits at most ONE frame period on the game thread, then the frame is a COUNTED drop (`writer_drops`) and the session is marked incomplete — never a 10 s stall. Mask encode failure ⇒ the frame still commits with `mask_state:"unmeasured"` and `masks_dropped` counted. `check` ⇒ `ensure`.
- **R7 Readback cap:** `PendingSnapshots` cap becomes an ini knob, default 32; a drop is counted (`readback_drops`) and marks the session incomplete. Gate: zero drops on every natural leg, both fixtures, 1080p native.
- **R8 Stale arms:** keep the rejection. Gate on the synth/decoupled legs: with the tick pin applied, `requested_frames == written` on every leg; without it, the hole is counted (`unrendered_ticks`) and `capture_complete:false` — and the client README says so in one sentence.
- **R9 Everything in §1 not named above:** keep as written.

## 4. What the m51 brief does with this branch (once the owner says go, after 077R-02)

1. Checkout `master` (post Phase B). Create `m51` from it. Re-apply `be16977` onto it (rebase or cherry-pick; conflicts in the drain loop, mask extension, label writer, mask measure and `FinishRun` are expected — Phase B's `DrawnCount`, `frames_drawn_unexpected` and `exposure_dip_scope` must survive).
2. Apply R1–R8 as separate commits, each named for its ruling.
3. Fix the stale counters (`SessionGlobalPositive/NegativeFrames`), the vestigial `TargetMaskHoldTicks`, the held-frame copies, the `Published` reset.
4. Gates, both tick orders, both fixtures, on ONE binary: the full existing set (ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE ×8, OBS-1..4, m50 G1/G2/G6, PB-1..PB-8, black-frame, A44, P-C7 v3 declared keys) PLUS one new can-fail leg per review finding: F1 (masks requested + `OutputHeight` 360 ⇒ full throughput, zero "NEVER RESOLVED"), F2 (moving target lever ⇒ frozen bbox == drawn bbox within tolerance; destroyed-target lever ⇒ row kept), F3 (>55 events in one session ⇒ all measured), F4/F8 (decoupled order ⇒ mask and colour `RenderFrame` equal on every row), F5 (stop/start with in-flight readbacks ⇒ zero cross-session results), F7/F8 (write-failure lever ⇒ `capture_complete:false`, auditor fails, no dangling `mask_file`), F9 (omit-depth lever ⇒ unmeasured, never `target_pixels:0`), F12 (`obj gc` during a texture anomaly ⇒ original material restored by pointer), throughput (1080p native, zero writer/readback drops), Lyra LG set + auditor `--integrity-only` PASS on every banked session that is schema-3.
5. Docs: client-readme §8 schema-3 additive reference + v2→v3 changelog (one line: nothing you read changes meaning), `capture-label-integrity.md` corrected to the rulings, PRE-DELIVERY-CHECKLIST gains the auditor box, gotchas, journal, predictions written BEFORE the first leg.

## 5. ADDENDUM 2026-09-06 (after the expert's reply) — rulings amended, one review error withdrawn

Provenance: `be16977` was produced by a **Codex session** run by the owner (smokes + commit under the owner's git identity). Smoke config: StackOBot only, natural order, tick pin NOT compiled, async SVE PNG 640×360 native, mask/census ON, GPU reduce, offscreen; iterative builds, no predictions, no second order, no Lyra, no A-side anchors; G-EDGE/MASK-TIE/P-C7 NOT run. The first bounded writer lost 19/90 frames at 640×360 before the 10 s wait was added.

- **Review error withdrawn:** "held frames copied via MoveTemp on a const ref" — wrong. `Frame` is `FAnomalyCapturedFrame& Frame = Batch[BatchIndex]` (mutable); the move is real. Struck from §2.3.
- **R1 amended (two conditions, not one):** the fallback (`affected_frames = injected_frames`) applies ONLY when an event has ZERO measured frames. A partially measured event (some measured, some unknown) keeps the observable subset — never the fallback. Fields: `observability_measured` keeps Phase B's meaning (`Measured > 0`, unchanged); NEW `observability_complete` = every injected frame measured. "Nothing you read changes meaning" then holds. Agreed with the expert that the fallback window is injection-labelled data, not verified visible ground truth — the client README says so in those words, and the M3 contract is where the meaning of `anomaly_present` is renegotiated with the client, not silently.
- **R2 unchanged;** mixed-component actors (Nanite body + conventional accessory ⇒ accessory-only counts presented as actor-level evidence) become a named Lyra leg — FUTURE, own gotcha.
- **R5 amended:** per-fire shader-pending attribution in capture AND the auditor (`verify_integrity.py` currently applies the frame-wide rule; it must key on the fire's own material state).
- **R6 amended:** a mask-write failure downgrades BOTH the committed row (`mask_state:"unmeasured"`, `mask_file:null`) AND the event's per-frame observation for that index (`target_pixels -1`, `observable null`) — the annotation must never disagree with the row. Parallel encoding keeps coordinated commit accounting (one commit record per session index, written after all of that index's artifacts).
- **R8 amended (the expert's defect):** stale-arm rejection currently removes the request from `PendingWanted` but leaves its `PendingSnapshots` entry until finalize, so enough unrendered ticks exhaust the readback cap and capture cannot recover when rendering resumes. Raising 8→32 only delays it. Required: a cancellation result that retires the snapshot and bookkeeping immediately and counts `unrendered_ticks`; can-fail leg = rendering resumes after more missed ticks than the cap, capture continues.
- **Phase B behaviour, not just keys:** the injection-free exposure baseline must not disable the target-excluded detector (`exposure_dip_scope` present but ineffective is a fail); gate PB-5 both legs stays in the m51 set.
- **Status by finding (expert's own table, accepted):** implemented candidates awaiting the gate bar — F1, F2, F3, F5, F9, F11, F12, F13, F14, F16; further work — F4 (cancellation + decoupled), F6 (schema/fallback), F7/F8 (writer + failure handling), F10 (stale counters), F15 (console ownership). None release-closed yet. Also explicit: shader attribution, mixed-Nanite scope, targeting compatibility, the silent `Published` reset.
- **Next tranche (accepted):** evidence levels / component attribution first, then control-server responses, shipping configuration, private renderer dependencies.
- **Split (agreed):** our Code re-bases and runs the gates; the expert reviews m51's exact commit, predictions and evidence bank read-only before it lands.

## 6. Open for the owner
- ~~Whether m51 precedes the Bates cook~~ — moot: Bates M2 shipped 2026-09-04 with expectations set; no delivery pending. m51 runs unhurried; the M3 re-issue (both games, one build, schema + verifier + auditor + changelog) comes after it.
- ~~Who ran the branch~~ — answered: a Codex session, run by the owner.
