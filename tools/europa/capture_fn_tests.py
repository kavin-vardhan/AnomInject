import copy
import json
from pathlib import Path
import sys
import capture_425_kit as kit
import capture_425_reader as reader
import job_scope

def main(output):
    cases=[]
    for leg in ('Smoke','C1PIE','C1Game'):
        argv=kit.command(leg,kit.JOB/'scratch'/leg)
        kit.require_frame_format(argv)
        assert not any('defaultbackbufferpixelformat' in a.casefold() for a in argv)
        mutant=argv+['-ini:Engine:[/Script/Engine.RendererSettings]:r.DefaultBackBufferPixelFormat=0']
        try:kit.require_frame_format(mutant)
        except RuntimeError:pass
        else:raise AssertionError('Backbuffer override accepted')
        cases.append(dict(case=leg+'_stock_format_and_override_negative',result='PASS'))
    data=dict(requested=60,written=60,rhi='D3D11',backbuffer_format=18,dropped=0,pitch=1280,mapped_height=720,elapsed_seconds=9,
        frames=[dict(index=i,request=i+1,file='frame-%06d.png'%i,width=1280,height=720,format=18,aa=0) for i in range(60)])
    reader.metadata(data)
    for field in ('backbuffer_format','frame_format'):
        bad=copy.deepcopy(data)
        if field=='backbuffer_format':bad[field]=10
        else:bad['frames'][0]['format']=10
        try:reader.metadata(bad)
        except ValueError:pass
        else:raise AssertionError('Unsupported format admitted')
    cases.append(dict(case='10bit_receipt_and_unsupported_format',result='PASS'))
    cfg=json.loads((kit.JOB/'build-job-config.json').read_text())
    assert job_scope.fm1(cfg,kit.JOB)
    assert not job_scope.fm1(cfg,kit.ROOT/'Receipts/unarmed')
    cases.append(dict(case='09432_FM1_scope_and_foreign_scope_refusal',result='PASS'))
    old=dict(arming_job='09431b',authority=kit.AUTHORITY)
    try:kit.validate(old)
    except RuntimeError:pass
    else:raise AssertionError('Spent arming accepted')
    cases.append(dict(case='spent_arming_refused',result='PASS'))
    import write_ledger as ledger
    original=ledger.shutil.disk_usage
    try:
        ledger.shutil.disk_usage=lambda path:type('Usage',(),dict(free=100*1024**3))()
        limits=ledger.floors(True)['D']
        assert limits['start']==25*1024**3 and limits['floor']==15*1024**3
        ledger.shutil.disk_usage=lambda path:type('Usage',(),dict(free=(14 if path.startswith('D:') else 100)*1024**3))()
        try:ledger.floors()
        except RuntimeError:pass
        else:raise AssertionError('D floor breach admitted')
    finally:ledger.shutil.disk_usage=original
    cases.append(dict(case='FJ1_D_25_start_15_floor_and_breach_refusal',result='PASS'))
    result=dict(result='PASS',cases=cases,launches=0,native_converter='AnomalyPixel425.h synthetic-buffer static_asserts must also compile in both actual targets')
    Path(output).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))

if __name__=='__main__':main(sys.argv[1])
