import re
import subprocess


FEATURE = 'refs/heads/feat/europa-port'
BACKUP = 'refs/backup/europa-pre-fv6-09440'
ORIGIN = 'refs/remotes/origin/feat/europa-port'
SPIKE42 = 'refs/heads/spike/europa-09442c-mesh-shader'
SPIKE43 = 'refs/heads/spike/europa-09443a-depthonly-byname'
BACKUP_SHA = '08fe652734994549e084d190fff57207bc2a5c58'
SPIKE42_SHA = '740220cf736e60b4fc74d564668f48cc42290a76'
SPIKE43_BASE = 'c1193bdc386cc77bccb7c6c756a6f6fe28f195d0'


def parse_refs(lines):
    result = {}
    for line in lines:
        name, sha = line.split(' ', 1)
        if name in result or not re.fullmatch('[0-9a-f]{40}', sha):
            raise RuntimeError('Invalid or duplicate ref: ' + name)
        result[name] = sha
    return result


def git_ancestor(git_dir, ancestor, descendant):
    result = subprocess.run(['git', '--git-dir=' + str(git_dir), 'merge-base',
                             '--is-ancestor', ancestor, descendant], capture_output=True, text=True)
    if result.returncode not in (0, 1):
        raise RuntimeError('Cannot establish ref ancestry: ' + result.stderr.strip())
    return result.returncode == 0


def validate_refs(baseline_lines, current_lines, is_ancestor):
    baseline = parse_refs(baseline_lines)
    current = parse_refs(current_lines)
    if FEATURE not in current:
        raise RuntimeError('Missing local feature ref')
    for name, sha in baseline.items():
        if current.get(name) != sha:
            raise RuntimeError('Missing or moved baseline ref: ' + name)
    admitted = {}
    for name in sorted(current.keys()-baseline.keys()-{FEATURE}):
        sha = current[name]
        if name == BACKUP:
            valid, rule = sha == BACKUP_SHA, 'exact 094-40 backup'
        elif name == ORIGIN:
            valid, rule = is_ancestor(sha, current[FEATURE]), 'ancestor of or equal to local feat'
        elif name == SPIKE42:
            valid, rule = sha == SPIKE42_SHA, 'exact 094-42c evidence'
        elif name == SPIKE43:
            valid, rule = is_ancestor(SPIKE43_BASE, sha), '094-43a base or descendant'
        else:
            raise RuntimeError('Unknown extra ref: ' + name)
        if not valid:
            raise RuntimeError('Extra ref violates GE1: ' + name)
        admitted[name] = dict(sha=sha, rule=rule)
    return dict(result='PASS', authority='094-43 GE1', baseline_refs=len(baseline),
                baseline_exact=True, current_refs=len(current), admitted=admitted)
