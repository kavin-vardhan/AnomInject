from __future__ import print_function
import json
import os
import struct

REGION=(450,170,830,590)

def require(value,message):
    if not value:raise ValueError(message)

def metadata(data):
    require(data['requested']==60 and data['written']==60,'Count must be exactly 60')
    rows=data['frames']
    require(len(rows)==60 and sorted(r['index'] for r in rows)==list(range(60)),'Missing or duplicate index')
    require(sorted(r['request'] for r in rows)==list(range(1,61)),'Missing or duplicate request')
    require(data['rhi'] in ('D3D11','D3D12'),'Unknown RHI')
    require(data['backbuffer_format'] in (2,18,37),'Unknown backbuffer format')
    require(isinstance(data['dropped'],int) and data['dropped']>=0,'Dropped count missing')
    require(data['elapsed_seconds']<=90,'Capture deadline exceeded')
    require(data['pitch']==1280 and data['mapped_height']==720,'Mapped layout differs')
    for row in rows:
        require((row['width'],row['height'])==(1280,720),'Native dimensions differ')
        require(row['format'] in (2,18,37) and row['format']==data['backbuffer_format'],'Unknown or inconsistent format')
        require(row['aa']==0,'AA must be off')
        require(row['file']=='frame-%06d.png'%row['index'],'Frame filename differs')
    return rows

def png_header(path):
    with open(path,'rb') as stream:head=stream.read(33)
    require(head[:8]==b'\x89PNG\r\n\x1a\n' and head[12:16]==b'IHDR','Unknown image format')
    width,height,depth,color=struct.unpack('>IIBB',head[16:26])
    require((width,height)==(1280,720) and depth==8 and color in (2,6),'PNG dimensions/encoding differ')

def check(root,reference,image_loader=None):
    if image_loader is None:
        from PIL import Image
        image_loader=lambda p:Image.open(p).convert('RGB')
    output=os.path.join(root,'plugin-frames')
    with open(os.path.join(output,'capture.json')) as stream:data=json.load(stream)
    rows=metadata(data)
    require(sorted(n for n in os.listdir(output) if n.lower().endswith('.png'))==sorted(r['file'] for r in rows),'Unexpected or missing PNG')
    ref=image_loader(reference)
    measurements=[]
    for row in rows:
        path=os.path.join(output,row['file']);png_header(path)
        img=image_loader(path)
        require(img.size==(1280,720),'Decoded dimensions differ')
        extrema=img.getextrema()
        require(max(c[1] for c in extrema)>8,'Black frame')
        a=list(img.crop(REGION).getdata());b=list(ref.crop(REGION).getdata())
        error=sum(abs(x-y) for px,py in zip(a,b) for x,y in zip(px,py))/float(len(a)*3)
        fraction=sum(max(abs(x-y) for x,y in zip(px,py))>24 for px,py in zip(a,b))/float(len(a))
        require(error<=8 and fraction<=0.10,'Fixture region differs from independent Phase A clean reference: %.3f %.3f'%(error,fraction))
        measurements.append(dict(index=row['index'],region_mean_error=error,region_fraction_gt24=fraction,black=False))
    return dict(result='PASS',count=len(rows),indices=list(range(60)),rhi=data['rhi'],backbuffer_format=data['backbuffer_format'],dropped=data['dropped'],unknown_formats=0,
        black_frames=0,region=REGION,reference=reference,frames=measurements,elapsed_seconds=data['elapsed_seconds'])
