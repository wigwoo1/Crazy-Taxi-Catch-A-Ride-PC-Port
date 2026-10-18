"""Original Crazy Turn callback scalar and opaque service dependencies."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
row=struct.unpack_from('<6I',r,0x4a4fdc);assert row==(0x0800aaad,0x08008b0d,0x08008c29,0x08070e50,0x087ebd30,150)
checks={0xaab4:('movs','r0, #4'),0xaabe:('strh','r0, [r1]'),0x8b1a:('bl','#0x80206bc'),0x8b2a:('bl','#0x800dec8'),0x8b2e:('bl','#0x8021c5c'),0x8b32:('bl','#0x800ba04'),0x8b78:('subs','r0, r0, r1'),0x8b80:('bgt','#0x8008b8e'),0x8b88:('strb','r0, [r1]'),0x8b8c:('strb','r0, [r7]'),0x8b9c:('cmp','r6, #9'),0x8ba0:('cmp','r0, #9'),0x8bb6:('bl','#0x80267c8')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0xaab2,0x02005f2c),(0xaab8,0x0200165c),(0xaaba,2100),(0x8b62,0x0200165c),(0x8bbc,0x08290740)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800
 assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
lines=[];allcalls={}
for name,start,end in [('initializer',0xaaac,0xaac4),('controller',0x8b0c,0x8c28)]:
 work=[start];seen={};outside=set();calls=set()
 while work:
  off=work.pop()
  if off in seen:continue
  if not start<=off<end:outside.add(off);continue
  i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
  if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
  work.append(off+i.size)
 assert not outside,outside
 allcalls[name]=[hex(x) for x in sorted(calls)]
 lines.append(name);lines.extend(f'{seen[x].address:08x}: {seen[x].mnemonic} {seen[x].op_str}' for x in sorted(seen));lines.append('OUTSIDE:none')
base=0x49dfe0;address=base+struct.unpack_from('<I',r,base+4+117*4)[0];end=r.index(0,address);assert r[address:end]==b'Crazy Turn'
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-turn-source.txt').write_text('\n'.join(lines))
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-turn-source.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'callback_row':'084A4FDC','label117':r[address:end].decode(),'world':'087EBD30','initializer':'0800AAAC after sharedABC4 sets2100ticks and option3=4 only','controller':'08008B0C phase0 source206BC/10B54 beforeouter; DEC8/21C5C/BA04 beforegates; scalar signedtimer subtract/clamp fail; warningseconds>9tocross<=9 palette08290740+optionalSFX39; phase1worldrender','calls':allcalls,'scope':'Exact local opcodes/literals/bounded reachable controller; services opaque and success owner unresolved. Native scalar test is notcourse integration/playable completion orfullGBAfidelity.'},indent=2))
print('PASS original Crazy Turn callback/label117/2100timer/failurewarning; boundedCFG no unexpected targets, externalservices remain explicit dependencies.')
