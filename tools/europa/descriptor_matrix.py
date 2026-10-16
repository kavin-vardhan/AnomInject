import argparse
import hashlib
import json
from pathlib import Path
from write_ledger import JOB,dump
from process_wrapper import HOSTS,run_json_batch

ALL={'AnomalyBench','AnomalyShaders','AnomalyInjector','AnomalyCapture','AnomalyControlServer'}

def predicates(label,mutated=False):
    path=JOB/'matrices'/label/'descriptor-predicates.json'
    rows=json.loads(path.read_text())['rows']
    types={'Game','Editor','Client','Server','Program'}
    configs={'Debug','DebugGame','Development','Test','Shipping'}
    if {(r['target_type'],r['configuration']) for r in rows}!={(t,c) for t in types for c in configs}:raise RuntimeError('Incomplete actual-UBT predicate domain')
    for r in rows:
        required=set() if r['configuration']=='Shipping' or r['target_type']=='Program' else ALL-({'AnomalyBench'} if r['target_type'] in ('Client','Server') else set())
        if mutated and r['configuration']=='Shipping' and r['target_type']!='Program':required={'AnomalyShaders'}
        if set(r['ia_modules'])!=required:raise RuntimeError('Native UBT descriptor predicate mismatch: '+json.dumps(r))
    return rows

def pairs(host):
    name=HOSTS[host][0].stem
    game=['DebugGame','Development','Shipping'] if host=='425' else ['Debug','DebugGame','Development','Test','Shipping']
    editor=['DebugGame','Development'] if host=='425' else ['Debug','DebugGame','Development']
    return [(name,c) for c in game]+[(name+'Editor',c) for c in editor]

def collect(label):
    folder=JOB/'matrices'/label
    rows=[]
    for p in folder.glob('*.json'):
        if not any(p.stem.endswith('-'+c) for c in ('Debug','DebugGame','Development','Test','Shipping')):continue
        x=json.loads(p.read_text(encoding='utf-8-sig'))
        if not isinstance(x,dict) or 'Modules' not in x:continue
        mods=x['Modules']
        row={'target':x['Name'],'configuration':x['Configuration'],'platform':x['Platform'],'all_modules':sorted(mods),'ia_modules':sorted(ALL&mods.keys()),'json':str(p),'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'ia_definitions':{name:mods[name].get('PublicDefinitions',[]) for name in sorted(ALL&mods.keys())}}
        rows.append(row)
    return rows

def verify(label,host):
    rows=collect(label)
    expected=pairs(host)
    if {(x['target'],x['configuration']) for x in rows}!=set(expected):raise RuntimeError('Missing/duplicate target configurations')
    for r in rows:
        required=set() if r['configuration']=='Shipping' else ALL
        if set(r['ia_modules'])!=required:raise RuntimeError('Descriptor mismatch '+json.dumps(r))
        if required and host!='51base':
            on='0' if host=='425' else '1'
            for module,define in [('AnomalyCapture','ANOMALY_CAPTURE'),('AnomalyControlServer','ANOMALY_CONTROL_SERVER'),('AnomalyShaders','ANOMALY_SHADERS'),('AnomalyBench','ANOMALY_BENCH')]:
                if define+'='+on not in r['ia_definitions'][module]:raise RuntimeError('Profile define mismatch '+module)
    native=predicates(label)
    dump(JOB/'matrices'/label/'matrix-summary.json',{'result':'PASS','host':host,'rows':rows,'native_predicate_rows':native,'invalid_pairs':'No Client, Server or Program Target.cs exists in these hosts; their filters are separately exercised through actual UBT native parser/predicates in the same invocation. Installed 4.25 supports Game DebugGame/Development/Shipping and Editor DebugGame/Development. Source 5.1 supports five Game configurations and Debug/DebugGame/Development Editor; Editor Test/Shipping are invalid. No Client/Server/Program build is claimed.'})
    return rows

def mutant(host,label):
    project=HOSTS[host][0]
    descriptor=project.parent/'Plugins/AnomalyInjector/AnomalyInjector.uplugin'
    saved=descriptor.read_bytes();before=hashlib.sha256(saved).hexdigest()
    receipt=JOB/'mutants'/label;receipt.mkdir(parents=True)
    (receipt/'descriptor-before.uplugin').write_bytes(saved)
    data=json.loads(saved.decode('utf-8-sig'))
    shader=next(m for m in data['Modules'] if m['Name']=='AnomalyShaders')
    assert shader.pop('BlacklistTargetConfigurations')==['Shipping']
    try:
        descriptor.write_text(json.dumps(data,indent='\t')+'\n',encoding='utf-8')
        run_json_batch(host,[(project.stem,'Shipping')],label)
        rows=collect(label)
        if len(rows)!=1 or set(rows[0]['ia_modules'])!={'AnomalyShaders'}:raise RuntimeError('Removed-blacklist mutant did not change the matrix as predicted')
        native=predicates(label,True)
        dump(receipt/'result.json',{'result':'PASS','can_fail_observed':True,'unmutated_expected':[],'mutant_actual':rows[0]['ia_modules'],'normal_gate_result_on_mutant':'FAIL','row':rows[0],'native_predicate_rows':native})
    finally:
        descriptor.write_bytes(saved)
        if hashlib.sha256(descriptor.read_bytes()).hexdigest()!=before:raise RuntimeError('Descriptor restore failed')
        dump(receipt/'restore.json',{'restored_sha256':before,'byte_equal':True})

def restored(host,label):
    project=HOSTS[host][0]
    run_json_batch(host,[(project.stem,'Shipping')],label)
    rows=collect(label)
    if len(rows)!=1 or rows[0]['ia_modules']:raise RuntimeError('Restored descriptor matrix failed')
    dump(JOB/'matrices'/label/'restore-proof.json',{'result':'PASS','rows':rows,'native_predicate_rows':predicates(label)})

def main():
    ap=argparse.ArgumentParser();ap.add_argument('host',choices=HOSTS);ap.add_argument('label');ap.add_argument('--mutant',action='store_true');ap.add_argument('--restored',action='store_true');a=ap.parse_args()
    if a.mutant:mutant(a.host,a.label);return
    if a.restored:restored(a.host,a.label);return
    run_json_batch(a.host,pairs(a.host),a.label)
    rows=verify(a.label,a.host)
    if a.host=='51port':
        old=json.loads((JOB/'matrices/51base/matrix-summary.json').read_text())['rows']
        b={(r['target'],r['configuration']):r for r in old}
        diffs=[]
        for r in rows:
            x=b[(r['target'],r['configuration'])]
            if r['all_modules']!=x['all_modules']:diffs.append({'target':r['target'],'configuration':r['configuration'],'removed':sorted(set(x['all_modules'])-set(r['all_modules'])),'added':sorted(set(r['all_modules'])-set(x['all_modules']))})
        dump(JOB/'matrices/51-comparison.json',{'result':'FAIL' if diffs else 'PASS','differences':diffs,'pairs':len(rows),'scope':'Entire exported module membership, including engine dependencies, plus IA profile checks.'})
        if diffs:raise RuntimeError('5.1 module membership differs from release')
        normal=lambda label:sorted((r['target_type'],r['configuration'],sorted(r['ia_modules'])) for r in predicates(label))
        if normal(a.label)!=normal('51base'):raise RuntimeError('5.1 native descriptor predicates differ from release')

if __name__=='__main__':main()
