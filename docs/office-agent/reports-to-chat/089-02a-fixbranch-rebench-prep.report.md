# REPORT for 089-02a-fixbranch-rebench-prep.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 089-02a-fixbranch-rebench-prep.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 23:53:38 | finished: 2026-09-30 02:22:30 | duration: 149 min | exit code: 0
context at last turn: 926351 tokens (93% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\089-02a-fixbranch-rebench-prep.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**Summary for the owner:** The re-bench (089-03) is ready to run: the evaluator bugs are fixed and locked, the LOD test level has real lights now, and a practice run with nothing launched finished cleanly. One thing is still unproven: I couldn't render the relit level without launching the game, so its first two legs check it isn't black before any of its other legs run.

**E1 fixes** (harness `_reviews\089-03-*`, built from 084-09, which is untouched)
- Selftest: **67/67** on the frozen bytes. With the fixes switched off, the new evaluator gives exactly 084-09's results, event for event.
- **G395** (a judged frame never sits inside its own reference):
  - Labels deliberately shifted by one frame on REAL_MT_NAT: the old evaluator reads end −3 / start +4; the fixed one reads end +1 / start −1, FAIL on all 10 events.
  - RD3: old reading was start −14 / end −4; fixed reading is FAIL 7/7 at end +1, which is the true offset.
  - CD3 still FAILs (+1). The 8 REAL/MT85 legs still PASS.
- **G405** (one-frame label runs): REAL_BL_SYN goes from FAIL 10 to **PASS 10/10 at +0**. Moving or dropping that one-frame label still FAILs 10/10.
- **G406** (null aligned by game time):
  - B3 si 181 goes from FAIL to PASS. B9 PASS 16/16 at +0. B0L still FAILs on both E1 and the kit.
  - Events outside the null's time range read UNJUDGEABLE, never judged. On B0, 13 events are judged and all PASS, and 5 sit past the null.
  - B0 is one event stricter than 089-01 (10 of the 12 censored offsets judged, not 11) because the declared +2 s window applies. I kept the stricter rule.
- The kit, unmodified, is also fed the aligned null ("KIT-GT"). On B3 it reads 7 PASS, 0 FAIL.
- **Frozen evaluator: `089-03-eval.py` SHA-256 `8e48e883…1621076`** (19 files in `freeze.json`).

**The LOD fixture**
- **Why it was black:** it already had a sun and a sky light. The sun was authored with the rotator's (roll, pitch, yaw) argument order mixed up, so it pointed up from under the floor, and the gate level's point lights were never copied (G410).
- **Change:**
  - Sun turned to point down.
  - Three 400 cd point lights added. I sized them from a real packaged CB_GateLevel frame: the bench's pinned exposure is bright, so 2,500 cd would have washed the targets out (G412).
  - The project content changed by exactly one file, `CB_LodFixture.umap`.
- **Cook:** 801 packages, 0 warnings.
  - The new pairing, `m52fix-cook-62633720`, is the archived exe **`8A6074AA`** plus utoc **`62633720`** / ucas `7AA00FBF` / pak `C1E3E83B`.
  - The globals `462B8AC6` / `BB05CF99` and the plugin descriptor are byte-identical to 084-08b's. Map gate 5/5.
  - One cook prediction missed in part: one unnamed shader chunk was replaced, the same shape 084-08b saw.
- **Not proven offline:** rendering the level from a commandlet came out black for the known-good CB_GateLevel too, so that method tells us nothing here (G411). The first check on the bench (LODP_FN/FD brightness) decides.

**Legs, predictions, minutes (24 legs)**
- In order:
  - B0, B3, B9, B0L, P1 (OffFrames 16 read back from each run), then the **1,800-frame null B5 immediately after**.
  - MASK55A (Route A; invalid unless peak live ≥ 2 and recycles ≥ 1).
  - REAL_BL_SYN plus REAL_BL_NAT as a control.
  - The 2 fixture premise legs and 8 fixture gate legs.
  - CC AA-off: one leg plus two nulls, read by the current gate and a proposed reading.
  - **Part 0 last**, after the restore.
- The kit runs as the third reading on every leg. Per-leg predictions are in README §5.
- Time: **about 77 min, about 87 with a retry and waits for a person at the PC**.
- **Dry run:** exit 0, **24/24 launches accepted**, the restore survived an injected file lock. All readings were as expected on the night's sessions used as stand-ins. It found one bug in my kit summary (censored events counted as passes), fixed before the final freeze.
- The live preflight now reports 0 problems.

**Commits pushed**
- `fix/m52-label-timing` **`ef4cc16`**: journal, status block, G410–G412. Main checkout still `m51` `53bf725`; M53 still staged.
- CaptureBench `e66b461` (local-only): the fixture authoring tool's lights.

**NEEDS-DECISION:** none blocking 089-03. Two for later:
- The camera_clipping AA-off reading. The current gate can't judge it, because the unconfirmed prop frames change no pixels at AA off. My proposed reading judges only the confirmed frames, and on the night's run it reads PASS. The re-bench reports both; which one becomes the gate is your call.
- Whether to add a `--null` option to the office kit.
api-equivalent cost (not billed on Max): $40.32 | turns: 215 | running total across all briefs: $3,779.35
