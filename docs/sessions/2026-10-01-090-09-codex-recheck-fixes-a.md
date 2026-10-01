# 090-09 — Codex re-check fixes, round A: installed = what renders now, full revert, Nanite gated per frame and fail-closed, kit 1.2; R4 verified and designed (no R4 code)

Date 2026-10-01 · Code (Opus 5.5), headless · Spec `_reviews/090-08-chat-ruling-codex-recheck.md` (rulings 1–13), findings
`_reviews/090-08-codex-source-recheck.md` (R1–R11 + NOTE) · Heads in: fix `c29e223`, m53 `2015cfa` · Compile-only: no game launch,
no cook, no staging.

## 1. Phase 0 — every finding verified against source before any edit

| Finding | Verdict | The code path that decides it (fix branch at `c29e223` unless noted) |
| --- | --- | --- |
| R1 partial ≠ none | **REAL** | `Anomaly_MissingTexture.cpp:267-280` / `Anomaly_CorruptedTexture.cpp:267-280`: the first live captured slot that is not ours returns `false` for the whole event; m53 `TexCorruptPure::ConditionHeld` likewise requires every slot and binding. |
| R2 dormant override | **REAL** | Revert `:184-191` skips a captured slot whose index is now `>= GetNumMaterials()`; the owner sweep `:231-243` loops only `GetNumMaterials()`; engine `UMeshComponent::SetMaterial` (`MeshComponent.cpp:48-77`) grows `OverrideMaterials` with no slot-count check, so the index-1 override survives a two→one-slot mesh change and resolves again when the mesh changes back. m53's sweep covers `OverrideMaterials.Num()` (the hole does not apply there), but its per-slot loop skips out-of-range slots, so a pre-apply override there was swept to null instead of restored. |
| R3 historical slot set | **REAL** | `:255-280` reads `Slot.Mesh.Get()` (skips invalid, no successor lookup) and `GetMaterial(slot)` only — no registration, visibility, or slot-range test; a hidden original + clean replacement reads installed. |
| R4 pairing | **REAL (async, reachable in delivery defaults) / REAL-BUT-UNREACHABLE in delivery defaults (sync)** | §2. |
| R5 selection-filtered Nanite enumeration, no per-frame check | **REAL** | `AnomalyViewport.cpp:1370-1394` `ActorDrawsAnyNanite` filters with `IsRenderableComponent` (`:778-807`), which applies `MatchesExcludedTargetPattern` and the foliage-owner exclusion; the frame gate (`IsFireLabelledThisFrame`, `SampleDeferredActiveState`) never re-checks Nanite. |
| R6 fail-open | **REAL-BUT-UNREACHABLE in delivery defaults** | `ComponentDrawsNanite` (`:1365-1368`) returns `false` with a null probe and `RefuseNaniteTarget` never consults `HasNaniteComponentProbe`. The probe is set in `FAnomalyCaptureModule::StartupModule` (`AnomalyCaptureModule.cpp:11-16`, both modules `Default` phase); in a Development/Test build with the plugin enabled the capture module is always loaded before any world, so the window is not reached by the shipped configuration. Fixed anyway (ruling 6). |
| R7 kit skips an unjudged run | **REAL** | `label_sync_check.py:1150-1152` `continue`s on a missing onset-reference image; `:1249-1256` / `:1278-1317` compute PASS from surviving runs without comparing them with `runs_of(L)`. Codex's `reinstall_wrong_missing_reference` reproduced: PASS with one edge record. Three neighbours found (zero-byte edge frame, missing interior frame, stuck_low_mip pre-onset frame). |
| R8 AA excuse on an interrupted frame | **REAL** | `transition_gate :897-899` excuses on any AA reason without giving `effect_interrupted` precedence; `interrupt_plus_aa` reproduced (raw end +1 → adjusted 0, PASS). No normal producer of the mixed reason set (Codex's note stands). |
| R10 per-call cost | **REAL** | `AnomalyCaptureSubsystem.cpp:5959-5967` times each call; the summary divides by calls and reports the slowest call; the `ConditionHeld` reads (`:5842`, `:5273`) are untimed. |
| R11 spline misclassified | **REAL** | `AnomalyMeasurability.cpp:25-39` tests the asset's Nanite data on any `UStaticMeshComponent`; engine `USplineMeshComponent::CreateSceneProxy` (`SplineMeshComponent.cpp:640-651`) always builds `FSplineMeshSceneProxy`. |
| NOTE manual re-apply | **REAL, documented unsupported** | `IsAnomalyVisualConditionHeld` keys on the anomaly id; a raw Apply of the same id to another actor can make a retained auto entry read installed. Kept unsupported; now stated in the readme's operator section. |

## 2. R4 — facts and the fix design (no R4 code in this round)

**Delivery-default capture settings.** `bAsyncCapture = true` and `bSveCapture = true` (compiled defaults, `AnomalyCaptureSubsystem.h:442-443`), pacing on (`bPaceCapture = true`, `:419`). `client-delivery.md` deliberately omits `bSveCaptureDefault`; no delivery ini or the office card sets `IAI.Capture.Async` or `IAI.Capture.SVE`. ⇒ **the delivery default is async + SVE.**

**When the async-rectangle fallback fires.** `CaptureCurrentFrame` (`:5141-5145`) takes the async branch when `bAsyncCapture` and a capturer exists; with SVE on, the rectangle is never computed (`bUseSve ||` short-circuits), so **the fallback cannot fire on the default path.** It fires only with `IAI.Capture.SVE 0` (the documented UI-on option, `PRE-DELIVERY-CHECKLIST.md:237`) and `ComputeGameViewportCapture` failing: no game viewport client, Slate not initialised, no viewport widget, no window, or the widget path not visible (`:4231-4263`). With `IAI.Capture.Async 0` every frame is sync. ⇒ **No shipped default reaches the sync path;** the UI-on option reaches it only on a frame whose rectangle cannot be resolved.

**Which label fields come from where (async).**

| Field | Source | Bound to the picture? |
| --- | --- | --- |
| `FireActive`, `FireLabelled`, installed state (`ConditionHeld`), Nanite block, mask values, telemetry | tick-end sample (`SampleDeferredActiveState`, `OnWorldTickEnd` of the armed tick) | frozen at the sample |
| `View` | arm (`CaptureCurrentFrame`, capture Tick) | frozen at arm |
| `FirePos` (actor location) | `FinalizeArmedLabel`, end of the capture Tick | frozen, but before tick end |
| `FireOnScreen` → `labelled` / annotation membership for FireWindow types | `FillAnnotationInputs` at **readback completion**: `ProjectFireBox(F, Snap.View)` projects the **live** actor's bounds | **no** |
| `bbox_px`, `bbox_norm`, `bbox_valid` | `BuildFrameLabelRecord` at completion, `ProjectFireBox` again on the **live** actor | **no** |
| `bbox_drawn_px`, `target_pixels`, `target_drawn_pixels`, the mask PNG | the mask pass rendered for that frame (GPU reduce) | yes |

**Latency between them.** Completion is consumed at the top of a later capture Tick (`ProcessCompletedFrames`, `:966-969`), so the live reads are at least one world tick after the sample by construction. Measured render-side readback latency on banked legs: `M1 readbackLatencyFrames samples=200..300 min=1 max=2 mean=1.015..1.020` (1 frame on 98–99 %, 2 on the rest). ⇒ the live geometry is typically 1, occasionally 2 ticks (33–67 ms at 30 fps) newer than the picture. A target that moves or is destroyed in that interval gets a box/membership for where it is then, not where the picture drew it.

**Sync.** `CaptureCurrentFrame`'s sync block samples the fire state in the capture Tick of frame N, then `ReadPixels` returns the viewport's last presented frame (N−1): engine order `UWorld::Tick` (`LevelTick.cpp:1606` tickables) then `RedrawViewports` (`GameEngine.cpp:1775`, `:1891`). Changes made earlier in tick N by actors or other tickables (a host material swap, a move, a raw revert) are in the label but not in the picture.

**Design — async (freeze at the sample).**
- At the tick-end sample, per fire also freeze `GetActorRenderableBounds(Actor)` (one `FBox`) and a valid flag (`TArray<FBox> FireBoundsFrozen; TArray<uint8> FireBoundsValid` in `FCaptureSnapshot`), and move `FirePos` capture to the same sample.
- At completion, `FillAnnotationInputs` and `BuildFrameLabelRecord` project the frozen box with the frozen `View` (`ProjectBoxToNormalizedRect`, as `ProjectActorBoundsToScreenRect` does today) instead of reading the live actor; a fire whose actor was gone at the sample is off screen, a fire destroyed after the sample keeps its frozen box.
- Event identity is already frozen (`FAutoLiveFireInfo` copied into the snapshot).
- Cost: one bounds union per fire per captured frame (the same work `ProjectActorBoundsToScreenRect` does now, moved earlier) and ~28 bytes per fire per pending snapshot (pending depth ≤ 2–3). Code ≈ 60–90 lines (capture + label writer + one exported viewport helper), plus a pure test of the frozen-vs-live selection.
- **Estimate: ≈ 2–3 h** including the selftest and both builds.

**Design — sync (one-deep history, else flag).**
- Preferred: keep a one-deep "last tick-end sample" during a run (the same arrays the async snapshot freezes, plus frozen bounds and view), refreshed at every `OnWorldTickEnd` while running. The sync block builds `SyncFrame`'s label inputs from that entry, which describes the world the presented picture N−1 was rendered from. If the entry is missing or not from the immediately previous tick (`GFrameCounter` gap ≠ 1: first frame, a hitch, a level transition), the frame is written with a new reason `capture_unpaired` (unlabelled, unmasked, dropped) and counted in `run_summary.capture_unpaired_frames`.
- Cost: one array copy per tick while running (a few hundred bytes). Code ≈ 120–160 lines; the transition state machines (`StepHideTransitions`) must run on the history entry, which is the delicate part.
- Fallback (if the history proves fragile in 090-10): flag every sync frame `capture_unpaired` and declare `IAI.Capture.Async 0` and the UI-on backbuffer fallback unsupported for delivery. ≈ 30–40 lines.
- **Estimate: history ≈ 4–5 h; flag-only ≈ 1 h.** Either way the readme marks the sync path as not delivery-grade until 090-10's `Async 0` leg passes.

## 3. The fixes as built

Each rule below is as built; "tests both ways" means a passing selftest on the fix and a mutant that undoes the fix and must fail.

| Finding | Rule as built | Where | Tests (pass) / mutants (must fail) |
| --- | --- | --- | --- |
| **R1** partial ≠ none | `GetVisualConditionState()` → none / full / partial by how many targeted slots render ours; none ⇒ `effect_interrupted`; partial ⇒ labelled, counted per frame in `run_summary.label_effect_partial_frames` | `AnomalyInstallState.h` (`Classify`, `ClassifySlots`); `Anomaly_MissingTexture/CorruptedTexture.cpp`; m53 `Anomaly_TexCorrupt.cpp`; `AnomalyLabelWriter.cpp` `FrameHasPartialLabelledEntry` | `install_state_selftest` R1 cases (one of two slots replaced ⇒ partial and an annotation member; the old rule shown to read none); `m52_window_selftest` R1 cases / mutant `r1_any_foreign_slot_is_none` fails |
| **R3** current render state | a slot counts only if the component (or same-named successor) is valid, `IsRegistered()`, `ShouldRender()`, the slot index is `< GetNumMaterials()`, and `GetMaterial(slot)` is ours (m53: our host MID and its RT bindings read back); re-evaluated at every captured frame's sample point | `AnomalyInstall::SlotRendersOurs`; the three anomalies' `GetVisualConditionState` | hidden original + replacement ⇒ none (old rule: full); unregistered ⇒ none; mesh swapped to one slot ⇒ partial / mutants `r3_ignore_render_state`, `r3_ignore_slot_range` fail |
| **R2** dormant override | revert restores or clears every captured index whatever the current slot count (reads the raw override past the mesh's range); sweep `max(GetNumMaterials(), OverrideMaterials.Num())` over every touched and resolved component; post-revert assertion counts overrides of ours at any index (`REVERT-RESIDUAL`, Error; `out-of-range=` / `residual=` on the revert line). m53: per-slot restore no longer skips out-of-range slots (our MID there is restored to the raw original, not swept to null) + the same residual assertion | `AnomalyInstall::DecideRevertSlot`, `SweepExtent`, `CountOwnedOverrides`; `Revert()` in both texture anomalies; m53 `RestoreAndRelease` | two slots → one slot → revert → two slots: residual 0 and the asset materials return (old revert: residual 1 and our material returns); explicit pre-apply override at the out-of-range index restored; host-retaken slot left alone / mutants `r2_sweep_current_slots_only`, `r2_never_restore` fail |
| **R5** enumeration + per-frame | `ActorDrawsAnyNanite` counts every visible geometry primitive (static/skinned, non-empty ISM) and ignores name-pattern and foliage-owner exclusions; every captured frame (both capture paths) reads `ActorBlocksLabelForNanite` per fire; a blocked fire is never labelled or masked (`IsFireLabelledThisFrame`, `IsAnnotationMemberGated`), its entry is transition-only with `nanite_unmaskable` (bit 64); one raw revert per event at the top of the next capture Tick (`NANITE-MIDEVENT` lines, `nanite_midevent_reverts`) | `AnomalyTargetPolicy::CountDrawnNanite`, `NaniteBlocksLabel`; `AnomalyLabelSync::IsAnnotationMemberGated`, `DecideNaniteEntry`; capture `IsFireNaniteBlocked`, `NoteNaniteBlocked`, `ServicePendingNaniteReverts`; `AnomalyLabelWriter::MarkNaniteUnmaskable`; verify_capture `--label-rule` knows the reason (unlabelled-only, object types) | excluded/foliage Nanite part counts; hidden part made visible blocks; no policy labels a blocked frame; revert requested once / mutants `r5_enumeration_uses_selection_exclusions`, `r5_nanite_ignored_in_membership`, `r5_nanite_never_flagged` fail; label-rule 4 new cases (2 clean, 2 must-fail) |
| **R6** fail closed | `DecideNaniteTarget(allow, probe, drawn, nanite)`: setting 0 with no probe ⇒ `RefuseProbeMissing` (`REFUSED-NANITE-PROBE-MISSING`, Error; `run_summary.refused_nanite_probe_missing`; summary Error line); setting 1 admits; the per-frame gate also blocks when the probe is missing; m53 E7N refuses when the probe is missing | `AnomalyTargetPolicy.h`; `AnomalyViewport::RefuseNaniteTarget`, `ActorBlocksLabelForNanite`; m53 `TexCorruptTree.cpp` E7N | `TestR6FailClosed` / mutant `r6_fail_open` fails |
| **R7** kit unjudged | kit 1.2: every labelled run must be judged; an unreadable onset/end reference or frame next to a run makes it unjudged; a judged FAIL outranks UNJUDGED; else UNJUDGED (never PASS); READ BACK `| nanite N | unjudged N` | `tools/label_sync_check.py` (`event_verdict`, `analyse_d`, `measure52`) | selftest 39 → 54 on both decoders incl. Codex's two repros (UNJUDGED) and a fail-beside-unjudged case (FAIL); regression 8 real sessions / 91 events / 0 differences / mutants A, A2, B, C fail |
| **R8** no AA excuse | `NO_AA_EXCUSE = (effect_interrupted, nanite_unmaskable)` blocks the onset and tail AA excuse whatever else the frame carries | `transition_gate` | Codex's `interrupt_plus_aa` now FAILs with end +1 / mutant B fails |
| **R10** per-frame cost | installed and Nanite readings are evaluated once per fire inside a sample window and the window's summed cycles are one captured frame's cost; `Capture(F1): EFFECT-INSTALLED COST PER CAPTURED FRAME frames= mean= max= (threshold mean<=0.05 max<=0.5: PASS/OVER)` plus the Nanite recheck's own mean/max | capture `OpenConditionWindow` / `CloseConditionWindow`, `GetFireInstallState` | runtime only — declared for 090-10 (§4) |
| **R11** spline | `RouteIsNanite(...)` excludes `USplineMeshComponent` (UE 5.1 `FSplineMeshSceneProxy`); `IsKnownUnmeasurable` inherits it — local, 13 lines + tests | `AnomalyMeasurability.cpp`, `AnomalyTargetPolicy.h` | `TestR11SplineRoute` incl. a classification→selection KAT (a lone spline-on-Nanite-asset candidate fires: 3 draws; under the old classification it took the no-candidate path) / mutant `r11_spline_is_nanite` fails |
| **R9 + NOTE + one-frame gap** | readme: Shipping wording ("the plugin's modules are not built for Shipping", descriptor and content remain); `effect_interrupted` = "the effect may be fully or partly gone: drop, never use as negatives"; R1 partial semantics; `nanite_unmaskable` row and meaning; release-rule paragraph distinguishes AA excuses from interruption frames; Nanite paragraph qualified (all-Nanite vs mixed, exclusion-pattern parts counted, mid-event revert, logging per site, fail-closed, spline, masks need their capture settings, setting 1 restores admission only); early-revert proof "unit-tested; capture proof pending"; run_summary keys; operator section: manual `IAI.Apply`/`IAI.Revert` of a type the capture is firing is unsupported. m53: uv/normal in the interruption row; its Labels paragraph reconciled. Office card (`tools/OFFICE-CHECK.md`): `nanite`, `unjudged`, and the censored one-frame-gap limit | `docs/client-readme.md`, `tools/OFFICE-CHECK.md` | text |

Carried forward: 090-05's five mutants (`f1_ignore_installed`, `f1_never_flag`, three Nanite-setting mutants) still fail. Totals:
`install_state_selftest` 27/0 (new), `target_policy` 41/0 (was 25), `m52_window` 305/0 (was 296), `camera_clipping` 154/0;
15 C++ mutants fail; verify_capture `--label-rule` 31 cases; Python suites fix 15/15, m53 23/23 incl. kit 54/54.
m53 C++: texcorrupt pure 251/0, draw KAT 987/0, exclusion 244/0, active-source 71/0 (its pinned membership strings
updated to `IsAnnotationMemberGated` / `FireNaniteBlocked`), 23/23 m53 mutants fail.

## 4. Declared in-engine / pixel proof for 090-10 (not provable here)

None of these can be shown on a static bench without a lever; each row is a leg 090-10 adds to the 090-06 harness, on the NEW
exe (this round's archive) and, where the old behaviour is wrong, on the OLD exe as a can-fail.

| # | Leg | Lever (exists / needed) | Prediction on NEW | OLD-exe can-fail |
| --- | --- | --- | --- | --- |
| 1 | **R1** partial replace: `missing_texture` and `corrupted_texture` on a multi-slot static mesh (e.g. `SM_Ramp3`, the m8 multi-slot target) | **needed**: a one-slot variant of `IAI.Bench.RetakeMaterialAfter` (retake slot 0 only) | frames after the retake stay labelled, in `annotation.json`, masked; `label_effect_partial_frames` > 0; `effect_interrupted` 0; kit judges the end at the scheduled revert, 0/0 | OLD flags those frames `effect_interrupted` while the other slot still shows checker/pink ⇒ kit FAIL "interrupted frames show the effect" |
| 2 | **R3** host hides the target mid-event | **needed**: `IAI.Bench.HideTargetMeshAt <si>` (`SetVisibility(false)` on the target's mesh components, materials untouched) | from `si`: unlabelled `effect_interrupted`, no mask; kit reads the interruption picture clean, PASS 0/0 | OLD keeps labelling (records still say ours) while nothing is drawn ⇒ kit FAIL "labelled not visible" |
| 3 | **R3 + R2** host changes the mesh mid-event: two-slot target → one-slot mesh at `si`, revert at the scheduled end, back to the two-slot mesh after the event | **needed**: `IAI.Bench.SwapTargetMeshAt <si> <mesh>` + a swap-back step | after the swap: partial (labelled); revert line `out-of-range=1 residual=0`; after the swap back the asset materials show, no checker/pink, no label | OLD: after the swap back the checker/pink returns on slot 1 with no event and no label (pixels positive, labels negative) |
| 4 | **R5** a Nanite part appears mid-event (MainWorld hosts Nanite meshes) | **needed**: `IAI.Bench.ShowNanitePartAt <si>` (make a hidden Nanite mesh component on the target visible, or attach one) | from `si`: entries `nanite_unmaskable`, no labelled or masked frame; next tick `NANITE-MIDEVENT REVERTED`; `nanite_midevent_reverts` 1; kit READ BACK `nanite 1`, end censored | OLD keeps labelling with no mask for the Nanite part (`target_pixels` short of the box; the label says nothing is wrong) |
| 5 | **R5** enumeration: a mixed actor whose Nanite component matches an exclusion pattern | `IAI.SetExcludedTargets <pattern>` (exists) | the actor is refused (`REFUSED-NANITE … site=auto_pool|targeted`) | OLD admits and labels it |
| 6 | **R10** F1 cost | none (every NEW leg) | `Capture(F1): EFFECT-INSTALLED COST PER CAPTURED FRAME … PASS` (mean ≤ 0.05 ms, max ≤ 0.5 ms) | n/a |
| 7 | **m53 R1** partial: `uv_corruption` / `normal_corruption` on a multi-slot fixture object | **exists**: m53's foreign-replace lever replaces the FIRST committed slot | labelled partial, `texcorrupt.condition_held` false, `label_effect_partial_frames` > 0 | OLD (`97D292F8`) flags `effect_interrupted` while the other slots still show the corruption |
| 8 | **R4 async** (090-10 code): a target crossing the frustum edge, and a target destroyed mid-event | `IAI.Bench.TeleportTargetOffscreenAt` (exists); **needed** `IAI.Bench.DestroyTargetAt <si>` | the frame armed before the move/destroy keeps its box and membership from the sample | OLD drops the label (or moves the box) on the last frame(s) the picture still shows |
| 9 | **R4 sync** (090-10 code): the F1 raw-revert leg under `IAI.Capture.Async 0` | `IAI.Bench.RawRevertAt` (exists) | labels pair with the picture's own tick (history), or the frames read `capture_unpaired` | OLD labels the revert frame one picture early |

Not provable by a leg: **R6** (needs the capture module unloaded while the injector runs; covered by `TestR6FailClosed` + mutant)
and **R11** unless the fixture has a spline mesh on a Nanite-enabled asset (StackOBot's `BP_Cable`/`BP_Spline` assets were not
checked for Nanite data this round).

## 5. Builds, scans, audits, archives

| | Fix branch | m53 |
| --- | --- | --- |
| Heads built | `1aa26fb` then `4fb6509` | `3f2c223` then `dcdb95b` (host `_r53_host` detached at `dcdb95b`) |
| Memory gate | waited for ≥ 6 GB free (2.4 → 6.6 GB; a foreign UE 5.7 editor held 8–11 GB) | 11.6 GB free at launch |
| Normal build | editor 13 actions / 1,219 s (compiler starved behind the foreign editor), game 6 / 140 s, 0 warnings, 0 errors; exe `F7526356` | editor 13 / 89 s, game 6 / 119 s, 0 warnings, 0 errors; exe `E6098412` |
| Strict-include | run 1 **FAIL**: one unique error in both targets — `AnomalyLabelWriter.cpp(806)` used `AnomalyInstall::` without its header (the unity build hid it); fixed in `4fb6509`; run 2 **PASS 0 errors / 0 warnings** both targets, restore OK | **PASS 0 / 0** both targets, restore OK |
| Final exe (post-strict normal relink) | **`EBB70E9B`** (242,186,752 B) | **`8131479A`** (242,508,800 B) |
| UTF-16 string scan vs the 090-07 archive | 63 differences (exe + 2 DLLs), every one a 090-09 string (new/changed log lines, run_summary keys, rule name; the em-dash splits some log strings into fragments) plus two known one-byte adjacency artefacts (`pUAnomalyInjectorSubsystem`, `pUsage: %s <frames|default>`); full list `strscan_fix_final.txt` in the archive | 79: the fix branch's strings + m53's three new TexCorrupt revert strings + one-byte `p`-prefix artefacts (`pIAI.Bench.HideOmitDepthPassSilencing`, `pUsage: IAI.Apply…`, `pUsage: IAI.TestVisibility…`, `pTexelSizeU`, and their mirror images) |
| Lever audit (source + exe + 4 editor DLLs) | 34/34 PASS | 47/47 PASS |
| C++ suites | install_state 27/0, target_policy 41/0, m52_window 305/0, camera_clipping 154/0; 15/15 mutants fail | the same four + texcorrupt pure 251/0, draw KAT 987/0, exclusion 244/0, active-source 71/0; 15/15 + 23/23 mutants fail |
| Python suites | 15/15 (verify_capture incl. `--label-rule` 31 cases, kit 54/54, kit mutants) | 22/22 before the kit merge; kit 54/54 after it (§6) |
| Archive (6/6 re-hashed at the destination) | `_binary_baselines\m52fix-09009-EBB70E9B\` | `_binary_baselines\m53-09009-8131479A\` |

Not run, by the brief: no game launch, no cook, no staging; `_reviews\090-06-*` untouched (090-10 re-pins it).

## 6. Commits

**Fix branch `fix/m52-label-timing`:** `1aa26fb` (source, tests, readme), `4fb6509` (strict-include: the missing include),
`37bf185` (kit 1.2 + office card), then this docs commit (journal, status block, architecture note, gotchas G432–G435).
**m53 `feat/m53-uv-normal-corruption`:** `3f2c223` (merge of `1aa26fb` + the m53 side: TexCorrupt state / revert / residual,
E7N fail-closed, source-selftest pins, m53 readme; conflicts resolved by keeping m53's `CapturedTelemetry` and adding
`FireNaniteBlocked` after it), `dcdb95b` (merge of `4fb6509`), then the merge of `37bf185` + this docs commit and the m53
status block. m53 kit selftest after that merge: 54/54. `m51`, `master`, tags, `ToCodex\`, `E:\AmmaYT` and
`_reviews\090-06-*` untouched. Gotcha numbers G432–G435 taken past G431, the max over all 31 refs.

## 7. NEEDS-DECISION / notes for chat

For chat's acceptance (none blocks; each is a choice made inside the ruling's words):

1. **Kit precedence: a judged FAIL outranks UNJUDGED.** Ruling 7 says an event with any unjudged run "reads unjudged". Taken
   literally, deleting one frame image of a second run would turn an observed mistimed first run into "unjudged", which counts in
   no release verdict — an observed failure erased by a missing file. Kit 1.2 therefore reads FAIL when a judged run fails (its
   unjudged runs still listed in the detail), and UNJUDGED otherwise; never PASS either way.
2. **R7 applied to every image a run needs, not only Codex's `continue`.** Three neighbours of the same silent PASS were found and
   closed (unreadable edge frame, missing interior frame, stuck_low_mip pre-onset frame). A frame *absent* right beside an edge stays
   CENSORED, as before.
3. **Rule name bumped to `…_v3_effect_rendered_nanite_gated`** because `labelled` changed meaning (partial now labelled; installed
   read from render state; Nanite gate). No consumer keys on the v2 string except the kit's synthetic-session builder.
4. **m53 `texcorrupt.condition_held` telemetry is unchanged (every slot held)**, so on a partial frame it reads false while the frame
   is labelled; the readme says so.
5. **E7N fail-closed reuses the `nanite_unmaskable` census reason** (no new census reason, so the census line format and its checker
   are unchanged); the `REFUSED-NANITE-PROBE-MISSING` count comes from the admission backstop, which runs first on a real apply.
6. **verify_capture `--label-rule`:** the `nanite_unmaskable` clean case uses `lod_popping`, not `blinking` (blinking's ActorHidden
   listing rules need hide frames the synthetic rows do not model; the reason check itself is type-independent).
7. **Known kit limits carried (not new):** a missing `labels.jsonl` row inside a run is still skipped silently; the two decoders can
   still disagree on a truncated PNG on the stuck_low_mip path.
8. **The NOTE (manual re-apply)** stays documented-unsupported (readme §5, operator section), per ruling 12.
