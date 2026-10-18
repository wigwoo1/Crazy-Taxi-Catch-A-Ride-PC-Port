"""Whole27FB8 sprite replacement request/deferredqueue ownership."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl','bvs','bvc'}
saved_allowed=allowed_thumb.copy();source=source.replace("'b':True,'bne'", "'bhs':c,'blo':not c,'b':True,'bne'");exec(source);allowed_thumb.update(saved_allowed)
todo=[0x8027fb8];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8027fb8<=a<=0x80280c0,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class ReplaceProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpectedhelper',hex(t)))
cases=[(1,language,frame,frame,10,flag,count) for language in range(4) for frame in range(4) for flag in [0,1,256,257] for count in [0,63,64,65535]]
cases += [(0,0,index,frame,0,flag,0) for index in [0,1,65535,65536] for frame in [0,1,2,32,33] for flag in [0,1]]
cases += [(1,0,65537,65536,65546,257,0),(1,0,0,4,10,1,0),(1,0,0,0,65535,1,0)]
rompath='Crazy Taxi - Catch a Ride (USA).gba'
rows=subprocess.run([str(root/'bin/sprite-replacement-rom-compare.exe'),rompath],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 kind,language,index,frame,group,flag,count=case;resource=0x8034634 if kind else 0x8082104
 p=ReplaceProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007e3c:bytearray(struct.pack('<32I',*([0x6010000]*32))),0x20091ec:bytearray([165]*768),0x4000000:bytearray(256)};p.scalars={0x2007e38:resource,0x20094ec:count}
 for a,v in [(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(a,v)
 source_identity=struct.unpack_from('<I',r,0x7ebf48+language*4)[0]
 p.reg[:4]=[source_identity,index,frame,group];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(p.reg[13],flag);saved=p.reg[4:12].copy();p.run(entry=0x8027fb8,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();got=[p.read(0x20094ec,2),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)]
 assert got==list(map(int,parts[:4])),(case,got,parts[:4])
 assert bytes(p.regions[0x20091ec])==bytes.fromhex(parts[4]),(case,'queue bytes')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Whole27FB8 original titlegroup10 fourframes/allfourlanguage source identities andintrogroup0 frames0/1/2/32 plusinvalid33; exactimmediateDMArequests orALL768deferredqueuebytes/count, SP/callee-saved. Queuelimits0/63lastslot/64/65535full, immediatebytewrap0/1/256/257, sourceindex/frame/group16bitwrap/invalidguards, original4bpp32/8bpp64tile stride/metric tileoffset/32bitaddresswrap. Immutablegroup/frame metadata resolvedfromactualROM anddestinationbase6010000 fixture; request-only, noDMAexecution/VRAM/27994assetload/27CD0publication/pixels/audio/titlehandoff proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/sprite_replacement.c',root/'src/sprite_replacement.h',root/'tests/sprite-replacement-rom-compare.c',root/'tools/rebuild_sprite_replacement.ps1',root/'bin/sprite-replacement-rom-compare.exe']}}
(root/'reports/sprite-replacement-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
