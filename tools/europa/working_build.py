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
    arguments = action.get('CommandArguments', '')
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

def compile_sources(action, response_archive=None):
    extensions = ('.cpp', '.c', '.cc', '.cxx', '.mm', '.rc', '.rc2')
    sources = {Path(p) for p in action.get('PrerequisiteItems', []) if Path(p).suffix.lower() in extensions}
    for response in response_paths(action):
        _, body = response_flags(Path((response_archive or {}).get(str(response), response)))
        for line in body.splitlines():
            value = line.strip()
            if value.startswith(('/Tp', '/Tc')):
                value = value[3:].strip()
            if value.startswith('/'):
                continue
            value = value.strip('"')
            if Path(value).suffix.lower() in extensions:
                path = Path(value)
                sources.add(path if path.is_absolute() else Path(action['WorkingDirectory']) / path)
    if Path(action.get('CommandPath', '')).name.casefold() == 'rc.exe':
        for pair in re.findall(r'"([^"\r\n]+\.rc2?)"|(?:^|\s)([^"\s]+\.rc2?)(?:\s|$)', action.get('CommandArguments', ''), re.I):
            path = Path(next(p for p in pair if p))
            sources.add(path if path.is_absolute() else Path(action['WorkingDirectory']) / path)
    return sorted(sources)


def engine_source_inputs(action, project, engines):
    if action['Type'] != 'Compile':
        return []
    sources = compile_sources(action)
    result = set()
    for source in sources:
        if any(source.resolve().is_relative_to(root.resolve()) for root in engines):
            result.add(str(source))
        elif source.is_file() and source.resolve().is_relative_to(project.parent.resolve()) and 'Intermediate' in source.parts and source.name.startswith(('Module.', 'SharedPCH.', 'PCH.')):
            for include in re.findall(r'^\s*#include\s+"([^"]+)"', source.read_text(encoding='utf-8-sig'), re.M):
                target = Path(include)
                if not target.is_absolute():
                    target = Path(action['WorkingDirectory']) / target
                if any(target.resolve().is_relative_to(root.resolve()) for root in engines):
                    result.add(str(target))
    return sorted(result)


def classify_actions(path, project, engine):
    data = json.loads(path.read_text(encoding='utf-8-sig'))
    counts = {'project': 0, 'plugin': 0, 'engine_source_in_host': 0, 'engine': 0, 'engine_plugin': 0}
    engine_roots = [Path('D:/UESource/UnrealEngine'), Path('E:/EpicGames/UE_4.25')]
    rows, responses, binary_paths = [], {}, set()
    for action in data['Actions']:
        outputs = [Path(p) for p in action.get('ProducedItems', [])]
        mutations = outputs + [Path(p) for p in action.get('DeleteItems', [])]
        if action.get('DependencyListFile'):
            mutations.append(Path(action['DependencyListFile']))
        if any(p.resolve().is_relative_to(root.resolve()) for p in mutations for root in engine_roots):
            category = 'engine'
        elif any(not p.resolve().is_relative_to(project.parent.resolve()) for p in mutations):
            raise RuntimeError('Action writes outside its owned host: ' + json.dumps(action))
        elif any('/plugins/' in p.as_posix().lower() for p in outputs):
            category = 'plugin'
        else:
            category = 'project'
        engine_inputs = engine_source_inputs(action, project, engine_roots)
        if category == 'project' and action['Type'] == 'Compile' and engine_inputs:
            category = 'engine_source_in_host'
        counts[category] += 1
        if category == 'engine' and any(p.resolve().is_relative_to((root / 'Engine/Plugins').resolve()) for p in mutations for root in engine_roots):
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
            'arguments': action['CommandArguments'], 'engine_source_inputs': engine_inputs, 'outputs': list(map(str, outputs)), 'responses': refs})
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
    if host in ('51port', '51base'):
        environ['DOTNET_ROOT'] = str(engine / 'Engine/Binaries/ThirdParty/DotNet/6.0.302/windows')
        environ['DOTNET_MULTILEVEL_LOOKUP'] = '0'
    environ['EUROPA_BUILD_CHILDREN'] = '1'


def main():
    from a4_build import main as qualified_main
    qualified_main()


if __name__ == '__main__':
    main()
