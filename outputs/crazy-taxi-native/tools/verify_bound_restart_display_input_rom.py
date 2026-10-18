from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_display_mode_rom.py').read_text().split('\ncases=')[0],ns)
class Probe(ns['DisplayProbe']):
 def __init__(self,*args):self.input_shadow=bytearray(66);self.write_log=[];super().__init__(*args)
 def read(self,a,size=4):
  if 0x20034dc<=a and a+size<=0x200351e:return int.from_bytes(self.input_shadow[a-0x20034dc:a-0x20034dc+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  self.write_log.append((a,size))
  if 0x20034dc<=a and a+size<=0x200351e:self.input_shadow[a-0x20034dc:a-0x20034dc+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  super().write(a,v,size)
rows=[]
for flag,seed in itertools.product([0,1,255,256,257],[0,165,255]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.input_shadow=bytearray((seed+off*13)&255 for off in range(66));before=bytes(p.input_shadow);p.write_log=[]
 p.reg[:2]=[4,flag];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8025c18,stop=0x10020000)
 assert before==bytes(p.input_shadow) and not any(a<0x200351e and a+n>0x20034dc for a,n in p.write_log)
 rows.append({'arg1':flag,'input_seed':seed,'instructions':p.steps,'writes':[[hex(a),n] for a,n in p.write_log if a<0x10000000]})
report={'status':'PASS_FULL_MODE4_DISPLAY_INPUT_RETAINED','cases':len(rows),'thumb_instructions':sum(x['instructions'] for x in rows),'observations':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'Whole actual25C18mode4 jump-table target25D90 pluscomplete28E04/28E20callees;66inputbyte shadow retained/nooverlappingstores acrossargumentbytewrap fixtures. Actualrequiredarg1=1included. Hardware writes memorycontracts, notdevices. Doesnotqualify otherbootstrapgraphics/palette/bulkfill/ARMhelpers.'}
(root/'reports/bound-restart-display-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),report['thumb_instructions'])
