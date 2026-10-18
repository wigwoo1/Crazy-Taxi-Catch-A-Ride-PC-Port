"""Original pause controller and draw ownership of palette requests."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native');functions=[]
for lo,hi in [(0x21c5c,0x221f8),(0x221f8,0x22508)]:
 todo=[lo];seen={}
 while todo:
  off=todo.pop()
  if off in seen:continue
  assert lo<=off<hi,hex(off)
  i=next(md.disasm(r[off:off+4],off+0x8000000,count=1));seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
   todo.append(int(i.op_str[1:],16)-0x8000000)
   if i.mnemonic=='b':continue
  todo.append(off+i.size)
 rows=[]
 for off,i in sorted(seen.items()):
  row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str};h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:
   at=((off+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,at)[0])
  rows.append(row)
 functions.append({'entry':hex(lo+0x8000000),'instructions':rows,'count':len(rows)})
assert r[0x224f8:0x224fc]==bytes.fromhex('02200870')
assert r[0x21e18:0x21e1c]==bytes.fromhex('0120e1e1')
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'functions':functions,'findings':['221F8 pause draw gates outerstate!=2, palette fade177B==0, signedfreeze5F20!=0, then submits original overlay before224F8 request5F24=2.','21C5C pause controller requests restore5F24=1 via21E16/21E42/21FDC/220D0 paths, commonstore221E0. Remaining controller state/audio/menu/input branch effects require native recovery; do not equate palette dispatch with completed pause.','Pause start21D08..21D1C copies16halfwords08290A44 toOBJbank11 at05000360 and changes registeredrecord11 source (table+236); record11 is excluded by alternate22508, consistent with retaining coloured pause artwork.','Shared B6EC customer reset clears request5F24 atB7F0..B7F2; original26318 consumer clears it after service.'],'scope':'Complete reachable static pause controller/draw CFGs and guarded request writers. No original wholepause execution or hostpause integration; callee side effects not traversed.'}
(root/'reports/pause-palette-request-owners-audit.json').write_text(json.dumps(report,indent=2));print('PASS pause request owner CFGs',[(f['entry'],f['count']) for f in functions])
