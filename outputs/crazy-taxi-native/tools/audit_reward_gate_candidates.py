"""Static reward gate/caller candidates; never execution or ownership proof."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
hits=[];calls=[]
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800==0x4800:
  p=((off+4)&~3)+(h&255)*4;v=struct.unpack_from('<I',r,p)[0]
  if 0x02009aa0<=v<=0x02009abc:
   hits.append(dict(address=hex(0x8000000+off),literal=hex(v),context=[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in md.disasm(r[max(0,off-8):off+32],0x8000000+max(0,off-8))]))
 if h&0xf800==0xf000:
  ins=next(md.disasm(r[off:off+4],0x8000000+off,count=1),None)
  if ins and ins.mnemonic=='bl' and ins.op_str=='#0x802901c':
   calls.append(dict(address=hex(ins.address),context=[f'{i.address:08x}: {i.mnemonic} {i.op_str}' for i in md.disasm(r[max(0,off-16):off+8],0x8000000+max(0,off-16))]))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),scope='Static Thumb candidates over ROM offsets0..34000; literal pools may decode as instructions. Caller context and gate ownership require reachable CFG/execution qualification.',gate_literals=hits,reward_callers=calls)
pathlib.Path('outputs/crazy-taxi-native/reports/reward-gate-candidates.json').write_text(json.dumps(report,indent=2))
print('Literal candidates',len(hits),'reward caller candidates',len(calls))
for row in hits:
 if row['literal'] in ['0x2009aa4','0x2009aa7']:print(row)
for row in calls:print(row)
