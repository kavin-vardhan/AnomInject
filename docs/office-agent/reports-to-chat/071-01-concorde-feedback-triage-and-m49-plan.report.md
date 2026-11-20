# REPORT for 071-01-concorde-feedback-triage-and-m49-plan.md
brief: 071-01-concorde-feedback-triage-and-m49-plan.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-fable-5-1 | effort: xhigh
started: 2026-09-04 12:36:27 | finished: 2026-09-04 13:11:11 | duration: 35 min | exit code: 0
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\071-01-concorde-feedback-triage-and-m49-plan.live.txt

---- FINAL MESSAGE (Claude Code) ----
Everything the brief asked for is done. Nothing further to request: the source reads are complete, the journal is committed and pushed, both worktrees are removed, and the main checkout is untouched. Final report follows.

```
# 071-01 REPORT — Concorde M2 feedback: triage, mechanism reads, m49 plan
Read-only session. No source edit, no build, no cook, no checkout switch.
Worktrees: master = 914f14b (== origin/master at start), bundle = 2b6c93f.
Journal: docs/sessions/2026-09-04-071-concorde-feedback-triage.md, commit 9682b2c on master, PUSHED (origin/master 914f14b..9682b2c).

## 1. Plain-language rundown
1. The client's build (2b6c93f) derives missing_texture and missing_object labels from state the capture subsystem changes inside its own tick. No subsystem tick order can move those labels. The Bates mechanism (P9) does not reach F1 or F3.
2. Under the injector-first order the same arithmetic reproduces P9's blinking shape exactly, with no free parameter. F6 is not that shape; neither order produces its 495, 499 or 500.
3. So F1/F3/F6-onset come from the picture-to-index pairing or from how frames were read, not from the sample point. Both are decidable from the client's own PNGs by the verifier in T6. The encoder numbers the MP4 exactly as the PNGs, which weakens the reading-offset candidate in the direction the client saw.
4. The owner's only in-round Concorde measurement of this label lineage (2026-08-22, tick pin on) read every offset +0 at both edges, on 5 events.
5. F2 is a documentation gap: frame_count is the count of frame_indices at 2b6c93f and on master; the delivered README defines none of these fields.
6. F5/F7: surviving the veto does not prove the target drew pixels; NOT_MEASURED also survives. annotation.json mask.provided for idx 9 and 44 is the reading that says which. A per-frame target_pixels field settles a sliver, half-settles occlusion, and cannot settle a checker collapsed to grey; the verifier's contrast reading covers that last case.
7. F1's actor cannot be picked by the auto pool at 2b6c93f (the m27 foliage exclusion is in the bundle); only a targeted capture reaches it, and the dashboard defaults to auto-pool.
8. F8: nothing at 2b6c93f or master excludes translucent-only targets at the picker (master does it only through the census), and no per-frame apply-state audit exists; the revert already detects "the game replaced it", after the fact.
9. No gate on master is end-side for any type, and missing_object has never been in an onset leg. The leg that closes it is specified, not run.
10. m49 = target_pixels + observable per frame from the frame's own render, schema v2, the verifier, the fog audit, with m39 folded in. About 5-6 sessions and one cook (shared with the m46 cook every post-bundle delivery already needs).

## 2. Per-finding table
| finding | mechanism | fixed on master? | what m49 adds |
|---|---|---|---|
| F1 (-1 both edges) | CANDIDATE: (a) reading offset, weakened by the encoder's -start_number 0; (b) picture/index pairing slip on the decoupled host, against which the tick-pin +0 measurement stands. NOT tick order (derived). | No fix exists or is needed: texture-type label timing is unchanged 2b6c93f -> master because it was never order-dependent. | target_pixels/observable per frame; the verifier decides (a)/(b) from the PNGs; the G-EDGE leg. |
| F2 (one record, frame_count) | MEASURED: frame_count = frame_indices.Num(), gapped active subset; one fire = one event; README has no field table. | Same on master. | span_frame_count, injected_frames, the v2 field table in the README. |
| F3 (-1 onset, end matched) | CANDIDATE as F1 for the onset; CONSISTENT-WITH a TAA re-appearance ladder at the end if (a)/(b) holds; otherwise an eye reading. | As F1. | As F1; a client-facing line on the revert edge under TAA. |
| F4 (leaves frame; 69-70 not observable) | MEASURED from code: the label needs only a projectable bbox; there is no per-frame observability. | No. | observable=false on those frames; the F4 shape is the can-fail leg OBS-2. |
| F5 / F7 (nothing visible) | CANDIDATE (i) sliver / (ii) mip-collapsed checker / (iii) partial occlusion; first read mask.provided (NOT_MEASURED also survives the veto). | m43/m45 give per-frame masks, not a per-frame gate. | target_pixels + ObservableMinPixels catch (i)/(iii); the verifier's checker-contrast reading reports (ii). |
| F6 (onset -1; 499-500 affected; 498 clean) | Onset: CANDIDATE as F1. Interior: NOT CLAIMED; candidates in journal §3.8 with one artifact reading each. Not the P9 shape by arithmetic. | m40 removes the injector-order class (P9); F6 is not that class. | Per-frame GPU evidence; the PNG luma series decides the interior. |
| F8 (checker gone 2 frames early on a fog BP) | CANDIDATE: the game re-took the material slot (the revert's left-to-game counter is the post-hoc reading). | No per-frame audit on master. | OnWorldTickEnd "material still ours" audit (mandatory) + picker-level translucent-only exclusion. |

## 3. Deliverables
T1(b) leg spec — m49-G-EDGE: packaged bench binary, CB_GateLevel, targeted, {missing_texture, corrupted_texture, missing_object, blinking} x {native, IAI.Bench.SynthTickOrder 1} = 8 legs; delivery OFF; IAI.Capture.Config 2 4 8 14 0 (ceiling +/-7); marker OFF; AA-off configuration; 90 frames; pose gate on. Per event, region = bbox_px (mask when present): ONSET := d(start) > tau AND d(start-1) <= tau; END := d(end+1) > tau AND d(end) <= tau; d(k) = fraction of region pixels differing > 8/255 from k-1; tau = 3x clean baseline, floor 0.5 %. Pass = 0/0 on every event, all 8 legs; any non-zero is a STOP, never re-run for green. Same legs at the delivered configuration REPORT-ONLY (G228). Instrument = the T6 verifier in gate mode, proven on a synthetic +/-1 first.

T2 design — labels.jsonl per anomaly entry: target_pixels (int; front-most drawn pixels of the target on THIS frame from the frame's reduce table; would-be silhouette for hidden-class frames; -1 unmeasured) and observable (bool: active AND condition held at draw time AND target_pixels >= ObservableMinPixels). Knob IAI.Capture.ObservableMinPixels / ini [AnomalyCapture] ObservableMinPixels, default 1, G139 echo; absolute pixels recommended over per-mille (the echo prints the per-mille equivalent). annotation.json per event: affected_frames.{frame_indices,start_frame,end_frame,frame_count} become the OBSERVABLE subset (semantic change of existing keys, schema v2), + span_frame_count; sibling injected_frames = today's active subset; observable_frame_count; root label_schema: 2. Veto semantics unchanged this milestone; an event with zero observable frames stays in the file (recommended; chat decides). The count is computed today in ServiceTargetMask (MASK-TIE's TableCount) and discarded; threading it into the snapshot is the whole mechanism.

T2b structural fix — same-frame evidence already exists for "front-most at N pixels" (the mask pass runs inside the frame's own render; MASK-PICTURE-PAIRING proves it). Minimum that closes the loop: (1) target_pixels from the frame's own table; (2) condition at OnWorldTickEnd — texture types: every captured slot still IsCheckerDerived; hide types: IsLogicallyHidden; (3) observable from (1)+(2)+knob. (4) Phase B: a second mask output "drawn" (|CustomZ - SceneZ| <= bias) reduced to DrawnCount, so hide types get the GPU's own statement that the target was absent from the picture — a global-shader change, needs a cook. Residual risk named: the texture condition is CPU-side (render-thread proxy swaps escape it); IsCheckerDerived walks <= 8 parents; Nanite and translucent-only targets have no mask.

T6 verifier — verify_capture.py --label-pixel-gate, reusing measure_label_offset.py's region/baseline/threshold code (it must be added to bundle_manifest.txt; it does not ship today). Per event: ONSET/END readings as above with a median+6*MAD baseline and 0.004 floor, baseline-contamination banner, MEASURABLE RANGE ceiling; NOT-VISIBLE = mask count < N on any positive frame, or (bbox-only) no pixel change anywhere near the claim; checker/magenta contrast REPORTED. Output one line per event: PASS | ONSET-SHIFT(+/-n) | END-SHIFT(+/-n) | NOT-VISIBLE | NOT-MEASURABLE(why). Without masks it can conclude edge shifts and "no change in the box" but cannot separate sliver/occlusion/empty or judge inside-window observability. --selftest proves both directions (G96/G142). Becomes a PRE-DELIVERY-CHECKLIST box run on the host's own smoke session.

T3 recommendation — carry BOTH: (ii) the per-frame material audit at OnWorldTickEnd feeding observable=false + frames_condition_lost (mandatory; it is the T2b condition); (i) picker-level exclusion of translucent-only targets for ALL types, default ON with a knob (they can never carry an observable positive under m49). If chat wants fog/glass hide events kept, (i) becomes texture-types-only.

T4 — CONFIRMED at 2b6c93f: frame_count == frame_indices.Num(); start/end = first/last; blinking's frame_indices = gapped active subset (sorted ActiveByIndex keys); FireWindow types use AffectedFrames. Identical on master.

T5 plan — files, gates and effort in journal §8. Phase A code-only (hot-swap): LabelWriter, CaptureSubsystem (knob, count join, condition, both subsets, run_summary), IAnomaly::IsVisualConditionHeld (precedent: IsCurrentlyAnomalous), MissingTexture/CorruptedTexture overrides, AnomalyViewport shared translucent test, m39's bbox in the same join, verify_capture, CaptureBench m49_gates.py + two bench levers (TeleportTargetOffscreenAt, RetakeMaterialAfter), docs. Phase B: the shader drawn-count + cook. m39: FOLD IN (one client-facing schema change, the verifier region should be the drawn box, P-C13 conjunct 2 rides the same campaign). Gates: ONSET, MASK-PICTURE-PAIRING, G7, MASK-TIE unchanged and re-run; OBS-1 inertness (injected == affected on the settled bench, delta 0); OBS-2/OBS-3 can-fail (F4 and F8 shapes); OBS-4 knob sweep; OBS-5 P-C7 v3 (byte-identical except declared keys, frame_indices identical value-for-value on a pose-matched pair) — NOT byte-inert by declaration; OBS-6 verifier; both build targets exit 0; all per-frame gates in both tick orders. Effort ~5-6 sessions + one cook. Bates: schema v2 + the verifier gate + the selection change + the shader ride Bates' delivery too; recommend both hosts get v2 in the same window (chat ruling).

## 4. Open questions for chat
1. F1's route: targeted or auto-pool (log's "mode=" line or nodes[0].component_class).
2. Which artifact the client scrubbed (MP4 or PNGs) and whether the viewer is 0- or 1-based.
3. The unmeasured fork: observable=false when target_pixels=-1 empties affected_frames on a mask-blind host; alternative true|false|null with unmeasured_frame_count (recommended).
4. Keep an event with an empty observable set (recommended) or delete it.
5. T3(i) for all types (recommended) or texture-swap only.
6. Root key name label_schema and whether the README carries a v1->v2 diff.
7. Bates: v2 in the same delivery window, or v1 now.
8. From the client's artifacts: mask.provided for idx 9/44 and run_summary.mask_nopass_discards.

## 5. NEEDS-DECISION
None blocking. Q3 is the one design fork that changes what a mask-blind host receives; the plan is written for the recommended form with the strict rule as a one-line switch.

## 6. Bookkeeping
Journal docs/sessions/2026-09-04-071-concorde-feedback-triage.md, commit 9682b2c on master (path-scoped add, pushed; origin/master = 9682b2c). Worktrees D:\IntrusiveAnomalies\_worktrees_071\master (914f14b) and \bundle-2b6c93f (2b6c93f) were created and REMOVED; the empty parent directory was deleted; git worktree list shows only the main checkout, still on europa-e1 at 1f5e305 with its 8 untracked CHAT-HANDOFF files unstaged. No fetch was run (the first attempt hung); the push completed.
```
api-equivalent cost (not billed on Max): $24.91 | turns: 18 | running total across all briefs: $689.11
