import json
from pathlib import Path
import numpy as np
from capture_c5_reader import c2_result,drawn_names,ROUTES

def run():
    rows=[dict(sweep=dict(score=True))]
    events=[dict(anomaly=a,scored=True) for a in ROUTES for unused in range(3)]
    assert c2_result(rows,events,[],[])['result']=='PASS'
    assert c2_result([dict(sweep=dict(score=False))],[],[],[])['result']=='FAIL'
    assert c2_result(rows,events[:-1],[],[])['result']=='FAIL'
    assert c2_result(rows,events,[dict(index=0)],[])['result']=='FAIL'
    bank={'small':np.packbits([False,True,False]),'occluded':np.packbits([False,False,False])}
    assert drawn_names(bank)==['small']
    assert 'occluded' not in drawn_names(bank)
    return dict(result='PASS',positive='three complete events for each anomaly and scored frames',negatives=['empty scoring coverage refused','insufficient events refused','mismatch refused','zero-pixel census actor excluded'],visible='nonzero pixels count before the eligibility coverage floor')

if __name__=='__main__':
    import sys
    result=run()
    if len(sys.argv)>1:Path(sys.argv[1]).write_text(json.dumps(result,indent=2))
    print(json.dumps(result))
