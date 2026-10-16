"""EA11: UHT's supported directives are the only conditional directives here."""
import argparse
import json
from pathlib import Path
import re

ALLOWED = re.compile(r'(?:0|1|WITH_EDITORONLY_DATA|WITH_EDITOR|WITH_HOT_RELOAD[A-Za-z_0-9]*|!\s*CPP)\Z')

def without_comments(text):
    token = re.compile(r'"(?:\\.|[^"\\])*"|//[^\n]*|/\*[\s\S]*?\*/')
    return token.sub(lambda m: m[0] if m[0].startswith('"') else ' ' + '\n' * m[0].count('\n'), text)

def inspect(text, path):
    clean = without_comments(text)
    reflected = bool(re.search(r'#\s*include\s*"[^"\n]+\.generated\.h"|\bU(?:CLASS|STRUCT|ENUM|INTERFACE)\s*\(', clean))
    violations = []
    if reflected:
        lines = clean.splitlines()
        i = 0
        while i < len(lines):
            start = i+1
            line = lines[i]
            while line.rstrip().endswith('\\'):
                i += 1
                if i >= len(lines):
                    raise ValueError('Unterminated directive in ' + str(path))
                line = line.rstrip()[:-1] + lines[i]
            m = re.match(r'\s*#\s*(if|elif|ifdef|ifndef)\b\s*(.*)', line)
            if m and (m[1] not in ('if','elif') or not ALLOWED.fullmatch(m[2].strip())):
                violations.append({'path': str(path), 'line': start, 'directive': '#'+m[1]+' '+m[2].strip()})
            i += 1
    return reflected, violations

def self_test():
    base = '#pragma once\n#include "Probe.generated.h"\nUCLASS()\nclass UProbe {};\n'
    reflected, violations = inspect(base, 'synthetic/Probe.h')
    assert reflected and not violations
    mutant = base + '#if ANOMALY_CONTROL_SERVER\nvoid Forbidden();\n#endif\n'
    _, detected = inspect(mutant, 'synthetic/ProbeMutant.h')
    assert len(detected) == 1 and detected[0]['directive'] == '#if ANOMALY_CONTROL_SERVER'
    accepted = []
    for expression in ['0','1','WITH_EDITORONLY_DATA','WITH_EDITOR','WITH_HOT_RELOAD','WITH_HOT_RELOAD_CTORS','!CPP']:
        _, errors = inspect(base+'#if '+expression+'\n#endif\n', 'allowlist/Probe.h')
        assert not errors
        accepted.append(expression)
    for expression in ['__has_include("Stats/LightweightStats.h")','ENGINE_MAJOR_VERSION == 5','WITH_EDITOR && UNKNOWN','defined(WITH_EDITOR)']:
        assert inspect(base+'#if '+expression+'\n#endif\n', 'deny/Probe.h')[1]
    assert inspect(base+'#if \\\n ANOMALY_CAPTURE\n#endif\n','continued/Probe.h')[1]
    assert inspect(base+'#if WITH_EDITOR\n#elif ANOMALY_CAPTURE\n#endif\n','elif/Probe.h')[1]
    assert inspect(base+'#if/**/ANOMALY_CAPTURE\n#endif\n','comments/Probe.h')[1]
    assert not inspect('#pragma once\n#if __has_include("Stats/LightweightStats.h")\n#endif\n','Compat/Stats.h')[0]
    return {'result':'PASS','mutant_detected':detected,'allowed':accepted,'mutant_kind':'in-memory reflected header with one forbidden #if; no production file mutated'}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--root',type=Path)
    parser.add_argument('--self-test',action='store_true')
    parser.add_argument('--output',type=Path,required=True)
    args = parser.parse_args()
    report = {'self_test':self_test() if args.self_test else None,'reflected_headers':[],'violations':[]}
    if args.root:
        if not args.root.is_dir():raise RuntimeError('Header root missing')
        import os
        def fail(error):raise error
        for directory, dirs, files in os.walk(args.root,onerror=fail,followlinks=False):
            for name in sorted(files):
                if not name.endswith('.h'):continue
                path = Path(directory)/name
                reflected, violations = inspect(path.read_text(encoding='utf-8-sig'),path.relative_to(args.root))
                if reflected:report['reflected_headers'].append(str(path.relative_to(args.root)))
                report['violations'].extend(violations)
        if not report['reflected_headers']:raise RuntimeError('No reflected headers: vacuous check')
    report['result'] = 'FAIL' if report['violations'] else 'PASS'
    args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(report))
    return 1 if report['violations'] else 0

if __name__=='__main__':
    raise SystemExit(main())
