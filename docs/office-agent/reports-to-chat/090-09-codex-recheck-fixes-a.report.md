# REPORT for 090-09-codex-recheck-fixes-a.md
verdict: GREEN (declared) - a NEEDS-DECISION or RED report HOLDS the next brief until it carries '# ACK-VERDICT: ...'
brief: 090-09-codex-recheck-fixes-a.md | cwd: D:\IntrusiveAnomalies\StackOBot\Plugins\AnomalyInjector | session: <uuid> | model: claude-opus-5-5 | effort: xhigh
started: 2026-10-01 07:42:57 | finished: 2026-10-01 09:39:50 | duration: 117 min | exit code: 0
context at last turn: 770465 tokens (77% of 1M) - roll the session at 70%
live feed: D:\IntrusiveAnomalies\_mailbox\to_chat\090-09-codex-recheck-fixes-a.live.txt

---- FINAL MESSAGE (Claude Code) ----
VERDICT: GREEN

**For the owner:** I made both branches' labels follow what is actually on screen. If the game swaps a material, hides the object or changes its mesh mid-event, the label now notices, and a partial swap stays labelled. Reverting no longer leaves stray checker or pink material behind. Objects that start drawing Nanite mid-event are dropped and reverted. The office check no longer passes an event it couldn't fully read. Everything compiled cleanly and was pushed; no game was launched. The label/picture timing fix (R4) is designed and goes into 090-10.

**Phase 0** (all verified against source; fix branch at `c29e223`)
- **R1 REAL:** one foreign slot made the whole event negative (`Anomaly_MissingTexture.cpp:267-280`, the corrupted_texture twin, and m53's `ConditionHeld`).
- **R2 REAL:** revert skipped indices at or past `GetNumMaterials()` (`:184-191`) and the sweep was bounded by it (`:231-243`). The engine's `SetMaterial` grows the override array regardless of slot count. On m53, its sweep already covered the whole array, but out-of-range slots were cleared rather than restored.
- **R3 REAL:** the check read the stored component pointers only (`:255-280`), with no registered/visible/slot-range test.
- **R4:** async path REAL and reachable on the delivery default; sync path REAL but unreachable from any shipped default (details below).
- **R5 REAL:** `ActorDrawsAnyNanite` filtered through `IsRenderableComponent`, so target-name and foliage exclusions hid Nanite parts. Nothing re-checked Nanite per frame.
- **R6 REAL-BUT-UNREACHABLE** in delivery defaults: with no probe, "Nanite" read false, but the capture module always registers the probe at startup in Development/Test. Fixed anyway.
- **R7 REAL:** `label_sync_check.py:1150-1152` skipped the run; PASS was computed from the surviving runs. Codex's repro gave PASS with one edge record. The kit agent found three more ways to the same silent PASS.
- **R8 REAL:** `transition_gate :897-899` granted the AA excuse when any AA reason was present.
- **R10 REAL:** the cost was timed per call, and the `ConditionHeld` reads weren't timed.
- **R11 REAL:** the spline check missed that `USplineMeshComponent` always draws through its own non-Nanite proxy (`SplineMeshComponent.cpp:640-651`).
- **NOTE REAL:** manual re-apply stays documented as unsupported.

**R4: facts, design, estimate**
- **Delivery defaults:** Async on, SVE on, Pace on (compiled defaults). No ini key and nothing on the office card changes them.
- **When the async-rectangle fallback fires:** only with `IAI.Capture.SVE 0` (the UI-on option), and only on a frame where the game-viewport rectangle can't be found (no viewport client, no window, or the widget path not visible). With SVE on it never fires. `Async 0` makes every frame sync. No shipped default reaches the sync path.
- **Frozen at the tick-end sample:** active, labelled, installed, Nanite block, mask values. The view is frozen when the capture is requested, and the actor position at the end of the capture tick.
- **Bound to the picture:** the mask PNG, `bbox_drawn_px` and target pixel counts (read back from the GPU for that frame).
- **Read from the live actor when the readback completes:** on-screen membership (so `labelled` and annotation membership for the four texture types) and `bbox_px` / `bbox_norm` / `bbox_valid`.
- **Latency:** completion is handled in a later tick, so the live reads are at least one tick late by construction. Banked legs measured readback latency of 1 frame on 98–99 % and 2 on the rest, so live geometry is 1–2 ticks (33–67 ms) newer than the picture.
- **Sync:** the state is read mid-tick N while the picture is the already-presented frame N−1.
- **Async design:** freeze the actor's bounds box and a valid flag at the tick-end sample; at completion, project that frozen box with the frozen view. About 60–90 lines, **≈ 2–3 h**.
- **Sync design:** keep a one-deep copy of the last tick-end sample; build sync labels from it, and mark a frame `capture_unpaired` (dropped) when that copy isn't from the previous tick. About 120–160 lines, **≈ 4–5 h**. The fallback (flag every sync frame and declare the path unsupported for delivery) is about 1 h.

**Each fix as built; every one has passing tests and a mutant that undoes it and fails**
- **R1 + R3**
  - **Rule:** each anomaly now reports none / full / partial for its targeted slots. A slot counts only if its component (or a same-named replacement) is registered and rendering, the slot exists on the current mesh, and it resolves to our material (m53: our corrupted material with its texture copies still bound). None → `effect_interrupted`. Partial → still labelled, counted in `label_effect_partial_frames`.
  - **Where:** the shared logic is in the new `AnomalyInstallState.h`, used by the three texture anomalies. It is evaluated once per fire inside a sample window at tick end (and in the sync capture block), and that one reading feeds the mask, m26 and label decisions.
  - **Tests:** `install_state_selftest` 27/0, including cases where the old rule is shown to disagree. Mutants: any-foreign-slot-is-none, ignore-render-state, ignore-slot-range.
- **R2**
  - **Rule:** revert restores or clears every index written at apply, even past the current slot count. The sweep covers every stored override, and a post-revert assertion (`REVERT-RESIDUAL`, Error) checks none of ours remain. m53 restores instead of nulling, and has the same assertion.
  - **Tests:** the two-slot → one-slot → revert → two-slot case leaves 0 residual (the old revert leaves 1, and our material comes back). Mutants: sweep-current-slots-only, never-restore.
- **R5**
  - **Rule:** every visible drawn primitive counts, ignoring selection exclusions. Every captured frame, a fire whose target draws Nanite is never labelled or masked; its entry carries the new reason `nanite_unmaskable` and the effect is reverted at the next tick (`nanite_midevent_reverts`). `verify_capture --label-rule` knows the new reason.
  - **Tests:** target_policy 41/0 and m52_window 305/0. Three mutants, plus four new label-rule cases.
- **R6:** setting 0 with no probe refuses every target as `nanite_probe_missing` (Error line, `run_summary.refused_nanite_probe_missing`). m53's decision step E7N also fails closed. Mutant `r6_fail_open` fails.
- **R7**
  - **Rule:** kit 1.2. An event with a run that couldn't be judged is UNJUDGED and never PASS. READ BACK ends `| nanite N | unjudged N`.
  - **Tests:** selftest 39 → 54 on both decoders, including Codex's two repros; kit mutants A, A2 and C fail; regression on 8 real sessions shows 0 differences.
- **R8:** an `effect_interrupted` or `nanite_unmaskable` frame never gets the AA excuse. Codex's `interrupt_plus_aa` now FAILs with end +1; mutant B fails.
- **R10:** a new log line reports cost **per captured frame** (mean and max, PASS/OVER against 0.05 / 0.5 ms), plus the Nanite re-check's own mean and max. This is runtime-only, so it's on the 090-10 list.
- **R11:** local (13 lines). Spline meshes are no longer classified as Nanite. A selection test shows a lone spline candidate on a Nanite asset now fires; mutant fails.
- **R9 + NOTE + one-frame gap:** readme corrected per the ruling, including the operator-section note that manual Apply/Revert of a type the capture is firing is unsupported. m53's sections are reconciled. The office card gains lines for `nanite`, `unjudged`, and the one-frame gap reading `censored`, not failed.
- **Carried tests:** 090-05's five mutants still fail (15/15 in total). m53's own suites: 6/6 base, 23/23 mutants. Its source-text selftest pinned the old membership call, so I updated its expected strings.

**Declared for 090-10's smoke** (lever, prediction on the new exe, and where the old exe should fail)

| Leg | Lever | New exe | Old exe can-fail |
| --- | --- | --- | --- |
| R1 partial replace on a multi-slot mesh | needed: one-slot variant of `RetakeMaterialAfter` | stays labelled, partial count > 0, kit 0/0 | flags interrupted while the effect shows → kit FAIL |
| R3 target hidden mid-event | needed: `HideTargetMeshAt` | `effect_interrupted`, kit PASS | keeps labelling a hidden object → FAIL |
| R3 + R2 mesh swapped to one slot, then back after the event | needed: `SwapTargetMeshAt` | partial while swapped; revert reports residual 0; clean after swap-back | checker/pink returns unlabelled |
| R5 Nanite part appears mid-event | needed: `ShowNanitePartAt` | `nanite_unmaskable` from that frame, revert next tick, kit `nanite 1` | keeps labelling with no mask |
| R5 excluded Nanite part | `SetExcludedTargets` (exists) | target refused | target admitted |
| m53 partial | m53's foreign-replace lever (exists) | stays labelled | flags interrupted |
| R4 async | `TeleportTargetOffscreenAt` (exists); needed: `DestroyTargetAt` | frame keeps its sampled box | label dropped or moved |
| R4 sync | `RawRevertAt` under `Async 0` | paired or `capture_unpaired` | label one picture early |
| R10 cost | every leg | cost line reads PASS | — |

R6 can't be shown in-engine (it needs the capture module unloaded while the injector runs), and R11 only if the fixture has a spline mesh on a Nanite asset.

**Builds**

| | Fix branch | m53 |
| --- | --- | --- |
| Normal build | 0 warnings | 0 warnings |
| Strict-include | run 1 FAIL (a missing include the unity build hid; fixed in `4fb6509`), run 2 0/0 | 0/0 |
| Final exe | `EBB70E9B` | `8131479A` |
| String scan vs 090-07 archive | 63 differences, all explained | 79, all explained |
| Lever audit | 34/34 | 47/47 |
| Archive (6/6 re-hashed) | `m52fix-09009-EBB70E9B` | `m53-09009-8131479A` |
| Python suites | 15/15 | 22/22, kit 54/54 after the final merge |

Every string difference is this round's own log lines, run_summary keys and rule name, plus known one-byte adjacency artefacts.

**Commits pushed**
- `fix/m52-label-timing` `c29e223..189286a`: `1aa26fb` (source), `4fb6509` (include), `37bf185` (kit 1.2), `189286a` (journal `docs/sessions/2026-10-01-090-09-codex-recheck-fixes-a.md`, status block, architecture note, gotchas G432–G435).
- `feat/m53-uv-normal-corruption` `2015cfa..31078c7`: merges `3f2c223`, `dcdb95b`, `31078c7`; the last adds only docs and the kit, so `8131479A` still represents the head's code.
- `m51`, `master`, tags, `ToCodex\`, `E:\<personal-folder>` and `_reviews\090-06-*` untouched.

**NEEDS-DECISION:** none. For chat's acceptance:
1. **Kit: an observed FAIL outranks UNJUDGED.** Ruling 7 literally says such an event "reads unjudged". Taken literally, one missing frame image in a second run would hide a mistimed first run, so I let FAIL win; it is never PASS either way.
2. **Rule name bumped to `…_v3_effect_rendered_nanite_gated`**, because `labelled` changed meaning.
3. **m53's `texcorrupt.condition_held` telemetry still means "every slot held"**, so it reads false on a labelled partial frame. The readme says so.
4. **E7N's probe-missing refusal reuses the `nanite_unmaskable` census reason**, so the census format is unchanged.
5. **Two kit limits remain, both older than this round:** a missing labels row inside a run is still skipped silently, and the two image decoders can still disagree on a truncated PNG.

Several claude.ai connectors (Gmail, Calendar and some plugin servers) need authorising in their connector settings, and the pdf-viewer server failed to connect; none was needed here.
api-equivalent cost (not billed on Max): $57.58 | turns: 226 | running total across all briefs: $4,119.76
