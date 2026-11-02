from __future__ import print_function
import json
import os
import sys
import types
import capture_route_driver425 as driver
import rotator_contract


def run(root):
    sys.dont_write_bytecode=True
    if not os.path.isdir(root):
        os.makedirs(root)
    outcomes=[]
    for raises in (False,True):
        callbacks=[]
        removed=[]
        commands=[]
        ticks=[0]
        fake=types.ModuleType('unreal')
        class Levels(object):
            @staticmethod
            def get_game_world():
                ticks[0]+=1
                return object() if ticks[0]>=3 else None
        class System(object):
            @staticmethod
            def execute_console_command(world,command):
                commands.append(command)
                if command!='QUIT_EDITOR':
                    callbacks[0](0.1)
                    if raises:
                        raise RuntimeError('synthetic callback fault')
        fake.EditorLevelLibrary=Levels
        fake.SystemLibrary=System
        def register(callback):
            callbacks.append(callback)
            return 7
        fake.register_slate_post_tick_callback=register
        fake.unregister_slate_post_tick_callback=lambda handle:removed.append(handle)
        sys.modules['unreal']=fake
        base=os.path.join(root,'raise' if raises else 'normal')
        command='IAI.Capture.C5 E:/fixture/frames 4242 blinking,lod_corruption'
        with open(base+'.json','w') as stream:
            json.dump(dict(command=command,receipt=base+'-receipt.json'),stream)
        driver.start(base+'.json')
        for i in range(8):
            callbacks[0](0.1)
        assert removed==[7] and commands.count(command)==1
        assert commands.count('QUIT_EDITOR')==int(raises)
        with open(base+'-receipt.json') as stream:
            receipt=json.load(stream)
        assert receipt['finished'] and receipt['exit_requests']==int(raises)
        outcomes.append(dict(case='raise' if raises else 'multi_tick_and_reentrant',result='PASS',commands=commands,unregistered=removed))
    with open(driver.__file__.replace('.pyc','.py'),'rb') as stream:
        assert not rotator_contract.check_source(stream.read(),'capture_route_driver425.py')
    assert rotator_contract.check_source('unreal.Rotator(1,2,3)','negative')
    with open(os.path.join(root,'result.json'),'w') as stream:
        json.dump(dict(result='PASS',python=sys.version,cases=outcomes,keyword_rotators='PASS'),stream,indent=2)
    print('PASS: Python 2.7 multi-tick command, re-entry and single-shot failure; keyword Rotator')


if __name__=='__main__':
    run(sys.argv[1])
