"""Whole original DE64, exact retained recorder bytes and unrelated RAM."""
exec(open('outputs/crazy-taxi-native/tools/compare_title_parent_rom.py').read().split('class ParentProbe')[0])
for i in md.disasm(r[0xde64:0xde8c],0x800de64):allowed_thumb[i.address]=bytes(i.bytes)
class PrepareProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):raise AssertionError(hex(target))
seeds=list(range(256));binary=root/'bin/attract-prepare-rom-compare.exe'
rows=subprocess.run([str(binary)],input=''.join(str(s)+'\n' for s in seeds),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==256
steps=0
for seed,row in zip(seeds,rows):
 p=PrepareProbe((0,0,0,0,0,1,0,0,0,0));ram=bytearray((seed+j*17)&255 for j in range(262144));p.regions={0x2000000:ram};p.scalars={}
 ram[0x1708:0x171c]=bytes((seed+j*37)&255 for j in range(20));ram[0x1ea8]=(seed+91)&255;ram[0x3895]=(seed+153)&255
 expected=ram.copy();expected[0x1708]=1;expected[0x1709:0x170b]=bytes(2);expected[0x1710:0x1714]=bytes(4);expected[0x1718:0x171a]=b'\xff\xff';expected[0x1ea8]=expected[0x3895]=0
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x800de64,stop=0x10020000)
 assert ram==expected and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert bytes.fromhex(row)==bytes(ram[0x1708:0x171c])+bytes([ram[0x1ea8],ram[0x3895]])
 steps+=p.steps
report={'status':'PASS','cases':256,'Thumb_instructions':steps,'scope':'WholeDE64 with no child contracts. Exact20recorderbytes/2flags and ALL262144RAM retention/sourcewrites, SP/savedregs. No attract playback/ABEC/source parent composition proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/attract_prepare.c',root/'src/attract_prepare.h',binary]}}
(root/'reports/attract-prepare-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS whole attract preparation',len(seeds),'cases',steps,'Thumb')
