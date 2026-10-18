"""Original startup sprite placement prefixes with source resource descriptors."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("self.reg[rn(ops[0])]=value\n    if m=='movs'", "self.reg[rn(ops[0])]=value\n    if rn(ops[0])==15:nextpc=value&~1\n    if m=='movs'")
source=source.replace("   elif m=='pop':", "   elif m=='ldm':\n    a=self.reg[rn(ops[0])];regs=sorted(rn(o) for o in ops[1:])\n    for j,k in enumerate(regs):self.reg[k]=self.read(a+4*j)\n    if ins.writeback:self.reg[rn(ops[0])]=(a+4*len(regs))&MASK\n   elif m=='pop':")
exec(source);allowed_thumb.update(saved_allowed)
for entry,stop,ends in [(0x8028288,0x802839a,[0x8028644]),(0x8028658,0x8028774,[0x8028bbc,0x80286c8])]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a==stop or a in ends or a in seen:continue
  assert entry<=a<stop,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='mov' and i.op_str=='pc, r0':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ObjProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpected helper',hex(t)))
cases=[(frame,scale,x,y) for frame in range(33) for scale in [128,255,256,257,264,512] for x,y in [(120,52),(-128&0xffffffff,111),(48,138),(0,0),(3840,2160),(0x7fffffff,0x80000000)]]
rows=subprocess.run([str(root/'bin/startup-obj-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 frame,scale,x,y=case;p=ObjProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray(struct.pack('<III',x,y,0))};p.scalars={0x2007e2f:0,0x2007e38:0x8082104,0x2007e2e:0};p.reg[:4]=[frame,0,0x10001000,0];p.reg[13]=0x10000400
 for i,value in enumerate([0,0,0,scale,0]):p.write(p.reg[13]+i*4,value)
 affine=scale!=256
 p.run(entry=0x8028658 if affine else 0x8028288,stop=0x8028774 if affine else 0x802839a)
 if affine:got=[p.read(p.reg[13]+12),p.read(p.reg[13]+16),(p.reg[6]*2)&65535,(p.reg[3]*2)&65535,p.read(p.reg[13]+44),1]
 else:got=[p.read(p.reg[13]),p.read(p.reg[13]+4),p.reg[7]*8,p.reg[4]*8,0,0]
 assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual28288 entry..2839A mode0 metric-offset prefix versus28658 entry..28774 centered zero-angle field prefix, all33originalstartup8bpp descriptors, 6positive scales/6normal-offscreen-wrap anchors. Scale256 nativedelegation placement checked againstunscaled sourceprefix; original286A0..286C8 delegation onlystatic source evidence. Doesnotexecute clipping/OAM attributes/buckets/affine matrix allocation/renderer/pixel composition; matrix values useexistingobj_order API, not separately qualified here. Offlineexecutor supportsactualLDM andMOV-PC jumptable; no productioninstruction runtime.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_obj.c',root/'src/startup_obj.h',root/'src/obj_order.c',root/'src/obj_order.h',root/'tests/startup-obj-rom-compare.c',Path(__file__),root/'bin/startup-obj-rom-compare.exe',root/'assets/startup-animation/sprites.crs']}}
(root/'reports/startup-obj-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
