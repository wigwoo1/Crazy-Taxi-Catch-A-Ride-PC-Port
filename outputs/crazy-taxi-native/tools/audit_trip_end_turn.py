exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0xcd5e:('cmp','r0, #0x80'),0xcd62:('b','#0x800ce60'),0xce6c:('bne','#0x800ce80'),0xce84:('strb','r1, [r0]'),0xcea4:('strb','r0, [r2]'),0xceb4:('strh','r0, [r2, #0xc]'),0xcec2:('str','r1, [r0, #0x1c]'),0xcedc:('bl','#0x802dc20'),0xcef6:('bl','#0x8029b44'),0xcf08:('beq','#0x800cfc4'),0xcfc6:('beq','#0x800d0a2'),0xcfce:('strb','r1, [r0]'),0xcfd4:('cmp','r0, #6'),0xcfea:('cmp','r0, #8'),0xd016:('cmp','r0, #6')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
# Follow source with mode80/submode4 known, bothsuccess branches retained.
forced={0xcd60:0xcd62,0xce6c:0xce80,0xcf08:0xcfc4,0xcfd6:0xcfe8,0xcfec:0xd012,0xd018:0xd0a2}
work=[0xcd38];seen={}
while work:
 off=work.pop()
 if off in seen:continue
 assert 0xcd38<=off<=0xd0a8,hex(off)
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
 if i.mnemonic=='bx':continue
 if off in forced:work.append(forced[off]);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  work.append(int(i.op_str[1:],16)-0x8000000)
  if i.mnemonic=='b':continue
 work.append(off+i.size)
assert {int(i.op_str[1:],16) for i in seen.values() if i.mnemonic=='bl'}=={0x0802dc20,0x08029b44}
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'constraints':'mode80/submode4, bothsuccess branches','finding':'CD38 common scales/direction/active flags/customer exit; challenge skips ordinary result timer/bonus/RNG/voice and trip-time clearing. Nonzero success increments byte1692. Submode4 skips delivery-count6/8 and submode6audio. External retained16A8 untouched; shared07992 success check remains caller-owned.','scope':'Source-local constrainedCFG no unexpected targets; original rotation/height external, native explicit exitXYZ not independentlyGBAqualified.','instructions':[f'{seen[x].address:08x} {seen[x].mnemonic} {seen[x].op_str}' for x in sorted(seen)]}
pathlib.Path('outputs/crazy-taxi-native/reports/trip-end-turn-source.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS original mode80/submode4 trip-end constrainedCFG; no ordinary RNG/bonus/time clearing.')
