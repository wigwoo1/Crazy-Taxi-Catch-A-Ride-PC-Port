"""Prove original taxi scale consumes target distance, separate from projected Z."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
blocks={
(0x156d8,0x156f4):[('ldr','r0, [r4, #0x28]'),('lsls','r0, r0, #0xb'),('movs','r2, #0xee'),('lsls','r2, r2, #3'),('adds','r1, r7, r2'),('ldr','r1, [r1]'),('asrs','r1, r1, #0xc'),('movs','r3, #0xd0'),('lsls','r3, r3, #3'),('adds','r2, r7, r3'),('ldr','r2, [r2]'),('subs','r1, r1, r2'),('bl','#0x8032488')],
(0xbafe,0xbb08):[('adds','r0, r2, #0'),('cmp','r0, r3'),('bge','#0x800bb06'),('adds','r0, r3, #0'),('str','r0, [r4, #8]')],
(0xba84,0xba8e):[('adds','r0, r2, #0'),('cmp','r0, r4'),('ble','#0x800ba8c'),('adds','r0, r4, #0'),('str','r0, [r3, #8]')]
}
rows=[]
for (a,b),want in blocks.items():
 got=[(i.mnemonic,i.op_str) for i in md.disasm(r[a:b],0x8000000+a)];assert got==want,(hex(a),got)
 rows.append({'address':hex(0x8000000+a),'bytes':r[a:b].hex(),'instructions':got})
assert struct.unpack_from('<i',r,0x84a554c-0x8000000+0x680-0x4ac)[0]==0
assert struct.unpack_from('<i',r,0x84a5794-0x8000000+0x680-0x4ac)[0]==0
result={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':rows,'finding':'Ordinary and challenge opening routines clamp axis2 to4096 from opposite directions. Mode2 target distance770=axis2<<10 plus zoom, therefore settled stationary distance1024 with zero zoom. Body scale numerator=focal28<<11; denominator=ASR12(target770)-base680, explicitly independent of transformed body depth. Both original templates initialize base680 to0. Native existing source-taxi path uses this same denominator.','scope':'Static original arithmetic and local consumers, not full running GBA state. Other writers, callback bindings, rendering matrices, original opaque sprite output and complete runtime still require qualification. Provisional PC pullback changes presentation denominator and remains an adaptation.'}
pathlib.Path('outputs/crazy-taxi-native/reports/camera-body-scale-audit.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS source startup Z clamps and original taxi scale target-distance denominator; no GBA framing claim.')
