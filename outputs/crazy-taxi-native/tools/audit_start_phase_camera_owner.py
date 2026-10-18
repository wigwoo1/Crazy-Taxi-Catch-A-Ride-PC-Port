"""Qualify original start-phase flag ownership; selected exact instruction spans."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xb72e,0xb734,[('ldr','r0, [pc, #0x124]'),('movs','r5, #1'),('strb','r5, [r0]')]),(0xbd36,0xbd40,[('ldr','r1, [pc, #0x20]'),('movs','r0, #0'),('strb','r0, [r1]'),('bl','#0x800c524')]),(0xb284,0xb298,[('ldrb','r0, [r2]'),('cmp','r0, #0'),('bne','#0x800b29c'),('ldr','r0, [pc, #0xc]'),('movs','r1, #0'),('bl','#0x80130ac'),('bl','#0x8015ac8'),('b','#0x800b2d8')])]
checks.append((0xc524,0xc532,[('add','sp, #4'),('pop','{r3, r4}'),('mov','r8, r3'),('mov','sb, r4'),('pop','{r4, r5, r6, r7}'),('pop','{r0}'),('bx','r0')]))
checks.append((0xbf64,0xbf70,[('ldr','r0, [r0]'),('movs','r1, #0'),('movs','r2, #0'),('bl','#0x80267c8'),('b','#0x800c524')]))
rows=[]
for a,b,expected in checks:
 ins=list(md.disasm(r[a:b],0x8000000+a));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 rows.append({'address':hex(a+0x8000000),'bytes':r[a:b].hex(),'instructions':expected})
for off in [0xb72e,0xbd36]:
 h=struct.unpack_from('<H',r,off)[0];assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==0x02001696
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'SetupB72E sets1696=1; sequenceBD36 clears1696=0. Active phase withdemo flagzero routesB284 to130AC/15AC8 insteadordinary driving path.','scope':'Selected state stores/caller order proven. FullBA04 sequence/timers/assets/audio/events not recovered here; do not fabricate countdown duration or togglealternateyaw without native phase integration.'}
pathlib.Path('outputs/crazy-taxi-native/reports/start-phase-camera-owner-audit.json').write_text(json.dumps(report,indent=2));print('PASS original setup/start-phase1696 ownership and alternate frame branch.')
