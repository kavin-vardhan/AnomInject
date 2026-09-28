# 084-07c — label-sync fixes, round 4: Codex N1–N8, unit tests both ways, rebuild, re-cook, Codex re-check v2 (2026-09-29)

Brief: `_mailbox` 084-07c (Code, Opus 5.5, xhigh). Spec: `_reviews\084-07b-chat-ruling-codex-recheck.md` (N1–N8 rulings and
the loop bound); findings and evidence: `_reviews\084-07-codex-sync-recheck.md`. Branch `fix/m52-label-timing` from `46e5894`,
edited in place in the warm scratch host `E:\IA_BuildCache\_r84_host`. Source commit **`3d6f702`**. ⛔ **No game launch, no
staging, nothing that took focus.** Evidence: `_reviews\084-07c-evidence\`; selftest/build logs `E:\IA_BuildCache\_r84_selftest\`.

## 1. N1–N8: where each lives, and the test both ways

| # | ruling | where it lives (3d6f702) | tested both ways |
|---|---|---|---|
| N1 | `labelled` from ONE authority = the rule that builds `annotation.json`'s frame list, per policy class | `AnomalyLabelSync::IsAnnotationMember` / `IsEntryLabelled`; `AnomalyLabel::ProjectFireBox` / `IsFireInAnnotation` / `IsSnapshotEntryLabelled`; `ResolveAnnotationPolicy` + `FillAnnotationInputs` (snapshot `FirePolicy`, `FireOnScreen`); the annotation list built from the same per-frame bit (`MemberByIndex`) | `TestAnnotationMembership`: 4 classes, each list equals the historical annotation rule, each row's `labelled` equals its list; the activity-bit variant fails `fire_window`, the event-live variant fails the other three, the game-thread-state variant fails `render_held_window`. `--label-rule`: the FF41BFF3 shape fails `LABELLED-MISSING`, the fixed shape (box off screen ⇒ unlisted, unlabelled) passes |
| N2 | a candidate becomes negative only on valid traces; a degenerate clipped segment uses the full slab segment; too few valid traces ⇒ `camera_clipping_unconfirmed` | `PrepareConfirmSegment`, `ECandidateOutcome::TooFewValidTraces`, `ConfirmSlab` (`AnomalyNearClipSlab.h`); engine-faithful trace stub (no trace at ≤ 1e-4 cm) | the zero-thickness wall (half-extent 0) is a confirmed positive; the 084-07 rule is shown to clip all 144 rays to zero length (unflagged miss); the degenerate-only case is flagged, never a miss; a 1-ray candidate with the footprint grid off is flagged, with it a real miss |
| N3 | the renderer's matrix product | `FAffine34` / `MulAffine` / `MakeMatrixBox`; the ISM loop uses `PerInstanceSMData[i].Transform * GetComponentTransform().ToMatrixWithScale()` (`InstancedStaticMesh.cpp:2665,2669`) | Codex's case from the engine formula: X [50,250], encloses every rendered corner, reaches the slab; the composed `FTransform` gives X [140,160], misses corners, rejects. 45° shear: matrix box encloses, TRS box does not. Orthogonal edges: exact volume and centre |
| N4 | known-full / known-partial / unresolved; unresolved never sets a boundary; reason `unresolved`; invalid endpoint ⇒ unresolved | `ClassifyLevel` (invalid endpoint ⇒ `Unknown`), `EHeldSet` / `ClassifyHeldSet`, `ReasonUnresolved` (5th bit), `R.HeldSet` → `EntryTransition` | held + unknown ⇒ unresolved (084-07: partial); endpoint 0 ⇒ unresolved (084-07: at-held, intermediate unflagged); an unresolved frame keeps the next partial frame on the onset edge (084-07: the unknown frame became the full boundary); known held + known baseline + unknown ⇒ partial (record-backed) |
| N5 | exact run-length ranges, no cap | `FSIRange` / `AddToRangeList` / `CountRangeListWithin` / `TPartialEdgeTrack<TList>` (UE holds a `TArray`) and the range-listed PARTIAL-SET report | 45 partial frames in 2 ranges, every frame identifiable; the 084-07 32-slot track names 32 of 40 with 8 bare overflow; out-of-order merge; 100 separated frames ⇒ 100 ranges |
| N6 | carried tails carry again | `ECarrySource` / `DecideRunEndCarry`; `CarryLabelSyncAcrossRun` finds the source from the trail **or** `CarriedTailFires`; the zero-frame early return is removed | two boundaries: A flags 1, B (1 frame) flags 1, C flags the remaining 6 = the 8-frame window; the 084-07 policy leaves C with 0. A zero-frame middle run passes the history through (C flags 7); 084-07 dropped it |
| N7 | verify every identity the value was ever applied to, kept until verified; test the production path | `GAppliedValues` (per component, a mask of every plugin value applied) recorded in `TagActor`, kept until that value's retirement verifies; `RetireStencilValue` builds holders from tracked ∪ applied ∪ former-owner and calls the pure `RetireHolders` | through `RetireHolders` itself: Codex's case (prior == V, custom depth off, moved away) fails verification and quarantines; the 084-07 verification passes it and the value aliases. A holder restored earlier and moved: new path checks it, 084-07 never did. Ordinary retirement, unrelated holders, destroyed holders |
| N8 | full snapshot, or a marker | **the marker**: a row written without per-entry activity (only `IAI.Capture.Shot`) carries `label_rule: "legacy_shot"`; readme §8.6; `verify_capture.py --label-rule` reads it as `LEGACY_SHOT` (old meaning, said so), refuses a file mixing shot rows with run rows | shot row read under the old meaning; a shot row with a wrong `visible_positive` fails; a shot file without `annotation.json` is read; mixing refused |

**Why the marker for N8:** a single shot has no render record (a `stuck_low_mip` frame's membership needs the async
render-truth readback, which a one-shot never arms) and no transition history, so a "fully sampled snapshot" would still
write the game-thread mirror for `stuck_low_mip` and no transition flags — a third semantics. Marking it is the honest,
proportionate route; no shot row carries the old `visible_positive` meaning unmarked.

**N1 detail.** The snapshot carries `FirePolicy` (FireWindow / ActorHidden / AnomalyState / RenderHeldWindow — a
render-truth fire is RenderHeldWindow) and `FireOnScreen` (the shared `ProjectFireBox`: whole frame for whole-frame and
target-less fires, else the projected actor bounds). `FillAnnotationInputs` fills both in the async drain immediately before
the row is built and in the sync path (whose snapshot now also carries its projection view), so the row, the reason
counter and the accumulator read one pair of bits taken at one instant on one view. The annotation's `injected_frames` is
now built from the per-frame membership bit the same function computed (`MemberByIndex`), for every class; for FireWindow it
equals the old `AffectedFrames` and for the others the old `FireActive` subset. `label_active_unlabelled_entries` uses the
same function. `run_summary.label_labelled_rule = "annotation_membership_per_policy_v1"`.

**No banked session has the FF41BFF3 shape.** 0 of 2,586 `labels.jsonl` files under `_bench_sessions_bank` carry a
`labelled` key (the newest is the 084-06 bench on `E9FF019A`); `FF41BFF3` was never run. The shape exists only as
`--label-rule`'s synthetic `FF41BFF3_shape_texture_labelled_false_FAILS` case.

**N4 detail — one design call made here:** a member frame whose record reads every texture at baseline (a settle-tail
frame) is **known not-held**: it neither sets the full-set boundary nor gets `unresolved` (it is known, not undecidable). A
member frame with no texture record at all is unresolved. Flagged for chat below.

## 2. Tests

- `tools/m52_window_selftest.cpp`: **262 checks, 0 failures** (was 220). `tools/camera_clipping_slab_selftest.cpp`:
  **128 checks, 0 failures** (was 118). Both built `/W4 /WX` clean.
- **Mutation check (product-level "both ways"):** `mutate_08407c.py` copies the headers, re-introduces each old behaviour
  (N1 fire_window reads the activity bit; N4 invalid endpoint ⇒ at-held; N4 unknown counts short; N5 32 cap; N6 tail not
  re-carried / zero-frame drop; N7 verify only tracked-after-restore or owned; N2 degenerate traced as-is; N2 too-few ⇒ miss;
  N3 box from edge lengths) — **all 9 mutants make the selftests fail** (`mutation-08407c.txt`).
  🔻 **Corrected after Codex v2:** the N3 mutant replaces the half-extents with an undersized length, it is **not** a
  replay of the old composed-`FTransform` behaviour. N3's both-ways evidence is the explicit selftest comparison (matrix
  box vs composed-TRS box), not this mutant.
- `verify_capture.py --label-rule --selftest`: **24 cases OK** (was 15; new: texture box off screen, FF41BFF3 shape,
  `unresolved` placement ×3, `LEGACY_SHOT` ×3, shot/run mix refused). `--label-pixel-gate --selftest` 98 and
  `--change-oracle --selftest` 35: output **identical** to the committed tool (0 diff lines); bare `--selftest` black-frame
  lines identical; `test_verify_capture_change_oracle` + `_consistency` 44 tests OK.

## 3. Build

- First pass: editor 13 actions / 65.7 s, game 6 / 45.9 s, exe `45387A44`. **A44 FAILED on it**: the retired rule string
  `view_slab_bounds_then_triangle_confirm_v2` was still in a run-summary log line (the `run_summary` key had been bumped, the
  log literal had not). Fixed (one token), rebuilt: editor 4 / 26.4 s, game 3 / 37.4 s. **0 compiler warnings in all four
  logs.** `45387A44` superseded, not archived.
- **Final exe `16053B80`** (242,115,072 B). A44 both encodings: 14 new strings present, `..._v2` absent, 5 controls present.
  Comment stripper 0 changed of 120. Archived `_binary_baselines\m52fix-16053B80\` (hash-verified at the archive; logs,
  `a44.txt`, selftest and mutation outputs, README).

## 4. Re-cook (as 084-07b; predeclared in `cook-predeclare.md`)

- Temporary junctions `Content` and `Plugins\RoomGenerator\Content` → the D: project, removed after
  (`[IO.Directory]::Delete(j, $false)`); D: content still 2,174 + 8 files. **Content manifest (2,182 files) identical before
  and after (`E4C7F38D…`), and identical to 084-07b's.**
- Command: 084-07b's, with the archive dir changed to `E:\IA_BuildCache\_r84_cookout_c1` (`cook-c1-command.txt`). **48 s wall, commandlet 7.9 s, 794
  packages, 0 warnings, 0 errors, exit 0.**
- **All five predictions met:** exe `16053B80` byte-identical to the build; **cooked `AnomalyInjector.uplugin` byte-identical
  to the branch's (`9EFB9B49…`)** — `AnomalyBench` first, `TargetAllowList [Editor, Game]`, `TargetConfigurationDenyList
  [Shipping]`, then `AnomalyShaders` (`PostConfigInit`), `AnomalyInjector`, `AnomalyCapture`, `AnomalyControlServer`;
  `verify_cooked_maps.ps1` PASS (`CB_GateLevel`/`MainMenu`/`MainWorld`/`Entry`; inverted probe: `CB_TexCorruptLevel` missing);
  content unchanged; globals `462B8AC6`/`BB05CF99` unchanged.
- **Pairing: exe `16053B80` + utoc `59C20959` / ucas `0EC2587E` / pak `26FFC026` + `462B8AC6` / `BB05CF99`**, archived
  `_binary_baselines\m52fix-cook-59C20959\` (6/6 re-hashed at the destination). **Not staged** — BenchGate still holds the m53
  set (`staged-set-before/after.txt` identical).
- ⚠ **Observation, not predicted:** against the 084-07b cook, 3 of 1,972 container entries differ — the two `ShaderArchive`
  tables (same size) and `MainWorld.umap` (+50 B) — with byte-identical source content and a code-only change. Cause not
  established (G120); 082-06d's identical-input cook series is the relevant prior and was not re-run.

## 5. Client readme (§8.6–§8.7)

§8.6: new row field `label_rule` (shot rows only); `labelled` now states its per-anomaly rule and the run_summary rule
name; `transition_reason` lists `unresolved`. §8.6a: an `unresolved` row in the reasons table and the "what to do" table;
`partial` is written only when the record proves it; the 32-frame listing cap and the "unknown ⇒ not yet held" limit are gone
(exact ranges, `unresolved`); carries pass through short and zero-frame captures; `_unresolved_entries`. §8.6b: the
too-few-valid-traces flag; flat objects traced along the whole slab; instances placed by the renderer's matrix product; the
two fixed misses removed from "what it can miss"; rule `v3` and the new counters. §8.7: `unresolved` in the stuck_low_mip row
and the any-AA list.

## 6. Codex re-check v2 — PARTIAL, no blocking finding established (collected, not acted on)

GDP Relay `runs\2026-09-29-084-07c-sync-recheck2\`, `gpt-6-astra` at `max` (requested == rollout), **1,119 s**, exit 0,
frozen `source-3d6f702` extract (14/14 files match). Collected byte-identical under a one-line header to
`_reviews\084-07c-codex-sync-recheck2.md`; ledger row appended. It replayed the selftests (262/0, 128/0), the verifier
selftest (24 OK) and the bank scan (2,586 files, 0 with `labelled`) independently.

| item | Codex | one line |
|---|---|---|
| N1 | **RESOLVED** | one per-policy authority, both writers and the annotation consume it; the accepted m26-veto exception is the only row/annotation difference left |
| N2 | PARTIAL | the length fallback and the valid-trace minimum are right (1e-3 sound); **NEW-1 (medium):** `GetBodyInstance()` defaults to the WELD PARENT's body and the hit's component is not checked, so the full-slab fallback can confirm a welded parent's face for a planar child |
| N3 | PARTIAL, **HIGH remains** | the matrix box is right and encloses every affine case (16,040 corners); but confirmation traces `InstanceBodies[i]`, which UE builds with the SAME `FTransform` composition (`InstancedStaticMesh.cpp:2514–2524`), so the original rotated/non-uniform case becomes an unflagged Miss at the trace stage. Not exercised: all 20 banked camera summaries read `instances_tested_total` 0 |
| N4 | PARTIAL | classification, boundary and settle-tail NotHeld are right; **NEW-2 (medium), a defect of this round:** a force-created render result (`FlushObserveQueue`) default-constructs `HeldSet = 0 = NotHeld`, so a forced-unknown member frame gets no `unresolved` reason and telemetry `held_set: not_held`; the verifier does not catch it (synthetic probe) |
| N5 | **RESOLVED** | exact ranges, complete reporting |
| N6 | **RESOLVED** | carried tails re-carry; zero-frame runs pass through |
| N7 | PARTIAL | the reported omission is fixed; **remaining medium variant:** V applied and restored, then W applied (its saved prior is V), then V retired — verification checks current values only, passes, and W's later restore writes V back (alias if the host re-enables custom depth). The run-bounded quarantine when the host's prior equals V is judged the intended conservative behaviour, not a defect |
| N8 | **RESOLVED** | the marker is a permitted, reasonable choice; no unmarked old-semantics writer remains |

Codex also flagged that this journal's N3 mutant is not a replay of the old behaviour (corrected in §2). Under the loop
bound, Codex states the remaining HIGH is on the ISM confirmation sub-path the banks never exercised and the mediums are
source counterexamples, so **no blocking finding is established; chat decides document vs fix.**

**Readme corrected to match the source after Codex v2 (documentation, not a fix):** §8.6b no longer says a rotated
instance under non-uniform scale "is still found" — it is selected, then traced against a collision body the engine
places with the composed transform, so it can still read "not clipped" (listed under "what it can miss"); the welded-parent
over-report is listed; §8.6a states that a forced-unknown frame carries no `unresolved` reason in this build; §8.7's
mask-value paragraph names the N7 conditional exception.

## 7. Deviations, limits, NEEDS-DECISION

- **NEEDS-DECISION (chat), from Codex v2 — none acted on:** (1) N3's remaining HIGH: confirm ISM candidates against
  renderer-matched geometry, or flag them unconfirmed when the instance body's composed transform cannot represent the
  rendered one; (2) NEW-2: default an unclassified held set to `Unresolved` and classify the forced / late-receipt path —
  **a defect introduced by this round** (`FRenderEventResult::HeldSet = 0`); (3) NEW-1: welded candidates (body identity);
  (4) N7's pending-prior variant. Each is a small source change + rebuild + re-cook if chat rules a fix.
- **N4 settle-tail call** (§1): all-baseline members are known not-held, not unresolved (Codex v2 agrees).
- **N7 can quarantine a value for the rest of a run** when a component's own pre-plugin value equals it (Codex's "quarantine
  when the value remains"); by design, bounded to the run (EndRun resets).
- **The "an off frame backed only by `unresolved` is a FAIL" gate rule is for the 084-08 harness** (it needs the pixel
  measurement); nothing here implements it. `verify_capture.py --label-rule` only checks where `unresolved` may appear.
- **Not in N1–N8, untouched:** the stale `_events_over_three` / "MORE THAN 3" diagnostics; the predicted-endpoint limit
  (documented); collision-vs-render equivalence for camera_clipping (documented); a run cancelled before focus still does not
  carry label-sync history (it never reaches the carry).
- The container difference in §4.

## 8. Hand-off

- **Chat:** rule on Codex v2 (§6) under the loop bound (only a HIGH finding on a bench path, or one in the data, blocks).
- **085-01 (m53 DC plan):** its integration dry run uses this head (`3d6f702` + docs).
- **084-08 (harness):** run `verify_capture.py --label-rule` on every leg; read `unresolved`, `stuck_mip_unresolved_*`,
  `label_labelled_rule`, the camera `too_few_valid` / `full_slab_fallback` counters; treat an off frame backed only by
  `unresolved` as a FAIL to investigate.
