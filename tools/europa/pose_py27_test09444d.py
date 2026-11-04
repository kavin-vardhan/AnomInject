from __future__ import print_function
import imp,json,os,sys,types
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
import rotator_contract
SCRIPT='E:/IA_Fixtures/Outline425/Scripts/texture_pose09444d.py'
CALLS=[]
class V(object):
    def __init__(self,x=0,y=0,z=0):self.x,self.y,self.z=x,y,z
class Mesh(object):
    def get_num_materials(self):return 1
    def set_material(self,slot,value):CALLS.append(['material',slot,value])
class Actor(object):
    def __init__(self,name):self.name=name;self.pos=V(450,-825 if name=='EA_Skeletal' else -575,2000);self.rotation=None
    def actor_has_tag(self,name):return self.name==name
    def set_actor_rotation(self,r,teleport):self.rotation=r;CALLS.append(['rotation',self.name,r])
    def set_actor_scale3d(self,v):CALLS.append(['scale',self.name,v.x,v.y,v.z])
    def get_actor_bounds(self,colliding):return V(self.pos.x,self.pos.y,self.pos.z+50),V(50,50,50)
    def get_actor_location(self):return self.pos
    def set_actor_location(self,v,sweep,teleport):self.pos=v;CALLS.append(['location',self.name,v.x,v.y,v.z])
    def get_components_by_class(self,kind):return [Mesh()]
    def get_name(self):return self.name
class PC(object):
    def set_control_rotation(self,r):CALLS.append(['control',r])
class Camera(object):
    def get_camera_location(self):return V(100,-700,2000)
actors=[Actor('EA_Skeletal'),Actor('OF_LOD_B'),Actor('OTHER'),Actor('OF_LOD_A')]
class Gameplay(object):
    @staticmethod
    def get_player_camera_manager(world,index):return Camera()
    @staticmethod
    def get_all_actors_of_class(world,kind):return actors
class System(object):
    @staticmethod
    def execute_console_command(world,cmd):CALLS.append(['command',cmd])
u=types.ModuleType('unreal');u.Vector=V;u.Actor=Actor;u.MeshComponent=object;u.GameplayStatics=Gameplay;u.SystemLibrary=System
u.Rotator=lambda **kw:kw
u.load_object=lambda outer,path:path
sys.modules['unreal']=u
source=open(SCRIPT,'rb').read()
assert not rotator_contract.check_source(source,SCRIPT)
assert rotator_contract.check_source('unreal.Rotator(1,2,3)','mutant')
module=imp.load_source('pose_driver',SCRIPT)
WRITES=[];module.write=lambda path,data:WRITES.append(data)
targets=module.setup(object(),PC(),'no-write-test')
assert len(targets)==2 and actors[2].rotation is None
assert actors[3].pos.z==300 and actors[3].rotation is None
assert actors[0].pos.y==-825 and actors[1].pos.y==-575
assert all(a.rotation==dict(pitch=-12.0,yaw=0.0,roll=0.0) for a in actors[:2])
assert len(module.EXPOSURES)==17 and module.EXPOSURES[0]==-1.5 and module.EXPOSURES[-1]==-2.5
assert not module.camera_settled(None,10) and not module.camera_settled(10,10.999)
assert module.camera_settled(10,11) and module.camera_settled(10,12)
assert WRITES[0]['runtime_only'] and ['command','fov 30'] in CALLS
assert 'save_asset' not in source and 'save_map' not in source
print(json.dumps(dict(result='PASS',python=sys.version.split()[0],targets=2,other_actor_unchanged=True,rotator_negative=True,pose=WRITES[0])))
