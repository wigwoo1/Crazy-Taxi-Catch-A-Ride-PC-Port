"""SHA-guarded Crazy Drift typed-callback recovery; external BL are dependencies."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
assert struct.unpack_from('<6I',r,0x4a4fc4)==(0x0800aa65,0x080087e5,0x08008925,0x08070e50,0x087ebcc8,150)
for off,want in [(0xaa6a,0x0200164e),(0xaa74,0x0200164c),(0xaa7a,0x02001650),(0xaa7e,0x02001654),(0xaa84,0x02001658),(0xaa88,0x02005f2c),(0x8836,0x0200164e),(0x8858,0x0200164c)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==want
checks={0xaa6c:('movs','r2, #0xe1'),0xaa6e:('lsls','r2, r2, #3'),0x885e:('cmp','r0, #0xe'),0x8860:('ble','#0x800889c'),0x8868:('bl','#0x8009ab0'),0x88b6:('cmp','r7, #9'),0x88ba:('cmp','r0, #9')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off));assert(i.mnemonic,i.op_str)==want
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-source-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'callback_table_row':'084A4FC4','initializer':'0800AA64','controller':'080087E4','hud':'08008924','world':'087EBCC8','rules':'1800tick timer; signedcount164C>14 onlyat timeout; success/rank request9AB0; warning at seconds>9 to<=9 with optionalSFX39/sourcepalette08290740. Phase1 submits objects. Nativeorientation counter owner alreadyrecovered; earneddriving/course/menu/HUD/result/save notintegrated.','scope':'Exact local opcodes/literals and boundedreachableCFG; externalBL opaque, not fullGBAexecution.'},indent=2)+'\n')
lines=[]
for name,start,end in [('init',0xaa64,0xaaac),('frame',0x87e4,0x8924),('hud',0x8924,0x8b0c)]:
 work=[start];seen={};calls=set();outside=set()
 while work:
  off=work.pop()
  if off in seen:continue
  if not start<=off<end:outside.add(off);continue
  decoded=list(md.disasm(r[off:off+4],0x8000000+off,count=1));assert decoded
  i=decoded[0];seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
  if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
  work.append(off+i.size)
 lines.append(f'ROM SHA verified; Crazy Drift {name}; external BL not executed/traversed.')
 for off,i in sorted(seen.items()):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}';h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:
   p=((off+4)&~3)+(h&255)*4;row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
 lines+=['CALLS:'+','.join(hex(x) for x in sorted(calls)),'OUTSIDE:'+','.join(hex(x) for x in sorted(outside))]
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-callback-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
