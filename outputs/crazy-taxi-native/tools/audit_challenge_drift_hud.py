"""Exact source HUD requests/registered label and shared assets; not live GBA oracle."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0x8938:('subs','r2, #0x17'),0x894e:('movs','r1, #4'),0x8950:('bl','#0x803407c'),0x8974:('adds','r7, #0x3c'),0x89fe:('subs','r0, #0x50'),0x8a14:('bl','#0x8032488'),0x8a44:('bl','#0x8032520'),0x8a6a:('movs','r0, #0xf'),0x8a82:('movs','r0, #0x11'),0x8a9a:('movs','r0, #0x15'),0x8aac:('movs','r0, #0x9a'),0x8aae:('lsls','r0, r0, #2'),0x8aba:('subs','r1, r0, #2'),0x8ac0:('adds','r2, #0x12'),0x8af6:('bl','#0x800f7dc'),0x1e204:('movs','r0, #5'),0x1e208:('bl','#0x8028fb8'),0x1e20c:('movs','r0, #5'),0x1e20e:('bl','#0x8028fa0')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0x8930,0x087ec364),(0x8956,0x0200164e),(0x8970,0x087ec120),(0x8a02,0x087ec12c),(0x8a0c,0x0200164c),(0x1e202,0x0849dfe0)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
base=0x49dfe0;assert struct.unpack_from('<I',r,base)[0]>=616
address=base+struct.unpack_from('<I',r,base+4+616*4)[0];end=r.index(0,address);assert r[address:end]==b'Combos'
assert pathlib.Path('outputs/crazy-taxi-native/assets/challenge-flag/flag-hud-positions.bin').read_bytes()==r[0x7ec12c:0x7ec168]
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'label_id':616,'label_address':hex(address+0x8000000),'label_text':r[address:end].decode('ascii'),'source':'08008924..08008B08; language5 registered/selected at0801E202..0801E20E; lookup08028FE4','rules':'shared 5 clock requests; signed164C tens optional/ones, slash glyph15, denominator glyph17/21; width-80 then16px steps; label width-2,y+18,rightalignment2,pausedbank1/normal2,scale256; gear height-23 andflags4ACbit5','qualification':'Exact bounded local opcodes/literals/registered original label and position bytes; not independent GBA execution or screen oracle.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-drift-hud-source.json').write_text(json.dumps(report,indent=2))
print('PASS original Drift HUD opcodes/literals/positions; localized616 at'+hex(address+0x8000000)+' = Combos.')
