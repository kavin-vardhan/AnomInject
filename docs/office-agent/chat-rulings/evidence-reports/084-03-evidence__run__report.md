# 084-03 part B report (live, 2026-09-28T16:52:25.608417+00:00, evaluator 084-04a)

## corrections applied (084-04a)

- (a) an edge with a missing session_index or an engine-frame jump adjacent to it, inside its 2-frame onset confirmation or inside its 3-frame clean suffix is CENSORED, never passed
- (b) every pixel-visible frame outside the pixel window FAILS unless the paired null shows a visible drop at the event-aligned frame (withdraws G345 "isolated runs are reported, not required")
- (c) a labelled frame without a measured mask (no mask_value, target_pixels -1, png absent, or empty mask on a pixel-visible frame) FAILS gate (ii)
- (d) a contaminated event FAILS gate (iii) unless joined log-to-log, flagged stuck_mip.contaminated = 1 on every member frame from its si, and its trail closed back at baseline
- (e) per-leg edge-path frequencies and the hold-monitor cost reading (not measurable on this build: proxies only)

| leg | sync (i) | window (ii) | judged / censored | visible outside window: failing / null-explained | unlabelled visible after label | post-restore visible | onset label-minus-visible |
|---|---|---|---|---|---|---|---|
| B0_BASE | NO-JUDGEABLE-EVENT (0 fail) | N/A (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B3_BASE | NO-JUDGEABLE-EVENT (0 fail) | N/A (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B9_BASE | NO-JUDGEABLE-EVENT (0 fail) | N/A (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B7_BASE | NO-JUDGEABLE-EVENT (0 fail) | N/A (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B0_FIX | NO-JUDGEABLE-EVENT (0 fail) | PASS (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B3_FIX | NO-JUDGEABLE-EVENT (0 fail) | FAIL (1 fail; mask misses {'target_pixels unmeasured (-1)': 1}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B0L_FIX | NO-JUDGEABLE-EVENT (0 fail) | N/A (0 fail; mask misses {}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B7_FIX | NO-JUDGEABLE-EVENT (0 fail) | FAIL (2 fail; mask misses {'target_pixels unmeasured (-1)': 3}) | 0 / 0 | 0 / 0 | [] | [] | [] |
| B9_FIX | NO-JUDGEABLE-EVENT (0 fail) | FAIL (1 fail; mask misses {'target_pixels unmeasured (-1)': 1}) | 0 / 0 | 0 / 0 | [] | [] | [] |

| purity / null | verdict | detail |
|---|---|---|
| B5_BASE | PASS | labelled_held_rows 0, render_held_rows 0, refs {"StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803": {"mu": 4.7825, "sigma_resid": 0.0088, "worst_drop_rel": 0.2664, "null_events_used": 20, "floor_rel": 0.02, "n": 1170}} |
| B5_FIX | PASS | labelled_held_rows 0, render_held_rows 0, refs {"StaticMeshActor_UAID_A036BC6AB247EBF902_2044254803": {"mu": 4.8229, "sigma_resid": 0.0085, "worst_drop_rel": 0.2893, "null_events_used": 18, "floor_rel": 0.02, "n": 1170}} |
| B0_FIX:purity | PASS (Apply census PASS) | enumerated 54, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |
| B3_FIX:purity | PASS (Apply census PASS) | enumerated 27, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |
| B0L_FIX:purity | PASS (Apply census PASS) | enumerated 69, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |
| P0_FIX:purity | PASS-REFUSED (Apply census PASS-REFUSED) | enumerated 0, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |
| P1_FIX:purity | FAIL (Apply census FAIL) | enumerated 19, bad 19, refused_shared_world 0, contaminated events 4, not handled as ruled ['stuck_low_mip@46', 'stuck_low_mip@114', 'stuck_low_mip@184', 'stuck_low_mip@254'] |
| B7_FIX:purity | PASS (Apply census PASS) | enumerated 51, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |
| B9_FIX:purity | PASS (Apply census PASS) | enumerated 54, bad 0, refused_shared_world 20, contaminated events 0, not handled as ruled [] |

## contaminated events (every one)

- P1_FIX:purity stuck_low_mip@46 from si 41: handled_as_ruled=False, flagged 0/0, trail closed at 74, problems ['no member frame at or after the contamination si to carry the flag']
- P1_FIX:purity stuck_low_mip@114 from si 108: handled_as_ruled=False, flagged 0/0, trail closed at 141, problems ['no member frame at or after the contamination si to carry the flag']
- P1_FIX:purity stuck_low_mip@184 from si 175: handled_as_ruled=False, flagged 0/0, trail closed at 209, problems ['no member frame at or after the contamination si to carry the flag']
- P1_FIX:purity stuck_low_mip@254 from si 243: handled_as_ruled=False, flagged 0/0, trail closed at 276, problems ['no member frame at or after the contamination si to carry the flag']

## edge-path frequency (ruling 084-02b decision 1)

| leg | occurred | trail reopens | ORDER HOLD | gap-unknowns | carried trails | contaminated | unresolved / timeout |
|---|---|---|---|---|---|---|---|
| B0_BASE | [] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| B5_BASE | ['unresolved_timeout'] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 19, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 19, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| B3_BASE | [] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| B9_BASE | [] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| B7_BASE | [] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| B0_FIX | ['carried_trails', 'unresolved_timeout'] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 1, "inherited_lines": 0, "live_window_cut_lines": 1, "restore_carried_at_end": 1, "restore_inherited_at_start": 0, "live_window_cut": 1} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 1, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": 0, "restore_unresolved_at_end": 1, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| B5_FIX | ['unresolved_timeout'] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": 0, "restore_inherited_at_start": 0, "live_window_cut": 0} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 18, "restore_unresolved": 0, "restore_unresolved_at_end": 0, "restore_timeout": 0, "hold_timeouts": 18, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| B3_FIX | [] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": 0, "restore_inherited_at_start": 0, "live_window_cut": 0} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": 0, "restore_unresolved_at_end": 0, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| B0L_FIX | [] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": 0, "restore_inherited_at_start": 0, "live_window_cut": 0} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": 0, "restore_unresolved_at_end": 0, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| P0_FIX | [] | {"log_lines": 0, "summary": null, "render_truth": null, "unwatched_after_close": null} | {"log_lines": 0, "summary_frames": null, "render_truth": null} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": null, "watch_missing_frames": null, "render_record_missing_frames": null, "render_unknown_frames": null, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": null, "restore_inherited_at_start": null, "live_window_cut": null} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": null, "contaminated_frames": null, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": null, "restore_unresolved_at_end": null, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": null, "observe_forced_64_tick_fallback": null} |
| P1_FIX | ['contaminated_events', 'unresolved_timeout'] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": 0, "restore_inherited_at_start": 0, "live_window_cut": 0} | {"capture_lines": 4, "anomaly_lines": 4, "hold_contaminations": 4, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 4, "restore_unresolved": 0, "restore_unresolved_at_end": 0, "restore_timeout": 0, "hold_timeouts": 4, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| B7_FIX | [] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 0, "inherited_lines": 0, "live_window_cut_lines": 0, "restore_carried_at_end": 0, "restore_inherited_at_start": 0, "live_window_cut": 0} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 0, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": 0, "restore_unresolved_at_end": 0, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |
| B9_FIX | ['carried_trails', 'unresolved_timeout'] | {"log_lines": 0, "summary": 0, "render_truth": "0", "unwatched_after_close": 0} | {"log_lines": 0, "summary_frames": 0, "render_truth": "0"} | {"trail_gap_lines": 0, "late_receipt_lines": 0, "trail_missing_frames": 0, "watch_missing_frames": 0, "render_record_missing_frames": 0, "render_unknown_frames": 0, "rows_unknown": 0, "rows_watch_missing": 0} | {"carried_lines": 1, "inherited_lines": 0, "live_window_cut_lines": 1, "restore_carried_at_end": 1, "restore_inherited_at_start": 0, "live_window_cut": 1} | {"capture_lines": 0, "anomaly_lines": 0, "hold_contaminations": 0, "contaminated_frames": 0, "rows_flagged": 0} | {"restore_unresolved_lines": 0, "restore_unresolved_at_run_end_lines": 1, "restore_timeout_lines": 0, "hold_timeout_lines": 0, "restore_unresolved": 0, "restore_unresolved_at_end": 1, "restore_timeout": 0, "hold_timeouts": 0, "observe_unresolved": 0, "observe_forced_64_tick_fallback": "0"} |

- FIX totals: {"trail_reopens": {"legs_where_it_occurred": [], "log_lines": 0.0, "summary": 0.0, "render_truth": 0.0, "unwatched_after_close": 0.0}, "order_hold": {"legs_where_it_occurred": [], "log_lines": 0.0, "summary_frames": 0.0, "render_truth": 0.0}, "gap_unknowns": {"legs_where_it_occurred": [], "trail_gap_lines": 0.0, "late_receipt_lines": 0.0, "trail_missing_frames": 0.0, "watch_missing_frames": 0.0, "render_record_missing_frames": 0.0, "render_unknown_frames": 0.0, "rows_unknown": 0.0, "rows_watch_missing": 0.0}, "carried_trails": {"legs_where_it_occurred": ["B0_FIX", "B9_FIX"], "carried_lines": 2.0, "inherited_lines": 0.0, "live_window_cut_lines": 2.0, "restore_carried_at_end": 2.0, "restore_inherited_at_start": 0.0, "live_window_cut": 2.0}, "contaminated_events": {"legs_where_it_occurred": ["P1_FIX"], "capture_lines": 4.0, "anomaly_lines": 4.0, "hold_contaminations": 4.0, "contaminated_frames": 0.0, "rows_flagged": 0.0}, "unresolved_timeout": {"legs_where_it_occurred": ["B0_FIX", "B5_FIX", "P1_FIX", "B9_FIX"], "restore_unresolved_lines": 0.0, "restore_unresolved_at_run_end_lines": 2.0, "restore_timeout_lines": 0.0, "hold_timeout_lines": 22.0, "restore_unresolved": 0.0, "restore_unresolved_at_end": 2.0, "restore_timeout": 0.0, "hold_timeouts": 22.0, "observe_unresolved": 0.0, "observe_forced_64_tick_fallback": 0.0}}

## hold-monitor per-tick cost

- B0_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [0.9, 0.87, 0.86, 1.15, 0.91, 0.91, 1.74, 0.91, 1.4, 0.92, 1.22, 0.9, 1.27, 1.36, 1.57, 0.95, 1.62, 0.95, 1.94] ms (max 1.938600034918636); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 490, 'stuck_mip_hold_monitor_ms_mean': 0.18050734818513905, 'stuck_mip_hold_monitor_ms_p95': 0.26090002059936523, 'stuck_mip_hold_monitor_ms_max': 0.45219993591308594, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- B5_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [0.9, 1.73, 0.89, 0.95, 0.97, 0.91, 1.2, 1.57, 1.01, 0.93, 1.24, 1.67, 0.94, 1.04, 1.64, 1.65, 1.06, 1.65, 0.94] ms (max 1.725599984638393); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 576, 'stuck_mip_hold_monitor_ms_mean': 0.1841567708551237, 'stuck_mip_hold_monitor_ms_p95': 0.2531999945640564, 'stuck_mip_hold_monitor_ms_max': 0.4198000431060791, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- B3_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [1.37, 1.0, 1.12, 4.85, 3.86, 2.4, 2.03, 2.42, 4.29, 2.04, 3.17, 5.64, 1.74, 5.85, 2.5, 2.67, 1.39, 1.09, 5.19, 1.65, 3.78, 0.96, 2.11, 1.61] ms (max 5.853300041053444); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 230, 'stuck_mip_hold_monitor_ms_mean': 0.20623651975968285, 'stuck_mip_hold_monitor_ms_p95': 0.33129996061325073, 'stuck_mip_hold_monitor_ms_max': 0.6341000199317932, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- B0L_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [1.04, 1.95, 2.07, 1.52, 1.79, 1.1, 1.65, 1.9, 2.17, 1.08, 1.38, 1.92, 1.05, 1.75, 1.07, 1.9, 1.11, 1.57, 1.08, 1.04, 2.02, 1.81, 1.62, 1.84] ms (max 2.173699962440878); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 546, 'stuck_mip_hold_monitor_ms_mean': 0.20875128057713693, 'stuck_mip_hold_monitor_ms_p95': 0.2872999906539917, 'stuck_mip_hold_monitor_ms_max': 0.4652999937534332, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- P0_FIX: measured=False; components walked per tick []; Apply census [3.1, 2.89, 3.44, 3.98, 2.9, 4.37, 4.44, 4.83] ms (max None); run_summary monitor keys {}
- P1_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180]; Apply census [] ms (max 0); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 10, 'stuck_mip_hold_monitor_ms_mean': 0.6025100010447204, 'stuck_mip_hold_monitor_ms_p95': 1.3346000909805298, 'stuck_mip_hold_monitor_ms_max': 1.3346000909805298, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- B7_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [0.89, 1.02, 1.45, 1.05, 0.93, 0.95, 2.03, 0.92, 1.11, 1.51, 1.48, 2.15, 0.94, 0.91, 0.93, 1.09, 0.99, 1.0] ms (max 2.1503999596461654); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 452, 'stuck_mip_hold_monitor_ms_mean': 0.19522500057000072, 'stuck_mip_hold_monitor_ms_p95': 0.30389997363090515, 'stuck_mip_hold_monitor_ms_max': 0.6140000224113464, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- B9_FIX: measured=False; components walked per tick [1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180, 1180]; Apply census [1.17, 1.82, 1.16, 2.86, 1.94, 1.91, 1.6, 2.18, 1.76, 1.2, 1.22, 1.19, 1.88, 1.78, 0.98, 1.48, 1.05, 1.64, 1.23] ms (max 2.8561000362969935); run_summary monitor keys {'stuck_mip_hold_monitor_scans': 458, 'stuck_mip_hold_monitor_ms_mean': 0.21432379924582406, 'stuck_mip_hold_monitor_ms_p95': 0.3086000084877014, 'stuck_mip_hold_monitor_ms_max': 0.5792999863624573, 'stuck_mip_hold_monitor_components_walked_max': 1180, 'stuck_mip_hold_monitor_unregistered_watched_max': 0, 'stuck_mip_hold_monitor_registration_rejudges': 0, 'stuck_mip_hold_monitor_dirty_requeued': 0}
- why not measured: this build carries no timer around the per-tick hold-monitor scan (ScanHoldForNewUsers counts HoldMonitorScans and emits neither the count nor a time), and every leg is paced at 30 fps, whose pacer sleeps inside the game tick and absorbs any sub-budget cost (G-M6). The per-tick game-thread cost cannot be read from these artifacts; it needs a source timer. What follows are proxies, NOT the cost

## can-fail

- lever_restores_old_timing_must_FAIL: proven=False (NO-JUDGEABLE-EVENT)
- old_purity_rule_must_FAIL: proven=True (FAIL)
- doctored_labels: proven=False (no measurable, uncensored, static event with room for the variants in any accepted SYNC leg)

| real-pixel check | proven on | contradicted on |
|---|---|---|
| repaired | [] | [] |
| onset_plus2 | [] | [] |
| offset_minus5 | [] | [] |
| early_onset_overlabel_minus2 | [] | [] |
| late_offset_overlabel_plus4 | [] | [] |
| masks_missing | [] | [] |
| mask_png_absent | [] | [] |
| gap_missing_si_after_offset | [] | [] |
| gap_engine_jump_in_suffix | [] | [] |
| gap_engine_jump_before_onset | [] | [] |
| gap_engine_jump_inside_window | [] | [] |
| recurrence_real_pixels | [] | [] |
| recurrence_shown_by_null | [] | [] |
| single_spike_real_pixels | [] | [] |
| original_spike_unedited | [] | [] |
  - skipped B0_FIX event 46: camera moved
  - skipped B0_FIX event 111: not measurable
  - skipped B0_FIX event 182: not measurable
  - skipped B0_FIX event 251: not measurable
  - skipped B0_FIX event 321: not measurable
  - skipped B0_FIX event 392: not measurable
  - skipped B0_FIX event 462: not measurable
  - skipped B0_FIX event 531: not measurable
  - skipped B0_FIX event 602: not measurable
  - skipped B0_FIX event 671: not measurable
  - skipped B0_FIX event 743: not measurable
  - skipped B0_FIX event 812: not measurable
  - skipped B0_FIX event 881: not measurable
  - skipped B0_FIX event 952: not measurable
  - skipped B0_FIX event 1022: not measurable
  - skipped B0_FIX event 1091: not measurable
  - skipped B0_FIX event 1162: not measurable
  - skipped B0_FIX event 1231: SHORT-SPAN
  - skipped B3_FIX event 46: camera moved
  - skipped B3_FIX event 115: not measurable
  - skipped B3_FIX event 185: not measurable
  - skipped B3_FIX event 507: not measurable
  - skipped B3_FIX event 785: not measurable
  - skipped B3_FIX event 899: not measurable
  - skipped B3_FIX event 1011: not measurable
  - skipped B3_FIX event 1119: not measurable
  - skipped B3_FIX event 1235: not measurable
  - skipped B0_BASE event 46: camera moved
  - skipped B0_BASE event 102: not measurable
  - skipped B0_BASE event 158: not measurable
  - skipped B0_BASE event 214: not measurable
  - skipped B0_BASE event 270: not measurable
  - skipped B0_BASE event 326: not measurable
  - skipped B0_BASE event 382: not measurable
  - skipped B0_BASE event 438: not measurable
  - skipped B0_BASE event 494: not measurable
  - skipped B0_BASE event 550: not measurable
  - skipped B0_BASE event 606: not measurable
  - skipped B0_BASE event 662: not measurable
  - skipped B0_BASE event 718: not measurable
  - skipped B0_BASE event 774: not measurable
  - skipped B0_BASE event 830: not measurable
  - skipped B0_BASE event 886: not measurable
  - skipped B0_BASE event 942: not measurable
  - skipped B0_BASE event 998: not measurable
  - skipped B0_BASE event 1054: not measurable
  - skipped B0_BASE event 1110: not measurable
  - skipped B0_BASE event 1166: not measurable
  - skipped B0_BASE event 1222: not measurable
  - skipped B0_BASE event 1278: SHORT-SPAN
  - skipped B3_BASE event 46: camera moved
  - skipped B3_BASE event 102: not measurable
  - skipped B3_BASE event 158: not measurable
  - skipped B3_BASE event 214: not measurable
  - skipped B3_BASE event 270: not measurable
  - skipped B3_BASE event 375: not measurable
  - skipped B3_BASE event 438: not measurable
  - skipped B3_BASE event 536: not measurable
  - skipped B3_BASE event 634: not measurable
  - skipped B3_BASE event 739: not measurable
  - skipped B3_BASE event 844: not measurable
  - skipped B3_BASE event 942: not measurable
  - skipped B3_BASE event 998: not measurable
  - skipped B3_BASE event 1061: not measurable
  - skipped B3_BASE event 1159: not measurable
  - skipped B3_BASE event 1257: not measurable

## settle-tail measurement

- B0_FIX: {"aa": "as delivered", "residual_after_record": [], "residual_before_record": [], "post_restore_visible": [], "closure_rule": {"rule": "NO-JUDGEABLE-EVENT"}}
- B3_FIX: {"aa": "as delivered", "residual_after_record": [], "residual_before_record": [], "post_restore_visible": [], "closure_rule": {"rule": "NO-JUDGEABLE-EVENT"}}
- B9_FIX: {"aa": "off", "residual_after_record": [], "residual_before_record": [], "post_restore_visible": [], "closure_rule": {"rule": "NO-JUDGEABLE-EVENT"}}
- B7_FIX: {"aa": "as delivered", "residual_after_record": [], "residual_before_record": [], "post_restore_visible": [], "closure_rule": {"rule": "NO-JUDGEABLE-EVENT"}}
- B0_BASE: {"after_label_visible": []}
- B9_BASE: {"after_label_visible": []}
- closure_rule_AA_as_delivered: {"rule": "NO-JUDGEABLE-EVENT"}
- closure_rule_AA_off: {"rule": "NO-JUDGEABLE-EVENT"}

## premise audit

- B0_BASE: {"focus_at_s": 2.3, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3"}, "run_log": true, "label_source": null, "bench_levers": false, "precondition_readback": [["1", "0"]], "frames": 1200}
- B3_BASE: {"focus_at_s": 1.4, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3"}, "run_log": true, "label_source": null, "bench_levers": false, "precondition_readback": [["1", "0"]], "frames": 1200}
- B9_BASE: {"focus_at_s": 1.4, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "0", "sg.TextureQuality": "3"}, "run_log": true, "label_source": null, "bench_levers": false, "precondition_readback": [["1", "0"]], "frames": 1200}
- B7_BASE: {"focus_at_s": 1.8, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3"}, "run_log": true, "label_source": null, "bench_levers": false, "precondition_readback": [["1", "0"]], "frames": 1200}
- B0_FIX: {"focus_at_s": 1.4, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3", "IAI.StuckMip.SettleTailFrames": "0"}, "run_log": true, "label_source": "RENDER RECORD", "bench_levers": true, "precondition_readback": [["1", "0"]], "frames": 1200}
- B3_FIX: {"focus_at_s": 1.4, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3", "IAI.StuckMip.SettleTailFrames": "0"}, "run_log": true, "label_source": "RENDER RECORD", "bench_levers": true, "precondition_readback": [["1", "0"]], "frames": 1200}
- B0L_FIX: {"focus_at_s": 2, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3", "IAI.StuckMip.SettleTailFrames": "0"}, "run_log": true, "label_source": "LEGACY GAME-THREAD MIRROR (IAI.Bench.StuckMipLegacyTiming)", "bench_levers": true, "precondition_readback": [["1", "0"]], "frames": 1200}
- B7_FIX: {"focus_at_s": 1.9, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "4", "sg.TextureQuality": "3", "IAI.StuckMip.SettleTailFrames": "0"}, "run_log": true, "label_source": "RENDER RECORD", "bench_levers": true, "precondition_readback": [["1", "0"]], "frames": 1200}
- B9_FIX: {"focus_at_s": 1.6, "focus_held_whole_leg": true, "echo": {"r.TextureStreaming": "1", "r.Streaming.UseAllMips": "0", "r.Streaming.FramesForFullUpdate": "5", "r.Streaming.OverlapAssetAndLevelTicks": "1", "r.AntiAliasingMethod": "0", "sg.TextureQuality": "3", "IAI.StuckMip.SettleTailFrames": "0"}, "run_log": true, "label_source": "RENDER RECORD", "bench_levers": true, "precondition_readback": [["1", "0"]], "frames": 1200}
