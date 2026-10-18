"""Static source guards for recovered post-field OBJ origin modes."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0x28774:0x02007e2e,0x2877e:0x08028790,0x287a4:0x020094f0,0x287ac:0x020094f4,0x287bc:0x020094f8,0x287d8:0x02003b08}
for off,want in checks.items():
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==want
assert list(struct.unpack_from('<5I',r,0x28790))==[0x080287ec,0x080287a4,0x080287ac,0x080287bc,0x080287d0]
want=[('mov','r6, sb'),('ldr','r0, [r6]'),('cmp','r0, #0xff'),('bhi','#0x80287ec'),('ldr','r1, [pc, #0x64]'),('asrs','r0, r0, #1'),('lsls','r0, r0, #4'),('adds','r1, #4'),('adds','r0, r0, r1'),('ldr','r1, [r0]'),('asrs','r1, r1, #8'),('ldr','r0, [r6, #4]'),('adds','r0, r0, r1'),('str','r0, [r6, #4]')]
assert [(i.mnemonic,i.op_str) for i in md.disasm(r[0x287d0:0x287ec],0x80287d0)]==want
for off in [0x287b0,0x287c0]:
 i=next(md.disasm(r[off:off+2],0x8000000+off));assert(i.mnemonic,i.op_str)==('asrs','r1, r1, #0xc')
refs=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 pool=((off+4)&~3)+(h&255)*4
 if struct.unpack_from('<I',r,pool)[0]==0x02007e2e:
  refs.append({'address':hex(0x8000000+off),'following':[(i.mnemonic,i.op_str) for i in md.disasm(r[off:off+10],0x8000000+off)]})
result={'rom_sha256':hashlib.sha256(r).hexdigest(),'literal_bindings':{hex(k+0x8000000):hex(v) for k,v in checks.items()},'mode4_instructions':want,'bounded_literal_references':refs,'finding':'Original28658 applies mode02007E2E after centered field calculation. Modes1/3 add ASR12 fixed X offset94F0/94F8; mode2 adds ASR12 Y offset94F4. Mode4 unsigned fieldX<=255 selects16-byte source row(X>>1)+4 and addsASR8 to Y. Mode0 and>4 bypass. Native helper has explicit128-row host buffer bound and invalid atomicity.','limits':'Direct literal candidate scan is not complete writer analysis or live gameplay mode proof. Existing playable taxi currently assumes mode0; helper deliberately not connected before owner qualification.'}
pathlib.Path('outputs/crazy-taxi-native/reports/obj-screen-origin-source.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS original OBJ origin dispatch/literals/unsigned row rule/signed shifts; live mode owner remains unqualified.')
