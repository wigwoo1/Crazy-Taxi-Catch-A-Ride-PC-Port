"""Offline original banner callback; never linked into native game."""
from pathlib import Path
import struct,subprocess,json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/original_marker_raster_probe.py').read_text().replace('while pc!=0xffffffff:','while pc!=self.stop:'),ns);ns['BASE']=0x03000008

def execute_banner(owner,rom):
 before=owner.reg.copy();assert before[4]==0x03004300 and before[0]==4 and before[3]==0
 for start,end in ((0x49f8,0x4bac),(0x29fc,0x2ce8),(0x49e4,0x49e8),(0x4418,0x441c),(0x2ce8,0x2cec)):
  address=0x03000008+start-0x700;assert bytes(owner.read(address+i,1) for i in range(end-start))==rom[start:end]
 assert owner.read(0x030042f0,4)==0x030016b8 and owner.read(0x030042f4,4)==0x030016cc
 frame=owner.read(0x030016cc,4);assert frame in (0x06000000,0x0600a000) and frame==owner.read(0x02007550,4);assert [owner.read(0x030016b8+i*4,4) for i in range(4)]==[0,0,240,160]
 rect=[owner.read(before[1]+i,4) for i in (32,44,96,108)];rect=[v-(1<<32) if v>>31 else v for v in rect];w,h,log=[owner.read(before[2]+i,1) for i in range(3)];texture=owner.read(before[2]+4,4);assert 0x08000000<=texture and texture+w*h<=0x08800000 and w*h<=4096
 prior=bytes(owner.read(frame+i,1) for i in range(38400));native=subprocess.run([str(root/'bin/retained-marker-owner-compare.exe')],input=struct.pack('<7i',*rect,w,h,log)+rom[texture-0x08000000:texture-0x08000000+w*h]+prior,capture_output=True);assert native.returncode==0 and len(native.stdout)==38400
 q=ns['MarkerRasterProbe'].__new__(ns['MarkerRasterProbe']);q.reg=before.copy();q.entry=before[4];q.stop=before[14];q.steps=0;q.n,q.z,q.c,q.v=owner.flags;q.blocks=[(0,bytearray()),(frame,bytearray())];q.read=lambda a,n=4:owner.read(a,n);stores=[]
 def write(a,v,n=4):
  assert (before[13]-188<=a and a+n<=before[13]) or (frame<=a and a+n<=frame+38400),('banner destination',hex(a),n,hex(q.reg[15]))
  if frame<=a<frame+38400:assert n==2 and a%2==0 and v&255==v>>8 and v!=0
  stores.append((a,n));owner.drawing_write_base(a,v,n)
 q.write=write;q.run();assert q.reg[4:15]==before[4:15];actual=bytes(owner.read(frame+i,1) for i in range(38400));assert actual==native.stdout
 owner.reg[:]=q.reg;owner.flags=(q.n,q.z,q.c,q.v);owner.steps+=q.steps
 return dict(rectangle=rect,descriptor=[w,h,log,texture],instructions=q.steps,pixel_stores=sum(frame<=a<frame+38400 for a,n in stores),compared_bytes=38400)
