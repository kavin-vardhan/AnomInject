# 2026-06-21 — 013 — Missing-Texture Anomaly (m8)

## Goal
Add an 8th anomaly, **`missing_texture`** — the canonical "missing texture" look on a single object via per-component
material swap. Started as **flat magenta** (locked at planning), shipped as **gray/white UV-checker** after an owner
visual-review fork (see Stage 2 / the glow finding). `IAnomaly` stays locked; the plugin gains its **first `Content/`**.

## Locked design (planning turns, pre-implementation)
- **Mechanism:** resolve the target actor's renderable static/skeletal mesh components (reuse `AnomalyLod::ResolveLodComponents`
  — the SM∥SK→`UMeshComponent` resolver, LOD-flavoured name only; future chore: extract a neutral
  `ResolveRenderableMeshComponents`). For each component, override **all** material slots via `UMeshComponent::SetMaterial`
  (per-component override → **object isolation**, never mutates the shared mesh/material asset). Capture per slot the original
  `UMaterialInterface*` + whether it was an explicit override; revert restores it (override → ptr; else `SetMaterial(i, nullptr)`
  to clear to asset default). Zero-match → return false (AMB-2). No Tick.
- **Material — ship ONE asset, content-free is impossible:** a runtime `UMID` from an engine base material can't guarantee a
  flat colour game-agnostically; runtime `UMaterial` graph authoring is editor-only; no engine-shipped flat-magenta exists.
  So the plugin ships its own material. Cook guarantee = **Option B (owner override): a CDO hard-ref** — a `static
  ConstructorHelpers::FObjectFinder` in `UAnomalyInjectorSubsystem`'s ctor assigning a **non-transient `UPROPERTY TObjectPtr`**
  — NOT a host `DirectoriesToAlwaysCook` (forgettable in a fresh project). Flip `.uplugin` `"CanContainContent": true`.
- **Born-complete:** `missing_texture` must be added to BOTH the selector `GAnomalyChoices[]` AND the auto `GAutoPool[]`
  (hardcoded arrays — NOT catalog-derived); capture inherits via the auto pool for free.
- **Invariants:** `IAnomaly` byte-identical; deps stay `Core/CoreUObject/Engine/InputCore`; match by Name/Class never label;
  G30 holds with no id→group table (`OverrideMaterials` is a distinct resource).

## Stage 1 — G-Asset (material + cook), proven
- Authored the materials via a checked-in reproducible script `tools/create_missing_texture_materials.py` (run over the
  unreal-mcpython bridge — scripted, not GUI). Added the CDO hard-ref to `UAnomalyInjectorSubsystem` (ctor `FObjectFinder` +
  non-transient `UPROPERTY` + getter). Flipped `CanContainContent` → true.
- **Two cook detours (now gotchas):**
  - **G47 — the cook runs on EDITOR binaries.** First package built clean but contained no material: the cook commandlet
    (`UnrealEditor-Cmd`) used stale editor DLLs (the CDO change was only a Live Coding patch; `-nocompileeditor`). Rebuilding
    `StackOBotEditor` first → the material cooked.
  - **G48 — UE 5.1 IoStore.** `UnrealPak -list` on the `.pak` showed only the `.uplugin`, but the packaged game loaded the
    material fine — cooked assets live in `StackOBot-Windows.ucas`/`.utoc`, not the `.pak`. Verify by **runtime load**.
- **PROVEN:** packaged Development build → both cooked + the subsystem CDO refs resolve **non-null at runtime in the package**
  (a temporary `Initialize` `[m8-gasset]` log; removed before commit). Owner accepted G-Asset.

## Stage 2 — anomaly class + wiring + gates
- `Private/Anomalies/Anomaly_MissingTexture.{h,cpp}` per the locked design; `Register()`; `GetAuthoredSpec` (Object); selector
  `GAnomalyChoices` + auto `GAutoPool` (`NumPoolKeys` 4→5, key `5` / `pool5` / static_assert / log strings); a generic
  catalog-driven **Draw-4** (auto draws a mode for any enum-leading id).
- **G-Compile** green (`DumpCatalog` = 8). **G-Apply, G-Isolation, G-BornComplete all driven green over the bridge:**
  - G-Apply: static multi-slot `SM_Ramp3` (3 slots) + skeletal `BP_Bot_C_0` (`CharacterMesh0`/`Jetpack` + static cam-mesh),
    exact revert validating **both** override branches (Bot slots held runtime MIDs → restore ptr; ramp asset-defaults →
    clear to null).
  - G-Isolation: actors A/B share `SM_RockFlats_02` + `M_Rock`; recolor A → B untouched; A reverts exact.
  - G-BornComplete: selector cycles to + injects `missing_texture`; auto `FireOnce` fires it; a 14-burst seeded capture run
    produced 29 labeled frames.
- **G49 — material usage flags (real bug caught only by the visual gate).** The override applied but rendered **default-gray**:
  StackOBot ramps are **Nanite**, the Bot is **skeletal**, and the material lacked those usage flags (UE can't add them at
  runtime → substitutes the default material). Fixed by authoring the material with all mesh usage flags
  (skeletal/nanite/ISM/morph/spline). After the fix, the look rendered correctly.

## The glow finding (G50) → owner fork → checker-only
- Owner review of the (then magenta) look: *"reflects magenta onto surrounding surfaces, like a glowing object."* Cause: the
  Unlit + **Emissive** magenta feeds **Lumen's surface cache** and bounces its colour onto neighbours. `bEmissiveLightSource`
  is **not** the lever (defaults false — a no-op). The fix for "flat colour that does not light the room" is a **Lit base-colour**
  material (matte; emissive = 0), which is also the canonical missing-texture look.
- Switched both materials Unlit-Emissive → Lit Base-Colour. Then **owner decision:** *"just have the checkered material for now,
  remove the magenta… I will find a solution for it later."* → **shipped checker-only; flat-magenta + the `mode` arg deferred.**

## Deviations from the locked plan (all owner-driven or bug-driven)
1. **Lit base-colour, not Unlit-Emissive** — unlit-emissive lit the Lumen scene (G50).
2. **Checker-only; flat-magenta deferred** — owner call after seeing the glow.
3. Consequently **no `mode` arg, no Draw-4** — removed the whole mode/enum infrastructure (re-added when magenta returns).
4. **Usage flags** added to the material (G49) — not in the original plan; required for runtime rendering.
5. Tried `bEmissiveLightSource` suppression first — reverted as a confirmed no-op (default already false).

## State (as-built, m8)
- `missing_texture` (object-scoped, no args) swaps SM/SK slots to the shipped **Lit gray/white UV-checker**
  `M_MissingTexture_Checker` (the plugin's only `Content/` asset). Catalog **8**. VersionName **0.9.0**.
- `IAnomaly` untouched; deps `Core/CoreUObject/Engine/InputCore`; G30 holds with no id→group table.
- One `feat(missing-texture)` commit, tagged **`m8`**.

## Deferred / hand-off
- **Flat-magenta variant + a `mode` arg {magenta, checker}.** The blocker is purely the *look*: unlit-emissive glows; lit
  base-colour magenta doesn't glow but shades (can read washed in low light). Owner is choosing the approach. When resolved,
  re-add: the magenta material (+ usage flags), the second CDO ref + the `bChecker` getter param, the catalog `mode` enum,
  the auto Draw-4, and the magenta option in the selector/console. The mode infrastructure was clean to add/remove.
- **Future chore:** extract/rename `AnomalyLod::ResolveLodComponents` → a neutral `ResolveRenderableMeshComponents`
  (now 2 consumers: lod_corruption, missing_texture).

## Environment note (important)
The **unreal-mcpython bridge was unstable** this session — it crashed the editor twice on driven Python calls
(`python39.dll` in `FMCPythonTcpServer::ProcessDataOnGameThread`, including on a trivial `delete_asset` and a `load_level`
during editor startup). G-Apply/Isolation/BornComplete were driven green **before** the instability; the final checker-only
build was validated by **compile** (`Build.bat StackOBotEditor`, clean). The **lit-checker live render is owner-eyeball-pending**
(`IAI.Apply missing_texture =<actor>` in a MainWorld Simulate session). The magenta `.uasset` was removed directly from disk
(the bridge `delete_asset` crashed the editor); it is no longer referenced by any code.
