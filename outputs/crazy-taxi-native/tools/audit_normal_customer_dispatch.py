"""ROM-bound full customer dispatcher inventory; static evidence only."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
start,end=0x22df4,0x233d8
todo=[start];seen={};outside=set()
while todo:
 off=todo.pop()
 if off in seen:continue
 if not start<=off<end:outside.add(off);continue
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
  todo.append(int(i.op_str[1:],16)-0x8000000)
  if i.mnemonic=='b':continue
 todo.append(off+i.size)
assert not outside
bases=list(struct.unpack_from('<32I',r,0x7ec51c))
assert all(0x8000000<=x<0x8000000+len(r) for x in bases)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry':'08022DF4','reachable_instructions':len(seen),'outgoing_boundaries':[],
'calls':[{'site':hex(i.address),'target':i.op_str} for _,i in sorted(seen.items()) if i.mnemonic=='bl'],
'frame_banks':[{'kind':k,'stage':st,'base':hex(bases[k*4+st]),'frame_stride':2048} for k in range(8) for st in range(4)],
'ownership_findings':[
{'source':'08022E38..08022ECA','behavior':'Signed-byte clock 5F28 drives 5F29/5F2A oscillation every >6 ticks; rendering global state, absent from native customer_tick.'},
{'source':'08022F34..0802318C','behavior':'Per-customer cull, idle, boarding and completion flags; current native idle and boarding cover bounded state updates, not entire dispatcher.'},
{'source':'0802318E..080231AA','behavior':'Writes selected ROM frame pointer into customer word36 before billboard call; current native actor service does not write this field. Native renderer independently derives flattened texture index.'},
{'source':'080231AC..0802324C','behavior':'Boarding heading determines flip; sprite request uses live scale752C; native renderer has a corresponding separate projection path.'},
{'source':'08023314..0802338C','behavior':'Eligible customer markers use class and global oscillation. Full marker composition remains to qualify.'}],
'next_dependency':'Capture post-update customer word36 and billboard/marker requests at original22DF4 boundaries; join exact source-frame selection and shared phase globals to native actor scheduling. Do not claim idle/boarding alone completes22DF4.',
'scope':'Static reachable decode, ROM data addresses and current-source inspection. No original full-dispatch execution or pixel equivalence claim.'}
root=pathlib.Path('outputs/crazy-taxi-native');(root/'reports/normal-customer-dispatch-audit.json').write_text(json.dumps(report,indent=2))
(root/'reports/normal-customer-dispatch-cfg.txt').write_text('\n'.join(f'{i.address:08x} {i.mnemonic} {i.op_str}' for _,i in sorted(seen.items())))
print('Recovered',len(seen),'reachable instructions and',len(report['calls']),'call sites; all32 frame banks ROM-valid.')
