import json
import time
from pathlib import Path

state = dict(phase=0, actions=[])


def step(context):
    plan = context['plan']
    out = Path(plan['out_dir'])
    sessions = list(out.glob('*/Actual_Frames'))
    frames = len(list(sessions[0].glob('*.png'))) if len(sessions) == 1 else 0
    command = None
    if state['phase'] == 0 and frames >= plan.get('identity_bind_at', 120):
        command = plan.get('identity_probe_command', 'IAI.Bench.ProxyVT StaticMeshActor_77 Tex 0')
    elif state['phase'] == 1 and frames >= plan.get('identity_restore_at', 300):
        command = 'IAI.Bench.ProxyVT restore'
    if command:
        context['say']('IDENTITY phase=%d written_frames=%d command=%s' % (state['phase'], frames, command))
        context['unreal'].SystemLibrary.execute_console_command(context['pie'], command)
        state['actions'].append(dict(phase=state['phase'], frames_written=frames, time=time.time(), command=command))
        state['phase'] += 1
        Path(plan['identity_actions']).write_text(json.dumps(state, indent=2), encoding='utf-8')
    if time.time() - context['state']['tphase'] > 240:
        context['unreal'].SystemLibrary.execute_console_command(context['pie'], 'IAI.Bench.ProxyVT restore')
        state['timeout'] = True
        Path(plan['identity_actions']).write_text(json.dumps(state, indent=2), encoding='utf-8')
        return True
    return state['phase'] == 2
