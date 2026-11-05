import datetime,json,os,shutil
from pathlib import Path

def snapshot():
    local=Path(os.environ['LOCALAPPDATA'])
    roots=[local/'D3DSCache',local/'NVIDIA/DXCache',local/'NVIDIA/GLCache',local/'UnrealEngine/Common/DerivedDataCache',local/'UnrealEngine/4.25/Saved',local/'StylizedParisStreet/Saved']
    rows=[]
    for root in roots:
        count=size=0;errors=0
        if root.exists():
            for path in root.rglob('*'):
                try:
                    if path.is_file():count+=1;size+=path.stat().st_size
                except OSError:errors+=1
        rows.append(dict(path=str(root),files=count,bytes=size,errors=errors))
    return dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),free_bytes=shutil.disk_usage('C:/').free,caches=rows)

def finish(before,after):
    delta=before['free_bytes']-after['free_bytes'];old={r['path']:r for r in before['caches']}
    changes=[dict(path=r['path'],delta_bytes=r['bytes']-old[r['path']]['bytes'],errors=r['errors']) for r in after['caches']]
    return dict(before=before,after=after,drop_bytes=delta,drop_GiB=delta/2**30,attribution_required=delta>=2**30,cache_changes=changes,attribution='Measured cache size deltas; other disk writers not established',floor_pass=after['free_bytes']>=10*2**30)
