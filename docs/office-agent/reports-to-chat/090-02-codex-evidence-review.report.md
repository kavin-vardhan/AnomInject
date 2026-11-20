# REPORT for 090-02-codex-evidence-review.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-02-codex-evidence-review.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: high
started: 2026-09-30 09:56:10 | finished: 2026-09-30 10:37:36 | duration: 41 min | exit code: 0
context at last turn: 336069 tokens (34% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-02-codex-evidence-review.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Plain summary for the owner:** Codex reviewed the delivery's label-sync claims against the saved evidence and did not approve. It found one real way a label can say "corrupted" after the picture is already clean, and several places where the client readme claims more than the tests showed. It judged none of them blocking. Nothing was changed; chat decides what to do.

**Codex's verdict: CHANGES REQUIRED** (Astra at effort max, 2,336 s, exit 0; the requested and actual model and effort agree). The report is collected unedited at `_reviews\090-02-codex-evidence-review.md` with one header line added, the body is byte-identical, and the ledger row is added. It asks for chat review before merge, especially of F1–F4.

**Findings:**
- **F1, HIGH:** a texture-corruption label can outlive the effect. If the effect is reverted early by hand, or the host replaces the material, the labels stay positive until the scheduled end. This existed before the final round. The existing exclusion selftest encodes it: apply at 10, revert at 20, still labelled 10–39. The cause is that FireWindow membership (`AnomalyLabelSync.h:63`) ignores whether the effect is still active. This comes from reading the source; no capture has shown it. It contradicts the readme's "from apply to revert" and "in step with the picture". Proposed fix: end new positive labels when the effect really ends, or disclose the case and flag it for filtering.
- **F2, HIGH:** the readme promises more than the release rule checks. It defines proof as zero error at both edges with one-frame sensitivity. The actual rule is the half-strength (t50) edge plus allowed transition frames. With TAA on, a picture signal shifted one frame earlier passes all 16 B0 events. Two wordings are also wrong: "fade ended 4–6 frames after the label" (it fell below half strength then) and "unflagged frames agree pixel for pixel".
- **F3, MEDIUM:** "17 of 17 … both ends" is wrong. Each on-leg has 16 complete events plus one whose end runs past the capture. The evaluator wrapper (`085-04-eval.py:70-71`) counts that unfinished end as proven. In a constructed test, 15 complete events plus the unfinished one reached the minimum of 16. The real legs still meet the floor.
- **F4, MEDIUM:** the readme's table implies a full test grid that was not run. There was no synthetic-order run with anti-aliasing (AA) off, and the repeat was natural-order only. The mode checks used separate objects, not "the same object". "AA on" was TSR. Nothing was run at another resolution or on Concorde or Bates.
- **F5, MEDIUM:** "every mesh / never an unchanged part" is too strong. The effect is applied only to the parts selected when the viewport filter is on, and a transformed texture does not change every pixel.
- **F6, MEDIUM:** the release edge evaluator still reports the RD5 test arm's end as −4. The saved pixels show +3. The separate pixel reading is sound, but the evaluator itself is not repaired (already queued in the 089-04b ruling).
- **LOW:** readme line 952 says "−1 = default". The source treats any negative value as the default.

**Post-result changes (question 2):**
- The 085-04 WARMUP exclusion, G405 (the one-frame-hide scale), G406 (game-time alignment), the camera_clipping AA-off confirmed-hold reading, the 089-04b G-MODE family gate and `d57246c`: all sound. Codex re-ran each against one-frame mutations and they still fail as they should.
- G395 (reference exclusion): right direction but incomplete — it did not fix RD5 (F6).
- The 085-04 edge-list adapter: the fix itself is fine, but its counting of unfinished ends is a real coverage hole (F3).
- G-MODE also accepts a window with only its first or last labelled frame dropped. That has always been true; the edge evaluator is what has to catch it.

**Final source round (question 3):** no functional defect in the OffFrames default change to 16, the LOD log wording, or the merge.

**Disk readings:** D: 126.84 GB free. E: 48.05 GB free, below the 50 GB floor. Nothing deleted.

**NEEDS-DECISION:**
1. F1: fix the early-revert and material-takeover case before merge, or accept it as the product contract and disclose and flag it.
2. F2–F5: correct the client readme's claims, counts and coverage wording.
3. F3 and F6: repair the evaluator now, or before either is next used as a release gate.
4. E: below the 50 GB floor: a cleanup decision is the owner's.
api-equivalent cost (not billed on Max): $3.87 | turns: 20 | running total across all briefs: $3,876.48
