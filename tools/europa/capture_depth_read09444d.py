from __future__ import print_function
import json
import os
import time
import unreal


def start(root):
    path = os.path.join(root, 'depth-readings.jsonl')
    epoch = os.path.join(root, 'plugin-frames', 'epoch01')
    state = dict(handle=None, next_index=0, samples=[])

    def record(kind, requested):
        row = dict(kind=kind, requested_capture_index=requested,
                   latest_saved_index=state['next_index'] - 1,
                   engine_frame=unreal.SystemLibrary.get_frame_count(),
                   value=unreal.SystemLibrary.get_console_variable_int_value('r.CustomDepth'),
                   time_utc_epoch=time.time(), source='in_process_SystemLibrary')
        state['samples'].append(row)
        with open(path, 'a') as stream:
            stream.write(json.dumps(row, sort_keys=True) + '\n')
        unreal.log('GI2 direct r.CustomDepth ' + json.dumps(row, sort_keys=True))

    def tick(delta):
        while os.path.isfile(os.path.join(epoch, 'frame-%06d.png' % state['next_index'])):
            state['next_index'] += 1
        for index in (80, 2300):
            if state['next_index'] > index and not any(s['requested_capture_index'] == index for s in state['samples']):
                record('live', index)
        if len(state['samples']) == 3:
            unreal.unregister_slate_post_tick_callback(state['handle'])

    record('startup', None)
    state['handle'] = unreal.register_slate_post_tick_callback(tick)
    return state
