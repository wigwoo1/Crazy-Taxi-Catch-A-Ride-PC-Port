from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
s=(root/'tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0]
s=s.replace('def original(case):',"code.update({i.address:i for i in md.disasm(rom[0x2d18:0x2fe0],0x08002d18)})\ndef original(case):")
s=s.replace('pc=0x080031ec','pc=0x08002d18')
anchor=' write(0x11000018,len(refs))'
setup=''' for off,v in [(24,sine[pitch&4095]),(36,cosine[pitch&4095]),(28,sine[yaw&4095]),(40,cosine[yaw&4095])]:write(0x12000000+off,v)
 for i in range(8):
  d=256 if i<4 else 38400;xx=120 if i%4 in (1,2) else -120;yy=80 if i%4>=2 else -80
  for k,v in enumerate([int(xx*d/192),int(yy*d/192),d]):write(0x03006860+i*16+k*4,v)
 sx=read(scene+64);sz=read(scene+68)
 for i,v in enumerate([0,0,sx,sx]):write(0x030068e0+i*4,v)
 for i,v in enumerate([0,sz,0,sz]):write(0x030068f0+i*4,v)
'''
assert anchor in s;s=s.replace(anchor,setup+anchor)
old="    assert ops[0].imm==0x08002ce8;aa=signed(regs['r0']);bb=signed(regs['r1']);regs['r0']=(abs(aa)//abs(bb)*(-1 if (aa<0)!=(bb<0) else 1))&0xffffffff"
new="""    if ops[0].imm==0x080031ec:regs['lr']=pc+4;nxt=ops[0].imm
    else:
     assert ops[0].imm==0x08002ce8;aa=signed(regs['r0']);bb=signed(regs['r1']);regs['r0']=(abs(aa)//abs(bb)*(-1 if (aa<0)!=(bb<0) else 1))&0xffffffff"""
assert old in s;s=s.replace(old,new).replace("'and','ands','eor','orr','orrs'", "'and','ands','eor','orr','orrs'")
exec(s)
steps=0;j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());cells=json.loads((root/'reports/source240-original-cell-loop-verification.json').read_text())['cells'];refs=[r for c in cells for r in c['references']]
o=original([0,refs,*j['camera'],j['yaw'],j['pitch']]);assert o['cursor']==37*144
pool=bytes.fromhex(o['packet']);heads=bytes.fromhex(o['heads']);mb,tb=struct.unpack_from('<II',rom,0x7556cc+28)
for i,p in enumerate(j['packets']):
 flags=struct.unpack_from('<H',pool,144*i+4)[0];mp,tp=struct.unpack_from('<II',pool,144*i+8)
 assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12]==[p['raster_flags'],p['material'],p['texture']],(i,p)
order=[]
for bucket in range(127,-1,-1):
 ptr=struct.unpack_from('<I',heads,bucket*4)[0]
 while ptr:
  off=ptr-0x02009eec;assert off%144==0;i=off//144;assert i not in order and j['packets'][i]['bucket']==bucket;order.append(i);ptr=struct.unpack_from('<I',pool,off)[0]
assert order==json.loads((root/'reports/source240-ordered-scenery-verification.json').read_text())['consumption_order']
report={'status':'PASS','submitted_packets':37,'original_arm_instructions':steps,'consumption_order':order,'scope':'Continuous original2D18 entry to2FDC return withrealROMscene/grid/cellrefs, originalbounds/cellvisibility/31ECnestedproducer andsharedstamps/queue. Actualcapturedcamera, patchedviewport/trig/corners/materialcache0/generation1; signed2CE8 division explicitcontract. Submissionmetadata/order match nativecurrentcapture; notoriginalbootstrap/consumer/fullframe/cycles/cadenceproof.'};(root/'reports/source240-continuous-scene-producer-verification.json').write_text(json.dumps(report,indent=2)+'\n');(root/'reports/source240-continuous-scene-producer-raw.json').write_text(json.dumps(o,indent=2)+'\n');print(report)
