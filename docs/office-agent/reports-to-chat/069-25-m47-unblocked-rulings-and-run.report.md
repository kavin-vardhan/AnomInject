# REPORT for 069-25-m47-unblocked-rulings-and-run.md
brief: 069-25-m47-unblocked-rulings-and-run.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 17:32:35 | finished: 2026-09-03 18:14:34 | duration: 42 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-25-m47-unblocked-rulings-and-run.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything is in place. Final report:

```
SESSION 069 / BRIEF 25 (m47 shader-readiness) — COMPLETE. SHIPPED, MERGED, PUSHED. NO TAG.
Office batch: … m44 → m45 → m46 → m47.

=====================================================================
1. THE CHECKOUT — BOTH DIRECTIONS, AS RULED (ND-1)
=====================================================================
  IN   europa-e1 1f5e305 (== origin/europa-e1, tracked tree clean) -> master 489c29d
  OUT  master 93c2b65 (pushed) -> europa-e1 1f5e305 (== origin/europa-e1, tracked tree CLEAN)
  master == origin/master == 93c2b65.  Two commits: 3549312 feat + 93c2b65 docs.
  Staged exe verified against _binary_baselines/README before any leg: it was the m46 quartet
  60AE8C61, untouched by thread 070. Committed DIRECTLY on master (no branch, so no merge commit).

⚠ FOR THREAD 070, AND IT IS THE NEW SHARED RULE IN ACTION: the staged bench exe is now
  F309D836 (m47, built from master) and the EDITOR DLLs on disk are master's too. 070 must
  rebuild for europa-e1 and re-stage before any leg. Its source checkout is untouched.

=====================================================================
2. THE HEADLINE — A NEGATIVE RESULT, AND IT IS THE STRONG KIND
=====================================================================
Chat's hypothesis is REAL AS A MECHANISM and DID NOT REPRODUCE, at maximum forcing.

Under -nomaterialshaderddc (a cold material DDC — the owner's exact "first use per
combination, cached afterwards" shape) BOTH swap materials were INCOMPLETE ON 90 OF 90
ARMED FRAMES with 3760 shader jobs outstanding, and corrupted_texture still drew correct
magenta on EVERY frame INCLUDING EVERY EVENT'S FIRST:
    target luminance 106.883–124.727  vs a warm control's 106.894–127.000
    zero black frames, zero dark first frames, on every leg, editor and packaged.

🔑 WHY, AND IT IS THE FINDING: completeness is a WHOLE-SHADER-MAP property while a draw
needs ONE VERTEX FACTORY, and FLocalVertexFactory (a plain static mesh) compiles first.
⇒ The fallback window is wide only for VFs that compile LATE — skeletal, Nanite, cloth —
which is where the owner's Concorde symptom (a held weapon) sat and where m30's usage-flag
defect sat. THAT LEG WAS ATTEMPTED TWICE AND DID NOT RUN (MainWorld target "Bot" -> 0
matched; MainMenu failed to load under -game), so it is UNTESTED, not a null. It is the
named next discriminator.

=====================================================================
3. A1 — EDITOR TARGET (new harness run_leg_editor.ps1)
=====================================================================
 leg                          pending    incomplete  frame_lum min  target_lum   black dark1st
 E1 first run after build     157 -> 73  (uninstr.)   60.111        106.9–127.0    0     0
 E1 second run (warm)         0          0/90        100.787        —              0     0
 E2 ForceAnomalyShaderRecompile 0        0 LEVER DID NOT FIRE       —              0     0
 E2b -nomaterialshaderddc     3760->3672 2 on 90/90    59.992       106.9–124.7    0     0

A3: materials ARE CDO hard-refs at module start (a COOK guarantee, not a shader-readiness
one); the 7 usage flags are still the right list (the reachable component set has not
widened). No "missing bUsedWith" line on any leg.

ND-4 ANSWERED BY A READ: 14 of 14 flags (7 × 2 materials) already True on disk, "already
correct - no change made", exit 0 ⇒ the m30/Concorde default-material route is EXCLUDED on
this bench. The script saves unconditionally, so the two .uassets came back byte-different
with IDENTICAL flag values — re-serialisation, not a change. THAT CHURN WAS REVERTED, so
the tree still matches the bytes the m46 cook was made from (verified: 039E934C1E79).

=====================================================================
4. PER-SYMPTOM VERDICTS (ND-3 applied; no mechanism asserted without its discriminator)
=====================================================================
 (1) "swap not visible on first frame(s)"  CONSISTENT with the mechanism, NOT OBSERVED here.
 (2) "target renders BLACK"                UNEXPLAINED. The mechanism's fallback is the
     engine GREY grid, not black. The lit-with-no-emissive candidate is UNTESTED — no dark
     target region ever occurred to attribute, so the emissive lever had nothing to
     discriminate. ⛔ NO MECHANISM ASSERTED.
 (3) "whole picture black for a burst"     UNEXPLAINED and NOT REPRODUCED. A per-material
     fallback cannot blacken a whole frame. Named, untested: auto-exposure (the bench runs
     it OFF, the owner's editor does not) and Lumen surface-cache invalidation.
 ND-3's mask discriminator was implemented and READY (m47_lum_table.py reads the m43 mask on
 every frame) and never had a black frame to read. Stated, not counted as a pass.

=====================================================================
5. TWO GATE-DESIGN DEFECTS FOUND AND FIXED BEFORE SHIPPING  (→ G232)
=====================================================================
(a) THE BRIEFED COUNTER IS PROCESS-GLOBAL. GetNumRemainingJobs() read 157 falling to 73
    across all 90 frames of a HEALTHY editor leg. render_state keyed on it would have marked
    90 OF 90 GOOD FRAMES — true, useless, and it trains the reader to ignore the key.
    ⇒ SHIPPED GATE KEYS ON THIS PLUGIN'S OWN TWO MATERIALS; the global number ships beside
      it as a reading (shader_jobs_pending).
(b) EVEN THAT PREDICATE OVER-FIRES — IsComplete() was false on all 90 E2b frames while the
    drawing VF was ready. It errs toward marking good frames suspect (the safe direction) and
    the log line says so.
ND-2 ACCEPTED AND NOW MEASURED: in packaged, EnsureIsComplete's body is WITH_EDITOR and the
prewarm costs 0.0017 ms, so frames_shaders_pending == 0 there is a READING. The packaged
claim rests on the pixel gate. The checklist box says so explicitly.

=====================================================================
6. A2 + GATE TABLE — PACKAGED, BOTH TICK ORDERS
=====================================================================
 leg  prewarm            prewarm_ms  incomplete  fsp  lum min  black dark1st ONSET     G7
 P1   OFF                −1 skipped  0/90        0    99.467   0     0       8/8 Δ0   stray 0
 P2   ON                 0.0017      0/90        0    99.611   0     0       8/8 Δ0   stray 0
 P3   ON + SynthTickOrder 0.0019     0/90        0    99.312   0     0       8/8 Δ0   stray 0
All three: 90 frames, 59 positive, vetoed 0, non-manifested 0, speed_ratio 1.0000,
target_mask 59, B1 PASS (modal_rot (0,0,0), distinct=1, modal 100%), A63 accepted attempt 1.
P-C7 v2: run_summary 55 -> 58, exactly the three new keys, 0 removed. labels.jsonl field set
21 -> 21 IDENTICAL. annotation.json 48 keys IDENTICAL — P6 DID NOT MOVE.
BOTH BUILD TARGETS EXIT 0 (G221).
BLACK-FRAME GATE: threshold 6.0 DERIVED (darkest legitimate frame 59.992, an order of
magnitude above); --selftest PASSES a synthetic mid-grey and FAILS a synthetic all-black —
proven both ways, G96.

=====================================================================
7. BUILD IDENTITY
=====================================================================
Staged exe F309D836. Predecessor 60AE8C61 archived as StackOBot.exe.m46-resolution-60AE8C61
and hash-verified AT the archive. Container quartet UNCHANGED (EF8EB23C/A8BFFF88/3C026A8D) —
code-only hot-swap, NO COOK: m47 adds no shader and no shader parameter.
⚠ A44 CAUGHT A REAL STALENESS: the first staged exe (2C167ADE) was linked before the second
round of edits — two new symbols read 0 while IAI.Capture.ShaderPrewarm read 7 and the
pre-existing IAI.Bench.MaskPairingProbe read 5, SO THE SCAN WAS SOUND, NOT BLIND. Rebuilt,
re-staged, re-scanned. 2C167ADE was overwritten unarchived; no gate ran on it; stated in
_binary_baselines/README.
⚠ IAI.Bench.ForceAnomalyShaderRecompile CANNOT FIRE ON A WARM DDC (incomplete BEFORE=0
AFTER=0). The lever that worked is the ENGINE switch -nomaterialshaderddc. The plugin lever
ships anyway because its PACKAGED REFUSAL is its own measurement. (m45's shadow-lever shape.)

=====================================================================
8. DOCS / NEEDS-DECISION
=====================================================================
CLAUDE.md status refreshed to m47 + SHARED-TREE RULE clause 4 (two threads, check out the ref
you need, verify == origin, leave it as found, verify the staged exe before any leg).
gotchas G231 + G232. Journal 069 §17. architecture.md m47 section. PRE-DELIVERY-CHECKLIST two
new boxes. client-readme one paragraph. _binary_baselines/README. CaptureBench (local-only)
commit 1496fcc for the two new harness tools.
📌 G232 was minted for the counter-scope lesson, NOT for the lighting mechanism — the brief
   reserved it for a confirmed lighting mechanism and there is none. Stated so the numbering
   is not misread.

NEEDS-DECISION:
 ND-A (the one that matters): symptoms (2) and (3) are UNEXPLAINED and the vertex-factory
   prediction is UNTESTED. Do you want a skeletal/Nanite cold-DDC leg next? It needs a
   fixture with a skeletal target that a targeted fire can resolve — CB_GateLevel has none,
   MainWorld's pawn does not match "Bot", MainMenu will not load under -game. That is a
   fixture decision, so I stopped rather than improvise one (G99/G135).
 ND-B: per the brief's contingency — the prewarm SHIPS (it fixes (1) and is free in
   packaged), and the fix for (2)/(3) is NOT chosen. Options as promised, unchosen:
   (i) an unlit-emissive variant with a Lumen-safe colour (re-opens G50's bleed),
   (ii) accept-and-mark via the pixel gate (what m47 ships today),
   (iii) test auto-exposure first — one editor leg without the AE-off cvars, cheapest of
        the three and the only one that could still explain symptom (3).
   Chat decides.

STOP.
```

**In plain terms:** chat's hypothesis turned out to be a real engine mechanism that doesn't produce the reported symptom here — and I could say that confidently because I forced the condition to its maximum (both materials incomplete on every single frame, 3760 shader jobs queued) and the pixels stayed correct. The reason is the useful part: shader-map completeness is asked of the whole material, but a draw only needs one vertex factory, and the common one compiles first. That predicts the problem bites on skeletal/Nanite targets — exactly where the owner saw it — but I could not get a skeletal fixture to fire, so that stays untested rather than dressed up as a null.

Two things I'd flag beyond the result: the gate as briefed would have marked 90 of 90 good frames because `GetNumRemainingJobs()` is process-wide, so I re-pointed it at this plugin's own materials; and the black-frame symptom never appeared at all, so the "lit material with no light" theory has nothing to attribute to it and I left it untested rather than asserting it.
api-equivalent cost (not billed on Max): $40.19 | turns: 178 | running total across all briefs: $597.47
