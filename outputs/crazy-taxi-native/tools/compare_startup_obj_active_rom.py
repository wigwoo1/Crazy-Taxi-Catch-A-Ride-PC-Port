"""Whole original27CD0 cold-resource path and logical output publication."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
source=source.replace("   elif m=='pop':",'''   elif m=='stm':
    address=self.reg[rn(ops[0])];registers=sorted(rn(o) for o in ops[1:])
    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])
    self.reg[rn(ops[0])]=(address+4*len(registers))&MASK
   elif m=='pop':''')
exec(source)
todo=[0x8027cd0];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8027cd0<=a<=0x8027f88,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
for name in ['obj-sort','obj-swap']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class ColdProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x80280c4,0x8028e88]:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8032454:
   src,dst=self.reg[:2];assert (src,dst) in [(0x20081ec,0x7000000),(0x2009508,0x20081ec)]
   self.regions[dst][:]=self.regions[src];self.copies.append((src,dst))
  else:raise AssertionError(('unexpected helper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   src,dst,words=self.read(0x40000d4),self.read(0x40000d8),(v&0xffff)
   assert v>>16==0x8400
   data=bytes(self.read(src+i,1) for i in range(words*4))
   for i,b in enumerate(data):self.write(dst+i,b,1)
   self.transfers+=1
cases=[(sum(n),seed,*n,affine) for n in [(0,0,0),(1,0,0),(0,1,0),(0,0,1),(1,2,3),(0,0,128),(64,32,32),(128,0,0)] for seed in [0,165,65535] for affine in [0,1,32]]
rows=subprocess.run([str(root/'bin/startup-obj-active-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (total,seed,n0,n1,n2,affine),row in zip(cases,rows):
 p=ColdProbe((0,0,0,0,0,1,0,0,0,0));p.regions={a:bytearray(n) for a,n in [(0x20081ec,1024),(0x20085ec,1024),(0x20089ec,1024),(0x2008dec,1024),(0x200806c,384),(0x7000000,1024),(0x2009508,1024),(0x2009908,384)]};p.scalars={0x2007e38:0x8082104,0x3004c40:0x10030001,0x2007e2f:total,0x2007e32:n0,0x2007e33:n1,0x2007e34:n2,0x2007e2c:99,0x2007e2d:165,0x2007e30:37,0x2007e31:affine,0x40000d4:0,0x40000d8:0,0x40000dc:0};p.transfers=0;p.copies=[]
 for a,n,k in [(0x2009508,1024,13),(0x2009908,384,19)]:p.regions[a][:]=bytes((seed+i*k)&255 for i in range(n))
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2);p.write(0x7000000+i*8+j*2,seed+i*91+j*17,2)
 for i in range(32):
  p.write(0x200806c+i*12,seed+i)
  for j in range(4):p.write(0x200806c+i*12+4+j*2,seed+i*37+j*53,2)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x8027cd0,stop=0x10020000)
 assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)] and p.copies==[(0x20081ec,0x7000000),(0x2009508,0x20081ec)]
 got=[p.read(a,1) for a in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d,0x2007e30,0x2007e31]];parts=row.split();assert got==list(map(int,parts[:8])),((total,seed),got,parts[:8])
 data=b''.join(p.regions[a] for a in [0x20081ec,0x20085ec,0x20089ec,0x2008dec,0x200806c,0x7000000]);assert data==bytes.fromhex(parts[8]),((total,seed),'owned bytes mismatch');steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'scope':'Whole27CD0 active path plus actual280C4/28E88 sorting. All5504 owned/retained bytes, scalar counters, SP and callee-saved registers. Installed03004C40 copy service remains a logical1024byte copy contract; DMA modeled logical copies, not platform proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_obj_transport.c',root/'src/obj_order.c',root/'tests/startup-obj-active-rom-compare.c',Path(__file__),root/'bin/startup-obj-active-rom-compare.exe']}}
(root/'reports/startup-obj-active-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
