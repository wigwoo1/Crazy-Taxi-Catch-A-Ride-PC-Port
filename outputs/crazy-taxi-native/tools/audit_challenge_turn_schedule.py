exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json

def cfg(start,limit):
 todo=[start];seen={};outside=[]
 while todo:
  off=todo.pop()
  if off in seen:continue
  if not start<=off<limit:outside.append(hex(0x8000000+off));continue
  i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bics') and i.op_str.startswith('#'):
   todo.append(int(i.op_str[1:],16)-0x8000000)
   if i.mnemonic=='b':continue
  todo.append(off+i.size)
 return seen,outside
rows={}
for name,start,limit in [('turn_hud',0x8c28,0x8e40),('passenger_phase1',0x22df4,0x23400)]:
 seen,outside=cfg(start,limit)
 rows[name]={'instructions':len(seen),'outside':outside,'calls':[{'site':hex(i.address),'target':i.op_str} for off,i in sorted(seen.items()) if i.mnemonic=='bl']}
 Path=pathlib.Path
 Path('outputs/crazy-taxi-native/reports/challenge-turn-'+name+'-cfg.txt').write_text('\n'.join(f'{i.address:08x} {i.mnemonic} {i.op_str}' for _,i in sorted(seen.items())))
# Exact original active trip glyph arguments and retained timer owner.
for off,want in [(0x8c58,0x02001694),(0x8c66,0x02007542),(0x8c6e,0x02005310)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==want
o=0x22f52;h=struct.unpack_from('<H',r,o)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((o+4)&~3)+(h&255)*4)[0]==0x31ff
rows['actor_cull']={'max_absolute_x_and_z':12799,'original_sites':['08022F3E..08022F66'],'max_signed_kind':7,'kind_site':'08022F68..08022F70','native_gap':'pickup_tick calls all actors, but passenger_idle and passenger_boarding both enforce source cull internally; connected boundary verification added'}
rows['finding']='Turn HUD active1694 group2 glyph0 uses center7542>>1, signed5310/16 truncation +1 as Y and signed5310 as scale; HUD then copies165C->16A8 before2083C countdown formatter. Original22DF4 phase1 culls passenger X/Z distance and kind before idle/boarding update, native idle/boarding already enforce these culls internally; full render/pickup/delivery phase ownership remains unverified.'
rows['scope']='Static bounded reachable Thumb decode; external calls opaque; no runtime path selection or GBA screen equivalence.'
rows['rom_sha256']=hashlib.sha256(r).hexdigest()
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-turn-schedule-source.json').write_text(json.dumps(rows,indent=2))
print(json.dumps(rows,indent=2))
