import json
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09205')
map_path = '/Game/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks'
report = dict(errors=[],before=[])
try:
    world = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        report['before'].append(dict(name=actor.get_name(),type=actor.get_class().get_name(),location=str(actor.get_actor_location())))
        if isinstance(actor,unreal.CameraActor):
            actor.set_editor_property('auto_activate_for_player',unreal.AutoReceiveInput.DISABLED)
        if isinstance(actor,unreal.PlayerStart):
            actor.set_actor_location(unreal.Vector(0,0,500),False,True)
            actor.set_actor_rotation(unreal.Rotator(0,0,0),True)
    camera = subsystem.spawn_actor_from_class(unreal.CameraActor,unreal.Vector(0,0,500),unreal.Rotator(0,0,0))
    camera.set_editor_property('auto_activate_for_player',unreal.AutoReceiveInput.PLAYER0)
    component = camera.get_component_by_class(unreal.CameraComponent)
    component.set_editor_property('field_of_view',90.0)
    component.set_editor_property('aspect_ratio',16/9)
    component.set_editor_property('constrain_aspect_ratio',False)
    component.set_editor_property('use_pawn_control_rotation',False)
    unreal.EditorLoadingAndSavingUtils.save_map(world,map_path)
    report['camera'] = camera.get_name()
except Exception as error:
    report['errors'].append(str(error))
finally:
    (w/'repair-camera-result.json').write_text(json.dumps(report,indent=2))
