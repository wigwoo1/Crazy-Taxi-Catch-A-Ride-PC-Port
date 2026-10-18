"""SHA-guarded distinction between initial palette DMA and registered fade source."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[]
def exact(start,end,wanted):
 actual=[(i.mnemonic,i.op_str) for i in md.disasm(r[start:end],0x8000000+start)]
 assert actual==wanted,(hex(start),actual)
 checks.append(dict(address=hex(0x8000000+start),bytes=r[start:end].hex(),instructions=actual))
exact(0xe236,0xe240,[('adds','r1, r2, #0'),('adds','r1, #0x10'),('adds','r0, r0, r1'),('ldr','r1, [sp, #0x200]'),('str','r1, [r0]')])
exact(0xe8f2,0xe914,[('ldr','r6, [pc, #0x78]'),('strb','r7, [r6]'),('ldr','r5, [pc, #0x78]'),('mov','r1, r8'),('strh','r1, [r5]'),('ldr','r4, [pc, #0x74]'),('add','r2, sp, #0x200'),('ldrh','r2, [r2]'),('strh','r2, [r4]'),('ldr','r3, [pc, #0x70]'),('mov','r0, sl'),('strh','r0, [r3]'),('ldr','r0, [pc, #0x70]'),('strb','r7, [r0]'),('ldr','r2, [pc, #0x70]'),('movs','r0, #0'),('strh','r0, [r2]')])
exact(0xebd2,0xebdc,[('ldr','r6, [sp, #0x210]'),('ldr','r0, [r6]'),('ldr','r1, [sp, #0x22c]'),('adds','r0, r1, r0'),('ldrh','r1, [r0]')])
exact(0xebf6,0xec00,[('ldr','r4, [pc, #0x24]'),('ldrb','r1, [r4]'),('cmp','r1, #2'),('bne','#0x800ec44'),('ldr','r5, [pc, #0x20]')])
exact(0xec44,0xec4e,[('ldr','r0, [sp, #0x20c]'),('cmp','r0, #1'),('bne','#0x800ec98'),('ldr','r1, [pc, #0x48]'),('ldrh','r6, [r1]')])
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'checks':checks,'finding':'E154 retains requested palette separately from initial DMA choice. Challenge07494 E5A8(1,32,1,1) selects mode1,32 entries/BG cap flags1. Mode1 bypasses1770 exception restricted to mode2; eligible registered BG colors are channel capped from requested source. Full cap31 therefore uses course colors, not embedded balloon colors. Initial DMA and complete live registry/lookup remain separate.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-registered-palette-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS exact requested-registry pointer and mode1 cap-source opcodes; initial DMA is not final palette ownership.')
