from collections import Counter
import json
from pathlib import Path
import re

import gate_readers as gates
from fixture_policy import sha
from fixture_uploads import check as upload_check
from runtime_monitor import validity
from runtime_probe_contract import stream as probe_contract, aa_echoes
from owner_attempts import create_json

ROIS={'blinking':[805,310,360,360],'missing_object':[115,310,360,360],
      'lod_corruption':[450,170,380,420],'lighting_mismatch':[80,250,1120,450]}


def jsonl(path):
    return [json.loads(line) for line in Path(path).read_text(encoding='utf-8-sig').splitlines() if line]


def clean_exit(leg,root,rows):
    execution=json.loads((root/'execution.json').read_text())
    gates.require(execution.get('exit_code')==0 and execution['result']=='PROCESS_PASS','Nonzero or stopped session')
    stops=[r for r in rows if r['kind']=='probe_stop']
    gates.require(len(stops)==1 and stops[0]['root_release']=='skipped' and not stops[0]['outstanding_screenshot'],
                  'Missing clean probe stop')
    contract=probe_contract(rows)
    skipped=[r for r in rows if r['kind']=='world_cleanup' and r.get('actor_snapshot')=='skipped']
    gates.require(not any(r['kind']=='driver_failure' for r in rows),'Native driver failure')
    if leg!='WorldsPIE':gates.require(not any(r.get('world_type')==3 for r in rows),'PIE outside its armed leg')
    upload=upload_check(root/'session')
    create_json(root/'upload-check.json',upload)
    gates.require(upload['result']=='PASS','Upload evidence or incomplete upload logs')
    text=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
    findings=[]
    for number,line in enumerate(text.splitlines(),1):
        if not re.search(r'(?i)(\berror:|\bensure condition failed|\bfatal error)',line):continue
        category=('our plugin or probe' if re.search(r'(?i)(LogAnomaly|EuropaPhaseA|Europa\.Probe|LogPython: Error)',line)
                  else 'host content' if re.search(r'(?i)(/Game/|StylizedParis|LogMaterial)',line)
                  else 'engine' if re.search(r'(?i)(LogEngine|LogSlate|LogStreaming|LogWindows|LogOnline|LogRHI|LogRenderer|LogLinker)',line)
                  else 'unclassified')
        findings.append(dict(line=number,text=line,category=category))
    create_json(root/'errors-ensures.json',dict(findings=findings))
    gates.require(not any(r['category'] in ('our plugin or probe','unclassified') or 'fatal error' in r['text'].lower() for r in findings),
                  'Plugin/probe/unclassified error or fatal error')
    gates.require('Engine exit requested' in text or 'Exiting.' in text or 'RequestExit' in text,'No normal engine shutdown log')
    return dict(exit_code=0,probe_stop=1,skipped_cleanup=len(skipped),uploads=upload,errors=findings)


def startup(rows,text):
    starts=[r for r in rows if r['kind']=='probe_start']
    gates.require(len(starts)==1,'Missing unique probe start')
    row=starts[0]
    gates.require(all(row.get(k)==0 for k in ('capture_define','control_define','shader_define')),'Disabled compile defines differ')
    messages=['AnomalyInjector module started.',
        'AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0).',
        'AnomalyControlServer module started (compiled out: ANOMALY_CONTROL_SERVER=0).',
        'AnomalyShaders compiled out (ANOMALY_SHADERS=0).','AnomalyBench compiled out (ANOMALY_BENCH=0).']
    gates.require(all(text.count(m)==1 for m in messages),'Compiled-out module startup evidence differs')
    return dict(result='PASS',compiled_defines=row,positive_core_smoke='requires A7 readers')


def states(rows):
    return {r['label']:r for r in rows if r['kind']=='component_state'}


def pictures(leg,root,rows,output=None):
    output=Path(output) if output is not None else root
    unused,route,control=leg.split(':')
    complete=gates.screenshot_pairs(rows)
    aa=aa_echoes(rows)
    create_json(output/'picture-aa.json',dict(pictures=list(aa.values())))
    by_name={Path(r['path']).name:r for r in complete}
    for row in complete:
        path=Path(row['path']).resolve()
        gates.require(path.is_relative_to((root/'session/probe').resolve()),'Screenshot outside bound receipt root')
        gates.image(path)
    clean=[by_name['clean-%02d.png'%n]['path'] for n in range(20)]
    observed=[r for r in jsonl(root/'monitor.jsonl') if r['kind']=='event_observation']
    state=states(rows)
    commands=[r for r in rows if r['kind']=='command_result']
    events=[]
    roi=ROIS[route]
    for number in range(6 if control=='positive' else 1):
        prefix=route+'-'+str(number)
        before=by_name[prefix+'-before.png']
        after=[by_name[prefix+'-after-%d.png'%n] for n in range(2)]
        during=[r for name,r in sorted(by_name.items()) if name.startswith(prefix+'-during-')]
        observation=validity(observed,before['monotonic_seconds'],after[-1]['monotonic_seconds'])
        interval=dict(valid=True,reasons=[],record_only=observation,authority='094-27c FJ1')
        event_pictures=[before]+during+after
        bad_aa=[r['path'] for r in event_pictures if not aa.get(r['path'],{}).get('counted_eligible')]
        bad_clean=[p for p in clean if not aa.get(p,{}).get('counted_eligible')]
        if bad_aa or bad_clean:
            interval['valid']=False;interval['reasons'].append('FH8 missing/nonzero effective AA echo')
        baseline=gates.image(before['path'])
        measurements=[gates.metrics(baseline,gates.image(r['path']),roi) for r in during]
        active=[r['path'] for r,m in zip(during,measurements) if m['mean']>=4 and m['fraction_ge8']>=.01]
        visible=[r['path'] for r,m in zip(during,measurements) if m['mean']<=.5 and m['fraction_ge8']<=.001]
        event=dict(number=number,validity=interval,roi=roi,before=before['path'],during=[r['path'] for r in during],
                   after=[r['path'] for r in after],pixel_measures=measurements,aa=[aa.get(r['path']) for r in event_pictures])
        try:
            gates.require(active,'No visible active picture')
            if route=='blinking':gates.require(visible,'No visible phase of blinking')
            event['pixels']=gates.picture_event(clean,before['path'],active,[r['path'] for r in after],roi,
                state[prefix+'-before']['actors'],state[prefix+'-after']['actors'],quantization=True)
            event['picture_result']='PASS'
        except gates.GateFailure as error:
            event.update(picture_result='FAIL',reason=str(error))
        event['counted']=interval['valid'] and event['picture_result']=='PASS' and control=='positive'
        event['chosen_during']=active[0] if active else during[0]['path']
        events.append(event)
    applied=[r for r in commands if r['command']=='apply' and r.get('success')]
    reverted=[r for r in commands if r['command']=='revert' and r.get('success')]
    attempts=[r for r in commands if r['command']=='apply']
    refusals=[r for r in attempts if not r.get('success')]
    result=dict(result='PASS',anomaly=route,control=control,events=events,attempted=len(attempts),
                applied=len(applied),reverted=len(reverted),refused=len(refusals),counted=sum(e['counted'] for e in events),
                invalid=sum(not e['validity']['valid'] for e in events),roi=roi,
                glass_mask='No Paris glass in authored primitive fixture; fixed ROI excludes all other content.')
    result['result']='PASS' if result['counted']>=3 else 'FAIL'
    create_json(output/'picture-events.json',result)
    if control=='positive':
        gates.require(result['counted']>=3,'Fewer than three valid visible restored events')
        gates.require(len(applied)==len(reverted)==6 and len(refusals)>=2,'Apply/revert/refusal accounting mismatch')
        contact_sheet(output,route,[e for e in events if e['counted']])
    else:
        gates.require(all(e['validity']['valid'] for e in events),'Control event invalid under FB3')
        expected='No visible active picture' if control=='NoApply' else 'Pixel restore residue'
        gates.require(all(e['picture_result']=='FAIL' and (e['reason']==expected or control=='SkipRestore' and e['reason']=='State restore residue or empty state') for e in events),
                      'Control did not fail the unchanged pixel/state reader for its intended reason')
        gates.require(len(applied)==(0 if control=='NoApply' else 1) and not reverted,'Wrong control execution')
        result['result']='CONTROL_CAN_FAIL_PASS'
    return result


def contact_sheet(root,route,events):
    from PIL import Image,ImageDraw
    sheet=Image.new('RGB',(3*640,len(events)*384),(20,20,20))
    draw=ImageDraw.Draw(sheet)
    index=[]
    for row,event in enumerate(events):
        for col,(phase,path) in enumerate(zip(('before','during','after'),
                                             (event['before'],event['chosen_during'],event['after'][-1]))):
            with Image.open(path) as source:sheet.paste(source.convert('RGB').resize((640,360)),(col*640,row*384+24))
            echo=next(a for a in event['aa'] if a and a['path']==path)
            draw.text((col*640+8,row*384+5),route+' event '+str(event['number'])+' '+phase+' AA='+str(echo['effective_AA_quality']),fill='white')
            index.append(dict(event=event['number'],phase=phase,path=path,sha256=sha(path),native_size=[1280,720],aa=echo))
    output=root/(route+'-contact-sheet.png');sheet.save(output)
    create_json(root/'contact-sheet-index.json',dict(contact_sheet=str(output),sha256=sha(output),pictures=index))


def admission(rows,text):
    commands=[r for r in rows if r['kind']=='command_result']
    seeds=[r for r in commands if r['command']=='auto-seed']
    fires=[r for r in commands if r['command']=='auto-fire']
    gates.require(len(seeds)==2 and all(r['arguments']==['auto-seed','4242'] for r in seeds),'Seed setup differs')
    gates.require(len(fires)==12 and all(r['success'] and r['live_fires'] for r in fires),'Missing seeded selections')
    refusals=[r for r in commands if r['command']=='apply']
    gates.require(len(refusals)==3 and all(not r['success'] for r in refusals),'Admission refusals missing')
    sequences=[[re.sub(r' (?:\d+(?:\.\d+)?s|persist)$','',value) for value in r['live_fires']] for r in fires]
    snapshots=[r for r in rows if r['kind']=='world_snapshot' and r.get('world_type')==1]
    before=next(r for r in snapshots if r.get('label')=='admission_before')
    tail=[r for r in snapshots if r.get('label') in ('seed_0_after_stop','seed_1_after_stop','pre_travel','cleanup') or 'EA_Travel' in r.get('world_name','')]
    gates.require(len(tail)>=4 and all(r['active_count']==0 for r in tail),'Stop/travel/teardown active residue')
    state=states(rows)
    gates.require(state['admission_before']['actors']==state['refusals_after']['actors'],'Refusal changed actor state')
    gates.require(state['admission_before']['actors']==state['pre_teardown']['actors'],'Seeded stop left state residue')
    package_lines=[line for line in text.splitlines() if re.search(r'(?i)(LogLinker|LogStreaming).*\b(load|package)',line)]
    gates.require(package_lines,'Package-load observation stream absent')
    gates.admission(before['catalogue'],before['enabled_pool'],sequences[:6],sequences[6:],package_lines,dict(active=0,residue=0))
    return dict(result='PASS',seed=4242,sequences=sequences,package_lines=len(package_lines),zero_residue_snapshots=len(tail))


def lifecycle(rows,pie):
    if pie:
        simultaneous={}
        for row in rows:
            if row['kind']=='world_update_begin' and row.get('world_type')==3:
                simultaneous.setdefault(row['process_frame'],set()).add(row['world'])
        gates.require(any(len(worlds)>=2 for worlds in simultaneous.values()),'No simultaneous two-world PIE frame')
    gates.require(sum(r['kind']=='dormant_created' for r in rows)>=(3 if pie else 1),'Dormant lifecycle observations missing')
    travels=[r for r in rows if r['kind']=='command_result' and r.get('command')=='travel' and r.get('success')]
    gates.require(len(travels)>=(3 if pie else 1),'Successful travel cycles missing')
    return gates.worlds(rows,cycles=3 if pie else 1,require_two_pie=pie,required_types={2,4} if pie else {1,4})


def read_leg(leg,root):
    root=Path(root)
    result=dict(result='INCOMPLETE',leg=leg)
    try:
        rows=jsonl(root/'session/probe/probe.jsonl')
        result['clean_exit']=clean_exit(leg,root,rows)
        text=(root/'session/Runtime.log').read_text(encoding='utf-8-sig',errors='replace')
        if leg in ('Smoke','Load','WorldsPIE'):
            driver=json.loads((root/'driver.json').read_text())
            gates.require(driver['result']==('SMOKE_PASS' if leg=='Smoke' else 'DRIVER_PASS'),'Editor driver failed')
            result['driver']=driver
        if leg=='Smoke':
            gates.require(all(r['present'] for r in result['driver']['bindings']) and result['driver']['module_alive'],'Runtime smoke binding/lifetime failure')
        elif leg=='Load':result['A3_load_compile_out']=startup(rows,text)
        elif leg=='WorldsPIE':result['A5_PIE']=lifecycle(rows,True)
        elif leg=='WorldsGame':result['A5_game']=lifecycle(rows,False)
        elif leg=='GC':gates.gc(rows);result['A6_GC']='PASS'
        elif leg.startswith('A7_'):result['A7']=pictures(leg,root,rows)
        elif leg=='Admission':result['A8']=admission(rows,text)
        result['result']='PASS'
    except Exception as error:
        result.update(result='FAIL',reason=str(error))
    finally:create_json(root/'reader.json',result)
    return result
