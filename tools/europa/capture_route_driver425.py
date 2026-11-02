import json
import os
import time


def start(path):
    import unreal
    with open(path) as stream:
        config=json.load(stream)
    command=config['command']
    if not command.startswith(('IAI.Capture.C4 ','IAI.Capture.C5 ')):
        raise RuntimeError('Unarmed capture command')
    state=dict(issued=False,finished=False,exit_requests=0,command=command,callbacks=0)
    handle=[None]
    began=time.time()

    def record():
        with open(config['receipt'],'w') as stream:
            json.dump(state,stream,indent=2)

    def tick(delta):
        if state['finished']:
            return
        state['callbacks']+=1
        try:
            world=unreal.EditorLevelLibrary.get_game_world()
            if not world:
                if time.time()-began>60:
                    raise RuntimeError('Game world unavailable')
                return
            state['finished']=True
            unreal.unregister_slate_post_tick_callback(handle[0])
            state['issued']=True
            record()
            unreal.SystemLibrary.execute_console_command(world,command)
        except Exception as error:
            state['error']=str(error)
            if not state['finished']:
                state['finished']=True
                unreal.unregister_slate_post_tick_callback(handle[0])
            state['exit_requests']=1
            record()
            unreal.SystemLibrary.execute_console_command(None,'QUIT_EDITOR')

    handle[0]=unreal.register_slate_post_tick_callback(tick)
    record()
