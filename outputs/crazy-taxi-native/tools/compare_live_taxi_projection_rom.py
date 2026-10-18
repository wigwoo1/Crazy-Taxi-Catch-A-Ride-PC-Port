"""Full original taxi renderer projection with fresh native captured pose."""
exec(open('outputs/crazy-taxi-native/tools/compare_taxi_renderer_rom.py').read().split('\ncases=')[0])
import sys
label=sys.argv[1];capture=root/('reports/'+label+'.json');d=json.loads(capture.read_text());assert d['alpha']==0, 'Only exact simulation endpoint capture supported'
class LiveTaxiProbe(TaxiProbe):
 def external(self,target):
  if target==0x802dd5c:self.reg[0]=angle_probe.run(self.reg[0],self.reg[1])&MASK
  else:super().external(target)
p=LiveTaxiProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.arm.mem[0x2001708]=0;p.arm.mem[0x30053e0]=0;base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
x,y,z,heading,bodyroll=d['vehicle'];cx,cy,cz,pitch,yaw,roll=d['camera']
for off,val,n in [(0,x,4),(4,y,4),(8,z,4),(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x1c,heading,4),(0x1a,bodyroll,2),(0x770,d['distance']*4096,4),(0x680,d['base'],4)]:p.write(base+off,val,n)
p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x3006810]=0x10002000
for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+o]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
got=[p.read(base+0xe64)]+[v for req in p.requests for v in req[:2]]+[p.requests[0][2],p.requests[0][3]];want=[d['frame']]+[v for pair in d['centers'] for v in pair]+[d['depth'],d['scale']]
report={'status':'PASS' if got==want else 'MISMATCH','original':got,'native':want,'Thumb_instructions':p.steps,'ARM_instructions':p.arm.steps,'scope':'Complete original1556C plus ARM camera and2DC78 at exact captured native endpoint pose. Actual ARM angle difference; allocator/shadow/groundheight0/division contracts. Not whole source camera/world bootstrap, OAM pixels, widescreen or rear-facing fidelity proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),capture,root/'src/scene_viewer.c',root/('bin/'+(sys.argv[2] if len(sys.argv)>2 else 'scene-viewer-response-state-check.exe'))]}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print(report);assert got==want
