from __future__ import print_function
import argparse
try:
    import imp
except ImportError:
    imp=None
import json
import os
import runpy
import sys
import types


class Value(object):
    def __init__(self,x=0,y=0,z=0):
        self.x,self.y,self.z=x,y,z
        self.pitch,self.yaw,self.roll=x,y,z


class Object(object):
    def __init__(self,props=None,path='stub'):
        self.props=props or {};self.path=path
    def get_editor_property(self,key):
        CALLS.append(['get',self.path,key])
        return self.props[key]
    def get_path_name(self):return self.path


class Actor(Object):
    def get_actor_location(self):return self.location
    def get_actor_scale3d(self):return self.scale
    def get_actor_rotation(self):return Value(*ROTATION) if self.path=='EA_Camera' else Value()
    def get_actor_label(self):return self.path


class World(Object):
    def get_world_settings(self):return Object({'default_game_mode':Object(path='/Script/Engine.GameModeBase')})


class PostProcessVolume(Actor):pass
class PointLight(Actor):pass
class CameraActor(Actor):pass
class PlayerStart(Actor):pass
class StaticMeshActor(Actor):pass
class GameModeBase(Object):pass
class ComponentMobility(object):MOVABLE='MOVABLE'


def Rotator(roll=0.0,pitch=0.0,yaw=0.0):return Value(pitch,yaw,roll)


POST={'override_auto_exposure_min_brightness':True,'override_auto_exposure_max_brightness':True,
      'auto_exposure_min_brightness':1.0,'auto_exposure_max_brightness':1.0,
      'override_motion_blur_amount':True,'motion_blur_amount':0.0}


class PostProcessSettings(Object):
    def __init__(self):Object.__init__(self,POST.copy(),'postprocess')


CALLS=[]
CLOCK=[0.0]
CYCLE=[0,-100.0]
CALLBACK=[None]
ROTATION=[-12,0,0]
REENTER=[False]
RAISE=[False]
WORLD=World(path='EditorWorld')


class EditorLevelLibrary(object):
    @staticmethod
    def get_editor_world():return WORLD
    @staticmethod
    def get_game_world():return World(path='UEDPIE_%d_EA_Core'%CYCLE[0])
    @staticmethod
    def load_level(path):
        CALLS.append(['load_level',path])
        if REENTER[0] and CALLBACK[0] and len([c for c in CALLS if c[0]=='load_level'])<3:
            CALLBACK[0](.1)
        return True
    @staticmethod
    def get_all_level_actors():
        if RAISE[0]:raise RuntimeError('Synthetic repeating callback failure')
        rows=[]
        positions={'EA_Floor':([700,0,-50],[18,18,1]),'EA_HideCube':([600,-260,110],[2,2,2]),
            'EA_BlinkCube':([600,260,110],[2,2,2]),'EA_LOD':([800,0,160],[3,3,3]),
            'EA_Light':([450,0,650],[1,1,1]),'EA_Camera':([-550,0,480],[1,1,1])}
        for name,(position,scale) in positions.items():
            actor=Actor({'tags':[name]},name);actor.location=Value(*position);actor.scale=Value(*scale)
            actor.light_component=Object({'intensity':5000.0,'attenuation_radius':1800.0,'cast_shadows':True})
            actor.light_component.props['mobility']=ComponentMobility.MOVABLE
            actor.camera_component=Object({'field_of_view':50.0})
            mesh='/Game/EuropaPhaseA/SM_EA_TwoLod.SM_EA_TwoLod' if name=='EA_LOD' else '/Engine/BasicShapes/Cube.Cube'
            actor.static_mesh_component=Object({'static_mesh':Object(path=mesh),'mobility':ComponentMobility.MOVABLE})
            rows.append(actor)
        pp=PostProcessVolume({'tags':[],'settings':PostProcessSettings(),'unbound':True});pp.location=Value()
        rows.append(pp)
        player=PlayerStart({'tags':[]},'PlayerStart');player.location=Value(-550,0,500);player.scale=Value(1,1,1)
        rows.append(player)
        return rows


class EditorStaticMeshLibrary(object):
    @staticmethod
    def get_lod_count(mesh):CALLS.append(['get_lod_count']);return 2


class SystemLibrary(object):
    @staticmethod
    def execute_console_command(world,command):
        CALLS.append(['console',command])
        if command.startswith('Automation RunTests'):
            CYCLE[0]+=1;CYCLE[1]=CLOCK[0]


class Paths(object):
    @staticmethod
    def project_dir():return 'D:/IA_Europa/StylizedParisStreet'


def load_asset(path):CALLS.append(['load_asset',path]);return Object(path=path)
def register_slate_post_tick_callback(callback):CALLBACK[0]=callback;return 1
def unregister_slate_post_tick_callback(handle):CALLS.append(['unregister',handle]);CALLBACK[0]=None
def ObjectIterator(cls):
    return [World(path='UEDPIE_%d_%d_EA_Core'%(CYCLE[0],i)) for i in range(2)] if 0<=CLOCK[0]-CYCLE[1]<2 else []


def stub():
    module=types.ModuleType('unreal')
    for name in ('Object','Actor','World','PostProcessVolume','PostProcessSettings','PointLight','CameraActor',
        'PlayerStart','StaticMeshActor','GameModeBase','ComponentMobility','Rotator','EditorLevelLibrary','EditorStaticMeshLibrary',
        'SystemLibrary','Paths','load_asset','register_slate_post_tick_callback','unregister_slate_post_tick_callback','ObjectIterator'):
        setattr(module,name,globals()[name])
    return module


def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',required=True);args=parser.parse_args()
    assert sys.version_info[:3]==(2,7,14),'FC2 requires engine Python 2.7.14'
    output=os.path.abspath(args.output)
    assert output.lower().startswith('d:\\ia_europa\\receipts\\09427c\\scratch\\') and not os.path.exists(output)
    os.makedirs(output)
    here=os.path.dirname(os.path.abspath(__file__))
    for name in ('runtime_editor_425.py','runtime_scripts.py'):
        with open(os.path.join(here,name),'rb') as stream:compile(stream.read(),name,'exec')
    scripts=imp.load_source('runtime_scripts_test',os.path.join(here,'runtime_scripts.py'))
    sys.modules['unreal']=stub()
    results=[]
    import rotator_contract
    static_rows=rotator_contract.check_all(here)
    for leg in ('Smoke','Load','WorldsPIE'):
        del CALLS[:];CLOCK[0]=0;CYCLE[:]=[0,-100]
        receipt=os.path.join(output,leg+'.json')
        launcher=scripts.launcher(os.path.join(here,'runtime_editor_425.py'),leg,receipt,'probe.jsonl','native.json')
        namespace={};exec(compile(launcher,'launcher','exec'),namespace)
        module=namespace['_europa_runtime']
        module.time=types.ModuleType('fake_clock');module.time.time=lambda:CLOCK[0]
        module.STATE['started']=0
        for tick in range(900):
            CLOCK[0]+=.1
            if CALLBACK[0] is None:break
            CALLBACK[0](.1)
        with open(receipt) as stream:data=json.load(stream)
        assert data['result']==('SMOKE_PASS' if leg=='Smoke' else 'DRIVER_PASS'),data
        assert not any('save' in str(c).lower() or 'create_asset' in str(c) for c in CALLS)
        if leg=='Smoke':assert data['module_alive'] and all(r['present'] for r in data['bindings'])
        if leg=='Load':assert len([c for c in CALLS if c[0]=='load_level'])==2
        if leg=='WorldsPIE':assert CYCLE[0]==3 and len(data['two_world_observations'])==3
        results.append(dict(case=leg,result='PASS',calls=list(CALLS)))
    for name,bad_angle,reenter,raises in (
        ('normalized_camera',False,False,False),('failing_camera_single_exit',True,False,False),
        ('reentrant_load_single_exit',True,True,False),('callback_exception_once',False,False,True)):
        del CALLS[:];CLOCK[0]=0;CYCLE[:]=[0,-100]
        ROTATION[:]=[0,0,-12] if bad_angle else [348,360,720]
        REENTER[0]=reenter;RAISE[0]=raises
        receipt=os.path.join(output,name+'.json')
        namespace={}
        exec(compile(scripts.launcher(os.path.join(here,'runtime_editor_425.py'),'Load',receipt,'p','n'),name,'exec'),namespace)
        module=namespace['_europa_runtime'];saved_tick=module.tick
        for unused in range(10):saved_tick(.1)
        with open(receipt) as stream:data=json.load(stream)
        assert data['result']==('DRIVER_FAIL' if bad_angle or raises else 'DRIVER_PASS'),data
        assert CALLBACK[0] is None and data['exit_requests']==1 and data['terminal']
        assert [c for c in CALLS if c[0]=='unregister']==[['unregister',1]]
        assert [c for c in CALLS if c[0]=='console']==[['console','QUIT_EDITOR']]
        assert CALLS[-1]==['console','QUIT_EDITOR']
        if bad_angle or raises:assert len([c for c in CALLS if c[0]=='load_level'])==1
        if reenter:assert data['reentrant_ticks_ignored']>=1
        if bad_angle:
            camera=next(c for c in data['readback'][0]['checks'] if c['name']=='camera_rotation')
            assert not camera['passed'] and camera['actual']['raw']==[0,0,-12] and camera['expected']['pitch']==-12
        before=list(CALLS)
        for unused in range(20):saved_tick(.1)
        assert CALLS==before
        results.append(dict(case=name,result='PASS',calls=list(CALLS),driver=data))
    ROTATION[:]=[-12,0,0];REENTER[0]=False;RAISE[0]=False
    native=[]
    legs=['WorldsGame','GC','Admission']+['A7_:%s:%s'%(route,control) for route in scripts.ROUTES for control in ('positive','NoApply','SkipRestore')]
    for leg in legs:
        rows=scripts.steps(leg);assert scripts.validate(json.loads(json.dumps(rows)))
        commands=[r['command'] for r in rows]
        if leg.startswith('A7_'):
            assert commands.count('apply')>=1 and commands.count('revert')>=1
            assert all(index>0 and rows[index-1]['command']=='prepare' for index,row in enumerate(rows) if row['command']=='screenshot')
            assert sum(r['command']=='screenshot' and r['args'][0].startswith('clean-') for r in rows)==20
        native.append(dict(leg=leg,steps=rows))
    assert scripts.validate(scripts.pie_steps())
    mutant_failed=False
    try:
        copied=runpy.run_path(os.path.join(here,'runtime_editor_425.py'))
        copied['start']('Smoke',os.path.join(output,'mutant.json'),'p','s')
        for i in range(12):copied['tick'](.1)
        with open(os.path.join(output,'mutant.json')) as stream:mutant=json.load(stream)
        mutant_failed=mutant.get('result')!='SMOKE_PASS' or not mutant.get('module_alive')
    except Exception as error:
        mutant_failed=True
        results.append(dict(case='runpy_dict_mutant',result='PASS',error=type(error).__name__))
    assert mutant_failed,'runpy returned-dict mutant survived'
    result=dict(result='PASS',python=sys.version,cases=results,native_game_scripts=native,rotator_static=static_rows,
                script_inventory=['runtime_editor_425.py','runtime_scripts.py','generated launch.py'],
                game_python_scripts=[],game_driver='Bound native EuropaPhaseAProbe JSON interpreter',unreal_launches=0,saved_assets=0)
    with open(os.path.join(output,'summary.json'),'w') as stream:json.dump(result,stream,indent=2)
    print(json.dumps(dict(result='PASS',editor_scripts=3,native_game_scripts=len(native),runpy_mutant_refused=True)))


if __name__=='__main__':main()
