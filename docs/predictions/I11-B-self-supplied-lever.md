I11-B — DOES H6 FIRE IN THE SHIPPING PATH WITH NO CONSTRUCTED LEVER?
Pre-declared 2026-08-20, BEFORE reading any of the quantities below. Restate this file
VERBATIM from the committed copy before reading any result.

THE QUESTION
I11-A supported H6 using a lever we built on purpose: one unrelated actor set to write
custom depth. That establishes the MECHANISM and says nothing about whether a real run
reaches it. I11-B asks whether the plugin supplies its OWN writer through its own
accumulated tags, with nothing constructed.

WHY IT COMES BEFORE ANY FIX — OWNER RULING, RECORDED
  1. We proved the mechanism with a lever we built. Whether the SHIPPING PATH supplies
     its own writer is the question that shapes the fix, and it is unanswered.
  2. I11-B IS THE BEFORE PICTURE. Fix first and we lose the ability to demonstrate the
     fix changed anything in a real run.
  3. It may be free — the banked play-gate smoke may already contain the answer.
NO FIX IS AUTHORISED, DESIGNED, PROPOSED OR QUEUED.

THE DECLARED FACT THAT MAKES STAGE 1 DECISIVE RATHER THAN SUGGESTIVE
MainWorld has ZERO pre-existing custom-depth writers — 982 primitive components scanned
across 425 actors, I11-A, direct property read. So in a banked MainWorld run, A
VIEW-SIZED EXTENT CAN ONLY HAVE COME FROM OUR OWN TAGS.

================================================================================
DECLARATION OF PRIOR EXPOSURE — this pre-declaration is over PARTLY-SEEN evidence
================================================================================

A pre-declaration over partly-seen evidence is WEAKER than one over unseen evidence, and
it is weaker in a way that must be VISIBLE IN THE RECORD rather than glossed. Journal 045
PART THIRTY-TWO was read at this session's cold start and it reports some of the smoke.

  S1  customStencilExtent           NOT SEEN. Never reported for the smoke, in any doc.
  S2  per-event M26S1 EVENT         PARTLY SEEN. Already known: state for all 8 events
                                    (4 MEASURED_ZERO, 4 MEASURED_NONZERO); "M26S1
                                    SUMMARY events=8 notMeasured=0"; framesNoPass=1,
                                    collisions=1 and framesContributed=3 on THREE of the
                                    four vetoed events; maxCount in px for the four kept
                                    events (4,519 / 54,779 / 3,800 / 6,661); and
                                    mask_nopass_discards=3 for the run.
                                    NOT seen: ArmsIssued on any event; the FOURTH vetoed
                                    event's buckets; maxCount on the vetoed events.
  S3  tagged actors + Nanite + bbox PARTLY SEEN. The target IDENTITIES are known
                                    (vetoed: BP_Stomper_C x2, RoomBuilderSquare_C x2;
                                    kept: InstancedFoliageActor_0_-1_0, BP_SplineSpawn_C,
                                    BP_Elevator_C, InstancedFoliageActor_0_0_0), as are
                                    the assets named in PART THIRTY-TWO's Finding 2.
                                    NOT seen: their Nanite status by direct property
                                    read, and their bbox_valid.
  S4  veto counters                 FULLY SEEN. countedEventsBefore=8,
                                    countedEventsAfter=4, vetoedEvents=4,
                                    nonManifestedEvents=0.

CONSEQUENCE, STATED NOT MITIGATED: S1 is the only quantity of the four that is wholly
fresh, and S1 is also the one the branches turn on. S4 carries no predictive weight here
because it was fully known before the question was posed. Any verdict must be read with
this declaration attached.

================================================================================
STAGE 1 — BANKED EVIDENCE ONLY. NO RUN. NO KEYSTROKE. NO LEVER.
================================================================================

Read from the banked play-gate smoke session, and any other banked AUTO-POOL session on
MainWorld, IN THIS ORDER, and only these quantities:

  S1  customStencilExtent from the M23 PASS lines - 1x1 or view-sized, per armed frame
  S2  per-event M26S1 EVENT: state, framesNoPass, framesContributed, maxCount, ArmsIssued
  S3  the set of actors TAGGED during the run, each one's Nanite status by direct
      property read, and its on-screen status from labels.jsonl bbox_valid
  S4  vetoedEvents / countedEventsBefore / countedEventsAfter

BRANCHES — pre-registered, every run lands in exactly one

  Y-1  SELF-SUPPLIED LEVER CONFIRMED.
       Extent view-sized on >=1 armed frame, AND >=1 MEASURED_ZERO event whose target is
       Nanite or off-screen (i.e. could not have contributed evidence), AND >=1 other
       tagged target on-screen and non-Nanite.
       -> H6 fires UNAIDED. Stage 2 is not needed for the mechanism claim, only for a
          packaged one.

  Y-2  Every MEASURED_ZERO event's target is non-Nanite AND on-screen.
       -> those zeros are genuine measurements. H6 not implicated in that run.

  Y-3  Extent never view-sized on any armed frame.
       -> the plugin does not supply its own lever; H6 needs an external writer, which is
          a different and narrower risk.
       NOTE: if Y-3 co-occurs with framesContributed > 0 anywhere, the two readings
       CONTRADICT and it is Y-4, not Y-3.

  Y-4  Any required quantity absent, or readings contradictory.
       -> UNDECIDABLE (A60). Never reconstructed, never replaced by a weaker reading
          reported as the original. Go to Stage 2.

WHAT EACH OUTCOME MEANS — pre-registered so the verdict is not argued afterwards

  Y-1 -> H6 reaches the shipping path unaided. It becomes a CANDIDATE explanation for the
         play-gate smoke's four vetoes - A CANDIDATE, NOT THE ANSWER. DO NOT ATTRIBUTE
         THOSE FOUR. An observation and its explanation are separate claims (G120), and
         that specific run is not re-derivable from a mechanism that merely could produce
         it.
  Y-2 -> the smoke's zeros stand as genuine, the RoomBuilderSquare_C question returns
         open, and I11's original branch table (Z-1..Z-5) is back on the table.
  Y-3 -> the risk is host-supplied writers only. Different fix, different urgency.
  Y-4 -> Stage 2.

================================================================================
STAGE 2 — PACKAGED, UNATTENDED. ONLY IF STAGE 1 RETURNS Y-4, OR Y-3 NEEDING
CONFIRMATION. PRE-APPROVED IN SHAPE; PLAN FIRST.
================================================================================

  Staged bench build, exe 5EA6AB92 - VERIFY THE FULL QUARTET FIRST (G121: an exe hash is
  half a build). MainWorld. AUTO-POOL, UNSHAPED - no coverage or poll-radius changes,
  because shaping the selectable set mutates the very behaviour under test. Mask ON.
  Multiple legs, differently seeded, every attempt banked BY SESSION ID.
  The leg count is proposed and justified at plan time. IT IS A SAMPLE SIZE, NOT A
  VALIDITY CONSTANT.

  The run_leg.ps1 auto-pool parameter is PRE-APPROVED - bench tooling, same class as -Map
  and -Target. NOT to be widened beyond driving auto-pool.

  DELIVERY MODE: RUN IT OFF. The argument, WHICH MUST BE CHECKED FROM SOURCE RATHER THAN
  ACCEPTED: with delivery ON, vetoed events vanish from annotation.json and labels.jsonl
  is not written, so S3's on-screen evidence would not exist at all. Delivery OFF is the
  only observable configuration. The belief to be confirmed is that THE VETO AND MASK
  PATHS ARE IDENTICAL IN BOTH MODES and delivery changes only what is WRITTEN.
  IF THAT IS NOT TRUE, STOP AND REPORT - do not run a leg whose mode difference is
  unexamined.
  RECORD THE RESIDUAL EITHER WAY: the client captures IN delivery mode, and that gap is
  already a standing P1 concern.

  PACKAGED LICENSES WHAT PIE DOES NOT. A Stage 2 result CAN speak to the shipping path.
  Say so explicitly in the result rather than carrying I11-A's PIE caveat over by habit.

================================================================================
STANDING CONSTRAINTS
================================================================================
  The veto rule is ZERO-ONLY and is LOCKED. No ratio, no threshold, ever proposed.
  P6 does not move. CB_GateLevel untouched (G99). Stencil range 200/255.
  feature/stencil-capture READ-ONLY at 76cac74. The m26 tag is NOT rewritten.
  L3 IS LOAD-BEARING FOR THIS INVESTIGATION and is not to be "fixed" while it runs.
  DO NOT MEASURE SM_GratIng's DRAWN-TO-CLAIMED RATIO. That lane opens deliberately,
  later, and not here.
  No same-turn fix to anything, whatever comes back. Report Stage 1 and STOP.

  AMENDMENT RULE, unchanged from I11-A: a pre-declaration may be amended before any
  measurement exists, and ONLY to TIGHTEN a validity condition. Never to loosen one, and
  never a prediction or a verdict once the instrument exists. Every amendment is dated,
  appends, and deletes nothing.

================================================================================
OUTCOME RECORD — 2026-08-20. NOT AN AMENDMENT.
================================================================================

THIS IS AN OUTCOME RECORD, NOT AN AMENDMENT. Nothing above it is changed. No prediction,
no verdict, no branch and no validity condition has been touched — the instrument is
recorded exactly as it was committed before the readings were taken.

STAGE 1 RESULT: BRANCH Y-1. SELF-SUPPLIED LEVER CONFIRMED.
  S1  extent view-sized (876x872) on 26 armed frames of the auto-pool smoke, and 1x1 on
      6. MainWorld has zero pre-existing custom-depth writers, so that extent came from
      OUR OWN TAGS.
  S2  8 events, notMeasured=0. Four MEASURED_NONZERO (4,519 / 54,779 / 3,800 / 6,661 px)
      and four MEASURED_ZERO.
  S3  BOTH MEASURED_ZERO targets are WHOLLY NANITE by direct property read —
      BP_Stomper_C (SM_Fan_Frame, SM_GenericMachine_Engine) and RoomBuilderSquare_C (four
      ISM components: SM_Modular_Window, SM_Modular_Wall_InnerCorner, SM_Modular_Wall_250,
      SM_FloorBase). Every event in the run reads bbox_valid TRUE on all its rows, so
      nothing was off screen. The other tagged targets are non-Nanite and drawing.
  S4  countedEventsBefore=8 countedEventsAfter=4 vetoedEvents=4 nonManifestedEvents=0.

  All three Y-1 clauses met. Per the pre-registered meaning: H6 fires UNAIDED, and Stage 2
  is not needed for the mechanism claim.
  ⛔ AND THE RESTRAINT THAT CAME WITH Y-1, HELD: the smoke's four vetoes are NOT
  attributed to H6. H6 was PRESENT AND ACTIVE in that run; that is not the same as having
  caused them (G120).

  WEIGHT: as declared before reading, S4 was fully seen and carries no predictive weight,
  S2 and S3 were partly seen, and S1 was wholly fresh. THE VERDICT RESTS ON THE FRESH
  QUANTITY.

I11-B IS CLOSED AT STAGE 1 BY OWNER DECISION, 2026-08-20.
STAGE 2 IS UNRUN — NOT CANCELLED. Its design above stands as written and is to be used
unchanged if it is ever needed. DO NOT DELETE THIS FILE.

CONSEQUENT DECISION, recorded elsewhere and pointed at from here rather than restated:
H6 IS DOCUMENTED, NOT FIXED. No m27, no fix, no veto-default change. The entry of record
is docs/invisible-anomaly-mechanisms.md → "H6 — DOCUMENTED, NOT FIXED".
