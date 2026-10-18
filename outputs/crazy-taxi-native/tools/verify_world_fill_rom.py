"""Original contact-buffer fill and multi-register diagnostic support."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
source=source.replace("'blt':n!=v}","'blt':n!=v,'bls':not c or z}")
source=source.replace("elif m=='bl':", "elif m in ['ldm','stm']:\n    base_reg=rn(ops[0]);address=self.reg[base_reg];regs=sorted(rn(o) for o in ops[1:])\n    for j,k in enumerate(regs):\n     if m=='ldm':self.reg[k]=self.read(address+4*j)\n     else:self.write(address+4*j,self.reg[k])\n    if ins.writeback:self.reg[base_reg]=address+4*len(regs)\n   elif m=='bl':")
source=source.replace('self.reg[13]+=4*len(regs)\n   elif', 'self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1\n   elif')
exec(source)
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-fill-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']};steps=0;cases=0
for offset in [0,1,2,3]:
 for count in [0,1,3,4,15,16,17,28,32]:
  for value in [0,0x55,0x123]:
   probe=MotionProbe((0,0,0,0,0,1),0);dest=probe.base+0x100+offset
   for a in range(probe.base+0xfc,probe.base+0x130,4):probe.write(a,0xa5a5a5a5)
   probe.reg[:3]=[dest,value,count];probe.reg[14]=0x10020001;probe.run(entry=0x8033f2c,stop=0x10020000)
   for a in range(probe.base+0xfc,probe.base+0x130):assert probe.read(a,1)==(value&255 if dest<=a<dest+count else 0xa5),(offset,count,value,hex(a))
   steps+=probe.steps;cases+=1
report=dict(cases=cases,thumb_instructions=steps,scope='Original33F2C fill executed; bytewise expected fill and adjacent guards. Includes actualworld aligned28zero contact initialization. Diagnostic multi-register transfer and popPC support, not fullworld.',rom_sha256=hashlib.sha256(r).hexdigest());(root/'reports/world-fill-rom-verification.json').write_text(json.dumps(report,indent=2));print('PASS',report)
