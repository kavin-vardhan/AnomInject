# m55 change evidence — Stage 2 field reference

Stage 2 adds measurement windows and immutable final events to the existing identity/lifetime
stage. It does not infer anomaly presence or change m49 `observable`, labels, annotation or vetoes.

## Current Stage 2 records (stage_version 2)

Pair rows now cover only the first K=4 labelled frames of each event phase. They retain the receipt,
file, index and camera fields below; Stage 1's all-capture diagnostic pair rows are superseded.
Pair order is ascending capture index. Event records are emitted when their closure watermark resolves.
The entire sidecar persists on finish/epoch reset and best-effort teardown in both delivery modes.

| Field | Meaning |
|---|---|
| `event`, `phase_ordinal`, `window_index` | Legacy Id@StartFrame key, zero-based phase and window index. Window zero is onset. |
| `mask_value` | Current delivered-mask tag, joined to the same legacy row/event. |
| `chg_measured`, `chg_eligible`, `pair_valid` | Equal in v1; identity and both nonempty denominators permit arithmetic. No verdict. |
| `chg_n`, `chg_gt8`, `chg_sum`, `chg_hist`, `chg_mean` | Current-tag region count, strict >8 max-RGB byte changes, sum, eight bins, sum/count/255 rounded **half-up** to four decimals (`floor(x·10⁴ + 0.5) / 10⁴`; a half-to-even consumer disagrees on exact midpoints). Unmeasured: -1 counts, null histogram/mean. |
| `ctl_*` | Same statistics over zero pixels of the entire filtered mask (all-tag complement). Raw control is retained when identity permits arithmetic and its denominator is nonzero, even if the target is empty. Otherwise -1/null. |
| `ref_session_index`, `ref_gt8`, `ref_mean` | Retained f0-1 canonical reference index (-1 if unavailable); current-mask target comparison with it. Statistics null when unavailable. Geometry and generation must match. |
| `chg_gt8_max_sofar`, `chg_mean_max_sofar` | Running maxima in this phase's window; -1 before any measured pair. |
| `prev_target_pixels` | Previous labelled row's own tag count, or -1; always -1 on onset, so pre-onset visibility is not established. |
| `tau_px` | Fixed strict byte threshold 8, not a calibrated visibility threshold. |

Bins are [0], [1–2], [3–4], [5–8], [9–16], [17–32], [33–64], [65–255]. Both denominators must
be positive. Alpha is ignored. Controls exclude tag pixels, not lighting/shadow spill beyond them.
Reference comparisons are screen-space, not motion compensated. A low control is not causal proof.

Event rows contain `event`, `anomaly_type`, `target`, nullable `reason` (`no_labelled_frames` when
phase_count=0), true `phase_count`, and at most eight `phases`. Each phase contains ordinal, onset
indices, last_labelled_index/closure_watermark, pairs_required=min(4,labelled length), pairs_measured,
refusal histogram, onset coverage state and reason, and final chg_gt8_max/chg_mean_max/ref_gt8_max.
An onset refusal stays indeterminate regardless of later measurements. `finalized_by` describes a
lifecycle cause, not an additional refusal enum value. Event records carry **no** `late_results`
field (removed in build 3, 081-21: it could only ever be 0). Deliveries rejected after finalisation
are counted in the run summary (`change_late_results`) and never edit a final event.

The worker releases the admission mutex during pixel scans. Phase references extend the unique
canonical buffer reservation; sharing the predecessor does not double-charge bytes or buffer count.
The 256 MiB payload cap is the sole admission cap (081-15 removed the colour-count cap; 081-17 build 2 raised the default from 64 MiB). Writer total memory remains outside it.
Stage 1 all-capture identity/refusal counters remain transport diagnostics. Stage 2 adds pairs_measured,
measurement_pairs_refused, phases_measured/indeterminate, events_with_phases, denominator_mismatch,
histogram_mismatch and ref_onset_mismatch under the `change_` summary prefix, plus tau_px=8/window_k=4.
No arithmetic feeds labels, annotation, selector, auto pool, observability or the m26 veto.

### Bench-only surfaces

`null_effect` / `solid_swap` share one lifecycle class. Development catalogue is 10 shipping + 2 bench
entries; Shipping registers neither. Apply requires ChangeEvidenceCases=1, -IAIBenchFixture, and
CB_GateLevel/L_ShooterGym. Neither id enters any auto/default/selector pool. Delay counts captured
labelled frames; the positive uses the shipped Lit pink and the existing exact material revert path.
`AnomalyBench` is allowed on non-Shipping Game/Editor targets, excluded from Client/Server and
Shipping. Typed authenticated `bench_input_lock` (`enabled` bool) and `bench_place_view` (`request`
number) invoke IAI.Bench.InputLock 1/0 and one-shot IAI.Bench.PlaceView. Both require -IAIBenchFixture,
CB_GateLevel/L_ShooterGym and inactive capture. An explicit bench request loads the linked module
even when the existing cooked descriptor predates it; no recook is needed. No generic exec bridge.
Input lock increments both controller ignore-input counters once; repeated enable does not stack.
It decrements only its own increments at run end, world cleanup, module shutdown or explicit off
outside capture. A replaced controller releases with a diagnostic rather than silently rearming.
Place after settle: CB camera origin(-1500,0,260), rotation(0,0,0); Lyra reference is unchanged.
Placement compensates measured camera-to-view-owner offset (pawn/spectator view target first, otherwise GetPawnOrSpectator) without replacing the host camera/movement.
CB placement requires the input lock. An executed receipt is not success: require lock/placement
logs and a fresh independent B1 (CB) or L1 (Lyra), plus Lyra's placement-to-capture interval audit.
Input lock suppresses player look/move input; it does not pin the camera or waive foreground.
Measurement legs require foreground throughout and record PID/sample counts. The 081-10 no-capture
preflight explicitly permits absent foreground while checking the pose, per Chat's ruling.
Status081-11: controller-only lock and resolved spectator placement qualify5/5 on final A699E9EE.
Commands emit IAI-BENCH READY identities before decisions and specific refusal reasons.
Explicit unlock restores entry flags; ordinary run-end release exercised. Teardown cleanup logs
controller=None after destruction, clearing ownership without claiming a surviving-controller decrement.
Typed bench_scene_fixture mode=occluder/motion is CB-only, locked/placed, outside capture:
occluder duplicates the loaded target Cube and logs Visibility traces; motion arms capture-only
-3deg/sec yaw. Occlusion qualified both orders; motion remains UNRUN. Non-Shipping named-map
capture_start benchDelayFrames=3 feeds only solid_swap delay=3; both orders qualified.
NoHold recipe failed before creating any fire (target Cube has0 candidate textures); therefore
no_labelled_frames runtime qualification remains UNEXERCISED. Stage2/Stage3 incomplete.
`IAI.Bench.ChangeTeardownAt` is default-off and fixture/command-line gated; it requests actual world
travel on StackOBot at a capture index, exercising subsystem teardown rather than simulating closure.

`IAI.Capture.ChangeEvidence` (default 1) and `IAI.Capture.ChangeMaxBytes` (default 268435456, 256 MiB, since 081-17; was 67108864)
are snapshotted at run start. Value 0 on the former produces neither the sidecar nor summary additions.
Effective values and engine set-by provenance are echoed, not inferred from configuration files.

## Historical Stage 1 identity record shape; receipt fields remain shared

`change_evidence.jsonl` contains one `kind: "pair"`, `stage_version: 1` diagnostic for each
issued capture, ordered by session index. This temporary Stage 1 coverage includes unlabelled frames
so first-frame, skip, lifetime and predecessor gates can be audited. It is not the Stage 2 per-tag
measurement window schema. Missing inputs are refused, never interpreted as zero change.

| Field | Meaning |
|---|---|
| `session_id`, `session_index` | Run directory's session id and the capture index; join to that run's labels row `session_index`. |
| `prev_session_index` | Actual retained predecessor index, or -1 when absent. |
| `expected_prev_session_index` | Current index minus one. A valid pair must match this; no bridging. |
| `frame_file` | Run-relative colour path (supported delivery is native PNG). |
| `pair_valid` | Identity/lifetime checks passed; **not** a pixel measurement or anomaly verdict. |
| `reason` | Null on valid identity; otherwise one of the closed 14 reasons below. |
| `stage` | Present for a delivery failure: `colour`, `mask`, or `writer`. |
| `receipt`, `prev_receipt` | Current issue/readback and retained predecessor receipt. Missing predecessor omits `prev_receipt`. |
| `mask_receipt` | Receipt shared by the reduce counts and the payload result, when available. |
| `cam_dpos_cm` | Rounded camera-position distance, integer centimetres. |
| `cam_drot_deg` | Quaternion angular distance, integer **tenths** of a degree. |
| `cam_dfov_deg` | Absolute horizontal-FOV difference, integer **tenths** of a degree. |
| `cam_moved` | Any of those three quantised differences is nonzero. Caveat only, never a validity condition. |

Each receipt block contains `run_epoch`, `cut_counter`, `capture_token`, `view_family_id`,
`serving_token`, `format` (UE pixel-format integer), `rect` `[x,y,width,height]`, `extent`
`[width,height]`, `family_frame`, `view_index`, `t_submit_ms` and `t_drain_ms`.
The times are integer monotonic engine milliseconds: submit is **GT issue time**, drain is
**RT readback completion**, not writer completion. They expose capture/readback gaps without
inventing a hitch classifier. A failed current delivery may only have issue identity: drain -1,
family 0, unknown format and empty geometry arrays explicitly indicate no published readback.

`family_frame` is diagnostic only; the engine can assign it to more than one family. A unique
`view_family_id` travels in UE's `ISceneViewFamilyExtentionData` through the renderer's family copy.
The sole owner is the capture subsystem's viewport/world; the supported family has exactly one view.
The issue-time token joins colour and mask; their request serials are deliberately independent.
Mask receipts additionally expose `mask_request_id`, `payload_owner_request_id`, and
`served_request_ids`. Those full uint64 serials are **decimal strings**, avoiding JSON number precision
loss from the mask request's high-bit namespace. Coalesced non-owners have no payload.

## Closed refusal vocabulary

`first_frame`, `predecessor_missing`, `predecessor_undelivered`, `out_of_order_timeout`,
`epoch_reset`, `view_mismatch`, `extent_mismatch`, `mask_payload_missing`,
`unsupported_delivery`, `budget_exceeded`, `empty_region`, `no_labelled_frames`,
`current_undelivered`, `closure_timeout`.

`current_undelivered` describes this frame's failed readback/encode/write and includes `stage`.
`no_labelled_frames` is reserved for Stage 2 event finalisation; Stage 1 emits no events.
Wrong-family and duplicate callbacks are counters, **not** new refusal strings.

**Exact predecessor and family mapping (as the code decides it; 081-20 F5, documented in build 3):**

| Situation of N−1 or of the mask | Reason on pair N |
|---|---|
| no N−1 retained: absent, never issued to the stage, a different index, or refused `out_of_order_timeout` | `predecessor_missing` |
| N−1 delivered, but its pixels were not retained (its admission was refused `budget_exceeded`) | `predecessor_missing` |
| N−1's colour was never accepted as delivered: its readback/encode/write failed, it ended `closure_timeout` before its colour arrived, or it was refused `unsupported_delivery` | `predecessor_undelivered` |
| the mask arrived but from another family (serving token or family id differs from the colour receipt's) | `view_mismatch` (plan §2.3 said `mask_payload_missing`; the payload exists, it is the wrong family's) |
| no mask receipt, or a coalesced non-owner without pixels | `mask_payload_missing` |

All are honest refusals; only the names needed stating.

## Summary additions

`change_evidence_file`, `change_stage_version`, `change_max_bytes`, `change_bytes_high_water`,
`change_bytes_retained`, `change_bytes_peak_census` (object or null, 081-17), `change_worker_ms_total`, `change_closure_watermark`, `change_bench_gate`,
`change_enabled_source`, `change_max_bytes_source`, `change_bench_gate_source`;
`change_reason_<reason>` for all 14 reasons; and these `change_`-prefixed integer counters:

`issued`, `pairs_identity_valid`, `pairs_refused`, `rows_dropped`, `late_results`, `epoch_rejected`,
`epoch_resets`, `view_rejected`, `duplicate_callback`, `duplicate_completion`,
`mask_capture_served_ge2`, `mask_pass_deferred`, `colour_multi_ready_drain`, `capture_arm_dropped`,
`throwaway_family_constructed`, `teardown_flush`, `persist_failed`, `late_record_mutations`,
`unissued_indices_skipped`, `bench_unregistered_capture`, `pending_colour_cancelled`,
`pending_mask_cancelled`, `pending_family_deferred`, and (build 3, 081-21) `multi_view_families`
and `unsupported_completion`.
Old-generation rejection also adds `change_epoch_rejected_<path>` for the path actually reached
(build 3 adds the paths `colour_family` and `mask_family`).

- `change_multi_view_families` — every eligible view family with more than one view seen while a
  stage exists (the ruling's `multi_view_families`; all stage counters carry the `change_` prefix, and
  with evidence off there is no stage and no key). Such a family keeps the pre-m55 legacy behaviour;
  m55 records its frames `unsupported_delivery`. **The pre-m55 multi-view unsoundness — one readback
  per view under one RequestId, so the delivered picture can be view 1 while the labels describe
  view 0 — is not fixed in m55**; it is queued for the m51 pairing work.
- `change_unsupported_completion` — drain admissions and writer/mask completions of a frame issued
  unsupported (JPEG, resampled, backbuffer, mask not effective). Counted here instead of
  `change_duplicate_completion` (item still pending) or `change_late_results` (item already final), so
  those two count true duplicates and true late results only (081-20 F7).
- `change_bench_gate_refused` (present only when it fired) and `change_bench_gate_source =
  refused_no_fixture_flag` — a non-zero `IAI.Bench.ChangeGate` without `-IAIBenchFixture` (081-20 F9).
Final event detail will live only in the sidecar at Stage 2, not be duplicated in the summary.

## Ownership and bounds

`Frame.RawBytes` still moves into the ordinary writer job. The writer's existing single
`ConvertTightToBGRA` conversion is moved into a shared immutable array **before encoding**; the
encoder reads that array. The stage takes its own reference only after a successful reservation.
Write success is a separate completion record; receipts never acquire a mutable delivered flag.

`Result.MaskPixels` moves into local `Gray`; existing non-event-tag filtering and mask/count ties run
on that local first. Only after filtering and exposure-exclusion folding is it moved into a shared
immutable array, used by both mask writer and stage. No second writer copy on that path.

The stage accounts allocated buffer bytes, not just used elements. Admission takes no buffer
reference on failure and never waits on a writer. The 256 MiB payload cap is the sole
admission cap, including current, predecessor, phase-reference colours and retained masks.
Shared references to the same colour allocation are charged once. **It does not bound the writer pool's total memory;
that remains the separate m51 limitation.** Metadata/JSON rows have their separate 100,000-row cap.

Outside phase/run closure, an incomplete head times out after four distinct later colour
completion notifications, including failed deliveries. Issue count never advances this clock.
Indices never issued to this asynchronous stage (a synchronous fallback) are counted and skipped;
the next pair still requires its actual predecessor to be N−1 and refuses a gap.
Run closure records the last issued index,
resolves through it on the serial worker, and freezes by five seconds or eight distinct colour
completions beyond that watermark, whichever comes first. A closing phase uses the same bound
with its last labelled index; its closure bound takes precedence over the ordering-gap bound. Run closure admits no further captures, so the wall-time arm is the active
terminal bound there. Persistence precedes reset; teardown also attempts persistence and is counted.
Late results can only increment diagnostics, not modify final rows. These clock/budget rules
are the dated 081-15 correction, superseding the old issued-index / 2-second / 3-colour limits.

**Bounded game-thread wait at closure (081-20 F4, documented, not changed).** `CloseAndPersist`
blocks the game thread until closure resolves, bounded by the 5-second wall. Two production paths
reach it: (a) an owner change mid-run (`ResetEpoch` from `Issue`), during which nothing
game-thread-driven — mask service, writer enqueue — can complete, so the pending items end
`closure_timeout`; (b) run end or teardown after a frame was dropped on the render thread (extent
clamp, NO KEY, empty rect, a key clobbered by another family), which never notifies the stage, so
closure waits the full 5 s. Bounded, no hang; a fix would touch the identity path for little gain.

**Row cap envelope (081-20 F10).** Sidecar rows are held in memory until persistence, capped at
100,000 rows (later rows are dropped and counted in `change_rows_dropped`). Rows are window pairs
(at most K=4 per phase per event) plus event records, so a single-event recipe writes far fewer than
one row per frame; a run that did write one row per captured frame would reach the cap after
100,000 / 30 ≈ 55 minutes at 30 fps, and co-labelled events reach it sooner. At the cap the row text
is ≈ 0.25 GB (UTF-16, banked median ≈ 1,173 characters per row), plus a same-size join and a UTF-8
copy made transiently on the game thread at persistence. The byte budget does not cover rows.
**FUTURE (v1.1):** stream rows to disk as they finalise.

**Scan ceiling (081-20 F11).** The pixel scan runs on one serial pool worker: ≈ 45 ms per scanned
pair at 1080p and ≈ 13.5 ms at 720p (upper bounds from banked `change_worker_ms_total`), i.e. about
22 scans per second at 1080p. Windows are bounded (K=4 per phase; co-labelled events share one scan),
so ordinary recipes stay below it. During run or phase closure the worker holds one pool thread in a
1 ms sleep loop for up to 5 s, in the same pool the PNG writer uses. **FUTURE (v2):** a GPU/SIMD scan;
revisit only if the 081-18 long-run readings implicate the worker.

**Deferred event end across an epoch reset (081-20 F13).** `ResetEpoch` does not clear a deferred
event end (`DeferredEndCause` / `DeferredEndAt`). It is unreachable with today's tick order — labels
are observed at the end of the capture tick, before any later `Issue` — so there is no code change:
an untestable edit in the identity path would be worse than this note.

## Legacy delivery never depends on m55 (081-21, build 3)

The prime invariant of a default-ON evidence layer: **it may refuse evidence; it never costs legacy
output.** Build 3 enforces it at every place m55 used to gate legacy work (081-20 F1/F2/F3):

- **Colour family (`BeginRenderViewFamily`).** Every eligible family — the pre-m55 filter still
  excludes scene and reflection captures — consumes the next colour arm and publishes its key exactly
  as before m55. m55 attaches its family data only when the consumed arm's issue is owned by that
  family: one view, the issue's render target and scene, and not a second family of the same frame and
  epoch. Otherwise the consumed issue is recorded `unsupported_delivery` with `stage: colour` and the
  family carries no m55 data. A dead or closed stage no longer returns before the consume.
- **Colour submission (`AfterPass`).** The legacy readback is submitted whenever the key is wanted.
  m55 only decides whether a receipt rides along; missing, mismatched or unclaimed family data means no
  receipt, never a dropped legacy frame, whatever the stage's open/closed or persistence state.
- **Masks (`AfterTonemap`).** Pending arms — m26, census, m49 and m55 alike — are served by any
  eligible view as before m55. m55 attaches its receipt only on an owned, claimed family; otherwise
  every served m55 arm is recorded `unsupported_delivery` with `stage: mask`, no receipt is built and
  the legacy mask PNG is written from the unfrozen buffer exactly as before.
- **Pending arms** are consumed by every eligible frame, so they cannot grow per frame.
- **Cancel before focus writes nothing (F1).** The stage knows whether its run began a session. A
  stage whose run wrote no session is discarded — never persisted, never creating a directory — in
  `FinishRun`'s cancel branch, at the next `StartRun` or `Deinitialize`, and as a backstop inside
  `CloseAndPersist`. One `Capture(m55): STAGE-DISCARDED` line names the run directory.

On a single-viewport host the attach path is unchanged from build 2 (every normal banked leg had
`change_view_rejected 0`). ⚠ **What legacy-exact also restores:** an eligible foreign family that
renders after the capture viewport in the same frame publishes its own key for that frame number, as
before m55, and the newest key wins at lookup, so that frame's legacy colour can be lost. Build 2 had
masked this pre-m55 behaviour by refusing foreign families outright; it belongs with the multi-view
unsoundness in the m51 pairing work.

## Bench devices (run-start setting `IAI.Bench.ChangeGate`)

| Value | Device |
|---|---|
| 0 | Healthy control. |
| 1 | Hold actual SI 8 readback until SI 9 is also ready; drain newest first in the same RT call. |
| 2 | Drop SI 8's actual colour arm, immediately remove its legacy snapshot, and leave an issued gap. |
| 3 | Fail SI 8's colour writer before encode/write. |
| 4 | Fail SI 8's real readback after readiness, before copying. |
| 5 | Fail SI 8's mask writer. |
| 6 | Defer SI 8's mask pass one family frame so two distinct capture-mask arms coalesce. |
| 7 | Hold SI 8 colour and mask results; close/persist/reset at issue 9, then let old work drain. |
| 8 | Construct a second `FSceneViewFamilyContext` with one index-0 view and the same scene frame but a foreign render target; call the real GT owner check. Also repeat the RT colour claim on the selected token. The throwaway family is not submitted for GPU rendering. |
| 9 | Perturb SI 8's recorded rect minimum by one pixel; synthetic geometry fault, no claim of a real letterbox change. |
| 10 | Force SI 8 colour admission to fail before retaining its shared ref. A separate byte-cap leg uses `ChangeMaxBytes 1`. |
| 11 | Hold actual SI 8 readback for 6.5 seconds (081-15: exceeds the 5-second wall bound). After closure/persistence, pump owned drains for four seconds and compare final sidecar bytes. Use a 9-frame capture so the held completion is beyond the watermark deadline. |
| 12 | Keep the actual SI 6 buffer instead of replacing it at SI 7, so SI 8 encounters a real index-2 predecessor. |
| 13 | Synthetic registration gap at SI 8: retain real legacy colour/mask delivery but omit this index from the change-stage queue. SI 9 must report predecessor_missing with actual predecessor 7; closure must complete. The actual synchronous-fallback device hit a D3D12 ensure and is not credited as natural-path coverage. |
| 14 | Healthy transport audit: log CRC32 of each frozen writer mask; compare with decoded delivered PNG bytes externally. |
| 15 | Keep SI 8 colour/mask arms unserved; force an epoch reset at SI 9; cancel only old pending arms and require capture recovery. |

These devices do not certify a gate merely by existing. Runtime readings, coverage and unexercised
cases are in the stage journal/report. No shader, asset, cook, pixel-difference arithmetic or event
verdict is part of Stage 1.

**Build 3 (081-21).** Every non-zero value is refused at run start unless the process was launched
with `-IAIBenchFixture`: the run uses gate 0, logs `Capture(m55): BENCH-GATE-REFUSED` with the
requested value, and reports `change_bench_gate_source = refused_no_fixture_flag`. A client typing a
bench command therefore cannot lose frames. Under the legacy-exact rule three devices read
differently: **gate 8**'s throwaway foreign family now consumes SI 8's colour arm and publishes its key
(as any eligible foreign family did before m55), so SI 8 is `unsupported_delivery` (`stage: colour`),
SI 9 `predecessor_undelivered`, and SI 8's legacy PNG is absent (the throwaway is never rendered —
a fixture-only fault); the RT duplicate-claim check no longer runs at SI 8. **Gate 2**'s SI 8 mask arm
is served in frame 8 (it used to coalesce into frame 9); SI 8 still ends `out_of_order_timeout`.
**Gate 15** keeps SI 8's colour and mask arms unserved through an explicit bench hold in the mask pass.

### Unserved-arm generation cleanup (Stage 1 correction)

On each new issue, remove only unserved colour/mask arms with a different immutable epoch/cut. Legacy snapshot and mask bookkeeping are removed too, with an unavailable mask outcome. Submitted GPU work is untouched and must still pass drain rejection. Summary counters: `change_pending_colour_cancelled`, `change_pending_mask_cancelled`, `change_pending_family_deferred`. Gate 15 leaves the real SI8 arms unserved, forces reset at SI9, and requires cleanup plus recovery. This is a synthetic epoch change, not a real viewport replacement.


## Completion latency diagnostics (081-15)

Pair rows add `colour_completion_latency_ms` and `colour_completion_latency_frames`:
GT issue to accepted colour-completion notification, in monotonic milliseconds and
engine frame-counter increments. `-1` means no accepted completion before the row
finalized; late arrivals never rewrite a row. Failed colour deliveries also count
as completions. This is not GPU-only readback latency or a validity condition.
Run summary adds `change_colour_completion_latency_samples` and
`change_colour_completion_latency_{ms,frames}_{p50,p95,max}`. Percentiles use
nearest rank over distinct accepted completions; no samples => -1. Report sample
coverage and late-result exclusions alongside the distribution. Epoch resets keep
run-wide statistics; pending completion membership remains generation scoped.

Bench gate16 loses only SI3's stage colour-completion notification; gate17 loses
all such notifications. Legacy writing continues unchanged. `CLOSURE-TIMEOUT`
logs distinguish the eight-later-completion path from the five-second wall path.
These devices require runtime proof; their existence is not a gate pass.

## Payload cap and ownership census (081-17, build 2)

**What the cap covers.** `IAI.Capture.ChangeMaxBytes` (compiled default 268,435,456 = 256 MiB) caps
**m55's admitted payload only**: the colour allocations the change stage reserves (a colour shared by
Previous and a phase reference is charged once) and the masks it retains. It is **not** a
process-RAM ceiling — the writer pool, receipts, rows, compression and metadata are outside it.
There is **no structural guarantee of full yield at any resolution**. The mechanism in one sentence:
a long labelled phase whose first colour completion is delayed keeps every later frame's buffers
until it can close, so long phases at high resolution can exhaust the cap and refuse. Refusals are
recorded as `budget_exceeded`; numbers are never wrong. 1440p and 4K are unexercised.

**Refusal census (every `budget_exceeded`).** A failed colour or mask reservation — including bench
gate 10's forced colour refusal — logs one `Capture(m55): BUDGET-EXCEEDED si=… kind=colour|mask
requested=… held=… max=… census={…}` line and appends the census to `budget_census`, an array on
the refused index's pair row(s). Only indices that own window rows have sidecar rows; for other
indices the log line is the record. The field is absent when no refusal happened.

**High-water census.** At every new high-water the census is recomputed after the admitted buffer's
ownership is recorded and kept as the peak. `Capture(m55): HIGH-WATER bytes=… census={…}` is logged
only when the peak has risen by at least one colour allocation (the first admitted colour's size)
since the last such line, plus one `Capture(m55): HIGH-WATER-PEAK` line at each closure. The run
summary carries it as `change_bytes_peak_census` (null if nothing was ever admitted). High-water is a
reading, never a gate; there is no closure-bound field and no formula-based warning.

| Census field | Meaning |
|---|---|
| `trigger`, `kind` | `budget_exceeded` or `high_water`; the reservation involved, `colour` or `mask`. |
| `si`, `requested_bytes` | Capture index of that reservation and its allocated size. |
| `max_bytes`, `bytes_held` | Cap and bytes reserved at the census (the refused buffer is not included). |
| `colours` | Distinct admitted colour allocations. |
| `colours_pending` / `_previous` / `_phase_ref` / `_in_hand` / `_unowned` | Split with priority pending > previous > phase reference > the serial worker's current item; a shared allocation is counted once. `_unowned` should be 0. |
| `colour_bytes`, `masks`, `mask_bytes` | Allocated bytes of those colours; retained masks and their bytes. |
| `unaccounted_bytes` | `bytes_held − colour_bytes − mask_bytes`; 0 when the accounting closes. |
| `cursor`, `latest_index` | Serial head index and latest issued index. |
| `open_phases` | Non-final phases: `event`, `ordinal`, `first`, `last`, `closing`. |
| `head` | `si` (= cursor), `state` `pending` / `in_hand` / `none`, and `waits_on` ⊆ colour, mask, observed, sealed. |
| `high_water` | Peak census only: `change_bytes_high_water` at that peak. |

**Measured high-water per exercised recipe (build 2, 081-17; readings, not limits).** StackOBot
`CB_GateLevel` 1920x1080 twins, 90 frames: 51.9 / 64.3 / 66.4 / 70.5 MB (null N / null S / solid N /
solid S), peak census 4–5 colours + 9–14 masks, zero refusals. Lyra `L_ShooterGym` 1920x1080 G270
auto-pool, 300 frames: 197.0 MB (73 % of the cap), peak census 22 colours (21 pending behind a head
whose inputs had all arrived) + 7 masks, zero refusals. Earlier priors (64 MiB era): 17.5 / 19.4 /
39.4 / 49.8 / 66.4 MB. 1440p and 4K are unexercised; a 1440p run shaped like the Lyra leg would need
more than the cap.

## Oracle — `tools/verify_capture.py --change-oracle` (Stage 3, 081-19)

An independent reader that recomputes the sidecar's numbers from the delivered images on disk, so a
reader can check that `change_evidence.jsonl` says what the PNGs contain. It is Python + Pillow only,
implements the definition above itself, and never reads a producer `chg_*`/`ctl_*`/`ref_*` value as
an input — only to compare after recomputing.

**How to run it**

```
python tools/verify_capture.py --change-oracle <sessionDir> [--quiet] [--oracle-json detail.json]
python tools/verify_capture.py --dir <sessionDir> --change-oracle
python tools/verify_capture.py --change-oracle --selftest
```

`<sessionDir>` is the folder holding `change_evidence.jsonl`; a bank leg folder with exactly one
session inside is resolved to it. `--quiet` prints only mismatched and unavailable rows plus the
summary. **Exit codes (081-21, per the 081-19 ruling):** **1** when any compared row or any reference
comparison mismatches; **0** when every compared comparison matches — including when nothing was
compared, which is printed as coverage 0 and which a gate that needs rows must treat as its own
failure, not the tool's; **3** when it cannot run (no sidecar, unreadable sidecar, no Pillow). It
changes no other mode's exit code and adds nothing to the 079 vocabulary.

**What it validates.** For every `"kind":"pair"` row with `chg_measured: true` it decodes
`Actual_Frames/frame_%05d.png` for `session_index` and `prev_session_index` and
`target_mask/frame_%05d.png` for `session_index`; target = `mask == mask_value`, control =
`mask == 0`; `d = max(|dR|,|dG|,|dB|)` in delivered bytes, counted when `d > tau_px`. It compares
`chg_n`, `chg_gt8`, `chg_sum`, `chg_hist` and the `ctl_*` twins exactly, `chg_mean`/`ctl_mean`
within **0.00005** of the exact `sum/n/255` (the producer rounds to four decimals), and checks
`tau_px == 8`, `prev_session_index == session_index − 1`, `frame_file` against the naming, and that
both denominators are positive. A row naming a reference (`ref_session_index` with `ref_gt8`/
`ref_mean`) is recomputed against that delivered frame over the current target region and counted
separately. `empty_region` refusals are cross-read against the mask PNG (agrees / disagrees /
unverifiable when no mask PNG exists — none is written for an all-zero mask). Coverage is printed:
rows claimed, compared, matched, mismatched, unavailable with reasons, and not-claimed rows by reason.

**What it does not validate.** Its output ends with the contract sentence, verbatim: *agreement
validates arithmetic and transport only — that the numbers in the sidecar are the numbers the
delivered images contain. It does not establish renderer pairing, visible effect, or cause.* It
does not check that the producer chose the right frames to pair (it uses the row's own ids): **a
producer that published consistent wrong pair ids together with the numbers of the frames those ids
name would match** — pair selection is the identity contract, tested by the Stage 1 identity gates
and the independent review, not by this oracle. Nor does it check that a
refusal other than `empty_region` was right, the event records, the `_sofar` running maxima,
`prev_target_pixels`, the camera caveat fields, or anything about the anomaly. JPEG, resampled
(PNG size ≠ receipt rect), backbuffer, missing or unreadable deliveries are **UNAVAILABLE** with the
reason, never guessed.

**Selftest.** `--change-oracle --selftest` builds 24×16 sessions whose sidecar numbers come from a
separate per-pixel loop and proves 24 cases (22 until 081-21, which added a reference-only mismatch
that must exit 1 and a nothing-compared session that must exit 0, and made the five must-fail
mutations also require exit 1): agreement; recomputation independent of the published
values; `G-GRAD(b)` (`0,3,6,9,12`: four adjacent `chg_gt8` of 0, reference `ref_gt8` 0,0,48,48 =
full count on the last pair); `G-TIES` (`d == 8` not counted, `d == 9` counted, `PF_A2B10G10R10`
10-bit `0 → 35` delivered as bytes `0 → 8`); `G-DENOM` (a measured claim on a whole-frame mask or an
absent tag is flagged; an `empty_region` refusal on a whole-frame mask agrees; the cross-read can
fire); JPEG / resampled / backbuffer / missing mask / non-grayscale mask are unavailable; and five
must-fail mutations each fire — a count off by one, a swapped predecessor, the wrong mask value, a
pair id shifted by one, a control that includes a target pixel. `tools/test_verify_capture_change_oracle.py`
pins the printed sentence, the exit codes and the six existing CLI cases' exit codes.
