import json

ROUTES = {
    'blinking': ['EA_BlinkCube', '12'],
    'lighting_mismatch': ['EA_Light', 'dim', '0.05'],
    'lod_corruption': ['EA_LOD', '1'],
    'missing_object': ['EA_HideCube']}


def steps(leg):
    rows = []
    def add(command, args=None, wait=1, success=True):
        if command == 'screenshot':
            rows.append(dict(command='prepare', args=[], wait_frames=1, expect_success=True))
        rows.append(dict(command=command, args=args or [], wait_frames=wait, expect_success=success))
    if leg == 'WorldsGame':
        add('preview'); add('snapshot', ['before_pause']); add('lifetime', wait=5)
        add('pause', ['1'], wait=60); add('snapshot', ['paused'])
        add('pause', ['0'], wait=60); add('snapshot', ['resumed'])
        add('travel', ['/Game/EuropaPhaseA/EA_Travel'])
    elif leg == 'GC':
        add('preview'); add('lifetime', wait=5); add('gc', wait=5); add('snapshot', ['gc_finished'])
    elif leg == 'Admission':
        add('snapshot', ['admission_before'])
        for target in (['blinking','EA_NO_SUCH_TARGET'], ['uv_corruption','EA_LOD','tile'], ['EA_unknown_id']):
            add('apply', target, success=False)
        add('snapshot', ['refusals_after'])
        for repeat in range(2):
            add('auto-seed', ['4242'])
            for draw in range(6):
                add('auto-fire', wait=3); add('auto-clear', wait=3)
            add('snapshot', ['seed_%d_after_stop' % repeat])
        add('revertall'); add('snapshot', ['pre_teardown'])
        add('travel', ['/Game/EuropaPhaseA/EA_Travel'])
    elif leg.startswith('A7_'):
        unused, route, control = leg.split(':')
        add('prepare', wait=180)
        for index in range(20):
            add('screenshot', ['clean-%02d.png' % index], wait=3)
        for event in range(6 if control == 'positive' else 1):
            prefix = '%s-%d' % (route, event)
            add('snapshot', [prefix+'-before'])
            add('screenshot', [prefix+'-before.png'])
            add('apply', [route]+ROUTES[route], wait=3, success=control != 'NoApply')
            for index in range(30 if route == 'blinking' else 2):
                add('screenshot', [prefix+'-during-%02d.png' % index], wait=3)
            add('snapshot', [prefix+'-active'])
            add('revert', [route], wait=10, success=control == 'positive')
            add('snapshot', [prefix+'-after'])
            add('screenshot', [prefix+'-after-0.png'], wait=5)
            add('screenshot', [prefix+'-after-1.png'])
        if control == 'positive':
            add('apply', ['blinking','EA_NO_SUCH_TARGET'], success=False)
            add('apply', ['uv_corruption','EA_LOD','tile'], success=False)
            add('snapshot', ['refusals_after'])
    else:
        raise ValueError('No native script for '+leg)
    return rows


def pie_steps():
    return [dict(command=command, args=args, wait_frames=wait, expect_success=True)
            for command, args, wait in [('snapshot',['two_worlds'],1), ('lifetime',[],5),
                ('pause',['1'],15), ('snapshot',['paused'],1), ('pause',['0'],15),
                ('snapshot',['resumed'],1), ('travel',['/Game/EuropaPhaseA/EA_Travel'],1)]]


def launcher(module_path, leg, receipt, probe, native_script):
    return ('import imp\n'
            '_europa_runtime = imp.load_source("europa_runtime_425", %r)\n'
            '_europa_runtime.start(%r, %r, %r, %r)\n' %
            (module_path, leg, receipt, probe, native_script))


def validate(rows):
    try:
        string_type = basestring
    except NameError:
        string_type = str
    allowed = set(['prepare','preview','snapshot','screenshot','apply','revert','revertall',
                   'pause','gc','lifetime','auto-seed','auto-fire','auto-clear','travel'])
    if not rows or len(rows) > 10000:
        raise ValueError('Empty/oversized native script')
    for row in rows:
        if row['command'] not in allowed or not isinstance(row['expect_success'], bool):
            raise ValueError('Unauthorized native command')
        if not 1 <= row['wait_frames'] <= 10000:
            raise ValueError('Invalid wait')
        if any(not isinstance(a, string_type) for a in row['args']):
            raise ValueError('Non-string native argument')
        if row['command'] == 'screenshot' and any(x in row['args'][0] for x in ('/', '\\', '..', ':')):
            raise ValueError('Screenshot path escape')
    return True
