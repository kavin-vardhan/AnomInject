import argparse
import copy
import json
import os
from pathlib import Path
from unittest.mock import patch

import root_attribution as roots


def tests(output):
    output=Path(output);output.mkdir(parents=True,exist_ok=False)
    scratch=output/'metadata-only';scratch.mkdir()
    (scratch/'recording.mkv').write_bytes(b'synthetic foreign media')
    with patch.object(Path,'open',side_effect=AssertionError('Foreign file contents must never be opened')):
        metadata=roots.metadata(scratch)
    cases=[]
    def check(name,condition):
        cases.append(dict(case=name,passed=bool(condition)))
        assert condition,name
    def change(path='E:/SyntheticForeign',removed=False):
        return dict(path=path,before_kind='directory' if removed else None,after_kind=None if removed else 'directory')
    def decide(c=None,details=metadata,contexts=()):
        return roots.decide(c or change(),contexts,['E:/IA_BuildCache','D:/UESource'],[roots.ROOT,roots.RUN],details)
    check('foreign_only_new_folder_continues_without_opening_files',decide()['result']=='FOREIGN' and metadata['files']==1 and metadata['file_contents_opened']==0)
    bad=copy.deepcopy(metadata);bad['port_pattern_matches']=['E:/SyntheticForeign/probe.jsonl']
    check('port_pattern_stops',decide(details=bad)['result']=='STOP')
    for field in ('command_line','working_folder','environment'):
        row={field:['e:/syntheticforeign']}
        check('own_'+field+'_stops',decide(contexts=[row])['result']=='STOP')
    check('removed_foreign_continues',decide(change(removed=True),None)['result']=='FOREIGN')
    check('removed_foreign_D_continues',decide(change('D:/SyntheticForeign',True),None)['result']=='FOREIGN')
    check('removed_P9_root_RED',decide(change('E:/IA_BuildCache',True),None)['result']=='RED')
    check('removed_P9_D_root_RED',decide(change('D:/UESource',True),None)['result']=='RED')
    check('adjacent_D_remains_port_attributed',decide(change('D:/SyntheticForeign'))['result']=='STOP')
    check('inside_port_remains_port_attributed',decide(change('D:/IA_Europa/Synthetic'))['result']=='STOP')
    check('missing_metadata_refuses',decide(details=None)['result']=='STOP')
    bad=copy.deepcopy(metadata);bad['errors']=[dict(type='PermissionError')]
    check('unreadable_metadata_refuses',decide(details=bad)['result']=='STOP')
    for name in ('Runtime.log','probe.jsonl','fixture_leg.py','receipt.json','ledger.sqlite','cache.ddc','cache.ddp','cache.udd','map.umap','mesh.uasset','data.ucas','ShaderWorking','full_suites09425.py','filtered09425.ps1'):
        check('pattern_'+name,any(roots.fnmatch.fnmatchcase(name.casefold(),p) for p in roots.port_patterns()))
    (scratch/'filtered09425.ps1').write_bytes(b'synthetic port script')
    with patch.object(Path,'open',side_effect=AssertionError('Foreign file contents must never be opened')):
        port_metadata=roots.metadata(scratch)
    check('run_script_metadata_stops_without_opening_files',decide(details=port_metadata)['result']=='STOP' and port_metadata['file_contents_opened']==0)
    for name in ('report.md','last-message.txt','summary.json','child-001.json'):
        check('receipt_pattern_'+name,any(roots.fnmatch.fnmatchcase(name.casefold(),p) for p in roots.port_patterns()))
    refs=roots.context(['python','--output=E:/SyntheticForeign/out.json'],'D:/IA_Europa',{'TEMP':'E:/SyntheticForeign/tmp'})
    check('references_have_no_environment_values',refs['environment']==['e:\\syntheticforeign'] and refs['values_recorded'] is False)
    import write_ledger as ledger
    for name,c,details,expected in [('new',change(),metadata,'FOREIGN'),
        ('removed',change(removed=True),None,'FOREIGN'),
        ('protected',change('E:/IA_BuildCache',True),None,'RED')]:
        attribution=decide(c,details)
        item=dict(c,scope='e_root',before_size=0,after_size=0,root_attribution=attribution)
        failed=False
        with patch.object(ledger,'LEDGER',output/'ledger'),patch.object(ledger,'floors'),\
             patch.object(ledger,'snapshot',return_value=({},[item])),patch.dict(os.environ):
            try:ledger.begin(name,'read_only')
            except RuntimeError:failed=True
        saved=json.loads((output/'ledger'/name/'between-operations.json').read_text())
        check('persisted_'+name+'_root_attribution',saved['root_attributions']==[attribution]
              and attribution['result']==expected and failed==(expected=='RED'))
    result=dict(result='PASS',cases=cases,patterns=roots.port_patterns(),metadata=metadata,
        synthetic_external_paths_created=0,foreign_file_contents_opened=0,unreal_launches=0)
    (output/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(dict(result='PASS',cases=len(cases),file_contents_opened=0)))
    return result


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True)
    tests(p.parse_args().output)
