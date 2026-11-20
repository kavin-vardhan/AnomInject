# REPORT for 069-26-m47b-auto-exposure-attribution.md
brief: 069-26-m47b-auto-exposure-attribution.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 18:15:54 | finished: 2026-09-03 18:41:52 | duration: 26 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-26-m47b-auto-exposure-attribution.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything verified and restored. Final report:

```
SESSION 069 / BRIEF 26 (m47b — AUTO-EXPOSURE) — COMPLETE. DOCS + HARNESS ONLY, PUSHED.
NO SOURCE CHANGE. NO MILESTONE. NO TAG. NO COOK.

=====================================================================
1. THE CHECKOUT (shared-tree rule clause 4) AND BUILD IDENTITY
=====================================================================
  IN   europa-e1 1f5e305 (== origin/europa-e1, tracked tree clean) -> master 93c2b65
  OUT  master b6e5588 (pushed) -> europa-e1 1f5e305 (== origin/europa-e1, tree CLEAN)
  master == origin/master == b6e5588.  One commit: b6e5588 docs(m47b).
  CaptureBench (local-only, no remote): d0d8643 tools(m47b).
  Staged exe VERIFIED F309D836 BEFORE any leg and STILL F309D836 after; quartet
  EF8EB23C/A8BFFF88/3C026A8D unchanged. Editor DLLs were master's (m47 build) so no
  rebuild was needed. Thread 070's checkout untouched; 8 owner handoffs never staged.

=====================================================================
2. THE PREMISE WAS VERIFIED BEFORE IT WAS TESTED — AND IT HELD
=====================================================================
Plugin never touches exposure (one log string, no code). StackOBot's DefaultEngine.ini
sets NO exposure key; neither does BaseEngine.ini. => the game's defaults ARE the
engine defaults: r.DefaultFeature.AutoExposure = 1 (SceneView.cpp:165-170), Histogram,
Bias 1.0, legacy range, Min 0.03 < Max 8.0 (Scene.cpp:468-469) — genuinely ADAPTIVE,
not the Min==Max fake-manual case. SpeedUp 3.0 / SpeedDown 1.0 (tau 10 / 30 frames).
It REACHES THE PIXELS: capture SVE subscribes at VisualizeDepthOfField, AFTER Tonemap.
A48 echo added to BOTH runners on BOTH sides of the switch and read off each log.

=====================================================================
3. A1/A2 — EIGHT LEGS. NO OVERLAP ON ANY STATISTIC.
=====================================================================
 leg  build     anomaly  AE   lum_min  lum_max  spread  max_dip  target_lum   >3%
 AE1  editor    CT       ON    72.662  103.920  31.258   8.61%   90.6-117.5   19
 AE2  editor    CT       OFF  100.490  106.240   5.750   1.73%  123.4-124.7    0
 AE3  PACKAGED  CT       ON    72.419  104.439  32.020   9.01%   90.5-117.8   21
 AE4  PACKAGED  CT       OFF   99.552  106.652   7.100   2.04%  123.7-128.1    0
 AE5  editor    MT       ON    74.524  103.923  29.398   7.27%  115.7-149.3   13
 AE6  editor    MT       OFF  102.739  106.240   3.501   0.97%  148.4-149.9    0
 AE7  editor    CT w40   ON    73.094  103.924  30.830   8.61%   90.8-117.5   17
 AE8  editor    CT w40   OFF  100.138  106.240   6.103   2.39%  123.4-173.1    0
AE-LIVE positive control (G96) PASSES: matched packaged pair, mean 102.488 -> 77.907
(-24%), spread 7.100 -> 32.020 (4.5x). The dip readings are evidence, not blindness.
DOMINANT EFFECT IS A SESSION-START CONVERGENCE TRANSIENT (103.9 -> ~74 over ~30-40
frames), NOT an event-locked dip; steady-state sawtooth is only 1.4-2.1%. AE7 bounds
recovery: after revert 75.06 -> 77.44 over 39 frames AND STILL RISING (SpeedDown 1.0).

=====================================================================
4. A3 — 🚨 THE DIP IS NOT EDITOR-ONLY. THE CLIENT WOULD SEE IT.
=====================================================================
Packaged reproduces editor almost exactly: spread 32.020 vs 31.258, max dip 9.01% vs
8.61%, target 90.5-117.8 vs 90.6-117.5. Packaged A48 echo reads AutoExposure="1"
(Constructor), EyeAdaptationQuality="2" (Scalability). This kills the working
assumption that these symptoms were editor-side.

=====================================================================
5. A4 — THE BLACK-FRAME GATE DOES NOT FIRE, AND THAT IS CORRECT
=====================================================================
--selftest passes grey, FAILS all-black (so it can fire). Deepest AE-ON leg:
min 72.419 vs threshold 6.0 => 0 BLACK FRAMES, 0 DARK FIRST FRAMES, VERDICT PASS,
12x margin. ⚠ The 6.0 was derived from a PINNED-exposure darkest frame (59.992). It
survives the regime change BY LUCK OF DIRECTION, not because AE was considered.

=====================================================================
6. PER-SYMPTOM VERDICTS
=====================================================================
 (2) "target renders BLACK"  🚨 AE REFUTED STRUCTURALLY, not by a null: eye adaptation
     is GLOBAL and cannot darken one object alone. DARK FIRST FRAMES = 0 on every leg.
     Lit-no-emissive candidate still UNTESTED — no dark target has ever occurred.
 (3) "whole picture black"   AE-PARTIAL. Real, large, previously unmeasured, in BOTH
     builds — but nothing approaches black (darkest 72.4/255, deepest drop 9.01%).
 ⚠ G135 GUARD, PRE-DECLARED: target is 7.2-7.8% of frame. This null does NOT exclude
   auto-exposure on the owner's content. Do not upgrade it.
 ⚠ NEW CONFOUND: under AE the first event fires while exposure is still converging, so
   its target reads 117.8 vs a session mean of 95.0. THAT LOOKS LIKE AN ONSET EFFECT AND
   IS NOT ONE. Any onset reading taken with AE live must control for session position.

=====================================================================
7. TWO CORRECTIONS, RECORDED NOT SMOOTHED
=====================================================================
(a) The pre-declaration called the DIRECTION correctly (first frame brighter, against
    the brief's clause) but FROM A WRONG PREMISE: it argued the swap is brighter than
    the scene; a pinned-exposure read shows both swaps are DARKER (CT -17.7%, MT -3.5%).
    Real driver is session position. ⛔ NO MECHANISM ASSERTED for the per-event sign.
(b) CHECKER DEFECT caught before its verdict was read (G142): bbox_px is [x,y,W,H]
    (AnomalyLabelWriter.cpp:95), not [x0,y0,x1,y1]. Read wrongly it degenerates to a
    1-pixel strip and returned 240 instead of 131. m47_lum_table.py had the same bug —
    LATENT ONLY, every m47 leg resolved region='mask', so no m47 number came through it.

=====================================================================
8. TASK B — m48 RECOMMENDATION (PROPOSED, NOT BUILT)
=====================================================================
A3 shows the dip in packaged, so per the brief's contingency: (i)+(ii) as m48, small.
 (i)  additive per-frame `exposure_dip: true` (emitted only when true, so a healthy run
      gains no key — the m47 precedent that kept labels.jsonl at 21 fields) +
      run_summary `frames_exposure_dip`.
      THRESHOLD 4.0% — DERIVED TWO-SIDED, stronger than m47's one-sided 6.0: every
      exposure-pinned leg's max drop <= 2.39%, every AE-ON leg reaches 7.27-9.01%; any
      value in (2.39, 7.27) separates perfectly; 4.0 sits 1.67x above the control
      ceiling and 1.82x below the lowest AE-ON max. Expected yield ~9-12 frames of 90.
 (ii) one client-readme paragraph: the game's auto-exposure re-adapts for ~a second
      after a large texture anomaly and at session start; those frames are marked.
 (iii) ⛔ the plugin must NOT force exposure. The dataset should look like the game.
NEXT DISCRIMINATOR if (2)/(3) stay open: Lumen surface-cache invalidation; and for (2)
specifically a fixture that actually produces a dark target — without one the
lit-no-emissive candidate has nothing to discriminate.

Docs: prediction 2026-09-03-m47b-auto-exposure.md (written BEFORE any leg), journal
069 §18, ledger §11, G233, CLAUDE.md status. master tip b6e5588.
STOP.
```

**In plain terms:** auto-exposure turned out to be a large, completely unmeasured variable — every bench leg this project has ever run had it switched off, while the build the client receives has it on. Turning it on drops the whole picture's brightness by a quarter and makes the *same* anomaly render 23% darker at the end of a session than at the start.

But it isn't the reported symptom. It can't blacken a single object (it's a global operator, so symptom 2 is refuted structurally rather than by a failed search), and at this fixture's 7% target it never gets near black. I pre-declared that a 7% stimulus might be too small to show it, so I've flagged the null as *not* excluding auto-exposure on the owner's own content.

Two things worth your attention: the dip appears in the **packaged** build too, so it isn't editor-only as assumed — that's what makes m48 worth doing. And the effect creates a trap for future work: under auto-exposure the first event of a session looks like it has an onset artefact, and it doesn't; it's just where in the session it fired.
api-equivalent cost (not billed on Max): $18.91 | turns: 94 | running total across all briefs: $616.38
