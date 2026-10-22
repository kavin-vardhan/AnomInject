import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import write_ledger as ledger
import working_build as build

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    root = args.output.resolve()
    if not root.is_relative_to(ledger.ROOT.resolve(strict=True)):
        raise RuntimeError('Self-test evidence must remain in owned D scratch')
    root.mkdir(exist_ok=False)
    results = []
    original = dict(os.environ)
    try:
        os.environ['uE-cMdLiNeArGs'] = 'synthetic-receipt-marker'
        os.environ['dotnet_root'] = 'synthetic-dotnet-marker'
        env, recorded = ledger.child_env(root)
        assert len({key.casefold() for key in env}) == len(env)
        assert recorded['UE-CMDLINEARGS'] == 'synthetic-receipt-marker'
        code = 'import json,os,sys; keys=json.loads(sys.argv[1]); print(json.dumps({k:v for k,v in os.environ.items() if k.casefold() in keys}))'
        argv = [sys.executable, '-c', code, json.dumps([key.casefold() for key in recorded])]
        actual = json.loads(subprocess.check_output(argv, env=env, cwd=root, text=True))
        assert {k.casefold(): v for k, v in actual.items()} == {k.casefold(): v for k, v in recorded.items()}
        ledger.dump(root / 'environment-child.json', {'argv': argv, 'recorded': recorded, 'child_observed': actual, 'equal': True})
        results.append({'case': 'case_insensitive_environment_receipt_matches_real_child', 'result': 'PASS'})
    finally:
        os.environ.clear()
        os.environ.update(original)
    for host in ('425', '51port'):
        env = {'ue-CMDlineARGS': '-UserDir=SYNTHETIC', 'UE-CMDLINEARGS': 'duplicate-case-value'}
        build.prepare_build_environment(env, host, Path('D:/SYNTHETIC-ENGINE-NOT-USED'))
        assert not any(k.casefold() == 'ue-cmdlineargs' for k in env)
        assert not any('-UserDir' in v for v in env.values())
        assert env['EUROPA_BUILD_CHILDREN'] == '1'
        results.append({'case': 'no_ineffective_UHT_UserDir_' + host, 'result': 'PASS'})
    lines = [
        ('AnomalyCapture', r'D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Source\AnomalyCapture\Private\Test.cpp(17): warning C5043: synthetic'),
        ('AnomalyControlServer', r'D:\IA_Europa\ReGate51\StackOBot\Plugins\AnomalyInjector\Source\AnomalyControlServer\Private\Test.cpp(19,2): error C2039: synthetic'),
        ('AnomalyShaders', r'D:\IA_Europa\StylizedParisStreet\Plugins\AnomalyInjector\Intermediate\Build\Win64\UE4Editor\Development\AnomalyShaders\Module.AnomalyShaders.cpp: error C2855: synthetic'),
        ('AnomalyBench', 'AnomalyBenchModule.cpp(507): warning C5043: synthetic'),
        ('UBT/user XML', r'C:\Users\kavin\AppData\Roaming\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml(16): warning: synthetic')]
    for expected, line in lines:
        parsed = build.diagnostics(line)
        assert len(parsed) == 1 and parsed[0]['module'] == expected, parsed
        results.append({'case': 'diagnostic_' + expected, 'result': 'PASS', 'parsed': parsed[0]})
    prefix = 'C:/Users/kavin/AppData/Local/UnrealHeaderTool/'
    for relative, kind, expected in [
        ('Saved/Config/Windows/Engine.ini', 'file', 'PASS'),
        ('Saved/Config/Windows/Unexpected.ini', 'file', 'STOP'),
        ('Saved/Config/Windows/Engine.ini/escape', 'file', 'STOP'),
        ('Saved/Config/CrashReportClient/UE4CC-Windows-SYNTHETIC', 'directory', 'PASS'),
        ('Saved/Config/CrashReportClient/UE4CC-Windows-SYNTHETIC/CrashReportClient.ini', 'file', 'PASS'),
        ('Saved/Logs/SYNTHETIC.log', 'file', 'PASS'),
        ('Saved/Config/SYNTHETIC.ini', 'file', 'STOP'), ('Outside/SYNTHETIC', 'file', 'STOP')]:
        item = {'scope': 'uht', 'path': prefix + relative, 'before_size': None, 'after_size': 1 if kind == 'file' else 0,
            'before_mtime_ns': None, 'after_mtime_ns': 1, 'before_kind': None, 'after_kind': kind}
        result = ledger.assess([item], 'build')
        assert result['result'] == expected, result
        results.append({'case': 'EE1_' + relative, 'expected': expected, 'result': 'PASS'})
    ledger.dump(root / 'results.json', {'result': 'PASS', 'cases': results, 'count': len(results), 'external_synthetic_paths_created': 0})
    print(json.dumps({'EE1_EE6_self_tests': 'PASS', 'cases': len(results)}))

if __name__ == '__main__':
    main()
