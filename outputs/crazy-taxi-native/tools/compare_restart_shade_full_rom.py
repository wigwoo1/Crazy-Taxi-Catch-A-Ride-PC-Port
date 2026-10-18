"""Complete original2B0B0 shade builder with complete33F2C fill helper."""
exec(open('outputs/crazy-taxi-native/tools/compare_restart_shade_gate_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True;decoded={}
for start,end in [(0x802b0b0,0x802b320),(0x8033f2c,0x8033f7e)]:
 todo=[start];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert start<=a<end,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes);decoded[a]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':
   todo.append(int(i.op_str.lstrip('#'),16))
   if i.mnemonic!='b':todo.append(a+i.size)
  else:todo.append(a+i.size)
source=source.replace('self.steps<50000','self.steps<50000000').replace('ins=next(md.disasm(raw,pc))','ins=decoded[pc]')
source=source.replace("   elif m=='pop':", """   elif m=='stm':
    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])
    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])
    self.reg[base]=(address+4*len(registers))&MASK
   elif m=='pop':""").replace('    self.reg[13]+=4*len(regs)','    self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1')
saved=allowed_thumb.copy();exec(source);allowed_thumb.update(saved);GateProbe.run=MotionProbe.run
class FullShadeProbe(GateProbe):
 def external(self,target):
  assert target==0x8033f2c,hex(target);self.run(entry=target,stop=self.reg[14]&~1)
palette=[int.from_bytes(r[0x4a5a34+2*i:0x4a5a36+2*i],'little') for i in range(256)]
cases=[(38400,0,palette),(1,0x00f87820,[i&31 for i in range(256)])]
cp=subprocess.run([str(root/'bin/restart-shade-full-rom-compare.exe')],input=''.join(' '.join(map(str,[depth,color,*p]))+'\n' for depth,color,p in cases),text=True,capture_output=True,check=True);rows=cp.stdout.splitlines();assert len(rows)==len(cases);steps=0
for (depth,color,palette),row in zip(cases,rows):
 p=FullShadeProbe((0,0,0,0,0,1,0,0,0,0));p.present=1;p.reg[:2]=[depth,color];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(0x2009018,0x2009100)
 for j,v in enumerate(palette):p.write(0x2009100+2*j,v,2)
 for j in range(2560):p.write(0x202baec+j,165,1)
 p.run(entry=0x802b0b0,stop=0x10020000)
 a,b,lookup=row.split();actual=bytes(p.read(0x202baec+j,1) for j in range(2560));want=bytes.fromhex(lookup)
 assert actual==want,{'depth':depth,'color':color,'differences':[(i,a,b) for i,(a,b) in enumerate(zip(actual,want)) if a!=b][:16]}
 assert [p.read(0x3006854),p.read(0x3006858)]==[int(a),int(b)];steps+=p.steps;print('Verified complete shade case',depth,color,p.steps,flush=True)
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original2B0B0 nonzero/present builder and full33F2C fill callee, all2560lookupbytes/globaldepth/color exact. Actualcitypalette blacktarget and tiedredramp distinctpackedcolor. Decodedinstruction cache retains exactROMopcode whitelist; explicit engine/palettepointer fixtures. NoGPU/wholebootstrap integration.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/shade_table.c','src/restart_render_context.c','tests/restart_shade_full_rom_compare.c','tools/compare_restart_shade_full_rom.py']})
(root/'reports/restart-shade-full-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
