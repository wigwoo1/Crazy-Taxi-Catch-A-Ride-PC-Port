import pathlib,struct,hashlib,sys,json
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=pathlib.Path('outputs/crazy-taxi-native');r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);targets={0x8011a3c:'driver',0x8011f80:'world',0x8014dd8:'camera',0x801556c:'renderer',0x8011540:'reset',0x8014d44:'finalizer'};refs=[]
for off in range(0,0x34000-4,2):
 a,b=struct.unpack_from('<HH',r,off)
 if a&0xf800!=0xf000 or b&0xf800!=0xf800:continue
 ins=list(md.disasm(r[off:off+4],0x8000000+off))
 if len(ins)!=1 or ins[0].mnemonic!='bl':continue
 target=int(ins[0].op_str.lstrip('#'),16)
 if target not in targets:continue
 start=max(0,off-16);window=[]
 for candidate in range(start,off+20,2):
  if candidate>=2 and struct.unpack_from("<H",r,candidate-2)[0]&0xf800==0xf000 and struct.unpack_from("<H",r,candidate)[0]&0xf800==0xf800:continue
  item=list(md.disasm(r[candidate:candidate+4],0x8000000+candidate,count=1))
  if item:window.append(item[0])
 refs.append(dict(address=hex(0x8000000+off),target=hex(target),routine=targets[target],context=[dict(address=hex(i.address),text=i.mnemonic+' '+i.op_str) for i in window]))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),direct_call_candidates=refs,scope='Static Thumb BL scan within08000000..08034000; no indirectcalls, data falsepositives possible, contexts not reachabilityproof.')
(root/'reports/ordinary-scene-callers-audit.json').write_text(json.dumps(report,indent=2)+'\n')
for x in refs:print(x['address'],x['routine'], '; '.join(i['address']+':'+i['text'] for i in x['context']))
