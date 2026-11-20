# EFFORT: xhigh
# MODEL: claude-opus-5

# 072-02 — m49 Phase A1: per-frame `target_pixels` + `observable`, the onset join fix, schema v2 — implement against the approved plan (journal 071 §8.1) and gate on BOTH fixtures
Headless; nobody answers; FINAL MESSAGE structure in §6; any failed gate = report and STOP (no iterating for green; the fix turn is a separate brief). Codename discipline.

## 0. Rulings on 072-01's NEEDS-DECISION (record in the journal)
1. LG-3 (host-PP custom-depth reader count reaches run_summary) and LG-4 (census hygiene separates host custom-depth writes from plugin leaks BY STENCIL VALUE before failing P-C6) are FOLDED INTO Phase A — LG-4 in THIS brief (it otherwise fails every Lyra leg), LG-3 in A2.
2. Lyra IS a standing gate host for m49: LG-1, LG-2, LG-5, LG-6, LG-8 join the pre-declared gate set; LG-7 is a regression check; LG-9 (Nanite leg on L_Convolution_Blockout) is a diagnostic leg in A2, not gating.
3. Minor items: add `tools/__pycache__/` to .gitignore here; `mask_map.json last_frame` and `lyra_leg.ps1` archive-not-delete go to A2.
4. 🚨 NEW READ REQUIRED (before any leg): Lyra's `MPP_Outline` reads CustomStencil. Determine from the material graph/asset how it selects what to outline — a specific stencil value/bit, or "any non-zero". If any-non-zero, then every census-tagged actor (200..254) is OUTLINED in Lyra's clean frames = an unlabelled artifact in negatives (the 069 hazard, G120 until now). Report which, and if it is any-non-zero, add a StartRun REFUSAL-OR-WARN rule proposal (do not implement here) and run this brief's Lyra legs with census OFF, stating so. This matters for Bates too (it writes custom stencil on some actors).

## 1. Scope of A1 (from journal 071 §8.1 Phase A — the label core only)
- `FCaptureSnapshot` += `TargetPixels`, `ConditionHeld`, `Observable`; labels.jsonl per-anomaly `target_pixels` (int; -1 unmeasured) and `observable` (true|false|null).
- `ServiceTargetMask` records per-session-index per-tag Count (TargetMaskOutcome → {state, counts}); the drain join fills `TargetPixels[i]` by the event's OWN tag. 🚨 THE ONSET JOIN GAP IS IN SCOPE AND IS THE FIRST THING TO FIX: today the first labelled frame of every FireWindow event carries `mask_value 0` while its PNG carries the tag (4/4 on Lyra, none on ActorHidden). Find the mechanism (record creation vs. arm ordering at OnWorldTickEnd is the obvious suspect — MEASURE, don't assume), fix it, and make `target_pixels` and `mask_value` come from the same join so they cannot disagree.
- `IAnomaly::IsVisualConditionHeld()` (default = IsCurrentlyAnomalous) with MissingTexture/CorruptedTexture overrides (every captured slot still checker/pink-derived); `OnWorldTickEndSample` computes `ConditionHeld[i]`; `frames_condition_lost` in run_summary.
- `observable = active && ConditionHeld && target_pixels >= ObservableMinPixels`; knob `IAI.Capture.ObservableMinPixels` / ini `[AnomalyCapture] ObservableMinPixels`, default 1, G139 echo with the per-mille equivalent.
- annotation.json v2: `affected_frames` = observable subset (+ `span_frame_count`), `injected_frames` = today's active subset, `observable_frame_count`, `unmeasured_frame_count`, `observability_measured` (false ⇒ affected_frames falls back to injected), root `label_schema: 2`. Veto semantics untouched. Events with zero observable frames stay.
- LG-4 hygiene-by-value; `.gitignore` `tools/__pycache__/`.
- NOT in A1: translucent-only picker exclusion, bench levers, m39 bbox fold-in, LG-3, docs beyond the journal + predictions + a client-readme v2 stub (A2 finishes docs).

## 2. Predictions first
`docs/predictions/2026-09-04-m49-phase-a1.md` before any leg: the mechanism you expect for the onset gap and the reading that confirms it; every gate below with its pass condition; what would falsify.

## 3. Build
Editor + packaged bench targets from the main checkout (both-targets rule, G221). Stage the new exe; archive DE65F84A as the A-side; hash both. Refresh the Lyra worktree to the new master SHA (detached) and rebuild LyraEditor; prove the main checkout untouched afterwards.

## 4. Gates (pre-declared; both tick orders where per-frame)
StackOBot (bench, packaged): ONSET, MASK-PICTURE-PAIRING (100%+50%), G7, MASK-TIE re-run unchanged · G-EDGE 8 legs re-run with the new binary (PASS 0/0) · OBS-1 inertness: on the settled bench `injected == affected` on every event, delta 0 · OBS-5 P-C7 v3: byte-identical to DE65F84A's artifacts except the declared new keys; `frame_indices` identical value-for-value on a pose-matched pair · the onset join: `mask_value` and `target_pixels` non-zero on the FIRST labelled frame of every FireWindow event (was 0/4 — the can-fail negative already exists).
Lyra (L_ShooterGym, host-shaped recipe from 072-01): LG-1 (onset join, first labelled frame carries its own tag and count) · LG-2 (event-3-shape: an unmeasured frame inside a window reads `observable: null`, `unmeasured_frame_count 1`, dropped from affected, kept in injected — if no such frame occurs naturally this run, say so; do NOT fake it) · LG-5 · LG-6 (verifier report-only on the Lyra session, quote confidence) · LG-7 · LG-8.
Known-answer, if present: if `D:\IntrusiveAnomalies\_bates_reads\` contains a new Bates session folder (owner banking it today: two texture events labelled one frame EARLY at onset, masks correct), run the verifier on it and report — expected ONSET-SHIFT on exactly those two events. Read-only.

## 5. Journal + commits
Journal `docs/sessions/2026-09-04-072-lyra-fixture.md` §N (append) — or a new 072-02 file, your call. Conventional commits: predictions → `feat(capture): m49 phase A1 - per-frame target_pixels/observable, onset join fix, schema v2` → docs. Push master when green. Both checkout states in the report.

## 6. Final message
1. Plain-language rundown (10 lines). 2. The onset-gap mechanism: MEASURED, with the reading. 3. MPP_Outline finding (§0.4). 4. Gate tables (StackOBot / Lyra), every per-frame gate both orders. 5. Schema v2 sample (one event, one row). 6. Commits, exe hashes, checkout states. 7. Open questions / NEEDS-DECISION.
