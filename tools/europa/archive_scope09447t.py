import hashlib
import json
from pathlib import Path

JOB=Path('D:/IA_Europa/Receipts/09447t')
ARCHIVE=Path('E:/IA_BuildCache/09447t-evidence')

def validate(change,intent,actual):
    if intent.get('job')!=str(JOB) or intent.get('root')!=str(ARCHIVE):raise RuntimeError('Wrong archive job/root')
    path=Path(change['path'])
    if not path.is_absolute() or '..' in path.parts or not path.is_relative_to(ARCHIVE):raise RuntimeError('Archive path escaped exact root')
    records=intent.get('files',[])
    paths=[r['archive'] for r in records]
    if not paths or len(paths)!=len(set(paths)) or str(path) not in paths:raise RuntimeError('Unplanned archive file')
    row=records[paths.index(str(path))]
    if change.get('before_kind') is not None or change.get('after_kind')!='file':raise RuntimeError('Immutable archive replacement/removal refused')
    if any(actual[k]!=row[k] for k in ('size','mtime_ns','sha256')):raise RuntimeError('Archive bytes/time/hash mismatch')
    if (change.get('after_size'),change.get('after_mtime_ns'))!=(row['size'],row['mtime_ns']):raise RuntimeError('Archive ledger mismatch')
    return 'owned_E_archive','OD15 exact09447t changed-binary archive; pinned per-file intent'

def classify(change,kind,scope):
    if Path(scope['job_root'])!=JOB or kind!='setup' or scope.get('archive_authorized') is not True:raise RuntimeError('Archive operation not admitted')
    path=Path(scope.get('archive_intent',''))
    pin=scope.get('archive_intent_sha256')
    if path.parent!=JOB or not pin or hashlib.sha256(path.read_bytes()).hexdigest()!=pin:raise RuntimeError('Missing/unpinned archive intent')
    target=Path(change['path'])
    if target.resolve(strict=True)!=target or any(p.lstat().st_file_attributes&1024 for p in (target,*target.parents)):raise RuntimeError('Reparse archive path')
    before=target.stat()
    with target.open('rb') as f:digest=hashlib.file_digest(f,'sha256').hexdigest()
    after=target.stat()
    if (before.st_size,before.st_mtime_ns,before.st_ino)!=(after.st_size,after.st_mtime_ns,after.st_ino):raise RuntimeError('Archive changed during hash')
    return validate(change,json.loads(path.read_text()),dict(size=after.st_size,mtime_ns=after.st_mtime_ns,sha256=digest))
