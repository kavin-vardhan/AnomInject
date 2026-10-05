import hashlib
import json
import subprocess
from pathlib import Path

w = Path('E:/IA_BuildCache/_r921/09205')
host = Path('E:/IA_BuildCache/_r53_host/StackOBot')


def checked_archive():
    archive = json.loads((w / 'probe-archive.json').read_text())
    head = subprocess.check_output(['git', '-C', str(host / 'Plugins/AnomalyInjector'), 'rev-parse', 'HEAD'], text=True).strip()
    assert archive['source_commit'] == head, (archive['source_commit'], head)
    feature = w.parent / 'AnomalyInjector'
    for relative in ('Source', 'Shaders', 'AnomalyInjector.uplugin'):
        current_tree = subprocess.check_output(['git', '-C', str(feature), 'rev-parse', 'HEAD:' + relative], text=True).strip()
        host_tree = subprocess.check_output(['git', '-C', str(host / 'Plugins/AnomalyInjector'), 'rev-parse', 'HEAD:' + relative], text=True).strip()
        assert current_tree == host_tree, (relative, current_tree, host_tree)
    assert not subprocess.check_output(['git', '-C', str(host / 'Plugins/AnomalyInjector'), 'status', '--porcelain'], text=True).strip()
    for relative, expected in archive['files'].items():
        live = host / ('Binaries/Win64/StackOBot.exe' if relative == 'StackOBot.exe' else 'Plugins/AnomalyInjector/Binaries/Win64/' + Path(relative).name)
        with live.open('rb') as stream:
            actual = hashlib.file_digest(stream, 'sha256').hexdigest()
        assert actual == expected, (str(live), actual, expected)
    return archive
