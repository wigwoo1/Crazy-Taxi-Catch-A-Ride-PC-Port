"""Complete static layer/asset initializer and256-color palette provenance."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native');todo=[0x5778];seen={};outside=set()
while todo:
 off=todo.pop()
 if off in seen:continue
 if not 0x5778<=off<0x5ec0:outside.add(off);continue
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
  p=((off+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,p)[0])
 rows.append(row)
assert seen[0x581a].op_str=='r6, r1, #0' and seen[0x581c].op_str=='r6, #0x10'
assert seen[0x57f8].op_str=='r1, r3, #2' and seen[0x5800].op_str=='r7, r1, #0'
assert seen[0x5914].op_str=='r0, r6, r7' and seen[0x5916].op_str=='r0, [r0]'
assert seen[0x591c].op_str=='#0x800e154'
callers=[]
for off in range(0,0x34000,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 d=((a&2047)<<12)|((b&2047)<<1)
 if d&0x400000:d-=0x800000
 if off+4+d==0x5778:callers.append(hex(0x8000000+off))
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry':'08005778','reachable_instructions':len(rows),'outgoing_boundaries':[],'instructions':rows,'direct_call_candidates':callers,'palette_pointer_formula':'*(input_descriptor +16 +4*layer_index)','registration_gate':'layer record halfword=512 at58DE..58E4, first eligible palette only (sp+16 gate)','registration_args':{'r0':'selected palette pointer','r1':0,'r2':1},'scope':'Complete reachable static initializer CFG; source pointer formula and guarded registration call. This is a layer/asset descriptor, not104byte scene descriptor. Actual normal gameplay descriptor inputs and registered palette contents not executed.','next_dependency':'Trace actual normal startup descriptor passed to5778 (caller contexts including162C6 asset dispatch); distinguish UI/layer palettes from software-rendered scene BG palette. Continue original ARM raster verification without inventing an active palette.'}
(root/'reports/background-asset-initializer-full-cfg.json').write_text(json.dumps(report,indent=2))
print('Recovered',len(rows),'reachable initializer instructions;',len(callers),'call candidates; selected palette is descriptor+16+layer*4.')
