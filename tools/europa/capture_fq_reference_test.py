import json
from pathlib import Path

from capture_c2_reader import pixel_states
from fixture_policy import verify_files
import capture_c3_kit as kit

def run():
    plan=json.loads((kit.J/'c2-reference-plan.json').read_text())
    verify_files(plan['files'])
    old=json.loads(Path('D:/IA_Europa/Receipts/09433/c2-reference-plan.json').read_text())
    root=Path(plan['source_session'])/'plugin-frames/epoch01'
    row=json.loads((root/'labels.jsonl').read_text().splitlines()[0])
    try:pixel_states(str(root),[row],old['references'])
    except ValueError as error:
        assert 'Ambiguous pixel edge/frame 0' in str(error)
        refusal=str(error)
    else:raise AssertionError('Wrong-exposure reference was accepted')
    observed,measurements=pixel_states(str(root),[row],plan['references'])
    assert observed==[False]
    c2=json.loads((kit.J/'reread-01/labels.json').read_text())
    assert c2['result']=='PASS' and c2['mismatches']==c2['ambiguous']==0
    assert all(r['result']=='PASS' for r in c2['negatives'])
    result=dict(result='PASS',old_reference_refusal=refusal,exposure_matched_frame=measurements,
        same_session_C2=dict(result=c2['result'],mismatches=0,ambiguous=0,negatives=c2['negatives']),
        reference_files=plan['files'],classifier_thresholds_unchanged=True)
    (kit.J/'tests/reference-exposure.json').write_text(json.dumps(result,indent=2))
    print(json.dumps({k:v for k,v in result.items() if k not in ('reference_files','same_session_C2')}))

if __name__=='__main__':run()
