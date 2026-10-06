import json
import re
from pathlib import Path

w = Path('E:/IA_BuildCache/_r921/09205')
cases = [
    ('stacks-release-refusal-final-v2', 'virtual_texture'),
    ('stacks-accepted-refusal-final-v2', 'stack_layout'),
    ('stacks-unnamed-refusal-final-v2', 'stack_unnamed'),
]


def check(log, reason):
    attempted = re.findall(r"Auto.FireSpecific: 'stuck_low_mip' on 'StaticMeshActor_77' -> not applied\.", log)
    if reason == 'virtual_texture':
        refused = re.findall(r"stuck_low_mip: matched 1 component\(s\) for '=StaticMeshActor_77' with 3 candidate texture\(s\) but HELD NONE .* - 3 virtual, 0 not streamable.*", log)
    else:
        refused = re.findall(r"TEXCORRUPT-REFUSED family=blur mode=proxy_blur target='=StaticMeshActor_77' final=virtual_texture:" + re.escape(reason) + r" step=V1 .*", log)
    return bool(attempted and refused), attempted, refused


records = []
for label, reason in cases:
    log = (w / 'runs' / label / 'game.log').read_text(errors='replace')
    valid, attempts, refusals = check(log, reason)
    assert valid, label
    mutant = log
    for line in refusals:
        mutant = mutant.replace(line, '')
    assert not check(mutant, reason)[0]
    records.append(dict(label=label, reason=reason, attempts=attempts, actual_refusals=refusals,
                        startup_banner_alone_mutant_caught=True))
record = dict(status='PASS', cases=records,
              refinement='Supplementary check requires actual attempted-fire refusal; the earlier generic named-text reader could match a startup banner. Raw logs and earlier results remain unchanged.')
(w / 'refusal-attempt-gates.json').write_text(json.dumps(record, indent=2))
print(json.dumps(dict(status='PASS', cases=len(records), mutants_caught=len(records))))
