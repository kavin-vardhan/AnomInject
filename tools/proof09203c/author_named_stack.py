import json
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09203c')
root = '/Game/CaptureBenchTexCorrupt/ProxyVT0921C'
report = dict(errors=[])
try:
    assets = unreal.EditorAssetLibrary
    editing = unreal.MaterialEditingLibrary
    assert not assets.does_asset_exist(root + '/M_VTNamedStack')
    material = unreal.AssetToolsHelpers.get_asset_tools().create_asset('M_VTNamedStack', root, unreal.Material, unreal.MaterialFactoryNew())
    material.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
    samples = []
    texture = unreal.load_asset(root + '/T_VTProof')
    for index, name in enumerate(('Tex', 'TexSecond')):
        sample = editing.create_material_expression(material, unreal.MaterialExpressionTextureSampleParameter2D, -320, 180*index)
        sample.set_editor_property('parameter_name', name)
        sample.set_editor_property('texture', texture)
        sample.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_COLOR)
        samples.append(sample)
    blend = editing.create_material_expression(material, unreal.MaterialExpressionLinearInterpolate, -100, 0)
    blend.set_editor_property('const_alpha', .5)
    for sample, pin in zip(samples, ('A', 'B')):
        editing.connect_material_expressions(sample, 'RGB', blend, pin)
    editing.connect_material_property(blend, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    editing.recompile_material(material)
    assert assets.save_loaded_asset(material)
    report.update(created=material.get_path_name(), parameters=['Tex','TexSecond'], source=texture.get_path_name())
except Exception as error:
    report['errors'].append(str(error))
finally:
    (w / 'author-named-stack.json').write_text(json.dumps(report, indent=2))
    unreal.log('PROXYVT-NAMED-STACK ' + json.dumps(report))
