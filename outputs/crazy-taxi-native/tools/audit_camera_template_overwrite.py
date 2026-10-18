"""Guard original vehicle template camera overwrite; no invented facing fix."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
ins=list(md.disasm(r[0x11840:0x1184e],0x8011840))
assert [(i.mnemonic,i.op_str) for i in ins]==[('ldr','r1, [pc, #0x10c]'),('adds','r1, r1, r6'),('mov','sl, r1'),('movs','r2, #0x92'),('lsls','r2, r2, #2'),('bl','#0x801e7f0')]
assert struct.unpack_from('<I',r,0x11950)[0]==0x4ac
normal=struct.unpack_from('<I',r,0x14ce8)[0];demo=struct.unpack_from('<I',r,0x14ca8)[0]
assert (normal,demo)==(0x084a554c,0x084a5794)
records=[]
for name,ptr in [('normal',normal),('demo',demo)]:
 off=ptr-0x8000000
 records.append({'template':name,'pointer':hex(ptr),'camera_pitch650':struct.unpack_from('<i',r,off+0x650-0x4ac)[0],'camera_heading654':struct.unpack_from('<i',r,off+0x654-0x4ac)[0]})
pathlib.Path('outputs/crazy-taxi-native/reports/camera-template-overwrite-audit.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'copy_destination':hex(0x4ac),'copy_size':584,'includes_camera654':True,'records':records,'scope':'Template copy includes camera fields; later camera updates and original full placement/copy ordering still require whole-path proof.'},indent=2))
print('PASS original584-byte template copy includes camera650/654; do not replace with guessed rear heading.',records)
