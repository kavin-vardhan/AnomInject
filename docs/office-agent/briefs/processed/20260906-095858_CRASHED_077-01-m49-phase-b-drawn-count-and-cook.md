# SESSION: new-default
# NAME: 077 m49-phase-b
# EFFORT: xhigh
# MODEL: claude-opus-5

# 077-01 — m49 Phase B: the GPU "drawn" mask output (DrawnCount) so hidden-class observability comes from the picture; exposure_dip excludes live targets; ONE cook; full gates both orders on both fixtures
Fresh session. COLD START: `CLAUDE.md` status → `docs/sessions/2026-09-04-076-m50.md` → journal 071 §5 (T2b) and §8.1 Phase B file list → journal 073/074 for the A1/A2 gate set → gotchas G251–G254. Summarise in 5 lines before acting.
Headless; nobody answers; FINAL MESSAGE structure in §6; failed gate = branch-and-restore, report, STOP. Codename discipline. ⛔ Office data never leaves the office PC. ⛔ Never write temp files into `_mailbox`.

## 0. Rulings on 076-01's NEEDS-DECISION (journal them)
1. `exposure_dip` firing on hide-class anomalies: FOLDED INTO PHASE B, additive — the m48 detector's whole-frame mean EXCLUDES the pixels of every live fire target's mask (the silhouette, present or would-be) on that frame; when no mask is available (unmeasured target) the frame is compared as today and `exposure_dip_scope: "frame"` vs `"frame_minus_targets"` is recorded per row. Gate: a pinned-exposure hide-class leg must read `frames_exposure_dip 0` post-fix (was >0), and the m48 AE-ON smoke leg must still fire (>0).
2. The `_mailbox` temp-file slip: noted, closed; the rule stands.

## 1. Scope (journal 071 §8.1 Phase B)
- `Shaders/Private/AnomalyVisibleMask.usf`: second output `drawn` = (|CustomDeviceZ − SceneDeviceZ| ≤ DepthBias) ? Stencil : 0 — "the target itself is what is drawn at this pixel". RT0 (the existing front-most mask) is UNTOUCHED.
- `AnomalyShaders/Public/AnomalyVisibleMaskShader.h`: RT1 binding. `AnomalyMaskReduce.usf` + `AnomalyMaskReduceShader.h`: second table (or stride 10) with `DrawnCount` per tag. `AnomalyMaskSceneViewExtension.h/.cpp`, `AnomalyMaskTypes.h` (`DrawnCount`).
- Capture subsystem: hide-class `observable` = active && `DrawnCount == 0` (the GPU says the target is absent from the picture where its silhouette would be) && `target_pixels >= ObservableMinPixels` (silhouette in view); texture-class unchanged (A1's condition + count). Per row add `target_drawn_pixels` (int; -1 unmeasured). Blinking's visible in-between frames must read `observable: false`? — NO: they are NOT labelled frames (G7); leave them unlabelled as today; only labelled hidden frames get the DrawnCount==0 condition.
- Ruling 1's exposure_dip change.
- A1/A2/m50 code untouched except the hide-class observable predicate and the new row key.

## 2. Predictions first
`docs/predictions/2026-09-04-m49-phase-b.md` before any leg: the drawn-count expectations (hidden frames: DrawnCount 0 while target_pixels ≈ silhouette; texture frames: DrawnCount ≈ target_pixels; a control OPAQUE untouched object: DrawnCount == front-most count), the exposure_dip expectations, every gate below.

## 3. Build + COOK (G129: a shader change needs a cook; sequence it AFTER all code-only legs)
Both targets; stage; archive A159EDFD as the A-side. Code-only legs first (editor/packaged hot-swap cannot carry the new shader — state which legs are valid pre-cook and which need the cook). Then ONE cook per PRE-DELIVERY-CHECKLIST §1.1 sequencing; record the new container quartet; the A4-style re-baseline proving the cook changed no behaviour outside the declared keys.

## 4. Gates (BOTH tick orders; StackOBot packaged post-cook; Lyra rebuilt from the new SHA)
PB-1 drawn-count identity: on hidden-class labelled frames `target_drawn_pixels == 0` while `target_pixels > 0` (35/35-style equality) · PB-2 texture-class: `target_drawn_pixels` within 1 % of `target_pixels` · PB-3 control object (opaque, untouched): drawn == front-most · PB-4 can-fail: the m45 "omit depth-pass silencing" lever (20/60 differ) must now read `target_drawn_pixels > 0` on hidden frames and `observable: false` with `frames_condition_lost` or a new `frames_drawn_unexpected` counter — the GPU catching a hide that did not take · PB-5 exposure_dip (ruling 1) both legs · full regression: ONSET, PAIRING 100/50, G7, MASK-TIE, TARGET-PIXELS TIE, G-EDGE ×8, OBS-1..4, m50 G1/G2/G6, black-frame, A44, both targets · P-C7 v3 (declared keys only: `target_drawn_pixels`, `exposure_dip_scope`, any new counters; `frame_indices` identical) · Lyra: Leg A repeat + the `L_Convolution_Blockout` leg (Nanite targets stay unmeasured: `target_drawn_pixels -1`) + verifier report-only with confidence.

## 5. Merge, docs, cold-start
Green ⇒ merge (`feat(capture): m49 phase B - GPU drawn-count; hidden-class observability from the picture; exposure_dip excludes live targets`), push. Journal `docs/sessions/2026-09-04-077-m49-phase-b.md`; CLAUDE.md status (m49 COMPLETE A1/A2/B + m50; NEXT = delivery gates on Lyra with the verifier, then card Section G/F on Bates, then client cook); client-readme §8 gains `target_drawn_pixels` + the hide-class sentence; client-delivery paragraph updated; gotchas. Container quartet recorded in `_binary_baselines/README`. No tag.

## 6. Final message
1. Plain-language rundown (10 lines). 2. Pre-cook vs post-cook leg validity table. 3. Gate table (both orders) + Lyra. 4. Cook record (quartet, re-baseline result). 5. Commits, hashes, both checkout states, pointers. 6. NEEDS-DECISION if any.
