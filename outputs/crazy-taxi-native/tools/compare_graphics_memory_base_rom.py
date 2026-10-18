"""Complete7378 and73C0 graphics address identity setters, all byte modes."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_setup_rom.py').read().split('\nvalues=')[0])
for entry,end in [(0x08007378,0x080073ba),(0x080073c0,0x08007400)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b'):todo.append(i.operands[0].imm)
class BaseProbe(ExitProbe):
 def __init__(self,*args):self.owners={0x0200152c:0x11223344,0x02001530:0x55667788};super().__init__(*args)
 def read(self,a,size=4):
  if a in self.owners:return self.owners[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in self.owners:self.owners[a]=v&MASK
  else:super().write(a,v,size)
 def external(self,t):raise AssertionError(hex(t))
rows=subprocess.run([str(root/'bin/graphics-memory-base-rom-compare.exe')],text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==512;steps=0
for which in range(2):
 for mode in range(256):
  p=BaseProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x02007541,mode,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001
  p.run(entry=[0x08007378,0x080073c0][which],stop=0x10020000)
  assert list(p.owners.values())==list(map(int,rows[which*256+mode].split())),(which,mode,p.owners);steps+=p.steps
report={'status':'PASS','cases':512,'actual_thumb_instructions':steps,'scope':'Complete7378/73C0 both setters across all256byte modes, untouched opposite global retained; identities only, no memory allocator/GPU ownership claim.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/graphics-memory-base-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
