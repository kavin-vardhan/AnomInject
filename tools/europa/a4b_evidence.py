import hashlib
import json
from pathlib import Path
import re

FEATURES = {'ANOMALY_CAPTURE': '1', 'ANOMALY_CONTROL_SERVER': '1', 'ANOMALY_SHADERS': '1'}


def definitions(body):
    result = {}
    for name, value in re.findall(r'^\s*#define\s+(ANOMALY_\w+|ANOMINJECT_FW_TICKPIN)\s+([^\r\n]+)', body, re.M):
        result.setdefault(name, set()).add(value.strip())
    return {key: sorted(values) for key, values in result.items()}


def validate_definitions(values, candidate, baseline_tick=None):
    required = dict(FEATURES)
    if candidate:
        required.update(ANOMALY_PROFILE_BENCH='1', ANOMALY_BENCH='1')
    for key, expected in required.items():
        if values.get(key) != [expected]:
            raise RuntimeError('R1 generated define absent, conflicting or wrong: ' + key)
    tick = values.get('ANOMINJECT_FW_TICKPIN')
    if tick not in (['0'], ['1']):
        raise RuntimeError('R1 missing/conflicting generated tick pin')
    if baseline_tick is not None and tick != baseline_tick:
        raise RuntimeError('R1 candidate tick pin differs from baseline')
    return tick


def snapshot_definitions(project, target, configuration, host, actions, output):
    from working_build import response_paths, response_flags
    plugin = project.parent / 'Plugins/AnomalyInjector'
    natives = ('UnrealEditor',) if target.endswith('Editor') else (target, 'UnrealGame')
    roots = [plugin / 'Intermediate/Build/Win64' / native / configuration for native in natives]
    output.mkdir(parents=True, exist_ok=False)
    rows, aggregate, referenced = [], {}, set()
    for action in actions['Actions']:
        for response in response_paths(action):
            _, body = response_flags(response)
            for value in re.findall(r'^\s*/FI\s*"?([^\r\n"]+)', body, re.M):
                p = Path(value.strip())
                referenced.add(str((p if p.is_absolute() else Path(action['WorkingDirectory']) / p).resolve()))
    paths = {path for root in roots for path in root.glob('*/Definitions*.h')}
    for path in sorted(paths):
        if path.parent.name not in ('AnomalyInjector', 'AnomalyCapture', 'AnomalyControlServer', 'AnomalyShaders', 'AnomalyBench'):
            continue
        raw = path.read_bytes()
        values = definitions(raw.decode('utf-8-sig'))
        saved = output / (path.parent.name + '-' + path.name)
        saved.write_bytes(raw)
        rows.append({'path': str(path), 'module': path.parent.name, 'sha256': hashlib.sha256(raw).hexdigest(),
                     'archived': str(saved), 'values': values, 'referenced_by_current_compile_response': str(path.resolve()) in referenced})
        for key, value in values.items():
            aggregate.setdefault(key, set()).update(value)
    values = {key: sorted(value) for key, value in aggregate.items()}
    tick = validate_definitions(values, host == '51port')
    modules = {row['module'] for row in rows}
    if modules != {'AnomalyInjector', 'AnomalyCapture', 'AnomalyControlServer', 'AnomalyShaders', 'AnomalyBench'}:
        raise RuntimeError('R1 generated definition module denominator incomplete')
    result = {'result': 'PASS', 'rows': rows, 'values': values, 'tick_pin': tick,
              'bench_control': 'Generated AnomalyBench module definitions and its compile/link membership; explicit profile macros exist only in candidate.'}
    (output / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    return result
