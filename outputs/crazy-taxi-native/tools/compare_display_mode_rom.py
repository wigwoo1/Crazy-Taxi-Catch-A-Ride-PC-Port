"""Original25C18 mode4 including sprite display helper, all owner bytes."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_setup_rom.py').read().split('\nvalues=')[0])
saved=allowed_thumb.copy()
patched=source.replace("if m=='movs':", "if rn(ops[0])==15:nextpc=value&~1\n    if m=='movs':")
exec(patched);allowed_thumb.update(saved)
# Only mode4 reaches jump table entry4. Include it explicitly as source data.
assert int.from_bytes(r[0x25c54:0x25c58],'little')==0x08025d90
for entry,low,high in [(0x08025c18,0x08025c18,0x08025e34),(0x08028e04,0x08028e04,0x08028e1a),(0x08028e20,0x08028e20,0x08028e32)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert low<=a<=high,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='mov' and i.op_str.startswith('pc,'):todo.append(0x08025d90);continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class DisplayProbe(ExitProbe):
 run=MotionProbe.run
 def __init__(self,*args):self.display=bytearray(32);self.hardware={0x04000000:0x1234,0x0400000c:0,0x0400004c:0x4567};self.sprite=0x89abcdef;super().__init__(*args)
 def read(self,a,size=4):
  if 0x02007540<=a<0x02007560:return int.from_bytes(self.display[a-0x02007540:a-0x02007540+size],'little')
  if a in self.hardware:return self.hardware[a]
  if a==0x02007e38:return self.sprite
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x02007540<=a<0x02007560:self.display[a-0x02007540:a-0x02007540+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif a in self.hardware:self.hardware[a]=v&65535
  elif a==0x02007e38:self.sprite=v&MASK
  else:super().write(a,v,size)
 def external(self,t):
  assert t in [0x08028e04,0x08028e20],hex(t)
  self.run(entry=t,stop=self.reg[14]&~1)
cases=[(mode,flag,seed) for mode in [4,260,0xffffff04] for flag in [0,1,255,256,257] for seed in [0,1,0xa5,0xffff]]
rows=subprocess.run([str(root/'bin/display-mode-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 mode,flag,seed=case;p=DisplayProbe((0,0,0,0,0,1,0,0,0,0));p.display=bytearray((seed+i*17)&255 for i in range(32));p.hardware[0x0400000c]=seed
 p.reg[:2]=[mode,flag];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x08025c18,stop=0x10020000)
 parts=row.split();assert bytes(p.display)==bytes.fromhex(parts[0]),case
 assert list(p.hardware.values())+[p.sprite]==list(map(int,parts[1:])),(case,p.hardware,parts);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete25C18 mode4 branch INCLUDING original28E04/28E20, all32globalbytes retained/written, DISPCNT/BG2CNT/mosaic/sprite request, bytewrapped arguments. Other modes/GPU host presentation excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/display-mode-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
