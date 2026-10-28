import hashlib
import ntpath

from environment import execution_forbidden, fingerprints


def selected(host):
    if host == '425':
        msvc = r'C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.16.27023'
        sdk = '10.0.17763.0'
        bins = [msvc + r'\bin\HostX64\x86', msvc + r'\bin\HostX64\x64']
    elif host in ('51base', '51port'):
        msvc = r'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130'
        sdk = '10.0.22621.0'
        bins = [msvc + r'\bin\Hostx64\x64']
    else:
        raise RuntimeError('Unqualified toolchain environment')
    kits = r'C:\Program Files (x86)\Windows Kits\10'
    return dict(host=host, msvc=msvc, sdk=sdk, kits=kits,
                prefix=[kits + '\\bin\\' + sdk + r'\x64', *bins])


def normalized(value):
    return ntpath.normcase(ntpath.normpath(value.strip('"')))


def toolchain_name(name):
    return name in ('INCLUDE', 'LIB', 'LIBPATH') or name.startswith(('WINDOWSSDK', 'VC'))


def inside_toolchain(value, policy):
    roots = [policy['msvc'], *[policy['kits'] + '\\' + part + '\\' + policy['sdk']
                             for part in ('bin', 'Include', 'Lib', 'UnionMetadata', 'References')]]
    entries = value.split(';')
    return bool(value) and all(entry and any(normalized(entry) == normalized(root) or
        normalized(entry).startswith(normalized(root) + '\\') for root in roots) for entry in entries)


def compare(expected, actual, policy):
    actual = {k.upper(): v for k, v in actual.items()}
    blocked = sorted(k for k in actual if execution_forbidden(k))
    hashes = {k: hashlib.sha256(v.encode()).hexdigest() for k, v in actual.items() if k not in blocked}
    missing = sorted(set(expected) - set(hashes))
    changed = sorted(k for k in expected.keys() & hashes.keys() if expected[k] != hashes[k])
    added = sorted(set(hashes) - set(expected))
    admitted, violations = {}, []
    for name in changed + added:
        if name == 'PATH' and 'PATH' in expected:
            parts = actual[name].split(';')
            candidates = []
            for length in range(1, len(policy['prefix']) + 1):
                prefix = parts[:length]
                permitted = [normalized(p) for p in policy['prefix']]
                indexes = [permitted.index(normalized(p)) if normalized(p) in permitted else -1 for p in prefix]
                suffix = ';'.join(parts[length:])
                if -1 not in indexes and indexes == sorted(set(indexes)) and hashlib.sha256(suffix.encode()).hexdigest() == expected['PATH']:
                    candidates.append(prefix)
            if len(candidates) == 1:
                admitted[name] = dict(prefix=candidates[0], base_suffix_byte_identical=True)
            else:
                violations.append(name)
        elif toolchain_name(name) and inside_toolchain(actual[name], policy):
            admitted[name] = dict(value='<masked>', selected_toolchain_only=True)
        else:
            violations.append(name)
    return dict(result='STOP' if blocked or missing or violations else 'PASS', blocked_names=blocked,
        missing_base_names=missing, changed_base_names=changed, added_names=added,
        admitted_toolchain_changes=admitted, unadmitted_changes=violations, selected_toolchain=policy)


def self_tests():
    from environment import decode
    import struct
    rows = []
    base = dict(PATH=r'C:\Windows;D:\Base', SYSTEMROOT=r'C:\Windows', TEMP=r'D:\Scratch', TMP=r'D:\Scratch',
                VULKAN_SDK='sdk-v', VK_SDK_PATH='sdk-vk', LINUX_MULTIARCH_ROOT='sdk-l')
    def decoded(values):
        parts = [struct.pack('<i', len(values))]
        for pair in values.items():
            for value in pair:
                raw = value.encode()
                parts += [struct.pack('<i', len(raw)), raw]
        return decode(b''.join(parts))['environment']
    for host in ('425', '51port'):
        policy = selected(host)
        good = dict(base, PATH=';'.join(policy['prefix'])+';'+base['PATH'],
            VC_COMPILER_PATH=policy['msvc']+r'\bin\HostX64\x64\cl.exe', VC_COMPILER_DIR=policy['msvc']+r'\bin\HostX64\x64')
        mutants = [('selected_prefix', good, 'PASS'), ('missing_base_entry', dict(good, PATH=good['PATH'].replace(';D:\\Base','')), 'STOP'),
            ('foreign_prefix', dict(good, PATH=r'D:\Foreign;'+good['PATH']), 'STOP'),
            ('canary', dict(good, IA_CANARY_TOKEN='synthetic'), 'STOP'),
            ('base_SDK_missing', {k:v for k,v in good.items() if k != 'VULKAN_SDK'}, 'STOP'),
            ('foreign_VC', dict(good, VC_COMPILER_DIR=r'D:\Foreign'), 'STOP'),
            ('unknown_addition', dict(good, UNRULED='synthetic'), 'STOP'),
            ('base_value_changed', dict(good, LINUX_MULTIARCH_ROOT='changed'), 'STOP')]
        for name, values, expected in mutants:
            result = compare(fingerprints(base), decoded(values), policy)
            if result['result'] != expected:
                raise AssertionError(host + ':' + name)
            rows.append(dict(host=host, case=name, expected=expected, result=result['result']))
    return dict(result='PASS', cases=rows)
