"""Full bounded registration CFGs and background caller; source static only."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native')
def recover(start,end):
 todo=[start];seen={};outside=set()
 while todo:
  off=todo.pop()
  if off in seen:continue
  if not start<=off<end:outside.add(off);continue
  i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
   todo.append(int(i.op_str[1:],16)-0x8000000)
   if i.mnemonic=='b':continue
  todo.append(off+i.size)
 assert not outside
 rows=[]
 for off,i in sorted(seen.items()):
  row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}
  h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:
   at=((off+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,at)[0])
  rows.append(row)
 return {'entry':hex(0x8000000+start),'instructions':rows,'count':len(rows),'outgoing_boundaries':[]}
a=recover(0xe154,0xe388);b=recover(0xe388,0xe5a8)
assert [(i.mnemonic,i.op_str) for i in md.disasm(r[0x25e9a:0x25ea0],0x8025e9a)]==[('adds','r0, r4, #0'),('movs','r1, #0x20'),('movs','r2, #1')]
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'registration256':a,'registration16':b,'source_findings':['E154 registers256 colors (record length256) and source pointer r0; selector lowbyte r2 chooses BG versus OBJ; E388 registers16 colors.','25E8A resets registrations via F5E8. Depth field8 branch25E92 supplies r4 palette pointer to E154 withargument32 andBGselector1 at25EA0.','27BE4..27C5A registers OBJ banks(selector0), so these calls are not a BG palette source.'],'scope':'Complete reachable static registration CFGs and guarded caller arguments. No runtime original execution; inputr4 origin, actual game setup calls and later palette overwrites unverified.','next_dependency':'Recover enclosing25E setup entry and r4 input origin; trace actual scene loader calls to it. Then verify registered256BGcolors against marker embedded colors and implement only source-supported differences.'}
(root/'reports/palette-registration-full-cfg.json').write_text(json.dumps(report,indent=2))
print('Recovered registration CFGs',a['count'],b['count'],'instructions; verified background256color caller25EA0.')
