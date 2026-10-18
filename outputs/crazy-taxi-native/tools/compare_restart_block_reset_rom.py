"""DF8C reset including DF9C(-1) and whole original33F2C fill."""
exec(open('outputs/crazy-taxi-native/tools/compare_restart_shade_full_rom.py').read().split('\ncases=')[0])
todo=[0x0800df8c,0x0800df9c];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x0800df8c<=a<=0x0800e01a,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes);decoded[a]=i
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 # DFA6 is never taken for wrapper's proven -1 argument; nonreset callee paths excluded.
 if i.mnemonic.startswith('b') and i.mnemonic!='bl' and a!=0x0800dfa6:todo.append(i.operands[0].imm)
class BlockProbe(FullShadeProbe):
 def __init__(self,*args):self.block=bytearray(528);self.reset_words={0x02001768:0,0x0200176c:0};super().__init__(*args)
 def read(self,a,size=4):
  if 0x0202b8ec<=a<0x0202bafc:return int.from_bytes(self.block[a-0x0202b8ec:a-0x0202b8ec+size],'little')
  if a in self.reset_words:return self.reset_words[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x0202b8ec<=a<0x0202bafc:self.block[a-0x0202b8ec:a-0x0202b8ec+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif a in self.reset_words:self.reset_words[a]=v&MASK
  else:super().write(a,v,size)
 def external(self,t):
  assert t in [0x0800df9c,0x08033f2c],hex(t);self.run(entry=t,stop=self.reg[14]&~1)
seeds=[0,1,0xa5,255,256,65535,0xffffffff]+[random.Random(i).getrandbits(32) for i in range(32)]
rows=subprocess.run([str(root/'bin/restart-block-reset-rom-compare.exe')],input=''.join(str(s)+'\n' for s in seeds),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(seeds);steps=0
for seed,row in zip(seeds,rows):
 p=BlockProbe((0,0,0,0,0,1,0,0,0,0));p.block=bytearray((seed+i*37)&255 for i in range(528));before=bytes(p.block);p.reset_words={0x02001768:seed,0x0200176c:seed^0xabcdef01};p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x0800df8c,stop=0x10020000);parts=row.split();assert [signed(p.reset_words[0x02001768]),p.reset_words[0x0200176c]]==list(map(int,parts[:2])) and bytes(p.block[:512])==bytes.fromhex(parts[2]),seed
 assert bytes(p.block[512:])==before[512:],seed;steps+=p.steps
report={'status':'PASS','cases':len(seeds),'actual_thumb_instructions':steps,'scope':'CompleteDF8C throughDF9C(-1) INCLUDINGwhole33F2C fill, twoownerwords/all512blockbytes and16adjacentbytes retention. DF9C nonreset arguments/consumer semantics excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/restart-block-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
