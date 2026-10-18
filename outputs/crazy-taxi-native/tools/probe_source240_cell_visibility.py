from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/verify_source240_near_consumer.py').read_text().split('\nsteps=0;j=')[0])
src=src.replace('pc!=0x080046d8','pc not in (0x08002fb0,0x08002fb4)').replace('assert 0x4424<=off<0x4704 or off in (0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x2ec0<=off<0x2fb4,hex(pc)').replace("'ands','mla'):","'ands','mla','orr'):").replace("    elif m=='orrs':", "    elif m=='orr':x=a|b\n    elif m=='orrs':")
exec(src);arm.detail=True;BASE=0x08000700
j=json.loads((root/'reports/source240-bounds-and-cells-observation.json').read_text());data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);offset=20+nv*16+np*24+nm*28;nx,nz,sx,sz,ox,oy,oz=struct.unpack_from('<7i',data,offset+4)
# Execute original initialization2B404..2B436 with original literal owner and actual scene spacings.
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
thumb=Cs(CS_ARCH_ARM,CS_MODE_THUMB);regs={f'r{i}':0 for i in range(8)};mem={0x03006810:0x12000000,0x12000040:sx,0x12000044:sz};writes=[]
def val(s):return int(s[1:],0) if s.startswith('#') else regs[s]
def read(a):return struct.unpack_from('<I',rom,a-0x08000000)[0] if 0x08000000<=a<0x08800000 else mem[a]
for ins in thumb.disasm(rom[0x2b404:0x2b438],0x0802b404):
 p=ins.op_str.split(', ')
 if ins.mnemonic=='ldr':
  terms=ins.op_str.split('[',1)[1].split(']')[0].split(', ');a=((ins.address+4)&~3) if terms[0]=='pc' else regs[terms[0]];a+=val(terms[1]) if len(terms)>1 else 0;regs[p[0]]=read(a)
 elif ins.mnemonic=='str':
  terms=ins.op_str.split('[',1)[1].split(']')[0].split(', ');a=regs[terms[0]]+(val(terms[1]) if len(terms)>1 else 0);mem[a]=val(p[0]);writes.append([a,mem[a]])
 elif ins.mnemonic=='movs':regs[p[0]]=val(p[1])
 elif ins.mnemonic in ('adds','subs'):
  a=regs[p[0]] if len(p)==2 else val(p[1]);b=val(p[-1]);regs[p[0]]=(a+b if ins.mnemonic=='adds' else a-b)&0xffffffff
 else:raise AssertionError(ins.mnemonic)
cx=[mem[0x030068e0+i*4] for i in range(4)];cz=[mem[0x030068f0+i*4] for i in range(4)];assert cx==[0,0,sx,sx] and cz==[0,sz,0,sz]
trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])]
class CellProbe(NearClip):
 run=MarkerRasterProbe.run
 def __init__(self,cell):
  self.blocks=[(0x03006810,bytearray(248)),(0x12000000,bytearray(104)),(0x13000000,bytearray(4096)),(0x14000000,bytearray(72))];self.reg=[0]*16;self.reg[13]=0x13001000;self.reg[11]=0x03006810;self.reg[12]=0x12000000;self.reg[10]=0x14000000
  self.reg[1]=(ox-j['camera'][0])&MASK;self.reg[2]=(oz-j['camera'][2])&MASK;self.reg[3]=cell['x'];self.reg[6]=cell['z'];self.steps=0;self.z=self.n=self.v=self.c=False;self.entry=0x08002ec0
  for off,v in [(0x40,sx),(0x44,sz)]:self.write(0x12000000+off,v)
  for off,v in [(0x34,sine[(-j['yaw'])&4095]),(0x40,cosine[(-j['yaw'])&4095]),(0x30,sine[(-j['pitch'])&4095]),(0x3c,cosine[(-j['pitch'])&4095])]:self.write(0x14000000+off,v)
  for off,v in [(0x38,160),(0x2c,256),(0x30,38400)]:self.write(0x03006810+off,v)
  for i in range(4):self.write(0x030068e0+i*4,cx[i]);self.write(0x030068f0+i*4,cz[i])
results=[]
for cell in j['cells']:
 p=CellProbe(cell);p.run();results.append({'x':cell['x'],'z':cell['z'],'visible':p.z,'original_arm_instructions':p.steps,'references':cell['references']})
report={'status':'OBSERVED','corner_x':cx,'corner_z':cz,'original_corner_owner_writes':writes,'cells':results,'candidate_cells':len(results),'visible_cells':sum(c['visible'] for c in results),'visible_raw_references':sum(len(c['references']) for c in results if c['visible']),'original_arm_instructions':sum(c['original_arm_instructions'] for c in results),'scope':'OriginalThumb2B404..2B436 cellcorner initialization actualsceneSX/SZ; originalARM2EC0..2FAC cellvisibility decisions atactualcamera/sourceviewport coefficients. Patchedengine/world/camera contracts; native visibility not yet implemented/compared. Not fullcellloop/cache/pixels/cycles/cadenceproof.'};(root/'reports/source240-cell-visibility-observation.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k not in ['cells','original_corner_owner_writes']})
