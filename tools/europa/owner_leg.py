import argparse
import contextlib
import hashlib
import json
import os
from pathlib import Path
import sys
import traceback
import uuid

from environment import boundary, filtered, masked, secret_name
from owner_attempts import ARMING_JOB, Guard, RUNTIME_SCOPE, create_json, validate
from owner_wait import SyntheticClock, wait_idle
from prepare_runtime import prepare, owned
from runtime_attribution import validate_redirects

ROOT = Path('D:/IA_Europa')
TOOLS = Path(__file__).resolve().parent


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def verify_files(rows):
    checked = []
    for row in rows:
        path = owned(Path(row['path']))
        if not path.is_file() or path.stat().st_size != row['size'] or digest(path) != row['sha256']:
            raise RuntimeError('Bound owner-leg file changed: '+str(path))
        checked.append(dict(path=str(path), sha256=row['sha256']))
    return checked


def check_ddc(project):
    path = project.parent / 'Config/DefaultEngine.ini'
    text = path.read_text(encoding='utf-8-sig')
    for graph in ('Europa425DDC', 'InstalledDerivedDataBackendGraph', 'DerivedDataBackendGraph'):
        section = text.split('['+graph+']', 1)[1].split('\n[', 1)[0]
        if 'D:/IA_Europa/Caches/425/Boot.ddc' not in section or 'D:/IA_Europa/Caches/425/Local' not in section or 'Shared' in section or 'EnvPathOverride' in section:
            raise RuntimeError('Owner DDC graph failed routing check.')
    return dict(path=str(path), sha256=digest(path), graphs=3)


def run(config_path, dry_run=False, owner_started=False, dry_run_samples=None, context=None):
    context = context if context is not None else {}
    config_path = owned(config_path)
    config = json.loads(config_path.read_text())
    print('Europa owner preview: checking authorization.', flush=True)
    validate(config, dry_run)
    if not dry_run and not owner_started:
        raise RuntimeError('Start this owner session using the double-click file.')
    if dry_run_samples is not None and not dry_run:
        raise RuntimeError('Synthetic gate readings are permitted only for a throwaway dry run.')
    if digest(Path(config['manifest'])) != config['manifest_sha256']:
        raise RuntimeError('Owner binary manifest changed.')
    verify_files(config.get('tooling_files', []))
    with Guard(config) as guard:
        slot, scope_path = guard.claim()
        root = Path(slot['job_root'])
        context['attempt_root'] = root
        result = dict(result='CHECKING', slot=slot['number'], launches=0, dry_run=dry_run,
            authorization_citation=config['authorization_citation'], runtime_scope=RUNTIME_SCOPE)
        admission_operation = None
        try:
            base = dict(os.environ)
            clean = filtered(base)
            clean['EUROPA_JOB_CONFIG'] = str(scope_path)
            contract = boundary(base, clean, {'EUROPA_JOB_CONFIG': str(scope_path)})
            os.environ.clear()
            os.environ.update(clean)
            from job_scope import load
            scope = load()
            if scope['runtime_scope'] != RUNTIME_SCOPE or scope['runtime_authorized'] is not True:
                raise RuntimeError('Attempt scope changed.')
            import write_ledger as ledger
            if ledger.SCOPE['config_path'] != str(scope_path):
                raise RuntimeError('Every attempt requires a fresh wrapper process.')
            ledger.floors(start=True)
            output = owned(root / ('owner-leg-dry-run' if dry_run else 'owner-leg'))
            output.mkdir(exist_ok=False)
            print('Attempt '+str(slot['number'])+' of 3: checking protected files, binaries and routing. This can take several minutes.', flush=True)
            manifest = json.loads(Path(config['manifest']).read_text())
            binaries = verify_files(manifest['files'])
            sources = verify_files(config['host_source_and_routing_files'])
            from prepare_runtime import HOSTS
            project, engine, graph, _ = HOSTS['425']
            ddc = check_ddc(project)
            argv = prepare('425', output, probe=True, audit=True, authoring=False)
            redirects = validate_redirects(argv, graph)
            with (output / 'preflight-console.log').open('x', encoding='utf-8') as log, contextlib.redirect_stdout(log):
                preflight = ledger.begin('owner-leg-preflight', 'read_only')
                try:
                    ledger.run_child([sys.executable, '-B', str(TOOLS/'owner_p9.py'), 'entry-owner'])
                finally:
                    ledger.finish(preflight, 'read_only')
            proof = root / 'P9/entry-owner/summary.json'
            if json.loads(proof.read_text())['result'] != 'PASS':
                raise RuntimeError('P9 owner preflight failed.')
            result.update(argv=argv, environment=masked(clean), boundary=contract, redirects=redirects, ddc=ddc,
                p9_proof=str(proof), manifest=config['manifest'], manifest_sha256=config['manifest_sha256'],
                binaries=binaries, sources=sources, engine_exe_sha256=digest(Path(argv[0])))
            ledger.dump(output/'prepared.json', result)
            with (output/'runtime-ledger-console.log').open('x', encoding='utf-8') as log, contextlib.redirect_stdout(log):
                admission_operation = ledger.begin('owner-leg-launch-admission', 'read_only' if dry_run else 'runtime')
            print('Protected files, binaries and routing passed. Checking for a quiet start; you may use the mouse and keyboard.', flush=True)
            arguments = {}
            if dry_run_samples is not None:
                samples = json.loads(owned(dry_run_samples).read_text())
                if not isinstance(samples, list) or not samples:
                    raise RuntimeError('Dry run requires a nonempty synthetic sample list.')
                clock = SyntheticClock()
                iterator = iter(samples)
                last = samples[-1]
                arguments = dict(collector=lambda: next(iterator, last), clock=clock.now, sleep=clock.sleep)
                result['gate_readings'] = 'SYNTHETIC; real P9, hashes and routing'
            else:
                result['gate_readings'] = 'LIVE'
            result['idle_gate'] = wait_idle(output/'idle.jsonl', **arguments)
            if result['idle_gate']['status'] != 'CLEAR':
                result['result'] = 'REFUSED_TIMEOUT'
                return result
            verify_files(manifest['files'])
            verify_files(config['host_source_and_routing_files'])
            verify_files(config.get('tooling_files', []))
            ledger.floors()
            if dry_run:
                result['result'] = 'ARMED_DRY_RUN_PASS'
                print('Dry run complete: the armed path passed. No editor was started; real attempts were not used.', flush=True)
                return result
            print('Starting the one authorized editor session. Do not open Play. Answer No to any rebuild prompt. Close with File > Exit and save nothing.', flush=True)
            def launched(pid):
                context['process_created'] = True
                result['launches'] = 1
                result['editor_pid'] = pid
                guard.mark_launched(slot, pid)
            os.environ['EUROPA_BUILD_CHILDREN'] = '1'
            with (output/'runtime-ledger-console.log').open('a', encoding='utf-8') as log, contextlib.redirect_stdout(log):
                result['exit_code'] = ledger.run_child(argv, cwd=project.parent, acceptable=tuple(range(256)),
                    on_started=launched, before_create=lambda: guard.reserve_launch(slot),
                    on_creation_failed=lambda: guard.cancel_unstarted(slot))
            result['result'] = 'EDITOR_CLOSED'
            print('Editor closed. Tell chat what happened. This kit will not start another editor session.', flush=True)
            return result
        except BaseException as error:
            result.update(result='STOP', error_type=type(error).__name__, error=str(error))
            raise
        finally:
            try:
                if admission_operation is not None:
                    with (output/'runtime-ledger-console.log').open('a', encoding='utf-8') as log, contextlib.redirect_stdout(log):
                        ledger.finish(admission_operation, 'runtime' if result['launches'] else 'read_only')
            except BaseException as error:
                result.update(result='STOP', ledger_error_type=type(error).__name__, ledger_error=str(error))
                raise
            finally:
                create_json(root/'attempt-result.json', result)


def stop_unexpected(error, context, sensitive=(), emit=print):
    root = context.get('attempt_root')
    if root is None:
        root = ROOT/'Receipts'/ARMING_JOB/'refused-attempts'/uuid.uuid4().hex
    root = owned(Path(root))
    root.mkdir(parents=True, exist_ok=True)
    path = root/('unexpected-error-'+uuid.uuid4().hex+'.txt')
    details = ''.join(traceback.format_exception(error))
    for value in sorted(set(sensitive), key=len, reverse=True):
        if value:
            details = details.replace(value, '<redacted>')
    path.write_text(details, encoding='utf-8')
    suffix = ('An editor may have started. This kit remains locked; tell chat before trying again.'
        if context.get('process_created') else 'No editor was started.')
    emit('Stopped: unexpected error; details saved to '+str(path)+'. '+suffix)
    return path


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--config', type=Path, default=ROOT/'Launch/Europa425-OwnerLeg.json')
    parser.add_argument('--dry-run', action='store_true')
    parser.add_argument('--owner-started', action='store_true')
    parser.add_argument('--dry-run-samples', type=Path)
    args = parser.parse_args(argv)
    context = {}
    sensitive = [v for k,v in os.environ.items() if secret_name(k) and v]
    try:
        result = run(args.config, args.dry_run, args.owner_started, args.dry_run_samples, context=context)
        return 0 if result['result'] in ('ARMED_DRY_RUN_PASS', 'EDITOR_CLOSED') else 2
    except BaseException as error:
        stop_unexpected(error, context, sensitive, emit=lambda line: print(line, flush=True))
        return 2


if __name__ == '__main__':
    sys.exit(main())
