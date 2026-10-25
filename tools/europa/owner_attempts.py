import datetime as dt
import hashlib
import json
import msvcrt
import os
from pathlib import Path

from prepare_runtime import owned

AUTHORITY = 'D:/IntrusiveAnomalies/_reviews/094-15-ruling.md#EP1'
RUNTIME_SCOPE = dict(host='425', kind='editor-preview-owner-leg', owner_started=True,
    world='EditorPreview', maximum_editor_sessions=1, preview_seconds=60,
    pie=False, cook=False, commandlet=False, packaging=False, automated_input=False, save=False)


def stamp():
    return dt.datetime.now(dt.timezone.utc).isoformat()


def create_json(path, data):
    with path.open('x', encoding='utf-8') as stream:
        json.dump(data, stream, indent=2)
        stream.write('\n')
        stream.flush()
        os.fsync(stream.fileno())


def validate(config, dry_run=False):
    if config.get('host') != '425' or config.get('kind') != 'editor-preview-owner-leg':
        raise RuntimeError('This kit permits only the 4.25 owner preview.')
    if config.get('authorized_by_chat') is not True or config.get('authorization_citation') != AUTHORITY:
        raise RuntimeError('This kit is not armed by chat. No editor was started.')
    template = config.get('job_template', {})
    if template.get('runtime_authorized') is not True or template.get('runtime_scope') != RUNTIME_SCOPE:
        raise RuntimeError('The runtime authorization is outside the one owner preview.')
    if template.get('build_pairs') != [] or template.get('query_pairs') != []:
        raise RuntimeError('The owner kit cannot build or query build targets.')
    if config.get('gate_profile') != 'owner-preview' or config.get('human_idle') is not False:
        raise RuntimeError('The owner gate profile is not the EP2 profile.')
    if dry_run != (config.get('dry_run_only') is True):
        raise RuntimeError('Dry runs require throwaway slots; live runs require real slots.')
    slots = config.get('attempt_slots', [])
    if len(slots) != 3 or [s.get('number') for s in slots] != [1, 2, 3]:
        raise RuntimeError('Exactly three ordered attempt slots are required.')
    roots, temps = [], []
    for slot in slots:
        root, temp = owned(Path(slot['job_root'])), owned(Path(slot['temp_root']))
        if not root.is_relative_to(Path('D:/IA_Europa/Receipts')) or not temp.is_relative_to(Path('D:/IA_Europa/Temp')):
            raise RuntimeError('Attempt routing is outside owned receipts and TEMP.')
        roots.append(root)
        temps.append(temp)
    if len(set(roots)) != 3 or len(set(temps)) != 3 or any(a.is_relative_to(b) for a in roots for b in roots if a != b):
        raise RuntimeError('Attempt paths overlap.')
    state = owned(Path(config['state_root']))
    if not state.is_relative_to(Path('D:/IA_Europa/Receipts')) or any(state.is_relative_to(r) or r.is_relative_to(state) for r in roots):
        raise RuntimeError('Launch state must be separate from the attempt slots.')
    return template


class Guard:
    def __init__(self, config):
        self.config = config
        self.state = owned(Path(config['state_root']))
        self.handle = None

    def __enter__(self):
        self.state.mkdir(parents=True, exist_ok=True)
        self.handle = (self.state / 'active.lock').open('a+b')
        self.handle.seek(0, 2)
        if self.handle.tell() == 0:
            self.handle.write(b'0')
            self.handle.flush()
        self.handle.seek(0)
        try:
            msvcrt.locking(self.handle.fileno(), msvcrt.LK_NBLCK, 1)
        except OSError:
            self.handle.close()
            self.handle = None
            raise RuntimeError('Another kit attempt is running. Keep its console open; no editor was started by this attempt.') from None
        return self

    def __exit__(self, *args):
        if self.handle:
            self.handle.seek(0)
            msvcrt.locking(self.handle.fileno(), msvcrt.LK_UNLCK, 1)
            self.handle.close()

    def check_unused_launch(self):
        if (self.state / 'launched.json').exists():
            raise RuntimeError('The authorized editor session has already started. Ask chat before trying again.')
        if (self.state / 'launch-reserved.json').exists():
            raise RuntimeError('A prior launch has an incomplete receipt. Ask chat before trying again.')

    def claim(self):
        self.check_unused_launch()
        for slot in self.config['attempt_slots']:
            root = owned(Path(slot['job_root']))
            try:
                root.mkdir(parents=True, exist_ok=False)
            except FileExistsError:
                continue
            scope = dict(self.config['job_template'], job_root=str(root), temp_root=slot['temp_root'], started_utc=stamp())
            create_json(root / 'claim.json', dict(slot=slot['number'], claimed_utc=stamp(), pid=os.getpid(), dry_run_only=self.config['dry_run_only']))
            create_json(root / 'job-config.json', scope)
            return slot, root / 'job-config.json'
        raise RuntimeError('All three attempts have been used. Ask chat to prepare more; no editor was started by this attempt.')

    def reserve_launch(self, slot):
        self.check_unused_launch()
        create_json(self.state / 'launch-reserved.json', dict(slot=slot['number'], reserved_utc=stamp(), owner_wrapper_pid=os.getpid()))

    def mark_launched(self, slot, pid):
        create_json(self.state / 'launched.json', dict(slot=slot['number'], editor_pid=pid, launched_utc=stamp(), authorization_citation=AUTHORITY))

    def cancel_unstarted(self, slot):
        path = self.state / 'launch-reserved.json'
        if not path.exists() or (self.state / 'launched.json').exists():
            return
        row = json.loads(path.read_text())
        if row['slot'] != slot['number'] or row['owner_wrapper_pid'] != os.getpid():
            raise RuntimeError('Launch reservation belongs to another attempt.')
        receipt = dict(path=str(path.resolve(strict=True)), sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
            size=path.stat().st_size, reason='Popen did not start an editor; release only this attempt reservation', time_utc=stamp())
        create_json(Path(slot['job_root']) / 'unstarted-reservation-removal.json', receipt)
        path.unlink()
