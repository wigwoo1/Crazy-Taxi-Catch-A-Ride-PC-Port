exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
spans=[(0x14df6,0x14e10),(0x14e18,0x14e3e),(0x14e44,0x14e50)]
rows=[]
for a,b in spans:
 ins=list(md.disasm(r[a:b],0x8000000+a));rows.append({'span':[hex(a+0x8000000),hex(b+0x8000000)],'bytes':r[a:b].hex(),'instructions':[(i.mnemonic,i.op_str) for i in ins]})
for off,value in [(0x14df6,0x02001708),(0x14dfa,0x020034d4),(0x14e18,0x64c),(0x14e20,0x64c)]:
 h=struct.unpack_from('<H',r,off)[0];p=((off+4)&~3)+(h&255)*4;assert struct.unpack_from('<I',r,p)[0]==value
assert rows[1]['instructions'][0:4]==[('ldr','r2, [pc, #0x24]'),('adds','r1, r7, r2'),('movs','r0, #1'),('strh','r0, [r1]')]
assert struct.unpack_from('<H',r,0x4a554c+0x64c-0x4ac)[0]==0
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'selection':'1708 nonzero sets count034d4+4 to1; count0 sets actor64c=1; count1 keeps actor64c; other counts bypass target selection. Actor64c clearsbit16 then signed mode0/1/2/3 targets14e50/14e88/14ebc/14ff0.','template_mode':0,'scope':'Original selector boundaries/owners. Full spawn/reset/copy order and human-count initialization not proven; mode2-only prototype does not cover all branches.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-mode-selector-audit.json').write_text(json.dumps(report,indent=2))
print('PASS camera selector owners: human-count branch and actor64c; normal template mode0.')
