from __future__ import print_function
import copy
import imp
import json
import os
import sys
import types
import rotator_contract
import capture_425_reader as reader

def main(output):
    assert sys.version_info[:3]==(2,7,14)
    os.makedirs(output)
    calls=[];callback=[None];game=[None];driver=[None]
    class World(object):
        def get_path_name(self):return '/Game/UEDPIE_0_EA_Core'
    class Editor(object):
        @staticmethod
        def get_editor_world():return World()
        @staticmethod
        def get_game_world():return game[0]
        @staticmethod
        def load_level(path):
            calls.append(['load',path])
            for unused in range(5):driver[0].tick(.1)
            return True
    def console(world,command):
        calls.append(['console',command])
        if command=='IAI.Capture.PIEStart':game[0]=World()
        if command=='IAI.Capture.PIEEnd':game[0]=None
    def register(fn):callback[0]=fn;return 1
    def unregister(handle):calls.append(['unregister',handle]);callback[0]=None
    unreal=types.ModuleType('unreal');unreal.EditorLevelLibrary=Editor
    unreal.SystemLibrary=type('System',(),{'execute_console_command':staticmethod(console)})
    unreal.register_slate_post_tick_callback=register;unreal.unregister_slate_post_tick_callback=unregister
    sys.modules['unreal']=unreal
    here=os.path.dirname(__file__)
    static=[]
    for name in ('capture_425_driver.py','capture_425_reader.py'):
        with open(os.path.join(here,name),'rb') as stream:source=stream.read()
        compile(source,name,'exec');assert not rotator_contract.check_source(source,name);static.append(name)
    assert rotator_contract.check_source('unreal.Rotator(-12,0,0)','mutant')
    cases=[]
    calls[:]=[]
    module=imp.load_source('capture_smoke',os.path.join(here,'capture_425_driver.py'));driver[0]=module
    module.start(os.path.join(output,'smoke.json'),os.path.join(output,'never-created'),True)
    for unused in range(10):module.tick(.1)
    assert module.STATE['result']=='SMOKE_PASS' and module.STATE['module_alive'] and callback[0] is None
    assert not any(c[0]=='load' for c in calls) and calls[-1]==['console','QUIT_EDITOR']
    cases.append(dict(case='smoke_no_load',result='PASS'))
    for fail in (False,True):
        calls[:]=[];game[0]=None
        module=imp.load_source('capture_test',os.path.join(here,'capture_425_driver.py'));driver[0]=module
        root=os.path.join(output,str(fail));os.makedirs(root)
        module.start(os.path.join(root,'driver.json'),os.path.join(root,'plugin-frames'))
        for unused in range(12):module.tick(.1)
        assert len([c for c in calls if c[0]=='load'])==1
        os.makedirs(module.STATE['output'])
        with open(os.path.join(module.STATE['output'],'capture.json'),'w') as stream:json.dump({'written':0 if fail else 60},stream)
        for unused in range(10):module.tick(.1)
        assert module.STATE['result']==('FAIL' if fail else 'PASS') and module.STATE['exit_requests']==1
        assert callback[0] is None and calls[-2:]==[['unregister',1],['console','QUIT_EDITOR']]
        frozen=list(calls)
        for unused in range(10):module.tick(.1)
        assert calls==frozen
        cases.append(dict(case='failure_once' if fail else 'multi_tick_load',result='PASS',calls=list(calls)))
    good=dict(requested=60,written=60,rhi='D3D11',dropped=0,pitch=1280,mapped_height=720,elapsed_seconds=8,
        frames=[dict(index=i,request=i+1,file='frame-%06d.png'%i,width=1280,height=720,format=2,aa=0) for i in range(60)])
    reader.metadata(good)
    for name in ('duplicate','zero_map','unknown_format'):
        bad=copy.deepcopy(good)
        if name=='duplicate':bad['frames'][1]['index']=0
        elif name=='zero_map':bad['pitch']=0;bad['mapped_height']=0
        else:bad['frames'][0]['format']=0
        try:reader.metadata(bad)
        except ValueError:cases.append(dict(case=name,result='EXPECTED_FAIL'))
        else:raise AssertionError(name)
    result=dict(result='PASS',python=sys.version,cases=cases,rotator_static=static,editor_launches=0,saves=0)
    with open(os.path.join(output,'summary.json'),'w') as stream:json.dump(result,stream,indent=2)
    print(json.dumps(dict(result='PASS',cases=len(cases),python=sys.version)))

if __name__=='__main__':main(sys.argv[1])
