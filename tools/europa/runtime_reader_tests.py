import argparse
import json
from pathlib import Path

from PIL import Image,ImageDraw
import gate_readers as gates
from runtime_readers import pictures,ROIS,clean_exit,lifecycle
from unittest.mock import patch
from runtime_policy import JOB,canonical


def fixture(root,route,control):
    folder=root/'session/probe';folder.mkdir(parents=True)
    rows=[];frame=0
    def emit(kind,**kw):
        rows.append(dict(kind=kind,sequence=len(rows)+1,world='synthetic-world',world_type=1,epoch=frame,
                         process_frame=frame,monotonic_seconds=100+frame,**kw))
    def picture(name,active=False):
        nonlocal frame
        emit('command_result',command='prepare',arguments=['prepare'],success=True,skip_restore_control=False,**{'r.PostProcessAAQuality':0})
        frame+=1;path=folder/name
        im=Image.new('RGB',(1280,720),(30,30,30))
        if active:
            x,y,w,h=ROIS[route];ImageDraw.Draw(im).rectangle((x+10,y+10,x+w-10,y+h-10),fill=(220,220,220))
        im.save(path)
        request=len(rows)+1
        emit('screenshot_request',path=str(path),request=request,show_ui=False,high_resolution=False)
        emit('screenshot_complete',path=str(path),request=request,request_world='synthetic-world',request_epoch=frame,
             request_frame=frame,written=True,viewport_matches=True,width=1280,height=720)
    def state(label,residue=False):emit('component_state',label=label,actors=[dict(identity='independent-target',hidden=residue)])
    for i in range(20):picture('clean-%02d.png'%i)
    for event in range(6 if control=='positive' else 1):
        prefix=route+'-'+str(event)
        state(prefix+'-before');picture(prefix+'-before.png')
        emit('command_result',command='apply',success=control!='NoApply')
        picture(prefix+'-during-00.png',control!='NoApply')
        picture(prefix+'-during-01.png',control!='NoApply' and route!='blinking')
        state(prefix+'-active',True)
        emit('command_result',command='revert',success=control=='positive')
        state(prefix+'-after',control=='SkipRestore')
        for i in range(2):picture(prefix+'-after-%d.png'%i,control=='SkipRestore')
    if control=='positive':
        for i in range(2):emit('command_result',command='apply',success=False)
    observation=dict(kind='event_observation',started_qpc=0,ended_qpc=10000,last_input_qpc=-1,foreign_gpu_percent=0)
    (root/'monitor.jsonl').write_text(json.dumps(observation)+'\n')
    return rows


def main(output):
    output=canonical(output,JOB/'scratch');output.mkdir(parents=True,exist_ok=False);cases=[]
    for route in ROIS:
        for control in ('positive','NoApply','SkipRestore'):
            root=output/(route+'-'+control);rows=fixture(root,route,control)
            result=pictures('A7_:'+route+':'+control,root,rows)
            assert result['counted']==(6 if control=='positive' else 0)
            cases.append(dict(case=route+'-'+control,result='PASS'))
    root=output/'invalid-input';rows=fixture(root,'missing_object','positive')
    events=[]
    completed=[r for r in rows if r['kind']=='screenshot_complete']
    for i in range(6):
        before=next(r for r in completed if Path(r['path']).name=='missing_object-%d-before.png'%i)
        after=next(r for r in completed if Path(r['path']).name=='missing_object-%d-after-1.png'%i)
        events.append(dict(kind='event_observation',started_qpc=before['monotonic_seconds']-1,
            ended_qpc=after['monotonic_seconds']+1,last_input_qpc=before['monotonic_seconds']+.1,foreign_gpu_percent=0))
    (root/'monitor.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in events))
    try:pictures('A7_:missing_object:positive',root,rows)
    except gates.GateFailure as error:assert 'three valid' in str(error)
    else:raise AssertionError('Human-input events counted')
    cases.append(dict(case='input-contaminated-events-do-not-count',result='PASS'))
    for case,mutation in (('missing-probe-stop','stop'),('upload','upload'),('plugin-error','plugin'),('outside-PIE','pie')):
        from runtime_real_files import clone,SMOKE
        root=output/case;rows=clone(SMOKE,root)
        if mutation=='plugin':
            with (root/'session/Runtime.log').open('a') as stream:stream.write('LogAnomaly: Error: synthetic\n')
        if mutation=='upload':
            with (root/'session/CrashClient.log').open('a') as stream:stream.write('POST synthetic\n')
        if mutation=='stop':rows=rows[:-1]
        if mutation=='pie':next(r for r in rows if r['kind']=='world_created')['world_type']=3
        try:clean_exit('Smoke',root,rows)
        except gates.GateFailure:cases.append(dict(case=case,result='PASS'))
        else:raise AssertionError(case)
    for value in (4,None):
        root=output/('aa-'+str(value));rows=fixture(root,'missing_object','positive')
        for row in rows:
            if row['kind']=='command_result' and row['command']=='prepare':row['r.PostProcessAAQuality']=value
        try:pictures('A7_:missing_object:positive',root,rows)
        except gates.GateFailure as error:assert 'three valid' in str(error)
        else:raise AssertionError('Nonzero/unreadable AA picture counted')
        cases.append(dict(case='AA-'+str(value)+'-does-not-count',result='PASS'))
    rows=[dict(kind='world_update_begin',world_type=3,process_frame=1,world=w) for w in ('first','second')]
    rows += [dict(kind='dormant_created') for n in range(3)]
    rows += [dict(kind='command_result',command='travel',success=True) for n in range(3)]
    with patch('runtime_readers.gates.worlds',return_value=dict(result='PASS')):
        assert lifecycle(rows,True)['result']=='PASS'
        for case,changed in (
            ('two-world-overlap-required',[dict(r,process_frame=i) if r['kind']=='world_update_begin' else r for i,r in enumerate(rows)]),
            ('three-travels-required',rows[:-1]),('dormant-probe-required',[r for r in rows if r['kind']!='dormant_created'])):
            try:lifecycle(changed,True)
            except gates.GateFailure:cases.append(dict(case=case,result='PASS'))
            else:raise AssertionError(case)
    (output/'summary.json').write_text(json.dumps(dict(result='PASS',cases=cases,unreal_launches=0),indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(cases))))


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args();main(a.output)
