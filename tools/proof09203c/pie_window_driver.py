import json, os, re, time, importlib.util
import unreal

def _plan_path():
    m = re.search(r'-IAIPiePlan=("[^"]+"|\S+)', unreal.SystemLibrary.get_command_line())
    return m.group(1).strip('"') if m else None

PLAN = json.load(open(_plan_path(), 'r', encoding='utf-8'))
S = dict(phase='wait_pie', t0=time.time(), tphase=time.time(), i=0, ticks=0, done_at=None, quit_at=None, last=None, dts=[], step=None, stepmod=None)

def say(msg):
    unreal.log('IAI-PIE-DRIVER ' + msg)

def pie_world():
    try:
        return unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    except Exception:
        return None

def editor_world():
    try:
        return unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    except Exception:
        return None

def les():
    return unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)

def session_done():
    out = PLAN.get('out_dir')
    if not out or not os.path.isdir(out):
        return False
    for n in os.listdir(out):
        if os.path.isfile(os.path.join(out, n, 'run_summary.json')):
            return True
    return False

def set_phase(p):
    S['phase'] = p
    S['tphase'] = time.time()
    say('phase=%s t=%.1f' % (p, time.time() - S['t0']))

def flush_dts():
    p = PLAN.get('dt_out')
    if p and S['dts']:
        with open(p, 'w', encoding='utf-8') as f:
            json.dump(S['dts'], f)

def ctx():
    return dict(unreal=unreal, pie=pie_world(), editor=None if pie_world() else editor_world(), say=say, plan=PLAN, state=S)

def tick(dt):
    S['ticks'] += 1
    now = time.time()
    if PLAN.get('dt_out') and S['last'] is not None and S['phase'] != 'wait_pie':
        S['dts'].append([round(now, 4), round((now - S['last']) * 1000.0, 2)])
        if len(S['dts']) % 600 == 0:
            flush_dts()
    S['last'] = now
    el = now - S['tphase']
    if S['step'] is not None:
        try:
            if S['step'](ctx()):
                S['step'] = None
                S['tphase'] = now
        except Exception as ex:
            say('PYSTEP FAIL %s: %s' % (type(ex).__name__, ex))
            S['step'] = None
        return
    if S['phase'] == 'wait_pie':
        w = pie_world()
        if w is not None and les().is_in_play_in_editor():
            say('pie world=%s type=%s' % (w.get_name(), str(w)))
            set_phase('settle')
        elif el > PLAN.get('pie_timeout_s', 240):
            say('FAIL no PIE world after %.0f s' % el)
            set_phase('quit')
    elif S['phase'] == 'settle':
        if el >= PLAN.get('settle_s', 8.0):
            set_phase('cmds')
    elif S['phase'] == 'cmds':
        cmds = PLAN['cmds']
        if S['i'] >= len(cmds):
            set_phase('wait_done' if PLAN.get('out_dir') else 'hold')
            return
        if S['ticks'] % max(1, PLAN.get('gap_ticks', 2)) == 0:
            c = cmds[S['i']]
            w = pie_world()
            if c.startswith('WAIT '):
                if el >= float(c.split()[1]):
                    S['i'] += 1
                    S['tphase'] = now
                return
            if c.startswith('PYSTEP '):
                path = c.split(' ', 1)[1]
                spec = importlib.util.spec_from_file_location('iai_step_%d' % S['i'], path)
                mod = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(mod)
                S['stepmod'] = mod
                S['step'] = mod.step
                say('cmd i=%d PYSTEP %s' % (S['i'], path))
                S['i'] += 1
                S['tphase'] = now
                return
            say('cmd i=%d %s' % (S['i'], c))
            unreal.SystemLibrary.execute_console_command(w, c)
            S['i'] += 1
            S['tphase'] = now
    elif S['phase'] == 'hold':
        if el >= PLAN.get('hold_s', 10.0):
            set_phase('end')
    elif S['phase'] == 'wait_done':
        if session_done():
            if S['done_at'] is None:
                S['done_at'] = now
                say('session done')
                for command in PLAN.get('post_commands', []):
                    say('post command ' + command)
                    unreal.SystemLibrary.execute_console_command(pie_world(), command)
            elif now - S['done_at'] >= PLAN.get('post_s', 4.0):
                set_phase('end')
        elif el > PLAN.get('done_timeout_s', 600):
            say('FAIL no run_summary after %.0f s' % el)
            set_phase('end')
    elif S['phase'] == 'end':
        if el < 0.1:
            return
        flush_dts()
        if les().is_in_play_in_editor():
            say('request end play')
            les().editor_request_end_play()
        set_phase('quit')
    elif S['phase'] == 'quit':
        if el >= 3.0 and S['quit_at'] is None:
            S['quit_at'] = now
            say('quit editor')
            unreal.SystemLibrary.quit_editor()

say('loaded plan=%s cmds=%d out=%s' % (_plan_path(), len(PLAN['cmds']), PLAN.get('out_dir')))
size = PLAN.get('viewport', [1920, 1080])
unreal.SystemLibrary.execute_console_command(editor_world(), 'IAI.Bench.PlayWindow %d %d' % tuple(size))
HANDLE = unreal.register_slate_post_tick_callback(tick)
