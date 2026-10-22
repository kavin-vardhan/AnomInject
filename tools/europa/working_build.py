import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import time

from process_wrapper import HOSTS, command
import write_ledger as ledger

PORT = ledger.ROOT / 'StylizedParisStreet/Plugins/AnomalyInjector'
ALLOWED = {'425': 'StylizedParisStreetEditor', '51port': 'StackOBotEditor'}

def build_command(host, target, configuration, output):
    if host not in ALLOWED or target != ALLOWED[host] or configuration != 'Development':
        raise RuntimeError('ED2 admits only the owned 4.25 and ReGate51 Editor Development targets')
    project, engine, graph = HOSTS[host]
    if project.resolve(strict=True) != project or not project.is_relative_to(ledger.ROOT):
        raise RuntimeError('Unscoped host')
    args = command(host, 'build', target, configuration, output)
    args += ['-NoBuildUHT', '-NoHotReloadFromIDE', '-NoXGE', '-MaxParallelActions=1']
    return args

def classify_actions(path, project, engine):
    data = json.loads(path.read_text(encoding='utf-8-sig'))
    counts = {'project': 0, 'plugin': 0, 'engine': 0}
    rows = []
    standards = []
    for action in data['Actions']:
        outputs = [Path(p) for p in action.get('ProducedItems', [])]
        mutations = outputs + [Path(p) for p in action.get('DeleteItems', [])]
        if action.get('DependencyListFile'):
            mutations.append(Path(action['DependencyListFile']))
        if any(p.resolve().is_relative_to(engine.resolve()) for p in mutations):
            category = 'engine'
        elif any(not p.resolve().is_relative_to(project.parent.resolve()) for p in mutations):
            raise RuntimeError('Action writes outside its owned host: ' + json.dumps(action))
        elif any('/plugins/' in p.as_posix().lower() for p in outputs):
            category = 'plugin'
        else:
            category = 'project'
        counts[category] += 1
        rows.append({'category': category, 'type': action['Type'], 'command': action['CommandPath'],
            'arguments': action['CommandArguments'], 'outputs': list(map(str, outputs))})
        for item in action.get('PrerequisiteItems', []):
            rsp = Path(item)
            if rsp.suffix.lower() in ('.response', '.rsp') and rsp.is_file():
                body = rsp.read_text(encoding='utf-8-sig', errors='replace')
                standard = re.findall(r'/std:[^\s"]+', body)
                if standard:
                    standards.append({'path': str(rsp), 'sha256': hashlib.sha256(rsp.read_bytes()).hexdigest(), 'standards': standard})
    if counts['engine']:
        raise RuntimeError('ED2/ED4 engine action STOP: ' + json.dumps(counts))
    return {'counts': counts, 'actions': rows, 'cpp_standard_responses': standards}

def diagnostics(text):
    rows = []
    for line in text.splitlines():
        if not re.search(r'\b(?:fatal error|error|warning)(?:\s+[A-Z]+\d+)?\s*:', line, re.I):
            continue
        module = next((name for name in ('AnomalyInjector', 'AnomalyCapture', 'AnomalyControlServer', 'AnomalyShaders', 'AnomalyBench', 'EuropaPhaseAProbe', 'CaptureBench', 'StylizedParisStreet', 'StackOBot') if name.lower() in line.lower()), 'engine-or-unresolved')
        location = re.search(r'(?P<file>[^\r\n:]+\.(?:cpp|h|cs))\((?P<line>\d+)(?:,\d+)?\)', line)
        if location and module == 'engine-or-unresolved':
            matches = list((PORT / 'Source').rglob(Path(location['file']).name))
            if len(matches) == 1:
                module = matches[0].relative_to(PORT / 'Source').parts[0]
        rows.append({'module': module, 'diagnostic': line, 'file': location['file'].strip() if location else None,
            'line': int(location['line']) if location else None, 'new_port_warning': None if 'warning' in line.lower() else False})
    return rows

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', choices=ALLOWED, required=True)
    parser.add_argument('--label', required=True)
    args = parser.parse_args()
    host = args.host
    operation = Path(os.environ['EUROPA_OPERATION_DIR'])
    project, engine, graph = HOSTS[host]
    output = ledger.JOB / 'builds' / args.label
    output.mkdir(parents=True, exist_ok=False)
    sha = ledger.capture_child(['git', '-C', str(PORT), 'rev-parse', 'HEAD']).strip()
    for tree in (PORT, project.parent / 'Plugins/AnomalyInjector'):
        if ledger.capture_child(['git', '-C', str(tree), 'status', '--porcelain']).strip():
            raise RuntimeError('Commit first: source checkout is dirty')
        if ledger.capture_child(['git', '-C', str(tree), 'rev-parse', 'HEAD']).strip() != sha:
            raise RuntimeError('Candidate checkout does not match committed port SHA')
    assert json.loads((ledger.JOB / 'matrices/step3-summary.json').read_text())['result'] == 'PASS'
    assert json.loads((ledger.JOB / 'toolchains' / (host + '-selection.json')).read_text())['result'] == 'PASS'
    if host == '51port':
        assert json.loads((ledger.JOB / 'hosts/effective-plugins-and-binaries.json').read_text())['result'] == 'PASS'
        os.environ['DOTNET_ROOT'] = str(engine / 'Engine/Binaries/ThirdParty/DotNet/6.0.302/windows')
        os.environ['DOTNET_MULTILEVEL_LOOKUP'] = '0'
    else:
        assert json.loads((ledger.JOB / 'precompile/425-write-routing.json').read_text())['result'] == 'ADMITTED'
    user = ledger.ROOT / 'Runtime' / host / args.label / 'UHT'
    shader = ledger.ROOT / 'Caches' / host / 'ShaderWorking'
    os.environ['UE-CmdLineArgs'] = '-UserDir=' + user.as_posix() + '/ -ShaderWorkingDir=' + shader.as_posix() + '/ -DDC=' + graph + ' -AbsCrashReportClientLog=' + (output / 'CrashClient.log').as_posix()
    os.environ['EUROPA_BUILD_CHILDREN'] = '1'
    argv = build_command(host, ALLOWED[host], 'Development', output)
    ledger.dump(output / 'invocation.json', {'commit': sha, 'host': host, 'target': ALLOWED[host],
        'configuration': 'Development', 'argv': argv, 'working_result_only': True, 'NoEngineChanges': True,
        'NoBuildUHT': True, 'runtime_launches': 0, 'operation': str(operation), 'started_utc': ledger.now()})
    start = time.monotonic()
    plan = output / 'outdated-actions.json'
    plan_argv = argv + ['-WriteOutdatedActions=' + str(plan)]
    result = {'commit': sha, 'host': host, 'operation': str(operation), 'NoEngineChanges': True, 'engine_actions': 0,
        'action_counts': {'project': 0, 'plugin': 0, 'engine': 0}, 'exit_code': None, 'phase': 'action-plan'}
    try:
        ledger.run_child(plan_argv, cwd=project.parent)
        admission = classify_actions(plan, project, engine)
        ledger.dump(output / 'action-admission.json', admission)
        result['action_counts'] = admission['counts']
        result['cpp_standard_responses'] = admission['cpp_standard_responses']
        result['phase'] = 'compile'
        result['exit_code'] = ledger.run_child(argv, cwd=project.parent, acceptable=tuple(range(256)))
    finally:
        result['duration_seconds'] = time.monotonic() - start
        result['ended_utc'] = ledger.now()
        logs = []
        for receipt in operation.glob('child-*.json'):
            value = json.loads(receipt.read_text())
            if value['argv'] in (argv, plan_argv):
                logs.append(receipt.with_suffix('.log'))
        text = '\n'.join(path.read_text(encoding='utf-8', errors='replace') for path in logs)
        result['diagnostics'] = diagnostics(text)
        result['toolchain_lines'] = [line for line in text.splitlines() if 'Anomaly profile:' in line or ('Using ' in line and ('toolchain' in line or 'Windows ' in line and 'SDK' in line))]
        result['logs'] = list(map(str, logs))
        ledger.dump(output / 'result.json', result)
        ledger.dump(operation / 'build-admission-result.json', result)
    if result['exit_code'] != 0:
        raise SystemExit(result['exit_code'] or 1)
    print(json.dumps({'working_build': 'PASS', 'commit': sha, 'host': host, 'action_counts': result['action_counts']}))

if __name__ == '__main__':
    main()
