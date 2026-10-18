"""Complete21C10 including original byte-clear1E784, retained slots/tail."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_setup_rom.py').read().split('\nvalues=')[0])
saved=allowed_thumb.copy();exec(source.replace("'bhi':c and not z", "'bhs':c,'blo':not c,'bhi':c and not z"));allowed_thumb.update(saved)
for entry,end in [(0x08021c10,0x08021c26),(0x0801e784,0x0801e7d0)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ResetProbe(ExitProbe):
 run=MotionProbe.run
 def __init__(self,*args):self.pool=bytearray(704);self.ring=0;super().__init__(*args)
 def read(self,a,size=4):
  if 0x02005c60<=a<0x02005f20:return int.from_bytes(self.pool[a-0x02005c60:a-0x02005c60+size],'little')
  if a==0x02005c5c:return self.ring
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x02005c60<=a<0x02005f20:self.pool[a-0x02005c60:a-0x02005c60+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif a==0x02005c5c:self.ring=v&MASK
  else:super().write(a,v,size)
 def external(self,t):
  assert t==0x0801e784,hex(t);self.run(entry=t,stop=self.reg[14]&~1)
seeds=[0,1,8,255,256,65535,0xffffffff]+[random.Random(i).getrandbits(32) for i in range(32)]
rows=subprocess.run([str(root/'bin/effect-pool-reset-rom-compare.exe')],input=''.join(str(s)+'\n' for s in seeds),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(seeds);steps=0
for seed,row in zip(seeds,rows):
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.pool=bytearray((seed+i*37)&255 for i in range(704));before=bytes(p.pool);p.ring=seed;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x08021c10,stop=0x10020000);parts=row.split();assert bytes(p.pool[:696])==bytes.fromhex(parts[0]) and p.ring==int(parts[1]),seed
 assert p.pool[648:]==before[648:],seed;steps+=p.steps
report={'status':'PASS','cases':len(seeds),'actual_thumb_instructions':steps,'scope':'Complete21C10 includingoriginal1E784byte-clear, all648recordbytes/ringzero, all48slotbytes and8adjacenttailbytes retained. No callback/allocator/render/fullframe claim.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/effect-pool-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
