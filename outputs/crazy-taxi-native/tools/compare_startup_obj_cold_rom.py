"""Whole original27CD0 cold-resource path and logical output publication."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
todo=[0x8027cd0];seen=set()
while todo:
 a=todo.pop()
 if a==0x8027e68 or a in seen:continue
 assert 0x8027cd0<=a<=0x8027f88,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ColdProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpected helper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert (self.read(0x40000d4),self.read(0x40000d8),v)==(0x20081ec,0x7000000,0x84000100)
   self.regions[0x7000000][:]=self.regions[0x20081ec];self.transfers+=1
cases=[(total,seed) for total in [0,1,31,127,128,255] for seed in [0,165,65535,123456789]]
rows=subprocess.run([str(root/'bin/startup-obj-cold-rom-compare.exe')],input=''.join(f'{total} {seed}\n' for total,seed in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (total,seed),row in zip(cases,rows):
 p=ColdProbe((0,0,0,0,0,1,0,0,0,0));p.regions={a:bytearray(n) for a,n in [(0x20081ec,1024),(0x20085ec,1024),(0x20089ec,1024),(0x2008dec,1024),(0x200806c,384),(0x7000000,1024)]};p.scalars={0x2007e38:0,0x2007e2f:total,0x2007e32:17,0x2007e33:33,0x2007e34:65,0x2007e2c:99,0x2007e2d:165,0x2007e30:37,0x2007e31:29,0x40000d4:0,0x40000d8:0,0x40000dc:0};p.transfers=0
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2);p.write(0x7000000+i*8+j*2,seed+i*91+j*17,2)
 for i in range(32):
  p.write(0x200806c+i*12,seed+i)
  for j in range(4):p.write(0x200806c+i*12+4+j*2,seed+i*37+j*53,2)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x8027cd0,stop=0x10020000)
 assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)] and p.transfers==1
 got=[p.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d,0x2007e30,0x2007e31]];parts=row.split();assert got==list(map(int,parts[:8])),((total,seed),got,parts[:8])
 data=b''.join(p.regions[a] for a in [0x20081ec,0x20085ec,0x20089ec,0x2008dec,0x200806c,0x7000000]);assert data==bytes.fromhex(parts[8]),((total,seed),'owned bytes mismatch');steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole27CD0 cold path withsprite resource7E38null, actual128disabledbase rows/32matrixslots/counter resets and1024byte output publication; ALL5504owned/retained bytes plus8scalar bytes andSP/callee-savedregs. DMA register requestchecked andmodeledas logical memorycopy, notphysicalWindows/GBA DMA. Non-null resource branches27E68 merge path andmustnotcallthiscoldclear; active lifecycle remainsunqualified here.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_obj_transport.c',root/'src/startup_obj_transport.h',root/'tests/startup-obj-cold-rom-compare.c',Path(__file__),root/'bin/startup-obj-cold-rom-compare.exe']}}
(root/'reports/startup-obj-cold-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
