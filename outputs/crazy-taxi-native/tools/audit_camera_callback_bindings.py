exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
wanted={0x03006a90:0x08000718,0x03006ab8:0x080009bc,0x03006a94:0x080009d4,0x03006ab4:0x080009ec,0x03006ac4:0x08000a40}
rows=[]
for off in range(0x2ac00,0x2ae5a,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0x4800 or (h>>8)&7!=2:continue
 pool=((off+4)&~3)+(h&255)*4;dest=struct.unpack_from('<I',r,pool)[0]
 if dest not in wanted:continue
 h2=struct.unpack_from('<H',r,off+2)[0];assert h2&0xff00==0x4900
 p2=((off+6)&~3)+(h2&255)*4;source=struct.unpack_from('<I',r,p2)[0];assert source==wanted[dest]
 ins=[(i.mnemonic,i.op_str) for i in md.disasm(r[off:off+10],0x8000000+off)]
 assert ins[2:]==[('subs','r1, r1, r4'),('adds','r1, r0, r1'),('str','r1, [r2]')],ins
 rows.append({'site':hex(off+0x8000000),'destination':hex(dest),'rom_body':hex(source),'instructions':ins})
assert len(rows)==5
pathlib.Path('outputs/crazy-taxi-native/reports/camera-callback-bindings-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'bindings':rows,'scope':'Exact relocation assignment sites and ROM bodies; runtime relocation base/copy span and full mode3 composition remain to qualify.'},indent=2))
print('PASS all five camera callback relocation assignments map to original ROM bodies.')
