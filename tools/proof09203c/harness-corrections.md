The first positive run is preserved as FAIL. PreFrames=120 produced onset at image 119, within
the frozen 0..119 null window. No reader threshold was changed. Positive PIE/staged launchers
now request preFrames=121; this schedules onset after all 120 null images. Reference 100, every
edge, control/revert null+0.05 and the change test remain byte-identical in check_positive.py.

The queued Auto run had already launched when the first result was inspected. Its original
plan remains in positive-pie-auto-01. It will be evaluated and repeated with corrected setup.

Repository label_rule_check originally recognized only ordinary proxy for FireWindow reasons.
Commit 74e2c53 admits the new proxy_vt route to that same rule. Nine added cases cover valid
settle/interruption and invalid staged, late, missing and unknown-route flags. All 74 selftests pass.
The label/capture producer and strengthened annotation equality checker are unchanged.

No E1 reader or selection threshold changed. The initial ten events independently passed
strict/t10/t50 with zero onset/offset difference and zero unexcused frames, but the contaminated
null still prevents calling that first capture an overall positive pass.
