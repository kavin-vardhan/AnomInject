# 093-02 — Claude Code review of the Codex natural-foundation plan

Written 2026-10-13 by Claude Code (brief 093-02). Read-only.

- **Plan reviewed:** `_reviews\093-02-codex-natural-foundation-plan.md` (relay run `_relay\runs\2026-10-13-093-02-natural-foundation-plan\`, `gpt-6-astra` / `max`, 2,741 s, exit 0, 570 lines).
- **Measured against:** `_reviews\093-01-ruling.md` (R1–R11), `093-02-request.md`, the release worktree `E:\IA_BuildCache\_r917\AnomalyInjector` (`d7eee97`, clean), git objects in the plugin repository, UE 5.1.1 at `D:\UESource\UnrealEngine`.
- **What I ran (no build, cook, launch, commit or ref change):**
  - A new detached scratch worktree `E:\IA_BuildCache\_r932\AnomalyInjector`, used only for `git apply --check` runs and one working-tree-only replay; restored clean each time.
  - `git apply --check` of the 091-06 WIP patch against `1eaf205`, `5d6352b`, `d7eee97`, and against `5d6352b` plus the release delta.
  - Read-only legacy `git merge-tree d660b5d d7eee97 5d6352b` (writes nothing).
  - `git diff-tree --cc` on every merge commit between `d660b5d` and `5d6352b`.
  - Source reads to check the plan's file/line and API claims.

## Counts

Ruling table: **14 rows — 9 OK · 4 GAP · 1 CONFLICT.** Further findings (gates, examples, jobs) are in §4–§6.

## 1. Rulings R1–R11

| Ruling | Where in the plan | Status | Note |
|---|---|---|---|
| R1 pilot order | §F1 J1–J6 foundation, J7 MISS-1, J8–J9 MISS-2; §G last para | OK | MISS-1 is J7, after the foundation jobs; it is still the first production adapter and qualifies the foundation ("foundation now qualified with calibration"). NRM-3/BLUR-1/GONE-1 not scheduled. |
| R2 `injection_mode`, type ids unchanged | §B5 table; §B6; §D | OK | `missing_texture` stays the id for checker, MISS-1 and MISS-2. Absent key = legacy, never inferred natural. |
| R2 shipped cause metadata | §B7 | **CONFLICT** | LOD-1, NRM-1 (green_flip only) and GONE-3 (mode 0 only) match. **CAM-1 is given `cause_family: content_error`; R2 says CAM-1 is "config: near plane set too large".** The plan's family vocabulary (`content_error`, `code_defect`, from 093-01 Q8) has no config value. Small, but it is the ruling's own wording. |
| R2 UI shape is the owner's | §B6 shapes A/B | OK | Two shapes offered; natural not default until proof. |
| R3 module policy | §G "Dependencies" | OK | No new module. I checked the APIs the MISS probes need are exported or inline: `FUniformExpressionSet::GetTextureValue` (`MaterialShared.h:607`, `ENGINE_API`), `GetTextureParameter`/`GetNumTextures` (inline, `:603-605`), `FMaterial::GetUniformExpressions` (`:2023`, `ENGINE_API`), `GetMaterialNoFallback` (virtual), `UMaterialInstanceDynamic` (class `ENGINE_API`, `MaterialInstanceDynamic.h:12`; `CopyMaterialUniformParameters` public at `:140`). |
| R4 evidence contract | §B3, §B4, §E2 G-MECHANISM/G-MASK/G-PICTURE | OK | Three legs, RT state keyed to the request, no per-draw claim, no engine instrumentation. |
| R5 GONE-1 reference mask | §B3 "GONE-1 later" | OK (deferred) | Out of 093-02 scope; `mask_measurement.method` reserved, RD order and FOV/fade corrections carried. The proposed IoU ≥ 0.995 / ≤ 0.5 % threshold is **unjustified** — R5 wants it justified; owed by the GONE-1 plan, not this one. |
| R6 BLUR-1 restore | §B3 "BLUR-1 later" | OK (deferred) | Bounded `StreamIn`, every visible user labelled. The per-event user-cap statement is owed by the BLUR-1 plan. |
| R7 attempt outcomes | §B1, §B4, §B5 counters | **GAP** | The outcome partition and counters match R7 exactly. **But the plan picks exclusion as the only route for ambiguous/unknown/unqualified frames and rejects `transition` in one phrase ("clean-looking orphan transition") without costing the trade-off.** See risk 1: exclusion cuts frames out of the video right next to every natural onset and offset. R7 allows "excluded **or** transition"; the transition route keeps the timeline continuous and uses an existing schema mechanism. |
| R8 label keys v1 | §B5 | **GAP** | Keys are the R8 set, additive, no meaning change; `target_drawn_pixels` never takes companion pixels; metadata never gates. Two presentation problems: (a) the **controlled MISS-1 record is placed in root `natural_causes[]`** (R8 scopes it to causes that emitted effects; R2 says MISS-1 is controlled), so a client reading the container name will read MISS-1 as natural; (b) MISS-1 gets **`cause_family: code_defect`**, which reads as a defect claim next to R1's "never described as a shader failure". The plan flags (a) for chat; (b) it defends as "catalog grouping". |
| R9 corrections | §C (PIE usage flags), §B3 (GONE FOV/fade), §D/§G (FlatNormal, VT pink, reduced-mip copy, menu) | OK | All carried. |
| R10 parked | §G last para | OK | MISS-1b, BLUR-2, BLUR-4, GONE-2 not scheduled. |
| R11 base | §A2 | **GAP** | The plan's route works but is not the best one, and its stated reason against merging is wrong (§3). |
| R11 identity / F1 caveat | §A3, §F2 checkpoint I | **GAP** | Recommends (a) with office readings, as R11 requires. The assessment of the WIP is correct, but it misses a new fail-closed path the WIP adds, and its can-fail depends on deleted artifacts (§2). |

## 2. F1: my checks of the plan's claims about the saved 091-06 WIP

**Patch applicability (my runs):**

| Target | `git apply --check` |
|---|---|
| `1eaf205` (its base) | **exit 0**; 9 files, +190/−32; the untracked `AnomalyCaptureFamily.h` does not collide |
| `5d6352b` (proxy head) | **exit 0** |
| `5d6352b` + the release's `d660b5d..d7eee97` Source/tools delta (replayed in scratch, working tree only) | release delta applies cleanly, defaults read 0/0/0, **WIP then applies with exit 0** |
| release `d7eee97` | **fails**: `AnomalyCaptureOwnership.h` does not exist there (the release predates the F1 guard); 3-way leaves conflicts in 6 files |

So the plan's caution ("port its intent … rather than assuming an unreviewed patch application") is stricter than needed: on any base containing `5d6352b` it applies as-is, then needs completing.

**Claims I confirm** (from the patch text and the 091-06 run folder):
- It re-resolves the owner at start, every tick and every world-tick-end (`RefreshCaptureOwner`). That fixes the one outage 091-06 reproduced (owner bound to a null viewport at capture start and never rebound: "912 sampled requests were dropped and no images were written", 091-06 `events.jsonl`). The office cause was never established; 091-06 was stopped at 10:53 with no `office-reads.txt`.
- With the owner target unknown, any family on the owner scene that passes the game-view test is accepted, so two game views in one scene both pass. The owner picks the first local player whose viewport client has a viewport, with no uniqueness check.
- The warning text tells the reader to "Read capture_ownership in run_summary.json", **but nothing serializes it**: the patch touches no label writer, and the start-time flags (`bCaptureOwnerSceneKnownStart`, `CaptureOwnerUpdates`, `bCaptureUnpairedByDesign`) are set and never emitted. `tools/capture_ownership_selftest.cpp` is unchanged and does not test the new fallback. Not a finished fix.

**What the plan misses:**
1. **The WIP adds a new rejection even when the owner target IS known.** `RejectReason` returns `NotGameView` unless every view has the `Game` show flag, `bIsGameView`, `PlayerIndex >= 0` and is not a capture. In UE 5.1:
   - `PlayerIndex` is the local player's `ControllerId` (`LocalPlayer.cpp:800`).
   - `bIsGameView` is set from the world only when the view is built on the game thread (`SceneView.cpp:837-842`).

   So the "robust" build can fail closed in a new way (a host with a negative controller id, or views built off the game thread) where `5d6352b` would have accepted. The plan's I-failure form does carry `not_game_view`, so an office zero would be explained. But §A3's risk list should name it, and J2 should prove it on both hosts.
2. **The G-OWNER can-fail inputs no longer exist.**
   - 091-06's reproduction harness (`reproduce.py`, `old_matrix.py`, `pie_driver.py`, a `Review0916` bench module) was written under `E:\IA_BuildCache\_r916\`, which the 2026-10-10 cleanup deleted. Only the source patch was saved.
   - The 091-05 archives, including proxy exe **`3ADC2EA4`** and the pre-F1 control **`C7FB5212`**, lived in `E:\IA_BuildCache\_r915\archives\` (091-05 `report.md` lines 281, 332-337), also deleted. `_binary_baselines` now holds 4 entries and neither hash.
   - J2 must therefore re-create the delayed-viewport control **and** rebuild a guard-bearing binary. The J1 candidate (guard present, WIP absent) is the natural can-fail, under `G201` (new hash, same source).
   - For the pre-F1 controls, the shipped release `218BA4C7` (in `_binary_baselines\release-2026-10-05-23098fa-218BA4C7`) has the same capture code as `d660b5d` and can stand in for `C7FB5212`.
   - The plan's "Archived `3ADC2EA4`" and "Old `d660b5d` … controls" should be reworded accordingly.

## 3. Base: the merge route the plan did not evaluate

The plan proposes `d7eee97` plus a cherry-pick of six non-merge commits (`6415b62`, `a15b24d`, `4c82ee2`, `45eba38`, `03e7eeb`, `97c20f2`), re-resolving conflicts by hand. Its argument against bringing in `5d6352b` is that "the raw `d7eee97..5d6352b` diff reverses those defaults to **3/40/1**".

**That is a two-dot-diff artifact.**
- `5d6352b` never changed the defaults: `git diff --stat d660b5d 5d6352b -- AnomalyLabelSync.h` is **6 insertions, 0 deletions** (`DecideScopeExit` only). The 3/40/1 values appear only because `5d6352b` lacks `23098fa`.
- A real 3-way merge (`git merge-tree d660b5d d7eee97 5d6352b`, read-only) gives **4 changed-in-both files and exactly one conflict, in `CLAUDE.md`**.
  - `AnomalyCaptureSubsystem.cpp`: 0 markers.
  - `AnomalyLabelSync.h`: only the six added lines, so **0/0/0 is kept**.
  - `docs/client-readme.md`: merges cleanly.
- The same result reproduces as a replay: the release delta applies cleanly onto `5d6352b`, defaults read 0/0/0, and the F1 WIP applies cleanly on top (§2).

Facts on the plan's route:
- The six commits are the complete set of non-merge implementation commits in `d660b5d..5d6352b`; the other two are docs (`1eaf205`, `4c93e5f`).
- Of the proxy/m53 merges, only `f54ff87` carries hand-resolved Source content: 2 adjacent declaration lines in `Anomaly_TexCorrupt.h` (`UpdatePartScope` / `LastRefusal`). `cc74af0`, `839f429`, `a8d7b2b`, `4f0ac50`, `33adf06`, `23a7da5` and `dc42e35` have none.
- So cherry-picking is workable. But it produces a tree that was **never proved**, while a merge reproduces the proved `5d6352b` Source exactly, plus the release's two commits.

**Suggested for chat:** base = a branch from `d7eee97` with `5d6352b` merged in (resolve the one `CLAUDE.md` conflict), then the completed F1 WIP. The re-proof list in §A2 stays the same.

## 4. Gates

- **Can-fail per gate:** present on all 15 rows of §E2. Two can-fails need re-sourcing: G-OWNER and G-BASE "old `d660b5d`" (§2, point 2).
- **PIE and staged:** required in every job and in §E1. ✓
- **Bench + office-like fixture:** StackOBot plus the retained Lyra worktree (`caa68c6`, present) with a 15,000-actor census leg. ✓ The <100 ms bound is the release checklist's own (`PRE-DELIVERY-CHECKLIST.md:41`).
- **Circularity:**
  - The home checks are independent: `natural_attempt_check.py` reads the saved PNGs/masks; the plan forbids "falsify only the telemetry field the checker reads"; it keeps a bounded diagnostic source window so excluded samples can still be judged.
  - **One near-circular spot: office checkpoint N.** Its `max_abs_onset_error` / `max_abs_offset_error` and `unflagged_changed_samples` are only meaningful if the excluded source samples are available. Otherwise the published frames start at the first accepted frame by construction and the edge error reads 0 trivially. The plan makes the diagnostic window optional at the office ("if enabled locally"). **It should be mandatory for the N readings.**
- **m26:** stays on. A pilot needs a nonzero same-key mask before publication. A post-hoc m26 removal of a qualified pilot event fails reconciliation. ✓
- **No-effect control:** already-default MISS-1 and already-`DefaultTexture` MISS-2, plus a count-on-apply mutant. ✓
- **Exact restore:** G-RM covers raw null, array length, tails and host takeover, with picture recovery. ✓

## 5. Label keys and the examples

- Additive only; legacy meanings preserved in §B5's "Legacy semantics" paragraph. ✓
- Every legacy key used in the examples exists in the release writer:
  - `label_schema_minor` (`AnomalyLabelWriter.cpp:1461`), `transition_frames` / `transition_reasons` (`:1495-1496`);
  - `event_id` in the form `type@start_frame|target` (`:735-738`).
  - (The readme §8.2 table does not list these three annotation keys; that is a readme gap, not a plan error.)
- `anomaly_subtype`, camera/engine/object nodes, `t`, `t_wall` and `view` are omitted. The plan declares the examples excerpts.
- **Minor inconsistency:** both examples are `run_epoch 7` / `run7:causeN` (one session), yet both events carry `mask_value 200`. In one session an event's mask value is held for the whole run (m50: `EventClaimed` is never released mid-run; `mask_map.json` maps value → event), so two events cannot share 200.
- The R2 mismatch (CAM-1 family) and the MISS-1 presentation points are in §1.
- **Observation for chat, not a plan defect:** blinking's hide mode defaults to **1** (`AnomalyHiddenClass.cpp:32`). Mode 0 is reachable only through the bench lever `IAI.Bench.HideMode` ("never in a client payload", `AnomalyInjectorSubsystem.cpp:1519-1540`). So R2's GONE-3 tag can never appear in client data.

## 6. Job split

- Serial, one implementer, one bench, one brief at a time. ✓ Continuations allowed without weakening the bar. ✓
- **Fit risk:**
  - **J5** (publication barrier: separating source and published indices across the writer, m55, the mask map, the video and the checkers) is the largest core change in the plan, and it touches every shipped anomaly's output path.
  - **J11** (full supported-anomaly regression, both pilots on both fixtures, PIE and staged, both tick orders, AA-edge legs) is more than one 3–6 h session by the evidence of 091-05/091-07, which needed a full session for a smaller regression.
  - **J2** now also has to rebuild its can-fail harness and binary (§2).
  - **J4/J8/J9** need bench-fixture cooks.
  - Expect continuations; the 40–62 h total looks optimistic.
- **J10** (UI) needs the owner's A/B choice before it starts. The dashboard repo named by the plan is correct: `D:\IntrusiveAnomalies\anomaly-dashboard` = AnomDash origin, `7ea2e51`.
- **Office load:**
  - Checkpoint I is 4 cases per host, a 26-integer row each.
  - Checkpoint N is 4 cases per host at 1,800 samples, a 31-integer row each.
  - Across two hosts that is about 450 hand-typed numbers. A fixed PASS/FAIL line plus a check digit from the reader would cut transcription risk.

## 7. Top 5 risks

1. **Exclusion-only publication cuts frames next to every natural onset and offset.**
   - Ambiguous, unknown and "applied but not yet visible" samples are removed from `Actual_Frames` and the video (§B4).
   - The published clip then has jump cuts placed exactly at anomaly edges. A detector can learn the cut instead of the anomaly.
   - It also breaks the readme's promise that the stamped `video.fps` plays at natural speed.
   - The transition-flag alternative that R7 allows is not costed.
2. **F1 is still unproven where it failed.** The office cause was never identified, and the WIP both widens acceptance (scene + game-view fallback) and adds a new rejection (`NotGameView`, §2). Checkpoint I is the only real proof, and its can-fail must be rebuilt from scratch.
3. **J5 regression surface.** Changing how published indices are made affects every shipped anomaly's labels, masks, `change_evidence` and the verify tools. The "existing all-controlled path is preserved" promise needs its own byte/field-identity gate on a controlled-only run, before and after J5. The gate matrix has G-SCHEMA but no controlled-only identity leg.
4. **MISS-2 yield on real content.** It needs an effective ordinary sRGB colour parameter. It skips VT bindings, needs sibling-MID cloning for host MIDs (`CopyMaterialUniformParameters` caveats are acknowledged) and needs whole-component section coverage. On the VT-heavy host, checkpoint N's `aimed_successes >= 3` may not be reachable. The plan correctly makes zero eligibility a limitation, not a pass, but then the pilot does not qualify there.
5. **Client-facing wording.** These can mislead a client reading the files before the owner's notice:
   - MISS-1 as `code_defect` inside `natural_causes[]`;
   - CAM-1's family not matching the ruling;
   - a GONE-3 tag that is inert in client builds.

## 8. Where the plan is wrong, with evidence

| Plan says | Evidence |
|---|---|
| Bringing in `5d6352b` would reverse the defaults to 3/40/1 (§A2) | `5d6352b` changes `AnomalyLabelSync.h` by +6/−0 since `d660b5d`. 3-way merge-tree: one conflict (`CLAUDE.md`); merged defaults stay 0/0/0 (§3). |
| Port the WIP's intent; do not assume the patch applies (§A2) | The patch applies cleanly to `5d6352b` and to `5d6352b` + release delta (§2). |
| G-OWNER can-fail: "Archived `3ADC2EA4` delayed-viewport control" (§E2) | Archive and harness were deleted with `_r915` / `_r916` (cleanup README 2026-10-10; 091-05 report :281, :332-337; 091-06 file_change list). |
| CAM-1 `cause_family: content_error` (§B7) | Ruling R2 table: CAM-1 = "config". |
| Examples: two events in run 7 both `mask_value 200` (§B5) | One event's value is held for the whole session (m50). |
| WIP assessment lists only the widened fallback (§A3) | The WIP also adds `NotGameView`, applied with a known target (`AnomalyCaptureOwnership.h` hunk; `LocalPlayer.cpp:800`, `SceneView.cpp:837-842`). |

## 9. For chat and the owner

**Chat decisions:**
1. Base route: merge `5d6352b` (verified, one docs conflict) or cherry-pick six commits.
2. Exclusion vs transition for unresolved natural samples. If exclusion, how the jump-cut and video-speed effects are handled.
3. MISS-1's container and family (`natural_causes[]` / `code_defect`).
4. CAM-1's family value: add a `config` family, or rule `content_error` acceptable.
5. J2 can-fail sourcing given the deleted archives (rebuilt guard binary under `G201`; release `218BA4C7` as the pre-F1 control).
6. Make the office diagnostic source window mandatory for checkpoint N.

**Owner questions:**
1. UI shape A (mode beside each pool row) or B (details panel).
2. Accept shorter clips when frames are excluded.
3. Which hosts the office checkpoints cover. The 2026-10-10 cleanup note records that the second host's M3 will not ship.
4. Timing of the client notice about the new keys.

**Housekeeping:**
- Scratch worktree `E:\IA_BuildCache\_r932\AnomalyInjector` (detached `1eaf205`, clean) was created for these checks and is removed at the end of this brief.
- No other repository state changed.
