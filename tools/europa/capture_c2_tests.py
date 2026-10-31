from __future__ import print_function
import json
import os
import sys
import capture_c2_reader as reader
import rotator_contract

def fixture():
    rows=[];observed=[];commands=[]
    for i in range(480):
        event=i//20;step=i%20;route=reader.ROUTES[(event//3)%4]
        active=(6<=step<10 or 14<=step<17) if route=='blinking' else 3<=step<17
        states=[dict(id=r,labelled=active and r==route) for r in reader.ROUTES]
        rows.append(dict(frame_index=i,session_index=i,request=i+1,epoch=1,world_owner=99,aa=0,tickpin_compiled=0,
            sample_frame=100+i,family_frame=50+i,event_id=event,event_anomaly=route,tick_order=i//240,
            pace=1 if i<240 else 0,fixed_step=i<240,sampled_states=states,anomalies=[dict(id=route)] if active else [],anomaly_present=active))
        observed.append(active)
        if step in (3,17):commands.append(dict(command='apply' if step==3 else 'revert',event_id=event,next_index=i,success=True))
    return rows,observed,commands

def main(output,references=None):
    rows,observed,commands=fixture()
    cases=[]
    if references:
        with open(references) as stream:refs=json.load(stream)['references']
        for row,active in zip(rows,observed):row['image']=refs[row['event_anomaly']]['active' if active else 'clean']
        pixels,metrics=reader.pixel_states('',rows,refs)
        assert pixels==observed
        cases.append(dict(case='independent_kept_Phase_A_pixels',result='PASS',frames=len(pixels)))
    baseline=reader.evaluate(rows,observed,commands,reader.annotation_for(rows))
    assert baseline['result']=='PASS' and len(baseline['totals'])==8
    for kind in ('apply','revert','blink_toggle'):
        mutant,indices=reader.mutate(rows,observed,kind)
        try:reader.evaluate(mutant,observed,commands,reader.annotation_for(mutant))
        except ValueError as error:
            assert 'Pixel/label mismatch' in str(error)
            cases.append(dict(case='one_frame_'+kind,result='PASS',mutated_indices=indices,refusal=str(error)))
        else:raise AssertionError('Shift escaped: '+kind)
    for kind in ('epoch','family'):
        bad=json.loads(json.dumps(rows))
        if kind=='epoch':bad[100]['epoch']=2
        else:bad[100]['family_frame']=bad[99]['family_frame']
        try:reader.evaluate(bad,observed,commands,reader.annotation_for(bad))
        except ValueError as error:cases.append(dict(case='wrong_'+kind,result='PASS',refusal=str(error)))
        else:raise AssertionError('Stale family/epoch escaped')
    for name in ('capture_c2_reader.py','capture_c2_tests.py','capture_425_driver.py'):
        with open(os.path.join(os.path.dirname(__file__),name),'rb') as stream:source=stream.read()
        assert not rotator_contract.check_source(source,name)
    assert rotator_contract.check_source('unreal.Rotator(-12,0,0)','negative')
    result=dict(result='PASS',python=sys.version,cases=cases,rotator='PASS',new_in_editor_python=False)
    with open(output,'w') as stream:json.dump(result,stream,indent=2)
    print(json.dumps(result))

if __name__=='__main__':main(sys.argv[1],sys.argv[2] if len(sys.argv)>2 else None)
