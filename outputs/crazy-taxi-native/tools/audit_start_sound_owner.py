"""Verify original startup sound indices and source pointers."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
rows=[]
for off,add in [(0xbb74,204),(0xbba0,208),(0xbcf2,212)]:
 ins=list(md.disasm(r[off:off+4],0x8000000+off))
 assert ins[0].mnemonic=='ldr' and ins[1].mnemonic=='adds' and ins[1].op_str==f'r0, #{hex(add)}'
 h=struct.unpack_from('<H',r,off)[0];pool=((off+4)&~3)+(h&255)*4
 assert struct.unpack_from('<I',r,pool)[0]==0x087ec658
 index=add//4;pointer=struct.unpack_from('<I',r,0x7ec658+add)[0]
 rows.append({'site':hex(0x8000000+off),'index':index,'pointer':hex(pointer)})
pathlib.Path('outputs/crazy-taxi-native/reports/start-sound-owner-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'requests':rows,'scope':'Source pointer/table indices only; sample names and hardware timing not asserted.'},indent=2))
print('PASS original startup sound table indices51/52/53.')
