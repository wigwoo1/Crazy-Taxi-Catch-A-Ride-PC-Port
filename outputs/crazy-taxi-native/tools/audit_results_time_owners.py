"""Static result frame-time read audit. Literal islands are data, not execution proof."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json,pathlib
refs=[]
for i in md.disasm(r[0x2392c:0x24072],0x802392c):
    off=i.address-0x8000000
    h=struct.unpack_from('<H',r,off)[0]
    if h&0xf800!=0x4800:continue
    literal=((off+4)&~3)+(h&255)*4
    value=struct.unpack_from('<I',r,literal)[0]
    if value not in (0x2003b00,0x30053c0):continue
    following=list(md.disasm(r[off+i.size:off+i.size+16],i.address+i.size))
    refs.append({'address':f'{i.address:08X}','load':i.op_str,'literal':f'{literal+0x8000000:08X}','owner':f'{value:08X}','following':[f'{x.address:08X}: {x.mnemonic} {x.op_str}' for x in following]})
assert len(refs)==7 and all(x['owner']=='02003B00' for x in refs)
assert {x['address'] for x in refs}=={'08023D5E','08023DF8','08023EBE','08023EEE','08023F86','08023FB2','08024012'}
def exact(a,b):return [(i.mnemonic,i.op_str) for i in md.disasm(r[a:b],0x8000000+a)]
class_hold=exact(0x23f12,0x23f26)
assert class_hold==[('ldr','r1, [sp, #0x94]'),('lsls','r0, r1, #0x10'),('movs','r2, #0x80'),('lsls','r2, r2, #9'),('adds','r0, r0, r2'),('lsrs','r1, r0, #0x10'),('str','r1, [sp, #0x94]'),('asrs','r0, r0, #0x10'),('cmp','r0, #0x14'),('beq','#0x8023f28')]
rank=exact(0x24008,0x24012)
assert rank==[('adds','r0, r2, #0'),('subs','r0, #1'),('lsls','r0, r0, #0x18'),('lsrs','r0, r0, #0x18'),('str','r0, [sp, #0x84]')]
report={'status':'STATIC_OWNER_READS_RECOVERED','references':refs,'class_hold_per_call':1,'class_hold_threshold':20,'rank_decrement_per_call':1,'qualification':'Seven literal reads plus exact class-hold increment/threshold and unsigned byte rank-decrement instruction sequences asserted.','scope':'Static disassembly of original2392C..24070. Frame literals and following instruction contexts; no full result execution, branch reachability, native animation output or registered palette proof. Native results_state_step accepts frame time; per-call holds/rank changes remain unscaled as source claims pending dynamic oracle.'}
pathlib.Path('outputs/crazy-taxi-native/reports/results-time-owner-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
