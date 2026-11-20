# 093-07 predictions: committed before product changes and engine legs

The design/FOV audit is in the 093-07 journal. K3-K7 amend the preserved
093-06 predictions: accepted_ambiguous is removed; original Lyra selected
framing is PF-1, while the positive selected case uses engine-verified
on-screen framing. All old identity, edge, measured-mask and pairing thresholds
remain. Order: PIE PF-1 through PF-4, all PIE J2 cases, PIE camera switches;
then cook; all staged gates; only then a fast-forward feature-branch push.

## Frozen PF-2 estimator and office box read

For each JSONL row use each labelled, non-transition anomaly with bbox_valid,
a finite positive-size bbox_px and bbox_drawn_px, present readable measured
mask, positive mask tag/count, and exact mask count/bounds matching the row.
Require matching image/row/mask dimensions. Exclude clipped boxes: both boxes
must lie at least one pixel inside all picture edges. On each axis separately
let C=written_dimension/2, P=projected_box_origin+size/2-C,
D=mask_box_origin+size/2-C. Use the row on that axis only if |D| is at least
0.05*written_dimension. Record every exclusion; missing masks never become
usable rows. Do not select by the observed scale or discard statistical outliers.

The axis estimate is median(P/D) over the eligible rows. Require at least
20 distinct contributing session rows on each axis. Both raw estimates must
be within [0.990,1.010]. `b` is round-half-up(1000*s) for the axis with the
largest absolute deviation from 1 (X wins ties). This prevents a good axis
or averaging from hiding a bad one. `n` is the number of distinct rows used
on either axis. Missing/insufficient geometry is b=-1, not 1000. Box-read
exit 0 requires both raw axis bounds and the row minimum; otherwise exit 1.

Use the exact same implementation for PF-2 and `--box-read`. Can-fails:
retained StackOBot f608d56 masks read approximately 1.000 and pass; retained
Lyra 1123x870 masks read approximately 0.967 and fail. The null-mask 942x869
stop reports no scale; retain its engine-corner mismatch reading (~0.813)
and supplement with the archived binary replay described in the design so
a measured-mask case also reads ~0.813 and fails. A copied projected-box
centre scale of 0.96, on either axis separately, must fail; originals rehash.

For moving-camera legs compute the same estimator both overall and over
the scripted blend interval, with the same minimum/exclusions and thresholds.
Choose blend duration long enough to contain >=20 measured active frames.

## Projection and regression gates

| Gate | Setup and positive prediction | Can-fail |
|---|---|---|
| PF-1 | Exact original Lyra selected 942x869 MaintainYFOV camera/target arrangement. No visible-positive or labelled target row; no target fire or counted refusal. Independent engine corners stay off left edge. | Retained f608d56 session has 96 false positive rows and must fail the identical checker. |
| PF-2 PIE | Both fixtures x X/Y constraints x 942x869,1123x870,1280x720. Override only process-local local-player state. Target genuinely on screen, noncentral in both axes. >=3 visible events, measured masks, raw scale on each axis 1 +/- .010. | Old f608d56 controls plus independent per-axis box mutants above. |
| PF-2 staged | Both fixtures x X/Y at 1280x720; same scale and pixel gates. | Same old-source/box controls; staged originals copied/rehashed for mutations. |
| PF-3 | StackOBot UV/proxy native regression compared with its f608d56 retained sessions, same config/fixture/viewport/seed. Canonicalise events by ordinal, type, target (not process-global frame/event IDs); compare session-index onset/end sets, visible_positive on every common requested row, and all four bbox_px values <=0.5px. Same attempted/fired events and targets. | Shift one onset/visible row or one bbox coordinate by 1px in separate copies; each must fail. |
| PF-4 | StackOBot PIE with constrained-aspect camera 2.39 and black bars in the display. Engine rect and copied crop recorded. Correct PF-2 boxes/zero edges/measured masks, or explicit counted projection refusal; never a wrong box. | Copied full-window-vs-constrained-rect mapping mutant must fail; missing rect cannot pass. |
| Camera switch | Both fixtures, PIE and staged. Scripted view-target cut during one active event, blend during another; choose framing that retains a visible target and freeze timing in each launch spec. Zero measured edges; every labelled row has a measured mask; PF-2 overall and blend-frame agreement. | Copy a stale pre-switch projection or shift mask-picture frame membership; checker rejects. |

Camera-switch drop counts per switch and whether the event continued/ended,
with its recorded reason, are readings only. Do not turn them into a new
fire gate. Use a sufficiently long active window for the blend scale sample.

Camera-switch launch details frozen before engine legs: `Config 0 30 180 60 4`,
cap 1100 (expected schedule 990 samples), exact 1280x720 with on-screen engine
framing. At session index 60 cut to a distinct camera translated (0,35,20);
at 300 blend linearly back to the original view target over 1.5 seconds.
The capture's existing fixed 30 Hz clock makes the blend interval reproducible.
Use PF-2 over indices 301-344 as well as all eligible rows. Log actual switch
indices and camera poses; do not infer a switch from an ownership update alone.

Moving-camera RGB evidence uses a separate identity-copy control with the same
camera script and timing, with `TexCorruptIdentity 1` and no explicit mode. At
each matching session index require equal saved camera pose (1e-4 tolerance)
and image dimensions before comparing the target's measured mask region against
that clean picture. Use the existing max(4,4*max(clean_noise,.5)) RGB threshold,
and the same per-event onset/offset indices. For clean edge neighbours use the
nearest event mask at that edge's unchanged pose. Every labelled picture must
exceed the threshold; all non-transition edge neighbours must be clean. The
identity control compared with itself must fail the visible-event requirement.
This avoids treating the camera movement itself as visible corruption. A stale
pre-switch projected box or shifted mask membership remains the geometry can-fail.

## Full final-candidate J2 matrix

Re-run all 84 planned 093-06 cases, not only the 61 previously unrun, on
both fixtures and applicable PIE/staged modes. Preserve the delay, foreign
editor, selected/two-client, resize, same-world ambiguity, split/stereo,
three NotGameView negatives, UV/proxy regression, native/synthetic alignment,
and native/synthetic 100%/50% mask-picture pairing cases and controls.
Editor-only arrangements use their declared standalone counterpart, never
claim an editor viewport exists in a packaged game. Healthy cases require
360 requested/written, >=3 visibly changed events, all same-sample barcodes
correct, wrong-world count 0, measured masks and 0-frame native edges.
Resize retains requested=written+dropped with stale results excluded.
Refusal cases require 360 requested/0 written and the proper positive count.
Exact J1/release DLL controls name their archives; rebuilt monolithic controls
are separately hashed and are not claimed to be the archived executable.
No geometry claim is substituted by an ownership counter.

## Reader and hygiene gates

PASS: `I<h>.<case> PASS v=<v> c=<0 X,1 Y,2 major> b=<b> #<check>`.
Requires r=w=360, >=3 visible events, e=0, x=0, m=0, measured paired masks,
owner diagnostics and both raw PF-2 scale bounds. Final office acceptance also
requires the owner's appended g=0 after the local 20-frame contact sheet.
FAIL: full r,w,v,e,x,m,c,b, then check digit and the existing ordered 16
I-failure counters. Null mask rows count toward m while other fields compute.
Box mode: `B<h> n=<n> b=<b> #<check>`, same estimator and exit status above.
For I check digits use weighted sum (weights 1..11) of host,case,pass=1/fail=0,
r,w,v,e,x,m,c,b modulo 10; B uses weights 1..3 for host,n,b. Unknowns are -1.

Quote four self-test lines: new healthy PASS; retained null-mask Lyra FAIL;
retained StackOBot box PASS; retained Lyra 1123x870 box FAIL. Run with actual
Python 3.10 and the normal home interpreter. No hashlib.file_digest or package
beyond existing numpy/Pillow/verify_capture. Contact-sheet generation and null
mask tests use job-owned copies/output paths; retained originals rehash.
Additional controls: corrupt mask count, missing ownership/projection metadata,
shifted edge, scaled X-only/Y-only bbox, and wrong home world marker must fail
their corresponding checks. The contact sheet is not automatic view proof.

Normal and strict Editor/Game builds on both hosts, standalone header checks,
meaningful CPU geometry/reader tests and prior suites, source/binary lever
audits, string attribution, comment strip, diff checks, archive rehash and
archive-byte mutant are required. New bench code is IAIBench-gated and denied
Shipping. Capture baselines and restoration are by hash receipt; disk floors
are 50 GiB on both volumes. Never change security settings or foreign processes.
The D2 stop conditions and two-attempt harness-fix limit remain binding.
