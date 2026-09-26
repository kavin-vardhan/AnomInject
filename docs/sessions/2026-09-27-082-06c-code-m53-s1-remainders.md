# 082-06c — m53 S1: the two remainders of Codex's fix delta fixed, harness extended, both targets rebuilt (G10), Codex mini-delta

**Brief 082-06c, 2026-09-27, Claude Code (Opus 5.5), headless.** Input: chat's ruling
`_reviews/082-06c-chat-ruling-s1-fix-delta.md`, which accepted the two remainders of Codex's fix delta
`_reviews/082-06b-codex-m53-s1-fix-delta.md` (P2-2 and P3-3 PARTIAL). Code base `40aa07a`.
**No authoring, no cook, no CaptureBench, no leg, no staging, no tag.** The bench's staged exe is still
`E0BE6F0A` (hashed this brief).

## 0. Where the work ran

- Source: the branch worktree `E:\IA_BuildCache\_r53_src\AnomalyInjector` (`feat/m53-uv-normal-corruption`).
- Builds: the kept warm host `E:\IA_BuildCache\_r53_host\StackOBot`, its plugin a detached worktree moved to
  `1fc20ea` with `git checkout --detach` (tree clean, no diff against the commit).
- The main checkout (`m51`), `master`, tags, containers, `ToCodex\` and `E:\AmmaYT` were not touched.
- Evidence: `D:\IntrusiveAnomalies\_reviews\082-06c-evidence\`: `offline-checks.txt`,
  `offline-checks-mutant.txt`, `single-fault-summary.txt` (+ `single-<fault>\checks.txt`), `pure-test\`,
  `pure-test-mutant\`, `build.ps1`, `build-status.txt`, both G10 logs, `a44-scan.json`, `archive.json`,
  `strip-run1.txt`.

## 1. The fixes

| item | commit | what changed | proof (this brief) |
|---|---|---|---|
| **P2-2 remainder** | `9b79486` | `GatherCollateral` passes every slot from `GetUsedMaterials` through `TexCorruptPure::MeasuredSlotMaterial`: a null slot is measured through `UMaterial::GetDefaultMaterial(MD_Surface)`, what the static and skeletal proxies draw there (`StaticMeshRender.cpp:2223–2226`, `SkeletalMesh.cpp:5788–5800`), and counted `null_slots`; a default that cannot be resolved counts unresolved. Every texture entry goes through `ClassifyCollateralEntry`: one whose game-thread value is null counts **`collateral_unresolved`**. `CollateralIncompleteCount` adds it to `collateral_incomplete`, so the result is never complete. New telemetry `texcorrupt.collateral_unresolved`; the G-COLL apply line and `TEXCORRUPT-COLL` carry `unresolved` and `null_slots`. | harness [15] |
| **P3-3** (+ Codex's new P3) | `9b79486` | `FailAt` schedules a frame-qualified terminal reading at F+2, the same mechanism a revert uses: `TEXCORRUPT-LEDGER … kind=rollback rollback_frame=F frame=F+2 … balance=`. The rollback line now says pending > 0 is expected there and that it is not a balance verdict, and names the reading frame. `TexCorruptPure::JudgeLedgerReading`: before terminal+2 → `not_yet_due`; at or after it, live=0 and pending=0 → `balanced`, else `unbalanced`. Every ledger line (post_revert, rollback, world_teardown) prints `balance=`. The `IAI.Bench.TexCorruptFailStep` help names the reading. | harness [11], [15] |
| (harness) | `74c25de` | [11] judges every sequence's own line and F+2 reading; group [15]; five mutant faults. | §2 |
| (plan) | `1fc20ea` | Revision 3.3: §R0.0000 table; §R9.4, §R10 and §R12.3 `G-COLL` marked 🔁 082-06c; §R0.000's P2-2 and P3-3 rows note Codex's PARTIAL. | — |

## 2. Offline harness

`tools/texcorrupt_pure_test.cpp` on the header the plugin compiles, `cl /std:c++17 /W4 /WX`:
**172 checks, 0 failures** (151 before; +1 in [11], +20 in [15]).

- **[11]:** each of the 129 rollback sequences now also judges its own readings: the line at F must read
  `not_yet_due` and the F+2 reading `balanced`. **126 of the 129 lines carry pending > 0** (every sequence that
  created a byte), which is allowed. The success paths judge the revert line and the revert+2 reading the same
  way.
- **[15] P2-2:** control (every entry resolved → complete); **a null slot's default-material textures join the
  set** with `null_slots` 1 and the set still complete; a null slot whose default cannot be resolved → unresolved
  1, never clean; **an unresolvable entry → `collateral_unresolved` 1, incomplete 1, never reported clean**, while
  the entry beside it is still measured; a resolved non-2D entry is neither collected nor unresolved.
- **[15] P3-3:** the rollback line at F reads live 0 with **pending > 0** and is `not_yet_due`; F+1 is
  `not_yet_due`; **F+2 reads 0/0 and `balanced`**; bytes still pending or still live at F+2 are `unbalanced`; a
  late reading is still judged.

**Mutant** (`tools/texcorrupt_make_mutant.py`, now 13 faults): **34 failures**, 9 of them in [15] and the new
[11] row (0 of 126, and 519 sequence violations, the first "the rollback line was judged as a balance verdict").
**Each new fault applied alone** (`single-fault-summary.txt`) fails its own rows and nothing else:

| fault | failures alone |
|---|---|
| `null_slot_not_measured` | 1: the null-slot set row |
| `unresolved_entry_not_counted` | 3: unresolved, incomplete, never clean |
| `unresolved_left_out_of_incomplete` | 3: both never-clean rows and incomplete |
| `rollback_line_judged_at_once` | 6: [11] violations, the three success paths, the F and F+1 rows |
| `balance_ignores_pending` | 1: bytes still pending at F+2 |

## 3. G10

Host detached at `1fc20ea`, tree clean. The plugin's build products were deleted first (G309):
`Plugins\AnomalyInjector\{Intermediate,Binaries}` and the game target's `Intermediate\...\Development\Anomaly*`.

| target | actions | exit | seconds | warnings |
|---|---|---|---|---|
| `StackOBotEditor Win64 Development` | 16, all five plugin modules compiled | 0 | 99 | 0 |
| `StackOBot Win64 Development` | 7, all five plugin modules compiled + link | 0 | 70 | 0 |

- **Module set unchanged:** `git diff 193bd35 1fc20ea -- '*.Build.cs' '*.uplugin'` is empty.
- **A44** on the new exe: `texcorrupt.collateral_unresolved` 1, `kind=rollback` 2, `balance=%s` 2,
  `null_slots=%d` 1, `this line is not a balance verdict` 1 (UTF-16); `not_yet_due` 1 (ASCII, the pure header's
  `char` literal). Controls `IAI.Capture.ShaderPrewarm` 7 and `IAI.Bench.StuckMipNoHold` 5, as in 082-05 and
  082-06b.
- **Archived, hash-verified at the destination (12/12), NOT staged:** game exe **`2FCDF059`** (241,945,600 B) as
  `E:\IA_BuildCache\_binary_baselines\StackOBot.exe.m53-s1rem-2FCDF059`; editor DLLs, PDBs and
  `UnrealEditor.modules` as `m53-s1rem-editor-1fc20ea\` (`UnrealEditor-AnomalyInjector.dll` `E4BD36D8`).
- **Comment strip:** 125 files, **0 changed**.

## 4. Declared choices and limits

1. **"Unresolved" means an entry whose game-thread value is null**, plus a null slot whose default material
   cannot be resolved. A resolved texture that is not a `UTexture2D` (cube, array, volume, render target) stays
   outside G-COLL and is not counted, as declared in 082-06b §4 item 2.
2. **The null-slot substitution is applied to every drawn primitive.** It is the documented fallback of the static
   and skeletal proxies; the fallback of other primitive types was not audited type by type.
3. **Not addressed, outside the ruling:** the same engine sites also draw the default material for a **non-null**
   material that fails a usage check (static lighting on static meshes; skeletal/clothing usage on skinned meshes).
   The collector measures the assigned material there. Declared in the plan's §R0.0000.
4. **The rollback reading also takes the G-COLL sample** (`TEXCORRUPT-COLL kind=post_rollback`): the collateral
   set is gathered at Apply, before the transaction runs, so a refused event's set existed and is now closed out
   at F+2 like a reverted one's.
5. **Scope of 0/0:** the ledger is shared by both m53 ids, so a reading taken while the other id holds bytes
   reads `unbalanced` by construction. `G-COST`'s legs are targeted, one event at a time (plan §R9.4).
6. **Citation slip, mine:** the fix commit's message cites `SkeletalMesh.cpp:5778`, the line that reads the slot;
   the fallback itself is `:5788–5800`, which the plan cites. The commit is pushed and is not rewritten.
7. **In-engine proofs are due in 082-07's legs:** a `FailStep` leg reading `kind=rollback … balance=balanced` at
   F+2, and a G-COLL reading with a null-slot primitive on screen (`null_slots` > 0).

## 5. Codex mini-delta

Recorded in §6 once collected.

## 6. Codex mini-delta — collected, NOT acted on

- Relay run `_relay\runs\2026-09-27-082-06c-m53-s1-mini-delta`: `gpt-6-astra`, effort `max` (requested and rollout
  agree), `workspace-write`, **290 s**, exit 0, 2,684 B `review.md`. Reviewed head `02987b7`.
- Collected verbatim to `_reviews/082-06c-codex-m53-s1-mini-delta.md` (one header line; body SHA-256 identical,
  `6E8DF20D`); ledger row added.
- **`VERDICT: APPROVE`.**
  - **P2-2 remainder: RESOLVED** (null slots through the default material; unresolved entries counted, in the
    incompleteness sum and blocking `collateral_complete`; an unavailable default counts unresolved).
  - **P3-3: RESOLVED** (rollback schedules F+2; the tick retires pending before the reading; the shared judge gives
    no verdict before F+2 and needs both counters zero after; plan text and harness rows cited).
  - **Anything new: no new defect.** It names the additions (unresolved telemetry, null-slot diagnostics, the
    post-rollback collateral sample) and notes that the declared non-null usage-fallback limit predates these commits
    and is outside the remainder.
  - It independently re-hashed the 12 archived artifacts (all match) and notes that build source `1fc20ea` differs
    from the reviewed head only in documentation.
  - **"Ready for authoring, cook and legs?" YES** for code readiness; the null-slot and F+2 in-engine readings remain
    due in the legs. It asks for chat's review for disposition and release of the next stage.
- Per the brief and the Relay rules, **none of this was acted on**; it goes to chat.
