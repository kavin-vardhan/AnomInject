import argparse
import hashlib
import json
from pathlib import Path
import re


def tokens(text):
    return re.sub(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'',
                  lambda m: ''.join('\n' if c == '\n' else ' ' for c in m.group()), text)


def functions(text):
    clean = tokens(text)
    found = {}
    for match in re.finditer(r'FEuropaPhaseAWorldProbe::(\w+)\s*\([^;{}]*\)\s*(?:const\s*)?\{', clean):
        start = match.end()
        depth, end = 1, start
        while depth and end < len(clean):
            depth += (clean[end] == '{') - (clean[end] == '}')
            end += 1
        if depth:
            raise ValueError('Unclosed function')
        if match[1] in found:
            raise ValueError('Ambiguous function')
        found[match[1]] = (start, end - 1, clean[start:end - 1])
    return found


def check(sources):
    violations, locations = [], []
    all_functions = {}
    lookup = re.compile(r'\b(?:FindObject\w*|StaticFindObject\w*|FindFirstObject\w*|StaticFindFirstObject\w*|LoadObject\w*|StaticLoadObject\w*|LoadClass\w*|StaticLoadClass\w*)\s*(?:<|\()')
    actor = re.compile(r'\b(?:T(?:Actor|Object)(?:Iterator|Range)\s*<|FObjectIterator\b|ForEachObjectOfClass\s*\(|GetObjectsOfClass\s*\()')
    for name, text in sources.items():
        clean = tokens(text)
        funcs = functions(text)
        all_functions.update({k: v[2] for k, v in funcs.items()})
        for match in actor.finditer(clean):
            violations.append(dict(file=name, line=text.count('\n', 0, match.start()) + 1, family='actor_or_object_iterator'))
        for match in lookup.finditer(clean):
            containing = [k for k, (a, b, _) in funcs.items() if a <= match.start() < b]
            row = dict(file=name, line=text.count('\n', 0, match.start()) + 1, family='object_lookup', function=containing)
            locations.append(row)
            if containing != ['CacheClasses']:
                violations.append(row)
    for name,text in sources.items():
        clean=tokens(text)
        funcs=functions(text)
        for match in re.finditer(r'\b(?:LoadSynchronous|ClearInternalFlags|SetInternalFlags)\s*\(',clean):
            violations.append(dict(file=name,line=text.count('\n',0,match.start())+1,family='synchronous_load_or_internal_root_change'))
        for function in ('Cleanup','Cleaned','Stop','Emit'):
            if function not in funcs:
                continue
            body=funcs[function][2]
            if re.search(r'\b(?:TWeakObjectPtr\s*<|FWeakObjectPtr\b)',body):
                violations.append(dict(file=name,function=function,family='weak_pointer_constructed_in_teardown'))
            if function in ('Cleaned','Stop'):
                for pointer in re.findall(r'([A-Za-z_]\w*(?:\s*\[[^]]+\])?)\s*->',body):
                    if pointer not in ('Audit','GLog','Data'):
                        violations.append(dict(file=name,function=function,family='unguarded_object_dereference',pointer=pointer))
                if re.search(r'(?:[;{(=,]|return)\s*\*\s*(?:Object|World|Dormant|GCInjector|DriverWorld|QueuedWorld|PictureWorld)\b',body):
                    violations.append(dict(file=name,function=function,family='unguarded_unary_object_dereference'))
    root_change = re.compile(r'\b(?:AddToRoot|RemoveFromRoot)\s*\(')
    for name, text in sources.items():
        clean = tokens(text)
        funcs = functions(text)
        for match in root_change.finditer(clean):
            containing = [k for k, (a, b, _) in funcs.items() if a <= match.start() < b]
            if containing != ['ChangeRoot']:
                violations.append(dict(file=name, line=text.count('\n', 0, match.start())+1,
                                       family='root_change_outside_guarded_helper', function=containing))
    guarded = ('Identity','IsInspectable','ChangeRoot','Start','CacheClasses','Command',
               'EndFrame','Execute','ScreenshotDone','ReleaseGC')
    expected = 'IsEngineExitRequested()||GExitPurge||IsGarbageCollecting()'
    for name in guarded:
        body = all_functions.get(name, '')
        first = re.match(r'\s*if\s*\(([^{};]*)\)\s*\{([^{}]*)\}', body)
        condition = re.sub(r'\s+', '', first[1]) if first else ''
        if not first or condition not in (expected, '!bEnabled||'+expected) or not re.search(r'\breturn\b', first[2]):
            violations.append(dict(family='exit_purge_entry_guard', function=name))
    for name in ('CaptureWorld','LoadScript','RequestPicture','GCProbe','LifetimeProbe','TickStart','TickEnd'):
        body = re.sub(r'\s+', '', all_functions.get(name, ''))
        if not body.startswith('if(UnsafeSnapshotReason(World)){return;}'):
            violations.append(dict(family='world_entry_guard', function=name))
    header = sources.get('EuropaPhaseAWorldProbe.h', '')
    if 'TMap<UPTRINT, FWorldState> Worlds;' not in header:
        violations.append(dict(family='purge_safe_receipt_key'))
    for name, text in sources.items():
        if re.search(r'\bWorlds\s*\.\s*(?:Find|FindOrAdd|Remove)\s*\(\s*World\s*\)', tokens(text)):
            violations.append(dict(file=name, family='weak_world_key_constructed_during_purge'))
    required = ('Cleanup', 'Cleaned', 'Stop', 'Snapshot', 'SnapshotState', 'EndFrame', 'ReleaseGC', 'UnsafeSnapshotReason', 'CacheClasses', 'Initialized')
    for name in required:
        if name not in all_functions:
            violations.append(dict(family='missing_function', function=name))
    if all(name in all_functions for name in required):
        cache_callers = [name for name, body in all_functions.items() if re.search(r'\bCacheClasses\b', body)]
        if cache_callers != ['Initialized']:
            violations.append(dict(family='lookup_capture_reachable', callers=cache_callers))
        cache = all_functions['CacheClasses']
        first_lookup = lookup.search(cache)
        for guard in ('IsEngineExitRequested', 'GExitPurge', 'IsGarbageCollecting'):
            if guard not in cache or first_lookup and cache.index(guard) > first_lookup.start():
                violations.append(dict(family='capture_guard', missing=guard))
            if guard not in all_functions['UnsafeSnapshotReason']:
                violations.append(dict(family='snapshot_guard', missing=guard))
        for name in ('Cleanup', 'Snapshot', 'SnapshotState'):
            if not re.search(r'\bUnsafeSnapshotReason\s*\(', all_functions[name]):
                violations.append(dict(family='missing_snapshot_guard', function=name))
        driver = sources['EuropaPhaseAProbeDriver.cpp']
        for required_text in ('World->GetLevels()', 'Level->Actors', 'skipped_null_actors', 'skipped_unreachable_actors', 'skipped_pending_kill_actors'):
            if required_text not in driver:
                violations.append(dict(family='missing_safe_traversal', missing=required_text))
        for field in ('actor_snapshot', 'skipped', 'skip_reason'):
            if 'TEXT("' + field + '")' not in sources['EuropaPhaseAWorldProbe.cpp']:
                violations.append(dict(family='missing_skip_evidence', missing=field))
    return dict(result='PASS' if not violations else 'FAIL', violations=violations,
                lookup_locations=locations, functions=sorted(all_functions),
                scope='EV5: iterator and lookup/load families across all probe code; lookup only in guarded initialization. Root changes only in a guarded helper; callback entry guards and purge-safe cached receipt keys. No runtime claim.')


def self_tests(sources):
    rows = []
    def test(name, source, expected):
        actual = check(source)
        rows.append(dict(name=name, expected=expected, actual=actual['result'], pass_=actual['result'] == expected,
                         violations=actual['violations']))
    test('real_fixed_probe', sources, 'PASS')
    for name, expression in (
        ('actor_iterator_cleanup', 'TActorIterator<AActor> Bad(World);'),
        ('actor_range_cleanup', 'TActorRange<AActor> Bad(World);'),
        ('find_object_cleanup', 'FindObject<UClass>(nullptr, TEXT("synthetic"));'),
        ('static_find_object_cleanup', 'StaticFindObject(UClass::StaticClass(), nullptr, TEXT("synthetic"));'),
        ('load_object_cleanup', 'LoadObject<UClass>(nullptr, TEXT("synthetic"));')):
        mutant = dict(sources)
        source = mutant['EuropaPhaseAWorldProbe.cpp']
        start = functions(source)['Cleanup'][0]
        mutant['EuropaPhaseAWorldProbe.cpp'] = source[:start] + '\n' + expression + '\n' + source[start:]
        test(name, mutant, 'FAIL')
    mutant = dict(sources)
    mutant['EuropaPhaseAWorldProbe.cpp'] += '\nvoid TeardownHelper() { StaticFindObject(nullptr, nullptr, TEXT("synthetic")); }\n'
    test('lookup_hidden_in_helper', mutant, 'FAIL')
    mutant = dict(sources)
    mutant['EuropaPhaseAWorldProbe.cpp'] = mutant['EuropaPhaseAWorldProbe.cpp'].replace('IsGarbageCollecting()', 'false')
    test('removed_gc_guard', mutant, 'FAIL')
    mutant = dict(sources)
    start = functions(mutant['EuropaPhaseAWorldProbe.cpp'])['Cleanup'][0]
    value = mutant['EuropaPhaseAWorldProbe.cpp']
    mutant['EuropaPhaseAWorldProbe.cpp'] = value[:start] + '\nCacheClasses();\n' + value[start:]
    test('lookup_capture_called_from_cleanup', mutant, 'FAIL')
    mutant = dict(sources)
    value = mutant['EuropaPhaseAWorldProbe.cpp']
    start = functions(value)['Cleanup'][0]
    mutant['EuropaPhaseAWorldProbe.cpp'] = value[:start] + '\nauto Bad = &FEuropaPhaseAWorldProbe::CacheClasses;\n' + value[start:]
    test('lookup_capture_address_taken_from_cleanup', mutant, 'FAIL')
    for name, expression in (
        ('load_class_teardown', 'LoadClass<UObject>(nullptr, TEXT("synthetic"));'),
        ('static_load_class_teardown', 'StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("synthetic"));'),
        ('object_iterator_teardown', 'TObjectIterator<UObject> Bad;'),
        ('object_range_teardown', 'TObjectRange<UObject> Bad;'),
        ('find_first_object_teardown', 'FindFirstObject<UObject>(TEXT("synthetic"));'),
        ('root_remove_after_purge', 'if (GExitPurge) { Dormant[0]->RemoveFromRoot(); }'),
        ('root_add_after_purge', 'if (GExitPurge) { Dormant[0]->AddToRoot(); }')):
        mutant = dict(sources)
        value = mutant['EuropaPhaseAWorldProbe.cpp']
        point = functions(value)['Stop'][0]
        mutant['EuropaPhaseAWorldProbe.cpp'] = value[:point]+'\n'+expression+'\n'+value[point:]
        test(name, mutant, 'FAIL')
    mutant = dict(sources)
    value = mutant['EuropaPhaseAWorldProbe.cpp']
    point = functions(value)['ChangeRoot'][0]
    mutant['EuropaPhaseAWorldProbe.cpp'] = value[:point]+'\nObject->RemoveFromRoot();\n'+value[point:]
    test('root_change_before_helper_guard', mutant, 'FAIL')
    for name, filename in (('ChangeRoot','EuropaPhaseAWorldProbe.cpp'),
                           ('ReleaseGC','EuropaPhaseAProbeDriver.cpp'),
                           ('ScreenshotDone','EuropaPhaseAProbeDriver.cpp'),
                           ('Command','EuropaPhaseAProbeDriver.cpp'),
                           ('Execute','EuropaPhaseAProbeDriver.cpp'),
                           ('Identity','EuropaPhaseAWorldProbe.cpp')):
        mutant = dict(sources)
        value = mutant[filename]
        a, b, _ = functions(value)[name]
        body = value[a:b].replace('GExitPurge', 'false')
        mutant[filename] = value[:a]+body+value[b:]
        test('missing_purge_guard_'+name, mutant, 'FAIL')
    mutant = dict(sources)
    mutant['EuropaPhaseAWorldProbe.h'] = mutant['EuropaPhaseAWorldProbe.h'].replace(
        'TMap<UPTRINT, FWorldState>', 'TMap<TWeakObjectPtr<UWorld>, FWorldState>')
    test('weak_key_receipt_after_purge', mutant, 'FAIL')
    for name, expression, function in (
        ('EW3_static_find_first','StaticFindFirstObject(nullptr, TEXT("synthetic"));','Cleanup'),
        ('EW3_f_object_iterator','FObjectIterator Bad;','Stop'),
        ('EW3_for_each_object','ForEachObjectOfClass(nullptr, [](UObject*) {});','Cleaned'),
        ('EW3_get_objects','GetObjectsOfClass(nullptr, Objects);','Cleaned'),
        ('EW3_clear_internal_root','Object->ClearInternalFlags(EInternalObjectFlags::RootSet);','Stop'),
        ('EW3_soft_load','TSoftObjectPtr<UObject> Soft; Soft.LoadSynchronous();','Cleanup'),
        ('EW3_raw_world_weak','TWeakObjectPtr<UWorld> Bad(World);','Cleaned'),
        ('EW3_plain_dereference','Dormant[0]->GetName();','Stop')):
        mutant=dict(sources);value=mutant['EuropaPhaseAWorldProbe.cpp']
        point=functions(value)[function][0]
        mutant['EuropaPhaseAWorldProbe.cpp']=value[:point]+'\n'+expression+'\n'+value[point:]
        test(name,mutant,'FAIL')
    assert all(r['pass_'] for r in rows), 'Static check self-test failed'
    return rows


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--self-test', action='store_true')
    args = parser.parse_args()
    files = sorted(p for p in args.root.iterdir() if p.name.endswith(('.cpp', '.h', '.cpp.txt', '.h.txt')))
    sources = {p.name.removesuffix('.txt'): p.read_text(encoding='utf-8') for p in files}
    result = check(sources)
    result['files'] = [dict(path=str(p), sha256=hashlib.sha256(p.read_bytes()).hexdigest()) for p in files]
    if args.self_test:
        result['self_tests'] = self_tests(sources)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(dict(result=result['result'], tests=len(result.get('self_tests', [])), violations=result['violations'])))
    raise SystemExit(0 if result['result'] == 'PASS' else 1)
