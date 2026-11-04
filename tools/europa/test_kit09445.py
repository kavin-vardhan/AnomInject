import json,sys
from pathlib import Path
import h4_kit09445 as kit
from calibration_reader09445 import test
J=kit.J;kit.J=J/'kit-selftest';kit.J.mkdir(exist_ok=False)
(kit.J/'freeze.json').write_text(json.dumps(dict(result='PASS',exposure=-5.0)))
rows=[]
for leg,spec in kit.SPECS.items():
    root=kit.J/leg;root.mkdir();(root/'session').mkdir()
    argv=kit.command(dict(spec=spec,leg=leg,number='01'),root)
    assert '-game' in argv and '-nowrite' in argv and '-IAIBench' in argv and '-BENCHMARK' not in argv
    assert '-IAICaptureExit' not in argv and '-'+spec[0].lower() in argv
    assert '-ResX='+str(spec[1]) in argv and '-ResY='+str(spec[2]) in argv
    macro=(root/'Binaries/capture.txt').read_text();assert 'Europa.TextureBench' in macro
    assert ('calibrate 0' in macro) if leg=='Calibration' else ('run -5.0' in macro)
    rows.append(dict(leg=leg,argv=argv,macro=macro))
result=dict(result='PASS',argv_cases=rows,pixel_negative_tests=test())
(J/'kit-tests.json').write_text(json.dumps(result,indent=2));print('PASS: four argv cases, calibration-before-fire and six GK2 pixel negatives')
