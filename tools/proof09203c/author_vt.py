import json
from pathlib import Path
import unreal

work = Path('E:/IA_BuildCache/_r921/09203c')
root = '/Game/CaptureBenchTexCorrupt/ProxyVT0921C'
assets = unreal.EditorAssetLibrary
materials = unreal.MaterialEditingLibrary
report = dict(created=[], actors=[], errors=[], map=root + '/CB_ProxyBlurVT', reference_size=[1920, 1080])


def scan_mainworld():
    reading = dict(map='/Game/StackOBot/Maps/MainWorld', actors=0, slots=0, named_bindings=0, virtual_bindings=[], errors=[])
    world = unreal.EditorLoadingAndSavingUtils.load_map(reading['map'])
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        components = actor.get_components_by_class(unreal.MeshComponent)
        if components:
            reading['actors'] += 1
        for component in components:
            for slot in range(component.get_num_materials()):
                material = component.get_material(slot)
                if not material:
                    continue
                reading['slots'] += 1
                try:
                    for parameter in materials.get_texture_parameter_names(material):
                        reading['named_bindings'] += 1
                        if isinstance(material, unreal.MaterialInstanceConstant):
                            texture = materials.get_material_instance_texture_parameter_value(material, parameter)
                        elif isinstance(material, unreal.Material):
                            texture = materials.get_material_default_texture_parameter_value(material, parameter)
                        else:
                            reading['errors'].append('unhandled material ' + material.get_path_name())
                            continue
                        if texture and texture.get_editor_property('virtual_texture_streaming'):
                            reading['virtual_bindings'].append(dict(actor=actor.get_name(), component=component.get_name(),
                                slot=slot, parameter=str(parameter), texture=texture.get_path_name()))
                except Exception as error:
                    reading['errors'].append(str(error))
    (work / 'mainworld-named-vt.json').write_text(json.dumps(reading, indent=2), encoding='utf-8')


def make_material(name, texture, virtual, constant_layer=False):
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    material = tools.create_asset(name, root, unreal.Material, unreal.MaterialFactoryNew())
    if not material:
        raise RuntimeError('material creation failed: ' + name)
    report['created'].append(material.get_path_name())
    material.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
    sample = materials.create_material_expression(material, unreal.MaterialExpressionTextureSampleParameter2D, -320, 0)
    sample.set_editor_property('parameter_name', 'Tex')
    sample.set_editor_property('texture', texture)
    sample.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_COLOR if virtual else unreal.MaterialSamplerType.SAMPLERTYPE_COLOR)
    result = sample
    if constant_layer:
        other = materials.create_material_expression(material, unreal.MaterialExpressionTextureSample, -320, 180)
        other.set_editor_property('texture', texture)
        other.set_editor_property('sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_VIRTUAL_COLOR)
        add = materials.create_material_expression(material, unreal.MaterialExpressionAdd, -100, 0)
        materials.connect_material_expressions(sample, 'RGB', add, 'A')
        materials.connect_material_expressions(other, 'RGB', add, 'B')
        result = add
    materials.connect_material_property(result, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    materials.recompile_material(material)
    assets.save_loaded_asset(material)
    return material


def main():
    if assets.does_directory_exist(root):
        raise RuntimeError('fixture destination exists; no overwrite')
    tools = unreal.AssetToolsHelpers.get_asset_tools()
    task = unreal.AssetImportTask()
    task.filename = str(work / 'T_VTProof.png')
    task.destination_path = root
    task.destination_name = 'T_VTProof'
    task.automated = True
    task.save = True
    tools.import_asset_tasks([task])
    texture = unreal.load_asset(task.imported_object_paths[0])
    texture.set_editor_property('lod_bias', 0)
    texture.set_editor_property('num_cinematic_mip_levels', 0)
    texture.set_editor_property('lod_group', unreal.TextureGroup.TEXTUREGROUP_WORLD)
    texture.set_editor_property('never_stream', False)
    texture.set_editor_property('mip_gen_settings', unreal.TextureMipGenSettings.TMGS_SIMPLE_AVERAGE)
    texture.set_editor_property('virtual_texture_streaming', True)
    assets.save_loaded_asset(texture)
    report['created'].append(texture.get_path_name())
    ordinary = assets.duplicate_asset(root + '/T_VTProof', root + '/T_OrdinaryProof')
    ordinary.set_editor_property('virtual_texture_streaming', False)
    assets.save_loaded_asset(ordinary)
    report['created'].append(ordinary.get_path_name())
    vt_mat = make_material('M_VTProof', texture, True)
    ordinary_mat = make_material('M_OrdinaryProof', ordinary, False)
    negative_mat = make_material('M_VTConstantLayer', texture, True, True)
    if not assets.duplicate_asset('/Game/CaptureBenchTexCorrupt/CB_TexCorruptLevel', report['map']):
        raise RuntimeError('fixture map duplication failed')
    report['created'].append(report['map'])
    world = unreal.EditorLoadingAndSavingUtils.load_map(report['map'])
    actors = list(unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor))
    target = next(actor for actor in actors if actor.get_name() == 'StaticMeshActor_77')
    rotation = target.get_actor_rotation()
    report['plane_rotation'] = str(rotation)
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for actor in actors:
        if actor != target and actor.get_component_by_class(unreal.StaticMeshComponent):
            subsystem.destroy_actor(actor)
    mesh = unreal.load_asset('/Engine/BasicShapes/Plane')
    layout = [('target', 704, 540, 1280, 320, vt_mat), ('co_user', 1580, 320, 256, 256, vt_mat), ('ordinary', 1580, 760, 256, 256, ordinary_mat), ('constant_layer', 2600, 540, 512, 512, negative_mat)]
    for index, (role, x, y, width, height, material) in enumerate(layout):
        actor = target if index == 0 else subsystem.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, 0))
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        component.set_static_mesh(mesh)
        component.set_material(0, material)
        actor.set_actor_rotation(rotation, True)
        actor.set_actor_location(unreal.Vector(1000, (x-960)*1000/960, 500+(540-y)*1000/960), False, True)
        actor.set_actor_scale3d(unreal.Vector(height*1000/960/100, width*1000/960/100, 1))
        actor.set_actor_label('VTProof_' + role)
        report['actors'].append(dict(name=actor.get_name(), role=role, box=[x-width/2, y-height/2, width, height], material=material.get_path_name()))
    unreal.EditorLoadingAndSavingUtils.save_map(world, report['map'])
    report['texture'] = texture.get_path_name()
    report['ordinary_texture'] = ordinary.get_path_name()
    report['virtual_flag'] = texture.get_editor_property('virtual_texture_streaming')


try:
    main()
except Exception as error:
    report['errors'].append(str(error))
finally:
    (work / 'author_vt.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
    unreal.log('PROXYVT-AUTHOR ' + json.dumps(report))
