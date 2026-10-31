import json
from pathlib import Path
import sys
from PIL import Image
import capture_425_kit as kit
import capture_425_reader as reader

def main(output):
    root=kit.JOB/'capture-attempts/C1Game/01'
    calibration=reader.check(str(root),str(kit.GAME_REFERENCE))
    cases=[dict(case='EA_Travel_source_reference_preserves_region_and_limits',result='PASS',count=calibration['count'])]
    for name,reference,loader in (
        ('wrong_map_reference',kit.REFERENCE,None),
        ('black_frame',kit.GAME_REFERENCE,lambda p:Image.new('RGB',(1280,720)) if Path(p).name.startswith('frame-') else Image.open(p).convert('RGB'))):
        try:reader.check(str(root),str(reference),loader)
        except ValueError as error:cases.append(dict(case=name,result='PASS',refusal=str(error)))
        else:raise AssertionError(name+' was accepted')
    config=dict(reference=dict(path=str(kit.REFERENCE)),game_reference=dict(path=str(kit.GAME_REFERENCE)))
    assert kit.reference_for(config,'C1PIE')==str(kit.REFERENCE)
    assert kit.reference_for(config,'C1Game')==str(kit.GAME_REFERENCE)
    result=dict(result='PASS',cases=cases,region=reader.REGION,limits_unchanged=dict(mean=8,fraction_gt24=0.10),
        prior_session_verdict='FAIL retained; this is a reference-selection test, not a relabelled session')
    Path(output).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))

if __name__=='__main__':main(sys.argv[1])
