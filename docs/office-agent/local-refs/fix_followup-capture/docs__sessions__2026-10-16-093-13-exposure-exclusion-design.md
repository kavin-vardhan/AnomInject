# 093-13 exposure-exclusion repair design

Date: 2026-10-16. Authority: 093-12 ruling AA2-AA5 and request 093-13.
Worktree: E:/IA_BuildCache/_r933/AnomalyInjector, fix/followup-capture.
Baseline product: eff797cf8dae0c886556e742cae4fe98f59458e1; initial documentation head 7049614.
This note and the companion predictions are committed before product edits.

## State and all reset/allocation sites

The state is ExposureExclusionMask, ExposureExclusionFolded (the set of already-folded event identities), ExposureExclusionW, ExposureExclusionH and ExposureExclusionPx. Its declaration/default initialization is in AnomalyCaptureSubsystem.h:563-567 at the baseline. Default construction initializes dimensions/count to zero and creates empty containers.

| Baseline site | Existing behavior | Change |
|---|---|---|
| FoldExposureExclusion, cpp:1486-1493 | Dimension change clears mask/set/count, allocates W*H zeros, stores dimensions | A dimension/shape change first invokes the common reset. Separately, actual array Num() != W*H invokes reset and allocation, then records W/H. An empty same-size array therefore cannot skip allocation. |
| Run initialization, cpp:3855-3859 | Clears all five state members | Replace with the common reset helper; equivalent state. |
| RefreshCaptureOwner invalidation, cpp:4732-4733 | Clears only mask and folded-event set | Replace with the common reset, including both dimensions and pixel count. |

The only allocation is SetNumZeroed in FoldExposureExclusion. Its existing input guards require positive dimensions, a tag map, and at least W*H source elements. Since Gray.Num() is int32, the existing guard also bounds a valid W*H to the array index range. No new cap or eligibility rule is added.

ResetExposureExclusion() is a private member, declared beside FoldExposureExclusion. It resets both containers, W/H and Px together. The existing shape-change reset remains so a changed shape with the same total pixel count cannot reuse a differently laid-out union. Allocation itself tests the actual element count, independently of the cached dimensions. No new dependency, engine edit, private header, label field or bench lever is needed.

Product scope is exactly Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp and Source/AnomalyCapture/Public/AnomalyCaptureSubsystem.h. Tests/orchestration live in the job's scratch folder; existing permanent readers are unchanged.

## Call paths, consumers and reach

ServiceTargetMask calls FoldExposureExclusion after obtaining the serving mask. The fold ignores the reserved pairing tag, unions newly seen event pixels, and remembers event identities. ProcessCompletedFrames uses the union and cached dimensions/count for target-excluded luminance. Its exposure-dip/suppression diagnostics and existing exposure classification can therefore depend on the corrected state. The fold's log and final DRAWN SUMMARY expose the pixel count. The fix does not write a render resource, select a target, change a serving record or change owner-generation retirement.

| J2 path/cases | Reach and qualification |
|---|---|
| All capture starts | Equivalent full reset; ordinary first allocation/fold should be identical. Healthy single-view and UV regression on both fixtures provide the strict identity proof. |
| Delayed viewport | Unresolved/resolved owner transition can reset state; rerun both fixtures with the original predicates. |
| Resize | Owner invalidation and size/shape allocation; rerun both fixtures with W2 accounting and original predicates. A previously stale pixel count is cleared. |
| Camera cut/blend | Same-size owner invalidation caused the crash; rerun identical reference and active specs on both fixtures, first in PIE and later staged. |
| Selected viewport, second editor view, two-client | Owner resolves before ordinary capture; no scripted owner change mid-run. Inherit only after the identity proof. |
| Ambiguity, split, stereo, NotGameView sub-reasons | Refusal paths do not supply a new valid event mask to the fold; initial reset is equivalent. Inherit only after identity proof. |
| PF-1, PF-2, PF-3, PF-4, PF-5, native UV/proxy, synthetic alignment, pairing, admission probe | Stable owner and ordinary allocation/folding; no other mutable exposure-state site. Inherit completed PIE results only after identity proof. AA4 separately requires new TSR box legs. |
| Exact J1/pre-F1 controls | Archived product DLLs are unaffected; retained PIE controls stand only after the candidate identity proof. |

Reach reruns are camera, delayed and resize on both fixtures, plus the healthy/UV identity legs. Every staged case uses the new binary. Any further reachable mid-run state-change case discovered before a leg must be documented before execution; it must not be silently omitted.

## Identity proof

Use the exact _r939 specs and retained sessions for stack_P_healthy (session_20261015-074726), stack_P_reg_uv (session_20261015-081842), lyra_P_healthy (session_20261015-083353), lyra_P_reg_uv (session_20261015-090825). Use the same AA-off arbiter commands, original 1280x992 viewport setup and native order. No new baseline is substituted.

Gate the frozen PF-3 comparator: equal events, targets, session edges and visible_positive, with bbox_px maximum difference <=0.5 px. Also require identical mask-file membership and SHA-256 bytes, and parsed label rows field-identical in session-index order. No label fields are ignored, rebased or rounded. In particular, frame_index, t, t_wall, event_id, start_frame and mask_value remain compared. These contain run-local identities/clocks in the existing format; the literal AA3 gate may expose differences. Such a difference is a STOP, not permission to normalize labels. Pictures are compared by bytes as a reading only.

run_summary is recursively identical except the following explicitly named telemetry fields. Report every allowed difference and every unexpected difference; do not ignore an unknown key.

| Named field(s) | Why values can differ between identical-content runs |
|---|---|
| sustained_wall_fps, speed_ratio, stamped_fps, game_clock_speed_ratio | Wall-clock throughput and pacing readings |
| shader_prewarm_ms, change_worker_ms_total | Measured CPU/worker elapsed time |
| change_colour_completion_latency_ms_p50, change_colour_completion_latency_ms_p95, change_colour_completion_latency_ms_max, change_colour_completion_latency_frames_p50, change_colour_completion_latency_frames_p95, change_colour_completion_latency_frames_max | Async completion latency varies with scheduling |
| change_bytes_high_water, change_bytes_peak_census | Async residency peak and its complete diagnostic snapshot vary with completion timing; hard-cap/failure counters still compare |
| capture_game_ticks, ticks_per_captured_frame, key_ring_published, key_ring_consumed, key_ring_wrapped | Extra game ticks while draining and their ring accounting |
| census_frames, census_cycles, capture_ownership.mask_served | Background census/mask work during run/drain |
| capture_ownership.rejections.scene_mismatch, capture_view_rejected_colour, capture_view_rejected_mask | Extra non-owner request/family checks during run/drain |

All counts of requested/written/dropped/stale, event outcomes, admission, mask validity, exposure dips/suppression, projection, error/cap counters and all other fields compare exactly. The exception list does not include end_frame or any label identity field. Expected functional summary difference on the identity legs: none.

AA3 refers to ExposureExclusionPx in run_summary. Source/retained JSON inspection shows that this member is currently emitted only by the existing DRAWN SUMMARY log, not as a run_summary key. The scoped state repair will not add a new diagnostic key or edit the writer. Record the existing log count on resize; a new JSON field would require authority outside this state-only fix. On resize the count clears on invalidation and then reflects only the new generation's folded pixels; other original predicates remain unchanged.

Comparator can-fails use copies: a mask byte mutation, a label field mutation, a protected summary count mutation, and the existing PF-3 onset/one-pixel mutations must reject. A timing-only summary mutation must remain reported and allowed. Originals rehash unchanged.

## CPU self-test

The existing standalone MSVC self-test runner can host a scratch test without a new product file or dependency. Extract the actual reset/fold function bodies and owner-reset statements from each source revision; compile them with minimal standard-library container shims. Run the same owner-change-at-equal-size then fold scenario on eff797c and the fixed source. Bounds-checked shim indexing converts the old assertion to a nonzero test outcome. Require old failure and new success. Also test count-only empty-array recovery, ordinary repeated-event suppression, and equal-pixel-count shape changes. Preserve generated source, source hashes, compiler command/output and results. This is a CPU isolation proof; the retained real engine crash remains the engine can-fail.

## TSR half-resolution PF-2 and execution order

See docs/predictions/093-13-exposure-exclusion.md. AA4 retires Z2 part 3 only for pairing legs; dedicated box legs must have >=20 usable rows per axis. Camera failure, unexpected identity difference, insufficient TSR sample count, any pairing PREVIOUS/NEITHER, spent marker/metadata budgets and the existing disk/security/idle conditions stop the job. No failed gate is retried or relaxed.
