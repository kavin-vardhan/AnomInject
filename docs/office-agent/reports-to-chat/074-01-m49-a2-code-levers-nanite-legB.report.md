# REPORT for 074-01-m49-a2-code-levers-nanite-legB.md
brief: 074-01-m49-a2-code-levers-nanite-legB.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5 | effort: xhigh
started: 2026-09-04 17:04:57 | finished: 2026-09-04 18:20:35 | duration: 76 min | exit code: 0
context at last turn: 707728 tokens (71% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\074-01-m49-a2-code-levers-nanite-legB.live.txt

---- FINAL MESSAGE (Claude Code) ----
`m49` Phase A2's code half is shipped and pushed. Report:

## 1. Plain-language rundown

1. **The picker now refuses translucent-only targets** — an actor the target mask can never measure. The predicate is shared with the census (which dropped its private copy), so the two can't drift.
2. It sits on the **classifier**, not the `G33` chokepoint — the chokepoint would have driven the census's own `excluded_translucent` counter to a permanent 0, silently retiring a live diagnostic.
3. **`m39` is folded in, additively**: rows gain `bbox_drawn_px` from the reduce table's own bounds; events gain `bbox_source`. `bbox_px` is untouched.
4. **All three can-fail gates fired.** `OBS-3` moved `frames_condition_lost` 0→5. `OBS-2` produced `affected [4,5] ⊊ injected [4,5,9,10]` deliberately on the bench in both orders — the Lyra shape, on demand.
5. **`OBS-4`'s specified sweep (1/64/4096) is vacuous here** — the target draws a flat 66,837 px, so all three sit below the signal. I added a 100,000 rung; it drove `observable_frames` to 0. Both readings reported.
6. **My OBS-2 lever was wrong twice.** v1 logged a teleport it hadn't achieved (static mobility) and produced an artifact identical to the control — a clean gate that was blindness. It now reads the location back.
7. **`P-C7 v3` found a real pre-existing defect**: the census hands out stencil values a live event holds, so `target_pixels` over-counts on 4.9% of tag-instances. **MASK-TIE structurally cannot see it.** Not fixed — it's a census-allocator change with its own gates.
8. **`m44`'s G7 was reading the wrong set** after schema v2 redefined `affected_frames`. A correct build read `FAILED G7`. Re-keyed to `injected_frames`, which restores the original predicate rather than loosening it.
9. **Leg B is answered**: no detectable outline effect, on a real like-for-like control the new census-dump fixture made possible.
10. **`LG-9`'s pre-declared reading didn't occur — what happened instead is bigger.**

## 2. Gate tables

**StackOBot** — staged `8839D806`, container byte-unchanged, no cook, no tag. Both build targets exit 0, zero warnings.

| gate | native | synth |
|---|---|---|
| OBS-1 | 6/6 delta 0 | 6/6 delta 0 |
| G-EDGE (4 types) | `PASS 4 · SHIFT 0 · NOT-VISIBLE 0 · NOT-MEASURABLE 0` | same |
| m44 gates (G1/G2/G3/G3b/G3c/G7) | ALL PASS, stray 0 | ALL PASS, stray 0 |
| MASK-TIE / TARGET-PIXELS TIE | 0 MISMATCH | 0 MISMATCH |
| onset-join halves A/B | 0 zeros / 0 failing | 0 zeros / 0 failing |
| pairing 100% / SP50 | `PREVIOUS 0`, `NEITHER 0` | `PREVIOUS 0`, `NEITHER 1` at si=8 (Δ49.3, known branch (a)) |
| black-frame | PASS, DARK FIRST 0 | PASS |
| **OBS-2** (hide class) | `aff [4,5]` ⊊ `inj [4,5,9,10]` | `aff [4,5]` ⊊ `inj [4,5,6,10]` |
| **OBS-3** | `frames_condition_lost 5` (OFF: 0) | 5 (OFF: 0) |
| **OBS-4** 1/64/4096/100k | 32/32/32/**0**, `injected` identical | — |
| P-C7 v3 (8 pairs) | 40/40 declared-only; `frame_indices` identical 4/4 on all 8 | |
| T3-PROBE | **UNRUNNABLE** — lever refused by name, nothing spawned | |

**Lyra** — worktree `fe89e11 → 328ca28` detached/clean both ways, main checkout stayed on `master`; LyraEditor exit 0, zero warnings.

| gate | result |
|---|---|
| LG-1 | PASS — 0 of 7 / 0 of 6; TIE **MATCH 40 / MISMATCH 0** |
| LG-3 | PASS — reader names in the artifact |
| **LG-4 (blocking)** | ⚠ **NOT EXERCISED** — host wrote no custom depth; `identical=1`. Not a pass. Code untouched by A2 |
| LG-5 | PASS — 5/2/2/3/**1**/**1**/3 of 9 |
| LG-6 / LG-8 | PASS — `PASS 6 · SHIFT 0 · NOT-VISIBLE 0 · NOT-MEASURABLE 1`, confidence quoted |
| LG-7 | PASS — `frames_shaders_pending 0` vs global `6784` |
| LG-2 | ⚠ **did not occur naturally again** — said so, not faked |

## 3. Leg B — numbers and verdict

36 dumped batches × 14 tagged actors. 21 TAG-CHANGE vs 25 control pairs, pose-matched, both sides negative, **one reference silhouette applied to both classes**:

| | RING2 | RING6 | RING12 | RING20 | AMBIENT |
|---|---|---|---|---|---|
| TAG-CHANGE | 0.9836 | 0.6383 | 0.5486 | 0.5141 | 1.0609 |
| control | 0.9204 | 0.6064 | 0.5251 | 0.4939 | 1.0777 |

Agreement within 7% at every radius; ring **below** ambient in both classes (DRAWS needed ≥3×). Proxy-independent corroboration: 14 actors tagged at once including a 733,944 px sky sphere — an outline would move the whole picture, and AMBIENT is *lower* on the treatment side.

**VERDICT: NO DETECTABLE EFFECT.** `G120` — a statement about the measurement, not a claim `MPP_Outline` never draws. Limit stated: "tagged" is inferred from a census ARM at that tick, a proxy; no per-frame stencil read exists and I did not manufacture one. Instrument proven both ways first.

## 4. LG-9

Pre-declared reading **NOT EXERCISED** — `annotation.json` has zero events, so no fallback could fire (`OBSERVABILITY UNMEASURED`: 0). What the leg measured instead: `census_candidates 63`, **`census_unmeasurable_nanite 55` (87%)**, 7 bursts, **6 events all vetoed as `MEASURED_ZERO maxCount=0`, all six targets Nanite** (verified per asset) → `anomalies: []` from 90 frames. That is **`I11-A` occurring naturally at scale**: the m26 veto fires before m49's fallback can, and the dataset gets nothing. Not fixed.

It is also where **the translucent exclusion fired** — `MeshA_17F7D1`, not Nanite, so the two exclusions are disjoint classes. That's the `G96` positive proof `T3-PROBE` couldn't give on StackOBot.

## 5. Known-answer Bates

**Absent** — `_bates_reads\` holds only the p9 transcription markdown, no session folder. Unrunnable, reported as such.

## 6. Commits, hashes, states

- Plugin `master`: `f39c69c` (predictions, before any leg) → `eb3506e` feat(select) → `328ca28` feat(capture) → `0d05c4b` docs. **Pushed**, `master == origin/master == 0d05c4b`.
- CaptureBench (local-only): `3b67de1`.
- Staged exe **`8839D806`**, archived and hash-verified; `EC60E9B9` stays load-bearing. Container quartet hashed before and after: **unchanged**. **No cook, no tag** (highest remains `m30`).
- Main checkout `master 0d05c4b`, tracked-clean. Lyra worktree detached at `328ca28` (the commit its binaries were built from), clean.
- Journal `docs/sessions/2026-09-04-074-m49-a2.md`; status block at the top of `CLAUDE.md`; `G245`–`G248` appended.

## 7. NEEDS-DECISION

1. **The `MPP_Outline` rule.** Leg B says no detectable effect, so the honest default is **no refuse-or-warn rule**. That's chat's call, not mine — A2b was scoped to decide it *from* this reading.
2. **`G246`, the census stencil-range collision.** Filed with numbers, not fixed. It corrupts `target_pixels`/`bbox_drawn_px` on ~5% of tag-instances and MASK-TIE cannot detect it. Needs its own milestone.
3. **`I11-A` on Nanite hosts** (LG-9 §5.3): a Nanite target reaches `MEASURED_ZERO` (veto) rather than `NOT_MEASURED` (admit) because the pass ran for other actors. On `L_Convolution_Blockout` that deleted 100% of events. The `H6` safe-direction argument does not hold there.
api-equivalent cost (not billed on Max): $85.57 | turns: 304 | running total across all briefs: $923.24
