import argparse
import hashlib
import json
from pathlib import Path

ROUTES={
 'missing_object':['EA_HideCube'],
 'blinking':['EA_BlinkCube','12'],
 'lighting_mismatch':['EA_Light','dim','0.05'],
 'lod_corruption':['EA_LOD','1']}

def script(route,control):
    if route not in ROUTES or control not in ('positive','NoApply','SkipRestore'): raise ValueError('Unknown recipe')
    rows=[]
    def add(command,args=None,wait=1,success=True):
        rows.append(dict(command=command,args=args or [],wait_frames=wait,expect_success=success))
    add('prepare',wait=90)
    for i in range(20): add('screenshot',['clean-%02d.png'%i],wait=2)
    for event in range(3):
        prefix='%s-%d'%(route,event)
        add('snapshot',[prefix+'-before'])
        add('screenshot',[prefix+'-before.png'])
        add('apply',[route]+ROUTES[route],success=control!='NoApply')
        for i in range(30 if route=='blinking' else 2):
            add('screenshot',[prefix+'-during-%02d.png'%i],wait=1)
        add('snapshot',[prefix+'-active'])
        add('revert',[route],wait=5,success=control=='positive')
        add('snapshot',[prefix+'-after'])
        add('screenshot',[prefix+'-after-0.png'],wait=3)
        add('screenshot',[prefix+'-after-1.png'])
        if control=='SkipRestore': break
    if control=='positive':
        add('apply',['blinking','EA_NO_SUCH_TARGET'],success=False)
        add('apply',['uv_corruption','EA_LOD','tile'],success=False)
        add('apply',['EA_unknown_id'],success=False)
        add('snapshot',['refusals-unchanged'])
    return rows

def selftest():
    rows=[]
    for route in ROUTES:
        for control in ('positive','NoApply','SkipRestore'):
            data=script(route,control)
            assert sum(x['command']=='screenshot' and x['args'][0].startswith('clean-') for x in data)==20
            names=[x['args'][0] for x in data if x['command']=='screenshot']
            assert len(names)==len(set(names))
            assert all(x['wait_frames']>0 for x in data)
            rows.append(dict(route=route,control=control,steps=len(data),pass_=True))
    return dict(result='PASS',cases=rows,executed=0)

def main():
    p=argparse.ArgumentParser();p.add_argument('--output',type=Path,required=True);a=p.parse_args()
    root=Path('D:/IA_Europa').resolve()
    if not a.output.resolve().is_relative_to(root) or a.output.exists(): raise RuntimeError('Fresh owned-D destination required')
    a.output.mkdir(parents=True)
    receipts=[]
    for route in ROUTES:
        for control in ('positive','NoApply','SkipRestore'):
            path=a.output/(route+'-'+control+'.json')
            path.write_text(json.dumps(script(route,control),indent=2)+'\n')
            receipts.append(dict(path=str(path),sha256=hashlib.sha256(path.read_bytes()).hexdigest(),route=route,control=control))
    (a.output/'manifest.json').write_text(json.dumps(dict(result='RECIPE_ONLY',files=receipts,self_tests=selftest(),
        controls='Run NoApply/SkipRestore with their explicit launch flags, fresh output and restarted world. SkipRestore intentionally stops after one residue event.',
        qualification='Freeze actual actor identities and ROIs first; classify blinking visible/hidden frames from pixels, never request timing.'),indent=2)+'\n')
    print('Twelve probe recipes prepared; no Unreal process launched.')

if __name__=='__main__': main()
