"""Complete295A8/2965C constructors against native real destination adapter."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import struct
for entry,end in [(0x080295a8,0x0802964e),(0x0802965c,0x08029662)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a);seen.add(a);i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  todo.append(a+i.size)
trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])]
sine_address=int.from_bytes(r[0x29654:0x29658],'little');cosine_address=int.from_bytes(r[0x29658:0x2965c],'little')
cases=[];counts={}
for city in ['city1','city2']:
 data=(root/f'assets/{city}.ctp').read_bytes();count,routes,dc=struct.unpack_from('<III',data,4);counts[city]=dc
 for j in range(dc):cases.append(struct.unpack_from('<6i',data,16+count*32+j*48))
cases += [(x,y,z,a,b,c) for x,y,z in [(0,0,0),(-2147483648,17,2147483647)] for a,b,c in [(0,0,0),(-1,4096,4097),(2147483647,-2147483648,-4097),(1024,2048,3072)]]
rows=subprocess.run([str(root/'bin/destination-view-transform-rom-compare.exe'),str(root/'assets/driving.ctr')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 p=InitProbe((0,0,0,0,0,1,0,0,0,0));actor=0x10010000;p.regions[actor]=bytearray([165])*72
 for addr,words in [(sine_address,sine),(cosine_address,cosine)]:p.regions[addr]=bytearray(struct.pack('<4096i',*words))
 p.reg[14]=0x10020001;p.reg[:4]=[actor]+[v&MASK for v in case[:3]];p.run(entry=0x0802965c,stop=0x10020000)
 assert p.regions[actor][12:]==bytearray([165])*60
 p.reg[14]=0x10020001;p.reg[:4]=[actor]+[v&MASK for v in case[3:]];p.run(entry=0x080295a8,stop=0x10020000);steps+=p.steps
 assert list(struct.unpack('<18i',p.regions[actor]))==list(map(int,row.split())),case
report=dict(cases=len(cases),real_destination_counts=counts,thumb_instructions=steps,pass_match=True,scope='Complete original295A8/2965C with all18words vs native destination adapter, everyrealcitydestination plus signedangle/position boundaries. Runtime trigtables supplied from actualCTR1 decoded signedhalfwordASR4; no opaque instruction helper. Does not qualify full31860 scene render/state restoration or pixel equivalence.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/destination-view-transform-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
