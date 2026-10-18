"""Hash-bound palette table owner inventory and explicit source boundaries."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native');refs=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800:continue
 p=((off+4)&~3)+(h&255)*4;v=struct.unpack_from('<I',r,p)[0]
 if v in (0x02001790,0x02001780):refs.append({'site':hex(0x8000000+off),'literal':hex(v)})
def block(lo,hi):
 out=[]
 for i in md.disasm(r[lo:hi],0x8000000+lo):
  out.append({'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str})
 return out
assert r[0xe454:0xe458]==bytes.fromhex('10210180')
assert r[0xe466:0xe46a]==bytes.fromhex('08990160')
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'literal_candidates':refs,'registration_fragment':block(0xe446,0xe482),'reset_fragment':block(0xf5e8,0xf62e),'confirmed_fragments':['0800E446..E468 writes a new record at table+count*20 with length16 and source pointer fromsp+20; full bank attributes and owning function still to recover.','0800F5E8 clears registered palette records and count; it is a reset, not a source registration.','0800AC6C/AE1E retained pointer writes bank5 source08290BAC (table+116); this path is documented attract initialization and cannot be assumed normal gameplay palette registration.','0802B44C copies104byte scene descriptor to030069C0, stores context pointer03006810; palette remains scene descriptor+24, but hardware registration not shown by this helper.'],'scope':'Static candidates plus guarded isolated writes. No source initialization execution, current gameplay palette or marker pixel equivalence.','next_dependency':'Find the complete bank allocation/registration routine and its scene caller; establish bank0 background source/offset/length and later overwrites. Do not substitute the attract palette write for normal gameplay evidence.'}
(root/'reports/palette-registration-owner-audit.json').write_text(json.dumps(report,indent=2))
print('Saved',len(refs),'palette-table reference candidates; guarded registration/reset fragments and corrected attract-path ownership.')
