import datetime
import json
from pathlib import Path

J = Path('D:/IA_Europa/Receipts/09444e')
B = Path('E:/IA_BuildCache/09444e-evidence/Calibration/01')
data = json.loads((B/'calibration-result.json').read_text())
execution = json.loads((J/'H4/Calibration/01/result.json').read_text())
def criteria(o):
    return dict(
        baseline=51 <= o['baseline_mean_min'] and o['baseline_mean_max'] <= 204,
        no_saturation=o['saturated_pixels'] == 0,
        pink=o['pink_min'][0] >= 200 and o['pink_min'][2] >= 200 and o['pink_max'][1] <= 80,
        pink_front=o['front_pink_min'][0] >= 200 and o['front_pink_min'][2] >= 200 and o['front_pink_max'][1] <= 80,
        checker=o['checker']['contrast'] >= 40 and o['checker']['low_share'] >= .15 and o['checker']['high_share'] >= .15)
objects = []
for name in ['EA_HideCube', 'EA_BlinkCube', 'EA_LOD']:
    values = [(r['exposure'], next(o for o in r['objects'] if o['actor'] == name)) for r in data['table']]
    accepted = {key: [ev for ev, o in values if criteria(o)[key]] for key in criteria(values[0][1])}
    common = [(ev, o) for ev, o in values if criteria(o)['baseline'] and criteria(o)['no_saturation']]
    best = max(common, key=lambda p: min(p[1]['pink_min'][0], p[1]['pink_min'][2])) if common else None
    objects.append(dict(actor=name, qualification_count=sum(o['result'] == 'PASS' for ev,o in values),
                        accepted_exposures=accepted,
                        brightest_baseline_eligible=dict(exposure=best[0], **best[1], criteria=criteria(best[1])) if best else None))
captures = [json.loads(p.read_text()) for p in sorted(B.glob('preview-*/capture.json'))]
value = dict(result='NEEDS-DECISION', utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
             reason='No common exposure satisfies the frozen GJ3 pre-fire criteria; no scored C5 launched.',
             exposures=len(data['table']), qualified_exposures=[r['exposure'] for r in data['table'] if r['qualified']],
             previews=len(captures), frames=sum(c['written'] for c in captures), drops=sum(c['dropped'] for c in captures),
             objects=objects, settings=json.loads((B/'settings.json').read_text()),
             exit={k:v for k,v in execution.items() if k in ('result','exit_code','root_started_utc','root_ended_utc','crashed','exit_classification','crash_count')},
             upload=execution['upload']['result'], no_freeze=not (J/'freeze.json').exists(),
             no_scored_pass=not (B/'plugin-frames').exists(), all_three_regression_legs='NOT RUN; pre-fire qualification failed')
(J/'calibration-stop-summary.json').write_text(json.dumps(value, indent=2))
print(json.dumps({k:v for k,v in value.items() if k not in ('objects','settings')}))
for o in objects:
    print(json.dumps(o))
