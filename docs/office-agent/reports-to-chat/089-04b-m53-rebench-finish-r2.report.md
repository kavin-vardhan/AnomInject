# REPORT for 089-04b-m53-rebench-finish-r2.md
verdict: NEEDS-DECISION (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 089-04b-m53-rebench-finish-r2.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-09-30 08:44:49 | finished: 2026-09-30 09:05:03 | duration: 20 min | exit code: 0
context at last turn: 499439 tokens (50% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\089-04b-m53-rebench-finish-r2.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: NEEDS-DECISION

**Summary for the owner**
- The re-bench ran cleanly overnight and every fixture check passed. Everything this brief asked for is done and pushed. Nothing was launched, built or re-run.
- Two of the ruling's expectations turned out wrong when measured. Normal-map mode checking **passes** (the ruling expected "insufficient"), and RD5's "−4" is a mistake in the frozen evaluator. The labels are fine.
- The MainWorld uv test objects are both Nanite, so no mask exists and their timing can't be judged from pixels. The labels on those runs are still consistent.

**Per-gate result** (full table in journal §2)

| gate | verdict | reading |
|---|---|---|
| Launches / postflight / restore | PASS | 47/47 accepted; 10 NOT-RUN-PREMISE as predicted; restore on attempt 1; 75.9 min |
| Census premise / host join | PASS | uv 0 eligible (over_budget 2, partial_footprint 1, texture_not_parameter 2); normal 0; bench join exact both families |
| Host probes 1U / 2U | USABLE | SM_Ramp2 and RoomBuilderSquare each applied 4/4 under the 256 MiB bench raise |
| P-ORC | as-built FAIL → **PASS per family** | uv16 16/16, uv64 64/64, normal 16 cells, interior 0 on each family's own recipe |
| G-LEVER-OFF / G-CENSUS | PASS | |
| B-M53 fixture, 4 modes | PASS | 17/17 onsets and offsets at +0/+0 in every mode; office kit agrees |
| G-MODE | as-built INCOMPLETE → **PASS 4/4** | §4 below |
| Arms CD3 / CD5 / RD3 / NOAPPLY | proven | +1 / +3 / end +1 / holes (7/7) |
| Arm RD5 | E1 {−4:7} / {−4:6} → **pixels +3 on 8/8** | §5 below |
| MIXED_N / MIXED_UV | PASS | 6/6 `partial_footprint:1/2:no_normal_map`, 0 applied / 6/6 APPLY, +0/+0 |
| G-BIND | PASS | every one of 57 frames changes the layer half (1,192 px); uniform half 8 < 16; far strip PASS |
| MainWorld uv (5 ON cells) | unjudgeable | Nanite, no mask; 0 judged; ND-5 rate 1.0 |
| GT-NULL | PASS | wrong-object PASS on all 5 ON legs |
| COST | reading | below this instrument's resolution (\|B−A\| 0.092 ≤ A spread 0.112) |
| label-rule / co-entry | PASS | all legs exit 0; 0 co-entry frames over 30 legs |

**G-MODE re-evaluated**
There were two family mix-ups, not one:
1. **The gate** checked the overall P-ORC. The uv legs' null frames are lit, so the normal oracle failed there and blocked the uv rows even though both uv oracles passed.
2. **The premise lock** (`089-04-window.py:175`) ran all oracles on the uv and normal recipes' frames pooled together.

The fix lives in a new module, `_reviews\089-04b-gmode.py`, with the diff declared. All 26 frozen files are unchanged and the as-built report is kept. Unit tests are 12/12 in both directions: a uv row is not INSUFFICIENT when only the normal oracle failed, and a normal row does not PASS when the normal oracle failed.

| mode | as-built | fixed | match | wrong-mode negatives (all < 0.5, fail as required) |
|---|---|---|---|---|
| tile | INSUFFICIENT | PASS | 9 events, 1.0 | n4 0.040 · n16 0.054 · identity 0.040 |
| scramble | INSUFFICIENT | PASS | 9 events, 1.0; logged pair = recomputed 9/9 | ±1 0.000 · forward 0.191 · identity 0.017 |
| invert | PASS | PASS | 1,152/1,152 cells | green_flip 0.0625 · identity 0.0625 |
| green_flip | PASS | PASS | 1,152/1,152 cells | invert 0.0625 · identity 0.0625 |

Both nulls read as each mode also PASS.

**RD5 — not a label defect**
- The log shows the revert at frame 68 and the delayed restore at 73.
- The capture skips 2 engine frames after each transition, so the 5 late engine frames show up as 3 captured frames (si 55–57).
- In the pixels, every labelled frame (si 47–54) sits at the full corruption level (mean difference 73.1), and si 58 is clean (0.35). The picture outlasts the label by exactly +3, which is what the arm declares.
- E1 reads −4 because it measures the offset against the 6 frames right after the label, and 3 of those are still corrupted. Its trim takes its noise threshold from that same window, so it drops none of them. RD3's window has one late frame and is trimmed correctly.
- The leg still FAILs, so the safety direction holds; E1 just names the wrong edge (G419).

**MainWorld hosts**
- Confirmed Nanite. Every fire logs `reason=nanite` (12/12 per ON leg, 17/17 per null) for both SM_Ramp2 and RoomBuilderSquare.
- The labels follow the unmeasurable contract: `target_pixels -1`, `observable null`, `observability_measured false`, `mask.provided false`, and affected frames equal injected frames.
- Label-rule PASS, 0 co-entry frames, GT-NULL wrong-object PASS.

**Normal P-ORC cause**
The normal oracle does **not** fail with `ShowFlag.Lighting 0`. On its own recipe it reads interior 0 and pattern 0 (3/3 frames, and 8/8 on MODE_NNULL). It fails only on the uv recipe's lit frames (`Lighting 2`: interior 12, pattern 21), which the pooled lock mixed in. The fix for next time is to compute each family's P-ORC on its own recipe and gate only that family on it. No oracle or fixture change is needed (G420).

**Commits pushed**
- `a7f542d` (the premise lock) was already on origin; the interrupted session had pushed it.
- `620e3cb` on `feat/m53-uv-normal-corruption`: the journal `docs/sessions/2026-09-30-089-04-m53-rebench.md`, the status block, and gotchas G419–G420. Next free gotcha is G421. No tag, no merge; `m51` untouched.

**NEEDS-DECISION**
1. **Accept G-MODE PASS 4/4, including normal.** This contradicts ruling 2's expected "normal INSUFFICIENT". Also decide whether the readme's evidence basis for normal should now cite this bench reading.
2. **RD5:** does the arm count as proven on the pixel reading (+3 on 8/8)? Please also record E1's limit: when a late picture fills half of its offset window, it misreads an under-label as an over-label.
api-equivalent cost (not billed on Max): $9.16 | turns: 70 | running total across all briefs: $3,847.57
