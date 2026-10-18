"""OriginalFBAC and nested28E3C against native font0 measurement."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
for entry,end in [(0x800fbac,0x800fcf6),(0x8028e3c,0x8028e86)]:
 pending=[entry];seen=set()
 while pending:
  a=pending.pop()
  if a in seen:continue
  assert entry<=a<end,hex(a);seen.add(a);off=a-0x8000000;i=next(md.disasm(r[off:off+4],a,count=1));allowed_thumb[a]=i.bytes
  if i.mnemonic=='bl':pending.append(a+i.size)
  elif i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  elif i.mnemonic.startswith('b') and i.mnemonic!='bics':
   pending.append(int(i.op_str.lstrip('#'),16))
   if i.mnemonic!='b':pending.append(a+i.size)
  else:pending.append(a+i.size)
class MeasureProbe(UpdateProbe):
 def external(self,target):
  assert target==0x8028e3c,hex(target);self.run(entry=target,stop=self.reg[14]&~1)
saved=allowed_thumb.copy();source=source.replace("'bne':not z,","'bne':not z,'bhs':c,");source=source.replace("   elif m in ['mov','movs']:","   elif m=='ldm':\n    base=rn(ops[0]);regs=sorted(rn(o) for o in ops[1:]);address=self.reg[base]\n    assert base not in regs and ins.writeback\n    for j,k in enumerate(regs):self.reg[k]=self.read(address+4*j)\n    self.reg[base]=(address+4*len(regs))&MASK\n   elif m in ['mov','movs']:");exec(source);allowed_thumb.update(saved);MeasureProbe.run=MotionProbe.run
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-requests.txt').read_text().splitlines()];texts=set()
for row in rows:
 tips=row[578:1138]
 for n in range(10):
  tip=tips[n*56:(n+1)*56]
  if tip[2]:
   whole=int.from_bytes(bytes(tip[4:8]),'little',signed=True);frac=tip[1] if tip[1]<128 else tip[1]-256;tens=trunc(frac,10);texts.add('$%d.%d%d'%(whole,tens,frac-tens*10))
texts=sorted(texts);assert texts
result=subprocess.run([str(root/'bin/tip-text-measure-compare.exe')],input='\n'.join(texts)+'\n',text=True,capture_output=True,check=True);native=list(map(int,result.stdout.split()));assert len(native)==len(texts);steps=0
for text,want in zip(texts,native):
 p=MeasureProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v in [(0x2007e38,0x8070e50),(0x2001e18,0),(0x2001e1c,32),(0x2001e20,0x8415bbc),(0x2001e24,1)]:p.write(a,v,4)
 for i,v in enumerate(text.encode()+b'\0'):p.write(0x2008000+i,v,1)
 p.reg[:3]=[0,0x2008000,256];p.run(entry=0x800fbac,stop=0x10020000);assert p.reg[0]==want,(text,p.reg[0],want);steps+=p.steps
report=dict(cases=len(texts),texts=texts,thumb_instructions=steps,match=True,scope='OriginalcompleteFBAC+nested28E3C vsnativefont0measure onallunique actualearnedtip strings. Explicit fontregistration fixture originalcontainer070E50/pairtable0415BBC/charbase32/spacing1; initialization and renderedglyphs unverified.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/tip-text-measure-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
