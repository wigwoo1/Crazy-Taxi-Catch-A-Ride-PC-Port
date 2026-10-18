"""Recover startup bitmaps, checking every scanline against original 1E860."""
from pathlib import Path
import sys
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
sys.path.insert(0,str(root/'tools'))
from png_writer import write_png
entry=0x801e860
todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x801e95c,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
records=[];steps=0
for state in range(3):
 descriptor=0x49d560+state*28
 address=struct.unpack_from('<I',r,descriptor)[0];offset=address-0x8000000
 width,height,palettes,compression=struct.unpack_from('<4H',r,offset)
 assert (width,height,palettes,compression)==(240,160,256,1)
 palette=struct.unpack_from('<256H',r,offset+8);cursor=offset+520;indices=bytearray()
 for y in range(height):
  start=cursor;line=bytearray()
  while len(line)<width:
   command=r[cursor];cursor+=1;count=command&127
   assert count>0 and len(line)+count<=width,(state,y,count)
   if command&128:line.extend(r[cursor:cursor+count]);cursor+=count
   else:line.extend([r[cursor]]*count);cursor+=1
  p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray([0xa5]*256)};p.scalars={}
  p.reg[0]=0x8000000+start;p.reg[1]=width;p.reg[2]=0x10001000;p.reg[13]=0x10000400;p.reg[14]=0x10020001
  p.run(entry=entry,stop=0x10020000)
  assert bytes(p.regions[0x10001000][:width])==line,(state,y)
  assert p.reg[0]==0x8000000+cursor,(state,y,hex(p.reg[0]),hex(cursor))
  assert p.regions[0x10001000][width:]==bytes([0xa5]*16)
  steps+=p.steps;indices.extend(line)
 rgba=bytearray()
 for index in indices:
  color=palette[index];channels=[(color>>shift)&31 for shift in (0,5,10)]
  rgba.extend([(c<<3)|(c>>2) for c in channels]+[255])
 destination=root/'assets'/f'startup-screen-{state}.png'
 write_png(destination,width,height,rgba)
 for suffix,data in [('indices',indices),('rgba',rgba),('palette.rgb555',struct.pack('<256H',*palette))]:
  (root/'assets'/f'startup-screen-{state}.{suffix}').write_bytes(data)
 records.append({'state':state,'source_address':hex(address),'width':width,'height':height,'timer':struct.unpack_from('<I',r,descriptor+4)[0],'flags':r[descriptor+25],'compressed_bytes':cursor-offset-520,'index_sha256':hashlib.sha256(indices).hexdigest(),'png':str(destination),'png_sha256':hashlib.sha256(destination.read_bytes()).hexdigest(),'native_asset_hashes':{suffix:hashlib.sha256((root/'assets'/f'startup-screen-{state}.{suffix}').read_bytes()).hexdigest() for suffix in ['indices','rgba','palette.rgb555']}})
descriptors=[]
for state in range(6):
 a=0x49d560+state*28
 image,timer,in_bg,in_obj,out_bg,out_obj,background,callback,typ,flags=struct.unpack_from('<IIHHHHIIbB',r,a)
 descriptors.append({'state':state,'image':image,'timer':timer,'fade_in_bg':in_bg,'fade_in_obj':in_obj,'fade_out_bg':out_bg,'fade_out_obj':out_obj,'background':background,'callback':callback,'type':typ,'flags':flags})
assert descriptors[3]['type']==7 and descriptors[3]['flags']==1
(root/'assets/startup-screen-descriptors.json').write_text(json.dumps(descriptors,indent=2)+'\n')
report={'extractor_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'descriptors':descriptors,'status':'PASS','source_Thumb_instructions':steps,'source_scanlines':480,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Every source bitmap scanline checked against actual complete 1E860 decoder, returned source cursor and output guard. PNG RGB expands original BGR555 channels; no original scanout, fade, full startup lifecycle or live viewer binding claim.','screens':records}
(root/'reports/startup-bitmap-extraction.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
