import ast
import json
import sys
import types

objects = {}
unreal = types.ModuleType('unreal')

class Value(object):
    def __init__(self,name,kind=None):
        self.name=name
        self.values=dict(two_sided=False,blend_mode=0,used_shading_models='Default Lit')
        self.kind=kind
        objects[name]=self
    def get_path_name(self):
        return self.name
    def set_editor_property(self,key,value):
        self.values[key]=value
    def get_editor_property(self,key):
        return self.values[key]

class Colour(object):
    def __init__(self,r,g,b,a):
        self.r,self.g,self.b,self.a=r,g,b,a

class Assets(object):
    @staticmethod
    def does_asset_exist(path):
        return path in objects
    @staticmethod
    def load_asset(path):
        return objects[path]
    @staticmethod
    def save_asset(path):
        return True

class Tools(object):
    def create_asset(self,name,root,kind,factory):
        return Value(root+'/'+name)

class Editing(object):
    connections=[]
    @staticmethod
    def create_material_expression(mat,kind,x,y):
        return Value(mat.name+':node'+str(len(objects)),kind.__name__)
    @staticmethod
    def connect_material_property(*args):
        Editing.connections.append(args)
        return True
    @staticmethod
    def connect_material_expressions(*args):
        Editing.connections.append(args)
        return True
    @staticmethod
    def recompile_material(mat):
        return None

for name in ('Material','MaterialFactoryNew','MaterialExpressionConstant','MaterialExpressionTextureCoordinate',
    'MaterialExpressionComponentMask','MaterialExpressionFloor','MaterialExpressionAdd','MaterialExpressionMultiply',
    'MaterialExpressionFrac','MaterialExpressionConstant3Vector','MaterialExpressionLinearInterpolate'):
    setattr(unreal,name,type(name,(object,),{}))
unreal.MaterialEditingLibrary=Editing
unreal.EditorAssetLibrary=Assets
unreal.AssetToolsHelpers=type('Helpers',(object,),dict(get_asset_tools=staticmethod(lambda:Tools())))
unreal.LinearColor=Colour
unreal.MaterialProperty=type('Property',(object,),dict(MP_ROUGHNESS=1,MP_SPECULAR=2,MP_BASE_COLOR=3))
unreal.BlendMode=type('BlendMode',(object,),dict(BLEND_OPAQUE=0))
unreal.load_object=lambda outer,path:objects[path]
sys.modules['unreal']=unreal
source=open(sys.argv[1]).read()
tree=ast.parse(source)
for node in ast.walk(tree):
    if isinstance(node,ast.Call) and isinstance(node.func,ast.Attribute) and node.func.attr=='Rotator':
        assert not node.args and set(k.arg for k in node.keywords)==set(['pitch','yaw','roll'])
namespace={}
exec(compile(source,sys.argv[1],'exec'),namespace)
paths=[namespace['build_checker'](),namespace['build_pink']()]
for path,count in zip(paths,[14,3]):
    material=objects[path]
    nodes=[dict(path=n.name,kind=k,expected=namespace['_expected'](k,x,y)) for n,k,x,y in namespace['NODES'][path]]
    assert len(nodes)==count
    assert namespace['_verify'](path,nodes)['used_shading_models']=='Default Lit'
    material.values['used_with_skeletal_mesh']=False
    try:
        namespace['_verify'](path,nodes)
        raise AssertionError('Missing usage accepted')
    except RuntimeError:
        pass
    material.values['used_with_skeletal_mesh']=True
    target=objects[nodes[0]['path']]
    saved=target.values['r']
    target.values['r']=0.25
    try:
        namespace['_verify'](path,nodes)
        raise AssertionError('Wrong constant accepted')
    except RuntimeError:
        pass
    target.values['r']=saved
    try:
        namespace['_fresh_material'](path.split('/')[-1])
        raise AssertionError('Overwrite accepted')
    except RuntimeError:
        pass
assert len(namespace['USAGE_FLAGS'])==6 and 'used_with_nanite' not in namespace['USAGE_FLAGS']
print(json.dumps(dict(result='PASS',python=sys.version,materials=2,expressions=[14,3],
    usage_flag_negative=2,constant_negative=2,overwrite_negative=2,keyword_rotator='PASS')))
