import hashlib
import json
from pathlib import Path
import re

def cleanup(plan, project, receipt, dry_run=False):
    host = project.parent.resolve(strict=True)
    from job_scope import load
    from process_wrapper import HOSTS
    allowed = {HOSTS[row[0]][0].parent.resolve(strict=True) for row in load()['build_pairs']}
    if host not in allowed:
        raise RuntimeError('EF2 cleanup outside an owned build host')
    data = json.loads(plan.read_text(encoding='utf-8-sig'))
    targets = set()
    intermediates = {host / 'Intermediate'}
    for action in data['Actions']:
        mutations = action.get('ProducedItems', []) + action.get('DeleteItems', [])
        if action.get('DependencyListFile'):
            mutations.append(action['DependencyListFile'])
        for raw in mutations:
            p = Path(raw).resolve()
            if not p.is_relative_to(host):
                raise RuntimeError('Refusing engine/foreign cleanup action: ' + raw)
            parts = p.relative_to(host).parts
            if 'Intermediate' in parts:
                index = parts.index('Intermediate')
                intermediates.add(host.joinpath(*parts[:index + 1]))
            targets.add(p)
    manifests = list((host / 'Intermediate').rglob('*.uhtmanifest'))
    for manifest in manifests:
        for module in json.loads(manifest.read_text(encoding='utf-8-sig')).get('Modules', []):
            raw = module.get('OutputDirectory') or module.get('GeneratedCodeDirectory')
            if raw:
                p = Path(raw).resolve()
                if p.is_relative_to(host) and 'Intermediate' in p.relative_to(host).parts:
                    parts = p.relative_to(host).parts
                    intermediates.add(host.joinpath(*parts[:parts.index('Intermediate') + 1]))
    for root in intermediates:
        if root.exists():
            for p in root.rglob('*'):
                if p.is_symlink() or p.stat(follow_symlinks=False).st_file_attributes & 1024:
                    raise RuntimeError('Refusing cleanup through reparse point: ' + str(p))
                if p.is_file():
                    targets.add(p.resolve(strict=True))
    rows = []
    for p in sorted(targets):
        if not p.exists():
            continue
        resolved = p.resolve(strict=True)
        parts = resolved.relative_to(host).parts
        if not resolved.is_relative_to(host) or not any(part in ('Binaries', 'Intermediate') for part in parts) or not resolved.is_file():
            raise RuntimeError('EF2 target not an owned generated file: ' + str(p))
        with p.open('rb') as source:
            value = hashlib.file_digest(source, 'sha256').hexdigest()
        rows.append({'path': str(resolved), 'size': p.stat().st_size, 'mtime_ns': p.stat().st_mtime_ns, 'sha256': value})
    result = {'action_plan': str(plan), 'action_plan_sha256': hashlib.sha256(plan.read_bytes()).hexdigest(),
        'host': str(host), 'files': rows, 'bytes': sum(r['size'] for r in rows), 'intermediate_roots': list(map(str, sorted(intermediates))),
        'basis': 'Action products/deletes/dependencies; affected host/plugin Intermediate trees include UBT caches, response/PDB side files and UHT generated files. No Source, Config, Content or engine cleanup.', 'dry_run': dry_run}
    receipt.parent.mkdir(parents=True, exist_ok=True)
    receipt.write_text(json.dumps(result, indent=2) + '\n')
    if not dry_run:
        for row in rows:
            p = Path(row['path'])
            if p.resolve(strict=True) != p or (p.stat().st_size, p.stat().st_mtime_ns) != (row['size'], row['mtime_ns']):
                raise RuntimeError('Cleanup target changed after receipt')
            p.unlink()
        result['removed_count'] = len(rows)
        result['result'] = 'PASS' if all(not Path(r['path']).exists() for r in rows) else 'STOP'
        receipt.with_name(receipt.stem + '-result.json').write_text(json.dumps(result, indent=2) + '\n')
    return result
