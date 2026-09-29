# 084-08 — the four 084-07c fixes (part A: build + re-cook) and the 084-09 label-sync harness (part B) (2026-09-29)

Brief: `_mailbox` 084-08 (Code, Opus 5.5, xhigh). Spec: `_reviews\084-07c-chat-ruling-round4.md` (part A),
`_reviews\084-06-chat-ruling-bench.md` rulings 1 (as replaced by `084-07-chat-ruling-source-round3.md` decisions 1–2), 2, 4, 5
and 6, and `084-07b-chat-ruling-codex-recheck.md` N1/N4. Branch `fix/m52-label-timing` from `11317a4`, edited in place in
the warm scratch host `E:\IA_BuildCache\_r84_host`. ⛔ **No game launch, no staging, nothing that took focus.** Evidence
`_reviews\084-08` part A: `_binary_baselines\m52fix-8A6074AA\`, `…\m52fix-cook-4BF63FB8\`, `_reviews\084-08-evidence\`;
part B: `_reviews\084-09-*` and `_reviews\084-09-evidence\`.

## 1. Part A — the four fixes (source `4bf0915`)

| # | ruling | where it lives | tested both ways |
|---|---|---|---|
| A1 | N3 HIGH remainder: an ISM instance whose collision body (placed by UE with the composed `FTransform`, `InstancedStaticMesh.cpp:2514`) differs from the rendered matrix product beyond ε ⇒ `camera_clipping_unconfirmed`, no trace | `AnomalyNearClipSlab::AffineMatchesRendered` (relative 1e-4 on each basis row and the origin), `FConfirmBodyFacts::bInstanceBodyMatchesRendered`, `ClassifyConfirmBody` → `EConfirmBody::InstanceTransform`; `ResolveConfirmTarget` builds both matrices with UE types | Codex's case from the engine formulas (instance 90° about Z, component scale (10,1,1)): mismatch ⇒ flagged, 0 traces; the 084-07c path traced the composed body (X [140,160]) and read the truly clipped instance as an UNFLAGGED MISS; controls: uniform scale, rotation about the scaled axis, unrotated instance under a rotated component all match and are traced; ε: 1e-6 matches, a 1.7 % off-axis term does not |
| A2 | NEW-2: a forced-unknown held set defaults to `unresolved` | `AnomalyStuckMipWindow::FHeldSetState` (`Set = Unresolved`, `bFromRecord`), `ClassifyHeldSetState`, `ForceHeldSetUnknown`, `IsPartialMemberFrame` / `IsUnresolvedMemberFrame`; `FRenderEventResult::Held` replaces `uint8 HeldSet = 0`; the force path calls `ForceHeldSetUnknown` | a force-created result reads unresolved, carries the `unresolved` reason with AA off, is counted unresolved and sets no full boundary, telemetry `held_set: unresolved`; the 084-07c default read `not_held` with no reason; a record-backed classification survives forcing; an unbacked classification is reset |
| A3 | NEW-1: welded candidates ⇒ `camera_clipping_unconfirmed` | `FConfirmBodyFacts::bWelded` = `P->IsWelded()` or the body's `WeldParent` or a non-empty `GetCurrentWeldInfo()` (a weld parent carrying children's shapes); `EConfirmBody::Welded` | Codex's welded case (planar child at X=50 with a hole, parent face at X=80): flagged, 0 traces; the 084-07c path traced the parent body through the full-slab fallback and CONFIRMED a positive the child does not render; the unwelded child traced against its own body is a correct negative |
| A4 | N7 saved-prior variant: tripwire | `AnomalyLabelSync::CheckPriorRestore` / `IsTagValueFree` / `FRecycleCandidate::bQuarantined`; `AnomalyStencilTag::SetRestoreWatch` (set by `FAnomalyMaskMeasure::BeginRun`, cleared at `EndRun`); `NotePriorRestore` in `RestoreActor`, `RestoreComponentsCarrying` and retirement; `FAnomalyStencilTagLedger::Quarantined` | Codex's sequence (host prior 210, custom depth off; apply 210, restore; apply 211; retire 210; restore 211's saved prior): the tripwire fires on both restores, 210 is quarantined, the recycler never reissues it; without it 210 is recycled and the later restore writes it back (the alias). Census-claimed values count too; host values (0) and 255 never |

- The body classification is ONE pure function production calls, so the selftest exercises the decision, not a model of it.
- `run_summary`: `camera_clipping_label_rule` **v4**, `camera_clipping_confirm_welded_unconfirmable`,
  `_instance_transform_unconfirmable`, `mask_prior_collision`, `mask_prior_collision_quarantined`; a run-summary log line
  lists the per-cause unconfirmed counts; the mask pool summary logs the tripwire total.
- **Tests:** `m52_window_selftest` **282 / 0** (was 262), `camera_clipping_slab_selftest` **154 / 0** (was 128), both `/W4 /WX`.
  **Mutation check** (`_r84_selftest\mutate_0848a.py`, `mutation-0848a.txt`): the nine 084-07c mutants plus seven new ones
  (A1: no instance-transform check / comparator always matches; A2: default not_held / force keeps an unbacked
  classification; A3: welded traced; A4: no tripwire / recycler ignores quarantine) — **all 16 fail the selftests**. One A2
  mutant first survived (equivalent under the existing inputs); a direct invariant check was added and it now fails.
- **Build:** editor 13 actions / 70.2 s, game 6 / 47.3 s, exit 0, **0 warnings**, first attempt. Comment stripper 0 changed of
  120. **Exe `8A6074AA`** (242,127,360 B); A44 both encodings: 9 new strings present, the `_v3` rule string absent, 7
  controls present. Archived `_binary_baselines\m52fix-8A6074AA\` (hash-verified at the archive).
- **Re-cook** (predeclared `_reviews\084-08-evidence\cook-predeclare.md`, recipe as 084-07c with archive dir
  `_r84_cookout_d1`): 46 s wall, 794 packages, 0 warnings, exit 0. **All five predictions met:** exe `8A6074AA` byte-identical;
  cooked `AnomalyInjector.uplugin` byte-identical to the branch (`9EFB9B49…`, `AnomalyBench` first, Shipping-denied); map gate
  PASS (4/4, `CB_TexCorruptLevel` inverted probe MISSING); **content manifest (2,182 files) identical before and after
  (`E4C7F38D…`)**; globals `462B8AC6` / `BB05CF99` unchanged. **Pairing exe `8A6074AA` + utoc `4BF63FB8` / ucas `4142CC1E` /
  pak `26FFC026`**, archived `_binary_baselines\m52fix-cook-4BF63FB8\` (6/6 re-hashed). Against 084-07c's cook the utoc/ucas
  differ (ucas −4,096 B) with the pak and globals identical — the same class as 084-07c's observation, not investigated.
  **Not staged** (the m53 set stayed staged; receipts before/after identical).

## 2. Part B — the 084-09 harness (`_reviews\084-09-*`, README first)

Copied from 084-06 and changed; 084-06's files are untouched. It targets the part-A pairing (the brief's item 4 named the
084-07b cook; 084-07c's ruling superseded that: part B targets the part-A build and cook, which also lists `AnomalyBench`).

- **G355:** `copy_retry` (every exception, backoff 3/6/12/24/48/96 s) under `stage`, and `restore_m53` retries the whole
  restore; the window's `finally` catches everything. **Proof:** the dry run held an exclusive handle on the staged exe for
  8 s during the restore; the copy failed twice with `WinError 32` and succeeded on attempt 3, all six verified. Selftest:
  a lock outliving every retry raises `Stop`.
- **G356:** `m52_basis` builds the null ratio from the null's own sharpness series at the ON leg's ROI whenever a null is
  given (it used to require null events for the target); `null_check_v2` passes an event-less null (rows present, 0
  labelled / render-held rows, >= 200 settled frames). **On the 084-06 bank:** B0 null ratio lag 0, r 0.9987; B5 PASSES
  (0 events, sigma 0.00825, n 1,170) where the 084-06 gate FAILED. **14 of the 17 CENSORED offsets resolve** — B0 12/12 → PASS
  (16/16 PASS, offsets +4…+6 inside the 8 off flags); B3 2 of 5 (one PASS, one FAIL: an isolated pixel-visible frame at
  label end +16, not contiguous with the tail, cause not established). 3 B3 offsets stay censored (post/pre ±0.11–0.13
  against the null ratio at lag 16, r 0.93) and 2 former B3 PASSes read CENSORED under the null ratio. B9 16/16 FAIL and the
  B0L lever FAIL are unchanged (`084-09-evidence\g356\g356.json`).
- **Partial-flag guard** (084-07 decisions 1–2 + N4): confirmed-partial vs flagged-invisible against the edge's calibrated
  null band; a partial frame excuses only when labelled and backed by >= 1 `held` texture in `stuck_mip.render_textures`
  (flagged-invisible frames counted, not failed); partial on an unlabelled frame FAILS; any unflagged off frame FAILS; an off
  frame backed only by `unresolved` FAILS; per-edge histograms with warm-up events separate and included; edges > 8 listed.
  Can-fails: a flag spread over a real late label FAILS; a flag on a frame with no held texture FAILS.
- **The self-test caught an evaluator defect (G368):** 084-06's "a transition flag on a leg without temporal AA FAILs" also
  fired on `partial`, which the build writes with or without AA — every AA-off partial onset would have false-FAILed. Now
  reason-scoped (temporal reasons only), through one helper production and the selftest share.
- **`--label-rule`** runs on every leg (pinned `4bf0915` `verify_capture.py`); `labelled_membership` and
  `presence_contract_v2` read the new rule, and old banks are declared "read under the OLD rule". Proven both ways on
  synthetic sessions including the FF41BFF3 shape (FAIL).
- **camera_clipping:** `cc_gate` (084-06's oracle + per-edge label-vs-pixel flip offsets + unconfirmed counts) on the B-CC
  schedule legs; `cc_mw_gate` (per-frame floor `D_k > 2 N_k` for labelled CONFIRMED frames, unconfirmed reported apart). The
  084-06 E9FF019A bank FAILS it (169 of 200 labelled frames at the floor) — the gate can fail on real data.
- **MASK55 v2:** capture fires one anomaly per burst, so `IAI.Auto.MaxConcurrent` cannot make events concurrent (G366); the
  only real route to two live mask records is a `stuck_low_mip` restore trail (084-06 B0 read peak live 2). The leg is
  MainWorld auto-pool with `stuck_low_mip` added and `IAI.Bench.TagPoolLimit 12`; `peak_live >= 2` is a validity condition;
  the gate adds TAG-OWNERS on every frame with 0 `shared` (one value, two holders) and reports the A4 counters. The 084-06
  bank reads INVALID (peak 1) — the known answer.
- **LOD fixture (ruling 4):** rule declared first (`084-09-evidence\lod-selection-rule.md`); commandlet scan of all 421
  MainWorld actors (content junctions, manifest identical before/after); **no target qualifies** — the only multi-LOD
  non-Nanite meshes are `SM_rock` / `SM_rock_02` (4 LODs, triangle ratio 7.997 against >= 8) and `SM_Bush` (2 LODs, off
  frustum). The harness carries premise legs on the two rocks; the 8 gate legs need `--with-lod-gate` and a passed premise.
  The commandlet cannot read LOD screen sizes (G367).
- **m52 re-run + B-REAL:** B0, B5, B3, B9, B0L, P1 as 084-06 with the new guards; B-REAL = the four proven types × both tick
  orders on the rock.
- **missing_texture reference (ruling 5):** amendment R1 declared in the README at 05:55:54 before the re-check ran (hash
  recorded). Result on the MASK55 bank: `StaticMeshActor_85` is **CONFOUNDED-REFERENCE** (clean frames 180–182 sit inside the
  settle after the previous event's `hide_return` at 179; post frames 191–192 are too few). Per type under R1:
  **missing_texture 37, corrupted_texture 29, missing_object 20 CONFOUNDED (+1 warm-up); blinking 0 (32 PASS).** The bank
  cannot decide it (G369); R1 was not loosened. An MT85 pair (targeted, `2 4 8 14 0`, both orders) is added to the bench.
- **Gate selftest 24 / 24.** **Dry run** (`dry --with-lod-gate`, E: root, injected lock): exit 0, preflight 0 problems, 29/29
  launches ACCEPTED, 8 NOT-RUN-PREMISE, 516 s.
- **Expected bench time:** ≈ 66 min (≈ 80 with retries and waits) for the 29 required legs; +≈ 15 min with the LOD gate.

## 3. Deviations and NEEDS-DECISION
- **LOD:** the declared rule selected nothing on MainWorld; a high-contrast fixture needs new content + a cook, which this
  brief forbade. Chat: accept the rocks (ratio 7.997) as best-remaining, or authorise a fixture mesh/level.
- **missing_texture MASK55 borderline:** undecidable on the bank under R1; decided by the MT85 pair on the bench.
- **MASK55 v2 concurrency:** the brief's "pool's concurrency setting" does not reach a capture; the substitute is
  `stuck_low_mip` in the pool (a real record overlap), with peak live >= 2 as a validity condition.
- The G356 re-evaluation surfaced one isolated late frame on B3 (old build) and two new CENSORED B3 events under the null
  ratio; reported, not attributed.

## 4. Hand-off
- **Chat:** rule on the LOD shortfall; note the MT85 addition and the MASK55 v2 route.
- **084-09 (bench):** `084-09-window.py live` on `8A6074AA` + `4BF63FB8`; `--with-lod-gate` only after chat's ruling.
