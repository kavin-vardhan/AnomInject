# SESSION: new-default
# NAME: 071 concorde-feedback
# EFFORT: xhigh
# MODEL: claude-fable-5-1

# 071-01 — Client feedback on the Concorde M2 build: triage, mechanism reads, and the `m49` plan
**READ-ONLY BRIEF. No source edits, no builds, no cooks, no checkout switch.** Docs-only commits allowed.

You are running headless. Nobody answers questions. Your FINAL MESSAGE is the only thing chat reads — give it the structure in §5. On a real fork or a failed verification: stop and mark the final message NEEDS-DECISION. Codename discipline applies (Concorde / Bates / Europa only; never a real title, studio, product, exe or fork name; never transcribe paths from photos).

## 0. Bench / tree rules for this brief
- The main checkout is on `europa-e1` by the shared-tree rule (CLAUDE.md, refined 2026-09-01). **Do not switch it.** Use `git worktree` for every read: one at `master` (`origin/master`, tip should be `914f14b`) and one at `2b6c93f` (the plugin tip of the Concorde delivery bundle, 2026-08-24). State both SHAs in the report.
- Journal: `docs/sessions/2026-09-04-071-concorde-feedback-triage.md`, committed on `master` from the master worktree, path-scoped add. No other commits.

## 1. Context (from chat — do not re-derive)
- Concorde's delivered build = the 2026-08-24 bundle = master at `2b6c93f` (m33 era). It is **pre-m40** (order-independent label sampling), **pre-m41/m43…m48** (census, target mask, hidden-class masks, view-rect mapping, shader readiness, exposure dip). Concorde ini: 7.5/300 LOD, clip radius 300, two `+ExcludedTargetNamePatterns`, mask + delivery defaults True — so **the m26/m27 veto was LIVE** in that build (`bMaskMeasureDefault=True`).
- The client captured `session_20260901-165056` (1080p, ~1,200 frames) and reviewed start / middle / end by eye; they say the same observations recur through the whole video. We do NOT yet have the session artifacts (chat has asked for them). Everything below is the client's eye reading, transcribed from their review document. Treat their frame numbers as `session_index`.

## 2. The eight findings + three recommendations (verbatim in substance)

| # | record | type | object | current label | client's eye | client's status |
|---|---|---|---|---|---|---|
| F1 | idx 2 | missing_texture | InstancedFoliageActor_0 | frames 27–34 | visible 26–33 | correction: whole range shifted one frame late |
| F2 | idx 0 | blinking | StaticMeshActor_64 | window 4–10; indices [4,5,9,10] | two runs 4–5 and 9–10 | why one record, not two events? `frame_count` semantics unclear |
| F3 | idx 4 | missing_object | StaticMeshActor_47 | frames 51–58 | visible 50–58 | correction: first frame missing |
| F4 | idx 5 | blinking | StaticMeshActor_2261 | window 64–70; indices [64,65,69,70] | object leaves the frame; 69–70 not observable | keep 64–65 only as positives |
| F5 | idx 9 | missing_texture | StaticMeshActor_33 | 111–118 | nothing visible in any of the 8 frames | likely false positive (off-screen / behind another object?) |
| F6 | idx 41 | blinking | StaticMeshActor_35 | window 496–502; indices [496,497,501,502] | affected 495–497 and 499–502; only 498 clean | correction |
| F7 | idx 44 | missing_texture | StaticMeshActor_261 | 531–538 | cannot be seen (outside view / occluded / too small) | invalid image-level positive |
| F8 | idx 98 | missing_texture | WLD_EasyFog_BP_C_3 | 1179–1186 | only 1179–1184 affected; 1185–1186 clean | correction |

Recommendations: (R1) `start_frame`/`end_frame` = min/max **visually affected** frame; (R2) separate event-span length from observable-affected count (`span_frame_count` + `affected_frame_count`); (R3) validate camera visibility before exporting scene-level injections as image-level positives — separate `injected_in_scene` vs `visually_observable` labels.

## 3. What chat already established (verify, cite, do not relitigate)
- `WriteSessionAnnotation`: `frame_count == frame_indices.Num()`, `start/end = first/last index`, `frame_indices` = the ACTIVE SUBSET for toggling types (`8c64be6`, 2026-08-22 — in the bundle). So the client's "frame_count describes the span" should be FALSE for their data — **T4 confirms this at `2b6c93f`**.
- `missing_texture` and `corrupted_texture` swap EVERY slot on EVERY mesh component (`Anomaly_MissingTexture.cpp:106-126`), so "swapped slot not facing the camera" is NOT a candidate for F5/F7.
- The m26 mask is occlusion-aware (`AnomalyVisibleMask.usf:38-43`, custom depth vs scene depth, front-most only); the veto deletes an event whose max drawn count is zero. F5/F7 SURVIVED the veto ⇒ the target drew ≥1 front-most pixel on some frame.
- Labels on `2b6c93f` were sampled inside the capture subsystem's own tick; m40 moved the sample to `FWorldDelegates::OnWorldTickEnd`. P9 (Bates) was the same class; the mechanism is host-dependent subsystem tick order (ledger §8, closed).

## 4. Tasks

**T1 — the timing class (F1, F3, F6-onset).** Ledger §8 style: transition arithmetic only, no free parameters, "consistent with" never promoted.
(a) On `2b6c93f`, state the exact sample point and, for a host where the capture subsystem ticks BEFORE the injector, derive the label-vs-pixel offset at onset AND at revert for `missing_texture`, `missing_object` and `blinking`. Does that one ordering produce all three observed shapes at once: F1 (+1 at onset and +1 at revert), F3 (+1 at onset, revert matched), F6 onset at 495 vs label 496? If F3's matched revert does not fall out, say so and name what would explain it (candidate only; e.g. re-appearance under temporal AA read by the eye as "still missing").
(b) Does ANY gate on master certify onset AND revert alignment for `missing_texture` and `missing_object` in BOTH tick orders (m40's legs were blinking's cadence; m44's ONSET gate is the target mask)? If not, specify the leg precisely (pixel-difference-at-onset like the O4 measurement: first labelled frame differs from its predecessor above the ~0.5 % baseline; first frame after `end_frame` differs from `end_frame`), both orders, packaged target. Do not run it.
(c) F6's 499–500 do not fit a one-tick shift of a 2-3-2 cadence. Name the candidates (the re-appearance opacity ladder in ledger §8 is one) and, for each, the ONE artifact reading that discriminates it. NOT CLAIMED.

**T2 — the observability class (F4, F5, F7) → design the per-frame observability layer.**
Read the reduce table (per-tag count + bounds per frame; m39's input), the m26 arm/veto path, `mask_state`, the logical-hidden registry (m45). Design, do not implement:
- `labels.jsonl`, per anomaly entry: `target_pixels` (int: front-most drawn pixels of the target on THIS frame from the reduce table; for hidden-class frames the would-be silhouette count, m45; `-1` = unmeasured) and `observable` (bool: event active on this frame AND `target_pixels >= ObservableMinPixels`).
- Knob `IAI.Capture.ObservableMinPixels` / ini `[AnomalyCapture] ObservableMinPixels`. **Default 1.** Chat sets a calibrated number when the client's session arrives (G135: no uncalibrated tolerance). Note whether a per-mille-of-picture form is better than absolute pixels at 720p/1080p — argue, then recommend one.
- `annotation.json` per event: `affected_frames.frame_indices` / `start_frame` / `end_frame` / `frame_count` become the OBSERVABLE subset (this is the client's R1 and the semantic tightening they asked for — record it as a semantic change of existing keys, not additive); add `affected_frames.span_frame_count` (= end−start+1) and a sibling `injected_frames` {start_frame, end_frame, frame_count, frame_indices} = today's active subset, unfiltered. Keep `manifested`, `coverage_ratio`, `coverage_pct`; add `observable_frame_count`. The veto's deletion semantics stay EXACTLY as they are this milestone (an event with zero observable frames but nonzero drawn pixels stays in the file with an empty observable set — say whether that is right or whether it should follow the veto; recommend, chat decides).
- F5/F7: what produces max drawn count > 0 yet nothing an eye can see? Candidates with the discriminating reading each: (i) a sliver of pixels (`target_pixels` magnitude); (ii) the checker material's mip collapse to flat grey at distance (a checker at small on-screen size is indistinguishable from an untextured surface — what would `coverage_pct` / bounds say?); (iii) partial occlusion. Say which the `target_pixels` field alone would settle.
- F1's target is `InstancedFoliageActor_0`. m27 (`409b67a`, 2026-08-20) excluded `AInstancedFoliageActor` from the renderable-visible set and is inside `2b6c93f`. Read the selection path at `2b6c93f` and explain how it was still chosen (auto pool vs manual vs a path that bypasses the viewport layer). MEASURED from code, or CANDIDATE.

**T3 — F8, the fog blueprint.** `missing_texture`'s revert already knows the pattern "the game replaced it with '%s' after apply" (`Anomaly_MissingTexture.cpp` revert). Hypothesis (chat, unmeasured): the fog BP re-applies its material (dynamic material instance) some ticks after our apply, so the checker vanishes early while the active flag keeps labelling. Read: (a) whether translucent-only actors can be fire targets outside the census on master (m41's rule lives at the census layer; Concorde's build had no census) and at `2b6c93f`; (b) whether any apply-state audit exists per frame. Propose and cost: (i) picker-level exclusion of translucent-only targets for texture-swap types (or all types — argue); (ii) a per-frame "material is still ours" audit at `OnWorldTickEnd` feeding `observable=false` when the slot no longer holds our material (this also covers any game that swaps materials at runtime). Say which of (i)/(ii) or both `m49` should carry.

**T2b — THE STRUCTURAL FIX (this is the design the plan must carry, not an option).** Every desync so far was host-specific and invisible on the bench (P9 on Bates; F1/F3/F6 on Concorde), and the client checks labels by looking at pixels. So the label must be derived from the frame's OWN render, not from CPU state sampled somewhere in the tick: per captured frame, `observable` = the target's tag is present in THIS frame's stencil/mask (front-most, occlusion-aware) AND the anomaly's visual condition holds on the target in this frame (texture types: our material is on the slots at draw time; hide types: main-pass-off state at draw time). A frame that the GPU did not draw the anomaly on can then never be a positive, and timing can no longer depend on tick order because the evidence and the picture come from the same frame. Read what the mask pass already gives per frame and specify the minimum that closes the loop. If the per-frame material/hidden condition cannot be read at draw time cheaply, say what CAN and what the residual risk is.

**T6 — a label-vs-pixel verifier the client can run.** Extend `verify_capture` (it ships in the bundle) with a check over any session folder: for each event, pixel difference inside the event's bbox/mask between `start_frame-1` and `start_frame` must exceed the session's baseline while `start_frame-2`→`start_frame-1` does not (the onset really is at `start_frame`); the mirror at `end_frame`→`end_frame+1`; and for every positive frame the target's visible pixel count ≥ threshold. Output one line per event: PASS / ONSET-SHIFT(+n) / END-SHIFT(+n) / NOT-VISIBLE. It must work on the M2 bundle's artifacts too (no mask files there — bbox only; say what it can and cannot conclude without masks). This becomes a delivery gate run on the HOST's own smoke session, not the bench, and the client gets it in the changelog.

**T4 — schema check at `2b6c93f`.** Confirm `frame_count == frame_indices.Num()` there and that blinking's `frame_indices` is the gapped active subset. If anything differs from master, quote it.

**T5 — the `m49` plan.** File-by-file plan for `m49` = per-frame observability derived from the frame's own render (T2 + T2b) + schema v2 + the verifier (T6) + the chosen T3 item, sequenced against `m39` (honest bbox; same reduce-table input — recommend fold-in or m39-first with reasons). Gates: both tick orders; ONSET, MASK-PICTURE-PAIRING, G7 untouched; P-C7 (packaged bench binary byte-inert?) — state whether the knob default 1 changes any delivered artifact and what re-anchors; a can-fail leg for `observable` (e.g. a target driven off-screen mid-event must read `observable=false` on those frames — the F4 shape). Client-facing doc deltas (`client-readme.md` §8 schema; the mask section). Effort estimate. State explicitly what in `m49` also changes Bates' upcoming delivery (chat ruling to follow).

## 5. Required final message structure
1. Plain-language rundown (10 lines max, no jargon).
2. Per-finding table: finding | mechanism (MEASURED / CONSISTENT-WITH / CANDIDATE) | already fixed on master? (which milestone) | what `m49` adds.
3. T1(b) leg spec, T2/T2b design, T6 verifier spec, T3 recommendation, T4 result, T5 plan (with the m39 sequencing recommendation).
4. Open questions for chat, numbered.
5. NEEDS-DECISION items, if any (product/scope only).
6. Journal path + commit SHA (master worktree), worktree paths created and whether removed.

Do NOT start any implementation. `m49` implementation is QUEUED behind chat's review of this report.
