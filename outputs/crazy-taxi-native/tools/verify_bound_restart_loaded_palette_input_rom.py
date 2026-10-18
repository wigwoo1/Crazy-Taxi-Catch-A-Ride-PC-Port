from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/verify_bound_restart_window_fade_input_rom.py').read_text().split('\nrows=[]')[0],ns)
class Probe(ns['Probe']):
 def __init__(self,*args):
  self.dma_copies=[];super().__init__(*args)
 def write(self,a,v,size=4):
  assert not(a<0x200351e and a+size>0x20034dc),('input overwrite',hex(a),size)
  super().write(a,v,size)
  if a==0x40000dc and v&0x80000000:
   assert v&0xffff0000==0x84000000,hex(v)
   source=self.hardware[0x40000d4];dest=self.hardware[0x40000d8];words=v&65535
   assert 0x5000000<=dest and dest+words*4<=0x5000400
   payload=[self.read(source+j*4) for j in range(words)]
   for j,value in enumerate(payload):self.write(dest+j*4,value)
   self.dma_copies.append([hex(source),hex(dest),words])
original=ns['ns'];saved_instructions=original['allowed_thumb'].copy()
exec(original['source'].replace('self.steps<50000','self.steps<200000').replace('self.steps<1000','self.steps<200000').replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z"),original)
original['allowed_thumb'].update(saved_instructions)
Probe.run=original['MotionProbe'].run
rows=[]
for count,seed,active in itertools.product([1,16,32],[0,165,255],[0,1]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536),0x5000000:bytearray([165])*1024};p.scalars={}
 # Install sentinel without treating fixture construction as original stores.
 p.regions[0x2000000][0x34dc:0x351e]=bytes((seed+off*13)&255 for off in range(66))
 p.regions[0x3000000][0x69c0:0x6a28]=ns['ns']['r'][0x7ebd98:0x7ebe00];p.write(0x3006810,0x30069c0)
 p.write(0x2007542,240,2);p.write(0x2007544,160,2);p.write(0x2001780,count,2)
 for n in range(count):
  rec=0x2001790+n*20
  p.write(rec,16,2);p.write(rec+2,(n%16)*32,2);p.write(rec+4,32,2);p.write(rec+6,active,1);p.write(rec+7,n//16,1);p.write(rec+16,0x84a5a34)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.writes=[];p.reg[:4]=[1,32,1,1];p.reg[13]=0x10000400;p.reg[14]=0x10020001;
 try:p.run(entry=0x800e5a8,stop=0x10020000)
 except Exception:
  print("PROBE FAILURE",count,seed,active,p.steps,hex(p.reg[15]),p.reg[:13],len(p.dma_copies));raise
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e])
 assert len(p.dma_copies)==(count if active else 0),(count,active,p.dma_copies)
 rows.append(dict(count=count,seed=seed,active=active,instructions=p.steps,dma=p.dma_copies))
report=dict(status='PASS_ORIGINAL_LOADED_PALETTE_RESTART_INPUT_RETAINED',cases=len(rows),thumb_instructions=sum(q['instructions'] for q in rows),observations=rows,rom_sha256=hashlib.sha256(ns['ns']['r']).hexdigest(),scope='CompleteoriginalE5A8(1,32,1,1) throughreturn with1/16/32registered16colorrecords, bothdomains andactive/inactive fixtures. OriginalpaletteROMsource andlogicalwordDMAcopies executed, all66inputbytesretained/nooverlapCPUorDMAstores. Syntheticregistrylayoutfixtures; actualwholewarmbootstrapotherhelpers andhardware timing excluded.')
(root/'reports/bound-restart-loaded-palette-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['cases'],report['thumb_instructions'])
