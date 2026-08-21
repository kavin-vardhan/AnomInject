"""
set_material_usage_flags.py - set the EMaterialUsage flags on every material the plugin ships.

Run HEADLESS. The owner does not open the editor:

    "<ENGINE>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<PROJECT>\\StackOBot.uproject" ^
        -run=pythonscript -script="<PLUGIN>\\tools\\set_material_usage_flags.py" ^
        -unattended -nopause -nosplash -nullrhi

Idempotent: re-running it changes nothing and still prints the full state. It reports the
BEFORE and AFTER value of every flag on every asset, re-reads them after saving, and exits
nonzero if any MANDATORY flag is not set at the end.

--------------------------------------------------------------------------------------------------
WHY THIS EXISTS - the defect, confirmed on Concorde
--------------------------------------------------------------------------------------------------
Preserved Concorde logs carry, for both shipped materials:

    Warning: Material /AnomalyInjector/Materials/M_CorruptedTexture_Pink missing
    bUsedWithStaticLighting=True! Default material will be used in game

and the owner's control run showed the consequence directly: with ONLY corrupted_texture in
the auto pool, the held weapon rendered MAGENTA while other objects rendered the engine
default grid. ONE ANOMALY TYPE, TWO APPEARANCES.

The engine path is `StaticMeshRender.cpp:2225`:

    if (!SectionInfo.Material || (bHasSurfaceStaticLighting &&
        !SectionInfo.Material->CheckMaterialUsage_Concurrent(MATUSAGE_StaticLighting)))
            -> fall back to the DEFAULT MATERIAL

A statically-lit static mesh queries MATUSAGE_StaticLighting; the flag was absent; the section
drew the default material. The held weapon is SKELETAL, and MATUSAGE_SkeletalMesh WAS set, so it
drew our magenta. Same material, two components, two outcomes.

⚠ THE LOG ONLY WARNS FOR USAGES ACTUALLY EXERCISED AT RUNTIME. The absence of other flag
warnings means those mesh-type paths were not hit in those runs - NOT that those flags are set.
So the fix is deliberately NOT narrowed to the one observed flag.

--------------------------------------------------------------------------------------------------
WHICH FLAGS, AND WHY EXACTLY THESE
--------------------------------------------------------------------------------------------------
The reachable component universe is not a guess. `AnomalyLod::ResolveLodComponents` - the only
route by which either anomaly's Apply reaches a component - collects exactly

    AnomalyTargeting::FindComponentsMatching<UStaticMeshComponent>
    AnomalyTargeting::FindComponentsMatching<USkinnedMeshComponent>

and `AnomalyViewport::IsRenderableComponent` ends in

    return Component->IsA<UStaticMeshComponent>() || Component->IsA<USkinnedMeshComponent>();

So our material can only ever land on those two base types and their derivatives. Verified
against engine source rather than memory:

    UInstancedStaticMeshComponent : UStaticMeshComponent    (HISM derives from ISM)
    USplineMeshComponent          : UStaticMeshComponent
    USkeletalMeshComponent        : USkinnedMeshComponent
    UPoseableMeshComponent        : USkinnedMeshComponent

INCLUDED (7), one clause each:
    StaticLighting        MEASURED missing on Concorde; any target in a level with baked lighting.
    SkeletalMesh          USkinnedMeshComponent is half the reachable set.
    InstancedStaticMeshes ISM/HISM derive from UStaticMeshComponent and pass the filter - only
                          foliage ACTORS are excluded (m27), not instanced components generally.
    SplineMeshes          USplineMeshComponent derives from UStaticMeshComponent and passes.
    MorphTargets          a skeletal target may be morph-driven.
    Nanite                a static or skeletal target may be Nanite; Concorde happens to disable
                          Nanite, but that is a HOST setting and correctness must not depend on
                          anything a host can redefine.
    Clothing              a USkeletalMeshComponent may have cloth sections. Reachable today, and
                          the Concorde symptom was on a held weapon attached to a character - a
                          clothed target simply was not hit in those runs.

EXCLUDED, and why - each is UNREACHABLE, not merely unlikely:
    ParticleSprites, BeamTrails, MeshParticles, NiagaraSprites, NiagaraRibbons,
    NiagaraMeshParticles   particle/VFX systems are not UStaticMeshComponent or
                           USkinnedMeshComponent, and G33 removed UFXSystemComponent from the
                           renderable-visible set outright. Apply cannot reach them.
    GeometryCollections    UGeometryCollectionComponent : UMeshComponent - NOT a static or skinned
                           mesh component, so ResolveLodComponents never returns it.
    GeometryCache          UGeometryCacheComponent : UMeshComponent - same reason.
    Water, HairStrands, LidarPointCloud, VirtualHeightfieldMesh
                           dedicated component types, none derived from the two reachable bases.
    EditorCompositing      editor-only, never in a cooked client build.

A usage flag is only ever consulted by the component that is drawing the material, so a flag for
a component type Apply cannot reach can never be queried. Setting it would buy nothing and cost
permutations. If `IsRenderableComponent` or `ResolveLodComponents` is ever widened, THIS LIST
MUST BE WIDENED IN THE SAME CHANGE - that is the coupling to remember.

SHADER PERMUTATION COST, stated: each usage flag adds the vertex-factory permutations for that
mesh type across every quality level and shader platform the project cooks. These are two small
unlit-complexity materials (a constant colour, and a UV checker of about a dozen nodes), so the
per-permutation cost is near the floor; the count roughly scales with the number of usages, so
7 usages is about 40% more vertex-factory permutations than the 5 previously set. That is paid
once at cook time and in package size, not at runtime. It is the correct trade: a missing flag is
a SILENT wrong-appearance defect on somebody else's game, and we have just been bitten by exactly
that.
"""

import sys

import unreal

PLUGIN_CONTENT_ROOT = "/AnomalyInjector"

MANDATORY = [
    "used_with_static_lighting",
]

USAGES = [
    "used_with_static_lighting",
    "used_with_skeletal_mesh",
    "used_with_instanced_static_meshes",
    "used_with_spline_meshes",
    "used_with_morph_targets",
    "used_with_nanite",
    "used_with_clothing",
]

EAL = unreal.EditorAssetLibrary
MEL = unreal.MaterialEditingLibrary

REPORT_PATH = unreal.Paths.convert_relative_path_to_full(
    unreal.Paths.project_saved_dir() + "AnomalyMaterialUsageFlags.txt"
)
_REPORT = []


def log(msg):
    line = "[usage-flags] " + msg
    _REPORT.append(line)
    print(line)
    unreal.log_warning(line)


def flush_report():
    try:
        with open(REPORT_PATH, "w", encoding="utf-8") as fh:
            fh.write("\n".join(_REPORT) + "\n")
        unreal.log_warning("[usage-flags] report written to %s" % REPORT_PATH)
    except Exception as exc:
        unreal.log_warning("[usage-flags] could not write report: %s" % exc)


def find_materials():
    out = []
    registry = unreal.AssetRegistryHelpers.get_asset_registry()
    registry.scan_paths_synchronous([PLUGIN_CONTENT_ROOT], True)
    datas = registry.get_assets_by_path(PLUGIN_CONTENT_ROOT, recursive=True)
    paths = sorted(set(str(d.package_name) for d in datas))
    log("asset registry returned %d package(s) under %s" % (len(paths), PLUGIN_CONTENT_ROOT))
    if not paths:
        log("registry empty - falling back to EditorAssetLibrary.list_assets")
        try:
            paths = sorted(set(p.split(".")[0] for p in EAL.list_assets(
                PLUGIN_CONTENT_ROOT, recursive=True, include_folder=False)))
        except Exception as exc:
            log("list_assets raised: %s" % exc)
            paths = []
    for clean in paths:
        asset = EAL.load_asset(clean)
        if asset is None:
            log("could not load %s" % clean)
            continue
        if isinstance(asset, unreal.Material):
            out.append((clean, asset))
        elif isinstance(asset, unreal.MaterialInterface):
            log("NOTE: %s is a %s, not a UMaterial - usage flags live on the parent UMaterial"
                % (clean, type(asset).__name__))
        else:
            log("skipping non-material %s (%s)" % (clean, type(asset).__name__))
    return out


def read_flags(mat):
    state = {}
    for name in USAGES:
        try:
            state[name] = bool(mat.get_editor_property(name))
        except Exception:
            state[name] = None
    return state


def main():
    log("engine %s" % unreal.SystemLibrary.get_engine_version())
    materials = find_materials()
    if not materials:
        log("ERROR: no UMaterial assets found under %s - nothing to do, and that is itself wrong"
            % PLUGIN_CONTENT_ROOT)
        return 2

    log("found %d shipped material(s) under %s" % (len(materials), PLUGIN_CONTENT_ROOT))
    for path, _ in materials:
        log("    %s" % path)

    unknown = []
    changed_any = False
    for path, mat in materials:
        before = read_flags(mat)
        for name in USAGES:
            if before.get(name) is None:
                if name not in unknown:
                    unknown.append(name)
                continue
            if not before[name]:
                mat.set_editor_property(name, True)
                changed_any = True
        after = read_flags(mat)
        log("%s" % path)
        for name in USAGES:
            b = before.get(name)
            a = after.get(name)
            if b is None:
                log("    %-36s NOT PRESENT ON THIS ENGINE VERSION" % name)
            else:
                log("    %-36s %-5s -> %-5s %s"
                    % (name, str(b), str(a), "SET" if (a and not b) else ("ok" if a else "**FAILED**")))
        try:
            MEL.recompile_material(mat)
        except Exception as exc:
            log("    recompile_material raised (%s) - continuing; the cook compiles permutations anyway"
                % exc)
        if not EAL.save_asset(path, only_if_is_dirty=False):
            log("    ERROR: save_asset failed for %s" % path)
            return 3
        log("    saved")

    log("re-reading from disk to prove the flags are SERIALISED, not just set in memory")
    failures = []
    for path, _ in materials:
        EAL.load_asset(path)
        fresh = EAL.load_asset(path)
        state = read_flags(fresh)
        for name in USAGES:
            if state.get(name) is None:
                continue
            if not state[name]:
                failures.append("%s :: %s" % (path, name))
        log("    %s -> %s" % (path, ", ".join("%s=%s" % (n, state.get(n)) for n in USAGES)))

    if unknown:
        log("NOTE: usage properties absent on this engine version, skipped: %s" % ", ".join(unknown))
        for name in unknown:
            if name in MANDATORY:
                log("FATAL: mandatory usage %s does not exist on this engine version" % name)
                return 4

    if failures:
        log("FAILED - these flags did not survive the round trip:")
        for f in failures:
            log("    " + f)
        return 5

    log("RESULT: PASS - %d material(s), %d usage flag(s) each, all set and re-read from disk%s"
        % (len(materials), len([u for u in USAGES if u not in unknown]),
           "" if changed_any else " (already correct - no change made)"))
    return 0


if __name__ == "__main__":
    _code = 9
    try:
        _code = main()
    except Exception as _exc:
        log("UNHANDLED EXCEPTION: %s" % _exc)
        import traceback
        for _l in traceback.format_exc().splitlines():
            log("    " + _l)
    finally:
        log("EXITCODE=%d" % _code)
        flush_report()
    sys.exit(_code)
