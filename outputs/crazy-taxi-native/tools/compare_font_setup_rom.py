"""WholeFEC8/FF20 font owner setup/disable, nohelpers/contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
for entry,end in [(0x800fec8,0x800ff16),(0x800ff20,0x800ff3c)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in ['beq','bne','bhi','bls','bge','blt','bgt','ble']:todo.append(i.operands[0].imm)
class FontProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpectedhelper',hex(t)))
cases=[(disable,slot,a,b,c,d,e,seed) for disable in [0,1] for slot in [0,1,2,3,4,255,256,257,0xffffffff] for a,b,c,d,e,seed in [(32,1,0x803461c,0x12345678,0x87654321,0),(0,0,0,0,0,165),(0xffffffff,255,0xffffffff,0xffffffff,0xffffffff,65535)]]
rows=subprocess.run([str(root/'bin/font-setup-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 disable,slot,a,b,c,d,e,seed=case;p=FontProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2001e14:bytearray((seed+i*37)&255 for i in range(144))};p.scalars={0x2001e10:0x12345678}
 p.reg[:4]=[slot,a,b,c];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(p.reg[13],d);p.write(p.reg[13]+4,e);saved=p.reg[4:12].copy();p.run(entry=0x800ff20 if disable else 0x800fec8,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();assert p.read(0x2001e10)==int(parts[0]) and bytes(p.regions[0x2001e14])==bytes.fromhex(parts[1]),case
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'WholeFEC8 font descriptor setup andFF20disable: all144bytes/four36byte descriptors with untouchedpadding/nonselectedslot/cursor/SP/callee-saved, slotbytewrap/invalidguard/allsourcewords. FEC8 onlyregisters descriptorowners andresetscursor; noassetallocation/decode. Nohelpercontracts, nofontdraw/titlecaller/frame/parentproof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/font_setup.c',root/'src/font_setup.h',root/'tests/font-setup-rom-compare.c',root/'tools/rebuild_font_setup.ps1',root/'bin/font-setup-rom-compare.exe']}}
(root/'reports/font-setup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
