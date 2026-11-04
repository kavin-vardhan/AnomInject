import json,re
from pathlib import Path
import calibration_reader09444f as calibration
import host_reader09444f as reader
J=Path('D:/IA_Europa/Receipts/09444f')
old=Path('D:/IA_Europa/Receipts/09444d/H4/D3D11-1280/01/session/Runtime.log').read_text(encoding='utf-8-sig')
preview=Path('D:/IA_Europa/Receipts/09444e/H4/Calibration/01/session/Runtime.log').read_text(encoding='utf-8-sig')
lines=[line for line in preview.splitlines() if 'Capture425 SELF_CHECK ' in line][:3]
combined='\n'.join(lines)+'\n'+old
scored,count=reader.scored_section(combined)
prefix=combined[:len(combined)-len(scored)]
filtered='\n'.join(line for line in prefix.splitlines() if 'Capture425 SELF_CHECK ' not in line)+'\n'+scored
assert count==3 and reader.h4.self_checks(filtered)['result']=='PASS'
assert [float(v) for v in re.findall(r'IAI.SetMinScreenCoverage -> ([0-9.]+)%',filtered)]==[0.5]
assert filtered.count('IAI.Auto.Run -> ON (seed 4242,')==1
assert reader.h4.self_checks(filtered.replace('pre_ui=1','pre_ui=0'))['result']=='FAIL'
marker='Capture425 routes=blinking,lod_corruption,missing_texture,corrupted_texture'
for bad in (combined.replace(marker,'absent'),combined+'\n'+marker,combined[len(lines[0])+1:]):
    try:reader.scored_section(bad)
    except AssertionError:pass
    else:raise AssertionError('bad capture segmentation admitted')
result=dict(result='PASS',calibration=calibration.test(),self_checks=dict(positive=1,negative=4,full_log_floor_and_seed_retained=True))
(J/'reader-tests.json').write_text(json.dumps(result,indent=2))
print(json.dumps(result))
