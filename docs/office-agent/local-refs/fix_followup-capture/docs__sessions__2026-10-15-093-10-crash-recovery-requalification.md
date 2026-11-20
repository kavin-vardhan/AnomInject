# 093-10 - Crash recovery and continued J2 qualification

## Request and recovery

Implement 093-09 ruling X1-X2 in the existing fix/followup-capture worktree,
E:/IA_BuildCache/_r933/AnomalyInjector. No product change is authorized or needed
for recovery. Local head is 571f15a; compiled candidate eff797c; remote remains
1bf7054. The four uncommitted documentation edits from 093-09 are preserved in
_r939/recovery09310/pre-recovery-docs.patch and remain in the worktree.

The PC boot receipt reads 2026-10-15 04:34:05 IST. None of the 23 recorded engine
process identities from the dead attempt is alive. The inspected editor, build,
Python, Codex and Claude processes all began after the boot; command lines were
checked, with hashes retained for foreign processes. No process was stopped.
Both Git date variables remain absent. The interrupted relay attempt has no
reported exit code; the earlier PF-3 runner's separately recorded exit 1 is not
an exit code for the interrupted attempt.

Both hosts remain clean detached at eff797c. All 2,748 original StackOBot and
2,879 original Lyra content files match their receipts. Installed fixture and
helper source files also match. Three additional Lyra plugin assets are tracked
by eff797c and match its Git blob bytes. A supplementary membership check first
classified those as unexpected because it compared only pre-install content and
copied fixture roots; its retained correction adds the candidate's tracked assets.

Four changed Saved/Config files (EditorPerProjectUserSettings.ini and Engine.ini
on each host) were preserved and restored from original receipts. No host Config
axis setting was edited. All 5,764 live staging files rehash unchanged.

Both candidate EXEs, all ten product Editor DLLs, four Review0936 helper DLLs,
the two CaptureBench fixture DLLs and the engine Editor EXE rehash identical,
both in the candidate bank and at their installed paths. StackOBot EXE remains
57F83797, Lyra C6D74FED. No rebuild is needed. X1 retains PF-1, all twelve effective
PF-2 PIE cases, the archived 0.813 replay and the reader self-tests, subject to
Code's later review. Full hashes and restore actions are in
_r939/recovery09310/crash-recovery.json.

## PF-3 viewport repair, attempt 1, before the repeat

The retained f608d56 UV and proxy runs used a 1280x992 viewport and produced
1123x870 pictures. The failed 093-09 comparison instead forced 1123x870 as the
viewport and produced 1096x849 pictures. It passed its independent pixel/mask
gate, but the comparison's largest box difference was 3.160942695 px.

X1 explicitly classifies this as the first harness repair attempt. The copied
specs use Projection 0 0 0 none: retain the helper's original 1280x992 window,
set the process-local X constraint, and leave framing unchanged. New output labels
end in _viewport_r1. A setup receipt checks the original viewport and picture
dimensions before the unchanged comparator runs. No bbox normalization, threshold,
event schedule, reader, product source or binary changes. The 0.5 px comparison
and both original comparison mutants remain binding. The original failed session,
stop receipt, specs and runners are retained in _r939/harness-repairs/pf3-viewport-r1.

The resumed runner skips only the 39 successfully completed effective PF-1/PF-2
steps and archived replay. It retains the superseded marker and viewport attempts
in the chronological receipt. K7 continues PF-3, PF-4, PF-5, the full J2 PIE matrix,
camera switches, cook, staged gates, and a feature-only fast-forward push only if
every gate is green. The idle and disk gates remain unchanged.

The UV repeat passes: 1,152 requested/written, four visible events, zero edge
error, every labelled frame measured, zero wrong-world/frame readings, admission
0/0. The retained comparison has identical events, targets, edges and
visible_positive; maximum bbox difference is exactly 0.0 px. Both shifted-visible
and one-pixel-box controls reject. Proxy follows on the same unchanged setup.

## Completion of marker preflight attempt 2

093-11 / Y3 provenance correction: the healthy office-reader PASS quoted in
the 093-10 report came from the original marker-ON PF-2 session, not marker_r1.
It also holds on marker_r1. The result is unchanged; the old report is retained.

Before any new J2 or camera leg, read-only samples confirmed that moving the
MaintainYFOV barcode vertically was insufficient: the fixed decoder also samples
the wrong horizontal cell positions. The unchanged fixture marker reconstructs
horizontal FOV, so Y-axis projection scales its strip by camera aspect divided
by viewport aspect. This is fixture geometry, not a product projection change.

Complete the existing second marker-repair attempt by calibrating decoder cell
positions from the frozen fixture host/axis and picture dimensions. The fixture's
default 4/3 camera aspect comes from Engine CameraTypes.h; the prepared helper
copies that aspect into both scripted cameras. No expected frame, label or
product projection value enters the position calculation. Twelve bits, two
reference cells, contrast threshold 40, sampling row band and exact low-12 frame
equality remain unchanged. The X path is byte-for-byte the original sampling.

Twenty distributed X samples decode identically before/after; twenty retained
Y/1280x720 samples decode the correct frames after calibration and fail with
the old coordinates; twenty marker-disabled samples reject. One-frame shifted
expectations reject for every sample, and input images rehash unchanged. This
repairs coordinates only: pixel edges, masks, office/PF-2 estimators and all
product/helper binaries are unchanged. Runtime J2/camera verification remains due.
Receipt: _r939/harness-repairs/marker-camera-r2/geometry-completion09310.

PF-3 proxy also passes all 1,152 frames, four visible events, zero edge error,
measured masks, zero wrong-view/frame readings, admission 0/0 and exactly 0.0 px
maximum bbox difference. Both comparison mutants reject. The complete first
repair attempt passes 2/2 cases; runtime-verification.json in the PF-3 repair
folder records both results and the unchanged comparator hash. PF-4 follows.

## PF-4 and PF-5

PF-4 passes by correct boxes, not refusal: 360 requested/written, four visible
events, zero edges, 96 paired measured masks and both box scales effectively
1.000. Its constrained/copy rect is [0,92,1280,536] within the 1280x720 viewport,
camera aspect 2.3900001049. X-scale, Y-scale and incorrect full-window mapping
controls all reject. Admission is 0/0; invalid projection refusals are zero.

PF-5 also passes 360/360 with four visible events, zero edges, 96 paired masks,
both box scales effectively 1.000 and admission 0/0. Every label reports fovDeg=70.
The live controller receipt is requested=70, GetFOVAngle=70, camera cache=90.
The exact f9b9328 Camera.FOV expression therefore reads 90 and fails the same
locked-value predicate. This closes W1 with a measured cache value, not an
assumed number. Both scale controls reject. The full J2 PIE matrix now follows.

## J2 PIE progress

StackOBot delayed viewport, healthy, competing-editor view, selected viewport
and both client selections pass. All are 360/360, four visible events, zero
edges, measured masks, zero wrong views and admission 0/0. The non-owner draw
case counts rejected non-owner families.

StackOBot resize passes W2: r=360, w=358, two owner_render_missing retirements
at indices 98 and 99, no duplicate/overlapping/unexplained drop, zero stale or
mismatched accepted frames. Owner updates=1, stale-generation rejections=6.
Every written picture after index100 is 960x720; all four events have zero
edge error and measured masks. PF-2 axes are 0.9999998371/0.9999998669 (b=1000,
96 rows). Admission is 0/0. No cook or staged leg has started.

StackOBot ambiguity and split-screen pass their refusal predictions: 360/0,
no target applications, admission off-screen/no-projection=0/4. Ambiguity counts
730 ambiguous_owner rejections; split-screen counts 366 multiple_views. Stereo
also refuses all360, with stereo_view counted and admission0/0. Controller ID,
game-thread-view and Game show-flag negatives each refuse all360 and count their
distinct NotGameView sub-reason; admission is0/0. Full per-case counters remain
in each j2-assessment.json and run_summary.json. Native UV/proxy regressions follow.

### J2 progress: both StackOBot regression legs and Lyra delayed viewport

StackOBot UV and proxy each passed 1152/1152, four visible events, zero measured
edge error, paired masks on all 192 labelled rows, zero wrong-view samples and
zero admission refusals. Each leg's ten negative controls rejected; original
sessions rehashed unchanged. Lyra's delayed-viewport leg passed 360/360, four
visible events, zero edge error, zero wrong-view samples and admission 0/0.
The full ordered PIE runner continues; these results do not authorize the cook
or a push before the remaining PIE gates pass.

### Lyra positive ownership cases and resize

Delayed, healthy, competing-editor-view, selected viewport and each of the two
client captures passed 360/360, four visible events, zero measured edge error,
zero wrong-world/frame samples and admission 0/0. The competing-view case counted
2502 scene-mismatch rejections. Corrected positive selected framing is separate
from the exact off-screen PF-1 negative arrangement.

Lyra resize passed 360=358+2 owner_render_missing drops (indices 98 and 99).
Owner generation advanced to 2, owner_updates=1; six stale-generation rejections
were counted and zero stale/mismatched frames were accepted. There were no
unexplained drops, duplicate receipts, written/dropped overlap or pending samples.
After index 100 every picture is 960x720, from the original 1280x992 viewport.
Four visible events have zero edge error and 96 paired measured masks; per-axis
PF-2 scales are 0.9999975287039962 and 0.999997528703996, b=1000. Admission is 0/0.
Evidence: _r939/legs/lyra_P_resize/j2-assessment.json.

### Lyra refusal cases

All six negative ownership cases passed their frozen refusal predicates with
360 requested, zero written, 360 counted dropped/unpaired and no pending samples.
Ambiguity counted ambiguous_owner=730; split-screen counted multiple_views=366;
each refused four named-target attempts for no projection (off-screen=0).
Stereo counted stereo_view=728. Controller ID, game-thread view and Game show
flag each counted not_game_view=1068 and the corresponding sub-reason=1068.
Stereo and the three NotGameView cases had admission 0/0. Rejection counters
count request/family checks, not distinct pictures. The healthy paths on both
fixtures had already passed; the negative cases do not waive those positives.


### Completed regression and alignment controls

Lyra UV and proxy each pass 1152/1152, four visible events, zero RGB edge error,
192 paired measured labelled masks and admission 0/0. Their ten per-leg controls
reject and original captures rehash unchanged. Those ten are four onset/offset
shifts, missing/blank/metadata-count masks, erased marker, dropped-summary and
one actual mask-pixel count mutation. Wrong-object evidence is the retained J1
Part 0 closure; it is not one of these ten per-leg controls.

StackOBot synthetic UV and proxy alignment pass 1152/1152, four event windows,
measured masks and zero annotation/frame/world identity errors. Mask identity,
label identity and donor-frame copies reject. These synthetic legs certify
identity and membership; their native companions alone certify RGB edges.

### D2 stop: native100 mask-picture pairing

The next leg, stack_P_pair_native_100, fails the unchanged permanent pairing
predicate. It writes 80/80 with zero ownership drops/unpaired/pending samples,
known scene/target, and admission 0/0. The pixel reader finds 56 decidable
samples: CURRENT=0, PREVIOUS=0, NEITHER=56; seven are same-position and 17
have no data. The no-data count is a reading under the existing gate; the
56 NEITHER samples are the failure. No threshold or prediction was changed.

Session: _r939/legs/stack_P_pair_native_100/capture/session_20261015-092748.
The unchanged reader SHA-256 is
54928b2ecce48d97a1af4bea16b00184ca091e8b22670fe68e8e3165671f3525.
At sample 2 its reported picture centroid is 435.6 and mask centroid 355.8,
against the existing 40 px tolerance. Both images are 1280x992. Read-only visual
inspection sees a full mask silhouette and a smaller visible magenta region.
The log echoes r.MotionBlurQuality=4, but no experiment establishes that as the
cause. Cause is not established; no proven own-harness defect permits a retry.
No pairing harness repair was attempted and no product code was changed.
The complete failed session and the unchanged reader are retained for review.

The ordered runner stopped immediately. Of the 84 J2 matrix cases, 32 pass,
one fails and 51 are unrun (13 remaining PIE and all 38 staged). This includes
30 ordinary PIE cases across both fixtures and two StackOBot alignment cases.
Remaining pairing/alignment, exact J1/release engine controls, supplemental
admission probes, the scheduled J2-session reader repeat, all camera switches,
the cook and every staged gate are unrun. The required healthy office I PASS
already exists from a new PF-2 engine session with its eight mutation controls;
it is not a camera-switch or staged proof. No camera drop reading is claimed.

The prepared final_gate_audit09310.py is a receipt-completeness check for a
future all-green candidate, not a new predicate. It is unrun because the
qualification stopped. The existing build, strict-include, CPU, string and
lever receipts remain valid for the byte-identical candidate.

### Final restoration and delivery state

Both original host heads and all receipted files are restored: StackOBot
23098fa7415f148e935acbd4a97e18da04e6c46c (201 files), Lyra
caa68c6799db023f7a24c0d6cfc56124ce27b542 (183 files). Both source trees are clean;
original content hashes match. All 5,764 live staging hashes match. Only owned
harness/content roots and reviewed job-created build/config files were removed
by receipt. Every new crash-client config GUID matched this job's launch logs;
a config file by itself is not evidence of a crash. No owned engine process
remains and no foreign process was stopped.

Receipt: _r939/finish-checks.json; detailed review:
_r939/recovery09310/restoration-extra-file-review.json. All axis, viewport,
FOV-lock, anomaly, marker, synthetic-order and render switches were process-local;
their processes are closed and saved settings restored. No host configuration
was edited to set an axis constraint. The live staging tree was never replaced.

No push, merge, tag or natural-cause code. Compiled product remains eff797c;
Source and Shaders remain the compiled candidate bytes, not a qualification
claim. Remote fix/followup-capture remains 1bf7054. The new archive is explicitly
UNQUALIFIED. Resolve the native pairing mismatch and authorize the next
continuation before later qualification or a push.
