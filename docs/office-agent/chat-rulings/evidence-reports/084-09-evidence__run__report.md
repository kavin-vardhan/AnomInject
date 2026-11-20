# 084-09 bench report (live, 2026-09-29T14:15:46.689533+00:00, evaluator 084-09, release threshold t50)

| leg | group | E1 release (t50) | start / end at t50 | E2 086-01 t50 | label-rule | membership | gate |
|---|---|---|---|---|---|---|---|
| B0 | M52 | PASS-WITH-CENSORED {"CENSORED": 12, "PASS": 6} | {+3:18} / {+4:3, +5:2, +7:1} | {"WARMUP": 1, "FAIL": 14, "CONFOUNDED": 3} | NO MISMATCH under the NEW rule | new  |  |
| B5 | M52 |   |  /  |  | NO MISMATCH under the NONE rule | empty  | NULL-PASS |
| B3 | M52 | FAIL {"CENSORED": 4, "FAIL": 1, "PASS": 3} | {+2:8} / {+5:3, +7:1} | {"CAMERA-MOVED": 1, "CONFOUNDED": 3, "FAIL": 5} | NO MISMATCH under the NEW rule | new  |  |
| B9 | M52 | PASS {"PASS": 16} | {+3:16} / {+0:16} | {"CAMERA-MOVED": 1, "FAIL": 16, "NO-LABELLED-FRAMES": 1} | NO MISMATCH under the NEW rule | new  |  |
| B0L | M52 | FAIL {"FAIL": 15, "CENSORED": 6} | {-1:4, +0:14, +1:3} / {+2:6, +3:1, +4:2, +5:1} | {"CAMERA-MOVED": 1, "FAIL": 19, "CONFOUNDED": 2, "NO-LABELLED-FRAMES": 1} | NO MISMATCH under the NEW rule | new  |  |
| P1 | M52 | NO-JUDGEABLE-EVENT {} | {} / {} | {"NO-LABELLED-FRAMES": 5} | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_NAT | BCC |   |  /  | {"CAMERA-MOVED": 1} | NO MISMATCH under the NEW rule | new  | PASS |
| CC_NULL_A_NAT | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_NULL_B_NAT | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_AAOFF | BCC |   |  /  | {"CAMERA-MOVED": 1} | NO MISMATCH under the NEW rule | new  | UNJUDGEABLE |
| CC_NULL_AAOFF | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_SYN | BCC |   |  /  | {"CAMERA-MOVED": 1} | NO MISMATCH under the NEW rule | new  | PASS |
| CC_NULL_SYN | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| MASK55V2 | MASK55V2 | PASS {"PASS": 34, "NOT-MEASURABLE": 21} | {+0:33, +2:1} / {+0:32, +1:2} | {"WARMUP": 2, "CAMERA-MOVED": 4, "PASS": 55, "NOT-MEASURABLE": 23, "PASS-WITH-CENSORED-EDGE": 8, "UNRESOLVED": 1, "FAIL": 1} | NO MISMATCH under the NEW rule | new  | INVALID |
| MT85_NAT | MT85 | PASS {"PASS": 12} | {+0:12} / {+0:12} | {"WARMUP": 2, "PASS-WITH-CENSORED-EDGE": 6, "PASS": 6} | NO MISMATCH under the NEW rule | new  |  |
| MT85_SYN | MT85 | PASS {"PASS": 12} | {+0:12} / {+0:12} | {"WARMUP": 2, "PASS": 4, "PASS-WITH-CENSORED-EDGE": 8} | NO MISMATCH under the NEW rule | new  |  |
| REAL_BL_NAT | BREAL | PASS {"PASS": 10} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 7, "PASS-WITH-CENSORED-EDGE": 3} | NO MISMATCH under the NEW rule | new  |  |
| REAL_BL_SYN | BREAL | FAIL {"FAIL": 10} | {+0:20} / {+0:10, +1:5, +14:5} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 8, "PASS-WITH-CENSORED-EDGE": 2} | NO MISMATCH under the NEW rule | new  |  |
| REAL_MO_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 6, "PASS": 4} | NO MISMATCH under the NEW rule | new  |  |
| REAL_MO_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 6, "PASS": 4} | NO MISMATCH under the NEW rule | new  |  |
| REAL_MT_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the NEW rule | new  |  |
| REAL_MT_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the NEW rule | new  |  |
| REAL_CT_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the NEW rule | new  |  |
| REAL_CT_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the NEW rule | new  |  |
| LODP_R1 | BLOD | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the NEW rule | new  | PREMISE-FAIL |
| LODP_R2 | BLOD | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 6, "CONFOUNDED": 1} | NO MISMATCH under the NEW rule | new  | PREMISE-FAIL |
| LOD_R1_NAT | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_SYN | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_NAT_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_SYN_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_NAT | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_SYN | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_NAT_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_SYN_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LODP_FN | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 9} | {} / {} | {"WARMUP": 1, "NOT-MEASURABLE": 9} | NO MISMATCH under the NEW rule | new  | PREMISE-FAIL |
| LODP_FD | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 9} | {} / {} | {"WARMUP": 1, "NOT-MEASURABLE": 9} | NO MISMATCH under the NEW rule | new  | PREMISE-FAIL |
| LOD_FN_NAT | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FN_SYN | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FN_NAT_AAOFF | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FN_SYN_AAOFF | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FD_NAT | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FD_SYN | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FD_NAT_AAOFF | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_FD_SYN_AAOFF | BLODF |   |  /  |  |  |   | NOT-RUN-PREMISE |
| CCMW_ON | BCCMW |   |  /  |  | NO MISMATCH under the NEW rule | new  | PASS |
| CCMW_NULL_A | BCCMW |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CCMW_NULL_B | BCCMW |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |

## can-fail

- B0L_timing_lever_must_FAIL: {"e1": "FAIL", "proven": true}
- P1_purity_lever_must_FAIL: {"observed": "FAIL", "proven": true}
- CC_ON_NAT_sphere_proxy_must_disagree: {"proven": true}
- CC_ON_AAOFF_sphere_proxy_must_disagree: {"proven": false}
- CC_ON_SYN_sphere_proxy_must_disagree: {"proven": true}

## B0 partial-flag guard
- counts {"all": {"partial": 56, "confirmed_partial": 31, "flagged_invisible": 25, "flagged_invisible_counted": 25, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "warmup": {"partial": 3, "confirmed_partial": 3, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "steady": {"partial": 53, "confirmed_partial": 28, "flagged_invisible": 25, "flagged_invisible_counted": 25, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}}
- edges {"all": {"onset": "{3:18}", "offset": "{0:16, 1:2}", "mid": "{0:18}"}, "warmup": {"onset": "{3:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{3:17}", "offset": "{0:15, 1:2}", "mid": "{0:17}"}}
- over 8: []
- guard fails: []

## B3 partial-flag guard
- counts {"all": {"partial": 21, "confirmed_partial": 10, "flagged_invisible": 11, "flagged_invisible_counted": 11, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "warmup": {"partial": 5, "confirmed_partial": 5, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "steady": {"partial": 16, "confirmed_partial": 5, "flagged_invisible": 11, "flagged_invisible_counted": 11, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}}
- edges {"all": {"onset": "{2:8, 5:1}", "offset": "{0:9}", "mid": "{0:9}"}, "warmup": {"onset": "{5:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{2:8}", "offset": "{0:8}", "mid": "{0:8}"}}
- over 8: []
- guard fails: []

## B9 partial-flag guard
- counts {"all": {"partial": 53, "confirmed_partial": 53, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "warmup": {"partial": 5, "confirmed_partial": 5, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "steady": {"partial": 48, "confirmed_partial": 48, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}}
- edges {"all": {"onset": "{3:16, 4:1}", "offset": "{0:16, 1:1}", "mid": "{0:17}"}, "warmup": {"onset": "{4:1}", "offset": "{1:1}", "mid": "{0:1}"}, "steady": {"onset": "{3:16}", "offset": "{0:16}", "mid": "{0:16}"}}
- over 8: []
- guard fails: []

## B0L partial-flag guard
- counts {"all": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "warmup": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "steady": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}}
- edges {"all": {"onset": "{0:22}", "offset": "{0:22}", "mid": "{0:22}"}, "warmup": {"onset": "{0:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{0:21}", "offset": "{0:21}", "mid": "{0:21}"}}
- over 8: []
- guard fails: []

## P1 partial-flag guard
- counts {"all": {}, "warmup": {}, "steady": {}}
- edges {"all": {"onset": "{}", "offset": "{}", "mid": "{}"}, "warmup": {"onset": "{}", "offset": "{}", "mid": "{}"}, "steady": {"onset": "{}", "offset": "{}", "mid": "{}"}}
- over 8: []
- guard fails: []

## MASK55V2 mask gate: INVALID
- events 94, masked 172/442, peak live 1, recycles 25, TAG-OWNERS lines 1200, shared frames [], prior collisions 0
- fail ['270 labelled frame(s) without a measured mask (G295-signature frames counted apart: 0)'] invalid ['mask_tag_peak_live 1 < 2: no two mask records were ever live together, so the concurrent path was NOT exercised (not a pass)']

## LOD fixture (084-08b): gate and fade at the pop edges

| condition | D gate | D judged / off | D fade edges | N gate | N judged / off | N fade edges | D rate > N rate |
|---|---|---|---|---|---|---|---|
| NAT |  |  /  |  of  |  |  /  |  of  |  |
| SYN |  |  /  |  of  |  |  /  |  of  |  |
| NAT_AAOFF |  |  /  |  of  |  |  /  |  of  |  |
| SYN_AAOFF |  |  /  |  of  |  |  /  |  of  |  |

- variant D fade FINDINGS (never excused): []
- premises: {"LODP_R1": {"pass_": false, "eligible": 7, "measurable": 2, "fraction": 0.286}, "LODP_R2": {"pass_": false, "eligible": 7, "measurable": 0, "fraction": 0.0}, "LODP_FN": {"pass_": false, "eligible": 9, "measurable": 0, "fraction": 0.0}, "LODP_FD": {"pass_": false, "eligible": 9, "measurable": 0, "fraction": 0.0}}
- rule: 084-08b: at each pop edge of a judged event (onset window a-3..mid, offset window mid+1..b+9, clipped to the run window), a frame is INTERMEDIATE when its per-frame effect level (the E1 frac: 0 = the unforced reference, 1 = the run peak) is in [0.10, 0.90); an edge with >= 2 intermediate frames is a MULTI-FRAME FADE. For variant D it is a FINDING, reported and never excused (084-08 ruling 1); for variant N it is the control reading. Transition flags do not remove a frame from this count.

reference amendment: 084-09 reference amendment, declared 2026-09-29 before the MASK55 re-check (084-06 ruling 5): an onset reference uses only clean frames at least PREV_SETTLE=4 frames after the previous event last labelled or transition frame; if fewer than REF_MIN=4 remain, the post-event clean frames (after the event last labelled or transition frame + POST_SKIP=2, up to 6, before the next event) are added; if still fewer than 4 the event is CONFOUNDED-REFERENCE, counted per type, never dropped. Applied to every type.
