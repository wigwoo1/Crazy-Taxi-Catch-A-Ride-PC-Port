"""ROM guarded exact submode3 rank branch and bounded reachable source."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0x9fe6:('cmp','r1, #3'),0x9fe8:('beq','#0x8009fec'),0xa024:('ldrsh','r1, [r2, r0]'),0xa028:('lsrs','r0, r0, #0xf'),0xa02c:('bgt','#0x800a030'),0xa070:('bl','#0x801e7f0'),0xa0f2:('adds','r1, #0x6d'),0xa0f6:('movs','r2, #0x80'),0xa0fc:('strb','r0, [r1]'),0xa144:('ldrsh','r1, [r3, r5]'),0xa146:('b','#0x800a92c'),0xa92c:('lsls','r1, r1, #0xf'),0xa932:('ands','r0, r2'),0xa934:('orrs','r0, r1'),0xa936:('str','r0, [r4, #0x6c]')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert (i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0x9fee,0x02001eb0),(0x9ff2,0x02001538),(0xa020,0x0200164c),(0xa046,0x02001f1c),(0xa140,0x0200164c),(0xa930,0x7fff)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
work=[0x9fec];seen={};calls=set();outside=set()
while work:
 off=work.pop()
 if off in seen:continue
 if not (0x9fec<=off<0xa172 or 0xa92c<=off<0xa93a or 0xa96a<=off<0xa97c):outside.add(off);continue
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='bl':
  target=int(i.op_str[1:],16);calls.add(target)
  if target==0x0800a96a:work.append(0xa96a);continue
 if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
 work.append(off+i.size)
assert not outside,outside
lines=[f'{seen[x].address:08x}: {seen[x].mnemonic} {seen[x].op_str}' for x in sorted(seen)]
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-score-source.txt').write_text('\n'.join(lines)+'\nCALLS:'+','.join(hex(x) for x in sorted(calls))+'\nOUTSIDE:none\n')
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'source_branch':'08009FEC..0800A170 plus0800A92C and0800A96A return','record_group':144,'profile_stride':208,'rank_rule':'signed164C count strictlygreater than prior packedunsignedword>>15; invalidrecord accepts evennegative fixture; three descendingrecords, ties retained','packing':'validbit0; priorbits1..4 retained; driverbits5..6; profilebit7; fractionbits8..14 cleared; signed count shifted15 modulo32bit','scope':'Exact local opcodes/literals plus bounded reachable source; copy1E7F0 external dependency; native controlled tests are not earned Drift or independent full GBA fidelity.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-score-source.json').write_text(json.dumps(report,indent=2))
print('PASS original submode3 strict higher combo rank/group144/352/metadata; bounded branch has no unexpected targets.')
