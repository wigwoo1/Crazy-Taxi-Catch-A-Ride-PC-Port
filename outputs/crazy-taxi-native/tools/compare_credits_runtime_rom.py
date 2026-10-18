"""Original243A4 credits loop gates/fade/return versus native runtime."""
exec(open('outputs/crazy-taxi-native/tools/probe_scene_bg_palette_fade_rom.py').read().split('\np=FadeProbe')[0])
fade_allowed=allowed_thumb.copy();fade_cfg=cfg+rows;fade_source=source
exec(open('outputs/crazy-taxi-native/tools/compare_credits_flow_rom.py').read().split('\nrng=random.Random')[0])
source=fade_source.replace("   pc=nextpc\n","   pc=nextpc\n  self.stopped_pc=pc\n")
exec(source)
allowed_thumb.update(fade_allowed)
# Whole reachable credits CFG, excluding inline literal pools.
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True;todo=[0x243a4];seen={}
while todo:
 off=todo.pop()
 if off in seen:continue
 assert 0x243a4<=off<0x2463a,hex(off)
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i;allowed_thumb[i.address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  todo.append(int(i.op_str[1:],16)-0x8000000)
  if i.mnemonic=='b':continue
 todo.append(off+i.size)
class RuntimeProbe(FadeProbe):
 def external(self,target):
  if target==0x800e5a8:
   assert self.reg[:4] in ([1,32,0x2401,0x2401],[0,32,0x2401,0x2401]);self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8027730:self.stops+=1
  elif target in (0x8026ef8,0x8027110,0x801e1bc,0x8005778,0x800e388):pass
  elif target in (0x8028fe4,0x801e420,0x801e784,0x80267c8):CreditsProbe.external(self,target)
  else:raise AssertionError(hex(target))
RuntimeProbe.run=MotionProbe.run
# Compare every frame, including held first fade and final native return.
cases=[(1,1,0,99999,100),(1,1,33,99999,100),(1,1,100,99999,160),(1,1,99999,100,160),(1,1,99999,99999,2100),(2,2,100,99999,220)]
cp=subprocess.run([str(root/'bin/credits-runtime-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
native=[list(map(int,x.split())) for x in cp.stdout.splitlines()];cursor=0;compared=0;returns=0
for interval,reload,confirm_tick,start_tick,ticks in cases:
 p=RuntimeProbe((0,0,0,0,0,1,0,0,0,0));p.dma=[];p.stops=0;p.sound29=p.fade=0;p.text_id=-1
 for i in fade_cfg:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for k in range(0,64,4):p.write((a&~3)+k,0)
 for a in range(0x4000000,0x4000060,4):p.write(a,0)
 for a in range(0x202c4f0,0x202c4f0+4096*4,4):p.write(a,0)
 p.write(0x2000000,0x200a000);p.write(0x2001771,interval,1);p.write(0x2001772,reload,1);p.write(0x2003b00,1);p.write(0x2007602,1,1)
 p.write(0x2003500,0,1);p.write(0x2003503,0,1);p.write(0x2003520,123)
 p.reg[14]=0x10020001;p.run(entry=0x80243a4,stop={0x802446e,0x80245fc});sp=p.reg[13]
 complete=False;lastline=0;lastscroll=[0,0,256];entry=p.stopped_pc
 for tick in range(ticks):
  p.steps=0 # Bound each independently observed frame, including nested fade.
  if complete:
   expected=[tick,last[1],last[2],last[3],last[4],last[5],last[6],1,lastline,0,0,0,0,0,0]
  else:
   p.write(0x2003500,int(tick==confirm_tick),1);p.write(0x2003503,int(tick==start_tick),1)
   if tick:p.run(entry=entry,stop={0x802446e,0x80245fc})
   running=int(p.stopped_pc==0x802446e);p.sound29=0
   if running:p.run(entry=0x802446e,stop=0x80245fc)
   lastline=p.reg[10];sound=p.sound29
   p.reg[14]=0x10020001;p.run(entry=0x800ee10,stop=0x10020000)
   progress=p.read(0x200178a,2);progress=progress if progress<32768 else progress-65536
   fields=[progress,p.read(0x200177b,1),p.read(0x2001774,1),p.read(0x2001771,1),p.read(0x4000054,2),p.read(0x2003895,1)]
   before=p.stops;p.run(entry=0x8024600,stop={0x8024458,0x8024464,0x10020000});complete=p.stopped_pc==0x10020000;entry=p.stopped_pc
   scroll=[p.read(a,2) for a in (0x4000012,0x4000014,0x4000016)]
   expected=[tick,*fields,int(complete),lastline,*scroll,running,int(p.stops>before),sound]
  want=native[cursor];cursor+=1;assert expected==want,((interval,reload,confirm_tick,start_tick),tick,expected,want)
  last=expected;compared+=1
 if complete:returns+=1
assert cursor==len(native)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'compared_frames':compared,'completed_screens':returns,'equal':True,'scope':'Original243A4 setup locals then actual4458..446C gates/running loop/4600..4626 return; actualE5A8 andEE10 creditsmask2401 fade/BLDY.26318 boundary executesEE10 separately, input bytes supplied explicitly; setup music/assets/palette registration opaque, font formatting explicitpriorcontracts, sine0fixture. Nativecompletion preserves private flow after originalepilogue restores registers. No liveaudio/font/fullframe/IRQ/playerQuit integration proof.','source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/credits_runtime.c','src/credits_runtime.h','src/credits_flow.c','tests/credits_runtime_rom_compare.c','tools/compare_credits_runtime_rom.py']}}
(root/'reports/credits-runtime-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
