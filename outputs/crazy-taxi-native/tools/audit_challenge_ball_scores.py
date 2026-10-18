exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json

def literal(off):
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 return struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]
for off,value in {0x9e88:0x02001558,0x9e8e:2700,0x9fac:0x02001558,0x9fb2:2700,0x1ff08:0x02001558,0x1ff0e:2700}.items():assert literal(off)==value
for off,mnemonic,operand in [(0x9e4e,'cmp','r1, #2'),(0x9e90,'subs','r1, r2, r1'),(0x9e98,'blt','#0x8009e9c'),(0x9e66,'adds','r0, r0, r1'),(0x9e6a,'lsls','r0, r0, #2'),(0x9f5e,'adds','r1, #0x6d'),(0x9f68,'strb','r0, [r1]'),(0xa92a,'subs','r1, r0, r1'),(0xa92c,'lsls','r1, r1, #0xf'),(0x1ff04,'cmp','r4, #2'),(0x1ff84,'subs','r0, r0, r1'),(0x1ff88,'bl','#0x8010988')]:
 ins=list(md.disasm(r[off:off+4],0x8000000+off,count=1))[0];assert (ins.mnemonic,ins.op_str)==(mnemonic,operand),(hex(off),ins.mnemonic,ins.op_str)
lines=['SHA-guarded originalBall9AB0 submode2 branch, externalBL opaque; linear segments exclude known literals.']
for lo,hi in [(0x9e4e,0x9fb8),(0x9fd4,0x9fe6),(0xa92a,0xa93a),(0x1ff04,0x1ff12),(0x1ff84,0x1ff8e)]:
 for ins in md.disasm(r[lo:hi],0x8000000+lo):lines.append(f'{ins.address:08x}: {ins.mnemonic} {ins.op_str}')
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-score-source.txt').write_text('\n'.join(lines))
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'submode':2,'record_group':'108+12*2+208*profile =132/340','time_input':'2700-signextended16(02001558)','comparison':'strict lower elapsed at9E98; ties skip','packing':'elapsed<<15; insertion row bits1..4 preserved, bits8..14 cleared; profilebit7, driverbits5..6, presentbit0','result':'1FF04..1FF10 ->1FF84->10988 originaltimeformatter','profile':'shared2518A mode128 calls9AB0(1), restore snapshot/recalculate own submode group; original profile dispatcher separately audited','scope':'Static game ROMsource and native arithmetic; no independent execution oracle/live earned-all20 proof'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-score-source.json').write_text(json.dumps(report,indent=2))
print('PASS ROM SHA/opcode/literal guarded Ball submode2/45s/time group132/strict lower/packed fields/source result formatter. Static source only.')
