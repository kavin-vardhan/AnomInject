import json
from pathlib import Path
import h4_kit09444d as kit

measurements=[]
for leg,spec in kit.SPECS.items():
    root=kit.J/'H4-kit-selftest'/leg
    root.mkdir(parents=True)
    (root/'session').mkdir()
    argv=kit.command(dict(spec=spec),root)
    expected='IAI.Capture.C5 '+(root/'plugin-frames').as_posix()+' 4242\n'
    assert (root/'Binaries/capture.txt').read_text()==expected
    assert '-game' in argv and '-BENCHMARK' not in argv and '-nowrite' in argv
    assert '-'+spec[0].lower() in argv
    assert '-ResX='+str(spec[1]) in argv and '-ResY='+str(spec[2]) in argv
    assert '/Game/EuropaPhaseA/EA_Core' in argv
    assert not any('IAICaptureLegacyReference' in a or 'IAIBenchBypassHostDepth' in a for a in argv)
    measurements.append(dict(leg=leg,argv=argv,default_routes=True))
value=dict(result='PASS',legs=measurements,source_equal=kit.c1.native_source_equal('8e59a56'))
assert value['source_equal']
(kit.J/'host-kit-tests.json').write_text(json.dumps(value,indent=2))
print('PASS: three host-only -game variants, odd size, compiled default routes and existing build-source equality')
