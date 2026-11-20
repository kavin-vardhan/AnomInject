# 084-09 bench report (dry, 2026-09-29T02:19:41.682665+00:00, evaluator 084-09, release threshold t50)

| leg | group | E1 release (t50) | start / end at t50 | E2 086-01 t50 | label-rule | membership | gate |
|---|---|---|---|---|---|---|---|
| LODP_FN | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | PREMISE-PASS |
| LODP_FD | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | PREMISE-PASS |
| LOD_FN_NAT | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FN_SYN | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FN_NAT_AAOFF | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FN_SYN_AAOFF | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FD_NAT | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FD_SYN | BLODF | NO-JUDGEABLE-EVENT {"NOT-MEASURABLE": 7} | {} / {} | {"WARMUP": 1, "CAMERA-MOVED": 2, "NOT-MEASURABLE": 5, "CONFOUNDED": 2} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FD_NAT_AAOFF | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |
| LOD_FD_SYN_AAOFF | BLODF | PASS {"NOT-MEASURABLE": 5, "PASS": 2} | {+0:2} / {+0:2} | {"WARMUP": 1, "CAMERA-MOVED": 2, "UNRESOLVED": 1, "CONFOUNDED": 2, "NOT-MEASURABLE": 2, "PASS-WITH-CENSORED-EDGE": 1, "PASS": 1} | NO MISMATCH under the OLD rule | old  | INSUFFICIENT |

## can-fail


## LOD fixture (084-08b): gate and fade at the pop edges

| condition | D gate | D judged / off | D fade edges | N gate | N judged / off | N fade edges | D rate > N rate |
|---|---|---|---|---|---|---|---|
| NAT | INSUFFICIENT | 0 / 0 | 0 of 0 | INSUFFICIENT | 0 / 0 | 0 of 0 | False |
| SYN | INSUFFICIENT | 0 / 0 | 0 of 0 | INSUFFICIENT | 0 / 0 | 0 of 0 | False |
| NAT_AAOFF | INSUFFICIENT | 2 / 0 | 0 of 4 | INSUFFICIENT | 2 / 0 | 0 of 4 | False |
| SYN_AAOFF | INSUFFICIENT | 2 / 0 | 0 of 4 | INSUFFICIENT | 2 / 0 | 0 of 4 | False |

- variant D fade FINDINGS (never excused): []
- premises: {"LODP_FN": {"pass_": true, "eligible": 7, "measurable": 2, "fraction": 0.286, "forced": true}, "LODP_FD": {"pass_": true, "eligible": 7, "measurable": 2, "fraction": 0.286, "forced": true}}
- rule: 084-08b: at each pop edge of a judged event (onset window a-3..mid, offset window mid+1..b+9, clipped to the run window), a frame is INTERMEDIATE when its per-frame effect level (the E1 frac: 0 = the unforced reference, 1 = the run peak) is in [0.10, 0.90); an edge with >= 2 intermediate frames is a MULTI-FRAME FADE. For variant D it is a FINDING, reported and never excused (084-08 ruling 1); for variant N it is the control reading. Transition flags do not remove a frame from this count.

reference amendment: 084-09 reference amendment, declared 2026-09-29 before the MASK55 re-check (084-06 ruling 5): an onset reference uses only clean frames at least PREV_SETTLE=4 frames after the previous event last labelled or transition frame; if fewer than REF_MIN=4 remain, the post-event clean frames (after the event last labelled or transition frame + POST_SKIP=2, up to 6, before the next event) are added; if still fewer than 4 the event is CONFOUNDED-REFERENCE, counted per type, never dropped. Applied to every type.
