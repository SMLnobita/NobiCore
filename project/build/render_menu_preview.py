from pathlib import Path
import struct,sys,numpy as np
from PIL import Image
HEIGHT=int(sys.argv[1]) if len(sys.argv)>1 else 344
r=Path(__file__).resolve().parent;raw=(r/'menu-font-atlas.bin').read_bytes();tw,th=struct.unpack_from('<ii',raw);tex=np.frombuffer(raw[8:],np.uint8).reshape(th,tw,4)/255
screens=[]
for tab in range(3):
 raw=(r/f'menu-preview-{tab}.bin').read_bytes();off=0;lists=struct.unpack_from('<I',raw,off)[0];off+=4
 pixels=np.empty((HEIGHT,464,3),np.float32);pixels[:]=[.065,.075,.1]
 for _ in range(lists):
  vc,ic,cc=struct.unpack_from('<3I',raw,off);off+=12
  v=np.frombuffer(raw,dtype=np.dtype([('p','<f4',2),('uv','<f4',2),('color','u1',4)]),count=vc,offset=off);off+=20*vc
  indices=np.frombuffer(raw,dtype='<u2',count=ic,offset=off);off+=2*ic
  for cmd in range(cc):
   count,index,base,cx,cy,cz,cw=struct.unpack_from('<III4f',raw,off);off+=28
   for t in range(index,index+count,3):
    ids=indices[t:t+3].astype(np.int64)+base;p=v['p'][ids];uv=v['uv'][ids];color=v['color'][ids]/255
    x0=max(0,int(np.floor(p[:,0].min())),int(np.ceil(cx)));x1=min(464,int(np.ceil(p[:,0].max())),int(np.ceil(cz)))
    y0=max(0,int(np.floor(p[:,1].min())),int(np.ceil(cy)));y1=min(HEIGHT,int(np.ceil(p[:,1].max())),int(np.ceil(cw)))
    if x0>=x1 or y0>=y1:continue
    den=(p[1,1]-p[2,1])*(p[0,0]-p[2,0])+(p[2,0]-p[1,0])*(p[0,1]-p[2,1])
    if abs(den)<1e-8:continue
    yy,xx=np.mgrid[y0:y1,x0:x1];xx=xx+.5;yy=yy+.5
    a=((p[1,1]-p[2,1])*(xx-p[2,0])+(p[2,0]-p[1,0])*(yy-p[2,1]))/den
    b=((p[2,1]-p[0,1])*(xx-p[2,0])+(p[0,0]-p[2,0])*(yy-p[2,1]))/den;c=1-a-b;mask=(a>=-1e-5)&(b>=-1e-5)&(c>=-1e-5)
    u=a*uv[0,0]+b*uv[1,0]+c*uv[2,0];vv=a*uv[0,1]+b*uv[1,1]+c*uv[2,1]
    tx=np.clip((u*tw).astype(int),0,tw-1);ty=np.clip((vv*th).astype(int),0,th-1);sample=tex[ty,tx]
    rgba=a[...,None]*color[0]+b[...,None]*color[1]+c[...,None]*color[2];rgba*=sample;alpha=(rgba[...,3]*mask)[...,None]
    region=pixels[y0:y1,x0:x1];region[:]=rgba[...,:3]*alpha+region*(1-alpha)
 image=Image.fromarray(np.clip(pixels*255,0,255).astype(np.uint8));image.save(r/f'menu-tab-{tab}.png');screens.append(image)
contact=Image.new('RGB',(928,HEIGHT*2));
for i,im in enumerate(screens):contact.paste(im,((i%2)*464,(i//2)*HEIGHT))
contact.save(r/'menu-preview.png');print(r/'menu-preview.png')
