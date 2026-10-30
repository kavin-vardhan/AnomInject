from __future__ import print_function
import json
import os
import sys
import time

STATE = {}
HANDLE = None
BUSY = False
BINDINGS = [
    'EditorLevelLibrary.get_editor_world','EditorLevelLibrary.get_game_world',
    'EditorLevelLibrary.load_level','EditorLevelLibrary.get_all_level_actors',
    'EditorStaticMeshLibrary.get_lod_count','SystemLibrary.execute_console_command',
    'register_slate_post_tick_callback','unregister_slate_post_tick_callback',
    'ObjectIterator','World','Paths.project_dir','load_asset',
    'Object.get_editor_property','Object.get_path_name','Actor.get_actor_location','Actor.get_actor_label',
    'Actor.get_actor_scale3d','Actor.get_actor_rotation','World.get_world_settings',
    'PostProcessVolume','PostProcessSettings','PointLight','CameraActor','PlayerStart',
    'StaticMeshActor','GameModeBase','ComponentMobility']


def record():
    with open(STATE['receipt'], 'w') as stream:
        json.dump(dict((k,v) for k,v in STATE.items() if k not in ('seen_worlds',)),stream,indent=2)


def console(world, command):
    import unreal
    if STATE.get('terminal'):
        return
    unreal.SystemLibrary.execute_console_command(world, command)
    STATE.setdefault('commands', []).append(command)


def finish(result, reason=None):
    import unreal
    global HANDLE
    if STATE.get('terminal'):
        return
    STATE['terminal'] = True
    STATE['result'] = result
    if reason:
        STATE['reason'] = reason
    STATE['ended'] = time.time()
    STATE['exit_requests'] = 1
    STATE.setdefault('commands', []).append('QUIT_EDITOR')
    record()
    if HANDLE is not None:
        handle = HANDLE
        HANDLE = None
        unreal.unregister_slate_post_tick_callback(handle)
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
    result=dict(map=map_name,checks=[])
    STATE.setdefault('readback',[]).append(result)
    def check(name,actual,expected,passed=None):
        row=dict(name=name,actual=actual,expected=expected,passed=(actual==expected if passed is None else passed))
        result['checks'].append(row)
        record()
        return row['passed']
    loaded=bool(ell.load_level('/Game/EuropaPhaseA/'+map_name))
    if not check('map_loaded',loaded,True):
        raise RuntimeError('Cold map load failed: '+map_name)
    actors = ell.get_all_level_actors()
    tagged = {}
    for actor in actors:
        for tag in actor.get_editor_property('tags'):
            if str(tag).startswith('EA_'):
                tagged[str(tag)] = actor
    expected = ['EA_Floor','EA_HideCube','EA_BlinkCube','EA_LOD','EA_Light','EA_Camera']
    if not check('tags',sorted(tagged),sorted(expected)):
        raise RuntimeError('Saved fixture tags differ')
    lod = unreal.load_asset('/Game/EuropaPhaseA/SM_EA_TwoLod')
    result.update(tags=sorted(tagged),lod_count=unreal.EditorStaticMeshLibrary.get_lod_count(lod),
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
        check(name+'.transform',result['transforms'][name],dict(location=position,scale=scale))
        check(name+'.label',tagged[name].get_actor_label(),name)
    rotation=tagged['EA_Camera'].get_actor_rotation()
    result['camera_rotation']=[rotation.pitch,rotation.yaw,rotation.roll]
    normalized=[(value+180.0)%360.0-180.0 for value in result['camera_rotation']]
    result['camera_rotation_normalized']=normalized
    check('camera_rotation',dict(raw=result['camera_rotation'],normalized=normalized),
        dict(pitch=-12.0,yaw=0.0,roll=0.0,tolerance_degrees=0.01),
        all(abs((actual-wanted+180.0)%360.0-180.0)<=0.01 for actual,wanted in zip(normalized,[-12,0,0])))
    pp = [a for a in actors if isinstance(a,unreal.PostProcessVolume)]
    if not check('postprocess_count',len(pp),1):
        raise RuntimeError('Expected one saved post-process volume')
    settings = pp[0].get_editor_property('settings')
    position=pp[0].get_actor_location()
    result['postprocess_location']=[position.x,position.y,position.z]
    check('postprocess_location',result['postprocess_location'],[0,0,0])
    wanted = [('override_auto_exposure_min_brightness',True),('override_auto_exposure_max_brightness',True),
              ('auto_exposure_min_brightness',1.0),('auto_exposure_max_brightness',1.0),
              ('override_motion_blur_amount',True),('motion_blur_amount',0.0)]
    result['postprocess'] = {key:settings.get_editor_property(key) for key,value in wanted}
    result['unbound'] = pp[0].get_editor_property('unbound')
    check('lod_count',result['lod_count'],'>=2',result['lod_count']>=2)
    for name,value in [('intensity',5000.0),('attenuation_radius',1800.0),('cast_shadows',True),
                       ('field_of_view',50.0),('unbound',True)]:
        check(name,result[name],value)
    for name,value in wanted:check('postprocess.'+name,result['postprocess'][name],value)
    result['meshes']={}
    for name in ('EA_Floor','EA_HideCube','EA_BlinkCube','EA_LOD'):
        component=tagged[name].static_mesh_component
        mesh=component.get_editor_property('static_mesh')
        path=mesh.get_path_name() if mesh else None
        result['meshes'][name]=path
        wanted_path='/Game/EuropaPhaseA/SM_EA_TwoLod.SM_EA_TwoLod' if name=='EA_LOD' else '/Engine/BasicShapes/Cube.Cube'
        check(name+'.mesh',path,wanted_path)
        mobility=component.get_editor_property('mobility')
        check(name+'.mobility',str(mobility),str(unreal.ComponentMobility.MOVABLE),mobility==unreal.ComponentMobility.MOVABLE)
    mobility=tagged['EA_Light'].light_component.get_editor_property('mobility')
    check('EA_Light.mobility',str(mobility),str(unreal.ComponentMobility.MOVABLE),mobility==unreal.ComponentMobility.MOVABLE)
    mode=ell.get_editor_world().get_world_settings().get_editor_property('default_game_mode')
    result['default_game_mode']=mode.get_path_name() if mode else None
    check('default_game_mode',result['default_game_mode'],'/Script/Engine.GameModeBase')
    players=[a for a in actors if isinstance(a,unreal.PlayerStart)]
    if check('player_start_count',len(players),1):
        position=players[0].get_actor_location();angle=players[0].get_actor_rotation()
        result['player_start']=dict(location=[position.x,position.y,position.z],rotation=[angle.pitch,angle.yaw,angle.roll])
        check('player_start.location',result['player_start']['location'],[-550,0,500])
        check('player_start.rotation',result['player_start']['rotation'],[0,0,0],
            all(abs((a+180.0)%360.0-180.0)<=0.01 for a in result['player_start']['rotation']))
    failed=[row['name'] for row in result['checks'] if not row['passed']]
    result['result']='FAIL' if failed else 'PASS'
    record()
    if failed:
        raise RuntimeError('Saved fixture values differ: '+', '.join(failed))
    return result


def tick(delta):
    global BUSY
    if STATE.get('terminal'):
        return
    if BUSY:
        STATE['reentrant_ticks_ignored']=STATE.get('reentrant_ticks_ignored',0)+1
        return
    BUSY=True
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
            for name in ('EA_Core','EA_Travel'):
                readback(name)
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
    finally:
        BUSY=False


def start(leg, receipt, probe, native_script):
    import unreal
    global STATE,HANDLE,BUSY
    if leg not in ('Smoke','Load','WorldsPIE'):
        raise RuntimeError('Unknown editor runtime leg')
    if os.path.normcase(os.path.abspath(unreal.Paths.project_dir())) != os.path.normcase(os.path.abspath('D:/IA_Europa/StylizedParisStreet')):
        raise RuntimeError('Wrong runtime host')
    if os.path.exists(receipt):
        raise RuntimeError('Driver receipt already exists')
    STATE = dict(result='RUNNING',leg=leg,receipt=receipt,probe=probe,native_script=native_script,
                 started=time.time(),ticks=0,cycle=0,next_cycle=2.0,seen_worlds=[],terminal=False,exit_requests=0)
    BUSY=False
    record()
    HANDLE = unreal.register_slate_post_tick_callback(tick)
