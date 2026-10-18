"""Original taxi renderer/shadow chain with original29B44 city terrain query."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');ground_ns={}
s=(root/'tools/compare_bound_ground_contact_rom.py').read_text().split('\nschema=')[0]
exec(compile(s,'original-ground-definitions','exec'),ground_ns)
Ground=ground_ns['GroundProbe'];ground_ns['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/effect-height-cfg.json').read_text())['instructions']})
ground_cases=[]
retained_hint=0
def original_ground(taxi):
 global retained_hint
 taxi.write(taxi.reg[0]+4,retained_hint)
 point=[ground_ns['signed'](taxi.read(taxi.reg[0]+i*4)) for i in range(3)]
 g=Ground((0,0,0,0,0,1),0)
 for off,v in zip([0,4,8],point):g.write(g.base+off,v)
 g.arm.mem[0x03006810]=0x087556cc;g.arm.mem[0x03006940]=0x080029fc;g.reg[0]=g.base;g.reg[14]=0x10020001
 g.run(entry=0x08029b44,stop=0x10020000)
 got=ground_ns['signed'](g.reg[0]);assert got==taxi.ground,(point,got,taxi.ground)
 ground_cases.append(dict(point=point,height=got,thumb_instructions=g.steps,scope='Actual29B44 ROMcity1 query in independent diagnostic context; bounded original ARM containment and signeddivision contracts.'))
 retained_hint=ground_ns['signed']((g.reg[0]-192)&0xffffffff)
 return g.reg[0]
s=(root/'tools/verify_taxi_shadow_joined_renderer.py').read_text()
s=s.replace('if target==0x08029b44:self.reg[0]=self.ground&MASK','if target==0x08029b44:self.reg[0]=original_ground(self)')
s=s.replace("(root/'reports/taxi-shadow-joined-renderer-verification.json')","(root/'reports/taxi-shadow-retained-ground-renderer-verification.json')")
s=s.replace('Nativegroundquery suppliesexplicit29B44return, originalterrainroutine stillunexecuted;','Actual original29B44 city1terrainquery replaces supplied height, marshalled into separate GroundProbe context; boundedoriginalARMcontainment/division contracts;')
ns={'original_ground':original_ground};exec(compile(s,'taxi-original-ground-joined','exec'),ns)
report=dict(status='PASS',cases=ground_cases,scope='Actual original1556C caller input marshalled to actualoriginal29B44 usingROMcity1 geometry, returnfed backinto1556C/31A50 shadow chain. Nativegroundheight19200 allfive; shadowprojection/body outputmatch previouslyqualifiednative. Ground andrenderer share pointvalues, NOT entire architectural memory/register context; containment/division contracts remain. No whole-game/cycles proof.')
(root/'reports/taxi-shadow-original-ground-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report))
