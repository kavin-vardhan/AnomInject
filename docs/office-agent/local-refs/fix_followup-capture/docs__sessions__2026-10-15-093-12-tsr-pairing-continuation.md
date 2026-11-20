# 093-12: TSR half-resolution pairing continuation

## Goal

Implement Z2-Z3 as bench proof and documentation only; finish the inherited K7
order on unchanged eff797c if every gate passes. No product or fixture changes.

## Freeze

Worktree E:/IA_BuildCache/_r933/AnomalyInjector, fix/followup-capture, initially
3d09293 and clean. Evidence root E:/IA_BuildCache/_r9312. The prior stopped
historic native50 is retained as "size assertion encoded AA-off", reading only.
The new prediction file freezes all four Z2 parts and Z3 verbatim before a leg.
The permanent pairing and PF-2 readers remain unchanged. Recipe, packaging and
marker budgets remain spent. Commit and first-launch times will be recorded.

## State

Preflight PASS: both eight-file candidate banks and both fixture DLLs match;
Stack/Lyra original files 201/183 and content 2748/2879 match, all 5764 live
staging hashes match, and no prior owned engine process identity survives.
Date overrides are absent. No new engine leg has run.
No push; remote stays 1bf7054. Full results and restoration receipts follow here.

## Pre-leg and early PIE record

Freeze cdef048e35cb25bc5c4d16d34ea278be477b720c was committed at
2026-10-15 20:47:54 +05:30. The first fresh leg launched at 20:57:18.620 IST,
after a passing idle gate. The exact archived target receipts and eleven support
DLLs were reinstalled by hash, with no UBT metadata regeneration and no new
packaging repair attempt. Both host checkouts use the compiled eff797c snapshot.

The retained stopped native50 session is assessed as a reading in
_r9312/readings/retained09311-z2-r1/result.json. All 82 mask-pass lines report
640x496 internal inside 1280x992 output. All 80 pictures and 69 existing masks
have the correct size; all 18 labelled rows have measured masks. PF-2 X/Y each
have n=0 because the target box touches the picture edge; both axes are readings,
not gates. The historical 100% log and copied wrong-width row reject.

The first dry-run wrapper had demanded mask files on unlabelled null-mask rows.
This was an instrument bug, not a Z2 requirement. Repair attempt 1 checks every
existing mask and every non-null reference, while retaining the measured-mask
requirement for every labelled row. The failed dry-run and original script are
preserved under harness-repairs/z2-null-mask-r1. No engine leg was retried and no
permanent reader, threshold, product or fixture changed.

Early results at the historic recipe:

| New PIE leg | C/P/N | Coverage | speed_ratio |
|---|---|---|---|
| Stack native50 | 56/0/0 | 0.688305011 | 1.7944412654902362 |
| Stack synthetic100 | 60/0/0 | 0.682945833 | 1.826303469700904 |
| Stack synthetic50 | 61/0/0 | 0.689513767 | 1.8847957215731657 |
| Lyra native50 | 57/0/0 | 0.692690879 | 1.90546165821673 |
| Lyra synthetic100 | 52/0/0 | 0.686543290 | 1.8354248731762548 |

All five write 80/80, zero drops/unpaired/pending, and both admission counters
zero. Their shifted-mask controls reject. The three new 50% cases meet Z2 with
80/81/82 mask-pass lines respectively, no missing captured request, consistent
1280x992 outputs, 18 measured labelled rows and n=0 on both PF-2 axes (readings).
The ledger at this point is 41 PASS, 0 FAIL, 43 NR. Lyra synthetic50 is queued
behind the idle gate; the original four-hour deadline is retained. No later
engine control, camera leg, cook or staged gate has yet run.

## Remaining pairing and exact-control progress

Lyra historic synthetic50 passes 56/0/0, shifted copy 8/47/0 rejected, coverage
0.6929439751252026 and speed_ratio 1.8737517344423487. It writes 80/80 with
65 existing masks and 18 measured labelled rows. All 80 mask-pass lines report
640x496 internal inside 1280x992 output. Both PF-2 axes have n=0 and are
unavailable readings, not assertions of measured scale. Its admission counters
and drop/unpaired/pending counts are zero. All eight historic PIE pairings now
pass, including the two retained native100 cases.

Exact archived J1 DLL controls on Stack and Lyra each request 360 and write 0,
as predicted. The Stack pre-F1 wrong-view control writes 360/360 with wrong-view
count 360. Every five-DLL swap is restored and rehashed before the next host
control. Old builds lack the admission fields; they are unavailable, not zero.
The Lyra pre-F1 control remains behind the original idle deadline. At this
snapshot the matrix has 45 PASS, 0 FAIL and 39 NR.

## Exact packaging sidecar restoration before cook

A read-only check of the archived target receipts found one missing Lyra
support artifact: Binaries/Win64/tbb.pdb. The receipt lists it as both
RequiredResource and DebugNonUFS; -nodebuginfo omits the latter category but
does not omit the RequiredResource copy. The exact file was available in the
engine installation and matched its retained _r939 created-file receipt:
e2dc5c1533d5dbfe27920c3eb8d9ff051b4c00bcdc205f6fcad8ce17e0b0ac4d,
6,230,016 bytes. It was copied into the job-owned host installation and added
to the restoration manifest. The target receipt and all executable code are
unchanged. Evidence: _r9312/receipts/exact-tbb-sidecar-install.json.
This completes exact artifact restoration before any cook; it is not metadata
regeneration, a relink or a retry of a cook. The packaging budget stays spent.

## Exact engine controls complete

The Lyra pre-F1 control's idle gate cleared after 2933 seconds under the
original four-hour deadline. It wrote 360/360 frames, all 360 from the wrong
view, so the negative passed. Its five candidate DLLs were restored and
rehashed. Both fixtures now reproduce J1's delayed-viewport 360/0 failure and
the release's wrong-view 360/360 failure using the exact archived editor DLLs.
All four engine negatives pass. The matrix now has 46 PASS, 0 FAIL, 38 NR;
supplemental admission probes and camera switches follow before any cook.

## Admission checker output repair

Stack PIE admission writes 360/360, four visible events, zero edge error and
wrong-view count zero. Counters are off_screen=0 and no_projection=1; the
generic apply probe records before=0/after=0 and restores its temporary state.
The scratch checker then failed while writing its negative-control log with
Windows' default cp1252 encoding: an existing replacement character could not
be encoded. The positive assessment had already passed.

Repair attempt 1 adds only encoding='utf-8' to that copied-log write. The
admission predicate AST is identical and the raw log hash is unchanged.
Reassessment of the same session passes both copied controls: zero counter
and an applied-after-refusal receipt reject. No engine leg was rerun. The
original script, empty failed output, error and exit-1 step remain retained
under harness-repairs/admission-log-encoding-r1. The ordered record appends
the successful reassessment and resumes with Lyra admission, then cameras.

## Final PIE results and product stop

Lyra admission also passes 360/360, v4/e0, no wrong-view frame and O/N=0/1.
Its zero-counter and applied-after-refusal copies reject. The full 84-case J2
ledger has 46 PASS and 38 NR: the entire PIE matrix is complete; the separate
camera requirement fails before the cook. All staged cases remain unrun.

The Stack camera identity reference launches after its idle gate passes. It
reaches the scripted cut at session_index=60, game_frame=226, then exits 3 with
an assertion: index 206410 from an array of size 0. The runner reports no
completed session because run_summary.json was never finalized; the partial
session_20261015-224226 is retained, not absent. Its raw files are unchanged.

The crash record is UECC-Windows-EB9BD6A7443BBD7AA23A54A531C9CB1C_0001.
The GameThread portable stack enters AnomalyCapture at RVA 0x579b3, inside the
PE runtime-function interval 0x56fe0..0x57f42 beginning at the exported
FoldExposureExclusion. Callers map to ServiceTargetMask, ProcessCompletedFrames
and Tick. Source inspection explains the failure: ownership invalidation clears
ExposureExclusionMask and the folded-event set but leaves width/height/pixel
count. FoldExposureExclusion allocates only when width or height changes, then
indexes the empty array for a same-size mask. This is a source-supported
diagnosis; no repaired execution is claimed. It is product scope, not a marker
repair. No product source was changed and no camera leg was retried.

Partial readings, not a camera PASS: planned r=990; 62 logged arm requests;
57 written rows/pictures at indices 0..56; three owner_render_missing drops
at 57,58,59, all pending before the cut; two unfinalized requests at 60,61.
All 28 labelled rows have measured masks; PF-2 X/Y n=28 each, scales
0.9999999403953554/1.0; the frozen frame/world decoders disagree on zero of
57 written frames. No post-cut frame was written. No final summary or
annotation exists, so admission totals and completed W2 accounting are
unavailable. The event remains tagged at 60/61; the change-evidence phase
records epoch_reset/extent_mismatch, but no normal event-end receipt exists.
The blend at 300 was never reached. Full cut/blend continuation/edge readings
remain unqualified. See gate-stop.json, stop-readings09312.json and the copied
crash XML/minidump under _r9312/crash-evidence.

D2 stops further cameras, the cook, staged gates and push. Recipe (2/2),
packaging metadata (2/2) and marker budgets remain spent. The required decision
is authorization of the product exclusion-state invalidation repair and renewed
camera/remaining K7 qualification; no third recipe or marker attempt is proposed.

## Restoration, audit and closure

Restoration returns Stack to 23098fa7415f148e935acbd4a97e18da04e6c46c and Lyra
to caa68c6799db023f7a24c0d6cfc56124ce27b542, with 201/183 original files rehashed,
both source trees clean, and original content unchanged. All 5764 live-stage
hashes match. Seven Stack and 21 Lyra receipt-owned extra files, plus the five
installed helper/content roots, are removed by path/hash or own-launch GUID
receipts. No owned engine process remains; foreign processes and system security
settings were untouched. No cook or live-stage replacement occurred.

The unchanged candidate retains eight normal/strict Editor/Game builds, 69 CPU
steps, nine family steps, five ownership mutants, string/lever audits and the
earlier measured PF-1..PF-5/reader proofs. The closing audit rehashes the candidate
and verifies the retained evidence; it does not claim the failed camera gate
passed. G485 is extended with the measured TSR lesson and the n=0 reading limit,
then rechecked across all refs before the documentation commit. The archive is
followup-capture-J2-tsr-09312-eff797c-57F83797-C6D74FED-UNQUALIFIED, with the
partial camera capture and crash dump preserved. report.md records all hashes,
readings, harness attempts, restoration and the final archive rehash. No push;
remote fix/followup-capture remains 1bf7054f5b8664f057282ed863a62ab5dbbd60e3.
