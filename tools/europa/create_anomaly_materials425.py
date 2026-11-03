import json
import os
import unreal

MOUNT_DIR = "/AnomalyInjector/Engine425/Materials"
CHECKER_NAME = "M_MissingTexture_Checker"
PINK_NAME = "M_CorruptedTexture_Pink"

MEL = unreal.MaterialEditingLibrary
EAL = unreal.EditorAssetLibrary
_TOOLS = unreal.AssetToolsHelpers.get_asset_tools()

USAGE_FLAGS = (
    "used_with_static_lighting",
    "used_with_skeletal_mesh",
    "used_with_instanced_static_meshes",
    "used_with_spline_meshes",
    "used_with_morph_targets",
    "used_with_clothing",
)


def _fresh_material(name):
    path = "{0}/{1}".format(MOUNT_DIR, name)
    if EAL.does_asset_exist(path):
        raise RuntimeError("Refusing to overwrite " + path)
    mat = _TOOLS.create_asset(name, MOUNT_DIR, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("two_sided", True)
    rough = _node(mat, unreal.MaterialExpressionConstant, -320, 220)
    rough.set_editor_property("r", 1.0)
    MEL.connect_material_property(rough, "", unreal.MaterialProperty.MP_ROUGHNESS)
    spec = _node(mat, unreal.MaterialExpressionConstant, -320, 320)
    spec.set_editor_property("r", 0.0)
    MEL.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR)
    for usage in USAGE_FLAGS:
        mat.set_editor_property(usage, True)
    return mat, path


def build_checker():
    mat, path = _fresh_material(CHECKER_NAME)

    uv = _node(mat, unreal.MaterialExpressionTextureCoordinate, -900, 0)
    uv.set_editor_property("u_tiling", 8.0)
    uv.set_editor_property("v_tiling", 8.0)

    mask_u = _node(mat, unreal.MaterialExpressionComponentMask, -700, -120)
    mask_u.set_editor_property("r", True)
    mask_u.set_editor_property("g", False)
    mask_u.set_editor_property("b", False)
    mask_u.set_editor_property("a", False)

    mask_v = _node(mat, unreal.MaterialExpressionComponentMask, -700, 120)
    mask_v.set_editor_property("r", False)
    mask_v.set_editor_property("g", True)
    mask_v.set_editor_property("b", False)
    mask_v.set_editor_property("a", False)

    floor_u = _node(mat, unreal.MaterialExpressionFloor, -500, -120)
    floor_v = _node(mat, unreal.MaterialExpressionFloor, -500, 120)

    add = _node(mat, unreal.MaterialExpressionAdd, -350, 0)
    half = _node(mat, unreal.MaterialExpressionMultiply, -250, 0)
    half.set_editor_property("const_b", 0.5)
    frac = _node(mat, unreal.MaterialExpressionFrac, -150, 0)
    times2 = _node(mat, unreal.MaterialExpressionMultiply, -50, 0)
    times2.set_editor_property("const_b", 2.0)

    gray = _node(mat, unreal.MaterialExpressionConstant3Vector, -300, 260)
    gray.set_editor_property("constant", unreal.LinearColor(0.5, 0.5, 0.5, 1.0))
    white = _node(mat, unreal.MaterialExpressionConstant3Vector, -300, 400)
    white.set_editor_property("constant", unreal.LinearColor(1.0, 1.0, 1.0, 1.0))
    lerp = _node(mat, unreal.MaterialExpressionLinearInterpolate, 100, 200)

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
    if not EAL.save_asset(path):
        raise RuntimeError("Material save failed: " + path)
    return path


def build_pink():
    mat, path = _fresh_material(PINK_NAME)

    pink = _node(mat, unreal.MaterialExpressionConstant3Vector, -300, 0)
    pink.set_editor_property("constant", unreal.LinearColor(1.0, 0.0, 1.0, 1.0))
    MEL.connect_material_property(pink, "", unreal.MaterialProperty.MP_BASE_COLOR)

    MEL.recompile_material(mat)
    if not EAL.save_asset(path):
        raise RuntimeError("Material save failed: " + path)
    return path



NODES = {}


def _node(mat, kind, x, y):
    value = MEL.create_material_expression(mat, kind, x, y)
    if value is None:
        raise RuntimeError('Material expression creation failed')
    NODES.setdefault(mat.get_path_name(), []).append((value, kind.__name__, x, y))
    return value


def _expected(kind, x, y):
    if kind == 'MaterialExpressionConstant':
        return dict(r=1.0 if y == 220 else 0.0)
    if kind == 'MaterialExpressionTextureCoordinate':
        return dict(u_tiling=8.0, v_tiling=8.0)
    if kind == 'MaterialExpressionComponentMask':
        return dict(r=y == -120, g=y == 120, b=False, a=False)
    if kind == 'MaterialExpressionMultiply':
        return dict(const_b=0.5 if x == -250 else 2.0)
    if kind == 'MaterialExpressionConstant3Vector':
        return dict(constant=[0.5,0.5,0.5,1.0] if y == 260 else ([1.0,1.0,1.0,1.0] if y == 400 else [1.0,0.0,1.0,1.0]))
    return {}


def _read(value, key):
    item = value.get_editor_property(key)
    if key == 'constant':
        return [item.r,item.g,item.b,item.a]
    return item


def _verify(path, nodes):
    mat = EAL.load_asset(path)
    flags = dict((key,mat.get_editor_property(key)) for key in USAGE_FLAGS)
    shading = str(mat.get_editor_property('used_shading_models'))
    checks = dict(flags)
    checks.update(two_sided=mat.get_editor_property('two_sided') is True,
        opaque=mat.get_editor_property('blend_mode') == unreal.BlendMode.BLEND_OPAQUE,
        default_lit=shading == 'Default Lit')
    values = []
    for row in nodes:
        node = unreal.load_object(None,row['path'])
        actual = dict((key,_read(node,key)) for key in row['expected'])
        values.append(dict(path=row['path'],kind=row['kind'],actual=actual,expected=row['expected']))
        checks[row['path']] = actual == row['expected']
    if not all(checks.values()):
        raise RuntimeError('Material readback failed: '+repr(checks))
    return dict(path=path,flags=flags,two_sided=True,blend_mode='BLEND_OPAQUE',
        used_shading_models=shading,checks=checks,nodes=values)


def main(receipt_path):
    project = os.path.normcase(os.path.abspath(unreal.Paths.project_dir()))
    if project != os.path.normcase(os.path.abspath('E:/IA_Fixtures/Outline425')):
        raise RuntimeError('Material authoring is fixture-only')
    if not str(unreal.SystemLibrary.get_engine_version()).startswith('4.25.'):
        raise RuntimeError('Material authoring requires 4.25')
    results = [build_checker(),build_pink()]
    materials = []
    for path in results:
        mat = EAL.load_asset(path)
        nodes = [dict(path=n.get_path_name(),kind=k,expected=_expected(k,x,y)) for n,k,x,y in NODES[mat.get_path_name()]]
        materials.append(_verify(path,nodes))
    value = dict(result='PASS',source='d7eee97:tools/create_anomaly_materials.py',materials=materials,
        shading_readback='Material.h:396-399 UsedShadingModels; Material.cpp:4303-4308',
        nanite_removed=True,project=project)
    with open(receipt_path,'w') as stream:
        json.dump(value,stream,indent=2)
    return value


def readback(author_receipt, receipt_path):
    with open(author_receipt,'r') as stream:
        authored = json.load(stream)
    materials = [_verify(row['path'],row['nodes']) for row in authored['materials']]
    value = dict(result='PASS',fresh_process=True,materials=materials)
    with open(receipt_path,'w') as stream:
        json.dump(value,stream,indent=2)
    return value
