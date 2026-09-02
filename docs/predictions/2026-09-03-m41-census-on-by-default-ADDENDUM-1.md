# `m41` PREDICTIONS — **ADDENDUM 1**: the `C-G1a` re-instrumentation

**Committed BEFORE any of the runs it describes.** Parent: `docs/predictions/2026-09-03-m41-census-on-by-default.md`
(`2543674`). Authorised by session 069 brief 3, ruling 1.

⛔ **THE PARENT FILE IS NOT EDITED.** `C-G1a` failed on its own written terms and that verdict stands.
This addendum ADDS an instrument; it does not soften, re-aim or retroactively excuse the failure
(`P-C2` route — annotate forward, never amend a closed prediction).

📌 **THE `CB_GateLevel` RESULT STANDS IN THE RECORD AS WRITTEN:**
`HOST-PP CUSTOM-DEPTH READERS = 0 (scanned 0 volume(s), 0 camera blend(s), 0 material(s))` on exe
`2BF9E1B9`, leg `M41_M41_ON_A`. Under the parent file's rule that is **BLINDNESS, NOT A CLEAN READ**,
and it remains a FAIL of `C-G1a` as originally written. What follows is how the question gets answered,
not a re-scoring of that leg.

---

## 1. FIXTURE MEASUREMENT — taken FIRST, before the instrument was designed

Two independent reads, neither using the suspect scan:

**(i) `CB_GateLevel` contains no `APostProcessVolume`.** Authoritative asset-side read of its authoring
script `CaptureBench/tools/make_gate_level.py`: the only classes it ever spawns are
`StaticMeshActor` (the grid), one skeletal actor, `DirectionalLight`, `SkyLight`, `SkyAtmosphere`,
`PointLight` and `PlayerStart`. **No post-process volume is authored, so `V = 0` is the TRUE answer on
that level.**

**(ii) The camera blend cache only ever holds camera-MODIFIER pushes, and is emptied every update.**
`APlayerCameraManager::ApplyCameraModifiers` calls `ClearCachedPPBlends()` as its FIRST statement
(`PlayerCameraManager.cpp:281`), and `AddCachedPPBlend` (`:300-305`) is called only from modifier code.
The `-unattended` bench pawn is a `SpectatorPawn` with no `UCameraComponent` (already established here
by the `IAI.Bench.Letterbox` refusal) and runs no camera modifier, so **`C = 0` is also the TRUE answer
on that fixture.**

⇒ **`V = 0` and `C = 0` are FIXTURE, not code.**

## 2. 🚨 A CODE DEFECT FOUND WHILE MEASURING THE FIXTURE — pre-declared here before it is fixed

The engine assembles a view's post-process from **THREE** sources (`LocalPlayer.cpp:866-881`):

1. the world's post-process **volumes**;
2. `PlayerCameraManager->GetCachedPostProcessBlends(...)` — the block the engine itself comments as
   *"CameraAnim override"*, i.e. camera modifiers/anims (`:870-878`);
3. **`View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, ViewInfo.PostProcessBlendWeight)`
   — the block the engine comments `// CAMERA OVERRIDE` (`:881`)**, i.e. the view target's own POV
   post-process, which is where a `UCameraComponent`'s `PostProcessSettings` actually arrive.

**`m41` as committed at `52f6430` scans (1) and (2) and MISSES (3).** A host that puts a blendable on
its camera component — the most ordinary way to apply a full-screen effect — would be **missed
entirely**, and the preflight would report a confident `= 0`.

⇒ **The diagnosis is BOTH.** `V=0`/`C=0` on `CB_GateLevel` is fixture; the scan being incomplete is
code. `m41` adds source (3) via `APlayerCameraManager::GetCameraCacheView().PostProcessSettings`.

⚠ **This defect would have shipped behind a green gate had `C-G1a` been written without the `scanned`
counts.** The counts are what turned a confident zero into a question.

---

## 3. `C-G1a(b)` — THE LEVEL-INDEPENDENT PROBE

New bench-only console command **`IAI.Bench.ProbeSceneTextureUsage <material path>`** — console-only,
no ini key, never in a client payload. It resolves the named material through **exactly the code path
the preflight uses** (`GetMaterialResource(FeatureLevel)` → `GetGameThreadShaderMap()` →
`UsesSceneTexture(id)`) and prints the `PPI_*` bits it finds. It touches no level and no capture.

**PRE-DECLARED EXPECTATIONS:**

| target | expected |
|---|---|
| `/Engine/BufferVisualization/CustomDepth.CustomDepth` | **`PPI_CustomDepth` PRESENT**; reported as a reader |
| `/Engine/BufferVisualization/FinalImage.FinalImage` (stock post-process) | **`PPI_PostProcessInput0` PRESENT**, `PPI_CustomDepth` and `PPI_CustomStencil` **ABSENT** |
| `/AnomalyInjector/Materials/M_CorruptedTexture_Pink.M_CorruptedTexture_Pink` (ordinary opaque surface material) | **NO scene texture of any kind** |

**PASS** = all three as written. That is a positive control, a discriminating control and a negative
control, so a `CustomDepth` bit that never lights and a `CustomDepth` bit that always lights are both
excluded (`G96`, both directions).

⚠ **DECLARED BRANCH:** a buffer-visualization material is cooked only if the project references it.
**If any target reports `NOT PRESENT IN THIS CONTAINER`, that is a MEASUREMENT, not a failure** — the
probe says so by name and the remaining targets still carry whichever controls they cover. ⛔ **Do not
substitute a different material to manufacture a pass.**

## 4. `C-G1a(a)` — THE `MainWorld` PREFLIGHT LEG

One census-ON leg on `MainWorld` (cooked into this container since `m36`), 30 frames.

**PREDICTED:** the `HOST-PP CUSTOM-DEPTH READERS =` line reports **`V >= 1` or `C >= 1`, and `M >= 1`**,
with **`= 0` readers**. That is the shape the parent file demanded and `CB_GateLevel` structurally
could not provide.

**FAILURE BRANCH, pre-declared:** if the enumeration finds volumes and/or blends on `MainWorld` but
**`M` stays 0**, the blendable walk is broken and that is a **CODE defect** — fix it and say so.
If `V = C = 0` on `MainWorld` too, report it and STOP; the level's own post-process content is then the
next thing to establish, and no verdict is offered.

⚠ **`MainWorld` is NOT `CB_GateLevel`.** This leg judges the **preflight only**. It is not a census
result, not a pose-gated leg, and nothing else may be read off it.

## 5. WHAT THIS ADDENDUM DOES NOT CLAIM

- It does **not** make `C-G1b` (a real custom-depth reader actually attached to a live view) obtainable
  on this bench. That still rides the cook. If §3's `CustomDepth` target turns out to be present, that
  proves **the query recognises such a material**, not that the **preflight finds one in a live view**.
- It does **not** revisit any other gate.
- It does **not** change the veto, the census rule, or any artifact field.
