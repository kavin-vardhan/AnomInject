# REPORT for 069-22-bench-cook-and-f1-validation.md
brief: 069-22-bench-cook-and-f1-validation.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-03 14:57:00 | finished: 2026-09-03 15:18:55 | duration: 22 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\069-22-bench-cook-and-f1-validation.live.txt

---- FINAL MESSAGE (Claude Code) ----
```
SESSION 069 / BRIEF 22 — TWO COOKS DONE, master RE-BASELINED, m46 (F1) VALIDATED AND MERGED.

FINAL STATE
  master  7284c6e == origin/master  (pushed)
            7284c6e  docs(m46): two cooks, master re-baselined, F1 validated
            80b382d  merge: m46 resolution mapping
            866aa0e  feat(capture): m46 mask pass maps output pixels through the internal view rect
  staged  exe 60AE8C61 · utoc EF8EB23C · ucas A8BFFF88 · pak 3C026A8D   (the m46 container)
  builds  game exit 0, editor exit 0.  NO TAG — office batch … m43 → m44 → m45 → m46.
  m44-f1-resolution-mapping-UNVALIDATED DELETED (local + origin); its content is on master.

=== TASK A — THE FIRST COOK (runbook §8.6, verbatim) ===
 STEP 0  disk 188 GB free (floor ≥15 GB GO).  Nothing in flight.
 Step 1  RE-BANK (G92): 16 unbanked sessions found BY SESSION ID — three M43G_LBOX attempts,
         M45_SYNTH_try1, a 2026-08-23 AnomalyCaptures session and others — rescued to
         B22_PRECOOK_RESCUE, 16/16, per-file counts matching.
 Step 2  quartet archived and hash-verified AT THE NEW LOCATION (A62).
 Step 3  MAP SET DECLARED IN WRITING BEFORE THE COOK: CB_GateLevel + MainMenu + MainWorld (+ Entry).
 Step 3.5 editor target rebuilt; A44 on the editor dll: HideOmitDepthPassSilencing ×4, HideMode ×4,
         control `blinking` ×9.  ⚠ The runbook's example control IsHideTypeAnomaly reads 0 — it is
         STALE (renamed at session 053); `blinking` was used instead, so the scan is sound not blind.
 COOK    BUILD SUCCESSFUL, exit 0, 1 m 6 s.
 Verify  MAP GATE PASS exit 0, all four maps read out of the CONTAINER INDEX (G119).
         TOKEN READ-BACK 64 chars from DefaultGame.ini, not the placeholder (G118).
         A44 on the staged exe, both encodings, live control.  No missing global shader.  Boots.

  quartet            exe        .utoc      .ucas      .pak
  pre-cook           8A895272   2A66CA57   A7EF9B12   D8009AD7
  m45 post-cook      38A55169   4621F571   513F4D35   2163A13A
  m46 (live)         60AE8C61   EF8EB23C   A8BFFF88   3C026A8D    — all three archived

=== A4 RE-BASELINE — THE COOK CHANGED NO BEHAVIOUR (G103) ===
  gate                          native        synth
  ONSET                         6/6           6/6
  G7 equality                   35=35 stray 0 35=35 stray 0
  G2 blank PNGs                 0             0
  G3 counts                     35+0+55=90    35+0+55=90
  MASK-TIE                      35 / 0        35 / 0
  MASK-PICTURE-PAIRING          33/33, N0 P0  35/35, N0 P0
  m26 known-answer              mask_probe_arms=1
  census                        candidates 77 · zero 13 · nonzero 62 · belowFloor 49 · tagFailed 0 ·
                                hidden 2 · framesPolluted 0 · batchesLost 0 · tagOvertaken 3
  EVERY ONE identical to the pre-cook container; only census_frames/cycles move at run-to-run scale.
  These legs are master's new P-C7 controls.

=== TASK B — m46, THE DECISIVE GATE ===
  r.ScreenPercentage 50 (internal 640×360 vs output 1280×720):
      BEFORE (banked 069-16 leg)   CURRENT  0 of 26 decidable, NEITHER 25
      AFTER                        CURRENT 35 of 35,           NEITHER 0, PREVIOUS 0
  100 % leg unchanged: 33 of 33 CURRENT, NEITHER 0.
  ONSET 6/6 · G7 equality 35=35 · G2 0 · G3 35+0+55=90 · MASK-TIE 35/0 · m26 probe fires ·
  census framesPolluted 0 / batchesLost 0 / tagFailed 0 — all both orders.
  Veto inputs, positive_frames and the three target_mask_* counters identical to the A4 control.

 🚨 P-C7 v2 LOOKED LIKE A FINDING AND WAS NOT. 5 of 35 mask silhouettes differ against the pre-m46
 control, one by 23,198 px at session_index 52. A SAME-BUILD control reproduces it EXACTLY — same 5
 frames, same frame, same 23,198 px symmetric difference, the two pixel counts merely swapped
 (66,635 ↔ 89,833). Census run-to-run variance, not m46 (G169). Without that control I would have
 reported it as a defect.

=== ONE THING I CAUGHT BEFORE COMMITTING ===
 docs/gotchas.md came back as 11,595 changed lines for a 4-line annotation — the G115 encoding smell
 from a PowerShell ReadAllText/WriteAllText round-trip. I halted the commit per the standing rule,
 restored the file from HEAD, and re-applied the annotation with the editor tool. Final diffstat is
 141 insertions / 10 deletions across six files, all additive.

DOCS: CLAUDE.md invariant (iii) PENDING → LANDED and the status block now carries m46 and both cooks;
G225 annotated FIXED; the nearest-neighbour mask-resampling follow-up RETIRED (the mapping is nearest
by construction); client-readme ("masks are correct at any screen percentage"); client-delivery (the
50 % leg joins MASK-PICTURE-PAIRING as a cook-time gate, with the requirement that internalViewRect
must actually differ from viewRect or the leg is vacuous); ledger §13; journal 069 §16; baselines
README with all three quartets.

NEEDS-DECISION: none.
```
api-equivalent cost (not billed on Max): $26.41 | turns: 37 | running total across all briefs: $490.15
