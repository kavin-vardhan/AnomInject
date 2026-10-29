from __future__ import print_function
import imp
import json
import os
import sys
import types

HERE=os.path.dirname(os.path.abspath(__file__))
CALLS=[]
ACTORS=[]


def value(item):
    if isinstance(item,type):return item.__name__
    if isinstance(item,Object):return item.identity
    if isinstance(item,(list,tuple)):return [value(x) for x in item]
    return item


def log(name,*args):
    CALLS.append(dict(name=name,args=[value(x) for x in args]))


class Object(object):
    def __init__(self,identity='object'):self.identity=identity
    def set_editor_property(self,name,v):log('set_editor_property',self,name,v)


class StructBase(Object):pass
class PostProcessSettings(StructBase):pass
class WorldSettings(Object):pass
class World(Object):
    def get_world_settings(self):
        log('get_world_settings',self)
        return WorldSettings('world_settings')


class SceneComponent(Object):
    def set_mobility(self,v):log('set_mobility',self,v)


class StaticMeshComponent(SceneComponent):
    def set_static_mesh(self,v):log('set_static_mesh',self,v)


class PointLightComponent(SceneComponent):pass
class CameraComponent(SceneComponent):pass
class Actor(Object):
    def set_actor_label(self,v):
        log('set_actor_label',self,v)
        self.label=v
    def set_actor_scale3d(self,v):log('set_actor_scale3d',self,v)
    def get_editor_property(self,name):
        log('get_editor_property',self,name)
        assert name=='settings'
        return PostProcessSettings('postprocess_settings')
    def get_actor_label(self):
        log('get_actor_label',self)
        return getattr(self,'label',self.identity)


class StaticMeshActor(Actor):static_mesh_component=None
class PointLight(Actor):light_component=None
class CameraActor(Actor):camera_component=None
class PostProcessVolume(Actor):pass
class PlayerStart(Actor):pass
class GameModeBase(Object):pass
class ComponentMobility(object):MOVABLE='MOVABLE'
class AntiAliasingMethod(object):AAM_NONE='AAM_NONE'


def Vector(*args):
    log('Vector',*args)
    return list(args)
def Rotator(*args):
    log('Rotator',*args)
    return list(args)
def Name(name):
    log('Name',name)
    return name
def load_asset(path):
    log('load_asset',path)
    return Object(path)


class Paths(object):
    @staticmethod
    def project_dir():
        log('project_dir')
        return 'D:/IA_Europa/StylizedParisStreet'


class EditorAssetLibrary(object):
    @staticmethod
    def does_asset_exist(path):
        log('does_asset_exist',path)
        return False
    @staticmethod
    def duplicate_asset(source,destination):
        log('duplicate_asset',source,destination)
        return Object(destination)
    @staticmethod
    def save_loaded_asset(asset,only_dirty):
        log('save_loaded_asset',asset,only_dirty)
        return True


class EditorStaticMeshLibrary(object):
    @staticmethod
    def set_lod_from_static_mesh(mesh,index,source,source_index,reuse):
        log('set_lod_from_static_mesh',mesh,index,source,source_index,reuse)
        return 1
    @staticmethod
    def get_lod_count(mesh):
        log('get_lod_count',mesh)
        return 2


class EditorLevelLibrary(object):
    @staticmethod
    def new_level(path):
        log('new_level',path)
        return True
    @staticmethod
    def spawn_actor_from_class(kind,position,rotation=None):
        log('spawn_actor_from_class',kind,position,rotation)
        actor=kind(kind.__name__+str(len(ACTORS)))
        if kind is StaticMeshActor:actor.static_mesh_component=StaticMeshComponent(actor.identity+'.mesh')
        if kind is PointLight:actor.light_component=PointLightComponent(actor.identity+'.light')
        if kind is CameraActor:actor.camera_component=CameraComponent(actor.identity+'.camera')
        ACTORS.append(actor)
        return actor
    @staticmethod
    def get_editor_world():
        log('get_editor_world')
        return World('world')
    @staticmethod
    def save_current_level():
        log('save_current_level')
        return True
    @staticmethod
    def get_all_level_actors():
        log('get_all_level_actors')
        return ACTORS


def expected(recipe):
    names=['project_dir']+['does_asset_exist']*(2 if recipe=='EA_Core' else 1)+['load_asset','load_asset']
    if recipe=='EA_Core':names+=['duplicate_asset','set_lod_from_static_mesh','get_lod_count','save_loaded_asset','get_lod_count']
    else:names+=['load_asset','get_lod_count','get_lod_count']
    names+=['new_level']
    for unused in range(4):
        names+=['Vector','spawn_actor_from_class','set_actor_label','Name','set_editor_property',
            'set_mobility','set_static_mesh','Vector','set_actor_scale3d']
    names+=['Vector','spawn_actor_from_class','set_actor_label','Name','set_editor_property','set_mobility']+['set_editor_property']*3
    names+=['Vector','Rotator','spawn_actor_from_class','set_actor_label','Name','set_editor_property','set_editor_property']
    names+=['Vector','spawn_actor_from_class','set_editor_property','get_editor_property']+['set_editor_property']*7
    names+=['get_editor_world','get_world_settings','set_editor_property','Vector','Rotator','spawn_actor_from_class',
        'save_current_level','get_all_level_actors']+['get_actor_label']*8
    return names


def main(output):
    assert sys.version_info[:3]==(2,7,14),sys.version
    output=os.path.abspath(output)
    assert output.lower().startswith('d:\\ia_europa\\receipts\\09426c\\')
    assert not os.path.exists(output)
    os.makedirs(output)
    stub=types.ModuleType('unreal')
    for name,item in list(globals().items()):
        if isinstance(item,type) or name in ('Vector','Rotator','Name','load_asset'):
            setattr(stub,name,item)
    sys.modules['unreal']=stub
    launcher=imp.load_source('test_launcher',os.path.join(HERE,'fixture_launcher.py'))
    smoke=imp.load_source('test_smoke',os.path.join(HERE,'fixture_smoke_425.py'))
    for kind,props in smoke.PROPERTIES.items():
        for prop in props:setattr(getattr(stub,kind),prop,None)
    source=os.path.join(HERE,'author_425_fixture.py')
    smoke_source=os.path.join(HERE,'fixture_smoke_425.py')
    syntax=[]
    for path in (source,smoke_source,os.path.join(HERE,'fixture_launcher.py')):
        with open(path,'rb') as stream:compile(stream.read(),path,'exec')
        syntax.append(path)
    results=[]
    for recipe in ('Smoke','EA_Core','EA_Travel'):
        del CALLS[:];del ACTORS[:]
        receipt=os.path.join(output,recipe+'.json')
        generated=launcher.script(source,recipe,receipt,smoke_source)
        with open(os.path.join(output,recipe+'-launcher.py'),'w') as stream:stream.write(generated)
        namespace={}
        exec(compile(generated,'generated_'+recipe,'exec'),namespace)
        with open(receipt) as stream:data=json.load(stream)
        assert namespace['fixture_module'].os is os
        if recipe=='Smoke':
            assert data['result']=='SMOKE_PASS',data
            assert not CALLS
        else:
            assert data['result']=='AUTHORED_NOT_QUALIFIED',data
            assert [c['name'] for c in CALLS]==expected(recipe),CALLS
            assert data['lod_count']==2 and len(ACTORS)==8
            assert set(data['maps'][0]['anomaly_targets'].values())==set(['EA_HideCube','EA_BlinkCube','EA_LOD','EA_Light'])
            props=[c['args'][1:] for c in CALLS if c['name']=='set_editor_property']
            assert not any(pair[0] in ('override_anti_aliasing_method','anti_aliasing_method') for pair in props)
            for pair in (['intensity',5000.0],['attenuation_radius',1800.0],['cast_shadows',True],['field_of_view',50.0],
                         ['auto_exposure_min_brightness',1.0],['auto_exposure_max_brightness',1.0],['motion_blur_amount',0.0],
                         ['default_game_mode','GameModeBase']):assert pair in props,pair
            if recipe=='EA_Core':
                assert [c['args'] for c in CALLS if c['name']=='set_lod_from_static_mesh']==[['/Game/EuropaPhaseA/SM_EA_TwoLod',1,'/Engine/BasicShapes/Cube',0,True]]
        with open(os.path.join(output,recipe+'-calls.json'),'w') as stream:json.dump(CALLS,stream,indent=2)
        results.append(dict(case=recipe,result='PASS',calls=len(CALLS)))
    mutant="import runpy\nmodule=runpy.run_path(%r)\nmodule['author']('EA_Core',%r)\n" % (source,os.path.join(output,'mutant-author.json'))
    try:exec(compile(mutant,'runpy_dict_mutant','exec'),{})
    except Exception as error:
        assert isinstance(error,AttributeError) and 'NoneType' in str(error),(type(error).__name__,str(error))
        results.append(dict(case='runpy_returned_dict_mutant',result='EXPECTED_FAIL',error_type=type(error).__name__,error=str(error)))
    else:raise AssertionError('runpy returned-dict mutant unexpectedly survived')
    del stub.EditorLevelLibrary.save_current_level
    failed=smoke.check(namespace['fixture_module'],os.path.join(output,'missing-api.json'))
    assert failed['result']=='SMOKE_FAIL'
    results.append(dict(case='missing_api_mutant',result='EXPECTED_FAIL'))
    result=dict(result='PASS',python=sys.version,syntax=syntax,cases=results,asset_writes=0,
        note='All Unreal operations are recording stubs. Only harness scripts and JSON evidence were written.')
    with open(os.path.join(output,'summary.json'),'w') as stream:json.dump(result,stream,indent=2)
    print(json.dumps(result))


if __name__=='__main__':main(sys.argv[1])
