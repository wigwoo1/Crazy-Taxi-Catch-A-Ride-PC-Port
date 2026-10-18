exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
rows=[]
for off,reg in [(0x3244c,'r0'),(0x32450,'r1'),(0x3245c,'r4')]:
 got=[(i.mnemonic,i.op_str) for i in md.disasm(r[off:off+4],0x8000000+off)]
 assert got==[('bx',reg),('mov','r8, r8')],got
 rows.append({'address':hex(off+0x8000000),'bytes':r[off:off+4].hex(),'instructions':got})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'3244C/32450/3245C are Thumb register-indirect interworking thunks, not matrix bodies. Mode3 loads callbacks from03006AC4/6A90/6AB8/6A94/6AB4. Callback binding initializer required before composition is established.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-matrix-thunks-audit.json').write_text(json.dumps(report,indent=2))
print('PASS camera matrix callsites resolve through exact register-indirect Thumb thunks.')
