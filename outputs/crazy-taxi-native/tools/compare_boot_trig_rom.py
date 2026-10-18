"""Whole original2DB54, all source trig table outputs and retained RAM."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
for line in (root/'reports/boot-0802db54-source.txt').read_text().splitlines()[1:]:
 a=int(line[:8],16);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
class TrigProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):raise AssertionError(('unexpectedtrighelper',hex(target)))
steps=0;results=[];binary=root/'bin/boot-trig-compare.exe';rom='Crazy Taxi - Catch a Ride (USA).gba'
for seed in [0,165,255]:
 p=TrigProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray((seed+i*13)&255 for i in range(262144))};p.scalars={};retained=p.read(0x2003899,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy()
 p.run(entry=0x802db54,stop=0x10020000);assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 capture=root/'reports/boot-trig-native.bin';subprocess.run([str(binary),rom,str(seed),str(capture)],check=True)
 assert capture.read_bytes()==bytes(p.regions[0x2000000]);assert p.read(0x2003899,1)==retained
 results.append({'seed':seed,'retained_parent_flag':retained});steps+=p.steps
report={'status':'PASS','cases':len(results),'original_Thumb_instructions':steps,'compared_bytes_per_case':262144,'results':results,'scope':'Actualwhole2DB54 helper no contracts. Original4096sine+4096cosine signed16ASR4->signed32 outputs at0202C4F0/020304F0 compared plus allsurroundingexternalRAM/retained3899/SP/savedregs. ExistingCTRassets source mapping previouslystaticchecked; nowwholehelper executed. Notwholeboot orlaterinitializers/liveparent proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/boot_memory.c',root/'src/boot_memory.h',root/'tests/boot-trig-compare.c',binary,Path(__file__)]}}
(root/'reports/boot-trig-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholeoriginal boot trig helper',len(results),'cases',steps,'Thumb')
