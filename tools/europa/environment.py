"""EK1/EK2: an allowlisted child environment and the native UBT string-list layout."""
import hashlib
import json
from pathlib import Path
import re
import struct

SECRET = re.compile(r'TOKEN|SECRET|PASSWORD|PASSWD|KEY|AUTH|COOKIE|CREDENTIAL', re.I)
IDENTIFIER_PREFIXES = ('CLAUDE_', 'CODEX_', 'ANTHROPIC_', 'OPENAI_')
SYSTEM = set('SYSTEMROOT WINDIR SYSTEMDRIVE COMSPEC PATH PATHEXT OS PROCESSOR_ARCHITECTURE PROCESSOR_ARCHITEW6432 PROCESSOR_IDENTIFIER PROCESSOR_LEVEL PROCESSOR_REVISION NUMBER_OF_PROCESSORS PROGRAMDATA PROGRAMFILES PROGRAMFILES(X86) PROGRAMW6432 COMMONPROGRAMFILES COMMONPROGRAMFILES(X86) COMMONPROGRAMW6432 ALLUSERSPROFILE USERPROFILE LOCALAPPDATA APPDATA HOMEDRIVE HOMEPATH USERNAME USERDOMAIN TEMP TMP INCLUDE LIB LIBPATH'.split())
JOB = set('EUROPA_JOB_CONFIG EUROPA_OPERATION_DIR EUROPA_DESCRIPTOR_PROBE EUROPA_BUILD_CHILDREN EUROPA_OWNED_ROOTS PYTHONDONTWRITEBYTECODE GIT_OPTIONAL_LOCKS'.split())
UE = set('UE-CMDLINEARGS UE-SHAREDDATACACHEPATH UE-LOCALDATACACHEPATH UE_DISABLECRASHREPORTER DOTNET_ROOT DOTNET_MULTILEVEL_LOOKUP DOTNET_CLI_HOME DOTNET_CLI_TELEMETRY_OPTOUT DOTNET_SKIP_FIRST_TIME_EXPERIENCE DOTNET_NOLOGO NUGET_PACKAGES UEBP_ENGINESAVEDFOLDER UEBP_LOGFOLDER UEBP_FINALLOGFOLDER'.split())
VS_PREFIXES = ('VS', 'VC', 'WINDOWSSDK', 'WINDOWSLIB', 'UNIVERSALCRT', 'UCRT', 'FRAMEWORK', '__VSCMD')

def secret_name(name):
    return bool(SECRET.search(name))

def forbidden_child_name(name):
    return secret_name(name) or name.upper().startswith(IDENTIFIER_PREFIXES) or name.upper() == 'CLAUDECODE'

def allowed_name(name):
    key = name.upper()
    return not forbidden_child_name(key) and (key in SYSTEM | JOB | UE or key.startswith(VS_PREFIXES))

def allowlisted(environ):
    upper = {k.upper(): v for k, v in environ.items()}
    if any(k in upper for k in ('GIT_AUTHOR_DATE', 'GIT_COMMITTER_DATE')):
        raise RuntimeError('W4 date variable present')
    return {k: v for k, v in upper.items() if allowed_name(k)}

def masked(environ):
    return {k: '<masked>' if v else '<empty>' for k, v in sorted(environ.items())}

def read_string(raw, pos, name=False):
    if pos < 0 or pos + 4 > len(raw):
        raise ValueError('truncated length')
    count = struct.unpack_from('<i', raw, pos)[0]
    pos += 4
    if count < 0 or count > (32767 if name else 1024 * 1024) or pos + count > len(raw):
        raise ValueError('invalid string extent')
    value = raw[pos:pos + count].decode('utf-8')
    if '\0' in value or (name and (not value or '=' in value or any(ord(c) < 32 for c in value))):
        raise ValueError('invalid environment string')
    return value, pos + count

def read_list(raw, start, anchor):
    count = struct.unpack_from('<i', raw, start)[0]
    if not 4 <= count <= 4096:
        raise ValueError('invalid environment count')
    cursor, pairs, anchor_seen = start + 4, {}, False
    for _ in range(count):
        key_start = cursor
        key, cursor = read_string(raw, cursor, True)
        value, cursor = read_string(raw, cursor)
        upper = key.upper()
        if upper in pairs:
            raise ValueError('duplicate environment name')
        pairs[upper] = value
        anchor_seen |= upper == 'PATH' and key_start == anchor
    if not anchor_seen or not {'PATH', 'SYSTEMROOT', 'TEMP', 'TMP'} <= pairs.keys():
        raise ValueError('environment identity markers absent')
    return {'offset': start, 'end': cursor, 'count': count, 'environment': pairs}

def decode(raw):
    marker = struct.pack('<i', 4) + b'PATH'
    accepted, attempted, cursor = {}, set(), 0
    while True:
        anchor = raw.find(marker, cursor)
        if anchor < 0:
            break
        cursor = anchor + len(marker)
        for start in range(max(0, anchor - 128 * 1024), anchor):
            if start in attempted:
                continue
            count = struct.unpack_from('<i', raw, start)[0]
            if not 4 <= count <= 4096:
                continue
            try:
                parsed = read_list(raw, start, anchor)
            except (ValueError, UnicodeDecodeError, struct.error):
                continue
            accepted[start] = parsed
    if len(accepted) != 1:
        raise RuntimeError('UBT environment list missing or ambiguous; accepted count=' + str(len(accepted)))
    return next(iter(accepted.values()))

def report_makefile(path, require_clean=False):
    raw = Path(path).read_bytes()
    decoded = decode(raw)
    values = decoded['environment']
    forbidden = sorted(k for k in values if forbidden_child_name(k))
    secrets = sorted(k for k, v in values.items() if secret_name(k) and v and v != '<redacted>')
    row = {k: decoded[k] for k in ('offset', 'end', 'count')}
    row.update(path=str(path), sha256=hashlib.sha256(raw).hexdigest(), environment=masked(values),
               forbidden_child_names=forbidden, nonempty_secret_names=secrets,
               result='PASS' if not require_clean or not forbidden else 'STOP')
    if require_clean and forbidden:
        raise RuntimeError('EK2 regenerated makefile contains forbidden names: ' + ','.join(forbidden))
    return row

def effective_receipt(plan, makefile, output):
    actual = decode(Path(makefile).read_bytes())['environment']
    delta = json.loads(Path(plan).read_text(encoding='utf-8-sig')).get('Environment', {})
    merged = dict(actual)
    merged.update({k.upper(): v for k, v in delta.items()})
    forbidden = sorted(k for k in merged if forbidden_child_name(k))
    result = {'result': 'STOP' if forbidden else 'PASS', 'makefile': str(makefile),
              'makefile_sha256': hashlib.sha256(Path(makefile).read_bytes()).hexdigest(),
              'source': 'Native UBT serialized environment plus exported effective delta',
              'effective_environment': masked(merged), 'exported_delta': masked(delta),
              'forbidden_child_names': forbidden}
    Path(output).write_text(json.dumps(result, indent=2)+'\n')
    if forbidden:
        raise RuntimeError('Effective child environment contains forbidden names: ' + ','.join(forbidden))
    for name in ('TEMP', 'TMP'):
        if not Path(merged[name]).resolve().is_relative_to(Path('D:/IA_Europa/Temp').resolve()):
            raise RuntimeError('Effective TEMP/TMP escaped owned D scratch')
    return result

def self_test():
    cases = []
    def check(name, passed):
        cases.append({'case': name, 'pass': bool(passed)})
        if not passed:
            raise AssertionError(name)
    def pack(values):
        data = struct.pack('<i', len(values))
        for key, value in values.items():
            for item in (key, value):
                encoded = item.encode('utf-8')
                data += struct.pack('<i', len(encoded)) + encoded
        return data
    values = {'ASL.LOG': 'x' * 69, 'PROGRAMFILES(X86)': 'program', 'PATH': 'path',
              'SYSTEMROOT': 'system', 'TEMP': 'scratch', 'TMP': 'scratch'}
    raw = pack(values)
    check('dotted_name_and_letter_length_byte', decode(b'prefix' + raw + b'suffix')['environment'] == values)
    for name, bad in [('truncated', raw[:-3]), ('ambiguous', raw + raw),
                       ('no_path', pack({k: v for k, v in values.items() if k != 'PATH'})),
                       ('nul_name', pack(dict(values, **{'BAD\0NAME': 'value'})))]:
        try:
            decode(bad)
        except RuntimeError:
            check(name + '_refused', True)
        else:
            check(name + '_refused', False)
    check('identifiers_not_secrets', not secret_name('CODEX_THREAD_ID') and not secret_name('CLAUDE_CODE_SESSION_ID'))
    for part in ('TOKEN', 'SECRET', 'PASSWORD', 'PASSWD', 'KEY', 'AUTH', 'COOKIE', 'CREDENTIAL'):
        check(part + '_secret_and_excluded', secret_name('FAKE_' + part) and not allowed_name('FAKE_' + part))
    fake = dict(values, CLAUDE_CODE_SESSION_ID='synthetic', CODEX_THREAD_ID='synthetic',
                CLAUDE_CODE_MESSAGING_TOKEN='synthetic', RANDOM='synthetic')
    clean = allowlisted(fake)
    check('child_identifiers_and_secret_dropped', not any(forbidden_child_name(k) for k in clean))
    check('unknown_name_dropped', 'RANDOM' not in clean)
    check('values_masked', set(masked(clean).values()) == {'<masked>'})
    return {'result': 'PASS', 'cases': cases}
