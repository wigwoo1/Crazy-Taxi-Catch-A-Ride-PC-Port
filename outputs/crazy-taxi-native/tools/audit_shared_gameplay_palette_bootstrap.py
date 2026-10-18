"""Guarded source CFG for shared palette setup leading to customer gameplay."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native');entry=0xabec;stop=0xaf7a;todo=[entry];seen={};edges={}
while todo:
 off=todo.pop()
 if off==stop or off in seen:continue
 assert entry<=off<stop,hex(off)
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i;nexts=[]
 assert not (i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str)),('early return',hex(off))
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  nexts.append(int(i.op_str[1:],16)-0x8000000)
  if i.mnemonic!='b':nexts.append(off+i.size)
 else:nexts.append(off+i.size)
 edges[off]=nexts;todo.extend(nexts)
def reachable(removed):
 todo=[entry];visited=set()
 while todo:
  off=todo.pop()
  if off==removed or off in visited:continue
  if off==stop:return True
  visited.add(off);todo.extend(edges[off])
 return False
assert reachable(None) and not reachable(0xad7a)
assert seen[0xad7a].op_str=='#0x800e154'
assert seen[0xae5a].op_str=='#0x800b6ec'
assert seen[0xaed4].op_str=='#0x800af5c'
assert seen[0xaf76].op_str=='#0x8026440'
rows=[]
for off,i in sorted(seen.items()):
 row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str};h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800==0x4800:
  at=((off+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,at)[0])
 rows.append(row)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry':'0800ABEC','stop':'0800AF7A','reachable_instructions':len(rows),'instructions':rows,'palette_registration_static_dominates_gameplay_entry':True,'findings':['Shared ABEC initializer reaches AF7A customer gameplay frame loop, with B6EC customer reset at AE5A. AD7A scene palette registration statically dominates this frame entry for all intraprocedural CFG paths.','AECE..AED4 tests02001708; zero goes AF5C then falls through AF7A, nonzero takes scene545C setup and joinsAF6C before same loop. Earlier attract-only characterization of AD7A was too restrictive; caller/runtime mode provenance remains unexecuted.','AD68..AD7A loads selected104byte scene palette+24 and E154(r1=32,r2=1). This supports shared scene BG registration source; active bank allocation, frame DMA and later overwrites remain unresolved.'],'scope':'Complete reachable intraprocedural initialization CFG, guarded bytes and dominator check. Calls are opaque: no runtime bootstrap execution, callee bank contents or full frame/palette equivalence. No production color changes justified yet.','next_dependency':'Execute/qualify E154 bank allocation with shared scene palette input, then trace later palette registrations and DMA ownership before changing native marker colors.'}
(root/'reports/shared-gameplay-palette-bootstrap-audit.json').write_text(json.dumps(report,indent=2));print('PASS shared initializer',len(rows),'instructions; AD7A dominates AF7A entry, external calls remain boundaries')
