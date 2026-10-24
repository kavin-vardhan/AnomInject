import argparse
import json
from pathlib import Path
import sys

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    import build_memory as memory
    import write_ledger as ledger
    import working_build as build
    root = args.output.resolve()
    assert root.is_relative_to(ledger.ROOT.resolve(strict=True))
    root.mkdir(parents=True, exist_ok=False)
    cases = []
    def check(name, condition):
        assert condition, name
        cases.append({'case': name, 'result': 'PASS'})
    def reading(physical, commit):
        return {'free_physical': physical, 'free_commit': commit}
    good = reading(6 * memory.GIB, 12 * memory.GIB)
    for name, value, expected in [('equal_start', good, True),
        ('one_byte_physical_below', reading(6 * memory.GIB - 1, 12 * memory.GIB), False),
        ('one_byte_commit_below', reading(6 * memory.GIB, 12 * memory.GIB - 1), False)]:
        check(name, memory.admission(value) == expected)
    clock = [0]
    def sleep(seconds):
        clock[0] += seconds
    rows = iter([good, reading(0, 0), good, good, good])
    memory.wait_for_window(root / 'wait.jsonl', sampler=lambda: next(rows), sleeper=sleep, clock=lambda: clock[0])
    check('three_consecutive_polls_resets_on_failure', clock[0] == 80)
    clock[0] = 0
    try:
        memory.wait_for_window(root / 'timeout.jsonl', sampler=lambda: reading(0, 0), sleeper=sleep, clock=lambda: clock[0], limit=40)
    except memory.NoMemoryWindow:
        check('wait_limit_returns_no_memory_window', clock[0] == 40)
    else:
        raise AssertionError('wait limit did not refuse')
    for name, value in [('physical', reading(memory.GIB - 1, 4 * memory.GIB)), ('commit', reading(memory.GIB, 4 * memory.GIB - 1))]:
        called = []
        def stop():
            called.append('synthetic-owned-UBT-only')
            return [{'pid': 'SYNTHETIC', 'ownership': 'fake sampler callback; no real process'}]
        streak = memory.monitor_sample(value, 0, stop, root / (name + '.jsonl'))
        check(name + '_one_low_does_not_stop', streak == 1 and not called)
        try:
            memory.monitor_sample(value, streak, stop, root / (name + '.jsonl'))
        except memory.MemoryStopped:
            check(name + '_second_low_stops', len(called) == 1)
        else:
            raise AssertionError('Second low failed to stop')
    check('in_run_equal_thresholds_allowed', not memory.pressure(reading(memory.GIB, 4 * memory.GIB)))
    check('in_run_recovery_resets_streak', memory.monitor_sample(good, 1, lambda: None, root / 'reset.jsonl') == 0)
    prefix = 'D:/UESource/UnrealEngine/Engine/'
    paths = [('Intermediate/Build/Win64/ActionHistory.bin', True),
        ('Intermediate/Build/Win64/UnrealEditor/Development/Core/LiveCodingInfo.json', True),
        ('Plugins/Runtime/Synthetic/Intermediate/Build/Win64/Inc/Synthetic/Timestamp', True),
        ('Intermediate/Build/Win64/SourceFileCache.bin', True),
        ('Intermediate/Build/Win64/DependencyCache.bin', True),
        ('Intermediate/Build/Win64/Makefile.ubt', True),
        ('Intermediate/Build/Win64/Test.uhtmanifest', True),
        ('Intermediate/UbtRuns/123_abcdef', True),
        ('Intermediate/UbtRuns/foreign.txt', False),
        ('Intermediate/Build/Win64/A.obj', False),
        ('Plugins/Runtime/Synthetic/Intermediate/Build/Win64/A.generated.h', False),
        ('Plugins/Runtime/Synthetic/Intermediate/Build/Win64/A.gen.cpp', False),
        ('Plugins/Runtime/Synthetic/Binaries/Win64/A.dll', False),
        ('Source/LiveCodingInfo.json', False), ('Content/LiveCodingInfo.json', False),
        ('Config/LiveCodingInfo.json', False), ('Shaders/Timestamp', False), ('Programs/Timestamp', False)]
    for relative, accepted in paths:
        for kind in ('build', 'ubt', 'read_only'):
            check('EG1_' + kind + '_' + relative, (ledger.classification('engine51', prefix + relative, kind)[0] == 'engine51_metadata') == accepted)
    for size, expected in ((100000000, 'PASS'), (100000001, 'STOP')):
        change = {'scope': 'engine51', 'path': prefix + 'Intermediate/Build/ActionHistory.bin',
            'before_size': 0, 'after_size': size, 'before_mtime_ns': 0, 'after_mtime_ns': 1, 'before_kind': 'file', 'after_kind': 'file'}
        check('engine_budget_' + str(size), ledger.assess([change], 'build')['result'] == expected)
    response = root / 'synthetic.response'
    response.write_text('/std:c++17\n/Yu"Shared.h"\n/Fp"D:/IA_Europa/Synthetic.pch"\n')
    for arguments in ('@' + str(response), '@"' + str(response) + '"', '"@' + str(response) + '"'):
        action = {'CommandArguments': arguments}
        check('response_path_' + arguments, build.response_paths(action) == [response])
    flags, _ = build.response_flags(response)
    check('effective_std_and_pch', flags['standards'] == ['/std:c++17'] and len(flags['pch_flags']) == 2 and not flags['pch_disabled_by_absence'])
    response.write_text('/std:c++17\n')
    flags, _ = build.response_flags(response)
    check('NoPCH_response_absence', flags['pch_flags'] == [] and flags['pch_disabled_by_absence'])
    binary = root / 'synthetic.dll'
    binary.write_bytes(b'not a PE; receipt hash can-fail only')
    first = build.hash_binaries({'binary_outputs': [str(binary)]})[0]['sha256']
    binary.write_bytes(binary.read_bytes() + b'!')
    second = build.hash_binaries({'binary_outputs': [str(binary)]})[0]['sha256']
    check('binary_hash_detects_one_byte_mutant', first != second)
    action_file = root / 'synthetic-actions.json'
    project = Path('D:/IA_Europa/ReGate51/StackOBot/StackOBot.uproject')
    engine = Path('D:/UESource/UnrealEngine')
    for name, target in [('engine', engine / 'Engine/Intermediate/Build/Bad.obj'),
        ('engine_plugin', engine / 'Engine/Plugins/SYNTHETIC/Intermediate/Build/Bad.obj')]:
        action_file.write_text(json.dumps({'Actions': [{'Type': 'Compile', 'CommandPath': 'synthetic-not-executed',
            'CommandArguments': '', 'ProducedItems': [str(target)]}]}))
        try:
            build.classify_actions(action_file, project, engine)
        except RuntimeError as error:
            check(name + '_outdated_action_refused', 'Engine action STOP' in str(error))
        else:
            raise AssertionError('Engine action mutant admitted')
    fixture = root / 'retained/Host'
    fixture_response = fixture / 'Intermediate/Build/Win64/UnrealEditor/Development/Synthetic/a.cpp.obj.response'
    fixture_response.parent.mkdir(parents=True)
    fixture_response.write_text('/std:c++17\n')
    excluded = fixture_response.parent.parent.parent.with_name('Shipping') / 'Synthetic/a.cpp.obj.response'
    excluded.parent.mkdir(parents=True)
    excluded.write_text('/std:c++20\n')
    retained = build.retained_response_flags(fixture / 'Host.uproject', 'HostEditor')
    check('incremental_retained_response_scope', len(retained) == 1 and retained[0]['standards'] == ['/std:c++17'])
    check('incremental_retained_response_not_execution_claim', 'not evidence of execution' in retained[0]['evidence'])
    import importlib.util
    import os
    from unittest.mock import patch
    strict_path = Path(__file__).resolve().parents[1] / 'strict_include_build.py'
    spec = importlib.util.spec_from_file_location('strict_synthetic', strict_path)
    strict = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(strict)
    import a4_build
    calls = []
    def fake_execute(host, target, configuration, folder, **kwargs):
        calls.append((host, target, configuration, folder, kwargs))
        argv, _ = a4_build.command(host, target, configuration, folder)
        assert '-MaxParallelActions=1' in argv and '-NoEngineChanges' in argv
        return {'exit_code': 0, 'duration_seconds': 0, 'logs': [], 'actions_path': 'SYNTHETIC-NOT-EXECUTED',
                'action_counts': {'project': 1, 'plugin': 0, 'engine': 0, 'engine_plugin': 0}}
    for name, engine, project, target in [
        ('425', Path('E:/EpicGames/UE_4.25'), Path('D:/IA_Europa/StylizedParisStreet/StylizedParisStreet.uproject'), 'StylizedParisStreetEditor'),
        ('51', Path('D:/UESource/UnrealEngine'), Path('D:/IA_Europa/ReGate51/StackOBot/StackOBot.uproject'), 'StackOBotEditor')]:
        calls.clear()
        with patch.object(a4_build, 'execute', fake_execute):
            result = strict.run_build(str(engine), target, str(project), str(root / ('strict-synthetic-' + name + '.log')), None)
        check('strict_' + name + '_delegates_to_supervised_wrapper', len(calls) == 1 and result['exit'] == 0)
        check('strict_' + name + '_one_worker_no_engine_changes', calls[0][4]['dirty_manifest'] is None)
    isolated = [sys.executable, '-I', '-B', '-c',
        'import sys; from pathlib import Path; sys.path.insert(0, sys.argv[1]); '
        'import strict_include_build; from europa.a4_evidence import self_tests; '
        'self_tests(Path(sys.argv[2]))', str(strict_path.parent), str(root / 'strict-isolated-reader.json')]
    ledger.run_child(isolated)
    check('strict_isolated_entry_imports', json.loads((root / 'strict-isolated-reader.json').read_text())['result'] == 'PASS')
    check('strict_synthetic_only', not any('SIC_' in str(p) for p in root.rglob('*')))
    ledger.dump(root / 'results.json', {'result': 'PASS', 'cases': cases, 'count': len(cases),
        'actual_memory_starvation': False, 'actual_process_termination': False, 'synthetic_C_E_paths_created': 0})
    print(json.dumps({'EF1_EG1_EG2': 'PASS', 'count': len(cases)}))

if __name__ == '__main__':
    main()
