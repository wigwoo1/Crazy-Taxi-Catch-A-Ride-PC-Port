from pathlib import Path
import json,hashlib,itertools,struct
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/verify_bound_restart_loaded_palette_input_rom.py').read_text().split('\nrows=[]')[0],ns)
original=ns['original'];rom=original['r'];md=original['md']
for i in md.disasm(rom[0xf5e8:0xf62e],0x800f5e8):original['allowed_thumb'][i.address]=bytes(i.bytes)
todo=[0x800e154];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x800e154<=a<=0x800e380,hex(a);seen.add(a)
 i=next(md.disasm(rom[a-0x8000000:a-0x8000000+4],a,count=1));original['allowed_thumb'][a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
scene=struct.unpack_from('<I',rom,0x4a4ff4+0x10)[0];palette=struct.unpack_from('<I',rom,scene-0x8000000+0x18)[0]
assert scene==0x87ebd98 and palette==0x87557ac
rows=[]
for count,seed,flags in itertools.product([0,1,16,32,128],[0,165,255],[0,1,2,4]):
 p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536),0x5000000:bytearray([165])*1024};p.scalars={}
 p.regions[0x2000000][0x34dc:0x351e]=bytes((seed+off*13)&255 for off in range(66))
 p.regions[0x2000000][0x1790:0x2190]=bytes([165])*2560
 p.write(0x2001770,flags,1);p.write(0x2001780,count,2);p.write(0x2001782,512,2);p.write(0x2001784,512,2)
 p.write(0x2007542,240,2);p.write(0x2007544,160,2);before=bytes(p.regions[0x2000000][0x34dc:0x351e]);sequence=[]
 for entry,args in [(0x800f5e8,[0,0,0,0]),(0x800e154,[palette,32,1,0]),(0x800e5a8,[1,32,1,1])]:
  previous=p.steps;p.reg[:4]=args;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert bytes(p.regions[0x2000000][0x34dc:0x351e])==before
  sequence.append(dict(entry=hex(entry),instructions=p.steps-previous))
 assert p.read(0x2001780,2)==1 and len(p.dma_copies)==2
 expected=0x84a50b0 if flags&1 else 0x84a52b0 if flags&2 else palette if flags&4 else 0x84a50b0
 assert p.dma_copies[0]==[hex(expected),'0x5000000',128],p.dma_copies
 rows.append(dict(prior_count=count,seed=seed,flags=flags,sequence=sequence,dma=p.dma_copies))
report=dict(status='PASS_RETAINED_ORIGINAL_BOUND_PALETTE_RESTART_CHAIN_INPUT',cases=len(rows),thumb_instructions=sum(x['instructions'] for row in rows for x in row['sequence']),scene=hex(scene),palette=hex(palette),observations=rows,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='ActualcompleteF5E8reset->E154Bounddescriptorpalette(32,1)->E5A8(1,32,1,1), retainedmemorybetweenhelpercalls. Registeredcount0/1/16/32/128 fixtures,4retainedtransferflags and3inputsentinels; all66inputbytesretained/nooverlapCPUorlogicalDMAstores, registrationandfadeeachcopy512bytes. Remaining743Cgraphics/time/music/indirectdescriptorhelpers andactualgameplaywarmowners/fullbootstrap excluded.')
(root/'reports/bound-restart-palette-chain-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['cases'],report['thumb_instructions'])
