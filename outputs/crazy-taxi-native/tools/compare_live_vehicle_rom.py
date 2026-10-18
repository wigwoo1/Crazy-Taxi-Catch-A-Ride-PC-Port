"""Full original126DC through return vs synchronized live native stages."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=')[0])
import sys
root=pathlib.Path('outputs/crazy-taxi-native');label=sys.argv[1]
def read(kind,n):
 path=root/('reports/'+label+'-'+kind+'.txt');rows=[list(map(int,l.split())) for l in path.read_text().splitlines()];assert rows and all(len(a)==n for a in rows);return rows
motions=read('motion',54);tractions=read('traction',23);responses=read('response',5);assert len(motions)==len(tractions)==len(responses);steps=armsteps=0
for index,(a,t,b) in enumerate(zip(motions,tractions,responses)):
 assert a[47]==t[1]==b[0] and a[50]==t[4] and a[53]==t[7] and a[12]==t[5] and a[13]==t[6],('stage linkage',index)
 case=(a[0],a[1],a[2],a[3]//4096,a[4],a[5],a[6],a[7],a[8],a[9]);p=VehicleProbe(case)
 for off,v,n in [(0x1c,a[3],4),(0x20,a[10],4),(0x358,a[11],1),(0xe6a,a[12],1),(0xe6b,a[13],1),(0x7b8,a[14],4),(0x7b0,a[15],4),(0x7c8,a[16],4),(0xe84,a[17],4),(0x594,a[18],4),(0x540,a[19],4),(0,a[20],4),(4,a[21],4),(8,a[22],4),(0x53c,t[0],4),(0x570,t[2],4),(0x574,t[3],4),(0x744,b[1],4),(0x4ac,b[2],1)]:p.write(p.base+off,v,n)
 p.arm.mem[0x2001eb0]=a[23];p.arm.mem[0x2001534]=(a[24]&255)<<24;p.arm.mem[0x2001694]=(t[10]&255)<<16|(t[11]&255)<<24;p.arm.mem[0x20025d4]=t[12];p.arm.mem[0x200262c]=t[13];p.arm.mem[0x2002630]=t[14]
 for i in range(4):p.arm.mem[0x30053d0+i*4]=sum(a[25+i*4+j]<<(j*8) for j in range(4))
 p.run(entry=0x80126dc,stop=0x10020000)
 expected=a[41:53];expected[6]=b[3];expected[9]=t[16];expected+= [t[15],b[4],t[17],t[18]]+t[19:23]
 got=p.outputs();assert got==expected,(index,got,expected);assert p.read(p.base+0x28,1)==a[53];steps+=p.steps;armsteps+=p.arm.steps
report={'status':'PASS','calls':len(motions),'Thumb_instructions':steps,'ARM_instructions':armsteps,'scope':'Actual complete126DC through129C8 return including actual steering/orientation/traction/vector angle/body response. Native synchronized motion/traction/response traces with stage linkage assertions. Actor fields unchanged by prefix (force53C/capacities/response744/flags4AC) supplied from following observer. Decoded CTR trig globals, sqrt/division contracts, reward capture. Does not prove acceleration/collision/shared-world trajectory/render/fullframe.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),root/'src/vehicle_update.c',root/'bin/scene-viewer-response-state-check.exe']+[root/('reports/'+label+'-'+k+'.txt') for k in ['motion','traction','response']]}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(motions),steps,armsteps)
