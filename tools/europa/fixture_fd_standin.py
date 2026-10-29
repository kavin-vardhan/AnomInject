import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import time

from environment import execution_env
from fixture_policy import canonical,ROOT

parser=argparse.ArgumentParser()
parser.add_argument('--scratch',required=True)
parser.add_argument('--action',required=True)
parser.add_argument('--fc6-ubt-standin',action='store_true')
args,extra=parser.parse_known_args()
scratch=canonical(args.scratch,ROOT/'Temp/09426b/fd-rehearsal')
if args.fc6_ubt_standin:
    if args.action=='write':
        time.sleep(.8)
        path=scratch/'host/Binaries/Win64/synthetic.dll'
        path.parent.mkdir(parents=True,exist_ok=True)
        path.write_bytes(b'FD2 stand-in; not an executable')
    time.sleep(2.5)
else:
    modes={'query':['-Mode=QueryTargets'],'validate':['-Mode=ValidatePlatforms','-allplatforms'],
           'build':['-Mode=Build'],'triple':['Target','Win64','Development'],'write':[],'commit':[]}
    env,_=execution_env(os.environ)
    child=subprocess.Popen([sys.executable,'-B',__file__,'--scratch',str(scratch),'--action',args.action,
                            '--fc6-ubt-standin']+modes[args.action],env=env)
    (scratch/'child.json').write_text(json.dumps(dict(pid=child.pid)))
    child.wait()
