"""Actual original Thumb packet dispatch to raster callback, isolated branch diagnostic."""
from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm import ARM_OP_IMM
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],ns);rom=ns['rom'];ns['steps']=0
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());o=ns['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']]);pool=bytes.fromhex(o['packet'])[:o['cursor']]
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
# Decode individual halfword sites, including branch-over embedded literals.
code={}
for off in range(0x2b720,0x2b964,2):
 ins=next(md.disasm(rom[off:off+4],0x8000000+off),None)
 if ins:code[ins.address]=ins
def word(off):return struct.unpack_from('<I',rom,off)[0]
results=[];steps=0
for index,p in enumerate(j['packets']):
 if p['raster_flags']&3:continue
 regs={f'r{i}':0 for i in range(13)};regs.update(sb=128,sl=0,fp=0,ip=0,sp=0x10001000,lr=0,r5=0x02009eec+144*index,r6=word(0x2b778),r8=word(0x2b77c))
 mem={a:a for a in range(0x03006800,0x03006b00,4)};pc=0x0802b720;zero=False;n=0
 def read(a,size=4):
  if 0x08000000<=a and a+size<=0x08800000:return int.from_bytes(rom[a-0x08000000:a-0x08000000+size],'little')
  if 0x02009eec<=a and a+size<=0x02009eec+len(pool):return int.from_bytes(pool[a-0x02009eec:a-0x02009eec+size],'little')
  return mem[a]
 def val(op):return op.imm&0xffffffff if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
 while pc not in (0x0802b94c,0x0802b960):
  ins=code[pc];ops=ins.operands;mn=ins.mnemonic;n+=1;steps+=1;assert n<100;nxt=pc+ins.size
  if mn in ('mov','movs'):regs[md.reg_name(ops[0].reg)]=val(ops[1])
  elif mn in ('adds','lsls'):
   a=val(ops[0] if len(ops)==2 else ops[1]);b=val(ops[1] if len(ops)==2 else ops[2]);regs[md.reg_name(ops[0].reg)]=(a+b if mn=='adds' else a<<b)&0xffffffff
  elif mn=='ands':regs[md.reg_name(ops[0].reg)]&=val(ops[1])
  elif mn in ('ldr','ldrb','ldrh','ldrsh','str'):
   m=ops[1].mem;base=md.reg_name(m.base);a=(((pc+4)&~3) if base=='pc' else regs[base])+m.disp+(regs[md.reg_name(m.index)] if m.index else 0)
   if mn=='str':mem[a]=val(ops[0])
   else:
    size=1 if mn=='ldrb' else 2 if mn in ('ldrh','ldrsh') else 4;v=read(a,size)
    if mn=='ldrsh' and v&32768:v-=65536
    regs[md.reg_name(ops[0].reg)]=v&0xffffffff
  elif mn=='cmp':zero=val(ops[0])==val(ops[1])
  elif mn in ('b','beq','bne'):
   if mn=='b' or (mn=='beq' and zero) or (mn=='bne' and not zero):nxt=ops[0].imm
  else:raise AssertionError((hex(pc),mn,ins.op_str))
  pc=nxt
 assert regs['r4']==int(p['callback'],16),(index,p['source_index'],hex(regs['r4']),p['callback'])
 assert regs['r1']==0x02009eec+144*index+16 and regs['r0']==(3 if p['raster_flags']&16 else 4)
 results.append({'source_index':p['source_index'],'kind':p['kind'],'callback':hex(regs['r4']),'instructions':n})
report={'status':'PASS','packets':results,'original_thumb_instructions':steps,'scope':'Actual2B720 packetprocessing through3245C call boundary at capturedsource240camera, real original packet/material/texture descriptor memory; callbackglobals bound to unique address identities so selectedglobal matches nativecapturedcallback. Count/vertexpointer exact. Near/farconsumer packets excluded; callbackfunctions not executed here, callback scratch/global uniforms not fully qualified; no fullframe/cycle proof.'}
(root/'reports/source240-original-packet-dispatch-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS packets',len(results),'instructions',steps)
