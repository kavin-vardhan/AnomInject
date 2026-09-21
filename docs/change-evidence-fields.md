# m55 change evidence — Stage 1 field reference

Stage 1 is identity/lifetime instrumentation on the feature branch. It does not measure pixel
differences, infer anomaly presence, or change m49 `observable`, labels, annotation, or vetoes.
Stage 2 will add tag/phase windows and measurements; those fields and event records do not exist yet.

`IAI.Capture.ChangeEvidence` (default 1) and `IAI.Capture.ChangeMaxBytes` (default 67108864)
are snapshotted at run start. Value 0 on the former produces neither the sidecar nor summary additions.
Effective values and engine set-by provenance are echoed, not inferred from configuration files.

## Pair identity records

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
`predecessor_undelivered` describes a known failed predecessor; a skipped or timed-out predecessor
is missing. `no_labelled_frames` is reserved for Stage 2 event finalisation; Stage 1 emits no events.
Wrong-family and duplicate callbacks are counters, **not** new refusal strings.

## Summary additions

`change_evidence_file`, `change_stage_version`, `change_max_bytes`, `change_bytes_high_water`,
`change_bytes_retained`, `change_worker_ms_total`, `change_closure_watermark`, `change_bench_gate`,
`change_enabled_source`, `change_max_bytes_source`, `change_bench_gate_source`;
`change_reason_<reason>` for all 14 reasons; and these `change_`-prefixed integer counters:

`issued`, `pairs_identity_valid`, `pairs_refused`, `rows_dropped`, `late_results`, `epoch_rejected`,
`epoch_resets`, `view_rejected`, `duplicate_callback`, `duplicate_completion`,
`mask_capture_served_ge2`, `mask_pass_deferred`, `colour_multi_ready_drain`, `capture_arm_dropped`,
`throwaway_family_constructed`, `teardown_flush`, `persist_failed`, `late_record_mutations`,
`unissued_indices_skipped`, `bench_unregistered_capture`.
Old-generation rejection also adds `change_epoch_rejected_<path>` for the path actually reached.
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
reference on failure, never waits on a writer, and holds at most three colour buffers total (the
future phase reference shares that same limit). Stage 1 retains no phase reference. The 64 MiB
bound includes the masks retained by the stage. **It does not bound the writer pool's total memory;
that remains the separate m51 limitation.** Metadata/JSON rows have their separate 100,000-row cap.

An ordering gap starts when a later colour completion exposes an unresolved cursor; it waits
at most four more captured frames from that observation, not from initial capture issue.
Indices never issued to this asynchronous stage (a synchronous fallback) are counted and skipped;
the next pair still requires its actual predecessor to be N−1 and refuses a gap.
Run closure records the last issued index,
resolves through it on the serial worker, and freezes by two seconds or eight more captured frames,
whichever comes first. Run closure admits no further captures, so the wall-time arm is the active
terminal bound there. Persistence precedes reset; teardown also attempts persistence and is counted.
Late results can only increment diagnostics, not modify final rows. Phase/event closure is Stage 2.

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
| 11 | Hold actual SI 8 readback for 3.5 seconds. After closure/persistence, pump owned drains for four seconds and compare final sidecar bytes. Use a 9-frame capture so the held completion is beyond the watermark deadline. |
| 12 | Keep the actual SI 6 buffer instead of replacing it at SI 7, so SI 8 encounters a real index-2 predecessor. |
| 13 | Synthetic registration gap at SI 8: retain real legacy colour/mask delivery but omit this index from the change-stage queue. SI 9 must report predecessor_missing with actual predecessor 7; closure must complete. The actual synchronous-fallback device hit a D3D12 ensure and is not credited as natural-path coverage. |
| 14 | Healthy transport audit: log CRC32 of each frozen writer mask; compare with decoded delivered PNG bytes externally. |

These devices do not certify a gate merely by existing. Runtime readings, coverage and unexercised
cases are in the stage journal/report. No shader, asset, cook, pixel-difference arithmetic or event
verdict is part of Stage 1.

### Unserved-arm generation cleanup (Stage 1 correction)

On each new issue, remove only unserved colour/mask arms with a different immutable epoch/cut. Legacy snapshot and mask bookkeeping are removed too, with an unavailable mask outcome. Submitted GPU work is untouched and must still pass drain rejection. Summary counters: `change_pending_colour_cancelled`, `change_pending_mask_cancelled`, `change_pending_family_deferred`. Gate 15 leaves the real SI8 arms unserved, forces reset at SI9, and requires cleanup plus recovery. This is a synthetic epoch change, not a real viewport replacement.
