import unreal

MOUNT_DIR = "/AnomalyInjector/Materials"
CHECKER_NAME = "M_MissingTexture_Checker"

MEL = unreal.MaterialEditingLibrary
EAL = unreal.EditorAssetLibrary
_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()


def _fresh_material(name):
    """Delete any existing asset at the path and create a fresh Lit (matte), two-sided, opaque UMaterial."""
    path = "{0}/{1}".format(MOUNT_DIR, name)
    if EAL.does_asset_exist(path):
        EAL.delete_asset(path)
    mat = _TOOLS.create_asset(name, MOUNT_DIR, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("two_sided", True)
    rough = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant, -320, 220)
    rough.set_editor_property("r", 1.0)
    MEL.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    spec = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant, -320, 320)
    spec.set_editor_property("r", 0.0)
    MEL.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)
    for usage in ("used_with_skeletal_mesh", "used_with_nanite", "used_with_instanced_static_meshes",
                  "used_with_morph_targets", "used_with_spline_meshes"):
        mat.set_editor_property(usage, True)
    return mat, path


def build_checker():
    """UV-tiled gray/white checker via floor(U)+floor(V) parity -> lerp(gray, white) -> Base Color. UV space
    keeps it deterministic per object regardless of world position / scale (unlike WorldGridMaterial)."""
    mat, path = _fresh_material(CHECKER_NAME)

    uv = MEL.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -900, 0)
    uv.set_editor_property("u_tiling", 8.0)
    uv.set_editor_property("v_tiling", 8.0)

    mask_u = MEL.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -700, -120)
    mask_u.set_editor_property("r", True)
    mask_u.set_editor_property("g", False)
    mask_u.set_editor_property("b", False)
    mask_u.set_editor_property("a", False)

    mask_v = MEL.create_material_expression(mat, unreal.MaterialExpressionComponentMask, -700, 120)
    mask_v.set_editor_property("r", False)
    mask_v.set_editor_property("g", True)
    mask_v.set_editor_property("b", False)
    mask_v.set_editor_property("a", False)

    floor_u = MEL.create_material_expression(mat, unreal.MaterialExpressionFloor, -500, -120)
    floor_v = MEL.create_material_expression(mat, unreal.MaterialExpressionFloor, -500, 120)

    add = MEL.create_material_expression(mat, unreal.MaterialExpressionAdd, -350, 0)
    half = MEL.create_material_expression(mat, unreal.MaterialExpressionMultiply, -250, 0)
    half.set_editor_property("const_b", 0.5)
    frac = MEL.create_material_expression(mat, unreal.MaterialExpressionFrac, -150, 0)
    times2 = MEL.create_material_expression(mat, unreal.MaterialExpressionMultiply, -50, 0)
    times2.set_editor_property("const_b", 2.0)

    gray = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -300, 260)
    gray.set_editor_property("constant", unreal.LinearColor(0.5, 0.5, 0.5, 1.0))
    white = MEL.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -300, 400)
    white.set_editor_property("constant", unreal.LinearColor(1.0, 1.0, 1.0, 1.0))
    lerp = MEL.create_material_expression(mat, unreal.MaterialExpressionLinearInterpolate, 100, 200)

    MEL.connect_material_expressions(uv, "", mask_u, "")
    MEL.connect_material_expressions(uv, "", mask_v, "")
    MEL.connect_material_expressions(mask_u, "", floor_u, "")
    MEL.connect_material_expressions(mask_v, "", floor_v, "")
    MEL.connect_material_expressions(floor_u, "", add, "A")
    MEL.connect_material_expressions(floor_v, "", add, "B")
    MEL.connect_material_expressions(add, "", half, "A")
    MEL.connect_material_expressions(half, "", frac, "")
    MEL.connect_material_expressions(frac, "", times2, "A")
    MEL.connect_material_expressions(gray, "", lerp, "A")
    MEL.connect_material_expressions(white, "", lerp, "B")
    MEL.connect_material_expressions(times2, "", lerp, "Alpha")
    MEL.connect_material_property(lerp, "", unreal.MaterialProperty.MP_BASE_COLOR)

    MEL.recompile_material(mat)
    EAL.save_asset(path)
    return path


def main():
    results = {"checker": build_checker()}
    unreal.log("[missing_texture] authored materials: {0}".format(results))
    return results


if __name__ == "__main__":
    main()
