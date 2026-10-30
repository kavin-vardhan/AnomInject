from __future__ import print_function
import imp
import json
import os
import sys

import runtime_python27_tests as stub
import runtime_scripts as scripts
import rotator_contract


def main(output):
    assert sys.version_info[:3]==(2,7,14)
    output=os.path.abspath(output)
    assert output.lower().startswith('d:\\ia_europa\\receipts\\09427d\\scratch\\')
    os.makedirs(output)
    here=os.path.dirname(os.path.abspath(__file__))
    source=os.path.join(here,'runtime_editor_425.py')
    for name in ('runtime_editor_425.py','runtime_scripts.py'):
        with open(os.path.join(here,name),'rb') as stream:compile(stream.read(),name,'exec')
    rows=[]
    original=stub.EditorLevelLibrary.load_level
    pending=[False]
    def multi_tick_load(path):
        assert not pending[0],'A second load began before the first finished'
        pending[0]=True
        stub.CALLS.append(['load_level',path])
        for unused in range(5):
            stub.CLOCK[0]+=.1
            if stub.CALLBACK[0]:stub.CALLBACK[0](.1)
        pending[0]=False
        return True
    stub.EditorLevelLibrary.load_level=staticmethod(multi_tick_load)
    sys.modules['unreal']=stub.stub()
    for case,leg,angle in [('smoke','Smoke',[-12,0,0]),('multi_tick_load','Load',[-12,0,0]),('failure_once','Load',[0,0,-12])]:
        del stub.CALLS[:];stub.ROTATION[:]=angle
        receipt=os.path.join(output,case+'.json');namespace={}
        exec(compile(scripts.launcher(source,leg,receipt,'p','n'),case,'exec'),namespace)
        driver=namespace['_europa_runtime'];tick=driver.tick
        for unused in range(20):tick(.1)
        assert driver.STATE['result']==('DRIVER_FAIL' if case=='failure_once' else 'SMOKE_PASS' if leg=='Smoke' else 'DRIVER_PASS'),driver.STATE
        assert driver.STATE['exit_requests']==1 and stub.CALLBACK[0] is None
        assert stub.CALLS[-1]==['console','QUIT_EDITOR']
        assert len([c for c in stub.CALLS if c[0]=='load_level'])==(0 if leg=='Smoke' else 1 if case=='failure_once' else 2)
        frozen=list(stub.CALLS)
        for unused in range(20):tick(.1)
        assert stub.CALLS==frozen
        rows.append(dict(case=case,result='PASS',calls=list(stub.CALLS),state=driver.STATE))
    with open(source) as stream:mutant=stream.read().replace('if BUSY:', 'if False:')
    mutant_path=os.path.join(output,'unguarded.py')
    with open(mutant_path,'w') as stream:stream.write(mutant)
    pending[0]=False;stub.ROTATION[:]=[-12,0,0];del stub.CALLS[:]
    driver=imp.load_source('unguarded_runtime',mutant_path)
    driver.start('Load',os.path.join(output,'unguarded.json'),'p','n')
    for unused in range(20):driver.tick(.1)
    assert driver.STATE['result']=='DRIVER_FAIL' and 'second load' in driver.STATE['reason'].lower()
    rows.append(dict(case='unguarded_multitick_mutant',result='EXPECTED_FAIL',driver=driver.STATE))
    static=rotator_contract.check_all(here)
    for route in ('blinking','lod_corruption'):
        generated=scripts.steps('A7_:'+route+':positive');assert scripts.validate(generated)
    result=dict(result='PASS',python=sys.version,cases=rows,rotator_static=static,saves=0,unreal_launches=0)
    with open(os.path.join(output,'summary.json'),'w') as stream:json.dump(result,stream,indent=2)
    print(json.dumps(dict(result='PASS',cases=len(rows),python=sys.version)))


if __name__=='__main__':main(sys.argv[1])
