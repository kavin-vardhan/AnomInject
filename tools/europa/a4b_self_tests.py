import argparse
import copy
import json
from pathlib import Path
import sys


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    root = parser.parse_args().output.resolve()
    import write_ledger as ledger
    import a4_build
    import working_build as build
    import a4_evidence
    import a4b_evidence as evidence
    from job_scope import load
    if not root.is_relative_to(ledger.ROOT.resolve(strict=True)):
        raise RuntimeError('Self-tests require physical D owned scratch')
    root.mkdir(parents=True, exist_ok=False)
    cases = []
    def check(name, test):
        if not test:
            raise AssertionError(name)
        cases.append({'name': name, 'result': 'PASS'})
    def refuses(name, fn):
        try:
            fn()
        except RuntimeError as error:
            cases.append({'name': name, 'result': 'REJECTED', 'reason': str(error)})
        else:
            raise AssertionError('Ineffective can-fail: ' + name)
    for host, target, configuration in ledger.SCOPE['build_pairs']:
        argv, _ = a4_build.command(host, target, configuration, ledger.JOB / 'synthetic-not-executed')
        check('permitted_' + host + '_' + target + '_' + configuration,
              '-MaxParallelActions=4' in argv and '-NoEngineChanges' in argv)
    refuses('baseline_Shipping', lambda: a4_build.command('51base', 'StackOBot', 'Shipping', ledger.JOB / 'synthetic'))
    refuses('unlisted_425', lambda: a4_build.command('425', 'StylizedParisStreetEditor', 'Development', ledger.JOB / 'synthetic'))
    refuses('wrong_receipt_job', lambda: a4_build.command('51port', 'StackOBot', 'Development', ledger.ROOT / 'Receipts/09408/synthetic'))
    refuses('unlisted_query', lambda: a4_build.command('51port', 'StackOBot', 'Development', ledger.JOB / 'synthetic', query=True))
    argv, _ = a4_build.command('51base', 'StackOBotEditor', 'Development', ledger.JOB / 'synthetic', query=True)
    check('query_explicitly_admitted', '-NoEngineChanges' in argv)
    config = dict(ledger.SCOPE, job_root=str(root))
    config_path = root / 'job-config.json'
    config_path.write_text(json.dumps(config))
    check('configuration_is_parameter', load(config_path)['job_root'] == str(root))
    config['max_parallel_actions'] = 8
    config_path.write_text(json.dumps(config))
    refuses('parallelism_above_ruling', lambda: load(config_path))
    fixture = root / 'Host'
    fixture.mkdir()
    project = fixture / 'Synthetic.uproject'
    project.write_text('{}')
    response = fixture / 'Intermediate/Build/Synthetic.response'
    response.parent.mkdir(parents=True)
    engine = Path('D:/UESource/UnrealEngine')
    engine_cpp = engine / 'Engine/Source/Runtime/SYNTHETIC.cpp'
    response.write_text(str(engine_cpp) + '\n/std:c++17\n')
    action = {'Type': 'Compile', 'WorkingDirectory': str(engine / 'Engine/Source'),
              'CommandPath': 'synthetic-not-executed', 'CommandArguments': '@' + str(response),
              'ProducedItems': [str(fixture / 'Intermediate/Build/Synthetic.obj')]}
    action_file = root / 'actions.json'
    def classify(value):
        action_file.write_text(json.dumps({'Actions': [value]}))
        return build.classify_actions(action_file, project, engine)
    result = classify(action)
    check('51_response_only_engine_source_in_host', result['counts']['engine_source_in_host'] == 1 and result['counts']['engine'] == 0)
    source = fixture / 'Source/Synthetic.cpp'
    source.parent.mkdir()
    source.write_text('SYNTHETIC NOT COMPILED')
    response.write_text('"' + str(source) + '"\n/std:c++17\n')
    check('project_cpp_not_engine_source', classify(action)['counts']['project'] == 1)
    generated = fixture / 'Intermediate/Build/Module.Synthetic.cpp'
    generated.write_text('#include "' + str(engine_cpp).replace('\\', '/') + '"\n')
    response.write_text(str(generated) + '\n/std:c++17\n')
    check('generated_unity_engine_source_in_host', classify(action)['counts']['engine_source_in_host'] == 1)
    for name, path, key in [
        ('engine51_product', engine / 'Engine/Intermediate/Build/Bad.obj', 'ProducedItems'),
        ('engine51_plugin_product', engine / 'Engine/Plugins/Synthetic/Intermediate/Build/Bad.obj', 'ProducedItems'),
        ('engine425_product', Path('E:/EpicGames/UE_4.25/Engine/Intermediate/Build/Bad.obj'), 'ProducedItems'),
        ('engine_delete', engine / 'Engine/Intermediate/Build/Bad.obj', 'DeleteItems'),
        ('engine_dependency', engine / 'Engine/Intermediate/Build/Bad.dep.json', 'DependencyListFile'),
        ('outside_host', root / 'Foreign.obj', 'ProducedItems')]:
        mutant = copy.deepcopy(action)
        mutant[key] = str(path) if key == 'DependencyListFile' else [str(path)]
        refuses(name, lambda m=mutant: classify(m))
    values = {name: [value] for name, value in evidence.FEATURES.items()}
    values.update(ANOMALY_PROFILE_BENCH=['1'], ANOMALY_BENCH=['1'], ANOMINJECT_FW_TICKPIN=['0'])
    check('feature_and_tick_positive', evidence.validate_definitions(values, True, ['0']) == ['0'])
    for name in values:
        mutant = copy.deepcopy(values)
        mutant[name] = ['1'] if name == 'ANOMINJECT_FW_TICKPIN' else ['0']
        refuses('wrong_generated_' + name, lambda v=mutant: evidence.validate_definitions(v, True, ['0']))
    check('definition_conflict_preserved', evidence.definitions('#define ANOMALY_CAPTURE 1\n#define ANOMALY_CAPTURE 0\n')['ANOMALY_CAPTURE'] == ['0', '1'])
    a4_evidence.self_tests(root / 'a1-parser.json')
    check('A1_parser_can_fails', json.loads((root / 'a1-parser.json').read_text())['result'] == 'PASS')
    ledger.dump(root / 'results.json', {'result': 'PASS', 'cases': cases, 'synthetic_only': True,
        'actual_engine_writes': 0, 'actual_compiles': 0, 'config': ledger.SCOPE})
    print(json.dumps({'A4b_self_tests': 'PASS', 'count': len(cases)}))


if __name__ == '__main__':
    main()
