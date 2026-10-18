"""Original renderer mutable data owner, not executable callback entry."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for i in md.disasm(r[0x2b6ac:0x2b6b2],0x0802b6ac):allowed_thumb[i.address]=bytes(i.bytes)
words=[int.from_bytes(r[a:a+4],'little') for a in range(0x1dc4,0x1dd4,4)]
assert words==[0,0x03006810,0x0202baec,0x03006960]
assert int.from_bytes(r[0x2cadc:0x2cae0],'little')==0x0202baec
class BlockProbe(ExitProbe):
 def __init__(self,*args):self.block=bytearray(r[0x1dc4:0x1dd4]);super().__init__(*args)
 def read(self,a,size=4):
  if a==0x030069a8:return 0x02009100
  if 0x02009100<=a<0x02009110:return int.from_bytes(self.block[a-0x02009100:a-0x02009100+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x02009100<=a<0x02009110:self.block[a-0x02009100:a-0x02009100+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
steps=0
for frame in [0,0x06000000,0x0600a000,0x02007550,0xffffffff]:
 probe=BlockProbe((0,0,0,0,0,1,0,0,0,0));probe.reg[0]=frame
 prior=bytes(probe.block);probe.run(entry=0x0802b6ac,stop=0x0802b6b2)
 assert probe.block==frame.to_bytes(4,'little')+prior[4:];steps+=probe.steps
report={'status':'SOURCE_EXECUTED','mutable_block_source':'08001DC4','binding_slot':'030069A8','initial_words':[f'{v:08X}' for v in words],'verified_frame_setter':'Actual2B6AC..2B6B0 replaces only block firstword with caller r0; all other12bytes retained. Five frame identities including0/FFFFFFFF, fixture relocated block address.','actual_thumb_instructions':steps,'source_identity_roles':{'word0':'selected framebuffer source identity, setter2B6A0 arg0','word1':'graphics global base03006810','word2':'shade lookup base0202BAEC, ten256-byte banks; original2CA94 fills all2560bytes from material+15','word3':'address of shared global03006960, value/consumer semantics unresolved'},'native_mapping_status':'Existing RestartRenderContext frame owner corresponds to framebuffer semantics; full source block/GPU renderer dispatch wiring not yet integrated. No pointer-valued ROM words may be host dereferenced.','next_dependency':'Map source framebuffer/graphics/shade owners to existing native frame/viewport/shade_lookup; recover shared03006960 owner and full2B6A0 dispatch. Cleared graphics storage pools are distinct from the shade table.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/renderer-data-bindings-source-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS mutable renderer block frame setter:',steps,'actual Thumb instructions; shade lookup distinct from cleared storage')
