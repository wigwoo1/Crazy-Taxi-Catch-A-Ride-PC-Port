"""Original scene/bitmap registration callsite ownership; static evidence."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native')
def rows(lo,hi):
 out=[]
 for i in md.disasm(r[lo:hi],0x8000000+lo):
  row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,p)[0])
  out.append(row)
 return out
challenge=rows(0x7484,0x7494);attract=rows(0xad68,0xad7e);bitmap=rows(0x25e38,0x25e66)
assert challenge[3]['operands']=='r0, [r0, #0x18]'
assert challenge[-1]['operands']=='#0x800e154'
assert bitmap[12]['operands']=='r4, r0, #0'
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'challenge_scene_registration':challenge,'attract_scene_registration':attract,'bitmap_header_prefix':bitmap,'findings':['Challenge-session07484..7490 reads scene pointer from03004C9C owner+16, then scene+24 palette and callsE154(argument32,BG1).','AttractAD68..AD7A uses selectedscene*104 from owner+12, then palette+24, argument32/BG1. This is attract, not verified normal initializer.','25E38 takes bitmap pointerr0; r4 initiallyr0 then advances8bytes through fourheaderhalfwords; palette registration25EA0 therefore uses embedded bitmap palette atinput+8. Sole direct call16298 is asset/display dispatch and must not be substituted for game-scene initialization.'],'scene_palettes':[{'scene':name,'descriptor':hex(off+0x8000000),'palette':hex(struct.unpack_from('<I',r,off+24)[0])} for name,off in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]],'scope':'Static guarded source handoffs only. Normal gameplay registration origin, bank offsets and later overwrites, current palette contents and marker framebuffer pixel output unverified.','next_dependency':'Trace normal game scene/display entry and palette bank allocations/overwrites; compare actual registered BG colors, then original patched ARM raster output.'}
(root/'reports/scene-palette-callers-audit.json').write_text(json.dumps(report,indent=2))
print('Verified challenge scene and attract scene palette handoffs; bitmap palette is input+8, distinct path.')
