# 081-32 — m55 requalification: legacy-identity AMENDMENT 5 implemented and proven; CB-N passes under it from the bank; one ruled proof (colour) cannot fail under the existing rules — NEEDS-DECISION, boundary NOT re-issued (Claude Code, Opus 5.5, 2026-09-26)

## Goal
Brief 081-32 (bench-free, xhigh), ruling `_reviews/081-32-chat-ruling-legacy-identity-amendment-5.md`. The work:
- run the two pre-conditions;
- declare AMENDMENT 5 and implement rulings A, B and C in the requalification lib;
- run proofs 1–8, including the CB-N re-evaluation from the bank (081-26 mechanism) and a boundary re-issue with a full dry run.

c1 would then resume at `CB_S1` in 081-33. Build 3 is `85c99b3`, Game 002805CF. Boundary on entry: lib `bb4fbb9d…`, window `56e86f2e…`,
feature head `66976ea` (local ref; `origin` was already at `cfff033` from 081-31).

Nothing was launched: no game, window, chunk or leg. No build and no plugin source change. `master`, `m51`, tags, containers and
CaptureBench are untouched.

## Pre-conditions (ruling A) — both clear
- **Source proof** (`_reviews/081-32-evidence/precondition-source.{py,json,diff}`, 14 of 14):
  - `git diff 031a103 85c99b3` over every function on the coalescing / PixelOwner path.
  - **The `PixelOwner` selection and the `k == PixelOwner` hand-off are byte-identical.** `Drain_RenderThread` and
    `AfterTonemap_RenderThread` lose no line. Every added block is either bookkeeping of the parallel `ChangeIssues` array or guarded by
    an issue-validity test.
  - `IsActiveThisFrame_Internal` and `TakeMaskResult` are byte-identical.
  - `ServiceTargetMask`:
    - its UNAVAILABLE decision (`pixels < W*H`) is unchanged;
    - its additions are `ChangeStage.IsValid()`-guarded;
    - the one rewritten `EnqueueTargetMaskPng` call carries the same Gray bytes when there is no frozen mask.
  - `ChangeStage` is created only when `FAnomalyChangeStage::IsEnabled()`. Census and m26 arms pass the default `nullptr` issue.
  - So with evidence OFF, no issue in the path is ever valid.
  - Beyond the ruling's wording: the `Issue.IsValid()`-guarded addition is the only change **inside the PixelOwner loop**. Around it
    there are more additions, all of the same guarded kind.
- **Bank search** (`precondition-bank.{py,json}`): all 30 banked legacy legs since 081-11 — 081-12 ×12, 081-22 D ×8 and C ×4, c1 CB-N ×6.
  081-11 and 081-15 banked none. Every leg ran on attempt 1, so there are 30 attempt logs.

  | binary | evidence | legs | with signature |
  |---|---|---|---|
  | pre-m55 0844220E | n/a | 8 | **0** |
  | build 3 002805CF | OFF | 6 | **1** (CB_N2 si 29) |
  | build 3 002805CF | ON | 4 | 0 |
  | m55 stage 1 AEBD09EA | OFF | 4 | 0 |
  | m55 stage 2 A699E9EE | OFF / ON | 4 / 4 | 0 / 0 |

  **CB_N2 si 29 is the only `TARGET MASK UNAVAILABLE` line in the whole legacy bank.** Build 3 has it on 1 of 10 legs, against the stop
  threshold of ≥ 3, so the stop rule does not fire.

## AMENDMENT 5 as implemented (`_reviews/081-23-lib.py`, `bb4fbb9d…` → `c36a60c2…`; the window runner is unchanged)
- **A — COALESCED-EXCUSED.**
  - `coalesced_unmeasured(text)` finds `TARGET MASK UNAVAILABLE … pixels=0`. It then requires the last `M23 PASS` line serving that id to
    also serve an **earlier target-mask arm** (id bit 61, lower serial, earlier in the list; ≥ 2 target arms).
  - A frame is excused only if it carries that signature **and** its label row reads `mask_state: unmeasured`.
  - The excusal is strict: for every pair, the comparator is re-read from its own raw files with **that frame made unmeasured by the
    writer's own rules** (`_unmeasure_raw` / `legacy_view`). Every rule then runs unchanged on that view. Each derived field must
    therefore equal the exact value the comparator would carry, not merely "may differ".
  - Symmetric across binaries.
  - Cap: at most 1 frame per leg and 1 leg per group. The third-build-3-leg frequency rule reads every window's group rows. Beyond the cap
    the verdict is NEEDS-DECISION.
  - Reported as `coalesced_unmeasured_excused` in the comparison JSON and in a new per-group ledger row.
- **B — one leg-wide tag bijection.**
  - π is built from every label `mask_value` and every mask PNG. It must be one-to-one and consistent across frames, with 0→0.
  - For every common mask, **x's pixels are remapped by π and hashed against y's decoded pixels.**
  - Also required: per-tag pixel counts, occupancy, `mask_ties` (tag remapped on MAIN) and the MASK-TIE cross-check are equal, and actor
    identity is preserved both in the labels (tag → `target_name`) and in `mask_map.json` (tag → event_id / target / type).
  - Only when all of that holds are the relabelled `mask_value` and mask-file paths excused.
  - π is reported per leg against the first pre-m55 leg.
- **C — the cross-pair clause excludes the historical run-unique set.**
  - The set is hash-pinned (`a347374e…`, 108 paths), checked before any leg runs.
  - `census_cycles` is tabulated per leg in every comparison and group row.
- **Harness plumbing.**
  - A group row (`LEGACY_GROUP_<map>_<order>`) is appended after every comparison.
  - A group with a PASS / PASS-REEVALUATED row is skipped. A recorded FAIL / NEEDS-DECISION re-raises without re-evaluation.
  - A NEEDS-DECISION verdict stops with its own word.
- **Identity.** On all 30 legacy legs, the amended lib's readings and flats are byte-identical to the old lib's on every pre-existing key.

## Proofs
| # | expected | actual | evidence |
|---|---|---|---|
| 1 A negatives | FAIL / NEEDS-DECISION | **8 of 9**: no-signature ×2, non-derived field ×3 → FAIL; 2 frames, 2 legs, third build-3 leg → NEEDS-DECISION (+2 frequency controls PASS). **"Colour differs" → PASS, not FAIL** | `am5-proof.json` |
| 1 A symmetric | excused on a pre-m55 leg and on a build-3 leg → PASS; same frame without signature → FAIL | 5 of 5 (MAIN D-N, synthetic) | `am5-proof.json` |
| 2 A positive | CB_N2 si 29 COALESCED-EXCUSED | as expected; the pre-081-32 lib FAILs the same readings | `am5-proof.json`, `proof/cb-n-amended.json` |
| 3 B negatives | FAIL | 6 of 6 (many-to-one, inconsistent, one pixel, per-tag count, actor, event swap) | `am5-proof.json` |
| 4 B positive | CB_N5 equal under π | π = {247→227, 248→247, 249→248, 250→249, 251→250, 252→251, 253→252, 254→253}, 30 masks pixel-identical after remap, 89 paths excused | `am5-proof.json` |
| 5 C | census_cycles excused; outside field FAILs; hash unchanged | 10 of 10 (both orders; old lib flags census_cycles; tampered set refused) | `am5-proof.json` |
| 6 no flips | every earlier verdict unchanged | 16 comparisons and dry-run mutations; the only flip is c1 CB-N FAIL → PASS (the intended re-evaluation); no PASS → FAIL | `proof6-no-flips.json` |
| 7 CB-N re-evaluation | PASS-REEVALUATED | **PASS-REEVALUATED, computed but NOT written** (would-be row `cb-n-reevaluation/ledger-row.json`) | `reevaluate-cb-n.py` (dry) |
| 8 boundary + dry run | re-issued, all green, c1 resumes at CB_S1 | **boundary NOT re-issued.** Dry run 198 of 198 (081-30's 182 identical + 16 new); resume proof 5 of 5 (with the row: first launch `R23_CB_S1`) | `dryrun/comparison-vs-081-30.json`, `resume-proof.json` |

Two synthetic MAIN symmetric cases first failed because my fixture kept the real `MASK-TIE si=29` line. A frame that is really
coalesced-unmeasured logs none (checked on CB_N2), so the fixture was corrected and they pass. Three resume scenarios first failed on my
checker's expectation about the `finally` block's `quiet()` call (the shape G293 already records). Neither was a harness defect.

## The stop — proof 1's colour negative
Ruling A says everything outside the excused set "must pass the existing rules. That includes the frame's colour file". Proof 1 expects
"signature present, but the colour differs → FAIL".

**No legacy rule reads colour frames.** The flat holds labels, annotation, run_summary, run and the mask PNG hashes. The `Actual_Frames`
hashes are stored and never compared. Measured: colour frames differ on 20–44 % of pixels between **any** two CB-N legs, including the two
pre-m55 runs of the same binary (CB_N1 vs CB_N6, si 29: 184,214 px, max Δ 59). Replacing CB_N2's si-29 colour frame with solid magenta
therefore PASSes, **with or without AMENDMENT 5** — the excusal does not reach colour because nothing does.

Making it FAIL would take a new colour comparison with a tolerance. The ruling excludes new thresholds, and one learned from one control
pair would repeat G294. Stopped rather than bent. What does fail is covered: the frame's own colour-side label fields (`bbox_px`, …), any
other frame, the event's `injected_frames`.

## Also for chat: the excused set in practice
The comparator view moves exactly 20 paths. 14 are named by ruling A:
- `mask_state`, `mask_file`, `target_pixels`;
- 4 × `bbox_drawn_px`;
- `observable` (the si's membership);
- 4 × `affected_frames`;
- the mask PNG;
- `target_mask_frames_measured`.

6 are fixed arithmetic consequences the ruling does not name:
- `target_drawn_pixels` −1, the same reduce's drawn count;
- the event's `observable_frame_count` −1 and `unmeasured_frame_count` +1 (both inside the `event_ranges` field-set line the ruling
  anticipates);
- run_summary `observable_frames` −1, `target_drawn_pixels_measured` −1 and `target_mask_frames_unavailable` +1.

Each must equal the writer-exact value. Chat should confirm this reading.

## State left
- **Harness:** `081-23-lib.py` is the amended lib (`c36a60c2…`). Its pre-081-32 copy is `_reviews/081-32-evidence/081-23-lib.pre-081-32.py`
  (`bb4fbb9d…`).
- **The boundary is NOT re-issued.** `prep-boundary.json` still pins lib `bb4fbb9d` and feature head `66976ea`, so the preflight refuses
  every window. That is deliberate until chat rules.
- **c1 ledgers are untouched:** no CB-N group row, and `legacy-CB-N-comparison.json` is unchanged.
- **Once ruled:**
  - `reevaluate-cb-n.py --commit` archives the 081-31 c1 files to `081-32-evidence/c1-invocation-3-081-31/` and appends the
    PASS-REEVALUATED row;
  - then re-issue the boundary (lib, feature head), replay the preflight offline, and re-run the dry run if the lib changed;
  - 081-33 then runs `081-23-window.py c1 540 081-33`, resuming at `CB_S1`.
- **Evidence:** `_reviews/081-32-evidence/`.
- **Recorded:** 🆕 G295 (the coalesced-arm yield issue, FUTURE fix) and G296 (a negative proof presupposes the comparator reads the thing
  mutated); G294 is resolved in the harness by ruling C and takes effect with the re-issue.

## Hand-off
NEEDS-DECISION:
1. Proof 1's colour negative: drop it as out of legacy scope, replace it with a colour-file presence/metadata check, or define a colour
   comparison.
2. Confirm the six writer-derived fields in the excused set.

Nothing else is outstanding for 081-32.
