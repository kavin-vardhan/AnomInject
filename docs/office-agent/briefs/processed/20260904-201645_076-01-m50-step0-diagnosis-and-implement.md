# SESSION: new-default
# NAME: 076 m50
# EFFORT: xhigh
# MODEL: claude-opus-5

# 076-01 — m50: step 0 diagnosis of the stencil collision (D1–D4), then implement rulings 2+3 per the 075 plan, gates G1–G10 both orders, merge when green
Fresh session. COLD START: `CLAUDE.md` status → `docs/sessions/2026-09-04-075-m49-a2b-docs-m50-plan.md` (the m50 plan, branch table D1–D4, the G246 amendment) → journal 074 §Leg B/§LG-9 → gotchas G245–G250. Summarise in 5 lines before acting.
Headless; nobody answers; FINAL MESSAGE structure in §5; failed gate = branch-and-restore, report, STOP. Codename discipline. ⛔ Office data never leaves the office PC.

## 0. Rulings on 075-01's NEEDS-DECISION (journal them)
1. CONFIRMED: m50 step 0 is a DIAGNOSIS (D1–D4), not the briefed allocator rewrite; the gate half (per-frame single-owner + its can-fail collision lever) is built as planned regardless of branch.
2. CONFIRMED: journal 074's mechanism sentence stays withdrawn via the G246 in-place amendment; journal 074 is not edited.
3. Section G's host is **Bates** — the owner's flagged session (two texture events labelled one frame early, masks correct) is on the Bates box. The Concorde findings are the client's M2 review and are read only through the verifier the client runs or the office runs on that host. Amend the card's header line accordingly (one line).
4. `unmeasurable_targets_admitted` in run_summary: BLESSED (64 → 65). annotation.json does not move.

## 1. Step 0 — diagnosis (commit the instrument first)
Add the tag-SET print at the ARM (`AnomalyCensus.cpp:794-797`) + `EventClaimed` at the same tick (Log level, one line per arm). Build both targets. Re-run the banked 448-tag-instance recipe (the leg that measured 22 of 448 = 4.9 %) in native order and classify EVERY over-count against D1 (direct collision) / D2 (release window) / D3 (rate dependence) / D4 (TAG-OVERTAKEN residue). Report the table with counts BEFORE writing the fix. If the branch is not one of D1–D4, STOP and report NEEDS-DECISION with the reading.

## 2. Implement (per the 075 plan §files, on the diagnosed branch)
- Ruling 2 by branch: D1 → ledger quarantine on release (`Quarantined` map; `IsFree()` excludes it; `IsAssignable()` untouched; census `RecentlyReleased` collapsed onto the ledger); D2/D3 → the release-window/rate fix the diagnosis names; D4 → count and exclude, no allocator change. Whatever the branch: the per-frame single-owner gate `m50_single_owner.py` + the collision LEVER (`IAI.Bench.ForceTagCollision`, console-only, default off, echoed) that the gate MUST catch.
- Ruling 3: `AnomalyMeasurability.h/.cpp` (shared `ComponentRendersAsNanite`, never in AnomalyViewport — G127); census `ClassifyCandidate` uses it (verdicts/counters unchanged); `AnomalyMaskMeasure` classifies at record creation and `ArmIfMeasurable` SKIPS unmeasurable records; `unmeasurable_targets_admitted` key.

## 3. Predictions + gates (both tick orders where per-frame)
`docs/predictions/2026-09-04-m50.md` BEFORE the first gate leg (after step 0 — the diagnosis result is an input to the predictions, say so). Gates exactly as planned: G1 single-owner 0 violations · G2 lever caught · G3 re-count 0 of 448 · G4 MASK-TIE unchanged (pre-declared as NOT evidence for G1) · G5 full regression (ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE ×8, OBS-1..4, black-frame, A44, both targets) · G6 allocator headroom, 0 TAG-POOL EXHAUSTED, tagOvertaken/census_fires_* read · G7 unmeasurable admission at SM_Ramp2 with census ON · G8 its BEFORE picture on 8839D806 (if the bench cannot reproduce the LG-9 shape, say G7 proves nothing) · G9 Lyra LG-9 leg on L_Convolution_Blockout delivers N events with `observability_measured: false` instead of `anomalies: []` (worktree refreshed, rebuilt, main checkout proven untouched) · G10 P-C7 v3 (declare per leg where `frame_indices` is expected to differ).

## 4. Merge + cold-start
Green ⇒ merge to master (`feat(capture): m50 - stencil single-owner gate + <branch fix>; unmeasurable targets admitted as NOT_MEASURED`), push, journal `docs/sessions/2026-09-04-076-m50.md`, CLAUDE.md status (m50 DONE; Phase B NEXT: shader drawn-count + cook; then delivery gates on Lyra; then card Section G/F on Bates), gotchas. No tag, no cook.

## 5. Final message
1. Plain-language rundown (10 lines). 2. Step 0 table (D1–D4 counts) and the chosen branch. 3. Gate table G1–G10 (both orders). 4. Lyra G9 readings. 5. Commits, exe hashes, both checkout states, pointers. 6. NEEDS-DECISION if any.
