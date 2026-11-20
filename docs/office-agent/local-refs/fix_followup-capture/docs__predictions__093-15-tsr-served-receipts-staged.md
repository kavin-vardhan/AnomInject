# 093-15 freeze: TSR setup receipts and physical D storage

Authority: request 093-15 and ruling 093-14 AC1-AC5. Product remains
978103163c0a1839bd6948614818b73f07674c72. Worktree is
E:/IA_BuildCache/_r933/AnomalyInjector, initial head 0e50b56. No product,
fixture-content, estimator, pairing-reader, tolerance or threshold change.
This note is committed before any assessment or engine leg in this continuation.

## AC1: TSR setup matcher, attempt 1 of 2

Requests map to samples through this leg's MASK-SERVED receipts. Never infer
a target-mask request ID from a colour sample index. Parse all M23 mask-pass
receipts with the existing rect parser; join each required sample's MASK-SERVED
request to the pass's served request IDs. Require an unambiguous receipt and
pass for each required sample. Required samples are the union of the labelled
rows used on either axis by the unchanged office PF-2 estimator. Determine
membership using that estimator's existing eligibility, not a second geometric
filter. Record sample, request, receipt and pass identities for review.

The predicate is: every labelled row the PF-2 estimator uses is served by a mask
pass whose internal rect is half the output rect on each axis, and every mask
pass in the leg is half-size. The rect's extent must match its logged dimensions.
The measured output size, image/mask/label size checks and historic TSR recipe
remain unchanged. A missing or ambiguous join cannot count as a passing proof.

Can-fails: make a receipt copy with exactly one required sample's served pass
changed to full-size internal dimensions and matching full-size internal rect.
Its required-sample proof and all-pass predicate must reject. The retained
historic 100% leg's mask-pass receipts must also reject the half-size predicate.
Keep both copied receipts and results; never edit original logs or sessions.

The estimator is still the median per-axis ratio of projected and measured-mask
box-centre displacements about the captured view centre, with its frozen row
eligibility. Require n>=20 on EACH axis, scale in [0.990,1.010], paired measured
masks, 360 requested and written, and each existing 0.96 axis mutant rejecting.
Fewer than 20 rows on either axis is a STOP. No estimator or limit is re-tuned.

First re-assess the retained _r9314/legs/stack_P_tsr50_pf2 session, without an
engine rerun. Preserve its original failed tsr-box-assessment.json and gate-stop
receipt unchanged. Its measured box result remains X .996108890/Y .994475138,
n96 per axis. Then run Lyra PIE TSR50 with the frozen 093-13 arrangement: well
inside-frame target, 1280x720, MaintainYFOV, historic TSR, ScreenPercentage 50,
marker OFF. Staged dedicated TSR50 legs use this same AC1 setup proof.
Pairing keeps its unchanged reader and Z2 parts 1, 2 and 4; part 3 stays retired.

## AC2: resolved-volume storage

All new captures, evidence and archives in this job go to physical D: under
D:/IA_BankOverflow/_r9315 or a new archive in D:/IA_BankOverflow. Resolve existing
ancestors/junctions before checking capacity or choosing output paths. The
50 GiB floor applies to every volume written, including E: during host install,
cook, control builds, restoration and the required repository documentation.
Check before substantial writes and monitor active engine/cook work. Stop for
a floor breach; do not recover space by deleting unowned files.

The incomplete 093-14 archive on physical E: stays untouched. The complete
093-14 overflow archive and all prior evidence are read-only inputs. No capture
or archive is routed through the E:-backed bank/baseline/staging junctions.
Host backups, cooked output, new staging and logs use physical D:. Existing host
working files on E: are modified only within named-host authorization and restored
by fresh receipts. Live delivery staging is read and rehashed, not replaced.

## Order, carried gates and stops

The accepted 9781031 full PIE matrix (42 fresh candidate cases plus four exact
archived-DLL controls), PF-1 to PF-5, AB1 camera controls and reader tests stand
after exact candidate/fixture rehash. Identity inheritance remains superseded.
Order: retained Stack TSR assessment; Lyra TSR PIE; cook both fixtures; every
staged gate, including four PF-2 cases, dedicated TSR50, all 38 staged ledger
cases, historic pairing, W2 resize, camera cut/blend, controls and admission.
The existing staged specs/predicates/can-fails are unchanged except AC1 and output
path routing. -nodebuginfo requires staged EXE/DLL identity to the frozen manifest.
Push only fix/followup-capture, fast-forward, after every gate is green.

W2 resize/camera accounting remains r=w+named drops, with no stale/mismatched
accepted sample. Other positive legs retain w=r, v>=3, edges0 and measured masks.
ExposureExclusionPx remains a DRAWN SUMMARY log reading. Record both targeted
admission counters per leg and camera event continuation/end reasons and drops.

Pairing recipe closed: PREVIOUS>0 is a STOP; NEITHER>0 is a STOP after the Z3
spill readings on that leg. Marker and packaging metadata budgets are spent.
Camera donor: at most two attempts, one used; unavailable control is a STOP.
Camera product failure is a STOP. TSR setup matcher: at most two attempts.
Disk floor applies on every resolved volume written. Otherwise D2 is unchanged.
No retry-until-pass, unauthorized reader change or fixture/product change.

Open after J2: AB3-D, validate change evidence under camera motion (the retained
16 extent_mismatch pairs); AC3-C, investigate a possible half-pixel convention
offset at reduced internal resolution. The measured TSR scale is a reading,
not proof of that proposed mechanism and not authority to change the product.
