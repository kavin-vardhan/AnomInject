import copy
import datetime
import json
import os
from pathlib import Path
import sys
import tempfile
import zlib
import numpy as np

P=Path(__file__).resolve().parents[2];T=P/'tools/europa';J=Path('D:/IA_Europa/Receipts/09436')
sys.path.insert(0,str(T))
import capture_c5_reader as r
import capture_c5_kit as kit
import capture_c5_coverage_tests as coverage
from capture_annotation_tests import run as annotation_tests
import job_scope
from rotator_contract import check_all
from capture_fr_tests import violations

def native_contract(source):
    advance=source.split('void FAnomalyMasks425::Advance(')[1].split('void FAnomalyMasks425::Observe(')[0]
    observe=source.split('void FAnomalyMasks425::Observe(')[1].split('TSharedPtr<FJsonObject> FAnomalyMasks425::State')[0]
    return all(x not in advance for x in ('RestoreAll','FlushRenderingCommands',' Release();')) and all(x in advance for x in ('EventTags.Contains(Fire.Id)','Fire.StartFrame==Pair.Value.Start','Batches.Num()>=2','HostDepth(A)','Batches.Add(Request,Batch)')) and 'Batches.Find(Frame.RequestId)' in observe and 'Batches.Remove(Frame.RequestId)' in observe and 'Self->ReleaseUnused();' in observe and 'host_custom_depth_untouched' in source

def run():
    out=[]
    images={};samples=[]
    for i in range(240):
        phase=i%7;image=np.full((40,40,3),90+phase%3,dtype=np.uint8)
        image[15,15]+=i%2*3;images[i]=image;samples.append(dict(index=i,phase=phase))
    refs,bad,check=r.phase_prepass(samples,images.__getitem__)
    assert int(bad.sum())==25 and all(x['fraction']==0 for x in check)
    assert not r.stable_map(list(images.values()))[1]
    mutant={i:a.copy() for i,a in images.items()};mutant[200][:20,:20]+=20
    _,_,check_bad=r.phase_prepass(samples,mutant.__getitem__);assert max(x['fraction'] for x in check_bad)>0.05
    assert r.changed_stats(images[0],refs[0],bad)['fraction']==0
    changed=images[0].copy();changed[:20,:20]+=20
    assert r.changed_stats(changed,refs[0],bad)['fraction']>0.05
    out.append(dict(fix='FT3',result='PASS',negatives=['unmatched phase fails stable share','late motion fails first-half self-check','census perturbation fails interference'],dilation_pixels=int(bad.sum())))
    root=J/'tests/freeze';root.mkdir(parents=True,exist_ok=True);ref=root/'ref';frame=root/'frame';ref.write_bytes(b'reference');frame.write_bytes(b'frame')
    os.utime(ref,(100,100));os.utime(frame,(101,101));frozen=dict(frozen_utc=datetime.datetime.fromtimestamp(100,datetime.timezone.utc).isoformat())
    assert r.freeze_before_frame(frozen,[ref],frame);os.utime(ref,(102,102));assert not r.freeze_before_frame(frozen,[ref],frame)
    out.append(dict(fix='FT5',result='PASS',negative='reference written after first scored frame refused, irrespective of injection receipt'))
    source=(P/'Source/AnomalyCapture/Private/Compat/AnomalyMasks425.cpp').read_text()
    assert native_contract(source)
    for old,new in [('    ReleaseUnused();','    ReleaseUnused(); RestoreAll();'),('Batches.Find(Frame.RequestId)','Batches.Find(0)'),('EventTags.Contains(Fire.Id)','false'),('HostDepth(A)','false')]:assert not native_contract(source.replace(old,new))
    c=dict(component='host',on=True,value=203,write_mask=1,prior_on=True,prior_value=203,prior_write_mask=1,tagged=False,bench_setup=True)
    rows=[dict(frame_index=0,custom_depth=dict(unexpected=0,components=[c]),census=dict(owners=[dict(tag=200)]))]
    before=dict(components=[copy.deepcopy(c)]);assert r.depth_check(before,before,rows)['result']=='PASS'
    c['value']=200;c['tagged']=True;assert r.depth_check(before,before,rows)['result']=='FAIL'
    out.append(dict(fix='FT2',result='PASS',negative='per-frame reset, wrong batch, event retag, host tag mutants refused; changed host state refused despite exact end restoration'))
    out.append(annotation_tests())
    for filename in ('build-job-config.json','c5-job-config.json'):job_scope.load(J/filename)
    assert kit.J==J and kit.AUTHORITY.startswith('094-36')
    for old in ('09435','09434'):
        try:kit.validate(dict(arming_job=old))
        except RuntimeError:pass
        else:raise AssertionError('Spent arming accepted')
    kept=Path('D:/IA_Europa/Receipts/09435/c5-attempts/C5/01/plugin-frames/epoch01/evidence-000050.u32x4.zlib')
    raw=zlib.decompress(kept.read_bytes());planes=np.frombuffer(raw,dtype=np.uint8).reshape(-1,16).T.copy().tobytes()
    packed=zlib.compress(planes);decoded=np.frombuffer(zlib.decompress(packed),dtype=np.uint8).reshape(16,-1).T.copy().tobytes();assert raw==decoded
    out.append(dict(fix='lossless evidence layout',result='PASS',bytes_before=kept.stat().st_size,bytes_after=len(packed)+4,byte_equal=True))
    scan=[str(p) for p in (P/'Source').rglob('*') if p.suffix in ('.cpp','.h','.cs') and violations(p.read_text(encoding='utf-8-sig'))];assert not scan,scan
    result=dict(result='PASS',tests=out,coverage=coverage.run(),rotators=check_all(),python27='N/A: no in-editor or in-game Python changed; driver is native',static_fixture_scan=scan)
    (J/'tests/ft-summary.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
if __name__=='__main__':run()
