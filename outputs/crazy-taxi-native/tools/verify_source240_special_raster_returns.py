"""Original billboard full return ABI using actual consumer texture descriptors."""
from pathlib import Path
import json,struct,hashlib,types
root=Path('outputs/crazy-taxi-native')
scope={};exec((root/'tools/compare_source240_special_raster_work_rom.py').read_text().split('exe=root')[0],scope)
cls=scope['Probe'];j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures']
requests=json.loads((root/'reports/source240-joined-render-requests-verification.json').read_text())['requests'];args={r['payload']:r['arguments'] for r in requests};rows=[]
for idx,packet in enumerate(j['packets']):
 if packet['kind']!=2:continue
 ptr=int(textures[packet['texture']]['pixels'],16);clipped=packet['callback']=='030069a4';q=cls(ptr,tuple(packet['rect']),clipped)
 q.descriptor_byte_reads=0;original_read=q.read;descriptor=args[idx][2]
 def counted_read(obj,a,n=4):
  if n==1 and descriptor<=a<descriptor+12:obj.descriptor_byte_reads+=1
  return original_read(a,n)
 q.read=types.MethodType(counted_read,q)
 q.reg[0]=args[idx][0];q.reg[2:4]=args[idx][2:4];saved=[0xe7100000+k*0x10203 for k in range(9)];q.reg[4:13]=saved;sp=q.reg[13];lr=q.reg[14];fb=q.run()
 assert q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==lr,(idx,q.reg[4:13],saved)
 expected=cls(ptr,tuple(packet['rect']),clipped);pixels=expected.run();assert pixels==fb;assert (q.reads-q.descriptor_byte_reads,q.writes)==(expected.reads,expected.writes),(idx,q.reads,q.writes,expected.reads,expected.writes)
 rows.append({'source_index':packet['source_index'],'texture_descriptor':f'{args[idx][2]:08x}','original_arm_instructions':q.steps,'texture_reads':q.reads-q.descriptor_byte_reads,'descriptor_byte_reads':q.descriptor_byte_reads,'halfword_writes':q.writes,'stack_and_r4_through_r12_lr_restored':True,'framebuffer_sha256':hashlib.sha256(fb).hexdigest()})
assert len(rows)==4
report={'status':'PASS','packets':rows,'scope':'Fouractualsource240 sprites originalfull49F8/4A94callbacks throughBXLR, actualconsumerROMtexturedescriptor/R0/R3, seededR4-R12/SP/LR restoration andexactpreviousnativequalifiedpixels/work. R1vertex/rectangle buffer andcode/frame/bounds fixture relocated; no sharedconsumerstack/fullframe/cycles/cadence proof.'}
(root/'reports/source240-special-raster-returns-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
