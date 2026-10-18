"""Original renderer prefix through actual indirect background branch, numeric bodies executed."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
angle={};src=(root/'tools/verify_background_pitch_owner.py').read_text().split('# Exact encoded ROM double limits')[0];src=src.replace("return signed(regs['r0']),steps","return regs,steps");exec(src,angle)
source=(root/'tools/original_thumb_numeric_probe.py').read_text()
source=source.replace("assert 0x08032000<=pc<0x08033500", "assert 0x08032000<=pc<0x08033500 or 0x0802b6ac<=pc<0x0802b6f8 or 0x0802dd5c<=pc<0x0802dd66 or 0x08034094<=pc<0x0803409c")
source=source.replace("if pc not in CODE_CACHE:","""if pc==0x08034098:
    branch=next(angle['md'].disasm(ROM[0x34098:0x3409c],0x08034098));assert branch.mnemonic=='b' and branch.op_str=='#0x80052b4'
    assert self.reg[0]==0
    registers,n=angle['source_angle'](self.reg[1]);self.reg[:4]=[registers['r'+str(i)] for i in range(4)];self.angle_steps+=n;self.stub_steps+=1;pc=self.reg[14]&~1;continue
   if pc not in CODE_CACHE:""")
# Thumb BXPC explicitly enters ARM state at alignedPC+4; helper return bridged after executing actual ARM052B4.
source=source.replace("elif m=='bx':nxt=val(o[0])&~1", "elif m=='bx':nxt=(((pc+4)&~3) if rn(o[0])==15 else val(o[0])&~1)")
exec(source)
class PrefixProbe(NumericProbe):
 def __init__(self,pitch,frame):
  super().__init__();self.shared={};self.angle_steps=self.stub_steps=0
  self.write(0x030069a8,0x02009100);self.write(0x03006a34,0x08002fe0)
  self.write(0x03006814,0x10001000);self.write(0x1000100c,pitch&MASK)
  for i,value in enumerate([0,0x03006810,0x0202baec,0x03006960]):self.write(0x02009100+i*4,value)
  self.reg[0]=frame
 def read(self,a,size=4):
  if 0x02009100<=a<a+size<=0x02009110 or 0x03006810<=a<a+size<=0x03006b00:return sum(self.shared.get(a+i,0)<<(8*i) for i in range(size))
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x02009100<=a<a+size<=0x02009110 or 0x03006810<=a<a+size<=0x03006b00:
   for i in range(size):self.shared[a+i]=(value>>(8*i))&255
  else:super().write(a,value,size)
cases=[0,1,-3,285,286,287,-285,-286,-287,2048,4095,-2147483648,2147483647]
cp=subprocess.run([str(root/'bin/background-pitch-probe.exe')],input=''.join(str(v)+'\n' for v in cases),capture_output=True,text=True,check=True);native=list(map(int,cp.stdout.split()));assert len(native)==len(cases);results=[]
for pitch,want in zip(cases,native):
 p=PrefixProbe(pitch,0x10080000);prior=[p.read(0x02009100+i*4) for i in range(4)];p.run(0x0802b6ac,stop=0x08002fe0)
 assert p.signed(p.reg[0])==want,(pitch,p.reg[:4],want);assert p.reg[1]==0x08002fe0
 assert [p.read(0x02009100+i*4) for i in range(4)]==[0x10080000,*prior[1:]]
 results.append({'camera_pitch':pitch,'background_pitch':want,'thumb_instructions':p.steps,'angle_arm_instructions':p.angle_steps,'arm_stub_instructions':p.stub_steps})
report={'status':'PASS','cases':results,'scope':'ActualThumb2B6AC framebuffer handoff, original2DD5C wrapper/BXPC/ARMstub tooriginal052B4, actualnested33340/33210/332A8 numericbodies, originalsourceclamp andindirect32450 BXr1 reachARM2FE0 withcorrectnativepitch. Fixture camera/frame/relocation bindings; mutableframeblock firstwordonly exact. Stopsatbackgroundentry; separate realbackground/rasterproof exists, continuous fullbackground call-return/frame/actors/palette/cycles not claimed.'}
(root/'reports/background-renderer-prefix-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
