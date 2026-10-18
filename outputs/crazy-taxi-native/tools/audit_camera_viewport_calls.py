"""Bounded Thumb direct-call candidates; not full indirect/runtime ownership."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
rows=[]
for off in range(0x700,0x34000,2):
 h,k=struct.unpack_from('<HH',r,off)
 if h&0xf800!=0xf000 or k&0xf800!=0xf800:continue
 d=((h&0x7ff)<<12)|((k&0x7ff)<<1)
 if d&0x400000:d-=0x800000
 if off+4+d!=0x2c790:continue
 ins=list(md.disasm(r[off-8:off+4],0x8000000+off-8))
 rows.append({'call':hex(off+0x8000000),'preceding8bytes':r[off-8:off].hex(),'instructions':[(i.mnemonic,i.op_str) for i in ins]})
assert len(rows)==9
assert [(i.mnemonic,i.op_str) for i in md.disasm(r[0x2c7e2:0x2c802],0x802c7e2)]==[('adds','r3, r0, r2'),('lsrs','r4, r3, #0x1f'),('adds','r3, r3, r4'),('asrs','r3, r3, #1'),('str','r3, [r5, #0x18]'),('adds','r3, r1, r6'),('lsrs','r4, r3, #0x1f'),('adds','r3, r3, r4'),('asrs','r3, r3, #1'),('str','r3, [r5, #0x1c]'),('subs','r2, r2, r0'),('str','r2, [r5, #0x20]'),('subs','r6, r6, r1'),('str','r6, [r5, #0x24]'),('movs','r0, #0xc0'),('str','r0, [r5, #0x28]')]
result={'rom_sha256':hashlib.sha256(r).hexdigest(),'direct_call_candidates':rows,'viewport_rule':'centres signed wrapped endpoint sums divided2 towards zero; focal192; explicit bounds remain caller-owned','scope':'Static bounded Thumb direct BL scan700..34000 and exact setter opcodes. Indirect/ARM/later writes and whole GBA execution not qualified.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-viewport-call-audit.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS original viewport centre/focal opcodes and9bounded direct call candidates; no running GBA framing claim.')
