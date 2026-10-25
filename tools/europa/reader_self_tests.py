import copy
import json
from pathlib import Path
import numpy as np
from PIL import Image
import gate_readers as r
import write_ledger as ledger

import argparse
parser=argparse.ArgumentParser()
parser.add_argument('--output',type=Path,required=True)
OUT=parser.parse_args().output
if not OUT.resolve().is_relative_to(ledger.ROOT.resolve()): raise RuntimeError('Owned D output required')
OUT.mkdir(exist_ok=False)
cases = []

def check(gates,name,fn,fail=False):
    caught = None
    try:
        fn()
    except r.GateFailure as error:
        caught = str(error)
    passed = bool(caught) == fail
    cases.append(dict(gates=gates,case=name,expected='FAIL' if fail else 'PASS',actual='FAIL' if caught else 'PASS',reason=caught,pass_=passed))
    if not passed:
        raise AssertionError(name)

def changed(data,fn):
    value = copy.deepcopy(data)
    fn(value)
    return value

log = '\n'.join(['AnomalyInjector module started.', 'AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0).',
    'AnomalyControlServer module started (compiled out: ANOMALY_CONTROL_SERVER=0).','AnomalyShaders compiled out (ANOMALY_SHADERS=0).','AnomalyBench compiled out (ANOMALY_BENCH=0).'])
defines = { 'ANOMALY_'+n:0 for n in ('CAPTURE','CONTROL_SERVER','SHADERS','BENCH') }
observed = dict(capture_starts=0,listeners=0,shader_registrations=0,bench_commands=0,core_commands=1,visible_applies=3)
check(['A3'],'disabled_module_positive',lambda:r.compile_out(log,defines,observed))
check(['A3'],'missing_startup_diagnostic',lambda:r.compile_out(log.replace('AnomalyBench compiled out','omitted'),defines,observed),True)
check(['A3'],'missing_generated_define',lambda:r.compile_out(log,{k:v for k,v in defines.items() if k!='ANOMALY_CAPTURE'},observed),True)
check(['A3'],'no_positive_core',lambda:r.compile_out(log,defines,dict(observed,visible_applies=0)),True)
check(['A4'],'shipping_empty_positive',lambda:r.descriptor([],[],[],True))
check(['A4'],'shipping_probe_extra',lambda:r.descriptor([],['EuropaPhaseAProbe.dll'],[],True),True)
check(['A4'],'development_omission',lambda:r.descriptor([],[],r.MODULES[:-1],False),True)

rows=[]
def emit(kind,world='none',pointer='0',typ=-1,epoch=0,frame=0,**fields):
    rows.append(dict(kind=kind,sequence=len(rows)+1,world=world,world_pointer=pointer,world_type=typ,epoch=epoch,process_frame=frame,**fields))
def snap(world,pointer,typ,instance_prefix):
    subs=[]
    for i,cls in enumerate(r.CLASSES):
        game=typ in (1,3)
        subs.append(dict(**{'class':cls},registered_count=int(game),instances=[dict(pointer=instance_prefix+str(i),initialized=True,allowed=True,template=False)] if game else [],cdo_allowed=False,cdo_tick_type=2,cdo_should_create=game))
    emit('world_snapshot',world,pointer,typ,subsystems=subs)
def audit(op,world,pointer,typ,cls,instance,frame,init):
    emit('subsystem_audit',world,pointer,typ,frame=frame,audit_line=f'EUROPA-TICK op={op} instance={instance} class={cls} world={pointer} world_name={world} frame={frame} world_time=0.000 initialized={init} template=0')
snap('editor','e',2,'unused')
snap('preview','f',4,'unused')
for cycle in range(3):
    for player in range(2):
        world=f'w{cycle}:{player}'
        pointer=f'a{cycle}{player}'
        inst=f'b{cycle}{player}'
        snap(world,pointer,3,inst)
        for i,cls in enumerate(r.CLASSES):audit('initialize',world,pointer,3,cls,inst+str(i),0,1)
        for epoch in range(1,5):
            paused=epoch==2
            ticktype=0 if epoch==3 else 2
            eligible=not paused and ticktype!=0
            emit('world_update_begin',world,pointer,3,epoch,epoch,eligible=eligible,paused=paused,tick_type=ticktype,previous_update_unclosed=False)
            if eligible:
                for i,cls in enumerate(r.CLASSES):audit('tick',world,pointer,3,cls,inst+str(i),epoch,1)
            emit('world_update_end',world,pointer,3,epoch,epoch,paused=paused,tick_type=ticktype)
        for i,cls in enumerate(r.CLASSES):audit('deinitialize',world,pointer,3,cls,inst+str(i),5,0)
check(['A5','R3'],'two_world_same_process_frame_pause_travel_cycles',lambda:r.worlds(rows))
mut=copy.deepcopy(rows)
index=next(i for i,x in enumerate(mut) if x['kind']=='subsystem_audit' and 'op=tick' in x['audit_line'])
mut.insert(index,copy.deepcopy(mut[index]))
for i,x in enumerate(mut):x['sequence']=i+1
check(['A5','R3'],'duplicate_tick_not_deduplicated',lambda:r.worlds(mut),True)
mut=changed(rows,lambda x:x[1]['subsystems'][0].update(cdo_should_create=True))
check(['A5','R3'],'positive_preview_admission_mutant',lambda:r.worlds(mut),True)
mut=changed(rows,lambda x:x[0]['subsystems'][0].update(cdo_allowed=True))
check(['A5','R3'],'cdo_allowed_mutant',lambda:r.worlds(mut),True)
mut=copy.deepcopy(rows)
mut[index]['audit_line']=mut[index]['audit_line'].replace('initialized=1','initialized=0')
check(['A5','R3'],'uninitialized_tick',lambda:r.worlds(mut),True)
mut=copy.deepcopy(rows)
mut[index]['audit_line']=mut[index]['audit_line'].replace('world=a00','world=a01')
check(['A5','R3'],'wrong_world_tick',lambda:r.worlds(mut),True)
check(['A5','R3'],'empty_worlds',lambda:r.worlds([]),True)

gcrows=[dict(kind='gc_retained',world='w',gc_cycles=1,field_alive=[True]*5,array_alive=True),
        dict(kind='gc_released',world='w',gc_cycles=2,field_expired=[True]*5,array_expired=True),
        dict(kind='gc_bench_components',world='w',gc_cycles=1,constructed=True,fields_present=True,owned_components_alive=[True]*3,engine_major=5)]
check(['A6','R3'],'gc_hold_release_positive',lambda:r.gc(gcrows))
check(['A6','R3'],'removed_array_uproperty',lambda:r.gc(changed(gcrows,lambda x:x[0].update(array_alive=False))),True)
check(['A6','R3'],'fabricated_alive_without_gc',lambda:r.gc(changed(gcrows,lambda x:x[0].update(gc_cycles=0))),True)
check(['A6','R3'],'release_leak',lambda:r.gc(changed(gcrows,lambda x:x[1].update(array_expired=False))),True)

check(['A6','R3'],'component_reference_lost',lambda:r.gc(changed(gcrows,lambda x:x[2].update(owned_components_alive=[True,False,True]))),True)
check(['A6','R3'],'425_shell_positive',lambda:r.gc(changed(gcrows,lambda x:x[2].update(engine_major=4,owned_components_alive=[False]*3))))
check(['A6','R3'],'425_shell_not_inert',lambda:r.gc(changed(gcrows,lambda x:x[2].update(engine_major=4))),True)

png=OUT/'images'
png.mkdir()
base=np.full((720,1280,3),100,dtype=np.uint8)
active=base.copy();active[100:300,100:300]=200
def save(name,arr):
    path=png/(name+'.png');Image.fromarray(arr).save(path);return str(path)
clean=[save('clean-%02d'%i,base) for i in range(20)]
a=save('active',active)
state={'actor':'fixed','visible':True,'shadow':True,'material':'original','forced_lod':0}
picture=dict(clean_paths=clean,before=clean[0],active=[a],after=[clean[-2],clean[-1]],roi=[100,100,200,200],state_before=state,state_after=state)
check(['A7','R5','R7'],'pixel_positive_with_artifacts',lambda:r.picture_event(**dict(picture,artifact_dir=OUT/'picture-artifacts')))
check(['A7','R5'],'NoApply_picture',lambda:r.picture_event(**dict(picture,active=[clean[0]])),True)
check(['A7','R7'],'SkipRestore_picture',lambda:r.picture_event(**dict(picture,after=[a,a])),True)
check(['A7','R7'],'state_mutant',lambda:r.picture_event(**dict(picture,state_after=dict(state,shadow=False))),True)
noisy=save('noisy',np.full((720,1280,3),101,dtype=np.uint8))
check(['A7'],'unqualified_noise',lambda:r.picture_event(**dict(picture,clean_paths=clean[:-1]+[noisy])),True)
events=[dict(anomaly=key,applied=True,reverted=True,state_before=state,state_after=state) for key in sorted(r.CORE) for _ in range(3)]
check(['A7'],'four_routes_three_events',lambda:r.event_counts(events,14,2))
check(['A7'],'missing_third_event',lambda:r.event_counts(events[:-1],13,2),True)
check(['A7'],'accounting_mutant',lambda:r.event_counts(events,13,2),True)

pair=[dict(kind='screenshot_request',request=1,path='image.png',world='a:1',epoch=3,process_frame=8,show_ui=False,high_resolution=False),
      dict(kind='screenshot_complete',request=1,path='image.png',world='a:1',request_world='a:1',request_epoch=3,request_frame=8,process_frame=8,written=True,viewport_matches=True,width=1280,height=720)]
check(['A7'],'screenshot_acknowledgement',lambda:r.screenshot_pairs(pair))
check(['A7'],'missing_screenshot_ack',lambda:r.screenshot_pairs(pair[:1]),True)
check(['A7'],'wrong_screenshot_world',lambda:r.screenshot_pairs(changed(pair,lambda x:x[1].update(request_world='b:1'))),True)
check(['A7'],'overlapping_screenshot',lambda:r.screenshot_pairs([pair[0],pair[0],pair[1]]),True)

ad=dict(catalogue=sorted(r.CORE),pools=['blinking'],selections=['a','b','a'],twin=['a','b','a'],loads=['/Engine/BasicShapes/Cube'],teardown=dict(active=0,residue=0))
check(['A8'],'admission_seed_teardown_positive',lambda:r.admission(**ad))
check(['A8'],'unsupported_admitted',lambda:r.admission(**dict(ad,pools=['uv_corruption'])),True)
check(['A8'],'forced_second_target',lambda:r.admission(**dict(ad,twin=['a','c','a'])),True)
check(['A8'],'plugin_asset_load',lambda:r.admission(**dict(ad,loads=['/AnomalyInjector/Materials/M_X'])),True)
check(['A8'],'teardown_residue',lambda:r.admission(**dict(ad,teardown=dict(active=1,residue=1))),True)

bits=[False]*3+[True]*2+[False]*3+[True]*2+[False]*3+[True]*2+[False]*3
paths=[save('edge-%02d'%i,active if b else base) for i,b in enumerate(bits)]
check(['R4','R5'],'independent_png_edges',lambda:r.rgb_edges(paths,clean[0],[100,100,200,200],bits,0))
for delta in (-1,1):
    for edge in ('onset','off'):
        labels=list(bits)
        at=3 if edge=='onset' else 5
        labels[at if delta==1 else at-1]=not labels[at if delta==1 else at-1]
        check(['R4','R5'],f'{edge}_shift_{delta}',lambda labels=labels:r.rgb_edges(paths,clean[0],[100,100,200,200],labels,0),True)
check(['R4','R5'],'no_visible_events',lambda:r.edges([False]*len(bits),bits),True)
check(['R5'],'unassessed_edges',lambda:r.edges([None]*len(bits),bits),True)
detail=[dict(found=True,offset=0,status='observed',kind=kind) for _ in range(3) for kind in ('onset','end')]
check(['R5'],'release_adapter_zero_edges',lambda:r.release_edge_details(detail))
check(['R5'],'release_exit_zero_offset_note_rejected',lambda:r.release_edge_details(changed(detail,lambda x:x[0].update(offset=1))),True)

s=dict(ticks_first=0,ticks_last=600,paced=True,instrumentation=False,frames=600,real_seconds=20.,simulation_seconds=20.,fps=30,resolution=[1280,720],fixture='fixed')
check(['R6'],'matched_speed_three_reps',lambda:r.speed([s]*3,[s]*3))
check(['R6'],'pace_off',lambda:r.speed([s]*3,[dict(s,paced=False)]*3),True)
check(['R6'],'delay_speed_mutant',lambda:r.speed([s]*3,[dict(s,real_seconds=22.)]*3),True)
check(['R6'],'relative_regression',lambda:r.speed([dict(s,simulation_seconds=20.8,ticks_last=624)]*3,[s]*3),True)
check(['R6'],'instrumentation_on',lambda:r.speed([s]*3,[dict(s,instrumentation=True)]*3),True)
check(['R6'],'invented_game_clock_span',lambda:r.speed([s]*3,[dict(s,simulation_seconds=20.8)]*3),True)
check(['R6'],'too_few_repetitions',lambda:r.speed([s],[s]),True)

p=dict(capture='c',frame=1,mask_frame=1,family='f',mask_family='f',state_before=state,state_after=state,co_users_before=['x'],co_users_after=['x'],eligible_count=3,census_ms=50)
check(['R7'],'renderer_pair_positive',lambda:r.pairing([p]))
for key,value in [('mask_family','stale'),('mask_frame',2),('co_users_after',['changed']),('census_ms',101),('eligible_count',0)]:
    check(['R7'],'renderer_mutant_'+key,lambda key=key,value=value:r.pairing([dict(p,**{key:value})]),True)
check(['R7'],'duplicate_receipt',lambda:r.pairing([p,p]),True)

transcript=[]
for conn in ('first','reconnect'):
    for op in ('authenticated','snapshot','preview','run_started','run_stopped','closed'):
        transcript.append(dict(connection=conn,auth_class='valid',operation=op,actors=4,image_bytes=50,frames=3,callbacks=0,open_sockets=0))
transcript.extend([dict(connection='invalid',auth_class='invalid',operation=op,callbacks=0,open_sockets=0) for op in ('auth_rejected','closed')])
check(['R8'],'control_exchange_reconnect_invalid_auth',lambda:r.control(transcript))
check(['R8'],'suppressed_preview',lambda:r.control([x for x in transcript if x['operation']!='preview']),True)
check(['R8'],'dangling_callback',lambda:r.control(changed(transcript,lambda x:x[-1].update(callbacks=1))),True)
check(['R8'],'empty_snapshot',lambda:r.control(changed(transcript,lambda x:x[1].update(actors=0))),True)

c=dict(catalogue=[str(i) for i in range(14)],pool_keys=[str(i) for i in range(9)],defaults={'seed':4242},seed_sequence=['a','b'],selectors=['blinking'],ordinary_bench_open=False,flagged_bench_open=True)
check(['R9'],'catalogue_preserved',lambda:r.catalogue(c,c))
check(['R9'],'catalogue_entry_removed',lambda:r.catalogue(c,dict(c,catalogue=c['catalogue'][:-1])),True)
check(['R9'],'bench_without_flag',lambda:r.catalogue(c,dict(c,ordinary_bench_open=True)),True)
check(['R9'],'changed_seed_sequence',lambda:r.catalogue(c,dict(c,seed_sequence=['b','a'])),True)

ledger.dump(OUT/'self-tests.json',dict(result='PASS',cases=cases,count=len(cases),runtime_launches=0,
    scope='Synthetic readers only; source mutants and all real A/R legs remain unrun. PNG evidence generated under owned D scratch.'))
ledger.dump(OUT/'synthetic-worlds.json',dict(rows=rows))
print('A3-A8 and R3-R9 reader synthetic cases PASS:',len(cases))
