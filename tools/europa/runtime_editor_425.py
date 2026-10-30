from __future__ import print_function
import json
import os
import sys
import time

STATE = {}
HANDLE = None
BINDINGS = [
    'EditorLevelLibrary.get_editor_world','EditorLevelLibrary.get_game_world',
    'EditorLevelLibrary.load_level','EditorLevelLibrary.get_all_level_actors',
    'EditorStaticMeshLibrary.get_lod_count','SystemLibrary.execute_console_command',
    'register_slate_post_tick_callback','unregister_slate_post_tick_callback',
    'ObjectIterator','World','Paths.project_dir','load_asset',
    'Object.get_editor_property','Object.get_path_name','Actor.get_actor_location',
    'Actor.get_actor_scale3d','Actor.get_actor_rotation','World.get_world_settings',
    'PostProcessVolume','PostProcessSettings','PointLight','CameraActor','PlayerStart',
    'StaticMeshActor','GameModeBase']


def record():
    with open(STATE['receipt'], 'w') as stream:
        json.dump(dict((k,v) for k,v in STATE.items() if k not in ('seen_worlds',)),stream,indent=2)


def console(world, command):
    import unreal
    unreal.SystemLibrary.execute_console_command(world, command)
    STATE.setdefault('commands', []).append(command)


def finish(result, reason=None):
    import unreal
    global HANDLE
    STATE['result'] = result
    if reason:
        STATE['reason'] = reason
    STATE['ended'] = time.time()
    STATE.setdefault('commands', []).append('QUIT_EDITOR')
    record()
    if HANDLE is not None:
        unreal.unregister_slate_post_tick_callback(HANDLE)
        HANDLE = None
    unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(), 'QUIT_EDITOR')


def binding_check():
    import unreal
    rows = []
    for name in BINDINGS:
        obj = unreal
        try:
            for part in name.split('.'):
                obj = getattr(obj, part)
            present = callable(obj)
        except AttributeError:
            present = False
        rows.append(dict(binding=name,present=present))
    settings=unreal.PostProcessSettings()
    for key in ('override_auto_exposure_min_brightness','override_auto_exposure_max_brightness',
                'auto_exposure_min_brightness','auto_exposure_max_brightness',
                'override_motion_blur_amount','motion_blur_amount'):
        try:
            settings.get_editor_property(key)
            present=True
        except Exception:
            present=False
        rows.append(dict(binding='PostProcessSettings.'+key,present=present))
    return rows


def readback(map_name):
    import unreal
    ell = unreal.EditorLevelLibrary
    if not ell.load_level('/Game/EuropaPhaseA/'+map_name):
        raise RuntimeError('Cold map load failed: '+map_name)
    actors = ell.get_all_level_actors()
    tagged = {}
    for actor in actors:
        for tag in actor.get_editor_property('tags'):
            if str(tag).startswith('EA_'):
                tagged[str(tag)] = actor
    expected = ['EA_Floor','EA_HideCube','EA_BlinkCube','EA_LOD','EA_Light','EA_Camera']
    if sorted(tagged) != sorted(expected):
        raise RuntimeError('Saved fixture tags differ')
    lod = unreal.load_asset('/Game/EuropaPhaseA/SM_EA_TwoLod')
    result = dict(map=map_name,tags=sorted(tagged),lod_count=unreal.EditorStaticMeshLibrary.get_lod_count(lod),
        intensity=tagged['EA_Light'].light_component.get_editor_property('intensity'),
        attenuation_radius=tagged['EA_Light'].light_component.get_editor_property('attenuation_radius'),
        cast_shadows=tagged['EA_Light'].light_component.get_editor_property('cast_shadows'),
        field_of_view=tagged['EA_Camera'].camera_component.get_editor_property('field_of_view'),
        transforms={name:dict(location=[getattr(actor.get_actor_location(),k) for k in ('x','y','z')],
                              scale=[getattr(actor.get_actor_scale3d(),k) for k in ('x','y','z')])
                    for name,actor in tagged.items()})
    expected_transforms={
        'EA_Floor':([700,0,-50],[18,18,1]),'EA_HideCube':([600,-260,110],[2,2,2]),
        'EA_BlinkCube':([600,260,110],[2,2,2]),'EA_LOD':([800,0,160],[3,3,3]),
        'EA_Light':([450,0,650],[1,1,1]),'EA_Camera':([-550,0,480],[1,1,1])}
    for name,(position,scale) in expected_transforms.items():
        if result['transforms'][name] != dict(location=position,scale=scale):
            raise RuntimeError('Saved transform differs: '+name)
    rotation=tagged['EA_Camera'].get_actor_rotation()
    result['camera_rotation']=[rotation.pitch,rotation.yaw,rotation.roll]
    if any(abs(a-b)>0.001 for a,b in zip(result['camera_rotation'],[-12,0,0])):
        raise RuntimeError('Saved camera rotation differs')
    pp = [a for a in actors if isinstance(a,unreal.PostProcessVolume)]
    if len(pp) != 1:
        raise RuntimeError('Expected one saved post-process volume')
    settings = pp[0].get_editor_property('settings')
    wanted = [('override_auto_exposure_min_brightness',True),('override_auto_exposure_max_brightness',True),
              ('auto_exposure_min_brightness',1.0),('auto_exposure_max_brightness',1.0),
              ('override_motion_blur_amount',True),('motion_blur_amount',0.0)]
    result['postprocess'] = {key:settings.get_editor_property(key) for key,value in wanted}
    result['unbound'] = pp[0].get_editor_property('unbound')
    if (result['lod_count'] < 2 or result['intensity'] != 5000.0 or result['attenuation_radius'] != 1800.0
        or not result['cast_shadows'] or result['field_of_view'] != 50.0 or not result['unbound']
        or any(result['postprocess'][key] != value for key,value in wanted)):
        raise RuntimeError('Saved fixture property differs from A5 recipe')
    return result


def tick(delta):
    import unreal
    try:
        STATE['ticks'] += 1
        if STATE['ticks'] < 10:
            return
        if STATE['leg'] == 'Smoke':
            STATE['bindings'] = binding_check()
            if not all(row['present'] for row in STATE['bindings']):
                raise RuntimeError('Missing native runtime binding')
            STATE['module_alive'] = sys.modules.get(__name__) is not None
            STATE.update(loads=0,creates=0,saves=0)
            finish('SMOKE_PASS')
        elif STATE['leg'] == 'Load':
            STATE['readback'] = [readback(name) for name in ('EA_Core','EA_Travel')]
            STATE.update(saves=0,creates=0)
            finish('DRIVER_PASS')
        else:
            elapsed = time.time()-STATE['started']
            if STATE['cycle'] < 3 and elapsed >= STATE['next_cycle']:
                console(unreal.EditorLevelLibrary.get_editor_world(), 'Automation RunTests System.Maps.PIE')
                STATE['cycle'] += 1
                STATE['next_cycle'] = elapsed+20.0
                STATE['seen_worlds'] = []
                record()
            worlds = [w for w in unreal.ObjectIterator(unreal.World) if 'UEDPIE_' in w.get_path_name()]
            if len(worlds) >= 2:
                world = unreal.EditorLevelLibrary.get_game_world()
                if world and world.get_path_name() not in STATE['seen_worlds']:
                    name = world.get_path_name()
                    if 'EA_Core' in name:
                        console(world, 'Europa.Probe script '+STATE['native_script'].replace('\\','/'))
                        STATE['seen_worlds'].append(name)
                        STATE.setdefault('two_world_observations',[]).append([w.get_path_name() for w in worlds])
                        record()
            if STATE['cycle'] == 3 and elapsed >= STATE['next_cycle'] and not worlds and not STATE.get('finish_tick'):
                if len(STATE.get('two_world_observations',[])) < 3:
                    raise RuntimeError('Three two-world PIE cycles were not observed')
                console(unreal.EditorLevelLibrary.get_editor_world(), 'Europa.Probe preview')
                STATE['leg'] = 'WorldsFinishing'
                STATE['finish_tick'] = STATE['ticks']+10
            if STATE.get('finish_tick') and STATE['ticks'] >= STATE['finish_tick']:
                finish('DRIVER_PASS')
    except Exception as error:
        finish('DRIVER_FAIL',type(error).__name__+': '+str(error))


def start(leg, receipt, probe, native_script):
    import unreal
    global STATE,HANDLE
    if leg not in ('Smoke','Load','WorldsPIE'):
        raise RuntimeError('Unknown editor runtime leg')
    if os.path.normcase(os.path.abspath(unreal.Paths.project_dir())) != os.path.normcase(os.path.abspath('D:/IA_Europa/StylizedParisStreet')):
        raise RuntimeError('Wrong runtime host')
    if os.path.exists(receipt):
        raise RuntimeError('Driver receipt already exists')
    STATE = dict(result='RUNNING',leg=leg,receipt=receipt,probe=probe,native_script=native_script,
                 started=time.time(),ticks=0,cycle=0,next_cycle=2.0,seen_worlds=[])
    record()
    HANDLE = unreal.register_slate_post_tick_callback(tick)
