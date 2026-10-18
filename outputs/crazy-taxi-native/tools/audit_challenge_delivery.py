"""Source-local challenge acceptance and AFTER-handler outcome, services opaque."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0x78d6:('movs','r6, #1'),0x7912:('cmp','r0, #2'),0x7914:('bls','#0x80079aa'),0x7944:('ldrsh','r0, [r2, r3]'),0x7946:('bl','#0x800e0f4'),0x7952:('cmp','r0, #0'),0x7970:('movs','r0, #0x78'),0x7974:('bl','#0x800df8c'),0x7978:('bl','#0x802939c'),0x797e:('bl','#0x800cd38'),0x7984:('subs','r0, r1, #4'),0x798a:('cmp','r0, #1'),0x798e:('cmp','r1, #7'),0x7996:('cmp','r0, #0'),0x799c:('strb','r6, [r2]'),0x799e:('strb','r6, [r5]'),0x79a2:('bl','#0x8009ab0')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0x790e,0x02001537),(0x7916,0x02001697),(0x7920,0x02005f20),(0x792a,0x02001768),(0x7940,0x02003494),(0x794e,0x02002b78),(0x7956,0x02001694),(0x7960,0x02001700),(0x7968,0x02001704),(0x7992,0x020016a8),(0x799a,0x0200153c),(0x79a6,0x0200153d)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
work=[0x790e];seen={}
while work:
 off=work.pop()
 if off==0x79aa or off in seen:continue
 assert 0x790e<=off<0x79aa,hex(off)
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  work.append(int(i.op_str[1:],16)-0x8000000)
  if i.mnemonic=='b':continue
 work.append(off+i.size)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'section':'0800790E..080079AA','finding':'Submode>2/unended/unpaused/selected destination/signed material accepted/stopped/active/no1700/no1704 =>1700=120 thenDF8C,2939C,CD38(1). AFTER those services, submode4/5/7 with retained16A8!=0 sets153C/1697=1 then9AB0(0)->153D. No1708 gate.','scope':'Bounded localCFG no unexpected targets; external services and E034 distance update opaque. Not integrated playable Turn or independent GBA behavioral oracle.','instructions':[f'{seen[x].address:08x} {seen[x].mnemonic} {seen[x].op_str}' for x in sorted(seen)]}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-delivery-source.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS SHA/opcode/literal guarded challenge delivery owner; localCFG bounded, handlers opaque.')
