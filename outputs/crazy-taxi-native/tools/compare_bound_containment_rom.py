"""Bounded ARM containment diagnostic, explicit entry V=0 condition fixture."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_pivot.py').read().split('# Source motion')[0])
def containment(case):
 reg=[0]*16;mem={};n=z=v=False;steps=0
 for j in range(4):
  reg[j]=0x10000000+j*16;mem[reg[j]]=case[2*j]&MASK;mem[reg[j]+8]=case[2*j+1]&MASK
 for pc in range(0x51cc,0x52b4,4):
  w=int.from_bytes(r[pc:pc+4],'little');steps+=1;cond=w>>28;assert cond in [14,0,12,13]
  if not {14:True,0:z,12:not z and n==v,13:z or n!=v}[cond]:continue
  if (w&0x0ffffff0)==0x012fff10:return signed(reg[0]),steps
  if (w>>25)&7==4:continue # bounded prologue/epilogue callee register save only
  if (w&0x0c000000)==0x04000000:
   assert not(w&(1<<25)) and w&(1<<20);reg[(w>>12)&15]=mem[reg[(w>>16)&15]+(w&4095)];continue
  if (w&0x0fc000f0)==0x00000090:
   dest=(w>>16)&15;val=reg[w&15]*reg[(w>>8)&15]
   if w&(1<<21):val+=reg[(w>>12)&15]
   reg[dest]=val&MASK
   if w&(1<<20):n=bool(reg[dest]&0x80000000);z=reg[dest]==0
   continue
  opcode=(w>>21)&15;rd=(w>>12)&15;left=reg[(w>>16)&15]
  if w&(1<<25):operand=ror(w&255,((w>>8)&15)*2)
  else:
   operand=reg[w&15];shift=(w>>7)&31;typ=(w>>5)&3
   assert not(w&16)
   if shift:assert typ==2;operand=(signed(operand)>>shift)&MASK
  if opcode==10:
   out=(left-operand)&MASK;n=bool(out&0x80000000);z=out==0;v=bool((left^operand)&(left^out)&0x80000000)
  else:
   assert opcode in [2,4,13];reg[rd]=({2:lambda:left-operand,4:lambda:left+operand,13:lambda:operand}[opcode]())&MASK
 raise AssertionError('no return')
root=pathlib.Path('outputs/crazy-taxi-native');ctw=(root/'assets/challenge-worlds/challenge5.ctw').read_bytes();nv=int.from_bytes(ctw[4:8],'little');polys=20+nv*16
queries=json.loads((root/'reports/bound-ground-query-candidates.json').read_text())['queries'];cases=[]
for q in queries:
 ids=struct.unpack_from('<4H',ctw,polys+q['selected_polygon']*24+8)
 for tri in [(ids[0],ids[1],ids[2])]+([] if ids[3]==65535 else [(ids[0],ids[2],ids[3])]):
  verts=[struct.unpack_from('<3i',ctw,20+i*16) for i in tri];cases.append(tuple(q['point'][j] for j in [0,2])+tuple(v[j] for v in verts for j in [0,2]))
res=subprocess.run(['outputs/crazy-taxi-native/bin/containment-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=list(map(int,res.stdout.split()));assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 got,count=containment(c);assert got==want,dict(case=c,original=got,native=want);steps+=count
report=dict(cases=len(cases),arm_instructions=steps,scope='OriginalARM51CC containment vsnative actualselectedpolygon triangles, entryV0fixture, prologue/epilogue save abstraction. Not groundselection or generalCPUoracle.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/bound-containment-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
