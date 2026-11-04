import datetime,hashlib,json,sys
from pathlib import Path
import numpy as np
from PIL import Image
sys.path.insert(0,'D:/IA_Europa/StylizedParisStreet/Plugins/AnomalyInjector/tools/europa')
from capture_c4_reader import raw,silhouette
from window09445r import require_reader
J=Path('D:/IA_Europa/Receipts/09445r')
REFERENCE=Path('D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/plugin-frames/epoch01')

def erode(mask):
    out=np.zeros_like(mask);h,w=mask.shape
    out[2:-2,2:-2]=np.logical_and.reduce([mask[2+y:h-2+y,2+x:w-2+x] for y in range(-2,3) for x in range(-2,3)])
    return out

def clusters(values):
    c=np.percentile(values,[20,80])
    for _ in range(20):
        low=values<(c[0]+c[1])/2
        if not low.any() or low.all():return dict(contrast=0,low_share=0,high_share=0)
        c=np.array([values[low].mean(),values[~low].mean()])
    return dict(low=float(c[0]),high=float(c[1]),contrast=float(c[1]-c[0]),low_share=float(low.mean()),high_share=float((~low).mean()))

def qualifies(base,saturated,pink,group):
    return bool(np.all(base>=25.5) and saturated==0 and np.all(pink[:,0]>=64) and np.all(pink[:,2]>=64) and np.all(pink[:,0]>=2.5*pink[:,1]) and np.all(pink[:,2]>=2.5*pink[:,1]) and group['contrast']>=20)

def read(root,prefire=False):
    require_reader(180)
    root=Path(root);receipt=json.loads((root/('prefire.json' if prefire else 'calibration.json')).read_text())
    assert receipt['preview_count']==(3 if prefire else 198) and not receipt['auto_injection']
    from geometry_reader09445r import read as geometry_read
    source=None
    def frames(sample):
        folder=Path(sample['directory']);cap=json.loads((folder/'capture.json').read_text());assert cap['written']==8 and cap['dropped']==0
        return np.stack([np.asarray(Image.open(folder/r['file']).convert('RGB')) for r in cap['frames']]).astype(float)
    settings=json.loads((root.parent/'settings.json').read_text())
    settings_values={r['name']:r['after'] for r in settings['cvars']}
    assert settings['bench'] and settings['runtime_only'] and settings['map']=='EA_Core'
    assert settings_values['r.BloomQuality']==0 and settings_values['r.EyeAdaptationQuality']==2 and settings_values['r.EyeAdaptation.MethodOverride']==3
    assert len(settings['shadowless_fills'])==6 and all(x['intensity']==3 and not x['shadows'] for x in settings['shadowless_fills'])
    assert all(x['after']==0 for x in settings['point_lights'])
    assert {x['actor'] for x in settings['base_targets']}=={'EA_HideCube','EA_BlinkCube','EA_LOD','EA_Floor'}
    assert not settings['texture_exclusions'] and settings['base_parent'].endswith('BasicShapeMaterial.BasicShapeMaterial')
    table=[]
    for i in range(receipt['preview_count']//3):
        samples=receipt['samples'][i*3:i*3+3];assert [r['material'] for r in samples]==[0,1,2]
        assert len({r['exposure'] for r in samples})==1
        if not prefire:assert samples[0]['exposure']==(-5 if i==0 else -.125*(i-1))
        sources=[]
        for sample in samples:
            geometry,regions=geometry_read(Path(sample['geometry']))
            assert geometry['result']=='PASS'
            sources.append(regions)
        source=sources[0]
        assert all(np.array_equal(source[name],regions[name]) for regions in sources[1:] for name in source),'Geometry differs across material previews'
        baseline,checker,pink=[frames(s) for s in samples];h,w=baseline.shape[1:3]
        objects=[]
        for center in samples[0]['centers']:
            region=source[center['actor']]
            assert (h,w)==region.shape,'Same-session geometry extent differs'
            front=erode(region)
            assert region.sum()>100 and front.sum()>100 and center['projected']
            base=baseline[:,region,:].mean(axis=(1,2));p=pink[:,region,:].mean(axis=1)
            group=clusters(checker.mean(axis=0)[front].mean(axis=1));sat=int(np.count_nonzero(baseline[:,region,:].max(axis=2)==255))
            fp=pink[:,front,:].mean(axis=1)
            good=qualifies(base,sat,p,group)
            swaps={}
            for anomaly,images in [('missing_texture',checker),('corrupted_texture',pink)]:
                counts=np.count_nonzero(np.max(np.abs(images-baseline[-1]),axis=3)[:,region]>2,axis=1)
                fraction=float(counts.min()/region.sum())
                swaps[anomaly]=dict(changed_pixels_min=int(counts.min()),silhouette_pixels=int(region.sum()),fraction_min=fraction,g3_mode='strict' if fraction>=.98 else 'explained-pixel G3 fallback')
            objects.append(dict(actor=center['actor'],pixels=int(region.sum()),front_pixels=int(front.sum()),baseline_mean_min=float(base.min()),baseline_mean_max=float(base.max()),saturated_pixels=sat,pink_min=p.min(axis=0).tolist(),pink_max=p.max(axis=0).tolist(),front_pink_min=fp.min(axis=0).tolist(),front_pink_max=fp.max(axis=0).tolist(),checker=group,swaps=swaps,result='PASS' if good else 'NO_QUALIFICATION'))
        table.append(dict(exposure=samples[0]['exposure'],objects=objects,qualified=all(o['result']=='PASS' for o in objects)))
    chosen=next((r for r in sorted(table,key=lambda r:r['exposure'],reverse=True) if r['qualified']),None)
    result=dict(result='PASS' if chosen else 'FAIL',utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),table=table,reference='Same-session private geometry before every material preview',no_fires=True,selection='Brightest common qualifying exposure on frozen descending grid; initial -5EV probe retained',authority='094-45 GM2 and GK2',settings=settings,rule='GK2: bloom off; full target mean>=25.5 and no channel255; full-target pink R/B>=64 and each>=2.5G; checker grey contrast>=20 on eroded silhouette; brightest passing frozen-grid exposure. GM2 pre-fire >2RGB swap visibility>=.98; lower visibility explicitly selects explained-pixel fallback for that target/texture only.')
    ((root/'prefire-result.json') if prefire else (J/'calibration-result.json')).write_text(json.dumps(result,indent=2))
    if chosen and not prefire:
        frozen=dict(result='PASS',utc=result['utc'],exposure=chosen['exposure'],objects=chosen['objects'],calibration=str(root),prediction_sha256=hashlib.sha256((J/'prediction.json').read_bytes()).hexdigest())
        with (J/'freeze.json').open('x') as stream:json.dump(frozen,stream,indent=2)
    return result

def test():
    b=np.full(8,128);p=np.tile([128,20,128],(8,1));g=clusters(np.array([70]*100+[100]*100))
    assert qualifies(b,0,p,g)
    assert qualifies(np.full(8,254),0,p,g)
    assert not qualifies(b,1,p,g)
    assert not qualifies(np.full(8,25),0,p,g)
    assert not qualifies(b,0,np.tile([255,225,254],(8,1)),g)
    assert not qualifies(b,0,np.tile([63,0,128],(8,1)),g)
    assert not qualifies(b,0,np.tile([128,0,63],(8,1)),g)
    assert not qualifies(b,0,p,clusters(np.array([70]*100+[89]*100)))
    return dict(result='PASS',positive=2,negatives=6,authority='GK2')
