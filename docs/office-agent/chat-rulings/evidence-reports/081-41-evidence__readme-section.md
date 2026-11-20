## 9. Change evidence — `change_evidence.jsonl` (m55)

### 9.1 What it is

For each anomaly event, the capture measures **how much the picture actually changed** inside the
target's silhouette on the first frames of the event, and measures the same thing over the **rest of
the picture** as a control. The numbers go into one extra file per session,
`change_evidence.jsonl`, and a block of `change_*` keys in `run_summary.json`.

- **There is no verdict in this version.** Nothing in the file says "visible", "present" or "absent".
  It reports measurements; **you decide** what counts as a visible change for your purpose.
- **Nothing else changes.** `observable`, `affected_frames`, the labels, the masks and the frames
  follow exactly the same rules with this file on or off.
- **A measurement that could not be made is never reported as zero.** Such a row names its reason
  (one of 14, listed below) and its numbers are `-1` or `null`.
- It is **on by default**. `IAI.Capture.ChangeEvidence 0` in the console, before starting a capture,
  turns it off for the following captures (no file, no `change_*` keys).
- A frame can be measured only with the default capture settings for this: the default image grab
  point, PNG frames, output height `0` (native size) and target masks on. Other settings produce rows
  refused `unsupported_delivery`; frames and labels are unaffected.

### 9.2 Reading a row

The file has two kinds of line. **`"kind": "pair"`** lines are one per measured window frame: the
**first four labelled frames of every phase** of every event (a `blinking` event has one phase per
hidden run). **`"kind": "event"`** lines summarise each event when it ends. Join a pair line to its
`labels.jsonl` row and its image by **`session_index`** (never `frame_index`).

For a pair line at frame N, each pixel's change is `d` = the largest of the red, green and blue byte
differences between frame N and frame N−1. The **target region** is where frame N's target mask equals
the line's `mask_value`; the **control region** is where the mask is `0` (every target excluded).

| Field | Meaning |
|---|---|
| `session_index`, `frame_file` | frame N and its image |
| `event`, `phase_ordinal`, `window_index` | the event (as in the labels), its phase, and the window 0–3 (0 = the phase's first labelled frame) |
| `mask_value` | the event's tag in frame N's target mask |
| `chg_measured` | `true` if the numbers below were measured (`chg_eligible` and `pair_valid` are equal to it in this version) |
| `reason` | `null` when measured, otherwise one of the 14 reasons below |
| `chg_n` | pixels in the target region |
| `chg_gt8` | target pixels with `d > 8` (the fixed threshold `tau_px`; not a calibrated visibility threshold) |
| `chg_sum`, `chg_mean` | sum of `d`, and `sum / n / 255` to four decimals |
| `chg_hist` | counts of `d` in eight bins: `0`, `1–2`, `3–4`, `5–8`, `9–16`, `17–32`, `33–64`, `65–255` |
| `ctl_n`, `ctl_gt8`, `ctl_sum`, `ctl_mean`, `ctl_hist` | the same over the control region (rest of the picture) |
| `ref_session_index`, `ref_gt8`, `ref_mean` | frame N compared with the frame **just before the phase began** (the pre-onset reference), over frame N's target region; `-1`/`null` when that frame is not available |
| `chg_gt8_max_sofar`, `chg_mean_max_sofar` | the largest values so far in this phase's window |
| `prev_target_pixels` | the previous window frame's own count of this tag; always `-1` on window 0, so it does **not** show whether the target was visible before the event |
| `cam_dpos_cm`, `cam_drot_deg`, `cam_dfov_deg`, `cam_moved` | how far the camera moved between N−1 and N: whole centimetres, and **tenths** of a degree for rotation and field of view; `cam_moved` is true if any is non-zero. A caveat only — the pair is measured either way. |
| `receipt`, `prev_receipt`, `mask_receipt`, latency fields | capture bookkeeping; you can ignore them |

**The control is there to be compared with the target, not subtracted from it.** A quiet control
with a large target change is the clean case. A busy control (the camera moved, the lighting changed)
means part of the target's change may not be the anomaly's own. The control does not include light or
shadow that the target throws onto its surroundings.

An **event line** carries `anomaly_type`, `target`, `phase_count` and, per phase (first eight):
`state` (`measured` if the phase's first-frame pair was measured, otherwise `indeterminate`),
`pairs_required` (up to 4), `pairs_measured`, the refusal counts in `reasons`, and the largest
`chg_gt8`, `chg_mean` and `ref_gt8` seen (`-1` when none). An event that never had a labelled frame
reads `reason: "no_labelled_frames"`.

**The 14 reasons a pair can be refused:**

| Reason | In plain words |
|---|---|
| `first_frame` | the first frame of the capture has nothing before it |
| `predecessor_missing` | the previous frame was not available (not captured, late, or not kept because of the memory cap) |
| `predecessor_undelivered` | the previous frame's image was never written |
| `out_of_order_timeout` | this frame's inputs arrived too late |
| `epoch_reset` | the game's view changed owner (e.g. a level change), so the two frames are not comparable |
| `view_mismatch` | the image or mask came from a different rendered frame than the one captured |
| `extent_mismatch` | the two frames (or the image and its mask) differ in size or format |
| `mask_payload_missing` | no target-mask pixels arrived for this frame |
| `unsupported_delivery` | the capture settings or the way the frame was rendered cannot be measured |
| `budget_exceeded` | keeping this frame for measurement would exceed the memory cap (9.4) |
| `empty_region` | the target is not in this frame's mask (for example fully hidden behind something), or nothing else is |
| `no_labelled_frames` | event lines only: the event never had a labelled frame |
| `current_undelivered` | this frame's own image or mask failed to be written; `stage` says which |
| `closure_timeout` | at the end of the event or the capture, this frame's inputs never arrived |

`run_summary.json`'s `change_reason_*` keys count these over **every** captured frame, not just the
window frames, so a large count there (typically `mask_payload_missing` on frames with no mask) does
not mean a measurement you needed was lost. Measured yield is `pairs_measured / pairs_required` from
the event lines.

### 9.3 A change that arrives late or gradually

Adjacent frames only show a change that happens **between** them. If an anomaly's visible change
arrives a few frames after its label begins, the first window pairs read quiet and the change appears
in the later pair where it happens — and in the `ref_*` comparison, which compares each window frame
with the frame before the phase began. In testing, an anomaly held back for three frames changed under
0.1 % of its target pixels on windows 0–2 and all of them on window 3, in both the adjacent pair and
`ref_*`. Because `ref_*` compares across several frames, it also collects the scene's ordinary small
changes (in that test, a few hundred of 66,837 target pixels by window 2), so compare it with the
control and with the anomaly's expected size rather than with zero. A change that
builds up **gradually**, in steps each smaller than the threshold, can read quiet in **every** adjacent
pair while `ref_gt8` grows with the accumulated change (shown on a synthetic test; not produced by any
current anomaly). **So read `ref_*` (and the event line's `ref_gt8_max`) alongside the adjacent-pair
numbers.** Only the first four labelled frames of each phase are measured; a change that arrives after
the fourth is not in the file.

### 9.4 Memory

The memory cap, `IAI.Capture.ChangeMaxBytes` (default **256 MiB**), covers **only the images and masks
this measurement keeps**, not the game's total memory. It is a hard cap: a frame that would exceed it
is not kept and its pair is refused `budget_exceeded` — **a refusal, never a wrong number**. Full yield
is not guaranteed at any resolution: a long event whose frames complete slowly keeps later frames
until it can close, so long events at high resolution can reach the cap.

- **Planning estimate** (not a guaranteed maximum): `(8 + 3) × image + 9 × mask`, where an image is
  width × height × 4 bytes and a mask width × height bytes — about 49 MB at 1280×720, 110 MB at
  1920×1080, 195 MB at 2560×1440 (within the cap, **not tested**), and 440 MB at 3840×2160 (**above
  the cap: expect `budget_exceeded`** unless you raise `IAI.Capture.ChangeMaxBytes` before capturing).
- **Measured** at the default paced 30 fps: about **20 MB** at 1280×720 and about **68 MB** at
  1920×1080.

### 9.5 Cost — use paced capture

At the default paced capture (`IAI.Capture.Pace 1`, 30 fps), with the run log off, the measurement has
**no measurable cost on the game thread** at 1280×720 or 1920×1080 (under +0.12 ms per frame at the
upper bound), image writing keeps up exactly as without it, no frame is dropped and **every** required
pair is measured. The work runs on a background thread.

With pacing **off** (`IAI.Capture.Pace 0`) at 1920×1080 the image writer falls behind; kept frames pile
up to the memory cap and pairs are refused `budget_exceeded`. Under heavy unpaced load the target mask
can also arrive from a different rendered frame than the image, and those pairs are refused
`view_mismatch` or `unsupported_delivery` rather than measured wrongly. **Keep pacing on when you want
change evidence.**

### 9.6 Checking the numbers yourself

```
python host-tools\verify_capture.py --change-oracle <sessionFolder>
python host-tools\verify_capture.py --change-oracle <sessionFolder> --quiet --oracle-json detail.json
python host-tools\verify_capture.py --change-oracle --selftest
```

It recomputes every measured pair from the PNGs in `Actual_Frames/` and `target_mask/`, without using
the numbers in the file, and compares. Exit code **0** — everything compared matched (if nothing was
compared, the output says coverage 0); **1** — at least one comparison did not match; **3** — it could
not run (no `change_evidence.jsonl`, an unreadable file, or the Python `Pillow` package missing).
**It validates arithmetic and transport only**: that the numbers in the file are the numbers in the
delivered images. It uses each row's own frame numbers, so it does not show that the right frames were
paired, that a change is visible, or what caused it.

### 9.7 `positive_frames` in `run_summary.json`

`positive_frames` counts every written frame during which **at least one anomaly was active**. That
includes frames where an anomaly has been started but its effect is not yet in place — for example the
blurry-texture anomaly while it waits for the blur to take hold, whose rows carry `target_pixels: -1`
and `observable: null`. **It is not a count of labelled frames**; use `injected_frames` and
`affected_frames` in `annotation.json` for that.

### 9.8 Two test entries in the anomaly list

`IAI.ListAnomalies` on a Development build also lists **`null_effect`** and **`solid_swap`**. They are
internal test fixtures: they refuse to apply outside the test setup and never enter the capture pool,
the dashboard's selection or any default. Ignore them.

### 9.9 Known limits (not specific to change evidence)

- **An occasional unmeasured frame.** When one frame's mask render slips, the next frame's mask
  request is served by that render and comes out unmeasured (`mask_state: unmeasured`,
  `target_pixels: -1`, `observable: null`); its change-evidence pair is refused
  `mask_payload_missing`. The label is honest — an unmeasured frame is not counted as observable — so
  this costs coverage, not correctness.
- **The shader-readiness marker can over-flag frames in packaged builds.** In one packaged capture
  every frame carried `render_state: "shaders_pending"` (`frames_shaders_pending` equal to the frame
  count, `shader_prewarm_incomplete: 1`) although no shader work was pending and the active anomaly
  used no affected material. The marker is run-wide, not per anomaly. **Do not discard frames on that
  marker alone** — check the pictures.
- **`coverage_pct` is sampled at processing time**, from the live scene when the event's first frame
  is processed, not from that frame itself. It can differ slightly between two runs of the same capture.
- **Leave the run log off for production captures** (`IAI.Capture.RunLog 0`, or the default in
  delivery mode). With it on, the game thread was measured spending about 1 ms more CPU per frame
  (with pacing off); why is not yet established.
