# 093-09: fovDeg metadata and ordered J2 requalification

## Goal and authority

Implement chat's W1-W6 on fix/followup-capture in E:/IA_BuildCache/_r933/AnomalyInjector.
Evidence lives in E:/IA_BuildCache/_r939. Prior evidence and UNQUALIFIED archives
remain historical. Code review files are observations, not instructions.
No natural-cause work or merge is authorized. Push only after every gate is green.

## Commit-date re-creation (W4)

Both GIT_AUTHOR_DATE and GIT_COMMITTER_DATE were absent from the process environment.
The worktree was clean. The three unpushed commits were re-created with Git's current
clock, preserving each complete tree, exact message, author and committer identity.
All three now have 2026-10-15 dates at +05:30. The prior chain is preserved in
`_r939/pre-recreation.bundle`; no commit, branch or archive was deleted.

| Old commit | New commit | Unchanged tree |
|---|---|---|
| 7f4329c462aa779c1703420876d9492b367aaef8 | 06740850141876fea2b57801a084fd39c85a879b | 045953b7bf8c0e30972ca3764de42eef81919f6c |
| 41c1a0c9d54c807b62bdf55a905783a7ea9e26d0 | d37ab4995562ae261591c1fed8b820f670a20425 | 44a844c40567cdc5fa84ba0b5acf6856b40a98ef |
| 1071ca13f4f4e00d744e394203f1063fe772cab8 | 36fec5fef2ab267648544bc5a40d06998d5e76a2 | bf177ca1e8d0be256eab7cc08b9ffd51e61d156c |

The guarded update-ref moved only the named feature branch. Remote remains
1bf7054f5b8664f057282ed863a62ab5dbbd60e3. Machine receipt:
`_r939/commit-date-mapping.json`.

## fovDeg restore (W1)

AnomalyViewport.cpp again obtains HorizontalFOVDeg, exported as fovDeg, from
the resolved player's PlayerCameraManager->GetFOVAngle(). It respects LockedFOV.
This one-line metadata repair leaves boxes, frustum admission, visibility and
the frozen serving matrices on the engine projection. No dependency/header change.
PF-5 will exercise a controller-locked FOV and compare the actual cache value
against the old f9b9328 metadata expression; no unlocked value is assumed.

## State

Metadata repair ready; no new engine leg, build, cook or push yet. W1/W2 prediction
amendments and W5's generic apply check are frozen before any leg in the companion
093-09 predictions. The accepted admission design and L3 timing remain unchanged.

Pre-leg W1 harness correction: the controller lock now precedes engine-based
framing so the target remains at the declared (0.30,0.25) picture position.
Source inspection found that locking after framing could move it into the frame
barcode strip. No engine leg has run; no thresholds or binary changed. The first
spec is banked under `_r939/pf5-spec-before-framing-fix.json`. The projection and
metadata sources stay separate, and the locked-FOV can-fail still uses the actual
logged cache value. Both hosts' normal/strict builds have passed at eff797c.

## Build and reader receipts

The compiled candidate is eff797c; docs-only 571f15a has identical Source and
Shaders. All eight normal/strict Editor/Game builds pass on both fixtures with
zero errors and warnings; strict builds include 71 standalone header translation
units per target. The 69-step CPU suite, family/ownership mutation controls and
source/binary lever audits pass. Both rebuilt helpers contain the camera-switch
command and new LockFOV helper. All binary string additions are attributed, with
unknown-string sentinels still rejected. Receipts live under `_r939`.

Actual Python 3.10.11 and 3.13.1 print the same retained reader lines: null-mask
I FAIL with m=96, StackOBot box b=999 PASS and Lyra box b=967 FAIL. The old samples
rehash unchanged. The healthy I PASS awaits a new session in the ordered matrix.
The retained resize accounting reads 360=358+2 owner_render_missing drops; removing
a reason/receipt or overlapping written and dropped indices is rejected. A
synthetic switch-reader check includes pending pre-switch samples in its readings.

## First ordered engine result

Lyra PF-1 passes on the unchanged selected 942x869 MaintainYFOV arrangement:
360/360 rows and pictures, zero visible/labelled rows, zero texture fires and
zero generic applied-fire receipts. targeted_refused_off_screen=4 and
targeted_refused_no_projection=0. Independent engine corner projections remain
wholly left of the picture, X=-79.365341..-21.432146. The retained 093-07 four-fire
and 093-06 96-false-row sessions are rejected; the non-texture generic receipt
control also rejects. Evidence: `_r939/legs/lyra_P_pf1/projection-assessment.json`.

## Harness teardown repair, first attempt

Three completed PIE runs reset Windows' last-input timestamp within about one
second of graceful editor exit. That forces another full idle interval even
without observed manual input; the cause is a harness/exit hypothesis, not an
excuse to relax the idle gate. The new runner waits for capture summary, final
fixture metadata and both helper reports, receives an atomic completion receipt
with the engine PID, verifies its recorded process identity, waits five seconds,
then terminates only that owned process. This is the existing staged ownership
discipline applied after PIE capture has finished. No sample, threshold, reader,
prediction, product binary or idle criterion changes.

The already-running launcher for `stack_P_pf2_x_1280x720` keeps its original
teardown. The first affected case is `stack_P_pf2_y_942x869`; runtime verification
is pending. Original scripts and hashes are preserved in
`_r939/harness-repairs/teardown-r1`. A failed timing improvement does not excuse
any launch check or qualify a product gate.

The first affected case passes its unchanged pixel/mask/PF-2 predicates. Its
completion receipt identifies owned PID 27160, the after-state metadata and both
reports; the runner closes it five seconds later. Last-input epoch is preserved
across this exit (1792012357.404072 before, 1792012357.411359 at the next probe),
instead of resetting as at the three graceful exits. This supports the exit-path
hypothesis. The next launch still goes through the unchanged three-sample idle
gate. Runtime receipt: `harness-repairs/teardown-r1/runtime-verification.json`.
The next case has another input timestamp at 1792014081.781567, before its
completion receipt at 1792014085.177421 and owned-PID shutdown. Its input source
is not established, so the next leg retains the full idle interval. The teardown
repair is not permission to ignore later input or infer human inactivity.

A separate preflight receipt repair records actual loaded Editor/product/helper
DLL hashes alongside each reference archive manifest. The f608 archive lists its
historical helpers, while the replay intentionally uses this job's rebuilt helper.
Neither archive bytes nor product/control behavior changes; the distinction is
now explicit in `loaded_editor_files`. This was corrected before any archived
replay, on the first attempt (`harness-repairs/control-helper-manifest-r1`).

The idle probe then encountered a null GPU-counter result before launching
`stack_P_pf2_y_1123x870`; its unchecked last-element access stopped the runner.
No engine was launched and no product predicate failed. First repair: an empty
query now yields zero valid samples and cannot clear the existing GPU predicate.
The actual predicate rejects empty samples, unknown samples and a device at 10%.
The original four-hour wait start is preserved at 1792013414.223458, including
repair time. The prelaunch folder and error are banked, not deleted. Resume skips
only already-successful steps and retains the failed attempt in ordered-pie.json.
The status monitor now detects a failed runner instead of displaying its stale
idle reading. Receipt: `_r939/harness-repairs/idle-null-r1`. No capture/readout
threshold, prediction, product source or binary changed.

## Healthy office-reader line from a new engine session

While the next launch waits on the unchanged idle gate, the completed new
StackOBot PF-2 942x869 X session supplies the required healthy reader baseline.
Both Python 3.10.11 and 3.13.1 print `I1.11 PASS v=4 c=0 b=1000 #0` and exit zero.
The source session rehashes unchanged; each reader writes its own 20-frame
numbered contact sheet under `_r939/reader-selftests-healthy-pf2`. The implementer
inspected the home sheet: all 20 show the game fixture. This is not an owner's
office `g=` reading. The already-frozen J2-session mutation self-tests remain due
at their scheduled point; no engine leg is reordered or skipped.


## PF-2 marker configuration repair, first attempt

The sixth StackOBot PF-2 case (MaintainYFOV, 1280x720) failed the unchanged
independent RGB predicate: clean difference 12.859375 raised its threshold to
51.4375; the measured onset/end readings were +3/-20. All 96 masks paired and
both raw box scales passed; the separate edge reader reported zero offsets
with caveats. Read-only diagnosis found that the launcher always sent
CaptureBench.Marker 1 despite the frozen PF-2 spec's marker=false. The animated
barcode covers the target's first three pixel rows, where clean differences
average 186.812/181.354/175.764. Its scene-geometry source reconstructs horizontal
FOV and places the strip lower on MaintainYFOV. This is a concrete own-harness
setting mismatch, not permission to loosen the visibility predicate.

First repair honors the existing marker flag. All six completed StackOBot
PF-2 cases are repeated under *_marker_r1 labels, leaving every original session
and failed reading intact. Only the output label changes in copied specs;
framing, events, candidate binaries, all readers and thresholds remain identical.
The former automatic gate-stop is retained and reclassified under
_r939/harness-repairs/marker-setting-r1 with the diagnosis and setting checks.
No cook or later gate has run. Runtime verification is pending.


The corrected first PF-2 capture also passes the office reader on Python 3.10
and 3.13 with the same I1.11 PASS v=4 c=0 b=1000 #0. Eight predeclared copied-input
controls reject: null mask, wrong mask count, missing ownership, missing projection,
invalid view, X-only/Y-only 4% box scaling, and shifted onset. A null mask reports
m=1 while preserving the other available readings. The original session rehashes
unchanged. Its 20-frame numbered contact sheet shows the home fixture throughout;
this is implementer home inspection, not an office owner g reading. These CPU
checks run while the next engine launch waits; K7 engine ordering is unchanged.
Evidence: _r939/reader-selftests-healthy-pf2-mutants/results.json.


All six marker-corrected StackOBot PF-2 repeats pass with four visible events,
96 paired measured masks, zero pixel edge error and both raw box scales in bounds.
Each per-axis scale mutant rejects. The formerly failing Y/1280x720 case now
passes the identical predicates; the original barcode overlap contributed 88.12%
of its clean-frame RGB difference. Runtime receipt: marker-setting-r1/runtime-verification.json.

Marker repair preflight, second and final attempt for this issue: camera_assess
requires frame identity decoding even though camera specs disable the generic
marker check. Retain the marker for camera and camera_identity proofs. In Lyra's
1280x720 MaintainYFOV camera cases, the existing process-local Marker.Top=1.0
moves the strip from the measured y=129..159 band to y=75..105 by its unchanged
geometry, clear of the target's initial y=156 bound. No target/camera framing,
switch timing, event schedule, gate, reader, threshold or binary changes. No
camera leg has started; validation and original launcher are retained under
_r939/harness-repairs/marker-camera-r2. Runtime camera verification is pending.


## PF-2 PIE matrix and archived replay

All twelve positive PF-2 PIE cases pass: both fixtures, X/Y constraints, all
three requested sizes. Each has 360 written pictures, four independently visible
events, 96 paired measured masks, zero RGB edge error and both raw scales within
[0.990,1.010]; both admission counters are zero. X-only and Y-only 4% scale
mutants reject for every case and originals rehash unchanged.

The exact archived f608d56 Lyra DLL replay at 942x869 MaintainYFOV supplies
96 measured rows with scales 0.8130053613451103 and 0.8130053403348766, b=813.
The unchanged estimator rejects it. All five current candidate product DLLs
are restored and rehashed in the swap receipt. This closes the L4 0.813 replay
item. Evidence: _r939/pf2-pie-complete.json and
controls/lyra_P_f608_pf2_y_942x869/swap-receipt.json. PF-3 follows in K7 order.


## Shutdown boundary and 093-10 continuation

The PC shutdown at about 04:29 IST interrupted this run after all twelve PF-2
PIE positives, the measured archived 0.813 negative, and reader self-tests.
There was no 093-09 report or exit code and nothing was pushed. The first PF-3
comparison used a different viewport from its retained reference; X1 authorizes
its original 1280x992 repeat as the first harness repair attempt. Recovery in
093-10 rehashed every candidate/installed EXE and DLL identically, restored the
saved-editor settings by receipt, and retained the completed gates under X1.
PF-3 repeats and PF-4/PF-5 passed. A later native pairing gate failed; the full
093-10 journal records the stop and host/staging restoration. The candidate
remains unqualified and the remote remains at 1bf7054.
