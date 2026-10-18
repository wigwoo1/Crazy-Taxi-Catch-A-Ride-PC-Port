exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import re
rows={}
# Local symbolic candidates, not execution or complete control-flow analysis.
for start in range(0x11000,0x15c20,2):
 regs={f'r{i}':(f'r{i}',0) for i in range(8)}
 ins=list(md.disasm(r[start:start+96],0x8000000+start))
 for index,i in enumerate(ins):
  args=[x.strip() for x in i.op_str.split(',')];op=i.mnemonic
  def val(x):
   if x.startswith('#'):
    try:return (None,int(x[1:],0))
    except ValueError:return None
   return regs.get(x)
  result=None
  if op in ('str','strh','strb'):
   m=re.search(r'\[(r[0-7])(?:, #(0x[0-9a-f]+|[0-9]+))?\]',i.op_str)
   if m:
    addr=regs.get(m[1]);extra=int(m[2],0) if m[2] else 0
    if addr and addr[0] is not None and addr[1]+extra==0x738:
     rows[i.address]=[f'{j.address:08x}: {j.mnemonic} {j.op_str}' for j in ins[max(0,index-9):index+1]]
   continue
  if op in ('mov','movs') and len(args)==2:result=val(args[1])
  elif op=='ldr' and len(args)==3 and args[1]=='[pc':
   off=i.address-0x8000000;h=struct.unpack_from('<H',r,off)[0];pool=((off+4)&~3)+(h&255)*4
   result=(None,struct.unpack_from('<I',r,pool)[0])
  elif op in ('adds','subs') and len(args) in (2,3):
   a=val(args[0] if len(args)==2 else args[1]);b=val(args[-1]);sign=1 if op=='adds' else -1
   if a and b and b[0] is None:result=(a[0],a[1]+sign*b[1])
   elif op=='adds' and a and b and a[0] is None:result=(b[0],b[1]+a[1])
  elif op=='lsls' and len(args)==3:
   a=val(args[1]);b=val(args[2])
   if a and b and a[0] is None and b[0] is None:result=(None,(a[1]<<b[1])&0xffffffff)
  if args and args[0] in regs:regs[args[0]]=result
  if op in ('bl','bx','pop','b'):break
lines=['SHA verified. Local symbolic store candidates only; code/data and base provenance require exact validation.']
for address,context in sorted(rows.items()):lines+=['CANDIDATE '+hex(address)]+context
pathlib.Path('outputs/crazy-taxi-native/reports/camera-computed738-candidates.txt').write_text('\n'.join(lines))
print('\n'.join(lines))
