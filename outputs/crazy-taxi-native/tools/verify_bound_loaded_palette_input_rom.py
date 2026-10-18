from pathlib import Path
import json,types,hashlib
root=Path('outputs/crazy-taxi-native');build={};exec((root/'tools/verify_bound_graphics_package_input_rom.py').read_text().split('\nresults=[]')[0],build)
fade={};exec((root/'tools/verify_bound_restart_window_fade_input_rom.py').read_text().split('\nrows=[]')[0],fade)
rows=[]
for seed in [0,165,255]:
 env={'input_seed':seed,'__file__':str(Path(__file__).resolve())};code=build['code'].replace('ordinary-assets-rom-comparison.json',f'bound-loaded-palette-pre-{seed}.json');exec(compile(code,'loaded-palette-before-fade','exec'),env)
 p=env['p'];env['allowed_thumb'].update(fade['ns']['allowed_thumb']);old_external=p.external
 def external(self,t):
  if t==0x8032488:
   a,b=env['signed'](self.reg[0]),env['signed'](self.reg[1]);assert b;self.reg[0]=env['trunc'](a,b)&0xffffffff
  else:old_external(t)
 p.external=types.MethodType(external,p);p.write(0x2007542,240,2);p.write(0x2007544,160,2);count=p.read(0x2001780,2);assert count>0
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);start=p.steps;p.reg[:4]=[1,32,1,1];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800e5a8,stop=0x10020000)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]);rows.append({'seed':seed,'palette_records':count,'instructions':p.steps-start})
report={'status':'PASS_LOADED_BOUND_PALETTE_FADE_INPUT_RETAINED','cases':rows,'scope':'WholeE5A8(1,32,1,1) afterreal27994/E154/E388/7190 rawBoundpackage andfonts, registeredpalettecountnonzero;66inputretained/nooverlappingCPUstores. Divisionregister-onlycontract; precedingARM/DMAlogicalcopies and28BD0setup contract remainexplicit, notwholebootstrap/hardware.'}
(root/'reports/bound-loaded-palette-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS loaded palettes',rows)
