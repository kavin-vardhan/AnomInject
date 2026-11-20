# REPORT for 082-07g-m53-amendment-2-boundary.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 082-07g-m53-amendment-2-boundary.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-28 01:39:01 | finished: 2026-09-28 02:51:22 | duration: 72 min | exit code: 0
context at last turn: 665793 tokens (67% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\082-07g-m53-amendment-2-boundary.live.txt

---- FINAL MESSAGE (Claude Code) ----
The 150 pre-supersede rows re-evaluated on the locked lib `3fd05513`: **150/150, 0 flips**, live files unchanged. All steps are done. Composing the report.

VERDICT: NEEDS-DECISION

Everything you ruled is in place: AMENDMENT 2 is committed and pushed, the proofs pass both ways, the supersede record is written and the boundary is re-issued, with the preflight clean. But reviewing every row not yet run turned up three more clauses that would falsely stop 082-07h. I adopted a proven default for each and locked them, and they need your OK before 082-07h runs.

**2. Plain summary for the owner**
- The test harness is updated for the fixed masked tile and the new build. It now tolerates the small glow a changed tile casts on its neighbours, out to 42 px as measured from saved runs, and still catches real leaks.
- While checking every remaining test, I found three more checks that could never have passed on this bench, for reasons unrelated to the feature. I fixed each one in a way you can reject; nothing ran on the bench.
- The next leg run (082-07h) is ready, about 1–1.5 h, and waits for chat's OK.

**3. AMENDMENT 2** (commit `18336a9`, in `docs/predictions/2026-09-27-m53-s1-legs.md` §A2.1–A2.10)
- **A2.1 Build under test:** exe `2FCDF059`, utoc `20DA6F98`, ucas `534C5863`, pak `FD766B7B`, globals `462B8AC6`/`BB05CF99`.
  - Rows judged on `30FE0FDE` stand as within-build results.
  - `082-07-common.py` changes only the utoc and ucas hashes. The diff is two lines (`082-07g-evidence/common-diff.txt`); the other staged hashes already matched.
  - Its `ARCHIVE` path is not a hash constant, so it is left pointing at the `30FE0FDE` archive. Nothing reads it at run time, and the boundary names the `20DA6F98` archive instead.
- **A2.2 MASKED re-runs whole (ID, NULL, WC), with a new row `PRE-CLIP MASKED`.** It is class PRE and is read in `NULL_MASKED` before the wrong copy launches. It passes when:
  - the mask reads exactly 2,048 px on every measured frame;
  - every alpha < 0.5 cell interior (1-px inset) is within the sky range R 12–36, G 27–46, B 47–69, ±2, on every frame.
  - The cell map comes from `T_TC_UC2`'s alpha, mirrored left-right, and matches the probe's colour-derived map exactly.
  - A failure stops the run as FIXTURE-CANNOT-EXERCISE, exit code 12.
  - Evidence cited: offline minimum 138/140 (floor 32), the G-BIND (3) offline proof, and `082-07f-evidence\audit.json`.
- **A2.3 G0_FSYN re-runs first** (flagged): A1.4's admission check must read a census of the build under test.
- **A2.4 R = 42** (section 4 below).
- **A2.5 G-BIND (3) outside clause split:**
  - pixels outside the event region within 42 px read < 32, with the halo published;
  - pixels beyond 42 px read ≤ 2;
  - a per-tile leak count and max is reported for 54 neighbouring tiles.
- **A2.6 Ruling-4 review** (section 6 below).
- **A2.7 Supersede entry 3** (section 7 below).
- **A2.8 Findings:**
  - the halo's mechanism is open, and its cause read goes to S2;
  - for the S4 client docs: masks don't include the few-px halo an anomaly casts on its surroundings;
  - TC_Layer's global half renders with an 8-level gradient, cause not established.
- **A2.9–A2.10:** the harness change list, and predictions for 082-07h: every row PASS, the PRE row PASS, G-BIND halo < 32.

**4. R, the ring radius** (`prep/measure_r.py`, `halo-r.json`)
- **Method:** for each pair, the largest exact Euclidean distance (scipy distance transform) from the event region of any outside pixel with |d| > 2. The event region is the union of both legs' masks at each labelled frame. Then R = the ceiling of the maximum over all pairs.
- **27 pairs:**
  - every banked wrong copy against its null: 23 live, 2 superseded U-N1 pairs, and MASKED (d = 0);
  - the 082-07f probe against the 082-07e `NULL_MASKED_A1`.
- **R = 42.** The maximum is 42.0, from the probe at si 20, pixel (926, 41), d = 3, on the lit neighbour TC_NN1. A cross-check using the tile rectangle as the region, over all 100 frames, also gives 42.0.
- The wrong-copy pairs reach at most 15 px, and the largest |d| outside any region is 17. Your "48" was the edge of a 32–48 px band; the exact figure is 42.0.

**5. Proofs, both ways** (all on the locked lib `3fd05513`)
- **AMENDMENT 2 suite: 33/33** (`proof-082-07g.json`).
  - **PRE row:**
    - the 082-07e null FAILS: mask 4,096, and 1,152 px outside the sky on all 100 frames;
    - the probe PASSES: 2,048, and 0 px outside;
    - two single-pixel flips each FAIL;
    - through the sequencer it stops with code 12, and a resume stays at 12 with nothing launched.
  - **G-BIND outside split:**
    - all 27 halo pairs PASS (ring max 17, far max 2);
    - a synthetic leak (the real TC_Layer tile-probe change, max 149) pasted onto a neighbour inside the ring FAILS on 27/27 pairs, and pasted beyond the ring FAILS on 27/27;
    - the leak reading names the right tile in 27/27;
    - end to end on real TC_Layer pixels: PASS with a real halo transplanted in; FAIL for a leak on TC_Alias (inside) and on TC_Dead (beyond).
- **Evaluators 210/210.** One expectation changed: +20 on the global half now passes (the declared cost), and +40 fails.
- **Admission 37/37, sequencer 20/20, disk 15/15.**
- **Dry run after the lock:** 194 legs, 549 gate rows, 0 problems, no console token missing from the exe.
- **Preflight negatives:** all 8 negative cases refuse, including three new ones: the 082-07e utoc, the 082-07d supersede record, and predictions carrying only AMENDMENT 1. The unchanged control passes; stub mode aimed at the live run is refused by both the preflight and the window.
- **End-to-end window (stubbed launch) 9/9:**
  - it re-launches G0_FSYN (A3) and then ID_MASKED only;
  - the PRE row stops the real window with code 12 on the unclipped null;
  - a spent budget exits 4;
  - the live files are byte-unchanged.
- **No-flip: 0 flips.**
  - Before the supersede: 150/150 on the locked lib (`noflip-pre-supersede-locked-lib.json`).
  - After it: 138/138, re-read against the census each row recorded.
  - A re-read against the current census gave NO-CENSUS on four ADMIT rows, only because G0_FSYN is waiting to re-run. It is kept as evidence (gotcha G327).
  - ⚠ Correction: the amendment and journal cite `noflip-pre-supersede.json`, which ran on an intermediate lib (`16730610`, differing only in the half-locating step). The locked-lib re-run gives the same 150/150 result; the next docs commit should point at the new file.

**6. Rows considered under ruling 4** (415 of 549 not yet run; the full table is §A2.6)
- **Unaffected (the check is inside the mask or post-revert):**
  - G-ID MASKED; every G3 row (MASKED, the G-ID-M rows, SHARED, FMW);
  - G-ID-M (29 rows) and G-ID-M-D (6); G-RD identity and noclear;
  - G-BIND (3)'s layer-half and identity checks; G-ID-FMW; BAND F-MW.
- **Unaffected (no pixel check):** G-BIND-DEAD, G-REASON, G6, G4, G-COLL, G3-COUNT, TRIP, G11, P2-1, P2-3, G0, FIXTURE-NAMES.
- **Changed:**
  - G-BIND (3) outside clause: ruled (A2.5);
  - G-BIND (3) global half: the halo reaches inside the tile (my default, A2.6.1);
  - locating TC_Layer's halves: not the halo (my default, A2.6.2);
  - G-RD drift: the tonemapper dither (my default, A2.6.3).

**7. Supersede record** (entry 3, record `85fb48dd`)
- **Four attempts:** `G0_FSYN_A2`, `ID_MASKED_A1`, `NULL_MASKED_A1`, `WC_MASKED_ALPHA_A1`.
- **Twelve rows:** G0, FIXTURE-NAMES, TRIP/G11 G0_FSYN, both MASKED ADMIT rows, TRIP/G11 ID and NULL, G3 MASKED, and the halt `G-ID MASKED FAIL`.
- **Never evaluated:** TRIP and G11 `WC_MASKED_ALPHA`.
- **Aliases:** four renamed `…__SUPERSEDED_082-07g`, with hardlinks verified before and after. Nothing was deleted.
- **After:** 51 live legs, 138 live rows, no halt.

**8. Boundary and preflight replay**
- `082-07-evidence/boundary.json` is `6520d84d…`. It locks:
  - head `18336a9`; predictions `acfbbe53`; lib `3fd05513`; common `91af53d8`; supersede `85fb48dd`;
  - legs, run.ps1, preflight, postflight and window, unchanged;
  - the staged build `2FCDF059` / `20DA6F98` / `534C5863` / `FD766B7B`, checked against the `20DA6F98` archive.
- The 082-07d boundary is kept as `boundary-082-07d.json`.
- The preflight replay right after the lock was clean (0 problems, bench quiet), and so was the final one.

**9. The 082-07h invocation**
```
C:\Python313\python.exe D:\IntrusiveAnomalies\_reviews\082-07-window.py 082-07h 540
```
- Don't set `IAI_R53_ROOT` or `IAI_R53_STUB_LAUNCH`. If the 45-minute wait budget runs out, re-run with a new tag (e.g. `082-07h-2`).
- **Order:** G0_FSYN (A3), then ID_MASKED, then NULL_MASKED and the PRE row, then WC_MASKED_ALPHA, then SHARED onward. That is 143 legs; no other decided leg is re-launched (V1 above).
- **Expected time:** about 60 min of legs at 082-07e's measured ~25 s per leg, or about 91 min at the dry-run estimator's 38 s per leg. Add quiet/person-present waits and any re-attempts.

**10. Commits pushed**
- `18336a9` on `feat/m53-uv-normal-corruption`, local = origin. It carries AMENDMENT 2, the journal `2026-09-28-082-07g-…`, the status block, and gotchas G325–G327.
- `m51` is still `53bf725`, `master` `b5f15a3`; no tags. CaptureBench and plugin source are untouched.
- Harness edits are in `_reviews\` (not a repo). Evidence and scripts are in `_reviews\082-07g-evidence\`; the end-to-end scratch folder `E:\IA_BuildCache\_r53_e2e_082-07g` is kept.

**11. NEEDS-DECISION** (all four are locked in; rejecting any one means re-issuing the boundary before 082-07h)
1. **G-BIND (3) global half.**
   - **Why:** the halo reaches inside the tile (the probe's drawn cells moved by up to 5). The global half sits 3 px from a half that changes by up to 149, so "≤ 2" would fail.
   - **Default:** the global half reads < 32. All of it lies within 42 px of the layer half.
   - **Still caught:** a cross-binding, a channel swap, and a +32 shift.
   - **Cost:** a uniform +20 shift on the global half now passes; before, it failed.
2. **Locating TC_Layer's halves.**
   - **Why:** on 708 real frames from 59 legs, the rendered global half has an 8-level gradient, so no half is "uniform" at ±2. G-BIND (3) would stop INVALID-FIXTURE on the null picture alone.
   - **Default:** a half counts as uniform below a range of 16. The global half measures ≤ 8 and the layer half ≥ 156. This changes how the halves are found, not the verdict checks.
3. **G-RD drift.**
   - **Why:** frame N vs frame 1 within one leg fails by the tonemapper dither alone (G318). On the bank, every event with a frame N differs, while all 203 same-phase pairs read 0.
   - **Default:** compare each frame with the first frame of the same engine-frame phase (mod 8), still requiring 0 px.
   - **Proven both ways:** the dithered synthetic passes (and the old lib fails it), while a +1 drift and a +1 patch each fail.
4. **G0_FSYN re-runs first.**
   - **Why:** under A1.4, the census must be of the build under test, and the only one on record is from `30FE0FDE`, where TC_Masked was effectively opaque.
   - **Cost:** about 35 s.
   - **Alternative:** keep `G0_FSYN_A2`, if you'd rather resume literally at MASKED.
api-equivalent cost (not billed on Max): $16.65 | turns: 112 | running total across all briefs: $2,755.66
