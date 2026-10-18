"""Execute complete original27610 against native represented channel fields."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
import sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x27610:0x27638],0x8027610):allowed_thumb[i.address]=bytes(i.bytes)
cases=list(itertools.product([0,1],[0,1,32768,65535],[0,1,32767,65535],[0,64,255]))
cp=subprocess.run([str(root/'bin/audio-stop-one-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 active,stamp,current,volume=case;p=ExitProbe((0,0,0,0,0,1,0,0,0,0));base=0x2009000
 for off in range(0,32,4):p.write(base+off,0xa5a5a5a5)
 for off,value,n in [(4,17,1),(5,19,1),(6,volume,1),(7,23,1),(8,stamp,2),(0x18,0x8100000 if active else 0,4),(0x1c,1234,4),(0x14,5678,4)]:p.write(base+off,value,n)
 p.write(0x2007564,current);p.reg[0]=base;p.reg[14]=0x10020001;p.run(entry=0x8027610,stop=0x10020000)
 actual=[int(bool(p.read(base+0x18))),p.read(base+8,2),p.read(base+0x1c),p.read(base+0x14),p.read(base+4,1),p.read(base+5,1),p.read(base+6,1),p.read(base+7,1),p.read(base)]
 assert actual==want,(case,actual,want)
 assert p.read(base+0xc)==(0 if active else 0xa5a5a5a5)
 assert p.read(base+0x10)==(0 if active else 0xa5a5a5a5)
 steps+=p.steps
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),cases=len(cases),thumb_instructions=steps,equal=True,scope='Complete27610 original active/inactive sound channel stop; all represented fields including rate16/retained age stamp/volume exact, decoder pointer clearing observed. Ten game handle ownership and live audio effects excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/audio_channel.c','src/audio_channel.h','tests/audio_stop_one_rom_compare.c','tools/compare_audio_stop_one_rom.py']})
(root/'reports/audio-stop-one-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
