import argparse
import copy
import hashlib
import json
from pathlib import Path
import re

from makefile_actions import executed, status_lines

ROOT = Path('D:/IA_Europa')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main(output):
    if not output.resolve().is_relative_to(ROOT.resolve()):
        raise RuntimeError('Evidence must remain in owned D scratch')
    cases = []
    inputs = []
    for label, operation, logfile, count, types, complete in [
        ('425-editor', '02-425-editor', 'child-009.log', 3, ['Compile'] * 3, False),
        ('425-editor-final', '04-425-editor-final', 'child-007.log', 5, ['Compile', 'Compile', 'Link', 'Link', 'WriteMetadata'], True)]:
        log = ROOT / 'Receipts/09414/ledger' / operation / logfile
        graphpath = ROOT / 'Receipts/09414/builds' / label / 'native-graph.json'
        graph = json.loads(graphpath.read_text())
        text = log.read_text(encoding='utf-8', errors='replace')
        result = executed(graph, text, complete=complete)
        assert len(result['Actions']) == count
        assert [r['Type'] for r in result['Actions']] == types
        assert all('EuropaPhaseAProbe' in p or p.endswith('StylizedParisStreetEditor.target') for a in result['Actions'] for p in a['ProducedItems'])
        failures = []
        blocks = re.split(r'^\s*\[\d+/\d+\]\s+', text, flags=re.M)[1:]
        for block in blocks:
            if re.search(r'\berror C\d+\b', block):
                failures.append(block.splitlines()[0].strip())
        assert len(failures) == (0 if complete else 2), failures
        cases.append(dict(case=label, result='PASS', executed=count, types=types, failing_compiles=failures, rows=result['executed_lines']))
        inputs.extend(dict(path=str(p), size=p.stat().st_size, sha256=digest(p)) for p in (log, graphpath))
    coverage = []
    for name, expected in [('baseline-editor', 29), ('candidate-game', 765)]:
        log = ROOT / 'Receipts/09409/builds' / name / 'UBT.log'
        text = log.read_text(encoding='utf-8', errors='replace')
        rows = status_lines(text)
        assert len(rows) == expected
        assert len(rows) == len(re.findall(r'^\s*\[\d+/\d+\]', text, re.M))
        coverage.append(dict(path=str(log), sha256=digest(log), status_lines=len(rows), rows=rows))
    log = ROOT / 'Receipts/09415/ledger/04-51base-editor/child-007.log'
    graphpath = ROOT / 'Receipts/09415/builds/51base-editor/native-graph.json'
    native51 = executed(json.loads(graphpath.read_text()), log.read_text(encoding='utf-8', errors='replace'), complete=False)
    assert len(native51['Actions']) == 3 and len(native51['cancelled_lines']) == 3
    assert all(a['Type'] == 'Compile' for a in native51['Actions'])
    coverage.append(dict(path=str(log), sha256=digest(log), status_lines=6, executed=3, cancelled=3, native_join='PASS'))
    action = copy.deepcopy(graph['Actions'][result['executed_lines'][0]['native_action_index']])
    good = dict(Actions=[action], action_count=1)
    line = '[1/1] ' + action['StatusDescription']
    assert len(executed(good, line)['Actions']) == 1
    engine = copy.deepcopy(good)
    engine['Actions'][0]['ProducedItems'] = ['D:/UESource/UnrealEngine/Engine/Binaries/Win64/SYNTHETIC.dll']
    ambiguous = dict(Actions=[copy.deepcopy(action), copy.deepcopy(action)], action_count=2)
    for name, mutant, log in [('engine_output', engine, line), ('no_match', good, '[1/1] Unknown.cpp'), ('ambiguous_match', ambiguous, line)]:
        try:
            executed(mutant, log)
        except RuntimeError as error:
            cases.append(dict(case=name, result='REJECTED', reason=str(error)))
        else:
            raise AssertionError('Ineffective can-fail: ' + name)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(dict(result='PASS', attempt=2, known_answers=cases[:2], can_fails=cases[2:], format51_coverage=coverage, input_hashes=inputs,
        citations=['UE_4.25/Engine/Source/Programs/UnrealBuildTool/Executors/ParallelExecutor.cs:207-213,262-276',
                   'UnrealEngine/Engine/Source/Programs/UnrealBuildTool/Executors/ParallelExecutor.cs:327-340,348-380,399-400']), indent=2) + '\n')
    print(json.dumps(dict(result='PASS', known_answers=[3,5], failing_compiles=2, format51_lines=sum(c['status_lines'] for c in coverage), can_fails=3)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', required=True, type=Path)
    main(parser.parse_args().output)
