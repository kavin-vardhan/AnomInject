from __future__ import print_function
import json
import os
import time

STATE={}
HANDLE=None
BUSY=False

def record():
    with open(STATE['receipt'],'w') as stream:json.dump(STATE,stream,indent=2)

def console(world,command):
    import unreal
    if STATE.get('terminal'):return
    STATE.setdefault('commands',[]).append(command)
    unreal.SystemLibrary.execute_console_command(world,command)

def finish(result,reason=None):
    import unreal
    global HANDLE
    if STATE.get('terminal'):return
    STATE.update(terminal=True,result=result,reason=reason,exit_requests=1,ended=time.time())
    record()
    if HANDLE is not None:
        handle=HANDLE;HANDLE=None
        unreal.unregister_slate_post_tick_callback(handle)
    unreal.SystemLibrary.execute_console_command(unreal.EditorLevelLibrary.get_editor_world(),'QUIT_EDITOR')

def step(name):
    if name in STATE['steps']:return False
    STATE['steps'].append(name);record();return True

def tick(delta):
    import unreal
    global BUSY
    if STATE.get('terminal') or BUSY:return
    BUSY=True
    try:
        STATE['ticks']+=1
        if time.time()-STATE['started']>80:raise RuntimeError('C1 driver deadline')
        if STATE['ticks']<5:return
        editor=unreal.EditorLevelLibrary.get_editor_world()
        if step('load'):
            if not unreal.EditorLevelLibrary.load_level('/Game/EuropaPhaseA/EA_Core'):raise RuntimeError('Load refused')
            return
        if step('pie_start'):
            console(editor,'IAI.Capture.PIEStart');return
        game=unreal.EditorLevelLibrary.get_game_world()
        if 'pie_end' in STATE['steps']:
            if not game:finish('PASS')
            return
        if not game:return
        if 'UEDPIE_' not in game.get_path_name():raise RuntimeError('Expected PIE world')
        if step('capture'):
            console(game,'IAI.Capture.C1 '+STATE['output'].replace('\\','/')+' 60');return
        path=os.path.join(STATE['output'],'capture.json')
        if os.path.exists(path):
            try:
                with open(path) as stream:data=json.load(stream)
            except (ValueError,IOError):return
            STATE['capture']=data
            if data['written']!=60:raise RuntimeError('Plugin frame count differs')
            if step('pie_end'):console(editor,'IAI.Capture.PIEEnd')
    except Exception as error:
        finish('FAIL',type(error).__name__+': '+str(error))
    finally:BUSY=False

def start(receipt,output):
    import unreal
    global STATE,HANDLE,BUSY
    if os.path.exists(receipt):raise RuntimeError('Receipt already exists')
    STATE=dict(receipt=receipt,output=output,started=time.time(),ticks=0,steps=[],terminal=False,exit_requests=0,result='RUNNING')
    BUSY=False
    record()
    HANDLE=unreal.register_slate_post_tick_callback(tick)
