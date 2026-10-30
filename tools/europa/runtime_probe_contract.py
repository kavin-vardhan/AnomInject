from collections import Counter
from pathlib import Path

from gate_readers import require,CLASSES

PROBE=Path('D:/IA_Europa/StylizedParisStreet/Source/EuropaPhaseAProbe')
WORLD=PROBE/'EuropaPhaseAWorldProbe.cpp'
DRIVER=PROBE/'EuropaPhaseAProbeDriver.cpp'

EMITTERS=[
    dict(readers='all',kind='*',fields='kind sequence process_frame monotonic_seconds world world_pointer world_name world_type epoch',source=str(WORLD),lines=[88,101]),
    dict(readers='clean_exit startup',kind='probe_start',fields='capture_define control_define shader_define',source=str(WORLD),lines=[143,155]),
    dict(readers='clean_exit',kind='probe_stop',fields='root_release outstanding_screenshot',source=str(WORLD),lines=[158,180]),
    dict(readers='clean_exit',kind='world_cleanup',fields='actor_snapshot session_ended',conditional_fields='skip_reason when actor_snapshot=skipped',source=str(WORLD),lines=[211,225]),
    dict(readers='clean_exit',kind='gc_restore_skipped',fields='skip_reason',source=str(DRIVER),lines=[393,403]),
    dict(readers='lifecycle admission',kind='world_initialized',fields='',source=str(WORLD),lines=[203,208]),
    dict(readers='lifecycle admission',kind='world_snapshot',fields='label subsystems',conditional_fields='catalogue active_count enabled_pool in Game/PIE',source=str(WORLD),lines=[294,359]),
    dict(readers='lifecycle',kind='world_snapshot.subsystems',fields='class registered_count instances cdo_should_create cdo_allowed cdo_tick_type',source=str(WORLD),lines=[267,290]),
    dict(readers='lifecycle',kind='world_snapshot.subsystems.instances',fields='pointer initialized allowed template',source=str(WORLD),lines=[276,283]),
    dict(readers='lifecycle',kind='world_update_begin',fields='tick_type paused eligible previous_update_unclosed',source=str(WORLD),lines=[234,251]),
    dict(readers='lifecycle',kind='world_update_end',fields='tick_type paused',source=str(WORLD),lines=[254,261]),
    dict(readers='lifecycle',kind='subsystem_audit',fields='audit_line',source=str(WORLD),lines=[183,188]),
    dict(readers='lifecycle',kind='dormant_created',fields='objects hold_frames',source=str(DRIVER),lines=[506,528]),
    dict(readers='lifecycle',kind='dormant_created.objects',fields='initialized allowed',source=str(DRIVER),lines=[488,502]),
    dict(readers='GC',kind='gc_retained',fields='field_alive array_alive gc_cycles',source=str(DRIVER),lines=[440,447]),
    dict(readers='GC',kind='gc_released',fields='field_expired array_expired gc_cycles',source=str(DRIVER),lines=[448,456]),
    dict(readers='GC',kind='gc_bench_components',fields='constructed fields_present owned_components_alive engine_major gc_cycles',source=str(DRIVER),lines=[459,482]),
    dict(readers='pictures admission lifecycle',kind='command_result',fields='command arguments success skip_restore_control',conditional_fields='r.PostProcessAAQuality for prepare; live_fires for successful auto-fire',source=str(DRIVER),lines=[149,241]),
    dict(readers='pictures',kind='command_result.prepare',fields='r.PostProcessAAQuality',source=str(DRIVER),lines=[217,241]),
    dict(readers='pictures',kind='screenshot_request',fields='path request show_ui high_resolution',source=str(DRIVER),lines=[245,264]),
    dict(readers='pictures',kind='screenshot_complete',fields='path request request_world request_epoch request_frame width height written viewport_matches',source=str(DRIVER),lines=[267,298]),
    dict(readers='pictures admission',kind='component_state',fields='label actors',source=str(DRIVER),lines=[303,390]),
    dict(readers='pictures admission',kind='component_state.actors',fields='identity name hidden components',source=str(DRIVER),lines=[326,389]),
    dict(readers='pictures admission',kind='component_state.actors.components',fields='identity name',conditional_fields='visible hidden_game main_pass depth_pass cast_shadow cast_hidden_shadow dynamic_shadow static_shadow distance_field dynamic_indirect custom_depth receives_decals ray_tracing stencil forced_lod mesh materials intensity color shadow according to component class; whole actors state compared',source=str(DRIVER),lines=[339,382]),
    dict(readers='clean_exit monitor',kind='driver_failure',fields='reason',source=str(DRIVER),lines=[69,100]),
    dict(readers='monitor',kind='script_complete',fields='steps',source=str(DRIVER),lines=[106,111]),
]


def required(row,names,context):
    require(all(name in row for name in names.split()),'Missing emitter field in '+context)


def stream(rows):
    require(bool(rows),'Empty probe stream')
    require([r.get('sequence') for r in rows]==list(range(1,len(rows)+1)),'Missing, duplicate or out-of-order probe row')
    require(rows[0].get('kind')=='probe_start' and rows[-1].get('kind')=='probe_stop','Probe start/stop ordering')
    require(sum(r.get('kind')=='probe_start' for r in rows)==sum(r.get('kind')=='probe_stop' for r in rows)==1,'Non-unique probe lifetime')
    common=EMITTERS[0]['fields']
    schemas={r['kind']:r['fields'] for r in EMITTERS[1:] if '.' not in r['kind']}
    for row in rows:
        required(row,common,'common Emit')
        required(row,schemas.get(row['kind'],''),row['kind'])
        if row['kind']=='world_cleanup' and row['actor_snapshot']=='skipped':required(row,'skip_reason','skipped cleanup')
        if row['kind']=='world_snapshot':
            require(Counter(s.get('class') for s in row['subsystems'])==Counter(CLASSES),'Missing/duplicate subsystem observation')
            for sub in row['subsystems']:
                required(sub,'class registered_count instances cdo_should_create cdo_allowed cdo_tick_type','subsystem')
                game=row['world_type'] in (1,3)
                require(sub['registered_count']==len(sub['instances'])==(1 if game else 0),'World admission/registration mismatch')
                require(not sub['cdo_allowed'] and sub['cdo_tick_type']==2 and sub['cdo_should_create']==game,'CDO world/tick admission mismatch')
                for instance in sub['instances']:
                    required(instance,'pointer initialized allowed template','subsystem instance')
                    require(instance['initialized'] and instance['allowed'] and not instance['template'],'Unsafe registered subsystem')
            if row['world_type'] in (1,3):required(row,'catalogue active_count enabled_pool','game snapshot')
        if row['kind']=='component_state':
            for actor in row['actors']:
                required(actor,'identity name hidden components','component-state actor')
                for component in actor['components']:required(component,'identity name','component-state component')
        if row['kind']=='dormant_created':
            for obj in row['objects']:required(obj,'initialized allowed','dormant object')
        if row['kind']=='command_result' and row['command']=='auto-fire' and row['success']:required(row,'live_fires','auto-fire')
    stop=rows[-1]
    require(stop['root_release']=='skipped' and stop['outstanding_screenshot'] is False,'Unsafe probe stop')
    cleanup=[r for r in rows if r['kind']=='world_cleanup' and r['actor_snapshot']=='skipped']
    require(cleanup and all(r['skip_reason'] in ('engine_exit','garbage_collection','invalid_world','world_teardown') for r in cleanup),'No skipped cleanup proof')
    restore=[r for r in rows if r['kind']=='gc_restore_skipped']
    require(restore and restore[-1]['sequence']<stop['sequence'],'Missing final GC cleanup guard')
    return dict(result='PASS',rows=len(rows),skipped_cleanup=len(cleanup),probe_stop=1,
                observed_kinds=dict(Counter(r['kind'] for r in rows)))


def aa_echoes(rows):
    prepare=None
    result={}
    for row in rows:
        if row['kind']=='command_result':
            if row['command']=='prepare':prepare=row
            elif row['command']!='screenshot':prepare=None
        if row['kind']=='screenshot_request':
            valid=bool(prepare and prepare['success'] and prepare['world']==row['world'] and
                       prepare['sequence']<row['sequence'] and prepare['process_frame']<=row['process_frame'])
            value=prepare.get('r.PostProcessAAQuality') if valid else None
            result[row['path']]=dict(path=row['path'],request=row['request'],effective_AA_quality=value,
                counted_eligible=valid and value==0,echo_sequence=prepare['sequence'] if valid else None,
                request_sequence=row['sequence'],emitter=str(DRIVER)+':219-222')
            prepare=None
    return result
