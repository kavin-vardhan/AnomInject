# REPORT for 089-01-night-adjudication.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 089-01-night-adjudication.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 23:23:31 | finished: 2026-09-29 23:52:03 | duration: 29 min | exit code: 0
context at last turn: 667004 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\089-01-night-adjudication.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner.** None of tonight's disputed FAILs is a real label problem; in every case the labels match the picture. Each FAIL is either a flaw in one of the checking tools, shown on that exact event, or scene drift in a tool's reference. Two stuck_low_mip events can't be judged yet because the comparison run was too short. MASK55's missing masks are expected: those targets are Nanite meshes, which the product deliberately leaves unmeasured. The lod_popping `worst=2` and the must-fail arm sizes come from mixed units, not bugs. Four decisions for you are listed at the end. Nothing was built, launched or staged.

**Adjudication table** (full per-event rows in `_reviews\089-01-adjudication.md` §1)

| event(s) | class | evidence |
|---|---|---|
| blinking SYN, 10 events (sf 108…342), E1 FAIL | ARTEFACT (G395 family, new G405) | Synthetic order ends each event in a one-frame label run. E1 scales that run by its own post-label noise (depth_off 1.8–2.6 vs 44.7–47.9), so the frame after the label scores 0.73–1.0. Pixels there read 1.4–3.3 against 34.6–39.8 on the hidden frames; 086-01 and the kit read 0. |
| B9, 5 events (120–130, 188–198, 460–470, 528–538, 800–810), 086-01 FAIL | CONFOUND (scene drift in 086-01's own reference) | 086-01 is **not** blind to `partial`: its onset partial frames are inside its transition set on all 5. Its FAILs are offsets where it records drift of 0.40–0.62 of the effect itself. With the null aligned by game time, the post-label residue is ≤ 0.08 of depth. |
| B0, 11 of 12 censored + 086-01 FAILs 563–573 and 903–913 | CONFOUND, PASS with the aligned null | E1's null lag was rejected and its fallback reference can't remove the rock-sharpness cycle. Aligned by game time, every event is below 50 % of depth by +5..+8, with nothing ≥ 50 % after the 8-frame window. |
| B0 1039–1049 | UNJUDGEABLE | At t = 44.2 s it lies beyond the null run's 41.3 s. |
| B3 si 181 (label 189–198), E1 FAIL | ARTEFACT (E1 null misalignment, G406) | See the next section. |
| B3 503 (485–494), 086-01 FAIL | CONFOUND | 086-01 records drift of 0.687 of the effect; aligned null is below 50 % at +6. |
| B3 1232 (1167–1176) | UNJUDGEABLE | At t = 41.7 s it lies beyond the null run. |
| RD3 arms, AA off and on (14 events) | E1 ARTEFACT; the truth is end +1 | E1's end −4 is G395 exactly: its offset reference starts with the delayed frame. Start −14 is the previous event's delayed frame falling inside this event's span. The pixels show end +1, as 086-01 and the kit read. |
| SR_B0 / SR_B3 / SR_B9 onset | no FAIL anywhere | E1 prints the raw onset including partial frames; the kit prints the transition-aware onset. |

**B3 si 181.** The blur was not resident after the label ended, nothing else was on the rock, and there was no real desync.
- **Texture state:** the render record shows all three textures back at 11/11 baseline mips from si 199, the frame after the label. The game-thread read-back confirms it by si 205, and the restore trail closed at si 208 with no reopen through si 239.
- **Other anomalies:** none were applied between si 189 and 240. The same rock with AA off (B9) has exact offsets.
- **What the picture shows:** TSR history of the blurred frames. Against the null at the same game time the effect decays to 0.81, 0.36 and 0.14 of depth at +1, +8 and +16. That is below 50 % from +6, inside the product's 8-frame transition window.
- **Why E1 saw a 41-frame tail:** it divided by a null 16 frames later in game time while the scene's rock sharpness was climbing. That produced the plateau and E1's single t50 frame (si 214 at 0.507 of depth).

**B0 censored offsets.**
- **Why:** after the firewall prompts, the accepted attempt started at engine frame 591, 250 frames of game time behind the null run. E1's lag search only covers ±30, so it rejected every lag and fell back to a detrend that meets ±8–30 % scene drift.
- **Evaluator change:** align the null by game time, frame by frame. On tonight's bank, with no re-run, that makes 11 of the 12 judgeable and all pass at t50.
- **Leg change:** capture the null for 1500 frames, run it right after the stuck_low_mip legs, and require it to cover each leg's full time range. Run Part 0 last so every leg starts at frame 1.

**MASK55 v2.**
- **Breakdown:** the rock has 172 labelled frames, all masked. The 270 unmeasured frames are all on three targets: `BP_SpawnPad_C` (21 events), `RoomBuilderSquare_C` (19) and `SM_Ramp2` (17). Each of those events logs `UNMEASURABLE TARGET … reason=nanite`. By type: corrupted 102, missing_texture 96, blinking 36, missing_object 36.
- **Cause:** the product's designed rule of admitting Nanite targets unmeasured. It is not G295 (0 `TARGET MASK UNAVAILABLE` lines) and not the tag-pool lever (25 recycles, yet the rock stayed at 172/172).
- **Not a regression:** 084-06's 586/586 ran on the non-Nanite gate level.
- **Route to peak live ≥ 2:** only targeted stuck_low_mip legs have ever reached 2 (11 of 15); auto-pool legs never have (0 of 5). Recommended route: B0's recipe plus `IAI.Bench.TagPoolLimit 12`, with peak live ≥ 2 and recycles ≥ 1 as a validity condition. Whether recycling works while two records are live has never been run.

**LOD `worst=2`.** `GetWorstLod` returns the mesh's LOD count, and that value goes into the engine's `ForcedLodModel`, which counts from 1. So `worst=2` forces LOD index 1 of 2, which is in range, and the engine's clamp doesn't touch it. The label doesn't depend on it (it is the popped phase); the pixels do. The only fault is the log line printing a 1-based number beside a 0-based `level=`. Proposed wording fix: `forced_lod_model=2 (LOD index 1 of 2)`.

**Arm sizes.** Both levers count engine frames, but the capture skips 2 settle frames after every apply and every revert. CD3 therefore commits on the very next captured frame (onset +1). RD3 leaves the revert-tick frame corrupted (end +1). For either lever the captured effect is `1 + max(0, D − 1 − 2)`, so a +3 arm needs D = 5. The arms still prove they can fail, at +1:
- CD3 FAILs on all three tools in both AA modes;
- RD3 FAILs on 086-01, and on the kit with AA off;
- the kit's AA-on "0 fails" is a censored edge with residual 1.000, not a pass.

**Commits pushed.** `fix/m52-label-timing` `89dc788` adds the journal `docs/sessions/2026-09-30-089-01-adjudication.md`, the status block, and gotchas G405–G409 (the previous max across all branches was G404). The review, its tables and the generator (`089-01-tables.py`) are in `_reviews\`, outside the repo. `m51` is still `53bf725`; `master` and tags are untouched.

**NEEDS-DECISION**
1. Accept the game-time-aligned null as the stuck_low_mip comparison basis (for E1, and for the kit when a null exists), and the classifications that rest on it: 17 confounds plus B3 si 181.
2. TSR leaves 5–45 % of the stuck_low_mip blur for 13–29 frames after the label; it is below 50 % by +5..+8. Keep the t50 release rule, or extend `IAI.Label.TransitionOffFrames` for stuck_low_mip under TSR?
3. MASK55 concurrency: Route A (no build, reaches peak 2 on about 11 of 15 runs) or Route B (a bench lever that holds records, which needs a build).
4. Arm predictions: re-declare them in captured frames (or use CD5/RD5), or change the levers to count captured frames.

Environment note: several claude.ai and plugin connectors are unauthenticated and need authorising from their connector settings; the pdf-viewer server failed to connect. None was needed for this brief.
api-equivalent cost (not billed on Max): $18.33 | turns: 113 | running total across all briefs: $3,739.03
