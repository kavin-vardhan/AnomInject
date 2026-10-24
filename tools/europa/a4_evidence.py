import ctypes
from ctypes import wintypes
import hashlib
import json
from pathlib import Path
import re

EXPECTED = {'425': ('14.16.27051', '10.0.17763.0', (4, 25)),
            '51port': ('14.38.33144', '10.0.22621.0', (5, 1))}
IA_MODULES = ('AnomalyInjector', 'AnomalyCapture', 'AnomalyControlServer', 'AnomalyShaders', 'AnomalyBench', 'EuropaPhaseAProbe')


def require_rules_version(host, pair):
    if host not in EXPECTED or tuple(pair) != EXPECTED[host][2]:
        raise RuntimeError('Rules-version mismatch; no build may be scheduled')


def product_version(path):
    api = ctypes.WinDLL('version', use_last_error=True)
    api.GetFileVersionInfoSizeW.argtypes = [wintypes.LPCWSTR, ctypes.POINTER(wintypes.DWORD)]
    api.GetFileVersionInfoSizeW.restype = wintypes.DWORD
    ignored = wintypes.DWORD()
    size = api.GetFileVersionInfoSizeW(str(path), ctypes.byref(ignored))
    if not size:
        raise ctypes.WinError(ctypes.get_last_error())
    buffer = ctypes.create_string_buffer(size)
    api.GetFileVersionInfoW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD, ctypes.c_void_p]
    if not api.GetFileVersionInfoW(str(path), 0, size, buffer):
        raise ctypes.WinError(ctypes.get_last_error())
    pointer, length = ctypes.c_void_p(), wintypes.UINT()
    api.VerQueryValueW.argtypes = [ctypes.c_void_p, wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_void_p), ctypes.POINTER(wintypes.UINT)]
    if not api.VerQueryValueW(buffer, '\\', ctypes.byref(pointer), ctypes.byref(length)):
        raise ctypes.WinError(ctypes.get_last_error())
    data = ctypes.cast(pointer, ctypes.POINTER(wintypes.DWORD))
    if data[0] != 0xFEEF04BD:
        raise RuntimeError('Invalid compiler version resource')
    return '.'.join(str(n) for n in (data[4] >> 16, data[4] & 65535, data[5] >> 16, data[5] & 65535))


def compiler_path(action):
    direct = Path(action['CommandPath'])
    if direct.name.casefold() == 'cl.exe':
        return direct
    match = re.search(r'-compiler="([^"]+cl\.exe)"', action['CommandArguments'], re.I)
    return Path(match[1]) if match else None


def toolchain_responses(data, host, archive):
    from working_build import response_paths, response_flags
    archive.mkdir(parents=True, exist_ok=False)
    versions, rows = {}, []
    for index, action in enumerate(data['Actions']):
        compiler = compiler_path(action)
        if action['Type'] != 'Compile' or compiler is None:
            continue
        key = str(compiler.resolve(strict=True))
        if key not in versions:
            versions[key] = product_version(compiler)
        responses = response_paths(action)
        if not responses:
            raise RuntimeError('Missing response for C++ compile command')
        for offset, path in enumerate(responses):
            flags, body = response_flags(path)
            sdks = sorted(set(re.findall(r'Windows Kits[\\/]10[\\/](?:include|lib)[\\/](10\.\d+\.\d+\.\d+)', body, re.I)))
            saved = archive / f'{index:04}-{offset:02}.response'
            saved.write_bytes(path.read_bytes())
            row = dict(flags, command=action['CommandPath'], arguments=action['CommandArguments'],
                       compiler=key, compiler_product_version=versions[key], SDKs=sdks,
                       archived_response=str(saved), action_index=index)
            expected_compiler, expected_sdk, _ = EXPECTED[host]
            if not versions[key].startswith(expected_compiler + '.') and versions[key] != expected_compiler:
                raise RuntimeError('Unexpected compiler product version: ' + json.dumps(row))
            if sdks != [expected_sdk]:
                raise RuntimeError('Unexpected or absent SDK in compile response: ' + json.dumps(row))
            if host == '425' and any(name in Path(p).parts for p in action.get('ProducedItems', []) for name in IA_MODULES[:-1]) and flags['standards'] != ['/std:c++17']:
                raise RuntimeError('Unexpected 4.25 plugin C++ standard: ' + json.dumps(row))
            rows.append(row)
    return rows


def diagnostics(text):
    from working_build import diagnostics as base
    result = []
    for row in base(text):
        line = row['diagnostic']
        severity = 'warning' if re.search(r'\bwarning\b', line, re.I) else 'error'
        code = re.search(r'\b(?:fatal error|error|warning)\s+([A-Z]+\d+)\s*:', line, re.I)
        environment = severity == 'warning' and 'BuildConfiguration.xml' in line and any(x in line for x in ('bAllowUBAExecutor', 'MemoryPerActionBytes'))
        result.append(dict(row, severity=severity, code=code[1] if code else 'UBT',
                           category='accepted_environment' if environment else 'compiler_or_build',
                           new_port_warning=severity == 'warning' and not environment))
    return result


def validate_build(result, require_compile=True):
    require_rules_version(result['host'], result['rules_version'])
    if result['exit_code'] != 0 or any(d['severity'] == 'error' for d in result['diagnostics']):
        raise RuntimeError('A1 rejects exit/error evidence')
    rows = result['actual_compile_responses']
    if require_compile and not rows:
        raise RuntimeError('A1 rejects a missing compile command')
    expected_compiler, expected_sdk, _ = EXPECTED[result['host']]
    for row in rows:
        if not row.get('command') or not row.get('arguments'):
            raise RuntimeError('A1 missing actual compile command')
        version = row['compiler_product_version']
        if version != expected_compiler and not version.startswith(expected_compiler + '.'):
            raise RuntimeError('A1 unexpected compiler product version')
        if row['SDKs'] != [expected_sdk]:
            raise RuntimeError('A1 unexpected SDK')
    if result['action_counts']['engine'] or result['action_counts']['engine_plugin']:
        raise RuntimeError('A1 forbidden engine actions')
    return True


def strict_response_problems(body, sources):
    problems = []
    if re.search(r'(?i)(?:^|\s)/Y[cu](?:\s|"|\S)', body):
        problems.append('PCH flag in strict compile response')
    if any(Path(s).name.startswith('Module.') and not Path(s).name.endswith('.gen.cpp') for s in sources):
        problems.append('unity source in strict compile action')
    return problems


def shipping_names(value):
    text = json.dumps(value)
    return sorted(name for name in IA_MODULES if re.search(r'(?<![A-Za-z0-9])' + name + r'(?![A-Za-z0-9])', text, re.I))


def self_tests(output):
    import copy
    cases = []
    good = {'host': '425', 'rules_version': [4, 25], 'exit_code': 0, 'diagnostics': [],
            'actual_compile_responses': [{'command': 'cl-filter.exe', 'arguments': '-compiler=cl.exe @x.response',
              'compiler_product_version': '14.16.27051.0', 'SDKs': ['10.0.17763.0']}],
            'action_counts': {'project': 1, 'plugin': 3, 'engine': 0, 'engine_plugin': 0}}
    assert validate_build(good)
    cases.append({'name': 'positive', 'result': 'PASS'})
    mutations = {
        'nonzero_exit': lambda x: x.update(exit_code=6),
        'added_error': lambda x: x['diagnostics'].extend(diagnostics('D:/SYNTHETIC/Source/AnomalyInjector/test.cpp(12): error C2065: undeclared identifier')),
        'compiler_version': lambda x: x['actual_compile_responses'][0].update(compiler_product_version='14.44.0'),
        'missing_compile': lambda x: x.update(actual_compile_responses=[]),
        'missing_command': lambda x: x['actual_compile_responses'][0].update(command=''),
        'SDK_mismatch': lambda x: x['actual_compile_responses'][0].update(SDKs=['10.0.22621.0']),
        'rules_version_before_schedule': lambda x: x.update(rules_version=[4, 26]),
        'engine_action': lambda x: x['action_counts'].update(engine=1),
        'engine_plugin_action': lambda x: x['action_counts'].update(engine_plugin=1),
    }
    for name, mutate in mutations.items():
        value = copy.deepcopy(good)
        mutate(value)
        try:
            validate_build(value)
        except RuntimeError as error:
            cases.append({'name': name, 'result': 'REJECTED', 'reason': str(error)})
        else:
            raise RuntimeError('Ineffective A1 can-fail: ' + name)
    assert not strict_response_problems('/std:c++17', ['Single.cpp'])
    for name, body, cpp in [('PCH', '/Yu"Shared.h"', ['Single.cpp']), ('unity', '/std:c++17', ['Module.AnomalyInjector.cpp'])]:
        problems = strict_response_problems(body, cpp)
        assert problems
        cases.append({'name': 'strict_' + name, 'result': 'REJECTED', 'reason': problems})
    assert shipping_names({'BuildProducts': ['UnrealEditor-AnomalyInjector.dll']}) == ['AnomalyInjector']
    assert shipping_names({'Modules': ['EuropaPhaseAProbe']}) == ['EuropaPhaseAProbe']
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps({'result': 'PASS', 'synthetic_only': True, 'cases': cases}, indent=2) + '\n')


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, required=True)
    self_tests(parser.parse_args().output)
