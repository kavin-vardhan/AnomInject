# 084-09 bench report (dry, 2026-09-29T00:32:40.609758+00:00, evaluator 084-09, release threshold t50)

| leg | group | E1 release (t50) | start / end at t50 | E2 086-01 t50 | label-rule | membership | gate |
|---|---|---|---|---|---|---|---|
| B0 | M52 | PASS {"PASS": 16} | {+1:12, +2:4} / {+4:1, +5:10, +6:5} | {"CAMERA-MOVED": 1, "CONFOUNDED": 3, "FAIL": 4, "PASS-WITH-CENSORED-EDGE": 6, "UNRESOLVED": 3, "NO-LABELLED-FRAMES": 1} | NO MISMATCH under the OLD rule | old  |  |
| B5 | M52 |   |  /  |  | NO MISMATCH under the NONE rule | empty  | NULL-PASS |
| B3 | M52 | FAIL {"CENSORED": 4, "FAIL": 1, "PASS": 3} | {+2:8} / {+5:3, +7:1} | {"CAMERA-MOVED": 1, "CONFOUNDED": 3, "FAIL": 5} | NO MISMATCH under the OLD rule | old  |  |
| B9 | M52 | FAIL {"FAIL": 16} | {+2:16} / {+0:16} | {"CAMERA-MOVED": 1, "FAIL": 16, "NO-LABELLED-FRAMES": 1} | NO MISMATCH under the OLD rule | old  |  |
| B0L | M52 | FAIL {"FAIL": 15, "CENSORED": 6} | {-1:3, +0:15, +1:2, +2:1} / {+2:3, +3:4, +4:1, +5:3} | {"CAMERA-MOVED": 1, "FAIL": 18, "CONFOUNDED": 3, "NO-LABELLED-FRAMES": 1} | NO MISMATCH under the OLD rule | old  |  |
| P1 | M52 | NO-JUDGEABLE-EVENT {} | {} / {} | {"NO-LABELLED-FRAMES": 5} | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_NAT | BCC |   |  /  | {} | NO MISMATCH under the NONE rule | empty  | INVALID |
| CC_NULL_A_NAT | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_NULL_B_NAT | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_AAOFF | BCC |   |  /  | {} | NO MISMATCH under the NONE rule | empty  | INVALID |
| CC_NULL_AAOFF | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CC_ON_SYN | BCC |   |  /  | {} | NO MISMATCH under the NONE rule | empty  | INVALID |
| CC_NULL_SYN | BCC |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| MASK55V2 | MASK55V2 | PASS {"PASS": 32} | {+0:32} / {+0:32} | {"WARMUP": 2, "PASS": 113, "PASS-WITH-CENSORED-EDGE": 3, "FAIL": 1} | NO MISMATCH under the OLD rule | old  | INVALID |
| MT85_NAT | MT85 | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the OLD rule | old  |  |
| MT85_SYN | MT85 | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} | NO MISMATCH under the OLD rule | old  |  |
| REAL_BL_NAT | BREAL | PASS {"PASS": 10} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 7, "PASS-WITH-CENSORED-EDGE": 3} | NO MISMATCH under the OLD rule | old  |  |
| REAL_BL_SYN | BREAL | PASS {"PASS": 10} | {+0:20} / {+0:20} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 7, "PASS": 2, "UNRESOLVED": 1} | NO MISMATCH under the OLD rule | old  |  |
| REAL_MO_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 6, "PASS": 4} | NO MISMATCH under the OLD rule | old  |  |
| REAL_MO_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 8, "PASS": 2} | NO MISMATCH under the OLD rule | old  |  |
| REAL_MT_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the OLD rule | old  |  |
| REAL_MT_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} | NO MISMATCH under the OLD rule | old  |  |
| REAL_CT_NAT | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS": 5, "PASS-WITH-CENSORED-EDGE": 5} | NO MISMATCH under the OLD rule | old  |  |
| REAL_CT_SYN | BREAL | PASS {"PASS": 10} | {+0:10} / {+0:10} | {"WARMUP": 2, "CAMERA-MOVED": 2, "PASS-WITH-CENSORED-EDGE": 5, "PASS": 5} | NO MISMATCH under the OLD rule | old  |  |
| LODP_R1 | BLOD | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | PREMISE-FAIL |
| LODP_R2 | BLOD | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | PREMISE-FAIL |
| LOD_R1_NAT | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_SYN | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_NAT_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R1_SYN_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_NAT | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_SYN | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_NAT_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| LOD_R2_SYN_AAOFF | BLOD |   |  /  |  |  |   | NOT-RUN-PREMISE |
| CCMW_ON | BCCMW |   |  /  |  | NO MISMATCH under the OLD rule | old  | FAIL |
| CCMW_NULL_A | BCCMW |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |
| CCMW_NULL_B | BCCMW |   |  /  |  | NO MISMATCH under the NONE rule | empty  |  |

## can-fail

- B0L_timing_lever_must_FAIL: {"e1": "FAIL", "proven": true}
- P1_purity_lever_must_FAIL: {"observed": "FAIL", "proven": true}

## B0 partial-flag guard
- counts {"all": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 187}, "warmup": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 11}, "steady": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 176}}
- edges {"all": {"onset": "{0:17}", "offset": "{0:17}", "mid": "{0:17}"}, "warmup": {"onset": "{0:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{0:16}", "offset": "{0:16}", "mid": "{0:16}"}}
- over 8: []
- guard fails: []

## B3 partial-flag guard
- counts {"all": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 98}, "warmup": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 11}, "steady": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 87}}
- edges {"all": {"onset": "{0:9}", "offset": "{0:9}", "mid": "{0:9}"}, "warmup": {"onset": "{0:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{0:8}", "offset": "{0:8}", "mid": "{0:8}"}}
- over 8: []
- guard fails: []

## B9 partial-flag guard
- counts {"all": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "warmup": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}, "steady": {"partial": 0, "confirmed_partial": 0, "flagged_invisible": 0, "flagged_invisible_counted": 0, "partial_without_held_texture": 0, "partial_on_unlabelled": 0, "unresolved": 0, "unresolved_only_off": 0, "legacy_transition": 0}}
- edges {"all": {"onset": "{0:17}", "offset": "{0:17}", "mid": "{0:17}"}, "warmup": {"onset": "{0:1}", "offset": "{0:1}", "mid": "{0:1}"}, "steady": {"onset": "{0:16}", "offset": "{0:16}", "mid": "{0:16}"}}
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
- events 119, masked 586/586, peak live 1, recycles 64, TAG-OWNERS lines 1200, shared frames [], prior collisions None
- fail [] invalid ['mask_tag_peak_live 1 < 2: no two mask records were ever live together, so the concurrent path was NOT exercised (not a pass)']

reference amendment: 084-09 reference amendment, declared 2026-09-29 before the MASK55 re-check (084-06 ruling 5): an onset reference uses only clean frames at least PREV_SETTLE=4 frames after the previous event last labelled or transition frame; if fewer than REF_MIN=4 remain, the post-event clean frames (after the event last labelled or transition frame + POST_SKIP=2, up to 6, before the next event) are added; if still fewer than 4 the event is CONFOUNDED-REFERENCE, counted per type, never dropped. Applied to every type.
