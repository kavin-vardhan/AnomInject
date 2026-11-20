# REPORT for 095-11-independent-review-m57-m59.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-11-independent-review-m57-m59.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 11:00:15 | finished: 2026-11-14 11:24:24 | duration: 24 min | exit code: 0
context at last turn: 727894 tokens (73% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-11-independent-review-m57-m59.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

The review is complete: `D:\IntrusiveAnomalies\_reviews\095-11-independent-review.md`. Receipts, including the arithmetic behind B1 and B2, are in `code-receipts-095-11\receipts.md`. I built nothing, launched nothing and ran no engine leg, so B1 and B2 are worked out from the code and engine source, not measured.

**1. For the owner**
- m59 (the LOD refusals) is ready for the test run. m57 (the new LOD pop) and its bench scripts are not.
- As written, almost every m57 test clip would come back with no event: the scripted camera turns arrive between two firing chances, and the walk-up never gets close enough before the arming time runs out.
- The m57 label code itself looks right. First fix the arming rules, the camera paths, one line in the label checker, and two bench scripts.

**2. Verdicts and counts**
- m57: **READY AFTER FIXES**.
- m59: **READY FOR GATES**.
- Bench scripts (CaptureBench `m57-gates`): **NOT READY**.
- Findings: **6 BLOCKER, 11 SHOULD-FIX, 14 NOTE.**

**3. BLOCKER and SHOULD-FIX, one line each**
- **B1 (m57 bench path):** with the default capture config, firing chances fall at si 3, 19, 35, 51 and 67. The turn targets can only be armed at si 36–40 (`CB_LodFix_N`) and 53–62 (`CB_LodFix_D`), so in native order C1, C2, L1-turn, L4 and L12a never arm, and L4 would pass for the wrong reason.
- **B2 (m57 product):** the distance arm looks ahead to the engine's LOD switch, but the 90-frame arm timeout waits for the first frame with 7 % coverage. On the scale-1 copy the fire times out at about 13.9 m with no event. The approach path also stops at about 6.9 % coverage, and the second mesh pair (N2/D2) never switches LOD on the path.
- **B3 (m57 tools):** `verify_capture.py:3900` still tests `minor == 2`. On schema 2.3, any vetoed event in the session excuses every labelled frame missing from `annotation.json`.
- **B4 (bench):** `m57_null_pair.py:98-99` reads `bbox_px` as two corners, but the plugin writes `[x, y, w, h]`, so it checks the wrong region.
- **B5 (bench):** `m57_null_pair.py` checks only one event's window, silently drops frames missing from the null leg, and can pass a session where nothing happened.
- **B6 (bench):** `m57_make_clip.py` looks for `frame_%05d.png`, but the overlay tool writes `frame_NNNNN_annotated.png` and only for frames with a box, so no clip can be built.
- **S1:** the render-thread check never looks at the swap frame, ignores the dither samples, and reuses the game thread's MinLOD, so a dithered crossfade after a release goes unflagged.
- **S2:** a wrong LOD seen through glass, a fence or foliage with collision is labelled a plain negative with no flag.
- **S3:** in both m57 and m59, the box, mask and visibility count cover the whole actor while only one component is forced; `SetEventComponentScope` is not used.
- **S4:** a vetoed LOD event that carried `lod_subvisible` or `lod_unconfirmed` frames makes the label checker reject the session, and the client kit reuses that check.
- **S5:** the client kit silently skips every spatial LOD event and the readme does not say so; since the old timed mode is now bench-only, a client can never check a LOD label.
- **S6:** schema 2.3 and the two new reasons are missing from the readme's reference tables.
- **S7:** `m57_lod_readout.py` can print PASS having checked nothing; its L5 and L6 checks are incomplete.
- **S8:** `make_lod_fixture.py` saves before its last checks (a failure leaves an orphan asset that blocks re-runs), running the two options separately fails, and N2/D2 are placed where they cannot switch.
- **S9 (m59, already on master):** `lod_corruption <target> 1` forces LOD 0, which changes nothing on a target already at LOD 0.
- **S10 (m59 gap):** a mesh with MinLOD set or stripped LODs always draws one LOD, but it is not refused as `single_lod`.
- **S11 (m59 docs):** readme lines 1255–1257 describe the skeletal switch wrongly.

**Decisions chat must make** (the review file gives a recommendation for each):
1. Whether to fix B2 in product code (my recommendation: look ahead to the coverage size and start the timeout at the first wrong frame), plus retune the bench paths and amend the predictions before the 095-07 m57 legs.
2. How to handle the client kit never judging LOD events.
3. Dithered meshes: whether to flag first or wait for agreement.
4. Whether S9 and S10 are fixed on m59 before the client drop.
5. Whether see-through occluders are flagged.

The review file also has my judgement of each of the 13 deviations; I would reject #11 (bench copy placement) as built.

**4. Safety**
- None. No repo was edited, committed or checked out; the primary checkout is still on `integ/m56-m58` `876c719` with no tracked changes, and CaptureBench is still on `master`, clean.
- I deleted two temporary tar files I had just created inside my receipts folder.
- The bench sub-reviewer's selftests created and removed their own temporary folders under `%TEMP%`, which is outside the allowed folder.
- No process was touched.
api-equivalent cost (not billed on Max): $39.26 | turns: 63 | running total across all briefs: $6,348.68
