import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import time

from environment import execution_env,execution_forbidden,masked
import fixture_policy as policy


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--attempt',type=Path,required=True)
    parser.add_argument('--host',type=Path,required=True)
    parser.add_argument('--recipe',choices=policy.RECIPES,required=True)
    parser.add_argument('--mode',choices=('pass','partial','extra','missing_probe_stop','deadline'),required=True)
    args,extra=parser.parse_known_args()
    if extra!=[policy.UPLOAD]:raise RuntimeError('Stand-in requires the same EU4 switch.')
    attempt=policy.canonical(args.attempt,policy.ROOT/'Receipts')
    rel=attempt.relative_to(policy.ROOT/'Receipts')
    if len(rel.parts)!=6 or rel.parts[0] not in ('09424','CodeReview09424') or rel.parts[1]!='rehearsal' or rel.parts[3]!='fixture-attempts' or rel.parts[4]!=args.recipe:
        raise RuntimeError('Stand-in attempt is not owned rehearsal scratch.')
    case=Path(*rel.parts[:3])
    host=policy.canonical(args.host,policy.ROOT/'Temp'/case)
    if host!=policy.ROOT/'Temp'/case/'host':raise RuntimeError('Stand-in host escaped scratch.')
    if any(execution_forbidden(k) for k in os.environ):raise RuntimeError('Stand-in received blocked execution metadata.')
    files=['SM_EA_TwoLod.uasset','EA_Core.umap'] if args.recipe=='EA_Core' else ['EA_Travel.umap']
    target=host/'Content/EuropaPhaseA';target.mkdir(parents=True,exist_ok=True)
    for name in files:
        with (target/name).open('xb') as stream:stream.write(('Synthetic '+args.recipe+' '+name+'\n').encode())
        if args.mode in ('partial','deadline'):break
    (attempt/'standin-environment.json').write_text(json.dumps(dict(environment=masked(os.environ),blocked_names=[],wrapper_metadata_names=[]),indent=2)+'\n')
    if args.mode=='deadline':
        env,_=execution_env(os.environ)
        child=subprocess.Popen([sys.executable,'-B','-c','import time;time.sleep(120)','--crash-reporter-standin'],env=env,
            creationflags=subprocess.DETACHED_PROCESS|subprocess.CREATE_NEW_PROCESS_GROUP)
        (attempt/'standin-child.json').write_text(json.dumps(dict(pid=child.pid,role='harmless detached crash-reporter stand-in'))+'\n')
        print('Stand-in: waiting past the shortened rehearsal deadline.',flush=True)
        time.sleep(120)
    if args.mode=='partial':
        print('Stand-in: synthetic partial output; exit 3.',flush=True)
        return 3
    if args.mode=='extra':(target/'undeclared.synthetic').write_bytes(b'synthetic extra output')
    probe=attempt/'session/probe';probe.mkdir(parents=True,exist_ok=True)
    rows=[dict(sequence=1,kind='world_cleanup',actor_snapshot='skipped',skip_reason='engine_exit',world_type=4)]
    if args.mode!='missing_probe_stop':rows.append(dict(sequence=2,kind='probe_stop',root_release='skipped'))
    (probe/'probe.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in rows))
    (attempt/'authoring.json').write_text(json.dumps(dict(result='AUTHORED_NOT_QUALIFIED',recipe=args.recipe,synthetic=True))+'\n')
    (attempt/'session/Runtime.log').write_text('Harmless stand-in. No Unreal module loaded.\n')
    print('Stand-in: synthetic outputs and exit rows written.',flush=True)
    time.sleep(2)
    return 0


if __name__=='__main__':raise SystemExit(main())
