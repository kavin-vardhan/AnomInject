# REPORT for 069-28-pairing-neither-regression-bisect.md
brief: 069-28-pairing-neither-regression-bisect.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 21:31:50 | finished: 2026-09-03 21:55:52 | duration: 24 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-28-pairing-neither-regression-bisect.live.txt

---- FINAL MESSAGE (Claude Code) ----
Bisect complete, gate green, everything committed and pushed. Report:

```
SESSION 069 / BRIEF 28 — MASK-PICTURE-PAIRING: THE `NEITHER 54` IS EXPLAINED.
IT WAS THE LEG RECIPE. THE BINARY AND THE ANALYSER WERE BOTH INNOCENT.
THE BATES COOK IS UNBLOCKED.
=================================================================================

FINAL STATE
  master           914f14b == origin/master  (pushed)
                     914f14b docs(m48): MASK-PICTURE-PAIRING - the NEITHER 54 was the leg recipe
  CaptureBench     6b04c50  (local-only repo, 0 remotes — stated, not pushed)
  checkout         LEFT AS FOUND: europa-e1 1f5e305 == origin/europa-e1
  staged exe       DE65F84A — EXACTLY AS FOUND.  container EF8EB23C/A8BFFF88/3C026A8D UNCHANGED.
  NO plugin source change · NO rebuild · NO cook · NO TAG (m48 stays the milestone).
  CHECKOUT SWITCHES, both directions: europa-e1 1f5e305 -> master 20db6e5 -> (commit 914f14b)
                                       -> europa-e1 1f5e305.  Owner's 8 untracked handoffs never staged.

B1 / B2 — THE BISECT TABLE. IT IS A 2x2 AND THE ANTI-DIAGONAL IS EXACT.
=================================================================================
                            | blinking/4242/40          | corrupted_texture/777/90
  m46 60AE8C61              | 33/33  N0  P0  (banked A4)| 25/79  N54 P0   <- B1, NEW
  m48 DE65F84A              | 33/33  N0  P0   <- B4, NEW| 26/80  N54 P0  (banked M48_P4)

  B1 reproduces 79 / 25 / 0 / 54 / 11 FRAME FOR FRAME — identical to the m47 A-side — on
  60AE8C61, THE VERY BINARY THAT PRODUCED THE PASSING BASELINE. The binary is exonerated
  from both sides at once. The reading tracks the RECIPE.

  B2 answered WITHOUT a leg, and better: m44_pairing_probe.py is BYTE-UNCHANGED since eea1a31.
  1496fcc/d0d8643 touched m47_lum_table.py, m47b_*, run_leg_editor.ps1 and run_leg.ps1 —
  never this file. Re-run today over the BANKED baselines it reproduces every number exactly:
  33/33 N0, 35/35 N0 (069-22 A4) and 33/33 N0, 31/31 N0 (069-16 P6).

B3 — THE CAUSE, WITH FILE:LINE, MECHANISM AND DISCRIMINATORS
=================================================================================
  AnomalyCaptureSubsystem.cpp:1408-1409 — the probe wears
  /AnomalyInjector/Materials/M_CorruptedTexture_Pink, LITERALLY the material the
  `corrupted_texture` anomaly swaps its target to, and picture_centroid() finds the probe
  BY THAT MAGENTA. Two magenta objects => the centroid is their average.

  MECHANISM: the target's bbox is [0, 485.2, 306.1, 234.8] — bottom-LEFT — so the extra
  magenta drags picCx LEFT past the 40 px tolerance on exactly the anomaly-present frames.

  FOUR DISCRIMINATORS, all positive:
   1. the NEITHER frames are EXACTLY the anomaly_present frames (si 3-10, 15-22, ...)
   2. picN inflates 8,000 -> 13,600 and grows within each burst, while maskN is rock-stable
      at 10,230-10,330 and maskCx is correct on EVERY frame — the mask half was never in doubt
   3. excluding the target's own bbox collapses picN back to the clean band (8,000-9,100) and
      every delta to 12-33 px => CURRENT
   4. magenta counts split by anomaly_present: 8,493 vs 11,497 colliding; 8,109 vs 8,040 clean

  THIS IS G226 RECURRING ON THE COLOUR AXIS. 069-16 retracted a confident, detailed, entirely
  false "the mask is wrong on a quarter of frames" for this exact reason and fixed the TAG half
  STRUCTURALLY (ReservedStencilMax, which the allocator can never hand out). The COLOUR half was
  fixed only by CHOOSING a non-magenta fixture anomaly — a choice that lived nowhere but in
  whoever typed the leg command. So it regressed the moment a brief picked a different anomaly.

  THE FOUR NAMED CANDIDATES — ALL REFUTED BY MEASUREMENT:
  (a) EXPOSURE CVARS — REFUTED. Read from the leg's OWN LOG ECHO, not the script, as asked.
      The m47/m48 probe legs sent, and the engine confirmed effective:
        r.DefaultFeature.AutoExposure = "0"   LastSetBy: Console
        r.EyeAdaptationQuality        = "0"   LastSetBy: Console
        .Method = "0" · .Bias = "1" · .ExtendDefaultLuminanceRange = "0" ·
        r.EyeAdaptation.MethodOverride = "-1"   (all LastSetBy: Constructor)
      -AutoExposure defaults to 0 and the historical pair WAS issued. Exposure was PINNED.
  (b) m47's ComputeSubsampledMeanLuma / DecodeTightPixel refactor — REFUTED BY AN IN-LEG
      CONTROL: the failing leg's OWN clean frames (si 0,1,2,11,12,14,26,36,37,38) read CURRENT
      at delta 12-30 px. A stride/decode shift moves EVERY frame, not only anomaly frames.
  (c) PROBE TAGGED/RETAGGED by census or prewarm — REFUTED. "MASK-PAIRING PROBE SPAWNED tag=255"
      identical in all legs; tagFailed 0; tagFlips = 0 on the failing legs AND on the passing
      baseline, so 0 is the expected reading here, not a defect.
  (d) the 069-26 bbox fix — NOT APPLICABLE. Different tool (m47_lum_table.py). This analyser
      reads no bbox at all.

B4 — THE FIX (HARNESS), AND THE RE-RUN GATE ON m48 DE65F84A, BOTH ORDERS
=================================================================================
  FIX IS HARNESS-SIDE, so DE65F84A was NOT moved. The gate now establishes its fixture from the
  leg's OWN artifacts (run.json target_anomaly + the run log's probe-spawn line — G119) and
  REFUSES with a non-zero exit and NO NUMBER PRINTED when it cannot see its subject:
  exit 2 no run.json · 3 colliding fixture · 4 no probe. The 40 px tolerance was NOT widened.

  PROVEN BOTH WAYS (G96): FIRES (exit 3) on the banked corrupted_texture leg; SILENT (exit 0)
  on all four passing baselines, whose verdicts return BYTE-IDENTICAL to the pre-fix analyser.
  With the name guard disabled as a known-answer test the numeric path reproduces M48_P4's
  80/26/0/54/10 exactly => the edit changed no arithmetic.
  A NAME LIST IS NOT THE ONLY GUARD: the magenta count is ALWAYS printed split by
  anomaly_present, so an UNLISTED magenta anomaly separates visibly (8,493 vs 11,497) where a
  clean fixture coincides (8,109 vs 8,040).

  RE-RUN GATE, m48 DE65F84A, baseline recipe -Anomaly blinking -Seed 4242 -MaxFrames 40:
    leg                       order   decid  CURRENT  PREVIOUS  NEITHER  no-data
    B28_B4_m48_blink_native   native    33      33        0        0        7     <- matches m46 baseline exactly
    B28_B4_m48_blink_synth    synth     33      32        0        1        7
    B28_L1_m48_blink_synth2   synth     34      34        0        0        6
  All A63-accepted attempt 1, pose_match=True, modal_rot (0,0,0), distinct=1, modal 100%.

  THE ONE NEITHER — RESOLVED BY A RULE FIXED BEFORE THE DECIDING LEGS RAN
  (CaptureBench/tools/b28_si8_predeclared.md, written first and left unedited):
    leg                        binary  si8 picCx  si8 picN  si8 delta  totals
    B22_A4_PAIR_SYN            m46     322.0      8351      33.6       35/35 N0
    B28_L2_m46_blink_synth     m46     316.7      8545      38.9       33/33 N0
    B28_B4_m48_blink_synth     m48     314.9      8653      40.7       32/33 N1
    B28_L1_m48_blink_synth2    m48     317.4      8543      38.2       34/34 N0
  BRANCH (a): the m48 readings STRADDLE 40 (38.2 and 40.7) and the m46 band reaches 38.9 — the
  bands OVERLAP => run-to-run variance at a marginal frame, NOT binary-attributable. maskCx
  355.6 / maskN 10333 are identical on that frame in every leg, so it is PICTURE-side, and
  PREVIOUS == 0 on all four legs (PREVIOUS is the half that detects mis-pairing).
  NO MECHANISM ASSERTED (G120). All four legs banked; none discarded for what it showed.
  CARRIED LIMIT, measured here for the first time: the SYNTH clause sits ~2 px from tolerance at
  si=8 and can read NEITHER 1 by ordinary variance ON EITHER BINARY. A future NEITHER 1 at si=8
  is THIS, not a new finding.

B5 — DOCS
=================================================================================
  journal 069 §20 (§20.1 the baseline · §20.2 the 2x2 · §20.3 instrument exonerated · §20.4 the
  four candidates · §20.5 cause+discriminators · §20.6 the fix · §20.7 re-run gate · §20.8 si=8
  · §20.9 binaries) · G235 ("moved nothing vs the previous binary" is not a gate result; the
  baseline is the LAST PASSING reading; an A-side is only a control if the A-side is itself
  on-baseline; the fixture and instrument are versioned WITH the gate) · predictions APPENDIX 6
  · CLAUDE.md new "you are here" block, and the m48 block's pairing paragraph marked SUPERSEDED
  in place with its now-wrong "pre-existing property of this fixture" reading corrected ·
  _binary_baselines/README updated with the m48 archive and both swap directions.
  Comment stripper 0 changed / 95 no-change. Diffstat checked before commit (G115): 331/4,
  proportionate, no encoding smell — and G141 caught a BOM in a file I'd written with
  Set-Content -Encoding utf8; rewritten through the editor tool.

BINARIES
=================================================================================
  DE65F84A had been STAGED BUT NEVER ARCHIVED. Archived FIRST as
  _binary_baselines\StackOBot.exe.m48-exposuredip-DE65F84A and hash-verified AT THE ARCHIVE
  before any swap (A62). Four stagings, every one hash-verified after the copy, all four stated:
  DE65F84A -> 60AE8C61 (B1) -> DE65F84A (B4/L1) -> 60AE8C61 (L2) -> DE65F84A (restored).
  60AE8C61 is now DOUBLY load-bearing: it produced the passing baseline AND it is what exonerated
  the binary. Do not delete.

FILED, NOT BUILT — flagged for scheduling, neither blocks the cook
=================================================================================
  F1. THE STRUCTURAL COLOUR FIX: give the probe a colour no anomaly uses — the true analogue of
      069-16's tag fix. Not built because it is a BINARY change and would retire the exe every
      other m48 gate ran on, and the brief scoped the re-run to DE65F84A. Until it lands, the
      refusal is what holds the line.
  F2. The si=8 synth-order margin above.

NEEDS-DECISION: none. MASK-PICTURE-PAIRING is green on m48 DE65F84A in both orders on the
baseline recipe, so the cook-time gate no longer blocks the Bates cook.
```

Two things worth your attention beyond the bisect itself.

**The 069-27 report's caution was what made this solvable.** It refused to call the band an m48 regression *and* refused to wave it through, and said in as many words that the clause did not pass. The defect wasn't the caution — it was accepting a same-as-last-binary reading as evidence that a *failing* clause was fine. That distinction is what G235 records.

**The gate's fixture was the unversioned part.** The tag collision got a structural fix in 069-16; the colour collision got a fixture choice that lived only in a typed command. It survived twelve days and two sessions and then regressed silently. The refusal now puts that choice inside the instrument, and the magenta-band printout means an *unlisted* magenta anomaly still shows up rather than passing quietly.
api-equivalent cost (not billed on Max): $21.95 | turns: 104 | running total across all briefs: $664.20
