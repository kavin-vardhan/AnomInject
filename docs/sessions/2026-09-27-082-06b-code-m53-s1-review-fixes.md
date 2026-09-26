# 082-06b — m53 S1: Codex's nine source-review findings fixed, harness extended, both targets rebuilt (G10), Codex delta

**Brief 082-06b, 2026-09-27, Claude Code (Opus 5.5), headless.** Inputs: chat's disposition
`_reviews/082-06b-chat-ruling-s1-source-review.md`, which accepted all nine findings of Codex's
`_reviews/082-06-codex-m53-s1-source-review.md` and rejected declared deviations 5 and 10. Code base `193bd35`.
**No authoring, no cook, no CaptureBench, no leg, no staging, no tag.** The bench's staged exe is still
`E0BE6F0A`.

## 0. Where the work ran

- Source: the branch worktree `E:\IA_BuildCache\_r53_src\AnomalyInjector` (`feat/m53-uv-normal-corruption`).
- Builds: the kept warm scratch host `E:\IA_BuildCache\_r53_host\StackOBot`, its plugin a detached worktree moved to
  each head with `git checkout --detach`.
- The main checkout (`m51`), `master`, tags, containers, `ToCodex\` and `E:\AmmaYT` were not touched.
- Evidence: `D:\IntrusiveAnomalies\_reviews\082-06b-evidence\`: build logs and status, `link-sets.txt`,
  `archive.json`, `a44-scan.json`, `offline-checks.txt`, `offline-checks-mutant.txt`, `pure-test\`,
  `pure-test-mutant\`, `strip-run1.txt`.

## 1. The fixes

| finding | commit | what changed | proof (this brief) |
|---|---|---|---|
| (shared) | `127b7e1` | `TexCorruptPure.h` gains the pure pieces every fix uses: `FLedgerCore`, `FEventAccount`, `PlanAllocations`, `ConditionHeld`, `RequiredUsages`, `WalkChain`, `IsCollateralPrimitive`, `PostRevertSampleFrame`. `EventRequirement` now shares `IsFirstOccurrence`/`OpensScratchClass` with the plan. | the existing 83 checks still pass on the refactored header |
| **P2-5** | `dbc7033` | Apply executes `PlanAllocations` step by step (`AllocateAll`). Each created target is recorded at once (`NoteCreated`). `AllocateTarget` reports a created-then-rejected resource, which goes to pending. Every release goes through `ReleaseCreated` into the two-frame pending ledger: outputs, each scratch level, the static scratch, rollback and revert. `Close` un-reserves only never-created bytes. `FailStep 2 <ordinal>` fails a plan step after creating its resource. | harness [9]–[11] |
| **P2-1** | `034564a` | `EvaluateCondition` reads every expected slot (qualified, with a host MID) and every bound parameter, then decides through `ConditionHeld`, which has no NoApply input. An empty set reads `no_expected_set`. Telemetry `texcorrupt.condition_detail`; the reading is printed on the APPLIED and NoApply 2 lines. | harness [12] |
| **P2-6** | `034564a` | `RegisterTargetWatch` also runs on the NoApply 2 path (deviation 10 withdrawn). | source path |
| **P2-2** | `f57004d` | G-COLL walks every actor's primitives and keeps registered non-target ones drawn on screen within max(0.2 s, frame delta): `GetLastRenderTimeOnScreen`, not `GetLastRenderTime`, which counts shadow-only draws. It reads `GetUsedMaterials`, with no injection filter. It counts incompleteness: dropped by cap, unmeasured materials, unknown residency, and no render evidence. New keys `texcorrupt.collateral_incomplete`, `collateral_complete`, run_summary `texcorrupt_collateral_incomplete_frames`; "INCOMPLETE, NOT A CLEAN READING" is printed. | harness [14] synthetic scene; `GatherCollateral` no longer calls `GetVisibleRenderableActors` / `IsRenderableComponent` |
| **P3-1** | `f57004d` | Post-revert sample at the first tick with `GFrameCounter >= RevertFrame + 2`, with endpoint on_frame / late / early / cancelled. | harness [14] |
| **P3-3** | `f57004d`, `1d7cada` | `TEXCORRUPT-LEDGER … kind=post_revert revert_frame frame offset endpoint live pending peak` after every revert; plan §R9.4 corrected. | harness [10], [11] |
| **P2-3** | `1067481` | `AccumulateFrameEvents` takes each frame's captured telemetry (snapshot async; capture-moment sync) and sets the subtype from `texcorrupt.mode`, keyed by (id, start frame, target). `GetLiveModeName` and its registry are deleted. | source path |
| **P2-4** | `c297ee2` | `FindUsageRefusal` gathers the facts; `RequiredUsages` returns every applicable flag (deviation 5 withdrawn). Undetermined ⇒ `usage_undetermined:<why>`. | harness [13], 18 rows |
| **P3-2** | `c297ee2` | `FindRuntimeLink` goes through `WalkChain`; past 16 links (a cycle included) ⇒ `host_mid` sub `<where>:chain_limit_16_unverified_tail`. | harness [14] |
| (compile) | `9382173` | The first editor build found two errors of mine: a local `Held` shadowing the member (C4458, warnings as errors), and `Root` still read by A3 after the P2-4 rewrite. | editor iter2 exit 0 |
| (harness) | `accdb9f` | Groups [9]–[14] and `tools/texcorrupt_make_mutant.py`. | §2 |
| (plan) | `1d7cada` | Revision 3.2: §R0.000 table, the P3-3 gate text, and every touched paragraph marked 🔁 082-06b. | — |

Commits are grouped where one change carries two findings (P2-1 with P2-6, P2-2 with P3-1/P3-3, P2-4 with P3-2).
Only the branch tip is compiled, as in 082-05.

## 2. Offline harness

`tools/texcorrupt_pure_test.cpp` on the header the plugin compiles, `cl /std:c++17 /W4 /WX`:
**151 checks, 0 failures** (`offline-checks.txt`). The new groups:

- **[9] allocation plan:** the order Apply executes (A's output, its 12 scratch levels, B's output sharing A's
  class, C's output then its 11 levels). The plan's bytes equal `EventRequirement`; a duplicate counts once; a
  single-mip source plans no scratch; overflow is refused.
- **[10] ledger core:** two-frame pending with the frame passed in (held at F+1, retired at F+2); bucket overflow
  merges later; the peak is never reset.
- **[11] transaction sequences:** **129 rollbacks**, i.e. 2N+1 per set for N = 26, 36 and 1 plan steps: every
  step, fail-before-create and created-then-rejected, plus a failure after all allocations. **0 violations** of
  live 0, pending == created, still pending at F+1, zero at F+2, peak == reserved. The static and redraw success
  paths balance at the post-revert frame.
- **[12] condition:** installed / slot_not_installed (NoApply 1) / no_expected_set (NoApply 2) /
  binding_readback / no_event.
- **[13] usage:** 18 rows over every flag class. They include Codex's instanced + lightmapped case (→
  `ism+static_lighting`), Nanite + lighting, Nanite ISM, spline (never Nanite), per-LOD lighting in both
  directions, LOD0-shared lighting (asset flag or forced by instancing), ForceVolumetric, cloth and morph. Plus
  two undetermined gaps.
- **[14]:** chain walk (16 links clean; 17 links and a cycle `limit_reached`). G-COLL over a synthetic scene:
  a 2 % prop, a 500 m backdrop, foliage and a translucent-only mesh are measured; the target, a shadow-only
  primitive and an unregistered one are not. The window is the engine's tolerance. The post-revert frame is
  revert + 2.

**Mutant** (`tools/texcorrupt_make_mutant.py`; each replacement must match exactly once): the two 082-05 faults,
plus pending retired a frame early, created bytes un-reserved instead of pending, an empty expected set read as
held, the first-category usage exit, the chain limit read as clean, and the target not excluded.
**24 failures; every new group fails** ([9] 3, [10] 2, [11] 5 with 344 sequence violations, [12] 1, [13] 2,
[14] 3), plus the old [2] 5 and [7] 3.

⚠ One expectation in the harness was mine and wrong before it was committed: the sequence count, which I first
wrote as 81; the plan gives 129. The code was not changed for it.

## 3. G10

Host detached at `1d7cada`, tree clean. The plugin's build products were deleted first (G309):
`Plugins\AnomalyInjector\{Intermediate,Binaries}` and the game target's `Intermediate\...\Development\Anomaly*`.

| target | actions | exit | seconds | warnings |
|---|---|---|---|---|
| `StackOBotEditor Win64 Development` | 16, all five plugin modules compiled | 0 | 43 | 0 |
| `StackOBot Win64 Development` | 7, all five plugin modules compiled + link | 0 | 74 | 0 |

- **Module set unchanged:** `git diff 193bd35 1d7cada -- '*.Build.cs' '*.uplugin'` is empty. `AnomalyInjector`
  links exactly `Core, CoreUObject, Engine, Foliage, InputCore, RenderCore, RHI` (`link-sets.txt`).
- **A44** on the new exe, UTF-16: `TEXCORRUPT-LEDGER` 2, `texcorrupt.condition_detail` 1,
  `texcorrupt.collateral_complete` 1, `texcorrupt_collateral_incomplete_frames` 1,
  `chain_limit_%d_unverified_tail` 1, `usage_undetermined` 2, `fail_alloc_ordinal` 1. The pre-existing controls
  read `IAI.Capture.ShaderPrewarm` 7 and `IAI.Bench.StuckMipNoHold` 5, as in 082-05, so the scan is sound.
- **Archived, hash-verified at the destination, NOT staged:** game exe **`8BF054FA`** (241,942,528 B) as
  `E:\IA_BuildCache\_binary_baselines\StackOBot.exe.m53-s1fix-8BF054FA`; editor DLLs, PDBs and
  `UnrealEditor.modules` as `m53-s1fix-editor-1d7cada\` (`UnrealEditor-AnomalyInjector.dll` `BDA1C2DC`).
- **Comment strip:** 125 files, **0 changed**.

## 4. Declared choices and limits

1. **P2-4 supersets, each one only adds a requirement:** the min-LOD clamp is not applied; MorphTargets is
   required when the asset has any morph target (the engine's per-material set is runtime state). The WITH_EDITOR
   GPU-lightmass preview override is not mirrored; it does not exist in a packaged game.
   `GForceDefaultMaterial` is a debug switch, not a usage flag, and is not mirrored.
2. **P2-2:** non-2D texture bindings (cube, array, volume, virtual) are outside G-COLL, as before, and are not
   counted as incomplete. "Nothing drawn on screen at all, target included" counts 1 toward incompleteness.
3. **P3-2** uses the existing `host_mid` final reason with a named sub-reason; no new final reason was added.
4. **P2-3** removes a public exported function (`AnomalyTexCorrupt::GetLiveModeName`); its only caller was the
   capture module.
5. **P2-5:** the warm-draw targets stay outside the event ledger. They are not event resources, have their own
   cleanup, and bind no host.
6. **Additive keys:** telemetry `texcorrupt.condition_detail`, `collateral_incomplete`,
   `collateral_complete`; run_summary `texcorrupt_collateral_incomplete_frames`. Bench lever
   `IAI.Bench.TexCorruptFailStep 2 <ordinal>`.
7. **In-engine proofs are due in 082-07's legs, not here** (this brief runs none): P2-1's reading in a leg log,
   P2-2's synthetic out-of-filter collateral, P2-3's revert before readback completion and a second event of one
   id, P2-6's EndPlay while the UObject is still valid, and P2-5's `FailStep 2 <ordinal>`.

## 5. Codex delta

Recorded in §6 once collected.

## 6. Codex delta — collected, NOT acted on

- Relay run `_relay\runs\2026-09-27-082-06b-m53-s1-fix-delta`: `gpt-6-astra`, effort `max` (requested and rollout
  agree), `workspace-write`, **649 s**, exit 0, 5,127 B `review.md`. Reviewed head `2f81b3b`.
- Collected verbatim to `_reviews/082-06b-codex-m53-s1-fix-delta.md` (one header line; body SHA-256 identical);
  ledger row added.
- **`VERDICT: CHANGES REQUIRED`.** RESOLVED: P2-1, P2-3, P2-4, P2-5, P2-6, P3-1, P3-2. PARTIAL: **P2-2**, **P3-3**.
  No new P1 or P2; one new P3.
  - **P2-2 residual:** a drawn non-target mesh with a **null material slot** renders the engine default
    material, but `GetUsedMaterials` returns the null and the collector skips it. A texture entry that
    cannot be resolved is also not counted, so `collateral_complete` can read true without measuring that
    coverage. Codex proposes collecting the effective fallback where determinable and otherwise counting the
    gap as incomplete, with collection/completeness cases.
  - **P3-3 / new P3:** the revised §R9.4 text asks for live=0 **and pending=0** on the rollback line, but a
    correct rollback that created bytes has pending > 0 there, and `FailAt` schedules no frame-qualified
    terminal reading. Codex proposes live=0 and pending=created at the failure frame F, then live=pending=0 on an
    F+2 reading emitted for rollback too.
  - **"Ready for authoring, cook and legs?" NO.** The blocker is P2-2's unreported material/binding gaps
    (under chat's no-unresolved-P2 rule), and the P3 rollback criterion needs correcting before that gate is
    evaluated.
- Per the brief and the Relay rules, **none of this was acted on**; it goes to chat for a ruling.
