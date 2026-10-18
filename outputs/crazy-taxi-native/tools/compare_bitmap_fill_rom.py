"""Whole26198 source fill comparison; no platform/helper contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
source=source.replace("'b':True,'bne'", "'bhs':c,'blo':not c,'b':True,'bne'")
exec(source)
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl','bvs','bvc'}
todo=[0x8026198];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8026198<=a<=0x8026316,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class FillProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpected helper',hex(t)))
cases=[(mode,w,h,red,green,blue,seed) for mode in [0,3,4,5,255] for w,h in [(0,8),(8,0),(1,1),(8,8)] for red,green,blue,seed in [(0,0,0,0),(31,31,31,165),(65535,32768,255,12345),(1024,1024,1024,65535)]]
rows=subprocess.run([str(root/'bin/bitmap-fill-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 mode,w,h,red,green,blue,seed=case;p=FillProbe((0,0,0,0,0,1,0,0,0,0))
 p.regions={0x10001000:bytearray(struct.pack('<256H',*((seed+i*113)&65535 for i in range(256)))),0x6000000:bytearray((seed+i*37)&255 for i in range(128)),0x600a000:bytearray((seed+i*53)&255 for i in range(128)),0x2007540:bytearray(24)}
 p.scalars={}
 p.write(0x2007541,mode,1);p.write(0x2007542,w,2);p.write(0x2007544,h,2);p.write(0x200754c,0x6000000);p.write(0x2007550,0x600a000);p.write(0x2007554,0x10001000)
 p.reg[:3]=[red,green,blue];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();owners=bytes(p.regions[0x2007540]);palette=bytes(p.regions[0x10001000])
 p.run(entry=0x8026198,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved,case
 assert bytes(p.regions[0x2007540])==owners and bytes(p.regions[0x10001000])==palette,case
 assert bytes(p.regions[0x6000000]+p.regions[0x600a000])==bytes.fromhex(row),case
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Whole26198 mode3/mode4/unsupported guards, signed16 RGB inputs, nearest Manhattan RGB5 selection with strict tie retention and initial256 threshold, correct bank ownership, zero dimensions, all256 bank bytes including retained tails, palette/state preservation and SP/callee-saved registers. Native capacity guard is extra unsupported-input protection; not full25E38 loader or hardware display proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/bitmap_fill.c',root/'src/bitmap_fill.h',root/'tests/bitmap-fill-rom-compare.c',Path(__file__),root/'bin/bitmap-fill-rom-compare.exe']}}
(root/'reports/bitmap-fill-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
