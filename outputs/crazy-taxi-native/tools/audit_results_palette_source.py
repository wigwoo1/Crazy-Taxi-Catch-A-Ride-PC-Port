"""Qualify mode1 results palette source versus registration DMA flags."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks=[(0xe466,0xe46a,[('ldr','r1, [sp, #0x20]'),('str','r1, [r0]')]),(0xebd2,0xebdc,[('ldr','r6, [sp, #0x210]'),('ldr','r0, [r6]'),('ldr','r1, [sp, #0x22c]'),('adds','r0, r1, r0'),('ldrh','r1, [r0]')]),(0xebf6,0xebfe,[('ldr','r4, [pc, #0x24]'),('ldrb','r1, [r4]'),('cmp','r1, #2'),('bne','#0x800ec44')]),(0xec44,0xec4a,[('ldr','r0, [sp, #0x20c]'),('cmp','r0, #1'),('bne','#0x800ec98')])]
lines=['ROM SHA verified. E388 original pointer retained; E5A8 mode!=2 bypasses1770 color substitution. Per-frame EE10 behavior remains separately qualified.']
for start,end,expected in checks:
 ins=list(md.disasm(r[start:end],0x8000000+start));assert [(i.mnemonic,i.op_str) for i in ins]==expected
 lines.extend(f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in ins)
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),checked_spans=[hex(start) for start,end,expected in checks],qualification='Results23996 mode1 reaches E5A8 registered original-palette processing and mask1 cap, bypassing mode2-only1770 color substitution. Initial registration DMA and full EE10 frame palette states remain separate.')
p=pathlib.Path('outputs/crazy-taxi-native/reports/results-palette-source-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
pathlib.Path('outputs/crazy-taxi-native/reports/results-palette-source-audit.txt').write_text('\n'.join(lines)+'\n');print('\n'.join(lines));print('PASS four exact ROM instruction spans.')
