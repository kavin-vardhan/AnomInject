import json
import time
from pathlib import Path

state = dict(phase=0, actions=[])

def step(context):
    plan = context['plan']
    sessions = list(Path(plan['out_dir']).glob('*/Actual_Frames'))
    frames = len(list(sessions[0].glob('frame_*.png'))) if len(sessions) == 1 else 0
    commands = []
    if state['phase'] == 0 and frames >= 120:
        unreal = context['unreal']
        world = context['pie']
        pc = unreal.GameplayStatics.get_player_controller(world,0)
        camera = unreal.GameplayStatics.get_player_camera_manager(world,0)
        geometry = dict(camera=str(camera.get_camera_location()),rotation=str(camera.get_camera_rotation()),fov=camera.get_fov_angle(),actors=[])
        for actor in unreal.GameplayStatics.get_all_actors_of_class(world,unreal.StaticMeshActor):
            component=actor.get_component_by_class(unreal.StaticMeshComponent)
            origin,extent,radius=unreal.SystemLibrary.get_component_bounds(component)
            hit=unreal.SystemLibrary.line_trace_single(world,camera.get_camera_location(),origin,unreal.TraceTypeQuery.TRACE_TYPE_QUERY1,False,[actor],unreal.DrawDebugTrace.NONE,False)
            geometry['actors'].append(dict(name=actor.get_name(),origin=str(origin),extent=str(extent),hit=str(hit),projection=str(unreal.GameplayStatics.project_world_to_screen(pc,origin))))
        Path(plan['census_actions']).with_name('geometry.json').write_text(json.dumps(geometry,indent=2))
        commands = ['IAI.Bench.VTCensusState', plan['census_command']]
    elif state['phase'] == 1 and frames >= 300:
        commands = ['IAI.Bench.VTCensusState']
    if commands:
        for command in commands:
            context['unreal'].SystemLibrary.execute_console_command(context['pie'], command)
        state['actions'].append(dict(frames_written=frames, commands=commands, time=time.time()))
        state['phase'] += 1
        Path(plan['census_actions']).write_text(json.dumps(state, indent=2))
    if time.time() - context['state']['tphase'] > 300:
        state['timeout'] = True
        Path(plan['census_actions']).write_text(json.dumps(state, indent=2))
        return True
    return state['phase'] == 2
