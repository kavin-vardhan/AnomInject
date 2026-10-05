import json
import os
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09203c')
map_path = '/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT'
report = dict(before=[], errors=[])
try:
    world = unreal.EditorLoadingAndSavingUtils.load_map(map_path)
    subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.CameraActor):
        report['before'].append(dict(name=actor.get_name(), location=str(actor.get_actor_location()), rotation=str(actor.get_actor_rotation()), auto_player=str(actor.get_editor_property('auto_activate_for_player'))))
        actor.set_editor_property('auto_activate_for_player', unreal.AutoReceiveInput.DISABLED)
    report['player_starts'] = []
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PlayerStart):
        report['player_starts'].append(dict(name=actor.get_name(), location=str(actor.get_actor_location()), rotation=str(actor.get_actor_rotation())))
        actor.set_actor_location(unreal.Vector(0, 0, 500), False, True)
        actor.set_actor_rotation(unreal.Rotator(0, 0, 0), True)
    camera = subsystem.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(0, 0, 500), unreal.Rotator(0, 0, 0))
    camera.set_actor_label('VTProof_FixedCamera')
    camera.set_editor_property('auto_activate_for_player', unreal.AutoReceiveInput.PLAYER0)
    component = camera.get_component_by_class(unreal.CameraComponent)
    component.set_editor_property('field_of_view', 90.0)
    component.set_editor_property('aspect_ratio', 16.0 / 9.0)
    component.set_editor_property('constrain_aspect_ratio', False)
    component.set_editor_property('use_pawn_control_rotation', False)
    unreal.EditorLoadingAndSavingUtils.save_map(world, map_path)
    report['camera'] = dict(name=camera.get_name(), location=str(camera.get_actor_location()), rotation=str(camera.get_actor_rotation()), auto_player=camera.get_auto_activate_player_index(), field_of_view=component.get_editor_property('field_of_view'), aspect_ratio=component.get_editor_property('aspect_ratio'))
except Exception as error:
    report['errors'].append(str(error))
finally:
    Path(os.environ['IAI_CAMERA_RESULT']).write_text(json.dumps(report, indent=2))
    unreal.log('PROXYVT-CAMERA ' + json.dumps(report))
