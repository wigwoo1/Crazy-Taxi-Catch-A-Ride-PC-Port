from pathlib import Path
import sys,json,struct,subprocess
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/compare_world_bounds_corners_rom.py').read_text().split('\nimport struct')[0])
trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])]
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);offset=20+nv*16+np*24+nm*28;nx,nz,sx,sz,ox,oy,oz=struct.unpack_from('<7i',data,offset+4)
corners=[]
for i in range(8):
 d=256 if i<4 else 38400;x=120 if i%4 in [1,2] else -120;y=80 if i%4>=2 else -80;corners.extend([int(x*d/192),int(y*d/192),d])
case=[j['camera'][0],j['camera'][2],j['yaw'],j['pitch'],nx,nz,sx,sz,ox,oz,*corners];steps=0;source=original(case)
cp=subprocess.run([str(root/'bin/world-bounds-corners-rom-compare.exe'),str(root/'assets/driving.ctr')],input=' '.join(map(str,case))+'\n',capture_output=True,text=True,check=True);native=list(map(int,cp.stdout.split()));assert source==native
cells=[];pos=offset+32
for zi in range(nz):
 for xi in range(nx):
  n=struct.unpack_from('<I',data,pos)[0];pos+=4
  if n==0xffffffff:continue
  refs=list(struct.unpack_from('<'+'H'*n,data,pos+24));pos+=24+n*2
  if source[0]<=xi<=source[2] and source[1]<=zi<=source[3]:cells.append({'x':xi,'z':zi,'references':refs})
assert pos==len(data)
report={'status':'PASS','camera':j['camera'],'yaw':j['yaw'],'pitch':j['pitch'],'corners':corners,'bounds':source,'original_arm_instructions':steps,'nonempty_cells':len(cells),'raw_reference_visits':sum(len(c['references']) for c in cells),'cells':cells,'scope':'Actualcamera original2D18 through2E6C bounds exactnativecorners with initializedsourcefocal192near256far38400/gridrealCTW. Candidatecell reference extraction fromrealCTW only, not originalcellloop/nativevisitor/cache proof.'};(root/'reports/source240-bounds-and-cells-observation.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k not in ['cells','corners']})
lines=[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in md.disasm(rom[0x2e70:0x2fe0],0x08002e70)];(root/'reports/source240-original-cell-loop.txt').write_text('\n'.join(lines)+'\n')
