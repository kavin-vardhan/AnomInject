# 085-04 B-M53 report (night, 2026-09-29T16:37:07.408413+00:00, evaluator 085-04, release t50)

| leg | group | verdict | t50 start / end | edges (on/off/fail) | NM | label-rule | mode_source | fail reasons |
|---|---|---|---|---|---|---|---|---|
| P_HOST_CEN | PREMISE | FAIL |  /  | //0 |  |  |  |  |
| P_HOST_BENCH | PREMISE |  |  /  | //0 |  |  |  |  |
| P_ORC_UV | PREMISE |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| P_ORC_N | PREMISE |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| P_M52F | PREMISE |  |  /  | //0 |  | NO MISMATCH under the NEW rule |  |  |
| P_FLOOR | PREMISE |  |  /  | //0 |  | NO MISMATCH under the NONE rule | True |  |
| P_SYN | PREMISE |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| LEVER_OFF | LEVER |  |  /  | //0 |  |  |  |  |
| LEVER_ON | LEVER |  |  /  | //0 |  |  |  |  |
| CENSUS_FSYN | CENSUS | PASS |  /  | //0 |  |  |  |  |
| CENSUS_FSYN_BENCH | CENSUS |  |  /  | //0 |  |  |  |  |
| F_TILE_NAT | BM53F |  |  /  | //0 |  |  |  |  |
| F_TILE_NAT_AAOFF | BM53F |  |  /  | //0 |  |  |  |  |
| F_TILE_SYN | BM53F |  |  /  | //0 |  |  |  |  |
| F_SCRAMBLE_NAT | BM53F |  |  /  | //0 |  |  |  |  |
| F_SCRAMBLE_NAT_AAOFF | BM53F |  |  /  | //0 |  |  |  |  |
| F_SCRAMBLE_SYN | BM53F |  |  /  | //0 |  |  |  |  |
| F_INVERT_NAT | BM53F |  |  /  | //0 |  |  |  |  |
| F_INVERT_NAT_AAOFF | BM53F |  |  /  | //0 |  |  |  |  |
| F_INVERT_SYN | BM53F |  |  /  | //0 |  |  |  |  |
| F_GREEN_FLIP_NAT | BM53F |  |  /  | //0 |  |  |  |  |
| F_GREEN_FLIP_NAT_AAOFF | BM53F |  |  /  | //0 |  |  |  |  |
| F_GREEN_FLIP_SYN | BM53F |  |  /  | //0 |  |  |  |  |
| F_NULL_UV_AAON | BM53F | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| F_NULL_UV_AAOFF | BM53F | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| F_NULL_N_AAON | BM53F | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| F_NULL_N_AAOFF | BM53F | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| MODE_TILE | GMODE |  |  /  | //0 |  |  |  |  |
| MODE_SCRAMBLE | GMODE |  |  /  | //0 |  |  |  |  |
| MODE_UVNULL | GMODE | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| MODE_INVERT | GMODE |  |  /  | //0 |  |  |  |  |
| MODE_GREENFLIP | GMODE |  |  /  | //0 |  |  |  |  |
| MODE_NNULL | GMODE | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| ARM_CD3_AAOFF | BM53ARM |  |  /  | //0 |  |  |  |  |
| ARM_CD3_AAON | BM53ARM |  |  /  | //0 |  |  |  |  |
| ARM_RD3_AAOFF | BM53ARM |  |  /  | //0 |  |  |  |  |
| ARM_RD3_AAON | BM53ARM |  |  /  | //0 |  |  |  |  |
| ARM_NOAPPLY1 | BM53ARM | FAIL | {} / {} | 0/0/0 | 9 | NO MISMATCH under the NEW rule | True | NOT-MEASURABLE on the fixture: 9 event(s); mask rows FAIL: missing 0 extra 0 owners 72 |
| REASON_E6 | REASON | FAIL | {} / {} | 0/0/0 | 0 | NO MISMATCH under the NONE rule | True | mask rows NOT-EXERCISED: missing 0 extra 0 owners 0 |
| REASON_MIXED_N | REASON | FAIL | {} / {} | 0/0/0 | 0 | NO MISMATCH under the NONE rule | True | mask rows NOT-EXERCISED: missing 0 extra 0 owners 0 |
| REASON_MIXED_UV | REASON | FAIL | {} / {} | 0/0/0 | 0 | NO MISMATCH under the NONE rule | True | mask rows NOT-EXERCISED: missing 0 extra 0 owners 0 |
| EXCL_A | EXCL |  |  /  | //0 |  |  |  |  |
| EXCL_B | EXCL |  |  /  | //0 |  |  |  |  |
| BIND_TILE | BIND |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| BIND_TILENULL | BIND |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| BIND_ID | BIND |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| BIND_IDNULL | BIND |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_UC1_ID | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_UC1_NULL | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_UC1_WRONG | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_NN1_ID | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_NN1_NULL | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| GID_NN1_WRONG | GID |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| SEED_A1 | SEED |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| SEED_A2 | SEED |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| SEED_EMPTY | SEED |  |  /  | //0 |  | NO MISMATCH under the NEW rule | True |  |
| AON_MW | AON | FAIL | {} / {} | 0/0/0 | 0 | NO MISMATCH under the NONE rule | True | mask rows NOT-EXERCISED: missing 0 extra 0 owners 0 |
| MW_H1_TILE_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_SCRAMBLE_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_INVERT_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_GREEN_FLIP_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_NULL_UV_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_NULL_N_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_TILE_NAT_AAOFF | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_INVERT_NAT_AAOFF | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_NULL_UV_NAT_AAOFF | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H1_NULL_N_NAT_AAOFF | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_TILE_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_SCRAMBLE_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_INVERT_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_GREEN_FLIP_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_NULL_UV_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| MW_H2_NULL_N_NAT | BM53MW | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| COST_DISCARD | COST | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| COST_A1 | COST | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| COST_B1 | COST | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| COST_B2 | COST | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| COST_A2 | COST | NOT-RUN-PREMISE |  /  | //0 |  |  |  |  |
| SR_B0 | SYNCREG | PASS | {+2:16} / {+4:2, +5:12, +6:2} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_B5 | SYNCREG | NULL-PASS |  /  | //0 |  | NO MISMATCH under the NONE rule |  |  |
| SR_B3 | SYNCREG | PASS-WITH-CENSORED | {+3:8} / {+5:3, +7:1} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_B9 | SYNCREG | PASS | {+3:16} / {+0:16} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_REAL_BL_NAT | SYNCREG | PASS | {+0:20} / {+0:20} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_REAL_MO_NAT | SYNCREG | PASS | {+0:10} / {+0:10} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_REAL_MT_NAT | SYNCREG | PASS | {+0:10} / {+0:10} | //0 |  | NO MISMATCH under the NEW rule |  |  |
| SR_REAL_CT_NAT | SYNCREG | PASS | {+0:10} / {+0:10} | //0 |  | NO MISMATCH under the NEW rule |  |  |

## gates

- **G-MODE:** `{"MODE_TILE": {"verdict": "ERROR", "error": "ll_as_modes\"] = judge_null_as_modes(nframes, maps, dict(tile_n=parsed[\"tile_n\"] or DC_TILE_N, scramble=pairs))\n                           ~~~~~~~~~~~~~~~~~~~^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n  File \"D:\\IntrusiveAnomalies\\_reviews\\085-04-oracle.py\", line 1193, in judge_null_as_modes\n    r = ev_mode_uv(frames, maps[\"uv16\"], \"tile\", dict(n=tile_n))\n  File \"D:\\IntrusiveAnomalies\\_reviews\\085-04-oracle.py\", line 863, in ev_mode_uv\n    pal = np.array(null_map[\"cell_colour\"], np.float32)\nValueError: setting an array element with a sequence. The requested array has an inhomogeneous shape a`
- **EXCL-A:** `{"carried": 0, "trail_open_excludes": 0, "first_exclude": null, "close_frame": null, "first_m53_apply": 69, "applies_before_close": 0, "m53_applied_after": 6, "frames": {"session": "D:\\IA_BankOverflow\\M53DC_EXCL_A_A1\\session_20260929-214036", "verdict": 0, "reason": "0 frames carry both an m52 entry (labelled or transition) and an m53 entry", "coentry_frames": [], "rows": 120, "m52_entry_frames": 0, "m53_entry_frames": 47, "overlap_counter": 0, "admitted_after_unresolved": 0}, "verdict": "FAIL", "note": "frame numbers are the log [fff] prefix (GFrameCounter % 1000)"}`
- **EXCL-B:** `{"first": "[2026.09.29-16.11.41:925][  9]LogAnomaly: Auto.Draw attempt=2 seed_before=3119884360 id=- eligible=0:00000000 excluded=stuck_low_mip target=- candidates=- hold=- mode=- seed_after=3119884360 result=all_excluded", "second": "422][ 24]LogAnomaly: Auto.Draw attempt=3 seed_before=3119884360 id=stuck_low_mip eligible=1:018e1206 excluded=- target=StaticMeshActor_55 candidates=4:6df7e507 hold=3.39270186 mode=- seed_after=562214221 result=0 matched", "first_all_excluded_seed_unchanged": true, "second_applies_m52": true, "m52_hold_lines": 2, "frames": {"session": "D:\\IA_BankOverflow\\M53DC_EXCL_B_A1\\session_20260929-214141", "verdict": 0, "reason": "0 frames carry both an m52 entry (labe`
- **G-SEED:** `{"a_draws": 14, "b_draws": 14, "repeat_identical": true, "replay": {"pass_": true, "lines": 14, "mismatches": [], "note": "per attempt: id over eligible, then target + hold + (m53) one mode draw when candidates > 0; the stream starts at the capture seed"}, "coverage": {"uv_corruption:tile": 2, "normal_corruption:green_flip": 2, "normal_corruption:invert": 1, "uv_corruption:scramble": 1}, "coverage_missing": [], "empty": {"m53_draws": 6, "all_none": true, "replay": {"pass_": true, "lines": 14, "mismatches": [], "note": "per attempt: id over eligible, then target + hold + (m53) one mode draw when candidates > 0; the stream starts at the capture seed"}}, "pass_": true}`
- **G-ID:** `{"UC1": {"identity_vs_null": {"verdict": "PASS", "exact": true, "max": 0, "hist": {"0": 233472, "1": 0, "2": 0, "3-7": 0, "8-15": 0, ">=16": 0}, "events": [{"start_frame": 18, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 34, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 50, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 66, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 82, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 98, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 114, "frames": 8, "measured": 8, "max": 0}, {"start_frame": 130, "frames": 1, "measured": 1, "max": 0}], "measured_events": 8, "frames_measured": 57, "frames_unmeasured": 0, `
- **G-BIND:** `{"per_frame": {"verdict": "INVALID-FIXTURE", "why": ["the global/layer halves could not be located from the NoApply picture"], "split": {"si": 15, "uniform_halves": [], "ranges": {"x_low": 153, "x_high": 151, "y_low": 155, "y_high": 17}, "uniform_lt": 16}, "rule": "S2 item 6 (plan 1.2 row 6, 5.1): every measured tile-probe frame must change the layer half (>= 1 pixel at |d| >= thr against the NoApply null at the same session_index); the per-event minimum over measured frames is reported with every failing si. S1 took the per-event maximum (082-07-lib:845, :855), which passes [1192,0,0,0,0,0,0,0]"}, "far_strip": {"verdict": "INVALID-FIXTURE", "why": ["the global/layer halves could not be loca`
- **COST:** `{"per_leg": {"COST_A1": {"fires": 0, "apply_ms_med": null, "revert_ms_med": null, "gpu_ms_med": null, "gt_ms_med": null, "cost_lines": 0}, "COST_B1": {"fires": 0, "apply_ms_med": null, "revert_ms_med": null, "gpu_ms_med": null, "gt_ms_med": null, "cost_lines": 0}, "COST_B2": {"fires": 0, "apply_ms_med": null, "revert_ms_med": null, "gpu_ms_med": null, "gt_ms_med": null, "cost_lines": 0}, "COST_A2": {"fires": 0, "apply_ms_med": null, "revert_ms_med": null, "gpu_ms_med": null, "gt_ms_med": null, "cost_lines": 0}}, "reading": "NOT READ", "rule": "A,B,B,A after a declared discard; no threshold (G169, G186)"}`
- **ND-5:** `{}`
- **G-CENSUS:** `{"host": {"verdict": "FAIL", "exit": 1, "tail": ["CENSUS-LOG census #1: frame 91, 4 line(s) in the block, 4 census line(s), 0 other-category line(s) (not plugin output)", "CENSUS-LOG census #2: the counts block is not scope / id=uv_corruption / id=normal_corruption / end stats_unchanged=1 (8 census line(s))", "CENSUS-LOG census #2: frame 91, 10 line(s) in the block, 8 census line(s), 2 other-category line(s) (not plugin output)", "CENSUS-LOG VERDICT FAIL (cold process)"], "pass_": false}, "fixture": {"verdict": "PASS", "exit": 0, "tail": ["CENSUS-LOG census #1: frame 91, 4 line(s) in the block, 4 census line(s), 0 other-category line(s) (not plugin output)", "CENSUS-LOG VERDICT PASS (cold pr`
- **B-M53:** `{"per_mode_condition": {}, "leg_verdicts": {"F_TILE_NAT": null, "F_TILE_NAT_AAOFF": null, "F_TILE_SYN": null, "F_SCRAMBLE_NAT": null, "F_SCRAMBLE_NAT_AAOFF": null, "F_SCRAMBLE_SYN": null, "F_INVERT_NAT": null, "F_INVERT_NAT_AAOFF": null, "F_INVERT_SYN": null, "F_GREEN_FLIP_NAT": null, "F_GREEN_FLIP_NAT_AAOFF": null, "F_GREEN_FLIP_SYN": null, "F_NULL_UV_AAON": "NULL-PASS", "F_NULL_UV_AAOFF": "NULL-PASS", "F_NULL_N_AAON": "NULL-PASS", "F_NULL_N_AAOFF": "NULL-PASS", "MW_H1_TILE_NAT": null, "MW_H1_SCRAMBLE_NAT": null, "MW_H1_INVERT_NAT": null, "MW_H1_GREEN_FLIP_NAT": null, "MW_H1_NULL_UV_NAT": null, "MW_H1_NULL_N_NAT": null, "MW_H1_TILE_NAT_AAOFF": null, "MW_H1_INVERT_NAT_AAOFF": null, "MW_H1_NU`
- **G-SYNC-REG:** `{"SR_B0": "PASS", "SR_B5": "NULL-PASS", "SR_B3": "PASS-WITH-CENSORED", "SR_B9": "PASS", "SR_REAL_BL_NAT": "PASS", "SR_REAL_MO_NAT": "PASS", "SR_REAL_MT_NAT": "PASS", "SR_REAL_CT_NAT": "PASS"}`

## can-fail

- ARM_NOAPPLY1: {"expect": "holes", "verdict": "NO-JUDGEABLE-EVENT", "not_measurable": 9, "fail_reasons": ["NOT-MEASURABLE on the fixture: 9 event(s)", "mask rows FAIL: missing 0 extra 0 owners 72"], "proven": true}

label_rule_all_pass: True

lock: `{"utc": "2026-09-29T15:23:53.264735+00:00", "harness": "085-04", "stub": false, "dispositions": {"ND-4": "no cap raise: every family with a host has one eligible at the delivered 128 MiB. uv: NO host at any cap <= 1 GiB (refusals are not budget): uv MainWorld legs NOT-RUN-PREMISE (chat); normal: NO host at any cap <= 1 GiB (refusals are not budget): normal MainWorld legs NOT-RUN-PREMISE (chat)", "hosts": "H1 uv None / normal None (PER-FAMILY hosts: no MainWorld actor is eligible for both families - flagged for chat); H2 uv None / normal None", "P-ORC": "FAIL: the mode-oracle rows cannot run (reported, never PASS)", "P-M52F": "FAIL (no RESTORE CARRIED: EXCL-A uses the rock next night)", "P-EXCL": "run 2 starts 63 DEFER ticks after the batch; pre = 4 (the first burst precedes the close)", "P-EXCL-B": "FireOnce 1 at +8 (inside the first positives), FireOnce 2 at +23 (PostGap after R+2)", "P`
