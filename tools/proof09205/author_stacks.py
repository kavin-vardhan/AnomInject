import json
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09205')
root = '/Game/CaptureBenchTexCorrupt/ProxyVT09205'
assets = unreal.EditorAssetLibrary
materials = unreal.MaterialEditingLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
report = dict(created=[], actors=[], errors=[], map=root + '/CB_ProxyBlurStacks', reference_size=[1920, 1080])

def make_material(name, textures, virtual=True, unnamed=False):
    result = tools.create_asset(name, root, unreal.Material, unreal.MaterialFactoryNew())
    assert result
    report['created'].append(result.get_path_name())
    result.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_DEFAULT_LIT)
    uv = materials.create_material_expression(result, unreal.MaterialExpressionTextureCoordinate, -650, 0)
    for index, (parameter, texture, prop, sampler) in enumerate(zip(['Color', 'Normal', 'Roughness'], textures,
        [unreal.MaterialProperty.MP_BASE_COLOR, unreal.MaterialProperty.MP_NORMAL, unreal.MaterialProperty.MP_ROUGHNESS],
        [unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_COLOR, unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_NORMAL, unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_MASKS] if virtual else
        [unreal.MaterialSamplerType.SAMPLERTYPE_COLOR, unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL, unreal.MaterialSamplerType.SAMPLERTYPE_MASKS])):
        node_class = unreal.MaterialExpressionTextureSample if unnamed and index == 1 else unreal.MaterialExpressionTextureSampleParameter2D
        sample = materials.create_material_expression(result, node_class, -350, index * 180)
        if node_class == unreal.MaterialExpressionTextureSampleParameter2D:
            sample.set_editor_property('parameter_name', parameter)
        sample.set_editor_property('texture', texture)
        sample.set_editor_property('sampler_type', sampler)
        materials.connect_material_expressions(uv, '', sample, 'Coordinates')
        materials.connect_material_property(sample, 'R' if index == 2 else 'RGB', prop)
    materials.recompile_material(result)
    assets.save_loaded_asset(result)
    return result

try:
    assert not assets.does_directory_exist(root)
    textures = []
    ordinary_textures = []
    for name, compression, srgb in [('Color', unreal.TextureCompressionSettings.TC_DEFAULT, True), ('Normal', unreal.TextureCompressionSettings.TC_NORMALMAP, False), ('Roughness', unreal.TextureCompressionSettings.TC_MASKS, False)]:
        task = unreal.AssetImportTask()
        task.filename = str(w / ('T_Stack' + name + '.png'))
        task.destination_path = root
        task.destination_name = 'T_Stack' + name
        task.automated = True
        task.save = True
        tools.import_asset_tasks([task])
        texture = unreal.load_asset(task.imported_object_paths[0])
        texture.set_editor_property('compression_settings', compression)
        texture.set_editor_property('srgb', srgb)
        texture.set_editor_property('lod_bias', 0)
        texture.set_editor_property('num_cinematic_mip_levels', 0)
        texture.set_editor_property('never_stream', False)
        texture.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_SIMPLE_AVERAGE)
        texture.set_editor_property('virtual_texture_streaming', True)
        assets.save_loaded_asset(texture)
        textures.append(texture)
        report['created'].append(texture.get_path_name())
        ordinary = assets.duplicate_asset(root + '/T_Stack' + name, root + '/T_Ordinary' + name)
        ordinary.set_editor_property('virtual_texture_streaming', False)
        assets.save_loaded_asset(ordinary)
        ordinary_textures.append(ordinary)
        report['created'].append(ordinary.get_path_name())
    stack_mat = make_material('M_Stack', textures)
    ordinary_mat = make_material('M_Ordinary', ordinary_textures, False)
    unnamed_mat = make_material('M_Unnamed', textures, True, True)
    assert assets.duplicate_asset('/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT', report['map'])
    report['created'].append(report['map'])
    world = unreal.EditorLoadingAndSavingUtils.load_map(report['map'])
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actors = list(unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor))
    target = next(actor for actor in actors if actor.get_name() == 'StaticMeshActor_77')
    for actor in actors:
        if isinstance(actor, unreal.CameraActor):
            continue
        if actor != target and (actor.get_component_by_class(unreal.MeshComponent) or actor.get_component_by_class(unreal.LightComponent)):
            subsystem.destroy_actor(actor)
    mesh = unreal.load_asset('/Engine/BasicShapes/Plane')
    for index, (role, x, y, width, height, mat) in enumerate([
        ('target', 704, 540, 1280, 320, stack_mat), ('co_user', 1580, 320, 256, 256, stack_mat),
        ('ordinary', 1580, 760, 256, 256, ordinary_mat), ('unnamed', 2600, 540, 512, 512, unnamed_mat)]):
        actor = target if index == 0 else subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        component.set_static_mesh(mesh)
        component.set_material(0, mat)
        actor.set_actor_rotation(unreal.Rotator(pitch=90, yaw=180, roll=180), True)
        actor.set_actor_location(unreal.Vector(1000, (x-960)*1000/960, 500+(540-y)*1000/960), False, True)
        actor.set_actor_scale3d(unreal.Vector(height*1000/960/100, width*1000/960/100, 1))
        actor.set_actor_label('VTStacks_' + role)
        report['actors'].append(dict(name=actor.get_name(), role=role, box=[x-width/2, y-height/2, width, height], material=mat.get_path_name()))
    sun = subsystem.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 800), unreal.Rotator(pitch=-15, yaw=15, roll=0))
    sun.light_component.set_editor_property('intensity', 1.0)
    sun.light_component.set_editor_property('cast_shadows', False)
    light = subsystem.spawn_actor_from_class(unreal.PointLight, unreal.Vector(200, -100, 650))
    light.point_light_component.set_editor_property('intensity', 2000.0)
    light.point_light_component.set_editor_property('attenuation_radius', 5000.0)
    light.point_light_component.set_editor_property('source_radius', 150.0)
    light.point_light_component.set_editor_property('cast_shadows', False)
    unreal.EditorLoadingAndSavingUtils.save_map(world, report['map'])
    report['textures'] = [texture.get_path_name() for texture in textures]
except Exception as error:
    report['errors'].append(str(error))
finally:
    (w / 'author_stacks.json').write_text(json.dumps(report, indent=2))
    unreal.log('VTSTACKS-AUTHOR ' + json.dumps(report))
