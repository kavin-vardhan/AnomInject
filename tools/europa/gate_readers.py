"""Phase A evidence readers. Synthetic qualification is never a runtime verdict."""
import argparse
from collections import Counter, defaultdict
import hashlib
import json
import math
from pathlib import Path
import re

import numpy as np
from PIL import Image

CLASSES = ('AnomalyInjectorSubsystem', 'AnomalyAutoInjectorSubsystem', 'AnomalySelectorSubsystem',
           'AnomalyCaptureSubsystem', 'AnomalyControlServerSubsystem')
MODULES = ('AnomalyInjector', 'AnomalyCapture', 'AnomalyControlServer', 'AnomalyShaders', 'AnomalyBench')
CORE = {'blinking', 'lighting_mismatch', 'lod_corruption', 'missing_object'}

class GateFailure(RuntimeError):
    pass

def require(condition, message):
    if not condition:
        raise GateFailure(message)

def image(path):
    with Image.open(path) as im:
        require(im.format == 'PNG' and im.size == (1280, 720), 'Not a native 1280x720 PNG')
        return np.array(im.convert('RGB'), dtype=np.int16)

def metrics(a, b, roi):
    x, y, w, h = roi
    require(0 <= x < 1280 and 0 <= y < 720 and w > 0 and h > 0 and x+w <= 1280 and y+h <= 720, 'Invalid frozen ROI')
    d = np.abs(a[y:y+h, x:x+w] - b[y:y+h, x:x+w])
    return {'mean': float(d.mean()), 'fraction_ge8': float((d.max(axis=2) >= 8).mean())}

def picture_event(clean_paths, before, active, after, roi, state_before, state_after, artifact_dir=None):
    require(len(set(clean_paths)) >= 20 and len(after) >= 2 and len(active) >= 1, 'Missing clean/active/two restore frames')
    clean = [image(p) for p in clean_paths]
    noise = max(metrics(a, b, roi)['mean'] for a, b in zip(clean, clean[1:]))
    require(noise <= .25, 'Unqualified clean-frame noise')
    reference = image(before)
    changed = [metrics(reference, image(p), roi) for p in active]
    require(all(m['mean'] >= max(4., 4*noise) and m['fraction_ge8'] >= .01 for m in changed), 'No visible active picture')
    restored = [metrics(reference, image(p), roi) for p in after]
    require(all(m['mean'] <= max(.5, 2*noise) and m['fraction_ge8'] <= .001 for m in restored), 'Pixel restore residue')
    require(state_before and state_before == state_after, 'State restore residue or empty state')
    if artifact_dir:
        output = Path(artifact_dir)
        require(output.resolve().is_relative_to(Path('D:/IA_Europa').resolve()), 'Picture artifacts outside owned D scratch')
        output.mkdir(parents=True, exist_ok=False)
        paths = [before, *active, *after]
        sheet = Image.new('RGB',(640*len(paths),360))
        for index,path in enumerate(paths):
            actual = image(path)
            difference = np.clip(np.abs(actual-reference),0,255).astype('uint8')
            Image.fromarray(difference).save(output/('difference-%02d.png'%index))
            sheet.paste(Image.fromarray(actual.astype('uint8')).resize((640,360)),(index*640,0))
        sheet.save(output/'contact-sheet.png')
    return {'noise': noise, 'active': changed, 'restored': restored}

def screenshot_pairs(rows):
    outstanding = None
    completed = []
    for row in rows:
        if row['kind'] == 'screenshot_request':
            require(outstanding is None, 'Overlapping screenshot requests')
            require(row['show_ui'] is False and row['high_resolution'] is False, 'UI or tiled screenshot')
            outstanding = row
        elif row['kind'] == 'screenshot_complete':
            require(outstanding is not None, 'Unrequested screenshot completion')
            require(row['request'] == outstanding['request'] and row['path'] == outstanding['path'], 'Screenshot request mismatch')
            require(row['world'] == row['request_world'] == outstanding['world'], 'Screenshot world mismatch')
            require(row['request_epoch'] == outstanding['epoch'] and row['request_frame'] == outstanding['process_frame'], 'Screenshot epoch mismatch')
            require(row['written'] and row['viewport_matches'] and (row['width'], row['height']) == (1280,720), 'Screenshot write/viewport failure')
            require(row['process_frame'] >= row['request_frame'], 'Screenshot time reversed')
            completed.append(row)
            outstanding = None
    require(outstanding is None and completed, 'Missing screenshot acknowledgement')
    require(len({r['path'] for r in completed}) == len(completed), 'Screenshot file reused')
    return completed

def compile_out(log, defines, observed):
    messages = ['AnomalyInjector module started.',
        'AnomalyCapture module started (compiled out: ANOMALY_CAPTURE=0).',
        'AnomalyControlServer module started (compiled out: ANOMALY_CONTROL_SERVER=0).',
        'AnomalyShaders compiled out (ANOMALY_SHADERS=0).',
        'AnomalyBench compiled out (ANOMALY_BENCH=0).']
    require(all(log.count(message) == 1 for message in messages), 'Missing/duplicate module startup')
    require(all(defines.get('ANOMALY_' + n) == 0 for n in ('CAPTURE','CONTROL_SERVER','SHADERS','BENCH')), 'Missing/wrong generated disabled define')
    require(observed['capture_starts'] == observed['listeners'] == observed['shader_registrations'] == observed['bench_commands'] == 0, 'Disabled feature did work')
    require(observed['core_commands'] > 0 and observed['visible_applies'] >= 3, 'Missing positive core smoke')

def descriptor(actions, products, modules, shipping):
    names = json.dumps([actions,products,modules])
    if shipping:
        require(not any(n in names for n in MODULES + ('EuropaPhaseAProbe',)), 'Shipping IA/probe artifact')
    else:
        require(set(MODULES) <= set(modules), 'Incomplete Development modules')

AUDIT = re.compile(r'EUROPA-TICK op=(\w+) instance=(\S+) class=(\S+) world=(\S+) .* frame=(\d+) .* initialized=(\d) template=(\d)')

def worlds(rows, require_audit=True, cycles=3, require_two_pie=True):
    require(rows and all(b['sequence'] > a['sequence'] for a,b in zip(rows,rows[1:])), 'Missing or reordered probe stream')
    snapshots = [r for r in rows if r['kind'] == 'world_snapshot']
    require(snapshots, 'No world observations')
    observed = {r['world_type'] for r in snapshots}
    require({2,4} <= observed and observed & {1,3}, 'Missing positive Editor/EditorPreview/Game-or-PIE world')
    pie = {r['world'] for r in snapshots if r['world_type'] == 3}
    if require_two_pie:
        require(len(pie) >= 2, 'No second PIE world')
    instances = {}
    for row in snapshots:
        subs = row['subsystems']
        require(Counter(s['class'] for s in subs) == Counter(CLASSES), 'Missing/duplicate subsystem class observation')
        game = row['world_type'] in (1,3)
        for sub in subs:
            require(sub['registered_count'] == len(sub['instances']) == (1 if game else 0), 'World admission/registration mismatch')
            require(not sub['cdo_allowed'] and sub['cdo_tick_type'] == 2, 'CDO can tick')
            require(sub['cdo_should_create'] == game, 'CDO world admission decision wrong')
            for instance in sub['instances']:
                require(not instance['template'] and instance['initialized'] and instance['allowed'], 'Uninitialized registered subsystem')
                instances[(row['world'], sub['class'])] = instance['pointer'].lower()
    begins = [r for r in rows if r['kind'] == 'world_update_begin']
    require(begins and any(r['eligible'] for r in begins) and any(r['paused'] for r in begins), 'No update/pause denominator')
    current, expected, actual = {}, Counter(), Counter()
    lifecycle, initialized, ended = {}, Counter(), Counter()
    for row in rows:
        kind = row['kind']
        if kind == 'world_update_begin':
            key = row['world_pointer'].lower()
            eligible = row['world_type'] in (1,3) and row['tick_type'] != 0 and not row['paused']
            require(row['eligible'] == eligible and not row['previous_update_unclosed'], 'Wrong or overlapping denominator')
            require(key not in current, 'Repeated begin without end')
            current[key] = row
            if eligible:
                for cls in CLASSES:
                    require((row['world'],cls) in instances, 'Eligible world has no observed subsystem')
                    expected[(row['world'],row['epoch'],cls)] += 1
        elif kind == 'world_update_end':
            key = row['world_pointer'].lower()
            require(key in current and current[key]['epoch'] == row['epoch'], 'Unmatched world update end')
            current.pop(key)
        elif kind == 'subsystem_audit':
            match = AUDIT.search(row['audit_line'])
            require(match is not None, 'Malformed native tick audit')
            op, instance, cls, key, frame, init, template = match.groups()
            instance, key = instance.lower(), key.lower()
            require(cls in CLASSES and template == '0', 'Unknown class or CDO audit')
            if op == 'initialize':
                require(not lifecycle.get(instance,False) and init == '1', 'Double initialization')
                lifecycle[instance] = True
                initialized[cls] += 1
            elif op == 'deinitialize':
                require(lifecycle.get(instance,False) and init == '0', 'Unbalanced deinitialization')
                lifecycle[instance] = False
                ended[cls] += 1
            elif op == 'tick':
                require(lifecycle.get(instance,False) and init == '1', 'Tick outside initialized lifetime')
                require(key in current and current[key]['eligible'], 'Tick outside eligible world update')
                require(instances.get((current[key]['world'],cls)) == instance, 'Wrong instance tick')
                require(int(frame) == current[key]['process_frame'], 'Audit process frame mismatch')
                actual[(current[key]['world'],current[key]['epoch'],cls)] += 1
            else:
                raise GateFailure('Unknown lifecycle operation')
        elif kind == 'dormant_created':
            require(len(row['objects']) == 5 and row['hold_frames'] >= 3, 'Dormant probe incomplete')
            require(all(not o['initialized'] and not o['allowed'] for o in row['objects']), 'Dormant instance is allowed to tick')
    require(not current, 'Unclosed world updates')
    if require_audit:
        require(expected == actual and expected, 'Per-world tick multiplicity mismatch')
        require(initialized == ended and all(initialized[c] >= cycles for c in CLASSES), 'Lifecycle cycles incomplete')
    return {'worlds':len({r['world'] for r in snapshots}), 'eligible_ticks':sum(expected.values()),
            'actual_ticks':sum(actual.values()), 'native_baseline_observation_only':not require_audit}

def gc(rows):
    retained = [r for r in rows if r['kind'] == 'gc_retained']
    released = [r for r in rows if r['kind'] == 'gc_released']
    require(retained and len(retained) == len(released), 'Missing GC cycles')
    for a,b in zip(retained,released):
        require(a['world'] == b['world'] and a['gc_cycles'] >= 1 and b['gc_cycles'] >= 2, 'No forced GC evidence')
        require(len(a['field_alive']) == len(b['field_expired']) == 5, 'Missing raw reference field')
        require(all(a['field_alive']) and a['array_alive'], 'Lost strong GC reference')
        require(all(b['field_expired']) and b['array_expired'], 'Released reference retained')
    components = [r for r in rows if r['kind'] == 'gc_bench_components']
    require(len(components) == len(retained), 'Missing component shell/ownership probe')
    for row in components:
        require(row['constructed'] and row['fields_present'] and row['gc_cycles'] >= 1 and len(row['owned_components_alive']) == 3, 'Component shell/GC domain incomplete')
        require(row['engine_major'] in (4,5), 'Unknown GC engine')
        require(all(row['owned_components_alive']) if row['engine_major'] == 5 else not any(row['owned_components_alive']), 'Component ownership/inert shell differs')


def event_counts(events, attempts, refusals, required=CORE):
    counts = Counter(e['anomaly'] for e in events if e['applied'])
    require(set(counts) == set(required) and all(n >= 3 for n in counts.values()), 'Missing visible event floor')
    require(attempts == sum(counts.values()) + refusals, 'Attempt accounting mismatch')
    require(all(e['reverted'] and e['state_before'] and e['state_before'] == e['state_after'] for e in events), 'Restore/state residue')

def admission(catalogue, pools, selections, twin, loads, teardown):
    require(set(catalogue) == CORE and len(catalogue) == 4, 'Unsupported/missing 4.25 catalogue')
    require(pools and set(pools) <= CORE, 'Unsupported/empty pool')
    require(selections and selections == twin, 'Empty or nondeterministic seeded draws')
    require(not any('/anomalyinjector/' in p.lower() for p in loads), 'Loaded 5.1 plugin package')
    require(teardown['active'] == teardown['residue'] == 0, 'Teardown residue')

def edges(pixel_active, labelled, minimum_events=3):
    require(pixel_active and labelled and len(pixel_active) == len(labelled), 'Empty/unmatched edge domain')
    require(all(type(x) is bool for x in pixel_active + labelled), 'Unmeasured pixel/label edge')
    def transitions(bits):
        require(not bits[0] and not bits[-1], 'Truncated event span')
        return [(i,bits[i]) for i in range(1,len(bits)) if bits[i] != bits[i-1]]
    p, l = transitions(pixel_active), transitions(labelled)
    require(len(p) >= 2*minimum_events and p == l, 'Missing visible events or displaced onset/off edge')
    return {'events':len(p)//2, 'onset_displacement':0, 'off_displacement':0}

def rgb_edges(frames, reference, roi, labels, noise, minimum_events=3):
    require(math.isfinite(noise) and 0 <= noise <= .25, 'Unqualified image noise')
    base = image(reference)
    measures = [metrics(base,image(p),roi) for p in frames]
    pixels = [m['mean'] >= max(4.,4*noise) and m['fraction_ge8'] >= .01 for m in measures]
    return dict(edges(pixels,labels,minimum_events), metrics=measures,
                scope='Fixed-camera, fixed-background AA-off route arbiter; moving/global routes use separately qualified release readers')

def release_edge_details(detail, minimum_events=3):
    require(len(detail) >= minimum_events*2 and len(detail)%2 == 0, 'Zero or insufficient assessed release edges')
    for row in detail:
        require(row['found'] and row['offset'] == 0 and row['status'] == 'observed', 'Release reader reports missing, unmeasured or displaced transition')
        require(not row.get('motion_caveat',False), 'Motion-confounded edge needs route-specific arbiter')
    require(Counter(r['kind'] for r in detail) == {'onset':len(detail)//2,'end':len(detail)//2}, 'Missing edge type')

def speed(baseline, candidate):
    require(len(baseline) >= 3 and len(candidate) == len(baseline), 'Insufficient speed repetitions')
    for a,b in zip(baseline,candidate):
        for row in (a,b):
            require(row['paced'] and not row['instrumentation'] and row['frames'] > 0 and row['real_seconds'] >= 10, 'Speed sample not qualified')
            require(all(math.isfinite(row[k]) and row[k] > 0 for k in ('real_seconds','simulation_seconds')), 'Invalid clocks')
            require(row['ticks_last'] > row['ticks_first'] and row['fps'] > 0, 'Missing native tick span')
            nominal = (row['ticks_last'] - row['ticks_first']) / row['fps']
            require(abs(nominal-row['simulation_seconds']) < 1e-6, 'Nominal span differs from capture tick span/fps')
            require(row['real_seconds']/nominal <= 1.05, 'Speed ratio bound')
        require((a['fps'],a['resolution'],a['fixture']) == (b['fps'],b['resolution'],b['fixture']), 'Speed fixtures differ')
        require(abs(a['real_seconds']/a['simulation_seconds']-b['real_seconds']/b['simulation_seconds']) <= .02, 'Speed ratio regression')

def pairing(rows):
    require(rows, 'Empty renderer receipt domain')
    seen = set()
    for row in rows:
        key = (row['capture'],row['frame'])
        require(key not in seen, 'Duplicate frame receipt')
        seen.add(key)
        require(row['frame'] == row['mask_frame'] and row['family'] == row['mask_family'], 'Stale/mismatched frame family')
        require(row['state_before'] and row['state_before'] == row['state_after'] and row['co_users_before'] == row['co_users_after'], 'State/ownership/co-user residue')
    require(any(r['eligible_count'] > 0 for r in rows) and all(r['census_ms'] <= 100 for r in rows), 'Census eligibility/time floor')

def control(rows):
    require(rows, 'Empty control transcript')
    by_connection = defaultdict(list)
    for r in rows:
        require(not any(re.search(r'token|secret|password',k,re.I) for k in r), 'Secret-named field in protocol evidence')
        by_connection[r['connection']].append(r)
    valid, invalid = 0,0
    for transcript in by_connection.values():
        ops = [r['operation'] for r in transcript]
        if transcript[0]['auth_class'] == 'invalid':
            invalid += 1
            require(ops == ['auth_rejected','closed'], 'Invalid authentication admitted')
        else:
            valid += 1
            require(ops == ['authenticated','snapshot','preview','run_started','run_stopped','closed'], 'Incomplete control exchange/close')
            require(transcript[1]['actors'] > 0 and transcript[2]['image_bytes'] > 0 and transcript[4]['frames'] > 0, 'Empty snapshot/preview/run')
        require(transcript[-1]['callbacks'] == 0 and transcript[-1]['open_sockets'] == 0, 'Dangling control callbacks/socket')
    require(valid >= 2 and invalid >= 1, 'No valid reconnect and invalid-auth control')

def catalogue(baseline,candidate):
    for key in ('catalogue','pool_keys','defaults','seed_sequence','selectors'):
        require(baseline[key] and baseline[key] == candidate[key], 'Catalogue/default/seed regression: '+key)
    require(len(baseline['catalogue']) == 14 and len(baseline['pool_keys']) == 9, 'Wrong released catalogue/pool domain')
    require(not candidate['ordinary_bench_open'] and candidate['flagged_bench_open'], 'Bench flag admission mismatch')

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('gate',choices=['worlds','gc','screenshots','picture','edges','release-edges','speed','control','pairing','admission','catalogue','compile-out','descriptor','events'])
    parser.add_argument('--input',type=Path,required=True)
    parser.add_argument('--output',type=Path,required=True)
    args = parser.parse_args()
    funcs = {'worlds':worlds,'gc':gc,'screenshots':screenshot_pairs,'picture':picture_event,'edges':rgb_edges,
        'release-edges':release_edge_details,'speed':speed,'control':control,'pairing':pairing,'admission':admission,
        'catalogue':catalogue,'compile-out':compile_out,'descriptor':descriptor,'events':event_counts}
    payload = json.loads(args.input.read_text())
    result = {'input':str(args.input),'input_sha256':hashlib.sha256(args.input.read_bytes()).hexdigest(), 'gate':args.gate}
    try:
        result.update(result='PASS',detail=funcs[args.gate](**payload))
    except (GateFailure,KeyError,TypeError,ValueError,OSError) as error:
        result.update(result='FAIL',reason=str(error))
    args.output.write_text(json.dumps(result,indent=2)+'\n')
    return 0 if result['result'] == 'PASS' else 1

if __name__ == '__main__':
    raise SystemExit(main())
