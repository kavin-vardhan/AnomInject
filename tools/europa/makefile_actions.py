from pathlib import Path
import re
import struct

from environment import decode


class Reader:
    def __init__(self, raw, host):
        self.raw, self.host, self.pos, self.objects = raw, host, 0, []

    def integer(self):
        value = struct.unpack_from('<i', self.raw, self.pos)[0]
        self.pos += 4
        return value

    def byte(self):
        value = self.raw[self.pos]
        self.pos += 1
        return value

    def string(self):
        length = self.integer()
        if length == -1:
            return None
        if not 0 <= length <= len(self.raw) - self.pos:
            raise ValueError('Invalid native archive string extent')
        value = self.raw[self.pos:self.pos+length].decode('utf-8')
        self.pos += length
        return value

    def array(self, function):
        count = self.integer()
        if count == -1:
            return []
        if not 0 <= count <= 200000:
            raise ValueError('Invalid native archive count')
        return [function() for _ in range(count)]

    def reference(self):
        index = self.integer()
        if index == -1:
            return None
        if not 0 <= index <= len(self.objects):
            raise ValueError('Invalid native archive reference')
        if index == len(self.objects):
            self.objects.append(self.string())
        return self.objects[index]

    def config(self):
        self.integer()
        for _ in range(4):
            self.string()
        self.array(self.string)

    def action(self):
        if self.host != '425':
            serializer = self.reference()
            if serializer == 'VCCompileActionSerializer':
                return self.vc_action()
            if serializer != 'DefaultActionSerializer':
                raise ValueError('Unqualified native action serializer')
        types = ['BuildProject', 'Compile', 'CreateAppBundle', 'GenerateDebugInfo', 'Link', 'WriteMetadata', 'PostBuildStep', 'ParseTimingInfo']
        if self.host != '425':
            types[1:1] = ['GatherModuleDependencies', 'CompileModuleInterface']
        row = dict(Type=types[self.byte()], WorkingDirectory=self.string())
        if self.host == '425':
            self.byte()
        row['CommandPath'], row['CommandArguments'] = self.string(), self.string()
        if self.host != '425':
            row['CommandVersion'] = self.string()
        row['CommandDescription'], row['StatusDescription'] = self.string(), self.string()
        for name in ('bCanExecuteRemotely', 'bCanExecuteRemotelyWithSNDBS', 'bIsGCCCompiler', 'bShouldOutputStatusDescription', 'bProducesImportLibrary'):
            row[name] = bool(self.byte())
        for name in ('PrerequisiteItems', 'ProducedItems', 'DeleteItems'):
            row[name] = self.array(self.reference)
        row['DependencyListFile'] = self.reference()
        if self.host != '425':
            row['bUseActionHistory'] = bool(self.byte())
        return row

    def vc_action(self):
        kind, compiler, compiler_type, version = self.integer(), self.reference(), self.integer(), self.string()
        fields = {name: self.reference() for name in ('source', 'object', 'preprocessed', 'dependency', 'interface', 'timing', 'response', 'create_pch', 'using_pch', 'through_header')}
        self.array(self.string)
        self.array(self.string)
        self.array(self.string)
        self.array(self.reference)
        args = self.array(self.string)
        show, remote, sndbs = self.byte(), self.byte(), self.byte()
        prerequisites, outputs, deletes = self.array(self.reference), self.array(self.reference), self.array(self.reference)
        prerequisites += [fields[k] for k in ('response', 'source', 'using_pch') if fields[k]]
        outputs += [fields[k] for k in ('object', 'preprocessed', 'dependency', 'timing', 'create_pch') if fields[k]]
        if kind != 3:
            raise ValueError('Unqualified VC action type')
        arguments = '@"' + fields['response'] + '"' if fields['response'] else ' '.join(args)
        command = compiler
        dep = fields['dependency']
        if dep and not dep.endswith(('.json', '.d')):
            command = r'D:\UESource\UnrealEngine\Engine\Build\Windows\cl-filter\cl-filter.exe'
            prefix = ['-dependencies="' + dep + '"']
            if fields['timing']:
                prefix.append('-timing="' + fields['timing'] + '"')
            if show:
                prefix.append('-showincludes')
            arguments = ' '.join(prefix + ['-compiler="'+compiler+'"', '--', '"'+compiler+'"', arguments, '/showIncludes'])
        return dict(Type='Compile', WorkingDirectory=r'D:\UESource\UnrealEngine\Engine\Source', CommandPath=command,
            CommandArguments=arguments, CommandVersion=version, CommandDescription='Compile',
            StatusDescription=Path(fields['source']).name if fields['source'] else 'Compiling',
            PrerequisiteItems=prerequisites, ProducedItems=outputs, DeleteItems=deletes, DependencyListFile=dep,
            native_serializer='VCCompileActionSerializer', command_reconstructed_from_native_fields=True)


def read(path, host):
    raw = Path(path).read_bytes()
    reader = Reader(raw, host)
    version = reader.integer()
    if version != (19 if host == '425' else 33):
        raise ValueError('Unqualified native makefile version')
    if host != '425' and reader.byte() != 0:
        raise ValueError('Unqualified VProject makefile')
    reader.pos += 8
    reader.array(reader.string)
    for _ in range(4):
        reader.string()
    reader.integer()
    if host != '425':
        reader.byte()
    reader.array(reader.config)
    reader.byte()
    reader.byte()
    if host != '425':
        for _ in range(4):
            reader.array(reader.string)
    reader.array(reader.string)
    if host != '425':
        reader.array(reader.string)
    reader.array(reader.string)
    if host != '425' and reader.integer() != 0:
        raise ValueError('Unqualified native pre-build targets')
    actions = reader.array(reader.action)
    environment = decode(raw)
    if reader.pos != environment['offset']:
        raise ValueError('Native action reader did not end at independently decoded environment boundary')
    return dict(Actions=actions, Environment={}, archive_version=version, environment_offset=reader.pos,
                action_count=len(actions), source='Read-only stock TargetMakefile/Action serialization')


def status_lines(text, complete=True):
    rows = []
    for line in text.splitlines():
        match = re.match(r'^\s*\[(\d+)/(\d+)\](.*)$', line)
        if not match:
            continue
        index, total = int(match[1]), int(match[2])
        description = match[3].strip()
        description = re.sub(r'^\[[^\]]+ (?:Win64|Linux|Mac) (?:Development|Shipping|DebugGame)\]\s*', '', description)
        description = re.sub(r'^\(Wall: [0-9.]+s(?: CPU: [0-9.]+s)?\)\s*', '', description)
        if description.startswith('Command:'):
            continue
        if not description or description.endswith(' cancelled'):
            raise RuntimeError('Unexecuted or empty action status')
        rows.append(dict(index=index, total=total, description=description))
    if rows:
        expected = rows[0]['total'] if complete else len(rows)
        if len({r['total'] for r in rows}) != 1 or sorted(r['index'] for r in rows) != list(range(1, expected + 1)):
            raise RuntimeError('Incomplete executed action denominator')
    return rows


def require_host_outputs(action):
    roots = [Path('D:/UESource/UnrealEngine'), Path('E:/EpicGames/UE_4.25')]
    paths = action.get('ProducedItems', []) + action.get('DeleteItems', [])
    if action.get('DependencyListFile'):
        paths.append(action['DependencyListFile'])
    for raw in paths:
        path = Path(raw)
        if not path.is_absolute():
            path = Path(action['WorkingDirectory']) / path
        if any(path.resolve().is_relative_to(root.resolve()) for root in roots):
            raise RuntimeError('Engine output in executed action')


def executed(graph, text, complete=True):
    rows = status_lines(text, complete)
    selected, used = [], set()
    for row in rows:
        short = re.sub(r'\[x64\]\s*', '', row['description'])
        found = []
        for offset, action in enumerate(graph['Actions']):
            command = action.get('CommandDescription')
            if command is None:
                command = Path(action['CommandPath']).stem
            status = action['StatusDescription']
            if short in (status, (command + ' ' + status).strip()):
                found.append(offset)
        if len(found) != 1:
            raise RuntimeError('Executed action has no unique native graph match: ' + row['description'])
        offset = found[0]
        if offset in used:
            raise RuntimeError('Native graph action executed more than once')
        action = graph['Actions'][offset]
        require_host_outputs(action)
        selected.append(action)
        used.add(offset)
        row['native_action_index'] = offset
    return dict(Actions=selected, Environment={}, executed_lines=rows, native_action_count=graph['action_count'])
