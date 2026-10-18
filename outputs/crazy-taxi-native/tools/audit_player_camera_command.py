exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
spans=[(0x11750,0x11754),(0x11762,0x11766),(0x117d4,0x117de)]
expected=[[('rsbs','r3, r3, #0'),('mov','sb, r3')],[('movs','r5, #3'),('rsbs','r5, r5, #0')],[('ldrb','r1, [r0, #5]'),('ands','r5, r1'),('mov','r1, sb'),('ands','r5, r1'),('strb','r5, [r0, #5]')]]
rows=[]
for (a,b),want in zip(spans,expected):
 got=[(i.mnemonic,i.op_str) for i in md.disasm(r[a:b],0x8000000+a)];assert got==want
 rows.append({'start':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':got})
assert struct.unpack_from('<H',r,0x1174e)[0]==0x2302
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'Player11734 clears command byte5 bits0 and1 via AND -3 and -2; retains upper six bits. Ordinary player routine supplies no camera switch/rear command.','scope':'Local original player command generator only; later writers, alternate AI and full call ordering remain unproven.'}
pathlib.Path('outputs/crazy-taxi-native/reports/player-camera-command-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original player command5 low two camera bits cleared.')
