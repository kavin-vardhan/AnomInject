import json
import time
from pathlib import Path

state = dict(done=False, actions=[])


def step(context):
    plan = context['plan']
    sessions = list(Path(plan['out_dir']).glob('session_*'))
    count = len(list((sessions[0] / 'Actual_Frames').glob('*.png'))) if len(sessions) == 1 else 0
    if count < 145:
        return False
    unreal = context['unreal']
    actor = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(context['pie'], unreal.StaticMeshActor) if a.get_name() == 'StaticMeshActor_77')
    action = plan['lifetime_action']
    if action == 'destroy':
        unreal.SystemLibrary.execute_console_command(context['pie'], 'IAI.Bench.ProxyVTInventory')
        actor.destroy_actor()
    elif action == 'displace':
        component = actor.get_component_by_class(unreal.StaticMeshComponent)
        component.set_material(0, unreal.load_asset('/Game/CaptureBenchTexCorrupt/ProxyVT0921C/M_VTProof'))
    elif action == 'world_end':
        unreal.SystemLibrary.execute_console_command(context['pie'], 'IAI.Bench.ProxyVTInventory')
        context['state']['phase'] = 'quit'
        context['state']['tphase'] = time.time()
        unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
    else:
        raise ValueError(action)
    state['done'] = True
    state['actions'].append(dict(action=action, written_frames=count, time=time.time()))
    Path(plan['lifetime_result']).write_text(json.dumps(state, indent=2))
    context['say']('LIFETIME ' + json.dumps(state))
    return True
