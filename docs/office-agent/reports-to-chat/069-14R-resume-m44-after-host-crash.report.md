# REPORT for 069-14R-resume-m44-after-host-crash.md
brief: 069-14R-resume-m44-after-host-crash.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: medium
started: 2026-09-03 11:46:52 | finished: 2026-09-03 12:19:44 | duration: 33 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-14R-resume-m44-after-host-crash.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 14R — m44: BUILT, GATED, **M44-G1 FAILED → STOPPED**

TREE AS FOUND: exactly the four files the brief named, all edits COMPLETE and consistent with the
plan (the last Edit had applied cleanly, not truncated). Nothing discarded, plan not restarted.
No .git/index.lock, no orphaned UBT/editor/bench processes, no partial Intermediate.

STATE
  master        62bd287  == origin/master   (docs only; m44 did NOT ship)
  source        m44-GATE-G1-FAILED-do-not-merge  dc1282b == origin  (pushed, DO NOT MERGE)
  staged exe    15A31043  (archived as .m44-GATE-G1-FAILED-unvalidated-15A31043)
  A-side        0EF535DC  archived as .m43-logexport-0EF535DC — now LOAD-BEARING
  both targets  game exit 0, editor exit 0. Container unchanged, no cook. NO TAG.
  CaptureBench  tools/m44_gates.py committed (local-only repo)

=== GATES — BOTH TICK ORDERS, IDENTICAL IN BOTH ===
  G2  zero blank PNGs .................. PASS   0 blank (was 61 of 90)
  G3  present==PNGs; sum==rows ......... PASS   23 present / 4 empty / 63 unmeasured = 90
  G3b mask_file non-null iff present ... PASS   0 bad rows
  G3c run_summary reconciles ........... PASS   23 / 4 / 63
  G7  masks ⊆ labelled frames .......... PASS   0 stray  ← the defect the owner saw on Bates
  G6  veto inputs unmoved (STOP gate) .. PASS
  G1  first mask == first label ........ **FAIL 0/4, delta +1 in both orders**
  G5  P-C7 v2 .......................... NOT RUN — it certifies a shipping build

G6 detail vs the m43 control (0EF535DC, banked M44_M44_EXPORT): vetoed_events, translucent_vetoes,
translucency_unknown_vetoes, mask_probe_arms, mask_residual_discards, mask_nopass_discards all
identical (0); event set, every manifested, every frame_indices count, positive_frames (43) all
identical. census_frames 96→97, cycles 31→32, zero 12→13, below_floor 50→49 — one extra census
frame, no verdict and no veto moved (P-C2 / gate-D precedent: a reading).

PER-EVENT SETS (native; synth identical except blink's interior flips 21→18, 45→42)
  blink             _85  16,17,21,22          1stLbl 16  1stMsk NONE   (hidden-class, m45)
  corrupted_texture _73  27..34               1stLbl 27  1stMsk 28   +1
  blink             _73  40,41,45,46          1stLbl 40  1stMsk NONE
  missing_texture   _49  51..58               1stLbl 51  1stMsk 52   +1
  corrupted_texture _49  63..70               1stLbl 63  1stMsk 64   +1
  missing_texture   _49  87,88,89             1stLbl 87  1stMsk 88   +1

WHAT DECIDES EACH mask_state (file:line)
  unmeasured — ArmTargetMaskOwn returned false, at AnomalyCaptureSubsystem.cpp:760 (no live fire
               labelled on this frame / target hidden / no tagged record); ALSO the readback-
               unavailable path in ServiceTargetMask, and the never-resolved path after the
               bounded 4-tick hold in ProcessCompletedFrames.
  empty      — armed AND drained AND the filtered mask has zero kept pixels (ServiceTargetMask,
               KeptPixels == 0). Measured, target drew nothing.
  present    — armed, drained, KeptPixels > 0. The only state that writes a file.
  Liveness for all three: IsFireLabelledThisFrame, which mirrors ResolveAnomalyActiveSource's
  THREE branches (ActorHidden → IsHidden; AnomalyState → IsAnomalyCurrentlyAnomalous;
  FireWindow → live), i.e. the label's own classification.

=== WHY G1 DID NOT PASS — 069-13's ROOT CAUSE WAS CORRECT AND INSUFFICIENT ===
With records created early, the first labelled frame IS armed and IS measured, and the reduce table
reads tableCount=0 for that tag while the next frame reads 48,587–66,862. MASK-TIE says MATCH on
both, so the instrument is right and the stencil is genuinely empty:
    session_index=27 tag=222 tableCount=0     pngCount=0     MATCH
    session_index=28 tag=222 tableCount=48587 pngCount=48587 MATCH
⇒ A NEWLY APPLIED STENCIL TAG IS NOT IN THAT SAME FRAME'S CUSTOM-DEPTH PASS.
The obvious fix was tried and REFUTED: tagging during Tick (flushed by the end-of-Tick
SendAllEndOfFrameUpdates) made it WORSE — 0/4 instead of 1/4. So the lag is NOT about placement
inside frame n. That experiment is reverted; only its measurement is kept. NO MECHANISM ASSERTED:
whether a newly *recreated* proxy joins that same frame's visibility (and so
bHasCustomDepthPrimitives) was not established and must not be guessed (G120).

⛔ SHIPPING AS-IS WOULD BE WRONG, NOT MERELY INCOMPLETE: the first labelled frame would read
mask_state "empty" — measured, drew nothing — while the target is plainly visible (onset at n is
5.9–8.2 % of the picture vs a ~0.5 % baseline). That is a false measurement claim, worse than m43's
blank PNG, which claimed nothing.

TWO WRONG PREDICATES, FOUND BY A DIAGNOSTIC AND NOT BY A GATE
The first build produced ZERO masks and 90 unmeasured rows — and every gate said PASS (G1 was 0/0,
G2 had no files to be blank, G7 none to be stray). G146, four milestones later. A NOT ARMED line
printing `scanned fires/labelled/withRecord/visible` located it: (i) ComputeFireActive is the
hidden-state sampler and reads 0 for FireWindow anomalies, whose frame_indices never depended on
it; (ii) the replacement then treated FireWindow as hidden-based. Only mirroring all three sources
gives the label's answer. "Use the same liveness the labels use" was right — there was more than
one such source and my first two readings of it were both wrong.

DOCS: journal 069 §8 (crash + resume + all measurements + the stop), predictions APPENDIX with the
gate results (append-only, G115), G223 (a record born in the drain is one cycle late) and G224 (a
tie gate and a count gate are both satisfied by a uniformly late artifact). _binary_baselines
README updated with both exes and what depends on them.
DELIBERATELY UNWRITTEN, named: client-readme, client-delivery, ledger m44 entry, card Section F —
they describe shipped behaviour and m44 did not ship. The both-tick-orders rule was NOT added to
CLAUDE.md / PRE-DELIVERY-CHECKLIST for the same reason; it rides the milestone that lands.

NEEDS-DECISION
 D1. The remaining fix is a DESIGN call, not an iteration: the target must be custom-depth-enabled
     BEFORE the frame it is first labelled on. That means tagging at selection/pre-fire time, or
     persisting tags (m42's territory) — both change when stencil tags exist relative to the census
     and m26, i.e. squarely M44-G6 ground. G6 passing here is the good news: the record-creation
     half is proven inert. I did not improvise this inside a gate-failure turn.
 D2. Option B if D1 is too costly this week: ship G2/G3/G6/G7 (which fix the stray-mask defect the
     owner actually reported) and document the +1 as a known limitation, with mask_state on the
     first labelled frame forced to "unmeasured" rather than "empty" so no false measurement claim
     is made. That is a one-line change to the code already on the branch.
 D3. m45 is untouched and unaffected; its source-read first turn (D4 of brief 14) still stands.
```
api-equivalent cost (not billed on Max): $21.94 | turns: 83 | running total across all briefs: $350.96
