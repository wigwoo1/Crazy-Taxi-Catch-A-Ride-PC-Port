"""Bounded ROM CFG discovery for Crazy Bound; external services remain unresolved."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
row=struct.unpack_from('<6I',r,0x4a4ff4)
assert row==(0x0800aad1,0x08008d61,0x08008f29,0x08070e50,0x087ebd98,150)
checks={0x8d96:('bne','#0x8008e2a'),0x8da2:('bne','#0x8008e2a'),0x8dd2:('subs','r0, r0, r1'),0x8e3c:('cmp','r2, r1'),0x8e3e:('bgt','#0x8008e98'),0x8e48:('bne','#0x8008e98'),0x8e52:('b','#0x8008f1c'),0x8e9a:('cmp','r0, #0x19'),0x8e9c:('bgt','#0x8008f1c'),0x8eb2:('bne','#0x8008f1c'),0x8ec2:('movs','r2, #0x9c'),0x8ec4:('lsls','r2, r2, #1'),0x8eda:('adds','r0, #0xb8')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0xaad6,0x02001660),(0xaae0,0x0200165e),(0x8e36,0x02002634),(0x8e3a,639)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
sections={};lines=[]
for name,start,end in [('initializer',0xaad0,0xaafc),('controller',0x8d60,0x8f28),('hud',0x8f28,0x9060)]:
 work=[start];seen={};outside=set();calls=set();literals={}
 while work:
  off=work.pop()
  if off in seen:continue
  if not start<=off<end:outside.add(off);continue
  i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
  h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:literals[hex(i.address)]=hex(struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0])
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
  if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
  work.append(off+i.size)
 assert not outside,(name,outside)
 sections[name]={'instructions':len(seen),'calls':[hex(x) for x in sorted(calls)],'literal_references':literals}
 lines.append(name);lines.extend(f'{seen[x].address:08x}: {seen[x].mnemonic} {seen[x].op_str}'+(' literal='+literals[hex(seen[x].address)] if hex(seen[x].address) in literals else '') for x in sorted(seen))
base=0x49dfe0;a=base+struct.unpack_from('<I',r,base+4+118*4)[0];assert r[a:r.index(0,a)]==b'Crazy Bound'
Path=pathlib.Path
Path('outputs/crazy-taxi-native/reports/challenge-bound-source.txt').write_text('\n'.join(lines))
Path('outputs/crazy-taxi-native/reports/challenge-bound-source.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'callback_row':[hex(x) for x in row],'label':'Crazy Bound','sections':sections,'scope':'ROM guarded bounded reachable CFG; no external service execution or playable Bound completion proof'},indent=2))
print('PASS Crazy Bound ROM SHA/label/callback/bounded initializer/controller/HUD CFG. External services not qualified.')
