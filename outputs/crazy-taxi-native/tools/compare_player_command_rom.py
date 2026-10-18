"""Original complete player command packer versus native, every button mask."""
exec(open('outputs/crazy-taxi-native/tools/compare_action_rom.py').read().split('\ncases=')[0])
cfg=json.loads((root/'reports/player-command-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class CommandProbe(ActionProbe):
 def write(self,a,v,n=4):
  if 0x30053d0<=a<0x30053e0:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
cases=[(mask,seed) for mask in range(1024) for seed in [0,127,255]]
res=subprocess.run([str(root/'bin/player-command-rom-compare.exe')],input=''.join(f'{m} {s}\n' for m,s in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (mask,seed),row in zip(cases,rows):
 probe=CommandProbe((0,0,0,0,0,1),0);probe.initialize((mask,0,0,0,0,0,0,0))
 for i in range(16):probe.write(0x30053d0+i,(seed+i*17)&255,1)
 probe.reg[0]=0;probe.run(entry=0x8011734,stop=0x10020000);got=[probe.read(0x30053d0+i,1) for i in range(16)];assert got==row,dict(mask=mask,seed=seed,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original11734 versusnativecommandpack all1024heldmasks x3existingcommandpatterns, all16outputbytes. Held-inputfixture; notphysicalXbox/inputsampler/fulltrajectory.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/driving.c',root/'tests/player_command_rom_compare.c',root/'tools/compare_player_command_rom.py',root/'bin/player-command-rom-compare.exe',root/'reports/player-command-cfg.json']}
(root/'reports/player-command-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
