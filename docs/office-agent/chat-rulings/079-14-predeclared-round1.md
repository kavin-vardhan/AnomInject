# 079-14 — predeclared fixture expectations (written BEFORE any fixture was run)

Geometry: 160x120, target rect 40x30 = 1200 px, mask value 200, constant background,
so every clean pair is byte-identical (d == 0) and tau collapses to SIGNAL_FLOOR 0.0040
except where the fixture itself introduces motion. edge_window = 4 (default).

| # | Fixture | Pixels | Predeclared reading | Exit |
|---|---------|--------|---------------------|------|
| F01 | interior_only_outside_windows | span [10..25]; target OFF on frame 18 ONLY; windows [6..14] and [22..30] | both edges NO-TRANSITION; whole-span check sees pair 18 d=1.0 > tau ⇒ run **UNASSESSABLE (whole-span change at frame 18)**. NOT NO-TRACE | 0 |
| F02 | edge_window_only | change only at pair 30 (far edge of the end window) | end TRANSITION 30 delta=+4; onset NO-TRANSITION ⇒ **OFFSET-NOTE** | 0 |
| F03 | moving_target_exact_masks | target translates 2 px/frame with exact per-frame masks; colour change exactly on the labelled onset/end | onset TRANSITION 10 delta=+0, end TRANSITION 16 delta=+0 ⇒ **CONSISTENT** | 0 |
| F04 | extrapolated_mask_change | masks only in-span [10..25]; the ONLY change is pair 8, a pre-span pair needing extrapolated masks | onset TRANSITION 8 delta=-2 tagged `(mask extrapolated from frame 10 …)`; end NO-TRANSITION ⇒ **OFFSET-NOTE**. NOT NO-TRACE | 0 |
| F05 | true_no_trace | nothing ever changes; masks on every frame; class `blink` | **NO-TRACE** (positive control: the gate CAN fail) | 2 |
| F06 | no_trace_missing_interior_mask | F05 + no mask_file at 13 | **UNASSESSABLE(mask missing at frame 13)** | 0 |
| F07 | no_trace_wrong_mask_id | F05 + frame 13 mask carries only value 201 | **UNASSESSABLE(mask id 200 not present at frame 13)** | 0 |
| F08 | no_trace_corrupt_mask_png | F05 + frame 13 PNG truncated | **UNASSESSABLE(mask unreadable at frame 13)** | 0 |
| F09 | no_trace_empty_mask | F05 + frame 13 mask all-zero | **UNASSESSABLE(mask missing at frame 13 (empty))** | 0 |
| F10 | blink_chain_five_runs | five one-frame runs at 10,14,18,22,26, each a real toggle | 5 runs, 10 edges, all TRANSITION delta=+0 ⇒ **all CONSISTENT**, event CONSISTENT | 0 |
| F11 | two_targets_overlapping_windows | TargetA OFF [10..13], TargetB OFF [12..15]; separate (node,mode) groups | both events **CONSISTENT**; no cross-target peak stealing | 0 |
| F12 | span_past_last_frame | span [25..32], last captured frame 29 | end edge **UNASSESSABLE(edge truncated by the session boundary)**, onset TRANSITION 25 delta=+0 ⇒ run **PARTIAL**. NOT NO-TRACE | 0 |
| F13 | one_frame_next_to_two_frame | runs {10} and {12,13}, both real | 2 runs, 4 edges all delta=+0 ⇒ **CONSISTENT** both | 0 |
| F14 | occluder_midspan | anomaly OFF [10..20]; a bright occluder covers the target on frame 15 only (pairs 15 and 16, outside both windows [6..14] and [17..25]) | onset TRANSITION 10 delta=+0, end TRANSITION 21 delta=+0 ⇒ **CONSISTENT** | 0 |
| F15 | lighting_step_on_onset | whole picture brightens on frame 10, target also changes on 10 | onset TRANSITION 10 delta=+0 with ring note, end TRANSITION 16 delta=+0 ⇒ **CONSISTENT (caveat: edge coincides with a whole-region change)** | 0 |
| F16 | small_change_2pct | 24 of 1200 target px change (2%, above tau=0.0040) | onset TRANSITION 10 delta=+0, end TRANSITION 16 delta=+0 ⇒ **CONSISTENT** | 0 |
| F17 | target_leaves_and_returns | target absent (empty mask) on frames 13,14 inside span [10..17] | onset UNASSESSABLE(empty mask in window), end TRANSITION 18 delta=+0 ⇒ **PARTIAL**. NOT NO-TRACE | 0 |
| F18 | no_trace_unlisted_class | nothing changes, class `lighting_mismatch` | **UNASSESSABLE(NO-TRACE unavailable for class lighting_mismatch)** | 0 |
| F19 | bbox_only_never_changes | no masks at all; nothing ever changes | **READING (bbox-only)**, UNREAD-BBOX-ONLY. Never NO-TRACE | 0 |
| F20 | plateau_no_peak_interior | target changes on every frame 11..14 (a plateau: interiors are not one-sided peaks) | peaks {11,14}; onset TRANSITION 11 delta=+1, end NO-TRANSITION ⇒ **OFFSET-NOTE**. NOT NO-TRACE | 0 |
| F21 | fabricated_run_beside_a_real_run | ONE event, run A [10..15] fabricated, run B [24..24] real | run A **NO-TRACE**, run B CONSISTENT, event token **NO-TRACE** | 2 |
| F22 | fabricated_event_beside_a_real_event | event 0 fabricated, event 1 real | **NO-TRACE 1, CONSISTENT 1**, VERDICT FAIL | 2 |
| F23 | missing_rgb_in_span | nothing changes; RGB frame 13 deleted | **UNASSESSABLE (RGB missing for pair …)**. NOT NO-TRACE | 0 |
| F24 | wrong_rgb_dimensions_in_span | nothing changes; RGB frame 13 is 80x60 | **UNASSESSABLE (RGB dimensions / unreadable)**. NOT NO-TRACE | 0 |

Batch contract: `--all <fixture root>` ⇒ exit 2 (F05/F21/F22 are NO-TRACE); with
`--report-only` ⇒ exit 0; no identifier collisions.
