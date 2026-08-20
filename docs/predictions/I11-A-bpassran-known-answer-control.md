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

>>> SUPERSEDED 2026-08-20 BY THE AMENDMENT AT THE END OF THIS FILE. <<<
>>> THE THREE ARMS BELOW ARE PRESERVED, NOT DELETED. They were restructured
>>> BEFORE ANY MEASUREMENT EXISTED — no leg had been run, no result could have
>>> biased the restructuring, and the record must show that. Arms 1 and 2 have no
>>> shipping realisation: a capture run carries exactly one targeted (anomaly,
>>> actor) pair, and the auto-pool cannot be told whom to pick. Read the AMENDMENT
>>> section for the arms that are actually run.

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


================================================================================
AMENDMENT — 2026-08-20. MADE BEFORE ANY MEASUREMENT EXISTS.
================================================================================

THIS AMENDMENT WAS MADE BEFORE ANY MEASUREMENT EXISTS. No leg of I11-A had been
run when it was written. Not one arm, not one attempt, not one log line. The
original three arms above are PRESERVED AND MARKED SUPERSEDED rather than deleted,
so the record shows the restructuring could not have been biased by a result.

WHY THE ARMS WERE RESTRUCTURED
Arms 1 and 2 required TWO NAMED TARGETS IN ONE RUN. Targeted mode carries exactly
one (anomaly, actor) pair for the whole run (AnomalyCaptureSubsystem.cpp:690-693,
:1237), and the auto-pool cannot be told whom to pick. That is journal 045 PART
ONE blocker B1, which halted H4's original brief for the same reason. The lever
construction below replaces the second target with a switch, which is one variable
instead of two and is independently checkable before any result is read.

THE REOPENING OF THE HOST-CUSTOM-DEPTH SCOPING — OWNER, RECORDED VERBATIM

  I scoped that route out while reasoning about RISK SURFACE — host games we do not
  control. Your Sharpening 2 established that the mechanism does not care whose
  primitive sets bHasCustomDepthPrimitives. It is therefore not a different mechanism
  sitting outside I11-A's scope; it is THE SAME MECHANISM WITH A CLEANER SWITCH ON IT.
  I was reasoning about risk when I should have been reasoning about instrument design.
  The out-of-scope note stands for the RISK (host titles remain unassessed). It does not
  stand against using the route as a LEVER.

Deciding factor over the shipping-path alternative: this construction is one
variable, AND the lever's own firing is independently checkable before any result
is read. That is the DPI-unaware lesson (G114) and it outranks the shipping-path
advantage at this stage.

THE CONSTRUCTION — THE CONTROL IS RE-RUN, NOT REUSED
THE SAME ACTOR IS PRESENT IN EVERY LEG. The only thing that changes is its
bRenderCustomDepth flag, true or false. Actor presence, level, pose, seed, session
shape, PIE — all held.

ARM A — NANITE ROUTE, LEVER ON
  Targeted SM_Ramp2 alone. Writer actor bRenderCustomDepth = TRUE,
  CustomDepthStencilValue = 1 (outside the reserved 200/255 range, so it
  contributes to no tag count and trips no reserved-tag detector).
    PREDICT if bPassRan SOUND   : NOT_MEASURED. Admitted.
    PREDICT if bPassRan UNSOUND : MEASURED_ZERO. Vetoed.

ARM B — OFF-SCREEN ROUTE, LEVER ON
  Targeted, a non-Nanite MainWorld actor verifiably outside the view's relevant
  set for the whole window. Same writer, same flag TRUE.
    PREDICT if SOUND   : NOT_MEASURED.
    PREDICT if UNSOUND : MEASURED_ZERO.
  IF ARM B CANNOT BE CONSTRUCTED CLEANLY, SAY SO AND RUN A AND C ONLY. The
  pre-registered verdict table already handles one-route-only as PARTIALLY
  SUPPORTED. Do not force it.

ARM C — BOTH TARGETS, LEVER OFF
  Writer actor present, bRenderCustomDepth = FALSE. Same PIE session shape as A
  and B.
    PREDICT, BOTH: NOT_MEASURED.
  Arm C is THE CONTROL. If it does not read NOT_MEASURED, THE RUN IS INVALID and
  nothing from A or B is read.
  P31_S3_RAMP stays what it is: prior packaged evidence that single-target
  SM_Ramp2 reads NOT_MEASURED in a shipping run. CONTEXT, NOT THE CONTROL.

DISCRIMINATOR UNCHANGED: NOT_MEASURED vs MEASURED_ZERO.
VERDICT TABLE UNCHANGED IN STRUCTURE — A and B are now the two independent routes.
Read the four pre-registered verdicts above with "arms 1/2" meaning ARMS A/B and
"arm 3" meaning ARM C.

LEVER VALIDITY — A HARD GATE, READ BEFORE ANY ARM RESULT
  POSITIVE evidence that the lever did something is required: the custom-stencil
  extent MUST differ between lever-ON and lever-OFF. This is NOT inferred from the
  arm's own outcome, which would be circular.
  If the lever cannot be shown to have fired, THE RUN IS INVALID. NOT REFUTING.
  This is Ruling 2's form: under this construction the "partner" of the original
  arms disappears and the validity condition transfers onto the lever. An arm
  whose lever cannot be shown to have fired is INVALID, NOT REFUTING.

LEVEL MUTATION — HARD CONSTRAINTS
  Editor-in-memory only. NEVER saved. Reverted after. `git status` on the content
  directory is reported afterwards as POSITIVE evidence the level is unmodified —
  the absence of a complaint is not the evidence.
  G97/A59: Paths.project_dir() and the engine version are echoed BEFORE any
  measurement. On a mismatch, ABANDON. Do not work around it.
  PIE only, so G76 holds: mechanism claim only, no shipping claim, no statement
  about delivered captures.

POSE RULE — THREE CLAUSES, AND THE TRAP IN CLAUSE (ii)
  A state difference between legs is attributable to the lever only if the
  target's VIEW RELATIONSHIP was the same in both.

  (i)  POSE-INVARIANT BY CONSTRUCTION — SM_Ramp2 is Nanite and cannot write custom
       depth from ANY pose (G134), which is why it was certified to read
       NOT_MEASURED EVERY TIME. Pose matching is NOT required for Arm A. Pose is
       still RECORDED per leg.
  (ii) POSE-DEPENDENT — for Arm B, "off-screen" IS a pose claim. The leg must carry
       POSITIVE evidence the intended view relationship held for the whole window.
       THAT EVIDENCE IS coverage_ratio -> 0 FROM THE PROJECTOR, AND ONLY THAT.
       DO NOT USE framesNoPass AS POSE EVIDENCE. The P29_S2_CYL73 lockstep
       (coverage_ratio -> 0 AND framesNoPass -> 4) is tempting and the framesNoPass
       half is DOWNSTREAM OF THE LEVER — H6 predicts precisely that it stops firing
       when the lever is on. Using it as pose evidence is circular and would make
       Arm B unfalsifiable.
       coverage_ratio comes from ProjectActorBoundsToScreenRect, a separate CPU path
       with no relationship to custom depth. It stays valid under both lever states.
  (iii) Pose evidence absent -> UNDECIDABLE (A60). Never reconstructed, never
       replaced by a weaker reading reported as the original.

NO SAME-TURN FIX TO ANYTHING, WHATEVER COMES BACK.


================================================================================
SECOND AMENDMENT — 2026-08-20. MADE BEFORE ANY MEASUREMENT EXISTS.
================================================================================

THIS AMENDMENT WAS MADE BEFORE ANY MEASUREMENT EXISTS. No leg of I11-A had been
run when it was written — not one arm, not one attempt, not one log line. All prior
text is PRESERVED. Nothing is deleted.

THE STANDING RULE THIS AMENDMENT OBEYS — a rule, not an excuse for this instance
  A pre-declaration may be amended before any measurement exists, and ONLY to
  TIGHTEN a validity condition. It is never amended to LOOSEN one, and a prediction
  or a verdict is never amended at all once the instrument exists. Every amendment
  is dated, appends, and deletes nothing.

PREDICTIONS AND VERDICTS ARE UNCHANGED BY THIS AMENDMENT. A and B remain the two
independent routes.

THE ENVIRONMENT — OPT P
  PLAY-IN-EDITOR, OWNER-INITIATED. The owner presses Play once per leg and touches
  nothing else. The FOCUS GATE IS LIVE (it is skipped in Simulate; that is one of
  the reasons Simulate was rejected). Every leg records its start_frame so a leg
  that rode the focus-gate timeout is visible as such (A63).
  Play-In-Editor cannot be started from Python in UE 5.1 and the bridge exposes no
  endpoint that starts one; the bridge drives everything else.
  G76 holds: mechanism claim only, no shipping claim, no statement about delivered
  captures.

TIGHTENING 1 — SOURCE QUESTIONS ANSWERED BEFORE THE FIRST PLAY
  (a) Does TryFireSpecific carry ANY visibility, frustum or coverage guard?
      ANSWERED NO, from source. AnomalyAutoInjectorSubsystem.cpp:258-326 gates on
      world/injector validity, non-empty id and name, MaxConcurrent, IsIdLive,
      exact "=" name match, IsActorLive, and ApplyAnomaly's return. There is no
      viewport predicate of any kind. A target behind the camera fires.
      CARRIED CAVEAT: the anomaly's own Apply re-filters if IAI.SetViewportScoping
      is ON (default OFF). The per-leg read-back is the fire log line — under
      scoping ON, blinking matches 0 and the line reads "not applied".
  (b) If the target is off-screen, does the mask ARM at all, or does LOCK-1 refuse?
      THE DANGEROUS ONE. If nothing armed, the leg would read NOT_MEASURED for a
      reason having NOTHING to do with bPassRan — a clean-looking pass on the SOUND
      prediction, produced by never reaching the precondition. That is a FALSE
      CONFIRMATION and it is excluded BY CONSTRUCTION, not spotted afterwards.
      ANSWERED, from source: it DOES arm. LOCK-1 tests AActor::IsHidden()
      (AnomalyMaskMeasure.cpp:174), the actor's bHidden flag, which is what the
      blinking anomaly toggles — it is NOT on-screen-ness. TagActor requires only
      IsRenderableComponent = component IsVisible() and type, again not
      on-screen-ness. SampleEndOfFrame reads the same IsHidden(), so an un-hidden
      off-screen actor confirms VISIBLE and is not residual-discarded.
      THE GATE BELOW MAKES THIS EXPLICIT RATHER THAN ASSUMED.
  (c) Confirm the PIE world is MainWorld before any capture, on EVERY leg. Cheap,
      and G87 exists.
  (d) Enumerate the BRIDGE'S OWN command surface, not the `unreal` namespace, and
      report whether any endpoint starts a Play session as opposed to Simulate.
      ANSWERED NONE, by positive enumeration of all 62 advertised endpoints.
      AND THE RULE THAT PRODUCED THIS QUESTION: an absence-of-finding is a WEAKER
      claim than a positive measurement, and it is only as good as the surface that
      was searched. The first PIE foreclosure enumerated the wrong surface.

TIGHTENING 2 — PER-ARM VIEW-RELATIONSHIP GATES
  A HOLE CLOSED: Ruling 4(i) said Arm A needs no pose matching, which is true — a
  Nanite target cannot write custom depth from any pose. But it never required the
  target to be ON SCREEN. If SM_Ramp2 falls out of frustum, ARM A SILENTLY BECOMES
  ARM B and "two independent routes" collapses into one route measured twice.

  Both gates read coverage_ratio FROM THE PROJECTOR, which is independent of the
  lever under both its states.
  framesNoPass REMAINS BARRED as view evidence, and THE BAR NOW APPLIES TO ARM A AS
  WELL AS ARM B. It is downstream of the lever; H6 predicts precisely that it stops
  firing when the lever is on.

    ARM A / C-Ramp   VALID only with coverage_ratio > 0 across the window.
                     Target on screen and still unable to write custom depth: the
                     Arm A condition.
    ARM B / C-Grat   VALID only with coverage_ratio -> 0 across the window.
    ALL ARMS         VALID only with ArmsIssued > 0. If nothing armed, the
                     precondition was never exercised. INVALID, NOT REFUTING.

  Gate failure = INVALID, never refuting, never "close enough".

  ADDITIONAL TIGHTENING, PERMITTED BY THE STANDING RULE BECAUSE IT ONLY NARROWS:
  coverage_ratio 0.0 is AMBIGUOUS in source — AnomalyCaptureSubsystem.cpp:1985
  emits 0.0 both when the target never projected (CoverageCount == 0, the sentinel)
  and when it projected with zero clamped area. Both readings mean "contributed no
  on-screen area", so the gate holds either way, but the ambiguity is recorded and
  the stronger per-frame evidence is required alongside it: labels.jsonl's
  per-row bbox_valid, from the same projector CPU path and equally independent of
  the lever. Delivery is OFF on every leg, so labels.jsonl is written.
    ARM A / C-Ramp   additionally requires bbox_valid TRUE on at least one row.
    ARM B / C-Grat   additionally requires bbox_valid FALSE on every row of the
                     event's window.

TIGHTENING 3 — FIVE LEGS. RETURN TO BASELINE.
  The lever goes ON and then OFF again, and the control is RE-RUN after it is
  cleared.

    1. C-Ramp   lever OFF · targeted SM_Ramp2_UAID_B42E9936F5429ADA00_2086822137
    2. C-Grat   lever OFF · targeted
                StaticMeshActor_UAID_A85E45CFE4047BD300_1260804314
       -- lever set ON: BP_Lamp_C_UAID_B42E9936F5429ADA00_2086829166,
          render_custom_depth TRUE, custom_depth_stencil_value 1. VERIFIED SET.
    3. ARM A    lever ON  · same target as leg 1
    4. ARM B    lever ON  · same target as leg 2
       -- lever cleared. VERIFIED CLEAR.
    5. C-Ramp'  lever OFF · same target as leg 1

  Held across all five: level, actor presence, pose (untouched by the owner), seed
  777, session shape, PIE. THE ONLY THING THAT MOVES IS ONE BOOLEAN ON ONE ACTOR.

  C-Ramp' PREDICTION: NOT_MEASURED, customStencilExtent 1x1 — identical to leg 1.
  IF LEG 5 DOES NOT MATCH LEG 1, THE WORLD DID NOT COME BACK and the lever was not
  the only thing that moved. THE ENTIRE RUN IS INVALID. Nothing from A or B is read.

  LEVER VALIDITY, unchanged and binding: customStencilExtent must differ between OFF
  legs and ON legs, read from the M23 PASS / M26S1 NO-PASS lines. Expectation 1x1 vs
  1280x720. NOT inferred from any arm's outcome.

OUT OF SCOPE FOR EVERY LEG OF I11-A
  DO NOT MEASURE SM_GratIng's DRAWN-TO-CLAIMED RATIO IN ANY LEG. The
  complex-silhouette finding is real and is a lane the owner will open deliberately
  after I11-A returns. It is recorded in the journal and acted on nowhere.
