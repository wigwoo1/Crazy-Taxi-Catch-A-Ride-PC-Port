"""Reachable original palette transfer owner; no runtime palette claim."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native');todo=[0x22508];seen={};outside=set()
while todo:
 off=todo.pop()
 if off in seen:continue
 if not 0x22508<=off<0x2276a:outside.add(off);continue
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
assert seen[0x22652].op_str=='r0, [r4, #0x10]'
assert seen[0x2264e].op_str=='r0, [r4, #2]'
assert seen[0x22640].op_str=='r0, [r4, #7]'
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry':'08022508','reachable_instructions':len(rows),'outgoing_boundaries':[],'instructions':rows,'record_contract':{'table':'02001790','count_halfword':'02001780','stride_bytes':20,'length_halfword':0,'palette_byte_offset_halfword':2,'background_selector_byte':7,'source_pointer_word':16},'transfer_contract':{'dma_registers':'040000D4','background_base':'05000000','object_base':'05000200','control':'84000000 | (length >> 1)','alternate_color_scratch':'030050B0'},'scope':'Complete reachable static transfer CFG and record layout. Source byte7 chooses BG versus OBJ. Alternate mode transforms selected banks; actual normal gameplay bank registration, transfer mode, current contents and final framebuffer colors are not verified.','next_dependency':'Resolve game initialization writes to palette table02001790, including retained table pointersp+24 in normal initializer. Then compare selected native marker color indices with actual registered BG entries; retain original raster relocation prerequisite.'}
(root/'reports/marker-palette-transfer-audit.json').write_text(json.dumps(report,indent=2))
print('Recovered complete palette transfer CFG:',len(rows),'instructions, no outgoing branches;20-byte registration records.')
