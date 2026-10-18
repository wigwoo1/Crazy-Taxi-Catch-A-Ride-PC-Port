"""Original31860 caller through viewport restoration; all helpers explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x31860:0x31902],0x08031860):allowed_thumb[i.address]=bytes(i.bytes)
class PreviewProbe(InitProbe):
 def __init__(self,seed):
  super().__init__((0,0,0,0,0,1,0,0,0,0));self.calls=[]
  self.regions={0x03006810:bytearray((seed+j*37)&255 for j in range(248)),0x020358f0:bytearray([165])*4096,0x10010000:bytearray(24)}
  for j,v in enumerate([101,202,303,11,22,33]):self.write(0x10010000+j*4,v)
 def external(self,t):
  a=self.reg[:4];self.calls.append((t,a.copy()))
  if t==0x08033ecc:
   data=[self.read(a[1]+j,1) for j in range(a[2])]
   for j,v in enumerate(data):self.write(a[0]+j,v,1)
  elif t==0x0801e784:
   for j in range(a[2]):self.write(a[0]+j,a[1],1)
  elif t==0x0802c790:
   for j,v in enumerate(a):self.write(0x03006818+j*4,v)
  elif t==0x0802c834:self.write(0x03006814,a[0])
  elif t==0x0802c840:self.write(0x03006844,(self.read(0x03006844)+1)&0xffffffff)
  else:assert t in [0x0802965c,0x080295a8,0x0802b6a0,0x0802c918],hex(t)
expected=[0x08033ecc,0x0801e784,0x0802c790,0x0802965c,0x080295a8,0x0802c834,0x0802c840,0x0802b6a0,0x0802c918,0x08033ecc,0x0802c790]
steps=0;cases=0
for seed in [0,1,165,255]:
 for width,height in [(1,1),(32,64),(64,32),(64,64)]:
  p=PreviewProbe(seed);initial=bytes(p.regions[0x03006810]);p.reg[0:4]=[width,height,0x10010000,0x1001000c];p.reg[13]=0x10000400;p.reg[14]=0x10020001
  p.run(entry=0x08031860,stop=0x08031902)
  assert [t for t,a in p.calls]==expected
  assert bytes(p.regions[0x03006810])==initial
  assert not any(p.regions[0x020358f0])
  assert p.calls[3][1][1:]==[101,202,303]
  assert p.calls[4][1][1:]==[11,22,33]
  assert p.calls[7][1][0]==0x020120ec
  assert p.calls[8][1]==[0x020120ec,240,160,0x020358f0]
  assert p.calls[-1][1]==[int.from_bytes(initial[j:j+4],'little') for j in [8,12,16,20]]
  steps+=p.steps;cases+=1
report=dict(cases=cases,actual_thumb_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual31860 caller through318FE viewport restore, <=64 dimensions. All11helper calls explicit contracts including copy/fill/viewport/publication/generation; not whole helper or full31860 border verification. Confirms argument identities/order/state restore under modeled helper mutations; NOTnative full preview fidelity.')
(root/'reports/preview-caller-sequence-source-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
