import json
from pathlib import Path
import unreal

w=Path('E:/IA_BuildCache/_r921/09205')
root='/Game/CaptureBenchTexCorrupt/ProxyVT09205'
report=dict(lights=[],textures=[],errors=[])
try:
    world=unreal.EditorLoadingAndSavingUtils.load_map(root+'/CB_ProxyBlurStacks')
    unreal.SystemLibrary.execute_console_command(world,'Editor.AsyncAssetCompilationFinishAll')
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.Actor):
        if isinstance(actor,unreal.DirectionalLight):
            report['lights'].append(dict(name=actor.get_name(),before=actor.light_component.get_editor_property('intensity'),after=.03))
            actor.light_component.set_editor_property('intensity',.03)
        if isinstance(actor,unreal.PointLight):
            report['lights'].append(dict(name=actor.get_name(),before=actor.point_light_component.get_editor_property('intensity'),after=60))
            actor.point_light_component.set_editor_property('intensity',60.)
    for name,bias in [('Color',0),('Normal',1),('Roughness',2)]:
        for prefix in ('T_Stack','T_Ordinary'):
            texture=unreal.load_asset(root+'/'+prefix+name)
            report['textures'].append(dict(path=texture.get_path_name(),before=texture.get_editor_property('lod_bias'),after=bias))
            texture.set_editor_property('lod_bias',bias)
            unreal.EditorAssetLibrary.save_loaded_asset(texture)
    unreal.SystemLibrary.execute_console_command(world,'Editor.AsyncAssetCompilationFinishAll')
    unreal.EditorLoadingAndSavingUtils.save_map(world,root+'/CB_ProxyBlurStacks')
except Exception as e:
    report['errors'].append(str(e))
finally:
    (w/'part-b-fixture-light.json').write_text(json.dumps(report,indent=2))
    unreal.log('VTSTACKS-LIGHT '+json.dumps(report))
