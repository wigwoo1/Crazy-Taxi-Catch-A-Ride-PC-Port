from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_music_continuous_rom.py').read_text().split('\ncases=')[0],ns)
for f in json.loads((root/'reports/restart-music-dependency-audit.json').read_text())['functions']:
 for i in f['instructions']:ns['allowed_thumb'][int(i['address'],16)]=bytes.fromhex(i['bytes'])
class Probe(ns['ContinuousProbe']):
 def write(self,a,v,size=4):
  assert not(a<0x200351e and a+size>0x20034dc),('input overlap',hex(a),size)
  return super().write(a,v,size)
 def external(self,t):raise AssertionError(('unexpected loader/start helper',hex(t)))
rows=[]
for seed,enabled,order in itertools.product([0,165,255],[0,1],[5,63,88,115]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={}
 p.regions[0x2000000][0x34dc:0x351e]=bytes((seed+j*13)&255 for j in range(66));p.write(0x2007600,enabled,1)
 if not enabled:
  p.write(0x2007600,1,1);p.reg[0]=0x8421fc0;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8026ef8,stop=0x10020000);p.write(0x2007600,0,1)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);sequence=[]
 for entry,arg in [(0x8027730,0),(0x8026ef8,0x8421fc0),(0x8027110,order)]:
  old=p.steps;p.reg[0]=arg;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]);sequence.append(dict(entry=hex(entry),instructions=p.steps-old))
 rows.append(dict(seed=seed,enabled=enabled,order=order,sequence=sequence,playing=p.read(0x20075f6,1)))
report=dict(status='PASS_ORIGINAL_RESTART_MUSIC_INPUT_RETAINED',cases=len(rows),thumb_instructions=sum(q['instructions'] for row in rows for q in row['sequence']),observations=rows,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),scope='Wholeoriginal27730->26EF8(originalmodule08421FC0)->27110 onretainedRAM, all66inputbytesretained/nooverlappingstores, noexternalcalleecontracts. Enabledmodulegate anddisabledgatewithpreviouslyloadedmodule andfouractualmenu/gameorders; fixturesnotactualwholewarmbootstrap, mixer/hardware/fullnativeequivalence excluded.')
(root/'reports/bound-restart-music-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['cases'],report['thumb_instructions'])
