I11-A — KNOWN-ANSWER CONTROL ON THE bPassRan PRECONDITION
Pre-declared before any run. Restate this file VERBATIM from this committed copy
before reading any result.

THE HYPOTHESIS UNDER TEST — H6, "the bPassRan hypothesis"
THIS IS A HYPOTHESIS DERIVED FROM SOURCE. IT IS NOT A FINDING. The four vetoed events
in the owner's play-gate smoke are NOT attributed to it, and must not be attributed to
it by this control either (G120 — an observation and its explanation are separate
claims). This control is built so that it can REFUTE H6.

  - Tags ACCUMULATE. AnomalyStencilTag.cpp does not untag until RestoreAll() at
    EndRun.
  - bPassRan tests Mask.CustomStencilExtent — a VIEW-LEVEL property, "was custom depth
    produced at all this frame" — and uses it as a PER-TARGET precondition.
  - Therefore in a multi-target run, one tagged actor being in the relevant set makes
    the extent view-sized, and a DIFFERENT tagged actor absent from that set passes
    bPassRan and contributes a clean Count = 0.

IF H6 HOLDS, THE CONSEQUENCE IS NOT A CLASSIFICATION PROBLEM:

    A TARGET THAT CANNOT BE MEASURED CAN PRESENT AS MEASURED_ZERO, AND BE VETOED,
    IN EXACTLY THE CONFIGURATION THAT SHIPS.

The admit bias is the whole safety argument for m26, and it rests on NOT_MEASURED
admitting. A leak from not-measured into measured-zero is a leak in the safety
argument itself, not in its bookkeeping.

THE SHARPEST FORM. G134: a Nanite target cannot write custom depth on UE 5.1.
  SINGLE-TARGET  -> empty extent -> framesNoPass -> NOT_MEASURED -> ADMIT.
                    This is what SM_Ramp2 was certified to do (handoff 4.4:
                    "must read NOT_MEASURED EVERY TIME").
  MULTI-TARGET   -> alongside one non-Nanite tagged target, the same Nanite target
                    passes bPassRan and reads a clean zero -> VETOED.
On a Nanite-heavy host title that is most authored geometry. SM_Ramp2's certification
would not have caught this if it was only ever exercised single-target.

RECORDED, NOT TESTED BY THIS CONTROL: the single-target NOT_MEASURED reading may itself
depend on nothing else in the scene writing custom depth. A host game using custom depth
for outlines or highlights could pass bPassRan with no plugin target in the set at all.
OUT OF SCOPE FOR I11-A. Noted so it is not rediscovered.

NOTHING IS BUILT
No shader, no render target, no new state, no new counters, no new flag. This control
reads the M26S1 EVENT log line, which already emits every per-event field, for VETOED
events included, because the veto edits Async->SessionEvents and never touches
MaskMeasure.Records.

THE DISCRIMINATOR — the only one
    the mask STATE of each arm's control target — NOT_MEASURED versus MEASURED_ZERO.

THREE ARMS. ALL THREE RUN. Arm 3 is NOT skipped if arms 1 and 2 look decisive.

ARM 1 — NANITE, MULTI-TARGET
  SM_Ramp2 (the designated known-Nanite control) armed in the same run with
  OVERLAPPING windows alongside a known-drawing NON-Nanite target that is on screen
  and in the relevant set (Cube StaticMeshActor_49 or equivalent).
    PRECONDITION OF THE ARM: the partner target must be on screen and in the relevant
    set for the window. An arm whose partner is not is INVALID, not refuting.
    PREDICT if bPassRan is SOUND    : SM_Ramp2 = NOT_MEASURED. Admitted.
    PREDICT if bPassRan is UNSOUND  : SM_Ramp2 = MEASURED_ZERO. Vetoed.

ARM 2 — OFF-SCREEN, MULTI-TARGET
  A known-drawing NON-Nanite target placed deliberately outside the view's relevant
  set for the whole window, armed alongside the same on-screen target.
    PRECONDITION OF THE ARM: as arm 1.
    PREDICT if SOUND    : off-screen target = NOT_MEASURED.
    PREDICT if UNSOUND  : off-screen target = MEASURED_ZERO.

ARM 3 — SINGLE-TARGET CONTROLS, BOTH OF THEM
  SM_Ramp2 alone. The off-screen target alone.
    PREDICT, BOTH CASES: NOT_MEASURED.
  This arm is what makes arms 1 and 2 mean anything. If arm 3 does not reproduce the
  historical single-target reading, the RUN IS INVALID and nothing from arms 1 or 2
  is read.

PRE-REGISTERED VERDICTS — restate verbatim before reading any result
  Arms 1 AND 2 both MEASURED_ZERO, arm 3 clean
      -> SUPPORTED by two independent routes. m26 has a delivery-affecting defect.
         STOP. Chat ruling before any fix is designed, let alone built.
  Exactly ONE of arms 1/2 MEASURED_ZERO, arm 3 clean
      -> PARTIALLY SUPPORTED, route-specific. Report, do NOT generalise, stop.
  Neither, arm 3 clean
      -> REFUTED. The smoke's four zeros need another explanation and R3 returns with
         the field narrowed. THIS IS A REAL AND GOOD OUTCOME. Do not go looking for a
         way to make it fire, do not widen an arm, do not re-run until it does.
  Arm 3 dirty
      -> INVALID RUN. Nothing is read. Report and stop.

READINGS RECORDED IN EVERY ARM, whether or not they enter the verdict
  the FULL M26S1 EVENT line for every event — State, MaxCount, ArmsIssued,
  ArmsResolved, FramesContributed, FramesNoPass, FramesDiscarded,
  FramesResidualDiscarded, FramesUnconfirmed, SkippedHidden, CollisionHits, bTagFailed,
  FirstCollisionDetail.
  COLLISIONS ESPECIALLY: three of the smoke's four vetoed events carried collisions=1.
  If CollisionHits differs systematically between the multi-target and single-target
  arms, REPORT IT AS AN OBSERVATION. It does not enter this verdict and it does not get
  an explanation this turn.

CONDITIONS
  PIE is acceptable — this is a mechanism question. G76 holds: no shipping claim, no
  statement about delivered captures.
  Bank every run attempt BEFORE reading any gate. Match by SESSION ID, never by
  directory name.
  Commit the pre-declaration to docs/predictions/ BEFORE the first run, and restate it
  VERBATIM from that file before reading any result.
  No same-turn fix to anything, whatever the result. Not to the arms, not to bPassRan,
  not to a log line.

STANDING CONSTRAINTS THAT DO NOT MOVE FOR THIS INVESTIGATION
  The veto rule is ZERO-ONLY and is LOCKED. No ratio, no threshold, ever proposed.
  P6 does not move. CB_GateLevel is untouched (G99). Stencil range stays 200/255.
  feature/stencil-capture is READ-ONLY at 76cac74.
