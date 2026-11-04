import datetime
import json
from pathlib import Path
import threading

from build_memory import OwnedTree


class SnapshotDict(dict):
    def values(self):
        return tuple(super().values())

    def __iter__(self):
        return iter(tuple(super().keys()))


class CreationTree(OwnedTree):
    def __init__(self, root_pid, creation_log=None):
        self.creation_lock = threading.RLock()
        self.creation_stop = threading.Event()
        self.creation_error = None
        self.creation_events = []
        self.creation_log = Path(creation_log) if creation_log else None
        self.creation_polls = 0
        super().__init__(root_pid)
        self.known = SnapshotDict(self.known)
        self.creation_thread = threading.Thread(target=self.watch_creations, name='owned-child-creation')
        self.creation_thread.start()

    def capture_command(self, row):
        if row.get('creation_command_line'):
            return
        first = 'first_observed_utc' not in row
        row.setdefault('first_observed_utc', datetime.datetime.now(datetime.timezone.utc).isoformat())
        try:
            command = self.command_line(row)
            if not command:
                raise RuntimeError('Owned process command line empty')
            row.update(command_line=command, creation_command_line=command,
                       command_observed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
            row.pop('command_line_error', None)
        except RuntimeError as error:
            row['command_line_error'] = str(error)
        if first or row.get('creation_command_line'):
            record = {key: value for key, value in row.items() if key != 'handle'}
            record.update(kind='created' if first else 'command-resolved',
                          live=self.kernel.WaitForSingleObject(row['handle'], 0) != 0,
                          authority='094-44 GG2 FE3b: owned identity, 10 ms creation poll')
            self.creation_events.append(record)
            if self.creation_log:
                with self.creation_log.open('a', encoding='utf-8') as stream:
                    stream.write(json.dumps(record)+'\n')

    def add(self, pid, parent, depth):
        with self.creation_lock:
            added = super().add(pid, parent, depth)
            if added:
                self.capture_command(self.known[pid])
            return added

    def poll_creations(self):
        with self.creation_lock:
            OwnedTree.poll(self)
            for row in self.known.values():
                if not row.get('creation_command_line') and self.kernel.WaitForSingleObject(row['handle'], 0) != 0:
                    self.capture_command(row)
            self.creation_polls += 1

    def watch_creations(self):
        try:
            while not self.creation_stop.is_set():
                self.poll_creations()
                self.creation_stop.wait(0.01)
        except BaseException as error:
            self.creation_error = str(error)

    def poll(self):
        if self.creation_error:
            raise RuntimeError('FE3b creation watch failed: '+self.creation_error)
        self.poll_creations()

    def receipt(self):
        with self.creation_lock:
            data = super().receipt()
            data['creation_watch'] = dict(interval_seconds=0.01, polls=self.creation_polls,
                                          error=self.creation_error, events=list(self.creation_events))
            return data

    def close(self):
        self.creation_stop.set()
        self.creation_thread.join()
        super().close()
