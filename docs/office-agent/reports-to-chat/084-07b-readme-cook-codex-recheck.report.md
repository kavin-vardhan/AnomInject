# REPORT for 084-07b-readme-cook-codex-recheck.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 084-07b-readme-cook-codex-recheck.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-29 03:52:40 | finished: 2026-09-29 04:20:07 | duration: 27 min | exit code: 0
context at last turn: 671641 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\084-07b-readme-cook-codex-recheck.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION. The cook, the readme, the `verify_capture.py` update and the checklist item are all done and pushed. Codex's re-check is RED. It confirms a label defect I also found in the source: every `missing_texture` and `corrupted_texture` frame is written `labelled: false` / `visible_positive: false`. Chat needs to rule on it before the next bench. Nothing was launched, staged, or changed in source.

**2. Plain summary for the owner**
- The fix branch is now cooked with the bench module in it, archived and not staged.
- The client readme now spells out, per anomaly, what each per-frame label field means and how to build a training label from them. The verifier checks those rules on any session.
- Codex and I each found that the round-3 build marks texture-anomaly frames as "not labelled". That has to be fixed and re-cooked before a bench, and it needs chat's call first.

**3. Codex re-check** (`_reviews\084-07-codex-sync-recheck.md`; 1,233 s; requested and actual model/effort match; verdict RED)
- 1, m52 `partial` flag: **PARTIAL**. Correct on the banked sets. But an unreadable texture counts as partial, the held level is a prediction, and `run_summary` lists at most 32 frames per event.
- 2, camera_clipping triangle confirmation: **PARTIAL**. A perfectly flat mesh gets zero-length traces and becomes a miss with no flag (high). The check reads collision, not what is drawn.
- 3, F2: **PARTIAL**. A rotated instance inside a component scaled unevenly per axis is still rejected before any trace (high).
- 4, F4: **PARTIAL**. A restored object outside the old actor's component list can escape the verification.
- 5, F5: **PARTIAL**. A detached tail carried into the next run is lost at the boundary after that.
- 6, F3: **RESOLVED**.
- 7, `labelled` / row families: **PARTIAL**. Its high finding is the texture defect below.
- The one-line `IAnomaly.h` change is additive only.

**4. The cook**
- **Time:** 60 s end to end (the cook commandlet itself 17.6 s), 794 packages, 0 warnings, exit 0. All four predictions I wrote down beforehand held.
- **Hashes:** exe `FF41BFF3` (the 084-07 exe, unchanged) + utoc **`87A46A27`** / ucas `55A65BA4` / pak `26FFC026` + global `462B8AC6` / `BB05CF99`.
- **Descriptor proof:** the plugin descriptor extracted from the `.pak` with UnrealPak is byte-identical to the branch's (`9EFB9B49…`). It lists five modules, `AnomalyBench` first and excluded from Shipping. Maps: CB_GateLevel, MainMenu, MainWorld, Entry.
- **Content safety:** the scratch host had no content of its own, so I linked it to the real project content for the cook and removed the links afterwards. A hash manifest of all 2,182 content files was identical before and after.
- **Archive:** `_binary_baselines\m52fix-cook-87A46A27\` (6/6 hashes re-checked there). The full staged tree is at `E:\IA_BuildCache\_r84_cookout_a1`.
- **Not staged:** the m53 set is still what the bench runs; I read it back after the cook.

**5. Readme and `verify_capture.py`**
- **New §8.7:** a per-anomaly table of the label fields, how mask values are reused and how to key them, and the training-label recipe.
- **§8.6:** the `visible_positive` change, and how to tell sessions delivered before this build (no `labelled` key).
- **§8.6a:** decision 3 (a `stuck_low_mip` event's first 1–5 frames can be partial), the partial flag's limits, and what to do with frames carrying each transition reason.
- **§8.6b:** camera_clipping's label rule, the cases it misses without a flag, and its cost.
- **Also fixed to match the source:** the out-of-date camera_clipping note, `annotation.json` writing `blink` rather than `blinking` (G362), and a stale "should be 0" sentence.
- **Field-to-source table:** in the journal, not the client readme. 32 rows: 31 match the source, 1 does not (the texture `labelled` defect).
- **`verify_capture.py`:** new `--label-rule` mode. It reads which rule a session was written under (new or old) and says so, then cross-checks `labelled` against `annotation.json`.
  - Selftest before: 2 black-frame cases. After: the same 2 plus 15 label-rule cases, including the ones the brief required both ways. All pass.
  - The label-pixel (98) and change-oracle (35) selftests give identical output before and after, and the unit tests still pass (19 + 25).
  - On the 084-06 banks it reproduces 084-07's row-family counts exactly (345 / 80 / 128 / 56).

**6. Commits pushed** to `fix/m52-label-timing`: `a6a4893..46e5894`.
- `927557d`: the `verify_capture.py` label-rule reader.
- `46e5894`: readme, packaging-checklist item (extract the cooked descriptor and diff its module list before delivery), journal, status block, G361–G362.
- `m51`, `master` and tags untouched.

**7. NEEDS-DECISION**
1. **The texture `labelled` defect (G361, Codex's top finding).**
   - `labelled` is copied from an internal "active" bit that is always false for `missing_texture` and `corrupted_texture`. Those frames are therefore written `labelled: false` and `visible_positive: false`, even though `annotation.json` lists them. This is the same class of bug as G241.
   - The flaw is in build `FF41BFF3` and in this cook. It comes from reading the source; there is no runtime evidence yet.
   - The readme states the intended rule. A fix should reproduce the annotation's own test for texture anomalies: the anomaly is live **and** the object's box is on screen.
   - Fixing it means a rebuild and a re-cook (the cook itself takes about a minute). The new `--label-rule` check fails any session built with this defect.
2. **Codex's other findings (N2–N7) need rulings before 084-08.** The readme already states the current miss cases, so it matches the source as it is today.

The Gmail, Calendar and several plugin connectors need authorizing in their settings, and the PDF viewer server failed to connect. None were needed for this work.
api-equivalent cost (not billed on Max): $23.66 | turns: 187 | running total across all briefs: $3,296.23
