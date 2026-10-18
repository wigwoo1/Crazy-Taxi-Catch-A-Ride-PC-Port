"""Joined background and complete original queue/packet-dispatch/cleanup; raster requests observed."""
from pathlib import Path
import sys,json,struct,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
text=(root/'tools/verify_source240_joined_background.py').read_text().split('rows=[list(map')[0]
# Extend original Thumb execution to the full consumer and capture exact indirect raster request boundaries.
text=text.replace("exec(prefix_source)","""prefix_source=prefix_source.replace('exec(source)', 'source=source.replace(\\\"0x0802b6ac<=pc<0x0802b6f8\\\",\\\"0x0802b6a0<=pc<0x0802b98e\\\");source=source.replace(\\\"   assert 0x08032000<=pc\\\",\\\"   if 0x03006900<=pc<0x03006ac0:\\\\n    pc=self.raster_request(pc);continue\\\\n   assert 0x08032000<=pc\\\");source=source.replace(\\\"[\\\'ldr\\\',\\\'ldrb\\\',\\\'ldrh\\\',\\\'str\\\',\\\'strb\\\',\\\'strh\\\']\\\",\\\"[\\\'ldr\\\',\\\'ldrb\\\',\\\'ldrh\\\',\\\'ldrsh\\\',\\\'str\\\',\\\'strb\\\',\\\'strh\\\']\\\").replace(\\\"m.endswith(\\\'h\\\')\\\",\\\"m.endswith(\\\'h\\\')\\\");source=source.replace(\\\"else:self.reg[rn(o[0])]=self.read(a,size)\\\",\\\"else:\\\\n     value=self.read(a,size);self.reg[rn(o[0])]=(value-65536 if m==\\\'ldrsh\\\' and value&32768 else value)&MASK\\\");exec(source)')
exec(prefix_source)""")
exec(text)
producer={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],producer);producer['steps']=0
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());packets=j['packets'];o=producer['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']]);pool=bytes.fromhex(o['packet'])[:o['cursor']];heads=bytes.fromhex(o['heads'])
class QueueProbe(JoinedBackgroundProbe):
 def __init__(self):
  super().__init__(j['pitch'],j['yaw'],0x087556cc);self.requests=[]
  for a in range(0x03006908,0x03006ac0,4):self.write(a,a)
  self.write(0x030069a8,0x02009100);self.write(0x03006a34,0x08002fe0);self.write(0x03006910,len(pool))
  for i,b in enumerate(heads):self.shared[0x02009cec+i]=b
  for i,b in enumerate(pool):self.shared[0x02009eec+i]=b
 def read(self,a,size=4):
  if 0x02009cec<=a<a+size<=0x02009eec+len(pool):return sum(self.shared.get(a+i,0)<<(8*i) for i in range(size))
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x02009cec<=a<a+size<=0x02009eec+len(pool):
   for i in range(size):self.shared[a+i]=(value>>(8*i))&255
  else:super().write(a,value,size)
 def raster_request(self,callback):
  i=(self.reg[1]-16-0x02009eec)//144;assert 0<=i<len(packets) and self.reg[1]==0x02009eec+144*i+16
  p=packets[i];expected=0x0300691c if p['raster_flags']&1 else 0x030069b0 if p['raster_flags']&2 else int(p['callback'],16)
  assert callback==expected and self.reg[0]==(3 if p['raster_flags']&16 else 4),(i,p,hex(callback),self.reg[:4])
  assert self.reg[2]==struct.unpack_from('<I',pool,i*144+12)[0]
  self.requests.append({'payload':i,'source_index':p['source_index'],'callback':hex(callback),'arguments':self.reg[:4]})
  return self.reg[14]&~1 # Explicit raster-request boundary; functions not executed by this check.
p=QueueProbe();p.run(0x0802b6a0)
expected=json.loads((root/'reports/source240-background-scenery-verification.json').read_text())['consumption_order'];assert [r['payload'] for r in p.requests]==expected
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-real-background-city1.bin').read_bytes()
report={'status':'PASS','packets':len(p.requests),'requests':p.requests,'heads_cleared':128,'packet_byte_cursor_after':p.read(0x03006910),'original_thumb_instructions':p.steps,'original_background_arm_instructions':p.background_steps,'scope':'Joined actualoriginal2B6A0 through2B98C return: framehandoff/numeric/camera/originalbackground call-return, all128bucket clearing, completeThumbpacketdispatch requests37 incl3originalnear-consumer selections, linkorder andbytecursorreset/epilogue. RealROMpacket/material/texture/background memory; callbackglobals bounduniqueidentities. Raster-request return explicit contract, functions not executed here; no fullscenerypixels/actors/palette/cycles/cadence claim.'}
(root/'reports/source240-joined-render-requests-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS requests',len(p.requests),'Thumb',p.steps,'backgroundARM',p.background_steps)
