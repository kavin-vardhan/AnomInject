# 095-02 — Plan C: terrain blink subtype (m58, `feat/m58-terrain-blink-subtype`)

**Author:** Claude Code (Opus 5.5, xhigh), headless, 2026-11-13. **Status:** PLAN ONLY. Nothing built, run or changed.
**Base:** `master` = `ebc34ed` (= release `d7eee97` + one docs commit). Every `file:line` is on `master` unless named.
**Binding input:** `_reviews\095-00-ruling.md` §4 (C1–C5).

**Marks.** **[V]** = I re-checked it against `master` source or UE 5.1.1 source. **[R]** = reported by a sub-reviewer or an
older record, not re-checked by me.

**Short names.** `SUB` = `Source/AnomalyCapture/Private/AnomalyCaptureSubsystem.cpp`. `LW` =
`Source/AnomalyCapture/Private/AnomalyLabelWriter.cpp`. `VP` = `Source/AnomalyInjector/Private/AnomalyViewport.cpp`.
`UE` = `D:\UESource\UnrealEngine\Engine\Source`.

---

## 0. Summary

- **The change (C1, C2).** One game-agnostic terrain rule, decided once per event while the target is alive. A `blinking`
  event whose target is terrain reads `anomaly_type: blink`, `anomaly_subtype: terrain_disappear_reappear`. All other blink
  events stay `disappear_reappear`. No new module dependency; an optional ini pattern list, default empty.
- **The plain answer to item 4: an engine-landscape blink cannot be labelled to the product standard on `master` today.**
  - The auto-pool and the census can never select engine terrain: the landscape component is not a mesh component, and the
    runtime grass reports zero instances when it is not Nanite, so it fails the empty-instance guard. [V]
  - A targeted fire can reach a landscape tile and the hide does remove it, but the event ships with **no mask** and a box
    that covers the grass, a single point or nothing — never the terrain. [V]
- **What C as ruled therefore delivers:** the new subtype on terrain that the plugin already labels correctly, i.e. mesh
  terrain (static meshes or Blueprints, the likely case on the client's hosts) recognised by the ini patterns. For engine
  landscape it can only rename events that are already below standard.
- **Recommendation:** ship C as ruled **plus a refusal**: a `blinking` fire on an engine-landscape target is refused by name
  (`terrain_unlabelable`, counted) until a separate unit makes landscape boxes and masks correct. Chat decides (Question 1).

---

## 1. Where the subtype is decided today, and what is known at that moment [V]

- **The decision.** `MapAnomalyToClient` (`SUB:652-664`): `blinking` → type `blink`, subtype `disappear_reappear`,
  always. Every other id → type and subtype = the id. The texture-corruption ids then overwrite the subtype with their mode
  (`SUB:10072-10075`, mode collected at `SUB:9927-9938`).
- **Called from two places, both at annotation write time** (inside `FinishRun`):
  - `WriteSessionAnnotationFile`, per accumulated event (`SUB:10070`);
  - the same function, for events that exist only through transition entries (`SUB:10244`).
- **What is known at that moment.** The event accumulator (`FSessionEventAccum`, filled by `AccumulateFrameEvents`
  `SUB:9890-9999`) holds the id, the target name, the start frame and a frozen anchor copied at the event's first sampled
  frame: actor path, asset name, component class, bounds (`SUB:9950-9959`).
- **Where the frozen anchor is made.** `FreezeEventAnchor` (`SUB:8518-8538`), called from `FreezeSampleGeometry`
  (`SUB:8540-8551`) during the tick-end sample, i.e. on the game thread **while the target actor is alive**. It calls
  `ResolveNodeIdentity` (`SUB:595-650`), which takes the first visible renderable component.
- **A fire has exactly one target actor.** `FAutoLiveFireInfo` carries one `TargetActor` weak pointer and one `Target`
  name (`Source/AnomalyInjector/Public/AnomalyAutoInjectorSubsystem.h`, struct `FAutoLiveFireInfo`). The live actor is
  **not** safe to read at write time (it may be destroyed or reverted), so terrain must be decided at the anchor freeze.

---

## 2. Terrain detection (C2)

- **No new dependency is possible or needed.** `AnomalyInjector` depends on `Core`, `CoreUObject`, `Engine`, `InputCore`
  (public) and `Foliage`, `RenderCore`, `RHI` (private); `AnomalyCapture` has no `Landscape` either [R, `Build.cs` files].
- **Exact UE 5.1 class names** (`UClass::GetName()`, no prefix letter) [V for `LandscapeComponent : UPrimitiveComponent`
  (`UE/Runtime/Landscape/Classes/LandscapeComponent.h:381-382`) and `GrassInstancedStaticMeshComponent :
  HierarchicalInstancedStaticMeshComponent` (`UE/Runtime/Foliage/Public/GrassInstancedStaticMeshComponent.h`); the rest R]:
  - actors: `LandscapeProxy` (base), `Landscape`, `LandscapeStreamingProxy`; related `LandscapeMeshProxyActor`;
  - components: `LandscapeComponent`, `LandscapeHeightfieldCollisionComponent`, `LandscapeMeshCollisionComponent`,
    `LandscapeNaniteComponent` (a `UStaticMeshComponent`), `LandscapeMeshProxyComponent`;
  - grass: `GrassInstancedStaticMeshComponent` (Foliage module, already a dependency).
- **The helper.** A new `AnomalyTerrain` namespace in the `AnomalyInjector` module (`Public/AnomalyTerrain.h`,
  `Private/AnomalyTerrain.cpp`), so both the capture module and the blink anomaly can call it.
  `bool AnomalyTerrain::Classify(const AActor*, FString& OutRule)`:
  1. **Class rule.** At run start, look up `/Script/Landscape.LandscapeProxy` and `/Script/Landscape.LandscapeComponent`
     with `FindObject<UClass>`. If found, an actor is terrain if `IsA` the proxy class, or owns a component that `IsA` the
     component class. True type semantics, subclasses included, no link dependency. If the lookup fails (module not loaded,
     a future rename), the class rule is **INERT and says so** in the run echo, instead of silently matching nothing
     (the foliage lesson: a name test must not fail quietly). Rule text `class:<ClassName>`.
  2. **Pattern rule.** `[AnomalyInjector] TerrainNamePatterns` (array, case-insensitive substrings, compiled default empty),
     with a console override `IAI.SetTerrainPatterns <p…> | clear` because an ini key does not reach a packaged build
     without a re-cook (G88). Matched against the same fields `ExcludedTargetNamePatterns` uses (actor name, component name,
     mesh asset name; precedent `Source/AnomalyInjector/Private/AnomalyDefaults.cpp:303-328`, matcher
     `Source/AnomalyInjector/Private/AnomalyViewport.cpp:101`) plus the actor's class names. Rule text
     `pattern:<p>@<field>=<value>`.
  - The run banner echoes both rules with their source (ini / console / compiled empty) and the class rule's state (G139).
- **Where it is called.** In `FreezeEventAnchor` (`SUB:8518-8538`), for `blinking` only, on the live actor. The result is
  stored in the frozen anchor (`bTerrain`, `TerrainRule`; struct `SUB:434-443`).
- **Where it is used.** `MapAnomalyToClient` takes the anchor's flag; both call sites (`SUB:10070`, `SUB:10244`) look the
  anchor up by the same key (`FrozenAnchorKey`, `SUB:8513-8516`), so the transition-only path cannot lose the subtype.
- **Traceability (internal):** a log line `TERRAIN-SUBTYPE event=… rule=…` once per event; `terrain_rule` in
  `selection_provenance.json` (not written in delivery mode); `run_summary.blink_terrain_events` (int).
- **Static-mesh terrain is invisible to the class rule.** StackOBot's MainMenu "Landscape" is a `StaticMeshActor` with
  `SM_SlopeWarpLandscape`; the Bates terrain recorded earlier is a Blueprint. Only a pattern catches those, and a careless
  pattern (for example `Landscape`) would also catch any non-terrain actor with that word in its name [R].

---

## 3. Events with several targets

- **Today a capture event has exactly one target actor.** Auto and targeted fires pass `"=" + GetName()`, which matches one
  actor (`Source/AnomalyInjector/Private/AnomalyAutoInjectorSubsystem.cpp:573, 783`) [R], and `FAutoLiveFireInfo` holds one
  `TargetActor` [V]. Only a manual console `IAI.Apply blinking <pattern>` can hide several actors, and those fires do not
  produce capture events.
- **So "mixed" means one actor with both terrain and non-terrain parts** (a landscape tile with grass; a Blueprint holding a
  ground mesh and props). The hide removes every primitive on the actor (§4), so the visible result includes the opening in
  the ground.
- **Recommended rule: the event is terrain if the actor, or any of its components, matches a rule.** Record which field
  matched in the rule text.
- **How often with today's picker:** engine landscape — never through the auto-pool (it cannot be selected, §4); targeted
  only. Mesh terrain mixed with props — depends on host content; none is known on StackOBot.

---

## 4. What terrain blinking actually does on `master` today

"Terrain" here means engine landscape. Mesh terrain behaves like any mesh and is labelled to the standard today.

**4.1 Eligibility — never selected by the auto-pool or the census.**
- The admission rule needs a visible `UStaticMeshComponent` or `USkinnedMeshComponent`, not owned by an
  `AInstancedFoliageActor`, not pattern-excluded, and, if instanced, `GetInstanceCount() > 0` (`VP:794-829`) [V].
- `ULandscapeComponent` derives directly from `UPrimitiveComponent` (`UE/Runtime/Landscape/Classes/LandscapeComponent.h:382`)
  [V], so the terrain itself is never admitted.
- Runtime grass (`UGrassInstancedStaticMeshComponent`) receives its instances only through `AcceptPrebuiltTree`
  (`UE/Runtime/Landscape/Private/LandscapeGrass.cpp:3317`) [V], which copies them into `PerInstanceSMData` **only for a
  Nanite proxy** (`UE/Runtime/Engine/Private/HierarchicalInstancedStaticMesh.cpp:2774-2778`) [V]. `GetInstanceCount()` returns
  `PerInstanceSMData.Num()` (`UE/Runtime/Engine/Private/InstancedStaticMesh.cpp:4103-4106`) [V]. So non-Nanite grass reads 0
  instances while it draws and fails `VP:816`.
  - ⚠ This also suggests the m5 note "zero-instance-grass LandscapeStreamingProxy" on MainWorld recorded this artefact, not
    empty grass [R].
- A Nanite landscape (`ULandscapeNaniteComponent`, a static mesh component) would be admitted, then refused in the
  auto-pool by the Nanite rule (`IAI.Targets.AllowNanite` default 0) [R].
- The census builds its candidates with the same rule, so it never measures terrain [R, `AnomalyCensus.cpp:90-93`].
- **Even if admitted, the census ceiling would usually exclude it:** above 25 % drawn coverage is excluded, and the ceiling's
  own help text names a landscape hide as the reason it exists (`SUB:10886-10891`) [V for the help text].
- **A targeted fire does reach it:** `TryFireSpecific` matches by exact name with no renderable check [R].
- **Unit C does not change eligibility (C3).**

**4.2 Hiding — yes, through the same route as other actors.**
- `AnomalyHiddenClass::Hide` walks **every** `UPrimitiveComponent` on the actor and turns off main pass, depth pass, shadows,
  indirect and distance-field lighting, ray tracing and decals, keeping custom depth (`AnomalyHiddenClass.cpp:93-128`) [V].
  That includes `ULandscapeComponent`, its collision components and the grass.
- The landscape scene proxy reads those flags from the component [R, `LandscapeRender.cpp:740, 1270-1271, 1387`], so the
  tile should visibly vanish.
- Two residues are not handled [R]: grass components rebuilt during the hold are not hidden; MainWorld's landscape writes a
  runtime virtual texture (`RVT_Crater`), which the hide does not stop.
- In World Partition, `Landscape_0` itself owns no components; each `LandscapeStreamingProxy` is one tile [R].

**4.3 Mask — not measured.**
- Every tagger (the m26 measure, the target mask, the census) tags only components that pass `IsRenderableComponent`
  (`Source/AnomalyCapture/Private/AnomalyStencilTag.cpp:169`) [V]. A landscape proxy has none.
- Zero tagged components sets the tag-failed path: the event is NOT_MEASURED and admitted, `target_pixels −1`, no mask
  PNG [R, `AnomalyMaskMeasure.cpp:508-512`].
- The engine itself does not block custom depth on landscape: `SetRenderCustomDepth` is the generic setter and the landscape
  proxy reads `ShouldRenderCustomDepth()` [R]. A Nanite landscape drops custom depth (G134 applies) [R]. MainWorld's
  landscape is not Nanite [R].

**4.4 Label box — never the terrain.**
- With no event component scope, the box comes from the union of static and skinned mesh components
  (`VP:861-879` via `GetActorLabelBounds` `VP:1761-1771`) [V for the call chain]. On a tile that is:
  the grass instances' bounds (if populated), a zero-size point (empty grass), or nothing (no grass; the
  "NO renderable geometry" warning `SUB:644-648`) [R].
- `blinking` is an actor-hidden type, so membership is "hidden" with no on-screen requirement (`AnomalyLabelSync.h:72-75`)
  [V]: hidden frames are labelled while the box is wrong or invalid.

**Verdict, per part of the standard:** selected — no (targeted only); hidden — yes; masked — no; boxed — no;
subtype — no. **A terrain event cannot be labelled to the product standard today. Chat should not promise the subtype for
engine landscape without either the refusal (Question 1, option b) or a label-scope unit (option c).**

---

## 5. Every reader of `anomaly_subtype`

| Reader | What it does with the subtype | Effect of `terrain_disappear_reappear` |
|---|---|---|
| C++ `MapAnomalyToClient` (`SUB:652-664`) — **the m22 pin** | forces `blink` → `disappear_reappear` | the one deliberate change: the pin gains a second value. `flicker` stays reserved for a future `flickering` class [V] |
| C++ transition-only event path (`SUB:10244`) | calls `MapAnomalyToClient` without the anchor today | must look the anchor up, or a terrain event would silently read `disappear_reappear` [V] |
| `LW:1498` | writes the value | none [V] |
| `tools/measure_label_offset.py:1141, 1818` | passes it through to its report | harmless [V] |
| `tools/label_sync_check.py:2586`, `tools/verify_capture.py:1403` | write it into synthetic test fixtures; type lookups key on `anomaly_type` | harmless [V] |
| Dashboard (`master` `7ea2e51`) — Library, timeline, overlay | never reads it; `git grep` finds it only in `docs/WHATS-NEW.md:68` | harmless; optional docs row [V] |
| CaptureBench `subset_gate.py` and the cross-binary comparators | compare type and subtype across binaries | update: declare the change on a terrain leg [R] |
| CaptureBench `p9_hidden_set.py` | keys on type | harmless [R] |
| `docs/client-readme.md:531` (subtype row), changelog table (`:845`) | documentation | add the value and a changelog row [V for `:531`] |
| Dashboard `docs/WHATS-NEW.md:68-72` | the variants table | optional row [R] |

- No reader keys on the subtype string, so nothing breaks or hides a session.

---

## 6. The bench fixture

- **`CB_GateLevel` has no landscape** (built from `/Engine/BasicShapes`; floor is a scaled cube) [R]. It is the right place
  for the pattern rule: a test pattern matching `StaticMeshActor_49`'s mesh exercises the pattern path on a fully labelled,
  masked, pose-gated target.
- **MainWorld has engine terrain** (already cooked into bench builds since the G120 re-cook): `Landscape_0` and six
  `LandscapeStreamingProxy_…` tiles, with grass `GT_Grass` [R]. It is the cheapest honest fixture for the class rule and for
  the refusal (targeted fire at one tile). Confounds: World Partition streaming, grass rebuilds, the RVT, moving actors.
- **MainMenu** has no engine landscape, only the `SM_SlopeWarpLandscape` static mesh [R] — a realistic mesh-terrain example
  for the pattern rule (optional leg).
- **A clean landscape level (`CB_TerrainLevel`)** is needed only if chat chooses the label-scope unit (option c): one small
  flat non-WP landscape, with and without grass, authored once in the editor, plus a cook.

---

## 7. Gates and can-fails (C5)

| Gate | Leg or test | Pass | Can-fail |
|---|---|---|---|
| C-G1 pure | `tools/terrain_rule_selftest.cpp`: class chain and pattern matcher on synthetic names | all cases | mutants: class rule removed; pattern rule removed; case-sensitive match — each must fail |
| C-G2 class rule | MainWorld, targeted `blinking` on one `LandscapeStreamingProxy` tile, PIE and staged | with the refusal (recommended): refused `terrain_unlabelable`, 0 events, counter 1, echo names the class rule ACTIVE. With a bench lever that disables the refusal: subtype `terrain_disappear_reappear`, rule `class:LandscapeStreamingProxy`, and the leg records honestly `target_pixels −1` and the box | class lookup forced to fail (bench lever): echo reads INERT and the subtype stays `disappear_reappear` |
| C-G3 pattern rule | `CB_GateLevel`, `StaticMeshActor_49`, `IAI.SetTerrainPatterns Cube`, staged both tick orders + PIE | subtype `terrain_disappear_reappear`, rule `pattern:…`; mask, box and edges as on any blink leg | the same leg with `IAI.SetTerrainPatterns clear` reads `disappear_reappear` ("removing the detection fails the gate") |
| C-G4 non-terrain unchanged | `CB_GateLevel` `StaticMeshActor_49`, no patterns | `disappear_reappear`; every other field equal to the release build on a pose-matched pair | — |
| C-G5 no eligibility change | MainWorld auto-pool, 300 frames, patterns empty | 0 terrain events; selection and census counts equal to a release-build leg at the same seed | — |
| C-G6 echo and docs | banner line; readme subtype row and changelog; schema note | present | — |

- **Disk and time:** about 9 legs × 90–300 frames ≈ 0.6–0.8 GB; ~1 h of bench. The PIE legs need only the rebuilt editor;
  the staged legs need a cook containing `CB_GateLevel` and `MainWorld` (the same cook A-1 needs; if C goes first, its
  brief carries that cook).

---

## 8. Commit breakdown and estimate

On `feat/m58-terrain-blink-subtype` off `master`:
1. `feat(injector): m58 game-agnostic terrain rule` — `AnomalyTerrain` helper (runtime class lookup + pattern list), the ini
   key, `IAI.SetTerrainPatterns`, the banner echo, `tools/terrain_rule_selftest.cpp`.
2. `feat(capture): m58 terrain blink subtype` — anchor flag, `MapAnomalyToClient` on both call sites, the log line,
   `selection_provenance.json` field, `run_summary.blink_terrain_events`.
3. (if Question 1 = b) `fix(blinking): m58 refuse engine-landscape targets that cannot be labelled` — in the blink anomaly's
   apply path, class rule only (pattern-matched mesh terrain stays allowed); the refusal goes through the existing
   matched-zero path (no fire, no label) and the yield tallies; a bench lever to disable it for C-G2.
4. `docs: m58 terrain blink subtype` — readme subtype row and changelog, the m22 pin note, architecture catalog, status block,
   journal, gotchas (the grass-instance finding is a gotcha).

**Estimate:** one implementation brief (~2–3 h of Code, both targets, selftest), one gate brief (~1 h bench, sharing the
rebuild and cook with A-1), one docs-and-merge step. About **1–1.5 bench hours**.

---

## Questions for chat

1. **Scope, given §4.** (a) C as ruled: the subtype appears on correctly labelled mesh terrain, and on targeted
   engine-landscape events that ship without a mask and with a wrong box. (b) **C plus a refusal of engine-landscape blink
   targets** until landscape labels are fixed. (c) C plus a label-scope unit: tag `ULandscapeComponent` for the mask and box
   the tile when the target is a landscape (bigger; needs `CB_TerrainLevel`, a cook and its own gates; tile boxes often exceed
   the 25 % ceiling). **Recommendation: (b).** It keeps every shipped label at the standard and costs one small commit; (c)
   only if the office read shows the client's terrain is engine landscape.
2. **Office read (yes/no, no asset names):** in the reviewed session, are the terrain events' targets engine landscape
   (`nodes[0].component_class` a landscape or grass class) or meshes? On `master`'s code an engine landscape cannot be picked
   by the auto-pool, so the client's terrain events were most likely mesh terrain [R, inference]. The answer decides whether
   (c) is ever needed and which pattern the owner must set.
3. **Default patterns stay empty** (ruling C2). Consequence: on the client's hosts the new subtype appears only after the
   owner sets that host's terrain pattern (an ini key needs a re-cook, G88; the console override works per session).
   **Recommendation: keep empty;** the run banner states the active patterns so a missing one is visible.
4. **`missing_object` on terrain** also opens a hole in the ground. **Recommendation: not in m58** (the client asked for the
   blink family); note it for a later unit.
5. **Mixed actors** (terrain and non-terrain parts on one actor). **Recommendation:** terrain if the actor or any component
   matches; the rule text says which.
6. **Schema minor.** **Recommendation:** no bump for C (a new value in an existing string field); A-1 bumps to 2.

---

## 4.25 note

Read-only diff `master` vs `feat/europa-port` for the files C touches (receipt
`code-receipts-095-02\europa-port-diff-touched-files.txt`) [V]:

| File | Diff on `feat/europa-port` |
|---|---|
| `AnomalyCaptureSubsystem.cpp` (`MapAnomalyToClient`, `FreezeEventAnchor`, `WriteSessionAnnotationFile`) | no hunk in those functions |
| `AnomalyViewport.cpp` | +18 / −15 (pattern matcher area to be checked at port time) |
| `AnomalyDefaults.cpp/.h` (ini key, console command) | identical |
| `Anomaly_Blinking.cpp` (the refusal) | identical |
| `AnomalyHiddenClass.cpp` | +4 / −3 |
| new `AnomalyTerrain.*` | new file; ports as is |

- Engine side [R, not re-checked against 4.25 source]: `ALandscapeProxy`, `ALandscape`, `ALandscapeStreamingProxy` and
  `ULandscapeComponent` exist in 4.25 under the same names, so the runtime class lookup works there; `ULandscapeNaniteComponent`
  and `UGrassInstancedStaticMeshComponent` are 5.x classes. The lookup is by path, so a missing class is reported as INERT,
  never a build break.
