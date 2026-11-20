# REPORT for 095-13-m57-review-fixes.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 095-13-m57-review-fixes.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-11-14 12:04:40 | finished: 2026-11-14 13:37:26 | duration: 93 min | exit code: 0
context at last turn: 623241 tokens (62% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\095-13-m57-review-fixes.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

Every ruled item is fixed and committed, every build and test is green, and `feat/m57-lod-spatial-trigger` is pushed at `3296576`. Two findings made during the work are chat's to decide (section 7).

**1. For the owner**
- The LOD pop now arms in time on the scripted camera paths. An object entering view shows its low-detail version for 30 frames (about one second), with 15- and 60-frame variants, before it snaps to full detail.
- Nothing ran in the engine. This is code plus offline tests only; the first real check is the heavy run.
- The "pop back to low detail on retreat" clip cannot be produced on the current test level. The distance clips need the second mesh pair, which the next cook must include.

**2. Per item: commit and test** (`feat/m57`, one commit per item)
- **Part 0, merge of `integ/m56-m58` `e15d06d`:** `16ed5dd`. Editor 0 errors / 0 warnings; 19/19 pure selftests.
- **B1, view entry uses a time-based margin:** `3180b19`. Its new test and its mutant fail as required.
- **B2:**
  - `29e4eb5`: the distance arm looks ahead to the later of the contrast release and the size where coverage reaches 7 %.
  - `96e9675`: the arm timeout restarts at the first wrong frame.
  - Both are covered by the new bench simulation and arm-clock tests, and each has a mutant that fails.
- **Other product fixes:**
  - `1a6fee5` N11: a positive needs the contrast test too.
  - `a763276` S2: a trace-blocked wrong frame reads `occluded` and `lod_subvisible`.
  - `82f15cd` N2: the hold releases as soon as the engine's LOD is past it (`past_held`, invisible release).
  - `fce89b0` N1: the stored tick-end evaluation is committed unconditionally.
  - `771a686` S1: the render check covers the swap window, compares the dither samples and reads the resident LOD on the render thread.
  - `01ae171` N9: spatial fires are refused on the backbuffer path.
  - `01df05d` S3: the box, mask and visibility count are scoped to the forced component.
  - `73a6ff5` N7: no hold-cap swap is recorded for a frame that was never captured.
  - `57cf109` N5: the candidate path honours the translucent-only exclusion.
  - `eee7790` N3: new `lod_predict_*` keys in run_summary.
  - `de593c8` S7: a revert line is logged when the held mesh was destroyed.
  - `39c7aed` N10: the setting commands print their console names.
  - `fb6970b` N12: the selector shows the refusal reason instead of "0 matched".
  - `543a882`: cosmetic, one statement per line.
- **Part 1b:** `8721da4`. `LodEntryFrames` default 30, range 1..90. No other default had to change: the hold cap is 150 and the arm timeout stops at the first labelled frame.
- **Readers and docs:**
  - `342bbc5` B3, S4, N13: label-rule selftest 119 cases; three reader mutants fail.
  - `e999a28` S5: check kit 1.7 prints "not judgeable (spatial lod)"; kit selftest 104 checks.
  - `cd8ff28` S6, N4, N6, N8: readme and architecture.
  - `3296576`: session journal, gotcha G582, status block.
- **Bench paths, B1/B2:** `40b9383`.
  - approach: x 600 → 1450 at 7.5 cm/frame.
  - turns: start at yaw −160 (`turn_in`) and −170 (`turn_away`), 2°/frame.
  - Copies at (1700, ∓180); second pair at (1700, ∓60).
- **CaptureBench `m57-gates`** (local, checkout left on `master`):
  - `e75584d` B4/B5, null-pair check: reads `[x, y, w, h]`; refuses unequal legs; judges every common frame (new L6); fails with nothing labelled or no up-swap; reports L4b "not exercised". 14 cases including the reviewer's probes C–F; 5 mutants fail.
  - `253275c` B6, clip script: builds from the overlay tool's real output and writes under `--out`. Its selftest runs the real overlay tool and ffmpeg (9 cases).
  - `0bc7e32` and `8cce410` S7, readout: refuses when no expectation is given; L5 reads `lod_unconfirmed`; L6 pairs each revert with its fire; L3 accepts a refusal as proof the still leg tried (22 cases).
  - `4946ad8` S8, fixture script: atomic, the two options work as separate runs, refuses over unsaved editor work. Syntax-checked only; not run.
  - `ac719f6`: predictions AMENDMENT 1.

**3. The worked numbers** (pure simulation and model, not measured; default `2 4 8 4 0`, 16-frame cycle, 10-frame motion lag)
- **Arming windows:** a turn window must exceed 26 frames. `turn_in` gives 35 frames for N and 56 for D; `turn_away` gives 38; approach on the second pair gives 86.
- **C1 (N):** armed at 51, wrong frame 67, labelled 69..98, swap 99. With 15 frames: swap 84; with 60: swap 129.
- **C2 (D):** labelled 93..122, swap 123.
- **C3 (N2/D2):** armed at 51, 82 frames ahead. Coverage reaches 7 % at frame 127; the 0.3 switch comes at 128. Labelled 128..132, swap 133.
- **L4 (`turn_away`):** armed at 51, never enters view, the arm clock expires at 143.
- **C4:** the down-pop band is empty (section 7).
- **Deviation:** distance "frames ahead" now uses the closing speed in distance. The linear screen-size rate under-counted by up to 3× and gave arming windows shorter than one cycle.

**4. Compile, tests and the dashboard**
- **Editor compiles:** eight on `feat/m57` (after the merge, B2, N1, S3, S7, Part 1b, the bench paths, and the final tip), all exit 0 with 0 errors / 0 warnings. The final one took 82 s.
- **Final tip, C++:** 19/19 pure selftests. The lod trigger selftest has 166 checks and all 15 mutants fail; the m59 selftest has 141 checks and all 13 mutants fail; all 5 terrain mutants fail.
- **Final tip, Python:** pixel gate 101, change oracle 35, unit tests OK, exclusion gate OK, measure_offset PASS. Lever audit PASS on source (62 names), on its selftest (12 cases), and on the final Editor DLLs.
- **Pre-push scan:** codename scrub clean over 36 files; token absent; no comment markers.
- **Dashboard** (read-only, `master` `7ea2e51`): it has no inject sender, and `capture_start` carries no arguments. It cannot send a numeric second argument to `lod_popping`.

**5. What the heavy brief must run**
- Before its cook: `make_lod_fixture.py` with `IAI_LODFIX_ADD=second_pair` (and `instanced` for m59 leg L59-5).
- Then clips C1–C6 (C1 and C2 at 30, 15 and 60 frames) and gates L0–L14 from `code-receipts-095-10\predictions-095-10.txt` with AMENDMENT 1.
  - Frozen prefix hash `269FECF8…` is unchanged; the file hash after the amendment is `1EEEE432…`.
  - Use the pinned capture configs on the SVE capture path.
- This will be the first runtime check of every change in this brief.

**6. Host binaries and context**
- The primary checkout is back on `integ/m56-m58` `e15d06d`, clean. The plugin's Editor DLLs were rebuilt from it (exit 0, 0/0) and match its source. The game exe is still not built.
- Context: one compaction happened mid-brief; the 60 % fallback was not needed.

**7. Safety, and decisions for chat**
- **Safety:**
  - No force, `reset`, `stash` or `rebase`; no merge into `integ` or `master`; no tag.
  - Deleted only my own temporary CaptureBench worktree (outside `Plugins/`) and test temp folders.
  - No process touched; several foreign Python processes are still running and were left alone.
  - Untouched: `m51`, `fix/followup-capture`, `feat/europa-port`, `master`, the m56, m58 and m59 unit branches, `D:\IA_Europa`, the dashboard repo.
  - Token never printed; codenames only.
- **Decision 1, down-pop:** it is unreachable on the bench paths. A re-hold needs coverage of at least 7 % below 0.85 × the swap size, and a straight retreat from N2 never gets there while the engine is finer than the held LOD. C4 will show the up-swap only. Options: keep it as built (on by default), change the re-hold rule, or change the geometry before the owner's clip.
- **Decision 2, the bench copies:** ruling B2 asked that they move onto the path's end. They did, but they can never trigger. Their LOD switch is at 0.05, beyond 17.8 m, while their 7 % size is at about 3 m, roughly 1,970 frames later at walking speed. They are refused `at_finest_lod` on every burst and serve only as a refusal reading. Every distance clip therefore needs the second pair in the cook.
- **Note (a property of the rule, not a defect):** under B2, a real mesh that switches LOD while still small cannot be armed at walking speed.
api-equivalent cost (not billed on Max): $64.51 | turns: 2 | running total across all briefs: $6,432.52
