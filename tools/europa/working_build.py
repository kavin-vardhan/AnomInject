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

def response_paths(action):
    paths = {Path(p) for p in action.get('PrerequisiteItems', []) if Path(p).suffix.lower() in ('.response', '.rsp')}
    arguments = action['CommandArguments']
    matches = re.findall(r'@"([^"]+)"|"@([^"]+)"', arguments)
    for pair in matches:
        paths.add(Path(next(value for value in pair if value)))
    if not matches and arguments.startswith('@') and '"' not in arguments:
        paths.add(Path(arguments[1:]))
    return sorted(paths)

def response_flags(path):
    raw = path.read_bytes()
    encoding = 'utf-16' if raw.startswith((b'\xff\xfe', b'\xfe\xff')) else 'utf-8-sig'
    body = raw.decode(encoding)
    standards = re.findall(r'/std:[^\s"]+', body)
    pch = [line.strip() for line in body.splitlines() if re.match(r'\s*/(?:Y[cu-]|Fp)', line)]
    return {'path': str(path), 'sha256': hashlib.sha256(raw).hexdigest(), 'encoding': encoding,
        'standards': standards, 'pch_flags': pch, 'pch_disabled_by_absence': not any(re.match(r'/Y[cu]', f) for f in pch)}, body

def classify_actions(path, project, engine):
    data = json.loads(path.read_text(encoding='utf-8-sig'))
    counts = {'project': 0, 'plugin': 0, 'engine': 0, 'engine_plugin': 0}
    rows, responses, binary_paths = [], {}, set()
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
        if category == 'engine' and any(p.resolve().is_relative_to((engine / 'Engine/Plugins').resolve()) for p in mutations):
            counts['engine_plugin'] += 1
        refs = []
        for rsp in response_paths(action):
            flags, body = response_flags(rsp)
            responses[str(rsp)] = flags
            refs.append(str(rsp))
            for line in body.splitlines():
                match = re.match(r'\s*/(?:PDB:|Fd)"?(.+?)"?\s*$', line, re.I)
                if match:
                    output = Path(match[1].strip('"'))
                    if output.suffix.casefold() == '.pdb':
                        if not output.is_absolute():
                            output = Path(action['WorkingDirectory']) / output
                        if not output.resolve().is_relative_to(project.parent.resolve()):
                            raise RuntimeError('Response PDB outside owned host: ' + str(output))
                        binary_paths.add(str(output))
        binary_paths.update(str(p) for p in outputs if p.suffix.lower() in ('.dll', '.exe', '.pdb'))
        rows.append({'category': category, 'type': action['Type'], 'command': action['CommandPath'],
            'arguments': action['CommandArguments'], 'outputs': list(map(str, outputs)), 'responses': refs})
    if counts['engine'] or counts['engine_plugin']:
        raise RuntimeError('Engine action STOP: ' + json.dumps(counts))
    return {'counts': counts, 'actions': rows, 'cpp_standard_responses': list(responses.values()), 'binary_outputs': sorted(binary_paths)}

def hash_binaries(admission):
    rows = []
    for raw in admission.get('binary_outputs', []):
        p = Path(raw)
        if not p.exists():
            rows.append({'path': raw, 'exists': False})
            continue
        before = p.stat()
        with p.open('rb') as stream:
            value = hashlib.file_digest(stream, 'sha256').hexdigest()
        after = p.stat()
        if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
            raise RuntimeError('Binary changed during hashing: ' + raw)
        rows.append({'path': raw, 'exists': True, 'size': after.st_size, 'mtime_ns': after.st_mtime_ns, 'sha256': value})
    return rows

def current_binaries(project):
    host = project.parent
    paths = []
    roots = [host / 'Binaries', *list((host / 'Plugins').glob('*/Binaries'))]
    for root in roots:
        if root.exists():
            paths.extend(str(p) for p in root.rglob('*') if p.suffix.lower() in ('.dll', '.exe', '.pdb') and p.is_file())
    return hash_binaries({'binary_outputs': sorted(paths)})


def retained_response_flags(project, target):
    host = project.parent
    roots = [host / 'Intermediate/Build/Win64', *list((host / 'Plugins').glob('*/Intermediate/Build/Win64'))]
    rows = []
    for root in roots:
        if not root.exists():
            continue
        for path in sorted(root.rglob('*')):
            relative = path.relative_to(root)
            if not path.is_file() or path.suffix not in ('.response', '.rsp') or 'Development' not in relative.parts:
                continue
            if not any(name in relative.parts for name in (target, 'UE4Editor', 'UnrealEditor')):
                continue
            flags, _ = response_flags(path)
            if flags['standards']:
                rows.append(dict(flags, evidence='existing response; not evidence of execution in this incremental invocation'))
    return rows


def diagnostics(text):
    rows = []
    for line in text.splitlines():
        if not re.search(r'\b(?:fatal error|error|warning)(?:\s+[A-Z]+\d+)?\s*:', line, re.I):
            continue
        normalized = line.replace('\\', '/')
        module_match = re.search(r'/Source/([^/]+)/', normalized, re.I)
        module = module_match.group(1) if module_match else 'engine-or-unresolved'
        location = re.search(r'(?P<file>(?:[A-Z]:/)?[^\r\n:]*?\.(?:cpp|h|cs|xml))\((?P<line>\d+)(?:,\d+)?\)', normalized, re.I)
        if location and module == 'engine-or-unresolved':
            matches = list((PORT / 'Source').rglob(Path(location['file']).name))
            if len(matches) == 1:
                module = matches[0].relative_to(PORT / 'Source').parts[0]
        if module == 'engine-or-unresolved':
            generated = re.search(r'/Development/([^/]+)/Module\.', normalized, re.I)
            if generated:
                module = generated.group(1)
        if 'BuildConfiguration.xml' in line:
            module = 'UBT/user XML'
        warning = bool(re.search(r'\bwarning\b', line, re.I))
        rows.append({'module': module, 'diagnostic': line, 'file': location['file'].strip() if location else None,
            'line': int(location['line']) if location else None, 'new_port_warning': warning and module != 'UBT/user XML'})
    return rows

def prepare_build_environment(environ, host, engine):
    for key in list(environ):
        if key.casefold() == 'ue-cmdlineargs':
            del environ[key]
    if host == '51port':
        environ['DOTNET_ROOT'] = str(engine / 'Engine/Binaries/ThirdParty/DotNet/6.0.302/windows')
        environ['DOTNET_MULTILEVEL_LOOKUP'] = '0'
    environ['EUROPA_BUILD_CHILDREN'] = '1'


def main():
    from build_memory import MemoryStopped
    from recovery_cleanup import cleanup
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', choices=[*ALLOWED, '51base'], required=True)
    parser.add_argument('--label', required=True)
    parser.add_argument('--query-only', action='store_true')
    args = parser.parse_args()
    host = args.host
    if args.query_only != (host == '51base'):
        raise RuntimeError('EG1 baseline is a query only; ED2 builds only the two candidate hosts')
    operation = Path(os.environ['EUROPA_OPERATION_DIR'])
    project, engine, graph = HOSTS[host]
    output = ledger.JOB / 'builds' / args.label
    output.mkdir(parents=True, exist_ok=False)
    sha = ledger.capture_child(['git', '-C', str(PORT), 'rev-parse', 'HEAD']).strip()
    compiled_sha = 'd7eee97731812384d4fd00af9032254923363ada' if args.query_only else sha
    for tree in (PORT, project.parent / 'Plugins/AnomalyInjector'):
        if ledger.capture_child(['git', '-C', str(tree), 'status', '--porcelain']).strip():
            raise RuntimeError('Commit first: source checkout is dirty')
        expected = sha if tree == PORT else compiled_sha
        if ledger.capture_child(['git', '-C', str(tree), 'rev-parse', 'HEAD']).strip() != expected:
            raise RuntimeError('Checkout does not match declared SHA')
    assert json.loads((ledger.JOB / 'matrices/step3-summary.json').read_text())['result'] == 'PASS'
    selection = '51port' if args.query_only else host
    assert json.loads((ledger.JOB / 'toolchains' / (selection + '-selection.json')).read_text())['result'] == 'PASS'
    if host != '425':
        assert json.loads((ledger.JOB / 'hosts/effective-plugins-and-binaries.json').read_text())['result'] == 'PASS'
    else:
        assert json.loads((ledger.JOB / 'precompile/425-write-routing.json').read_text())['result'] == 'ADMITTED'
    prepare_build_environment(os.environ, '51port' if host == '51base' else host, engine)
    target = 'StackOBotEditor' if args.query_only else ALLOWED[host]
    if args.query_only:
        argv = command(host, 'json', target, 'Development', output)
        argv = [a for a in argv if not a.startswith(('-Mode=', '-OutputFile='))]
        argv += ['-NoBuildUHT', '-NoHotReloadFromIDE', '-NoXGE', '-MaxParallelActions=1']
    else:
        argv = build_command(host, target, 'Development', output)
    ledger.dump(output / 'invocation.json', {'commit': compiled_sha, 'tooling_commit': sha, 'host': host, 'target': target,
        'configuration': 'Development', 'argv': argv, 'working_result_only': True, 'query_only': args.query_only,
        'NoEngineChanges': True, 'NoBuildUHT': True, 'runtime_launches': 0, 'operation': str(operation), 'started_utc': ledger.now()})
    start = time.monotonic()
    result = {'commit': compiled_sha, 'tooling_commit': sha, 'host': host, 'operation': str(operation), 'NoEngineChanges': True,
        'engine_actions': None, 'engine_plugin_actions': None, 'exit_code': None, 'query_only': args.query_only, 'attempts': []}
    invocations = []
    attempt = 0
    try:
        while True:
            attempt += 1
            plan = output / ('outdated-actions.json' if attempt == 1 else f'outdated-actions-{attempt}.json')
            plan_argv = argv + ['-WriteOutdatedActions=' + str(plan)]
            admission = {}
            record = {'attempt': attempt, 'started_utc': ledger.now()}
            try:
                invocations.append(plan_argv)
                ledger.run_child(plan_argv, cwd=project.parent, memory_guard=True)
                admission = classify_actions(plan, project, engine)
                ledger.dump(output / f'action-admission-{attempt}.json', admission)
                result.update({'action_counts': admission['counts'], 'engine_actions': admission['counts']['engine'], 'engine_plugin_actions': admission['counts']['engine_plugin'],
                    'cpp_standard_responses': admission['cpp_standard_responses']})
                if args.query_only:
                    result['exit_code'] = 0
                    record['outcome'] = 'query_complete'
                else:
                    invocations.append(argv)
                    result['exit_code'] = ledger.run_child(argv, cwd=project.parent, acceptable=tuple(range(256)), memory_guard=True)
                    record['outcome'] = 'compile_complete'
                break
            except MemoryStopped:
                record['outcome'] = 'EF1_stopped_not_D2'
                if args.query_only:
                    raise
                cleanup_plan = plan
                if not cleanup_plan.exists():
                    cleanup_plan = ledger.ROOT / ('Receipts/09405/builds/425-01/outdated-actions.json' if host == '425' else 'Receipts/09405/builds/51-01/outdated-actions.json')
                cleanup(cleanup_plan, project, output / f'EF1-cleanup-{attempt}.json')
            finally:
                record['ended_utc'] = ledger.now()
                record['binary_hashes'] = hash_binaries(admission)
                record['admission'] = admission
                result['attempts'].append(record)
                ledger.dump(output / f'attempt-{attempt}.json', record)
    finally:
        result['duration_seconds'] = time.monotonic() - start
        result['ended_utc'] = ledger.now()
        logs = []
        for child in operation.glob('child-*.json'):
            value = json.loads(child.read_text())
            if value.get('argv') in invocations:
                logs.append(child.with_suffix('.log'))
        text = '\n'.join(path.read_text(encoding='utf-8', errors='replace') for path in logs if path.exists())
        result['diagnostics'] = diagnostics(text)
        result['toolchain_lines'] = [line for line in text.splitlines() if 'Anomaly profile:' in line or ('Using ' in line and ('toolchain' in line or 'Windows ' in line and 'SDK' in line))]
        result['logs'] = list(map(str, logs))
        result['binary_hashes'] = result['attempts'][-1]['binary_hashes'] if result['attempts'] else []
        if not args.query_only:
            result['host_binary_inventory'] = current_binaries(project)
            result['retained_compile_responses'] = retained_response_flags(project, target)
        ledger.dump(output / 'result.json', result)
        ledger.dump(operation / 'build-admission-result.json', result)
    if result['exit_code'] != 0:
        raise SystemExit(result['exit_code'] or 1)
    print(json.dumps({'working_build': 'PASS', 'commit': compiled_sha, 'host': host, 'action_counts': result['action_counts'], 'query_only': args.query_only}))

if __name__ == '__main__':
    main()
