import json
import os
import re
import time
from pathlib import Path

names=('r.VT.SyncProduceLockedTiles','r.VT.MaxTilesProducedPerFrame')

def monitor(process,root,out,macro):
    start=time.time()
    record=dict(started=start,restore_after_written_frames=128,original={},restored=False)
    path=root/'upload-delay.json'
    while process.poll() is None:
        text=(root/'game.log').read_text(errors='replace') if (root/'game.log').exists() else ''
        for name in names:
            values=re.findall(re.escape(name)+r'\s*=\s*"?(\d+)',text)
            if values: record['original'].setdefault(name,int(values[0]))
        sessions=list(out.glob('session_*'))
        count=len(list((sessions[0]/'Actual_Frames').glob('*.png'))) if len(sessions)==1 else 0
        if len(record['original'])==len(names) and (count>=128 or time.time()-start>120):
            commands=[name+' '+str(record['original'][name]) for name in names]
            temporary=macro.with_suffix('.next')
            temporary.write_text('\n'.join(commands)+'\n')
            os.replace(temporary,macro)
            record.update(restored=True,restore_written_frames=count,restore_time=time.time(),commands=commands,timeout=count<128)
            path.write_text(json.dumps(record,indent=2))
            return
        path.write_text(json.dumps(record,indent=2))
        time.sleep(.05)
    record.update(process_exit=process.poll(),ended=time.time())
    path.write_text(json.dumps(record,indent=2))
