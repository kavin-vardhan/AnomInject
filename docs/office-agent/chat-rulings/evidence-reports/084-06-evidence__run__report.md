# 084-06 bench report (gates, 2026-09-28T21:08:18.277842+00:00, evaluator 084-06, release threshold t50)

| leg | group | E1 release (t50) | E1 strict / t10 | start / end at t50 | E2 086-01 t50 raw / transition-aware | presence contract |
|---|---|---|---|---|---|---|
| B0 | M52 | PASS-WITH-CENSORED {"CENSORED": 12, "PASS": 4} | {"CENSORED": 8, "FAIL": 8} / {"CENSORED": 9, "FAIL": 7} | {+1:13, +2:3} / {+4:2, +5:2} | {"CAMERA-MOVED": 1, "CONFOUNDED": 3, "FAIL": 4, "PASS-WITH-CENSORED-EDGE": 6, "UNRESOLVED": 3, "NO-LABELLED-FRAMES": 1} / {"CONFOUNDED": 3, "PASS(transition-excused)": 2, "PASS-WITH-CENSORED-EDGE": 6, "UNRESOLVED": 3, "FAIL": 2} | 0 |
| B5 | NULL | FAIL | labelled_held_rows 0 | | | |
| B3 | M52 | PASS-WITH-CENSORED {"CENSORED": 5, "PASS": 3} | {"CENSORED": 4, "FAIL": 4} / {"CENSORED": 5, "FAIL": 3} | {+2:8} / {+5:2, +6:1} | {"CAMERA-MOVED": 1, "CONFOUNDED": 3, "FAIL": 5} / {"CONFOUNDED": 3, "FAIL": 2, "PASS(transition-excused)": 3} | 0 |
| B9 | M52 | FAIL {"FAIL": 16} | {"FAIL": 7, "PASS": 3, "CENSORED": 6} / {"CENSORED": 8, "FAIL": 8} | {+2:16} / {+0:5} | {"CAMERA-MOVED": 1, "FAIL": 16, "NO-LABELLED-FRAMES": 1} / {"FAIL": 16} | 0 |
| B0L | M52 | FAIL {"CENSORED": 9, "FAIL": 12} | {"FAIL": 21} / {"FAIL": 21} | {-1:3, +0:15, +1:3} / {+2:4, +3:1, +4:1, +5:1} | {"CAMERA-MOVED": 1, "FAIL": 18, "CONFOUNDED": 3, "NO-LABELLED-FRAMES": 1} / {"FAIL": 18, "CONFOUNDED": 3} | 345 |
| P1 | M52 | NO-JUDGEABLE-EVENT {} | {} / {} | {} / {} | {"NO-LABELLED-FRAMES": 5} / {} | 0 |
| CC_ON_NAT | BCC | INVALID ['ON: no IAI-CC-SCHEDULE ARMED line', 'ON: 200 k without a FRAME line (first [0, 1, 2, 3, 4])', 'NULL: no IAI-CC-SCHEDULE ARMED line', 'NULL: 200 k without a FRAME line (first [0, 1, 2, 3, 4])', 'NULL_B: no IAI-CC-SCHEDULE ARMED line', 'NULL_B: 200 k without a FRAME line (first [0, 1, 2, 3, 4])'] | | | | |
| CC_NULL_A_NAT | BCC | null leg: read through its ON pair | | | | |
| CC_NULL_B_NAT | BCC | null leg: read through its ON pair | | | | |
| CC_ON_AAOFF | BCC | INVALID ['ON: no IAI-CC-SCHEDULE ARMED line', 'ON: 200 k without a FRAME line (first [0, 1, 2, 3, 4])', 'NULL: no IAI-CC-SCHEDULE ARMED line', 'NULL: 200 k without a FRAME line (first [0, 1, 2, 3, 4])'] | | | | |
| CC_NULL_AAOFF | BCC | null leg: read through its ON pair | | | | |
| CC_ON_SYN | BCC | INVALID ['ON: no IAI-CC-SCHEDULE ARMED line', 'ON: 200 k without a FRAME line (first [0, 1, 2, 3, 4])', 'NULL: no IAI-CC-SCHEDULE ARMED line', 'NULL: 200 k without a FRAME line (first [0, 1, 2, 3, 4])'] | | | | |
| CC_NULL_SYN | BCC | null leg: read through its ON pair | | | | |
| MASK55 | MASK55 | FAIL {"PASS": 115, "FAIL": 1, "CENSORED": 1} | {"PASS": 115, "FAIL": 1, "CENSORED": 1} / {"PASS": 115, "FAIL": 1, "CENSORED": 1} | {+0:116, +1:1} / {+0:116} | {"WARMUP": 2, "PASS": 113, "PASS-WITH-CENSORED-EDGE": 3, "FAIL": 1} / {"PASS": 113, "PASS-WITH-CENSORED-EDGE": 3, "FAIL": 1} | 128 |
| REAL_BL_R1 | BREAL | PASS {"PASS": 10} | {"FAIL": 6, "PASS": 4} / {"PASS": 10} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 7, "PASS-WITH-CENSORED-EDGE": 3} / {"PASS": 7, "PASS-WITH-CENSORED-EDGE": 3} | 56 |
| REAL_BL_R2 | BREAL | PASS {"PASS": 10} | {"FAIL": 4, "PASS": 6} / {"FAIL": 3, "PASS": 7} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 7, "PASS": 2, "UNRESOLVED": 1} / {"PASS-WITH-CENSORED-EDGE": 7, "PASS": 2, "UNRESOLVED": 1} | 56 |
| REAL_MO_R1 | BREAL | PASS {"PASS": 10} | {"FAIL": 6, "PASS": 4} / {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 6, "PASS": 4} / {"PASS-WITH-CENSORED-EDGE": 6, "PASS": 4} | 0 |
| REAL_MO_R2 | BREAL | PASS {"PASS": 10} | {"PASS": 6, "FAIL": 4} / {"PASS": 8, "FAIL": 2} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 8, "PASS": 2} / {"PASS-WITH-CENSORED-EDGE": 8, "PASS": 2} | 0 |
| REAL_MT_R1 | BREAL | PASS {"PASS": 10} | {"FAIL": 8, "PASS": 2} / {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} / {"PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | 0 |
| REAL_MT_R2 | BREAL | PASS {"PASS": 10} | {"FAIL": 4, "PASS": 6} / {"PASS": 9, "FAIL": 1} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} / {"PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} | 0 |
| REAL_CT_R1 | BREAL | PASS {"PASS": 10} | {"FAIL": 8, "PASS": 2} / {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} / {"PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | 0 |
| REAL_CT_R2 | BREAL | PASS {"PASS": 10} | {"FAIL": 4, "PASS": 6} / {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} / {"PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} | 0 |
| REAL_BL_R1_AAOFF | BREAL | PASS {"PASS": 10} | {"PASS": 5, "FAIL": 5} / {"PASS": 10} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 10} / {"PASS": 10} | 56 |
| REAL_MO_R1_AAOFF | BREAL | PASS {"PASS": 10} | {"PASS": 5, "FAIL": 5} / {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} / {"PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | 0 |
| LOD_NAT | BLOD | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {"NOT-MEASURABLE": 7} / {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} / {"NOT-MEASURABLE": 5, "CONFOUNDED": 2} | 80 |
| LOD_SYN | BLOD | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {"NOT-MEASURABLE": 7} / {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} / {"NOT-MEASURABLE": 5, "CONFOUNDED": 2} | 80 |
| LOD_NAT_AAOFF | BLOD | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {"NOT-MEASURABLE": 5, "FAIL": 2} / {"NOT-MEASURABLE": 5, "FAIL": 1, "PASS": 1} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} / {"UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | 80 |
| LOD_SYN_AAOFF | BLOD | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {"NOT-MEASURABLE": 5, "FAIL": 2} / {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} / {"UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | 80 |
| CCMW_ON | BCCMW | OVER-LABEL-SUSPECT (hollow mesh around the eye) | labelled 200 eye_inside 200 | | | |
| CCMW_NULL_A | BCCMW | null leg: read through its ON pair | | | | |
| CCMW_NULL_B | BCCMW | null leg: read through its ON pair | | | | |
- B0 purity (iii): PASS
- B3 purity (iii): PASS
- B9 purity (iii): PASS
- B0L purity (iii): PASS
- P1 purity (iii): FAIL

## MASK55 mask gate: PASS
- events 119, labelled frames 586, masked 586, G295 0, recycles 64 (log 64), peak live 1, exhausted 0, values reused 55, owner violations 0
- fail reasons: []

## can-fail

- B0L_timing_lever_must_FAIL: {"e1": "FAIL", "proven": true, "e2": {"CAMERA-MOVED": 1, "FAIL": 18, "CONFOUNDED": 3, "NO-LABELLED-FRAMES": 1}}
- P1_purity_lever_must_FAIL: {"observed": "FAIL", "proven": true}

## transition readings (legs under temporal AA)

- B0: {"start_t50": "{+1:13, +2:3}", "end_t50": "{+4:2, +5:2}", "release_verdicts": {"CENSORED": 12, "PASS": 4}, "label_summary": {"label_aa_method": "tsr", "label_aa_method_cvar": 4, "label_temporal_aa": true, "label_transition_on_frames": 3, "label_transition_off_frames": 8, "label_transition_hide_frames": 1, "label_transition_on_frames_cvar": 3, "label_transition_off_frames_cvar": -1, "label_transition_hide_frames_cvar": -1, "label_transition_entries": 187, "label_transition_frames": 187, "label_en
- B3: {"start_t50": "{+2:8}", "end_t50": "{+5:2, +6:1}", "release_verdicts": {"CENSORED": 5, "PASS": 3}, "label_summary": {"label_aa_method": "tsr", "label_aa_method_cvar": 4, "label_temporal_aa": true, "label_transition_on_frames": 3, "label_transition_off_frames": 8, "label_transition_hide_frames": 1, "label_transition_on_frames_cvar": 3, "label_transition_off_frames_cvar": -1, "label_transition_hide_frames_cvar": -1, "label_transition_entries": 99, "label_transition_frames": 99, "label_entries_supp
- B0L: {"start_t50": "{-1:3, +0:15, +1:3}", "end_t50": "{+2:4, +3:1, +4:1, +5:1}", "release_verdicts": {"CENSORED": 9, "FAIL": 12}, "label_summary": {"label_aa_method": "tsr", "label_aa_method_cvar": 4, "label_temporal_aa": true, "label_transition_on_frames": 3, "label_transition_off_frames": 8, "label_transition_hide_frames": 1, "label_transition_on_frames_cvar": 3, "label_transition_off_frames_cvar": -1, "label_transition_hide_frames_cvar": -1, "label_transition_entries": 0, "label_transition_frames"
- P1: {"start_t50": "{}", "end_t50": "{}", "release_verdicts": {}, "label_summary": {"label_aa_method": "tsr", "label_aa_method_cvar": 4, "label_temporal_aa": true, "label_transition_on_frames": 3, "label_transition_off_frames": 8, "label_transition_hide_frames": 1, "label_transition_on_frames_cvar": 3, "label_transition_off_frames_cvar": -1, "label_transition_hide_frames_cvar": -1, "label_transition_entries": 0, "label_transition_frames": 0, "label_entries_suppressed": 259, "label_transition_out_of_o
- MASK55: {"start_t50": "{+0:116, +1:1}", "end_t50": "{+0:116}", "release_verdicts": {"PASS": 115, "FAIL": 1, "CENSORED": 1}, "label_summary": null}
- REAL_BL_R1: {"start_t50": "{+0:20}", "end_t50": "{+0:20}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_BL_R2: {"start_t50": "{+0:20}", "end_t50": "{+0:20}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_MO_R1: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_MO_R2: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_MT_R1: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_MT_R2: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_CT_R1: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- REAL_CT_R2: {"start_t50": "{+0:10}", "end_t50": "{+0:10}", "release_verdicts": {"PASS": 10}, "label_summary": null}
- LOD_NAT: {"start_t50": "{}", "end_t50": "{}", "release_verdicts": {"NOT-MEASURABLE": 7}, "label_summary": null}
- LOD_SYN: {"start_t50": "{}", "end_t50": "{}", "release_verdicts": {"NOT-MEASURABLE": 7}, "label_summary": null}
