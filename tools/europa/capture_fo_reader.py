from __future__ import print_function
import json
import os

REGION=(0,0,780,160)
CONTROLS={
    'C1PIE':'D:/IA_Europa/Receipts/09432/capture-attempts/C1PIE/01/plugin-frames/frame-000030.png',
    'C1Game':'D:/IA_Europa/Receipts/09432/capture-attempts/C1Game/02/plugin-frames/frame-000030.png',
    'C2':'D:/IA_Europa/Receipts/09432/c2-attempts/C2/01/plugin-frames/epoch01/frame-000030.png'}

def green(image):
    return sum(g>=64 and g>1.5*r+20 and g>1.5*b+20 for r,g,b in image.convert('RGB').crop(REGION).getdata())

def check(root,leg):
    from PIL import Image
    control=CONTROLS[leg]
    with Image.open(control) as img:control_pixels=green(img)
    if control_pixels<100:raise ValueError('Heartbeat positive control is not visible')
    rows=[]
    for base,dirs,files in os.walk(str(root)):
        for name in sorted(files):
            if not (name.startswith('frame-') and name.endswith('.png')):continue
            path=os.path.join(base,name)
            with Image.open(path) as img:
                if img.size!=(1280,720):raise ValueError('Heartbeat image extent differs')
                count=green(img)
            rows.append(dict(path=path,heartbeat_pixels=count))
    if not rows:raise ValueError('No heartbeat-check frames')
    failed=[row for row in rows if row['heartbeat_pixels']!=0]
    if failed:raise ValueError('Heartbeat present in %d frames: %s'%(len(failed),json.dumps(failed[:3])))
    return dict(result='PASS',frames=len(rows),region=REGION,control=control,control_heartbeat_pixels=control_pixels,maximum_heartbeat_pixels=0,frames_measured=rows)
