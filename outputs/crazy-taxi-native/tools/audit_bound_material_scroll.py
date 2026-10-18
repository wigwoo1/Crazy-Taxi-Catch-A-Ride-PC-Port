exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
arm=Cs(CS_ARCH_ARM,CS_MODE_ARM)
checks={0x3754:('ldr','r8, [r7, #0x18]'),0x3764:('mul','r8, r0, r8'),0x377c:('and','r8, r8, r0'),0x3780:('ldr','sb, [r7, #0x1a]'),0x3790:('mul','sb, r0, sb'),0x37a8:('and','sb, sb, r0'),0x37d0:('add','r0, r0, r8'),0x37d4:('add','r1, r1, sb'),0x37d8:('lsl','r0, r0, #6'),0x37dc:('lsl','r1, r1, #6')}
for off,want in checks.items():
 i=next(arm.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
m=r[0x7ad0b0:0x7ad0cc];d=r[0x780fe4:0x780fe4+12];assert m[24:28]==bytes([0,0,81,0]) and d[:4]==bytes([64,64,6,6])
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-scroll-source.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':{hex(k):v for k,v in checks.items()},'material_word':'00510000','ARM_rotated_plus26':'00000051','texture_logs':[6,6],'scope':'Source local ARM guards and exact data; native fixtures not independent execution oracle'},indent=2))
print('PASS Bound source ARM scrolling guards and exact64x64 texture descriptor.')
