"""Stateful original input sampler versus native; hardware key read explicit fixture."""
exec(open('outputs/crazy-taxi-native/tools/compare_action_rom.py').read().split('\ncases=')[0])
source=source.replace('self.steps<1000','self.steps<2000000').replace("a=val(ops[1]);n=val(ops[2]);value=","a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else val(ops[1]);value=").replace("['muls','ands','orrs']","['muls','ands','orrs','bics']").replace("else a&b))&MASK","else (b&~a if m=='bics' else a&b)))&MASK")
exec(source);ActionProbe.run=MotionProbe.run
cfg=json.loads((root/'reports/input-sampler-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class InputProbe(ActionProbe):
 def write(self,a,v,n=4):
  if 0x20034dc<=a<0x2003520:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
cases=[(1023^mask,0,0,0,1) for mask in range(1024)]
cases+=[(0,0,0,0,d) for d in [1]*65+[30,65535,65536,2147483648]]
cases+=[(raw,s,o,mask,d) for raw in [0,1023] for s in [0,1] for o in [0,1] for mask in [0,1,8,1023] for d in [0,1,60,65535]]
res=subprocess.run([str(root/'bin/input-sampler-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
probe=InputProbe((0,0,0,0,0,1),0)
for a in range(0x20034dc,0x2003520,4):probe.arm.mem[a]=0
for index,(case,row) in enumerate(zip(cases,rows)):
 raw,s,o,mask,d=case;probe.arm.mem[0x4000130]=raw;probe.arm.mem[0x2001ea8]=s<<16;probe.arm.mem[0x2001708]=o;probe.arm.mem[0x200170c]=mask;probe.arm.mem[0x2003b00]=d;probe.reg[14]=0x10020001;probe.run(entry=0x8015c3c,stop=0x10020000)
 got=[probe.read(0x20034de,2),probe.read(0x20034dc,2),probe.read(0x20034e0,2)]
 for i in range(10):got.extend([probe.read(a+i,n) for a,n in [(0x20034e2,1),(0x20034ec,1),(0x20034f6,1),(0x2003500,1)]]+[probe.read(0x200350a+i*2,2)])
 assert got==row,dict(index=index,case=case,original=got,native=row)
report=dict(cases=len(cases),thumb_instructions=probe.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal15C3C statefulinputsample comparison; explicithardwareactive-lowkeyread fixture; held/previous/pressed/repeat/countdown/allmaskstate compared, notphysicalXbox.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/input.c',root/'tests/input_sampler_rom_compare.c',root/'tools/compare_input_sampler_rom.py',root/'bin/input-sampler-rom-compare.exe',root/'reports/input-sampler-cfg.json']}
(root/'reports/input-sampler-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
