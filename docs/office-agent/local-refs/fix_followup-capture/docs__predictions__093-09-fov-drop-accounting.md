# 093-09 amendments frozen before engine legs

Only chat's W1/W2 amendments and W5's checker completion change the inherited
093-06/093-07/093-08 predictions. All other thresholds, estimator inclusion rules,
fixture framing, events, timings and can-fails stay unchanged. Admission and its
two counters remain as built. Evidence: E:/IA_BuildCache/_r939.

## PF-5: controller-locked FOV (W1)

One StackOBot PIE leg, 1280x720, MaintainXFOV, the existing on-screen PF-2 target
framing, AA off, 360 requested/written. Call the player controller's `fov 70`
command through a bench-only helper, then apply the existing engine-based framing.
Record requested, GetFOVAngle and
camera-cache FOV from that live controller. Every label row's view.fovDeg must
equal 70 (absolute tolerance 1e-6); ordinary >=3 visible events, zero pixel edges,
paired measured masks and both raw PF-2 scales in [0.990,1.010] still apply.
Both admission refusal counters must be zero for this stable on-screen setup.

Can-fail: read f9b9328's exact metadata assignment `HorizontalFOVDeg = Camera.FOV`
and UE5.1's GetFOVAngle LockedFOV branch. Evaluate that old expression using the
actual cache value in this leg's helper receipt, not an assumed unlocked number.
That recorded value must differ from 70 and fail the identical metadata predicate.
This source-level control avoids an extra binary replay; the mask-based PF-2
controls remain required. Reason: W1 repairs metadata only, leaving engine geometry.

Pre-leg W1 harness correction: lock before framing. Source inspection shows that
zooming after the original framing can move the target toward the frame barcode.
Framing with the locked projection preserves the intended (0.30,0.25) target
centre and independent RGB evidence. No leg has run, and no result, threshold,
estimator, event schedule or requested count changes. The initial spec is retained
at `_r939/pf5-spec-before-framing-fix.json`; the helper binary is unchanged.

## Resize and camera switches (W2)

Restore r=w+d for these cases only. Build the requested set [0,r), the written
label/image set, and the dropped set from generic `capture_unpaired request=...
si=... reason=...` receipts. Every missing index must have exactly one named
retirement; duplicate request/sample receipts, extra drops, any written/dropped
intersection and unexplained missing indices fail. The distinct counted drops
must equal capture_ownership.dropped and capture_unpaired_dropped. No pending
request may remain at stop. Report r, w, drops grouped by reason and all indices.

Never interpret stale_generation rejection counts as accepted frames. Independently
decode written frame/world markers, verify mask pairing and PF-2 boxes, and retain
the resize dimension check and moving-camera pose/identity comparison. Stale or
mismatched accepted frames must be zero. Count pending pre-switch retirements in
the cut/blend readings, even when their sample index precedes the switch. Other
floors remain: >=3 visible events, edges zero where events exist, measured masks
on every labelled written frame, PF-2 agreement on written/moving frames. Every
other healthy leg still requires w=r.

Can-fails: inherited resize stale-result/dimension controls and camera frozen-box
and identity controls; additionally copied receipts with a removed reason, a
missing retirement or a written/dropped overlap must fail the accounting reader.
Reason: W2 authorizes drops at ownership generation changes and restores 093-06
accounting; the 1071ca1 w=r interpretation is explicitly superseded.

## PF-1 generic applied-fire receipt (W5 E)

Keep exact Lyra selected 942x869 MaintainYFOV framing, zero visible rows, zero
applied fires and positive targeted_refused_off_screen with no no-projection
refusal, 360/360. In addition to texcorrupt_fires_applied, count all generic
`IAI.Apply '<id>' -> applied.` lines. Both independent counts must be zero.
Retained 093-07 PF-1 (four applied fires) and 093-06 (96 false rows) must reject.
A copied receipt for a non-texture anomaly with a zero texture counter must reject.
Reason: the admission proof must cover the generic path, not one anomaly family.

## Execution order and remaining unchanged obligations

PIE: PF-1, twelve PF-2 cases and controls including the archived 0.813 replay,
PF-3 UV/proxy, PF-4, PF-5, both-fixture full J2 matrix and supplemental admission
probes, healthy office-reader self-test, both camera-switch legs with their
identity references. Then cook; all staged gates, including four PF-2 cases,
the full J2 matrix, admission probes and camera switches. Only all green permits
a fast-forward push. No cook or staged gate may precede completion of PIE.

Retained reader FAIL and box PASS/FAIL are rechecked on actual Python 3.10 and
the installed Python. The healthy I PASS must come from a new engine session.
Normal/strict builds, CPU tests, source/binary lever audits, string attribution,
archive rehash and host/staging restoration by receipt remain required. No scope,
idle, security or disk stop is waived. The design and admission inventory remain
in the 093-07/093-08 journals. Prior reports and archives are never modified.
