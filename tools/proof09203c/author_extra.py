import json
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09203c')
root = '/Game/CaptureBenchTexCorrupt/ProxyVT0921C'
assets = unreal.EditorAssetLibrary
materials = unreal.MaterialEditingLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
report = dict(created=[], actors=[], errors=[])

def material(name, vt, ordinary=None):
    assert not assets.does_asset_exist(root + '/' + name)
    result = tools.create_asset(name, root, unreal.Material, unreal.MaterialFactoryNew())
    report['created'].append(result.get_path_name())
    result.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
    sample = materials.create_material_expression(result, unreal.MaterialExpressionTextureSampleParameter2D, -320, 0)
    sample.set_editor_property('parameter_name', 'Tex')
    sample.set_editor_property('texture', vt)
    sample.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_COLOR)
    output = sample
    if ordinary:
        sample2 = materials.create_material_expression(result, unreal.MaterialExpressionTextureSampleParameter2D, -320, 180)
        sample2.set_editor_property('parameter_name', 'OrdTex')
        sample2.set_editor_property('texture', ordinary)
        sample2.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
        blend = materials.create_material_expression(result, unreal.MaterialExpressionLinearInterpolate, -100, 0)
        blend.set_editor_property('const_alpha', .5)
        materials.connect_material_expressions(sample, 'RGB', blend, 'A')
        materials.connect_material_expressions(sample2, 'RGB', blend, 'B')
        output = blend
    materials.connect_material_property(output, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    materials.recompile_material(result)
    assets.save_loaded_asset(result)
    return result

try:
    task = unreal.AssetImportTask()
    task.filename = str(w / 'T_UniformVT.png')
    task.destination_path = root
    task.destination_name = 'T_UniformVT'
    task.automated = True
    task.save = True
    assert not assets.does_asset_exist(root + '/T_UniformVT')
    tools.import_asset_tasks([task])
    flat = unreal.load_asset(task.imported_object_paths[0])
    flat.set_editor_property('lod_bias', 0)
    flat.set_editor_property('num_cinematic_mip_levels', 0)
    flat.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_SIMPLE_AVERAGE)
    flat.set_editor_property('virtual_texture_streaming', True)
    assets.save_loaded_asset(flat)
    report['created'].append(flat.get_path_name())
    flat_mat = material('M_UniformVT', flat)
    mixed = material('M_MixedProof', unreal.load_asset(root + '/T_VTProof'), unreal.load_asset(root + '/T_OrdinaryProof'))
    map_path = root + '/CB_ProxyBlurVT'
    world = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    world.get_world_settings().set_editor_property('default_game_mode', unreal.GameModeBase)
    report['game_mode'] = str(world.get_world_settings().get_editor_property('default_game_mode'))
    report['removed_skeletal_actors'] = []
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        if actor.get_component_by_class(unreal.SkeletalMeshComponent):
            report['removed_skeletal_actors'].append(actor.get_name())
            subsystem.destroy_actor(actor)
    for index, (role, mat) in enumerate([('uniform', flat_mat), ('mixed', mixed)]):
        actor = subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(1000, 2500, 500 + 600 * index))
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        component.set_static_mesh(unreal.load_asset('/Engine/BasicShapes/Plane'))
        component.set_material(0, mat)
        actor.set_actor_rotation(unreal.Rotator(90, 180, 180), True)
        actor.set_actor_scale3d(unreal.Vector(1, 1, 1))
        actor.set_actor_label('VTProof_' + role)
        report['actors'].append(dict(name=actor.get_name(), role=role, material=mat.get_path_name()))
    unreal.EditorLoadingAndSavingUtils.save_map(world, map_path)
    report['modified_map'] = map_path
except Exception as error:
    report['errors'].append(str(error))
finally:
    (w / 'author-extra.json').write_text(json.dumps(report, indent=2))
    unreal.log('PROXYVT-EXTRA ' + json.dumps(report))
