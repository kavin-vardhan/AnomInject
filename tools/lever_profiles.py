import hashlib
import json
from pathlib import Path
import re
import subprocess

DECLARATIONS=(
    'Source/AnomalyInjector/Public/Compat/AnomalyEngineVersion.h',
    'Source/AnomalyCapture/Private/Compat/AnomalyCaptureFeatures.h',
    'Source/AnomalyBench/AnomalyBench.Build.cs',
    'Source/AnomalyCapture/AnomalyCapture.Build.cs',
    'Source/AnomalyControlServer/AnomalyControlServer.Build.cs')

def sha(path):
    with Path(path).open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()

def configurations(engine):
    if engine not in ('425','51'):raise ValueError('Explicit supported engine profile required')
    major,minor=(4,25) if engine=='425' else (5,1)
    result={}
    for config in ('Shipping','Development','Editor'):
        ship=config=='Shipping';editor=config=='Editor'
        result[config]=dict(ENGINE_MAJOR_VERSION=major,ENGINE_MINOR_VERSION=minor,
            ANOMALY_ENGINE_425=int(engine=='425'),ANOMALY_ENGINE_51=int(engine=='51'),
            UE_BUILD_DEBUG=0,UE_BUILD_TEST=0,UE_BUILD_SHIPPING=int(ship),UE_BUILD_DEVELOPMENT=int(not ship),
            WITH_DEV_AUTOMATION_TESTS=int(not ship),WITH_EDITOR=int(editor),WITH_EDITORONLY_DATA=int(editor),
            ANOMALY_CAPTURE=int(not ship),ANOMALY_CONTROL_SERVER=int(not ship),ANOMALY_BENCH=int(engine=='51' and not ship),
            ANOMALY_CAPTURE_ADVANCED=int(engine=='51' and not ship),ANOMALY_CAPTURE_FRAMES425=int(engine=='425' and not ship))
    return result

def declaration_proof(root):
    root=Path(root)
    texts={p:re.sub(r'\s+','', (root/p).read_text(encoding='utf-8-sig')) for p in DECLARATIONS}
    required={
        DECLARATIONS[0]:['#defineANOMALY_ENGINE_425(ENGINE_MAJOR_VERSION==4&&ENGINE_MINOR_VERSION==25)','#defineANOMALY_ENGINE_51(ENGINE_MAJOR_VERSION==5&&ENGINE_MINOR_VERSION==1)'],
        DECLARATIONS[1]:['#defineANOMALY_CAPTURE_ADVANCED(ANOMALY_CAPTURE&&ANOMALY_ENGINE_51)','#defineANOMALY_CAPTURE_FRAMES425(ANOMALY_CAPTURE&&ANOMALY_ENGINE_425)'],
        DECLARATIONS[2]:['boolbEnabled=AnomalyEngineRules.Is51(Target)&&Target.Configuration!=UnrealTargetConfiguration.Shipping&&(Target.Type==TargetType.Game||Target.Type==TargetType.Editor);','PublicDefinitions.Add(bEnabled?"ANOMALY_BENCH=1":"ANOMALY_BENCH=0");'],
        DECLARATIONS[3]:['if((AnomalyEngineRules.Is51(Target)||AnomalyEngineRules.Is425(Target))&&Target.Configuration!=UnrealTargetConfiguration.Shipping)','PublicDefinitions.Add("ANOMALY_CAPTURE=1");','PublicDefinitions.Add("ANOMALY_CAPTURE=0");'],
        DECLARATIONS[4]:['boolbEnabled=(AnomalyEngineRules.Is425(Target)||AnomalyEngineRules.Is51(Target))&&Target.Configuration!=UnrealTargetConfiguration.Shipping;','PublicDefinitions.Add(bEnabled?"ANOMALY_CONTROL_SERVER=1":"ANOMALY_CONTROL_SERVER=0");']}
    for path,items in required.items():
        if any(item not in texts[path] for item in items):raise ValueError('Unsupported profile declaration: '+path)
    return [dict(path=p,sha256=sha(root/p)) for p in DECLARATIONS]

def source_fingerprint(root):
    root=Path(root);files=sorted(p for p in (root/'Source').rglob('*') if p.is_file())+[root/'AnomalyInjector.uplugin']
    if len(files)<2:raise ValueError('Empty source inventory')
    rows=[dict(path=p.relative_to(root).as_posix(),sha256=sha(p)) for p in files]
    return hashlib.sha256(json.dumps(rows,sort_keys=True,separators=(',',':')).encode()).hexdigest()

def checked_json(path,pin):
    if not pin or sha(path)!=pin:raise ValueError('Missing or wrong receipt pin')
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))

def validate_provenance(root,engine,config,binaries,path,pin):
    p=checked_json(path,pin)
    if p.get('engine')!=engine or p.get('configuration')!=config:raise ValueError('Wrong binary profile')
    if p.get('source_fingerprint')!=source_fingerprint(root):raise ValueError('Wrong built-source fingerprint')
    if p.get('declarations')!=declaration_proof(root):raise ValueError('Changed profile declarations')
    build=checked_json(p['build_receipt'],p['build_receipt_sha256'])
    expected_host='425' if engine=='425' else '51port'
    actual_config='Editor' if build['target'].endswith('Editor') else build['configuration']
    if build.get('gate_result')!='PASS' or build.get('exit_code')!=0 or build.get('query_only') or build['host']!=expected_host or actual_config!=config:
        raise ValueError('Build does not establish this target/profile')
    if build.get('rules_version')!=([4,25] if engine=='425' else [5,1]) or build['configuration']!='Development':raise ValueError('Unsupported build configuration')
    diff=subprocess.run(['git','-C',str(root),'diff','--exit-code',build['commit'],'--','Source','AnomalyInjector.uplugin'],capture_output=True)
    if diff.returncode:raise ValueError('Current source differs from built commit')
    inventory={str(Path(r['path']).resolve()):r for r in build['host_binary_inventory'] if r.get('exists')}
    requested={str(Path(x).resolve()) for x in binaries}
    supplied={str(Path(r['path']).resolve()):r for r in p['artifacts']}
    if not requested or requested!=set(supplied) or len(supplied)!=len(p['artifacts']):raise ValueError('Wrong or empty artifact inventory')
    for item in requested:
        if item not in inventory:raise ValueError('Artifact absent from build inventory')
        actual=Path(item);row=supplied[item];built=inventory[item];s=actual.stat()
        if actual.suffix.lower() not in ('.exe','.dll') or row['sha256']!=built['sha256'] or sha(actual)!=row['sha256'] or s.st_size!=built['size'] or s.st_mtime_ns!=built['mtime_ns']:
            raise ValueError('Binary hash/size/time provenance mismatch')
    macros=configurations(engine)[config]
    required={'AnomalyBench':'ANOMALY_BENCH','AnomalyCapture':'ANOMALY_CAPTURE','AnomalyControlServer':'ANOMALY_CONTROL_SERVER'}
    seen=set()
    for row in p['module_definitions']:
        module=row['module']
        if module not in required or module in seen:raise ValueError('Wrong/duplicate module definition')
        if sha(row['path'])!=row['sha256']:raise ValueError('Changed compiled definitions')
        text=Path(row['path']).read_text(encoding='utf-8-sig')
        definitions={k:int(v) for k,v in re.findall(r'^\s*#define\s+(\w+)\s+([01])\s*$',text,re.M)}
        for key in (required[module],'UE_BUILD_SHIPPING','UE_BUILD_DEVELOPMENT','WITH_EDITOR','WITH_EDITORONLY_DATA','WITH_DEV_AUTOMATION_TESTS'):
            if definitions.get(key)!=macros[key]:raise ValueError('Actual module macro disagrees with profile: '+module+'/'+key)
        seen.add(module)
    if seen!=set(required):raise ValueError('Missing module definition provenance')
    return dict(result='PASS',engine=engine,configuration=config,built_commit=build['commit'],artifacts=len(requested),source_fingerprint=p['source_fingerprint'])
