import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import time

import write_ledger as ledger
from process_wrapper import HOSTS
from working_build import classify_actions, hash_binaries, current_binaries, prepare_build_environment
from a4_evidence import require_rules_version, toolchain_responses, diagnostics, validate_build, shipping_names

PORT = ledger.ROOT / 'StylizedParisStreet/Plugins/AnomalyInjector'
PAIRS = {('425', 'StylizedParisStreetEditor', 'Development'),
         ('425', 'StylizedParisStreet', 'Development'),
         ('425', 'StylizedParisStreet', 'Shipping'),
         ('51port', 'StackOBotEditor', 'Development')}


def command(host, target, configuration, output):
    if (host, target, configuration) not in PAIRS:
        raise RuntimeError('EH1/EH5 target not authorized')
    project, engine, _ = HOSTS[host]
    if project.resolve(strict=True) != project or not output.resolve().is_relative_to(ledger.JOB.resolve()):
        raise RuntimeError('Unscoped build host or receipt directory')
    version = json.loads((engine / 'Engine/Build/Build.version').read_text())
    require_rules_version(host, (version['MajorVersion'], version['MinorVersion']))
    ubt = engine / ('Engine/Binaries/DotNET/UnrealBuildTool.exe' if host == '425' else 'Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe')
    return [str(ubt), target, 'Win64', configuration, '-Project=' + str(project),
            '-Log=' + str(output / 'UBT.log'), '-NoEngineChanges', '-NoBuildUHT', '-NoHotReload',
            '-NoHotReloadFromIDE', '-NoXGE', '-MaxParallelActions=1', '-Verbose'], version


def output_inventory(host):
    roots = [host / 'Binaries', host / 'Intermediate/Build',
             host / 'Plugins/AnomalyInjector/Binaries', host / 'Plugins/AnomalyInjector/Intermediate']
    rows = {}
    for root in roots:
        if root.exists():
            for path in root.rglob('*'):
                if path.is_file():
                    if path.resolve(strict=True) != path:
                        raise RuntimeError('Reparse path in owned build outputs')
                    stat = path.stat()
                    rows[str(path)] = [stat.st_size, stat.st_mtime_ns]
    return rows


def clean_paris(receipt):
    host = ledger.ROOT / 'StylizedParisStreet'
    roots = [host / 'Binaries', host / 'Intermediate/Build',
             host / 'Plugins/AnomalyInjector/Binaries', host / 'Plugins/AnomalyInjector/Intermediate']
    files, directories = [], []
    for root in roots:
        if not root.exists():
            continue
        if root.resolve(strict=True) != root or not root.resolve().is_relative_to(host.resolve(strict=True)):
            raise RuntimeError('EH2 cleanup path is not inside physical owned Paris host')
        directories.append(root)
        for path in sorted(root.rglob('*')):
            if path.stat(follow_symlinks=False).st_file_attributes & 1024 or path.resolve(strict=True) != path:
                raise RuntimeError('EH2 refuses reparse points')
            if path.is_dir():
                directories.append(path)
            elif path.is_file():
                with path.open('rb') as stream:
                    digest = hashlib.file_digest(stream, 'sha256').hexdigest()
                stat = path.stat()
                files.append({'path': str(path), 'size': stat.st_size, 'mtime_ns': stat.st_mtime_ns, 'sha256': digest})
    data = {'authority': 'EH2; includes host probe outputs', 'host': str(host), 'roots': list(map(str, roots)),
            'files': files, 'directories': list(map(str, directories)), 'bytes': sum(row['size'] for row in files)}
    if receipt.exists():
        raise RuntimeError('Refusing to overwrite cleanup receipt')
    ledger.dump(receipt, data)
    for row in files:
        path = Path(row['path'])
        stat = path.stat()
        if path.resolve(strict=True) != path or [stat.st_size, stat.st_mtime_ns] != [row['size'], row['mtime_ns']]:
            raise RuntimeError('EH2 file changed after receipt')
        path.unlink()
    for directory in sorted(directories, key=lambda p: len(p.parts), reverse=True):
        directory.rmdir()
    data.update(result='PASS', removed_files=len(files), removed_directories=len(directories))
    ledger.dump(receipt.with_name(receipt.stem + '-result.json'), data)
    return data


def execute(host, target, configuration, output, *, dirty_manifest=None, single_file=None, expected_failure=None, require_compile=True):
    from build_memory import MemoryStopped
    from recovery_cleanup import cleanup
    project, engine, _ = HOSTS[host]
    if output.exists():
        raise RuntimeError('Build receipt already exists')
    argv, version = command(host, target, configuration, output)
    output.mkdir(parents=True)
    operation = Path(os.environ['EUROPA_OPERATION_DIR'])
    if operation.parent.resolve() != ledger.LEDGER.resolve():
        raise RuntimeError('Build needs this job ledger operation')
    sha = ledger.capture_child(['git', '-C', str(PORT), 'rev-parse', 'HEAD']).strip()
    tree = project.parent / 'Plugins/AnomalyInjector'
    if ledger.capture_child(['git', '-C', str(tree), 'rev-parse', 'HEAD']).strip() != sha:
        raise RuntimeError('Candidate checkouts must bind to the same SHA')
    status = ledger.capture_child(['git', '-C', str(tree), 'status', '--porcelain=v1', '--untracked-files=all'])
    if status.strip() and dirty_manifest is None:
        raise RuntimeError('Commit before build')
    if dirty_manifest:
        if not dirty_manifest.resolve(strict=True).is_relative_to(ledger.JOB.resolve(strict=True)):
            raise RuntimeError('Strict source mutation manifest outside this job')
        permitted = json.loads(dirty_manifest.read_text())['allowed_changes']
        for line in status.splitlines():
            path = line[3:].strip('"')
            if not any(path == item or path.startswith(item.rstrip('/') + '/') for item in permitted):
                raise RuntimeError('Unexpected dirty source in strict build: ' + line)
    if single_file:
        if not single_file.resolve(strict=True).is_relative_to(tree / 'Source'):
            raise RuntimeError('Single-file mutant outside plugin Source')
        argv.append('-SingleFile=' + str(single_file))
    prepare_build_environment(os.environ, host, engine)
    before = output_inventory(project.parent)
    ledger.dump(output / 'outputs-before.json', before)
    result = {'commit': sha, 'host': host, 'target': target, 'configuration': configuration,
              'rules_version': [version['MajorVersion'], version['MinorVersion']], 'argv': argv,
              'operation': str(operation), 'started_utc': ledger.now(), 'exit_code': None,
              'NoEngineChanges': True, 'dirty_manifest': str(dirty_manifest) if dirty_manifest else None,
              'expected_failure': expected_failure, 'require_actual_compile': require_compile, 'attempts': []}
    ledger.dump(output / 'invocation.json', result)
    start = time.monotonic()
    invocations = []
    admission = {'counts': {'project': 0, 'plugin': 0, 'engine': 0, 'engine_plugin': 0}, 'binary_outputs': []}
    attempt = 0
    try:
        while True:
            attempt += 1
            plan = output / f'actions-{attempt}.json'
            record = {'attempt': attempt, 'started_utc': ledger.now()}
            try:
                plan_argv = argv + ['-WriteOutdatedActions=' + str(plan)]
                invocations.append(plan_argv)
                code = ledger.run_child(plan_argv, cwd=project.parent, acceptable=tuple(range(256)), memory_guard=True)
                if code != 0:
                    result['exit_code'] = code
                    record['outcome'] = 'plan_failed'
                    break
                data = json.loads(plan.read_text(encoding='utf-8-sig'))
                admission = classify_actions(plan, project, engine)
                result['action_counts'] = admission['counts']
                result['actual_compile_responses'] = toolchain_responses(data, host, output / f'responses-{attempt}')
                ledger.dump(output / f'admission-{attempt}.json', admission)
                if require_compile and not result['actual_compile_responses']:
                    raise RuntimeError('Zero actual C++ compile command before execution')
                if configuration == 'Shipping':
                    names = shipping_names(admission['actions'])
                    if names:
                        raise RuntimeError('Shipping action includes forbidden module: ' + repr(names))
                invocations.append(argv)
                result['exit_code'] = ledger.run_child(argv, cwd=project.parent, acceptable=tuple(range(256)), memory_guard=True)
                record['outcome'] = 'compile_complete'
                result['actions_path'] = str(plan)
                break
            except MemoryStopped:
                record['outcome'] = 'EF1_stopped_not_D2'
                if plan.exists():
                    cleanup(plan, project, output / f'EF1-cleanup-{attempt}.json')
                elif host == '425':
                    clean_paris(output / f'EF1-before-plan-cleanup-{attempt}.json')
                else:
                    raise RuntimeError('EH5 plan stopped before cleanup denominator; resume by receipt')
            finally:
                record['ended_utc'] = ledger.now()
                record['binary_hashes'] = hash_binaries(admission)
                result['attempts'].append(record)
                ledger.dump(output / f'attempt-{attempt}.json', record)
    finally:
        logs = []
        execution_logs = []
        for child in sorted(operation.glob('child-*.json')):
            value = json.loads(child.read_text())
            if value.get('argv') in invocations:
                logs.append(child.with_suffix('.log'))
                if value.get('argv') == argv:
                    execution_logs.append(child.with_suffix('.log'))
        text = '\n'.join(p.read_text(encoding='utf-8', errors='replace') for p in logs if p.exists())
        executed_text = execution_logs[-1].read_text(encoding='utf-8', errors='replace') if execution_logs else ''
        action_lines = re.findall(r'^\s*\[(\d+)/(\d+)\]\s*(.*)$', executed_text, re.M)
        result.update(duration_seconds=time.monotonic() - start, ended_utc=ledger.now(),
                      diagnostics=diagnostics(text), logs=list(map(str, logs)),
                      toolchain_lines=[line for line in text.splitlines() if 'Anomaly profile:' in line or ('Using ' in line and ('toolchain' in line or 'SDK' in line))],
                      binary_hashes=hash_binaries(admission), host_binary_inventory=current_binaries(project))
        result['executed_action_lines'] = [{'index': int(i), 'total': int(n), 'description': description} for i, n, description in action_lines]
        result.setdefault('action_counts', admission['counts'])
        result.setdefault('actual_compile_responses', [])
        after = output_inventory(project.parent)
        result['changed_outputs'] = [{'path': key, 'before': before.get(key), 'after': value} for key, value in after.items() if before.get(key) != value]
        ledger.dump(output / 'result.json', result)
    if configuration == 'Shipping':
        receipt = project.parent / 'Binaries/Win64' / (target + '-Win64-Shipping.target')
        value = json.loads(receipt.read_text(encoding='utf-8-sig'))
        products = value['BuildProducts']
        changed_binary = [r for r in result['changed_outputs'] if Path(r['path']).suffix.casefold() in ('.exe', '.dll', '.pdb', '.lib', '.obj', '.modules')]
        forbidden = shipping_names(products) + shipping_names(changed_binary)
        evidence = {'receipt': str(receipt), 'receipt_sha256': hashlib.sha256(receipt.read_bytes()).hexdigest(),
                    'build_products': products, 'changed_compiled_outputs': changed_binary, 'forbidden_modules': forbidden,
                    'result': 'STOP' if forbidden else 'PASS', 'basis': 'Fresh Shipping actions, target BuildProducts and newly written compiled outputs; retained Development files are not Shipping products.'}
        ledger.dump(output / 'shipping-exclusion.json', evidence)
        if forbidden:
            raise RuntimeError('Shipping receipt/output STOP')
    if expected_failure:
        matched = [d for d in result['diagnostics'] if d['severity'] == 'error' and expected_failure in d['diagnostic']]
        if result['exit_code'] == 0 or not matched:
            raise RuntimeError('Missing-include mutant did not fail at its declared source')
        result['gate_result'] = 'EXPECTED_FAILURE'
    else:
        validate_build(result, require_compile=require_compile)
        if require_compile:
            count = len(admission['actions'])
            if {row['index'] for row in result['executed_action_lines']} != set(range(1, count + 1)) or any(row['total'] != count for row in result['executed_action_lines']):
                raise RuntimeError('A1 actual executed-action denominator does not equal the admitted action plan')
        if result['exit_code'] == 0 and any(not row['exists'] for row in result['binary_hashes']):
            raise RuntimeError('Successful build is missing a produced binary')
        result['gate_result'] = 'PASS'
    ledger.dump(output / 'result.json', result)
    print(json.dumps({'build_result': result['gate_result'], 'commit': sha, 'target': target, 'configuration': configuration,
                      'counts': result['action_counts'], 'duration_seconds': result['duration_seconds']}), flush=True)
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--host', choices=('425', '51port'), required=True)
    parser.add_argument('--target', required=True)
    parser.add_argument('--configuration', choices=('Development', 'Shipping'), required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--allow-up-to-date', action='store_true')
    args = parser.parse_args()
    execute(args.host, args.target, args.configuration, args.output, require_compile=not args.allow_up_to_date)


if __name__ == '__main__':
    main()
