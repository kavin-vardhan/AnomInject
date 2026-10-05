import json
from pathlib import Path
import unreal

w = Path('E:/IA_BuildCache/_r921/09205')
rows = {}
for name, path in [('single','/Game/CaptureBenchTexCorrupt/ProxyVT0921C/CB_ProxyBlurVT'),('stacks','/Game/CaptureBenchTexCorrupt/ProxyVT09205/CB_ProxyBlurStacks')]:
    world = unreal.EditorLoadingAndSavingUtils.load_map(path)
    rows[name] = []
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor):
        if isinstance(actor, (unreal.StaticMeshActor,unreal.CameraActor,unreal.PlayerStart)):
            row = dict(name=actor.get_name(),location=str(actor.get_actor_location()),rotation=str(actor.get_actor_rotation()),scale=str(actor.get_actor_scale3d()),up=str(actor.get_actor_up_vector()))
            if isinstance(actor,unreal.CameraActor):
                row['auto'] = str(actor.get_editor_property('auto_activate_for_player'))
            rows[name].append(row)
rows['positional'] = str(unreal.Rotator(90,180,180))
(w/'inspect-fixture.json').write_text(json.dumps(rows,indent=2))
