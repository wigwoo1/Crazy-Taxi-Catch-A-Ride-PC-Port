exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
for off,value in [(0xa1ac,0x0200165c),(0xa1b2,2100),(0xa2d0,0x0200165c),(0xa2d6,2100),(0x1ff3c,0x0200165c),(0x1ff42,2100),(0x8ca0,0x020016a8),(0x8ca2,0x0200165c)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
checks={0xa172:('cmp','r1, #4'),0xa1b0:('ldrsh','r1, [r2, r0]'),0xa1b4:('subs','r1, r2, r1'),0xa1b8:('lsrs','r0, r0, #0xf'),0xa1bc:('blt','#0x800a1c0'),0xa28c:('strb','r0, [r1]'),0xa2d8:('b','#0x800a92a'),0xa92c:('lsls','r1, r1, #0xf'),0x1ff38:('cmp','r4, #4'),0x1ff44:('b','#0x801ff84'),0x1ff88:('bl','#0x8010988'),0x8ca6:('ldrsh','r0, [r0, r2]'),0x8ca8:('str','r0, [r1]')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'finding':'Turn submode4 scores2100-signed165C elapsed, strictlower priorword>>15; originaldynamicgroup108+12*4+208*profile=156/364; sharedelapsedpacking<<15, metadata and clearedfraction. Results1FF38..44 useoriginal10988timeformat. HUD8CA0..A8 signextends165C into retained16A8, linking delivery gate.','scope':'Exact source-local opcode/literal guards, external formatting/profilecalls opaque; not fullfunction CFG or independentGBA live success.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-turn-score-source.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS Turn source2100timer/strictelapsed comparison/recordpacking/resultformatter/HUDretainedtimer copy.')
