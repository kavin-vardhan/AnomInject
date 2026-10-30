import argparse
import datetime
import json
import os
from pathlib import Path

import runtime_policy as policy
from fixture_policy import sha,canonical
from environment import filtered


def arm(output, mode='real'):
    output=canonical(output)
    evidence=policy.JOB/'tooling-qualification.json'
    qualification=json.loads(evidence.read_text())
    if qualification.get('result')!='PASS':raise RuntimeError('Runtime tests/rehearsal qualification absent')
    from fixture_policy import verify_files
    verify_files(qualification['files'])
    verify_files(qualification['source_files'])
    old=json.loads((policy.ROOT/'Launch/Europa425-FixtureAuthoring.json').read_text())
    if old['arming_job']!='09426d':raise RuntimeError('Unexpected authoring predecessor')
    before=policy.protection_snapshot()
    policy.manifest_check(policy.MANIFEST)
    engine_files=list(old['engine_files'])
    extra=Path('E:/EpicGames/UE_4.25/Engine/Plugins/Tests/EditorTests/Binaries/Win64/UE4Editor-EditorTests.dll')
    engine_files.append(dict(path=str(extra),size=extra.stat().st_size,sha256=sha(extra)))
    base,temp=(policy.JOB,policy.TEMP) if mode=='real' else (policy.JOB/'scratch/dry',policy.TEMP/'scratch/dry')
    config=dict(arming_job='09427',authority=policy.AUTHORITY,scope=policy.SCOPE,limits=policy.LIMITS,
        memory=policy.MEMORY,window_deadline_utc=policy.WINDOW_END,manifest=policy.MANIFEST,mode=mode,
        receipts_root=str(base),temp_root=str(temp),slots={leg:policy.paths(base,temp,leg) for leg in policy.LEGS},
        protected=before,host_files=old['host_files'],engine_files=engine_files,
        tooling_files=[dict(path=str(p),size=p.stat().st_size,sha256=sha(p))
                       for p in sorted(policy.TOOLS.rglob('*')) if p.is_file() and p.suffix in ('.py','.txt','.bat')],
        job_config=dict(path=str(policy.JOB/'runtime-job-config.json'),size=(policy.JOB/'runtime-job-config.json').stat().st_size,
                        sha256=sha(policy.JOB/'runtime-job-config.json')),
        entry_proof=str(policy.JOB/'P9/entry/summary.json'),tooling_qualification=str(evidence),
        armed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat())
    import subprocess
    config['source_commit']=subprocess.check_output(['git','-C',str(policy.TOOLS.parents[1]),'rev-parse','HEAD'],
        env=filtered(os.environ),text=True).strip()
    status=subprocess.check_output(['git','-C',str(policy.TOOLS.parents[1]),'status','--porcelain'],env=filtered(os.environ),text=True)
    if status.strip():raise RuntimeError('Commit the qualified runtime tooling before arming')
    policy.validate(config,mode)
    for slot in config['slots'].values():
        if any(Path(p).exists() for p in slot.values()):raise RuntimeError('Runtime slot already exists')
    output.parent.mkdir(parents=True,exist_ok=True)
    with output.open('x',encoding='utf-8') as stream:json.dump(config,stream,indent=2);stream.write('\n')
    print(json.dumps(dict(result='ARMED',mode=mode,config=str(output),sha256=sha(output),legs=len(policy.LEGS))))
    return config


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);p.add_argument('--mode',choices=('real','dry'),default='real')
    a=p.parse_args();arm(a.output,a.mode)
