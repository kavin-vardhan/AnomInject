# m55 change evidence — field reference

m55 adds one optional sidecar file, `change_evidence.jsonl`, and a block of `change_*` keys in
`run_summary.json`. This page lists every field the writer emits, as the source emits it (stage 2 of
the record format, `stage_version: 2`).

## What it is — measurements only, no verdict in v1

- **Per frame and per target, it measures how much the delivered picture changed** inside the anomaly
  target's silhouette, and the same statistics over the rest of the picture (the whole-frame
  **control**).
- **There is no verdict in v1.** Nothing in the file says whether an anomaly is visible, present or
  absent. The numbers are measurements; the reader decides what they mean.
- **`observable` is unchanged.** No m55 number feeds `labels.jsonl`, `annotation.json`, target
  selection, the capture pool, observability or the removal of unmeasured events. Frames, labels and
  annotation follow the same rules with evidence on or off; with it on, the only additions are the
  sidecar and the `change_*` summary keys.
- **A refusal is never a zero.** Where a measurement is not possible the row says why, with one of 14
  closed reasons, and the statistics it could not measure — the target (`chg_*`) and the pre-onset
  reference comparison (`ref_gt8`, `ref_mean`) — are `-1` or `null`. Other fields keep real values
  (see "The closed refusal vocabulary").

## Where it is written, and when

- **File:** `<session>/change_evidence.jsonl`, UTF-8 without BOM, one JSON object per line. It is
  written in both delivery modes when the run closes (at run end or world teardown; an epoch reset
  mid-run also writes it). A capture cancelled before it began writes nothing.
- **Switch:** `IAI.Capture.ChangeEvidence` (default `1`), read at run start. `0` produces neither the
  sidecar nor any `change_*` summary key.
- **Measurable captures:** a frame can be measured only on the default grab point, with PNG frames, at
  native output size (`IAI.Capture.OutputHeight 0`) and with the target mask on. Any other capture is
  refused `unsupported_delivery`; its images and labels are written exactly as without m55.
- **Row order:** pair rows in ascending `session_index`; an event row is written when all its phases
  have closed, after the pair rows it depends on. Join a pair row to its `labels.jsonl` row and frame by
  `session_index` (never by `frame_index`).
- **Rows exist only for measurement windows.** A pair row is written for each of the first four
  labelled frames of every phase of every event (`window_index` 0–3). Frames outside those windows
  are still checked for identity and counted in the summary, but have no row.

## The measurement

For a window frame N, `d = max(|ΔR|, |ΔG|, |ΔB|)` of the delivered 8-bit bytes at each pixel (alpha
ignored). The **target region** is every pixel whose value in frame N's delivered target mask equals
the row's `mask_value`; the **control region** is every pixel whose mask value is `0` (outside every
target silhouette in that frame). Three comparisons use those regions:

| Comparison | Pixels compared | Fields |
|---|---|---|
| **Adjacent pair** | frame N against frame N−1, target region | `chg_n`, `chg_gt8`, `chg_sum`, `chg_hist`, `chg_mean` |
| **Control** | frame N against frame N−1, control region | `ctl_n`, `ctl_gt8`, `ctl_sum`, `ctl_hist`, `ctl_mean` |
| **Pre-onset reference** | frame N against the frame just before the phase's first labelled frame, target region of frame N | `ref_session_index`, `ref_gt8`, `ref_mean` |

- `*_n` pixels in the region; `*_gt8` pixels with `d > 8` (strictly greater); `*_sum` the sum of `d`;
  `*_mean` = `sum / n / 255`, rounded half-up to four decimals (`floor(x·10⁴ + 0.5) / 10⁴` — a
  half-to-even reader disagrees on exact midpoints).
- **Histogram** `*_hist`: eight counts of `d` in the bins `[0]`, `[1–2]`, `[3–4]`, `[5–8]`,
  `[9–16]`, `[17–32]`, `[33–64]`, `[65–255]`. The bins sum to `*_n`; the last four sum to `*_gt8`.
- **The threshold 8** (`tau_px`) is a fixed byte threshold, not a calibrated visibility threshold.
- **The control** measures the rest of the picture on the same frame pair. It excludes the tagged
  target pixels but can include light, shadow or reflection spill from a target onto its
  surroundings, so part of the anomaly's own effect can appear in it. A quiet control
  does not prove cause; a busy one (camera motion, a lighting change) warns that the target's change
  may not be the anomaly's alone. It is never subtracted from the target numbers.
- **The reference** is the frame just before the phase's first labelled frame, kept for the phase's
  four windows when it was delivered with the same rectangle, size and format. On the onset window (0)
  the reference frame *is* frame N−1, so `ref_*` equals `chg_*` there. On windows 1–3 it shows the change accumulated
  since just before onset — which is where a change that arrives gradually, or a few frames late,
  appears when each adjacent pair is small. It is screen-space: it is not motion-compensated, so under
  camera or object motion it measures motion as well, and across several frames it also accumulates the
  scene's ordinary small changes. Measured on the three-frame-delay test (`solid_swap delay=3`, static
  camera, 66,837 target pixels, both tick orders): the adjacent pairs of windows 0–2 changed at most
  63 target pixels (under 0.1 %); window 3 changed all 66,837 in the adjacent pair and in `ref_*`; and
  `ref_*` on windows 1–2 had already accumulated 156–819 of them (up to about 1.2 %) of ordinary scene
  change.
- **Denominators.** A pair is measured only when both regions are non-empty. An empty target region
  (the target is not in the frame's mask, e.g. fully occluded) is refused `empty_region`; its control
  numbers are still written when the control region is non-empty.

## Camera delta (`cam_delta`)

The camera delta between frame N and frame N−1 is written as three quantised integers, plus a flag.
There is no single field named `cam_delta`.

| Field | Type | Unit |
|---|---|---|
| `cam_dpos_cm` | integer | camera-position distance, rounded to whole centimetres |
| `cam_drot_deg` | integer | quaternion angular distance in **tenths** of a degree (despite the name) |
| `cam_dfov_deg` | integer | absolute horizontal field-of-view difference in **tenths** of a degree |
| `cam_moved` | boolean | any of the three is non-zero |

All four are **caveats, never validity conditions**: a pair is measured whether or not the camera
moved, and a camera cut to the same size and field of view is not detected. With no predecessor all
three are `0`.

## Pair row (`"kind": "pair"`) — every field

| Field | Type | When present | Meaning |
|---|---|---|---|
| `kind` | string | always | `"pair"` |
| `stage_version` | integer | always | `2` |
| `session_id` | string | always | the session folder's name |
| `session_index` | integer | always | frame N; joins to `labels.jsonl` `session_index` and `Actual_Frames/frame_NNNNN.png` |
| `prev_session_index` | integer | always | the frame actually retained as N's predecessor, `-1` if none |
| `expected_prev_session_index` | integer | always | `session_index − 1`; a measured pair always has `prev_session_index` equal to it |
| `frame_file` | string | always | frame N's image, relative to the session folder |
| `event` | string | always | the event key, `<anomaly id>@<start frame>` as in the labels |
| `phase_ordinal` | integer | always | zero-based phase within the event (a blinking event has one phase per hidden run) |
| `window_index` | integer | always | 0–3; 0 is the phase's onset (its first labelled frame) |
| `mask_value` | integer | always | the event's tag in frame N's target mask |
| `chg_measured` | boolean | always | the adjacent pair was measured |
| `chg_eligible` | boolean | always | equal to `chg_measured` in v1 |
| `pair_valid` | boolean | always | equal to `chg_measured` on window rows; **not** an anomaly verdict |
| `reason` | string or null | always | `null` when measured, otherwise one of the 14 refusal reasons below |
| `stage` | string | only on a delivery refusal | where frame N's delivery failed or was unsupported: `colour`, `mask` or `writer` |
| `tau_px` | integer | always | `8` |
| `chg_n`, `chg_gt8`, `chg_sum` | integer | always | target statistics; `-1` when not measured |
| `chg_hist` | array of 8 integers or null | always | target histogram; `null` when not measured |
| `chg_mean` | number or null | always | target mean; `null` when not measured |
| `ctl_n`, `ctl_gt8`, `ctl_sum`, `ctl_hist`, `ctl_mean` | as `chg_*` | always | control statistics; written when identity permits arithmetic and the control region is non-empty, else `-1`/`null` |
| `ref_session_index` | integer | always | the reference frame's index, `-1` when no usable reference exists |
| `ref_gt8` | integer or null | always | reference comparison; `null` when unavailable or the pair is not measured |
| `ref_mean` | number or null | always | as `ref_gt8` |
| `prev_target_pixels` | integer | always | the previous window frame's own count of this tag; always `-1` on window 0, so visibility before onset is not established |
| `chg_gt8_max_sofar` | integer | always | largest `chg_gt8` measured so far in this phase's window; `-1` before any |
| `chg_mean_max_sofar` | number | always | largest `chg_mean` so far; `-1` before any |
| `cam_dpos_cm`, `cam_drot_deg`, `cam_dfov_deg`, `cam_moved` | see above | always | camera delta |
| `colour_completion_latency_ms` | integer | always | from frame N's capture to its image completion, milliseconds; `-1` if it had not completed when the row was written |
| `colour_completion_latency_frames` | integer | always | the same in engine frames |
| `receipt` | object | always | frame N's capture receipt (below) |
| `prev_receipt` | object | when a predecessor exists | the predecessor's receipt |
| `mask_receipt` | object | when a mask result arrived | frame N's mask receipt |
| `budget_census` | array of objects | only when frame N's colour or mask was refused `budget_exceeded` | the memory census at each refusal (internal diagnostic) |

**Receipt object.** Always: `run_epoch`, `cut_counter`, `capture_token` (identity of the capture)
and `t_submit_ms` (capture time, integer monotonic milliseconds). When the frame was read back:
`t_drain_ms` (readback completion, same clock), `view_family_id` and `serving_token` (the rendered
frame that served it), `format` (engine pixel-format number), `rect` `[x, y, width, height]`,
`extent` `[width, height]`, `family_frame` and `view_index`. When nothing was read back:
`t_drain_ms` `-1`, `view_family_id` and `serving_token` `0`, `format` `0`, `rect` and `extent` empty
arrays, and no `family_frame` or `view_index`. Mask receipts add `mask_request_id`,
`payload_owner_request_id` and `served_request_ids` as **decimal strings** (they exceed JSON's
exact-integer range).

## Event row (`"kind": "event"`) — every field

| Field | Type | Meaning |
|---|---|---|
| `kind`, `stage_version` | string, integer | `"event"`, `2` |
| `event`, `anomaly_type`, `target` | string | the event key, the anomaly id and the target, as in the labels |
| `finalized_by` | string | what closed the event: `event_end`, `fire_removed_or_target_lost`, `run_end`, `teardown` or `epoch_reset` |
| `phase_count` | integer | the true number of phases |
| `reason` | string or null | `no_labelled_frames` when the event never had a labelled frame (phase count 0), otherwise `null` |
| `phases` | array | at most the first 8 phases |

Each phase object:

| Field | Type | Meaning |
|---|---|---|
| `ordinal` | integer | zero-based phase number |
| `state` | string | `measured` if the onset pair (window 0) was measured, otherwise `indeterminate` — a later measured window does not change it |
| `first_labelled_index`, `onset_prev_index` | integer | the phase's first labelled frame, and that index − 1 (the onset predecessor and the reference frame) |
| `last_labelled_index`, `closure_watermark` | integer | the phase's last labelled frame (both fields carry it) |
| `finalized_by` | string | what closed the phase: `phase_end` or one of the event causes above |
| `pairs_required` | integer | `min(4, labelled frames in the phase)` |
| `pairs_measured` | integer | window pairs actually measured |
| `reason` | string or null | the onset pair's refusal reason, `null` when it was measured |
| `reasons` | object | count of each refusal reason over the phase's window rows |
| `chg_gt8_max`, `chg_mean_max`, `ref_gt8_max` | integer / number / integer | maxima over the measured windows; `-1` when none |

**Yield** is `pairs_measured / pairs_required`, summed over phases.

## The closed refusal vocabulary (14 reasons)

A refused row keeps its identity, file, camera and receipt fields. Its target statistics (`chg_n`,
`chg_gt8`, `chg_sum` `-1`; `chg_hist`, `chg_mean` `null`) and its reference comparison (`ref_gt8`,
`ref_mean` `null`) are unavailable. It can still carry real values in: the control statistics
(`ctl_*`), written on an `empty_region` refusal whose control region is non-empty (for example
`ctl_n` 921,600, `ctl_gt8` 4,579 on an onset refused `empty_region`); `ref_session_index`, which names
the reference frame when one existed; `prev_target_pixels`; the running phase maxima
`chg_gt8_max_sofar` / `chg_mean_max_sofar` from earlier windows; and the completion latency.

| Reason | Meaning |
|---|---|
| `first_frame` | frame N is the run's first capture; there is no predecessor. |
| `predecessor_missing` | no frame N−1 was retained: it was never captured, arrived out of order and timed out, or was delivered but not retained because of the memory cap. |
| `predecessor_undelivered` | frame N−1's image was never delivered (its readback, encode or write failed, it timed out at closure before arriving, or its capture was unsupported). |
| `out_of_order_timeout` | frame N's inputs did not arrive before four later frames had completed. |
| `epoch_reset` | the capture's owner changed (viewport, scene or level) between N−1 and N, so the two frames belong to different epochs; also the first frame after such a reset. |
| `view_mismatch` | frame N's image or mask (or frame N−1's image) was served by a different rendered frame than the one it was captured for. |
| `extent_mismatch` | N and N−1 (or N's image and mask) differ in rectangle, size or pixel format. |
| `mask_payload_missing` | no target-mask pixels arrived for frame N (no mask was requested for it, or its mask shared another frame's render). |
| `unsupported_delivery` | the capture configuration cannot be measured (see "Measurable captures"), or the frame was rendered in a way m55 does not support. |
| `budget_exceeded` | retaining frame N's image or mask would exceed the memory cap, so it was not retained. |
| `empty_region` | the target region (or the control region) is empty in frame N's mask. |
| `no_labelled_frames` | event rows only: the event never had a labelled frame. |
| `current_undelivered` | frame N's own image or mask failed to be read back, encoded or written; `stage` says which. |
| `closure_timeout` | at the end of a phase or of the run, frame N's inputs had not arrived within five seconds or eight later completions. |

Exact assignment for the predecessor and family cases (the order the code tests them):

| Situation of N−1 or of the mask | Reason on pair N |
|---|---|
| no N−1 retained: absent, never issued, a different index, or refused `out_of_order_timeout` | `predecessor_missing` |
| N−1 delivered, but its pixels were not retained (refused `budget_exceeded`) | `predecessor_missing` |
| N−1's image was never accepted as delivered (failure, `closure_timeout` before arrival, or `unsupported_delivery`) | `predecessor_undelivered` |
| the mask arrived from another rendered frame (serving token, family or view differ from the image's) | `view_mismatch` |
| no mask receipt, or a mask receipt without pixels | `mask_payload_missing` |

## `run_summary.json` additions — every key

Present only when evidence was on for the run.

| Key | Meaning |
|---|---|
| `change_evidence_file` | `"change_evidence.jsonl"` |
| `change_stage_version`, `change_tau_px`, `change_window_k` | `2`, `8`, `4` |
| `change_max_bytes`, `change_max_bytes_source` | the memory cap in bytes for this run, and where it was set (`compiled`, `console`, `command_line`, …) |
| `change_enabled_source` | where `IAI.Capture.ChangeEvidence` was set |
| `change_bytes_high_water`, `change_bytes_retained` | peak retained bytes, and bytes still retained at the summary (normally 0) |
| `change_bytes_peak_census` | the retained-memory breakdown at the peak (object), or `null` |
| `change_worker_ms_total` | total measurement-worker time, milliseconds (not on the game thread) |
| `change_closure_watermark` | the last capture index at run closure |
| `change_colour_completion_latency_samples` | completions in the latency distribution |
| `change_colour_completion_latency_{ms,frames}_{p50,p95,max}` | nearest-rank percentiles of capture-to-completion latency; `-1` with no samples |
| `change_reason_<reason>` | one key for each of the 14 reasons: how many captures ended with it. **These count every captured frame, not only window rows**, so a large count (for example `mask_payload_missing` on frames without a mask) does not mean a required window was lost — read yield from the event rows. |
| `change_issued`, `change_pairs_identity_valid`, `change_pairs_refused` | captures registered, and how many passed or failed the identity checks |
| `change_pairs_measured`, `change_measurement_pairs_refused` | window rows measured and refused |
| `change_phases_measured`, `change_phases_indeterminate`, `change_events_with_phases` | phase and event outcomes |
| `change_denominator_mismatch`, `change_histogram_mismatch`, `change_ref_onset_mismatch` | internal consistency checks; each should read 0 |
| `change_late_results`, `change_late_event_observations`, `change_late_record_mutations` | results that arrived after their row was final (counted, never applied) |
| `change_rows_dropped` | rows beyond the 100,000-row cap |
| `change_persist_failed`, `change_teardown_flush` | sidecar write failure; the sidecar was written at world teardown |
| `change_epoch_resets`, `change_epoch_rejected`, `change_epoch_rejected_<path>` | owner changes, and results rejected as belonging to an earlier epoch (by path: `colour_drain`, `colour_writer`, `colour_family`, `colour`, `mask_drain`, `mask_admission`, `mask_writer`, `mask_family`, `mask`) |
| `change_view_rejected`, `change_multi_view_families`, `change_unsupported_completion` | rendered frames m55 would not attach to, multi-view frames seen, completions of unsupported captures |
| `change_duplicate_callback`, `change_duplicate_completion` | repeated callbacks and completions (ignored) |
| `change_mask_capture_served_ge2`, `change_mask_pass_deferred`, `change_colour_multi_ready_drain`, `change_capture_arm_dropped`, `change_unissued_indices_skipped` | transport diagnostics |
| `change_pending_colour_cancelled`, `change_pending_mask_cancelled`, `change_pending_family_deferred` | pending work cancelled or deferred at an epoch reset |
| `change_bench_gate`, `change_bench_gate_source`, `change_bench_gate_refused`, `change_throwaway_family_constructed`, `change_bench_unregistered_capture` | test-fixture devices; `0` / `compiled` / absent in a normal run (`refused_no_fixture_flag` if a test device was requested outside the test fixture — the run then uses none) |

Keys created only when they first fire (`change_late_event_observations`, `change_unissued_indices_skipped`,
`change_epoch_rejected_<path>`, `change_pending_family_deferred`, `change_bench_gate_refused`,
`change_bench_unregistered_capture`) are absent when zero. `change_teardown_flush` is always present
(`0`, or `1` when the sidecar was written at world teardown), and `change_pending_colour_cancelled` /
`change_pending_mask_cancelled` are present in every run that issued a capture (normally `0`).

## Memory envelope

`IAI.Capture.ChangeMaxBytes` (compiled default 268,435,456 = **256 MiB**, read at run start) caps
**m55's retained payload only** — the images and masks it holds for measurement. It is not a limit on
the process's total memory: the image writer, receipts, rows and metadata are outside it. The cap is a
hard limit: anything that would exceed it is refused `budget_exceeded` and not retained, so a refusal
never produces a wrong number.

There is **no structural guarantee of full yield at any resolution**: a long labelled phase whose
first completion is delayed keeps every later frame's buffers until it can close, so long phases at
high resolution can reach the cap and refuse.

**Sizing estimate (not a bound).** The planning estimate is `(8 + 3) × image + 9 × mask`: the eight
later completions a closing phase can wait for, plus the predecessor, the phase reference and the
image being measured, and nine masks. An image is `width × height × 4` bytes and a mask
`width × height` bytes. That gives ≈ 49 MB at 1280×720, ≈ 110 MB at 1920×1080, ≈ 195 MB at
2560×1440 (inside the cap, not exercised) and ≈ 440 MB at 3840×2160 (above the default cap: expect
`budget_exceeded` unless the cap is raised). Longer phases can hold more; the cap still refuses.

**Measured high-water — observations for one scene and load, not bounds:** on the static test scene
and machine of the cost measurement below (paced, 600-frame captures) 19.4–20.3 MB at 1280×720 and
56.0–68.5 MB at 1920×1080; a paced capture on a second test game reached 134.8 MB; with pacing off at
1920×1080 the image writer falls behind, retained images accumulate and the cap is reached (267.6 MB
measured, with 134 `budget_exceeded` refusals). Retention depends on completion order, writer delay and
phase/reference density, so only the cap bounds it.

## Cost

**A dated result on one test scene and one machine (2026-09-26), not a general guarantee.** Setup: the
same build with evidence off (A) and on (B); a static test scene; the test anomaly `solid_swap`;
`IAI.Capture.Config 2 4 16 4 0`; 600-frame captures at 1280×720 and 1920×1080, four per side in ABBA
order; the run log off; paced capture (`IAI.Capture.Pace 1`) configured at 30 fps, which on that
machine armed about 25.07 captures per second; statistics over the steady window, capture indices
60–599.

- **Statistic:** the one-sided 95 % upper bound on the **mean game-thread CPU-cycle-equivalent
  difference per engine frame** (B − A): **−0.0718 ms** at 1280×720 and **+0.1100 ms** at 1920×1080.
  Cycle-equivalent means the game thread's CPU cycles converted to milliseconds with that machine's
  calibration; time the thread spent **blocked or waiting is excluded**, and a steady-window mean does
  not capture rare or closure-only latency. The bound treats the four captures per side as the samples.
- **Workload:** each evidence-on capture measured **120 of 120** required pairs, with **0** dropped
  frames on every capture. A denser workload measures more pairs per second.
- **Writer:** no detected loss of throughput at this load. This is not a measurement of maximum writer
  capacity.
- **Worker:** the measurement runs on a pool worker, about 2.5 ms per captured frame at 1280×720 and
  7.3 ms at 1920×1080.

There is no structural guarantee of full yield, and no memory bound other than the 256 MiB admission
cap (see "Memory envelope"). With pacing off at 1920×1080 the image writer saturates and m55 refuses pairs
`budget_exceeded`; under heavy unpaced load the target mask can arrive from a different rendered frame
than the image, and m55 refuses those pairs `view_mismatch` or `unsupported_delivery` rather than
measuring a mismatched pair. **Use paced capture with m55.**

## Checking the numbers — `verify_capture.py --change-oracle`

```
python tools/verify_capture.py --change-oracle <sessionFolder> [--quiet] [--oracle-json <file>]
python tools/verify_capture.py --change-oracle --selftest
```

(In a delivered bundle the tool is `host-tools\verify_capture.py`.) The full oracle description is in
the internal "Oracle" section further down this page.

It recomputes every measured row (and every reference comparison) from the delivered PNGs and the
target-mask PNGs, independently of the numbers in the sidecar, and compares. It also checks that a
measured row does not contradict itself (`reason` null, `chg_eligible` and `pair_valid` true,
`expected_prev_session_index` = `session_index` − 1, `chg_hist`/`chg_sum`/`ctl_hist`/`ctl_sum`
present), and cross-reads each `empty_region` refusal against its mask PNG: both regions non-empty is
a **disagreement**; no mask PNG (none is written for an all-zero mask) or an unreadable one is
**unverifiable**, never a disagreement.

Exit **1**: a row or reference comparison did not match, a measured row contradicts itself, or an
`empty_region` refusal disagrees with its mask (a `tau_px` outside 0–255 counts as a mismatch too).
Otherwise **3**: something was **uninterpretable** — a line that is not UTF-8 JSON, a line that is not a
JSON object, a measured row whose pair ids, `mask_value` or `tau_px` are not integers, whose `receipt`
is not an object or whose `receipt.rect` is not an array, or a `chg_measured` that is not a boolean
(each listed with its line number) — or it could not run (no
sidecar, unreadable sidecar, Pillow missing). Otherwise **0**: every line was interpretable and every
comparison matched (including when nothing was compared — the output then shows coverage 0). A proven
mismatch outranks incompleteness; the summary always prints both counts. The tool never exits through
a traceback.

**It validates arithmetic and transport only** — that the numbers in the sidecar are the numbers the
delivered images contain. It does not check renderer pairing (it uses each row's own frame indices,
so consistent wrong ids with matching numbers would pass), phase-reference selection (a reference index
changed to another identical image passes), or the full record schema, and it does not establish that
a change is visible or what caused it.

---

# Internal development notes (not client-facing)

Everything below is the development record of the fields above: design history, test devices,
bounds and the oracle's full description. It keeps internal identifiers. **Where it differs from the
reference above, the reference above is authoritative** (it was written against the source at the
end of Stage 3, 081-41).
## Stage 2 notes (history, superseded where the reference above differs)

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

## Legacy comparisons: the timing-sampled set (ruling 081-22)

A legacy-identity comparison (build under test vs the pre-m55 binary, evidence ON or OFF, same recipe) treats these values as
**timing-sampled**: they may differ between binaries without failing the comparison, because the capture code samples them at drain or
tick time rather than from the captured frame's own state.

| Path | Why it is timing-sampled |
|---|---|
| `annotation.json` `anomalies[].coverage_pct` | Computed by `EvaluateSelectionProvenance` when the drain processes the event's anchor frame, against the **live** world and view at that moment, not the arm-time view the label rows carry. The pre-m55 binary already differs from itself run to run in native order (081-22 (d): events 3 and 5), and build 3's event-2 value (8.6518 vs 8.6927 pre-m55, ≤ 0.47 % relative) equals the synthetic-order value on both binaries. |
| `run_summary.json` `end_frame`, `capture_game_ticks`, `key_ring_published`, `key_ring_consumed`, `key_ring_wrapped`, `ticks_per_captured_frame` | Tick and key-ring counters that vary within one binary across same-recipe runs (081-12 bank: `end_frame` 121/122). |

Everything else in the comparison stays exact: mask bytes (MainWorld) or occupancy and own-tag bytes (CB), label rows, camera rows, onsets,
key structure, event ranges and MASK-TIE tuples. With evidence ON the only allowed additions are the `run_summary` `change_*` block and the
`change_evidence.jsonl` sidecar; with evidence OFF there must be none. **Queued (m51 pairing / M3, not m55):** compute `coverage_pct` from
the frame's own mask instead of the live view — exact and frame-consistent, but a legacy-output change that belongs outside m55.

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
| `colours_pending` / `colours_previous` / `colours_phase_ref` / `colours_in_hand` / `colours_unowned` | Split with priority pending > previous > phase reference > the serial worker's current item; a shared allocation is counted once. `_unowned` should be 0. |
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

**081-43 update (Codex 081-42 F1, F3 and the scope note; ruling 081-43; the reference section above is
authoritative).** A contradicted `empty_region` refusal is a mismatch (exit 1, summary
`empty_region_disagrees`); a missing or all-zero mask PNG stays unverifiable. Input it cannot
interpret — a non-UTF-8 or malformed line, a non-object line, a measured row whose pair ids,
`mask_value` or `tau_px` are not integers, whose `receipt` is not an object or whose `receipt.rect` is
not an array, a non-boolean `chg_measured`, or a row the recomputation cannot process — is exit 3 with
line diagnostics (a `tau_px` outside 0..255 is a mismatch); the
precedence is 1 > 3 > 0 and the summary prints both counts; the CLI never exits through a traceback. A
measured row carrying a refusal reason, `chg_eligible`/`pair_valid` not true,
`expected_prev_session_index` ≠ `session_index` − 1, or no `chg_hist`/`chg_sum`/`ctl_hist`/`ctl_sum`
is a mismatch (summary `contradicted`). The selftest now proves 35 cases and the unit file 19 tests.
Replayed old vs new over every banked m55 session folder (476): every exit code unchanged (0 → 0).

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
