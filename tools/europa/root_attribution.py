import datetime
import fnmatch
import functools
import json
import os
from pathlib import Path, PureWindowsPath
import re
import sys

ROOT = Path('D:/IA_Europa')
RUN = Path('D:/IntrusiveAnomalies/_relay/runs/2026-10-16-094-01-europa-phase-a-plan')
PATTERNS = (
    'europa*', 'ia_canary*', '094*-*', '094*_*.py', 'probe*', 'owner-leg*', 'fixture-*',
    'receipt*.json*', '*-receipt.json', 'child-*.json', 'summary.json', '*-ledger*', 'ledger*', 'job-config.json',
    'runtime.log', 'crashclient.log', 'ue4*.log', 'unreal*.log', 'port*.log',
    'makefile.bin', 'makefile.ubt', '*.uhtmanifest', '*.uhtpath', '*.target', '*.modules',
    '*.uasset', '*.umap', '*.uexp', '*.ubulk', '*.pak', '*.ucas', '*.utoc',
    '*.ddc', '*.ddp', '*.udd', '*.upipelinecache', '*.ushaderbytecode', '*.ushadercache',
    'deriveddatacache', 'ddc', 'shaderworking', 'unrealshaderworkingdir*')


def norm(path):
    return str(PureWindowsPath(str(path))).rstrip('\\').casefold()


def related(a,b):
    a,b=norm(a),norm(b)
    return a==b or a.startswith(b+'\\') or b.startswith(a+'\\')


def root_mentions(values):
    result=set()
    for value in values:
        for drive,name in re.findall(r'(?i)\b([cde]):[/\\]([^/\\\r\n;"<>|]+)',str(value)):
            name=name.strip().rstrip("')],}")
            if name:result.add(norm(drive+':/'+name))
    return sorted(result)


def context(argv,cwd,env):
    return dict(command_line=root_mentions(argv),working_folder=root_mentions([cwd]),
                environment=root_mentions(env.values()),values_recorded=False)


def contexts(job):
    rows=[context(sys.argv,Path.cwd(),os.environ)]
    for p in Path(job).rglob('root-contexts.jsonl'):
        for line in p.read_text(encoding='utf-8').splitlines():
            rows.append(json.loads(line)['references'])
    return rows


@functools.lru_cache(maxsize=1)
def protected_roots():
    result=set()
    with (ROOT/'Receipts/P9/baseline-v2/full-sha256.jsonl').open(encoding='utf-8') as stream:
        for line in stream:
            parts=PureWindowsPath(json.loads(line)['path']).parts
            if len(parts)>1:result.add(norm(PureWindowsPath(*parts[:2])))
    return sorted(result)


def port_patterns():
    names={p.name.casefold() for p in Path(__file__).parent.iterdir() if p.is_file()}
    names.update(p.name.casefold() for p in RUN.iterdir() if p.is_file())
    return sorted(set(PATTERNS)|names)


def metadata(path):
    path=Path(path)
    result=dict(path=str(path),created_utc=None,files_by_extension={},files=0,bytes=0,
                port_pattern_matches=[],errors=[],file_contents_opened=0)
    patterns=port_patterns()
    try:
        stat=path.stat(follow_symlinks=False)
        stamp=getattr(stat,'st_birthtime',stat.st_ctime)
        result['created_utc']=datetime.datetime.fromtimestamp(stamp,datetime.timezone.utc).isoformat()
        stack=[path]
        while stack:
            item=stack.pop()
            stat=item.stat(follow_symlinks=False)
            if stat.st_file_attributes & 1024:
                result['errors'].append(dict(path=str(item),reason='Reparse entry cannot prove absence of port files'))
                continue
            if any(fnmatch.fnmatchcase(item.name.casefold(),pattern) for pattern in patterns):
                result['port_pattern_matches'].append(str(item))
            if item.is_dir():
                with os.scandir(item) as entries:stack.extend(Path(e.path) for e in entries)
            else:
                extension=item.suffix.casefold() or '<none>'
                totals=result['files_by_extension'].setdefault(extension,dict(count=0,bytes=0))
                totals['count']+=1;totals['bytes']+=stat.st_size
                result['files']+=1;result['bytes']+=stat.st_size
    except OSError as error:
        result['errors'].append(dict(path=str(path),type=type(error).__name__))
    return result


def decide(change,own_contexts,protected,declared,details=None):
    path=change['path'];removed=change.get('after_kind') is None
    protected_match=any(related(path,p) for p in protected)
    declared_match=any(related(path,p) for p in declared)
    adjacent=not removed and norm(path).startswith('d:\\') and (
        PureWindowsPath(path).parent in (PureWindowsPath(ROOT).parent,PureWindowsPath(RUN).parent))
    mentions=[dict(index=i,field=k,root=value) for i,row in enumerate(own_contexts)
        for k in ('command_line','working_folder','environment') for value in row.get(k,[]) if related(path,value)]
    row=dict(authority='094-24-ruling EZ1',path=path,change='removed' if removed else 'new',
        protected=protected_match,declared=declared_match,adjacent_D=adjacent,own_process_mentions=mentions,
        metadata=details,writer=None,file_contents_opened=0)
    if protected_match:
        row.update(result='RED',reason='P9-protected root entry removed or changed')
    elif declared_match or adjacent:
        row.update(result='STOP',reason='Port root or adjacent D entry remains port-attributed')
    elif removed:
        row.update(result='FOREIGN',reason='Removed entry is outside declared and P9-protected roots')
    elif mentions:
        row.update(result='STOP',reason='Port command line, working folder or environment names this root')
    elif details is None or details['errors']:
        row.update(result='STOP',reason='Metadata cannot prove absence of port-pattern files')
    elif details['port_pattern_matches']:
        row.update(result='STOP',reason='New entry holds a port-pattern file')
    else:
        row.update(result='FOREIGN',reason='No own-process path reference and no port-pattern file; metadata only')
    return row


def classify(change,job):
    if 'root_attribution' not in change:
        declared=[ROOT,RUN,Path('E:/EpicGames/UE_4.25'),Path('E:/Unreal Projects/StylizedParisStreet')]
        removed=change.get('after_kind') is None
        details=None if removed else metadata(change['path'])
        change['root_attribution']=decide(change,contexts(job),protected_roots(),declared,details)
    result=change['root_attribution']
    return ('foreign' if result['result']=='FOREIGN' else 'STOP'),result['authority']+': '+result['reason']
