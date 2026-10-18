"""SHA-guarded Crazy Ball typed-callback recovery; external BL are dependencies."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=[]
for name,start,end in [('palette-entry-configuration',0xe5a8,0xee00),('material-setup',0x2b0b0,0x2b330),('palette-initialize',0xe154,0xe388),('scene-install',0x2b44c,0x2b540),('world-frustum-setup',0x2b330,0x2b44c)]:
 work=[start];seen={};calls=set();outside=set()
 while work:
  off=work.pop()
  if off in seen:continue
  if not start<=off<end:outside.add(off);continue
  decoded=list(md.disasm(r[off:off+4],0x8000000+off,count=1));assert decoded
  i=decoded[0];seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic=='bl':calls.add(int(i.op_str[1:],16))
  if i.mnemonic=='b':work.append(int(i.op_str[1:],16)-0x8000000);continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):work.append(int(i.op_str[1:],16)-0x8000000)
  work.append(off+i.size)
 lines.append(f'ROM SHA verified; Crazy Ball palette {name}; external BL not executed/traversed.')
 for off,i in sorted(seen.items()):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}';h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:
   p=((off+4)&~3)+(h&255)*4;row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
 lines+=['CALLS:'+','.join(hex(x) for x in sorted(calls)),'OUTSIDE:'+','.join(hex(x) for x in sorted(outside))]
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-palette-source.txt').write_text('\n'.join(lines))
print('\n'.join(lines))

import json
scene=struct.unpack_from('<I',r,0x4a4f7c+48+16)[0]
address=struct.unpack_from('<I',r,scene-0x08000000+24)[0]
course=r[address-0x08000000:address-0x08000000+512]
assert pathlib.Path('outputs/crazy-taxi-native/assets/challenge-worlds/challenge2-palette.rgb555').read_bytes()==course
embedded=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-ball/balloon.palette').read_bytes()
indices=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-ball/balloon.indices').read_bytes()
rows=[dict(index=i,embedded=hex(struct.unpack_from('<H',embedded,i*2)[0]),course=hex(struct.unpack_from('<H',course,i*2)[0]),equal=embedded[i*2:i*2+2]==course[i*2:i*2+2]) for i in sorted(set(indices)-{0})]
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),scene=hex(scene),scene_palette=hex(address),used_colors=rows,different_used_colors=sum(not x['equal'] for x in rows),scope='Original extracted bitmap and course palette comparison; active BG palette depends on1770 flags and palette registration/order, not proven by this comparison. Neither embedded nor course palette alone is an established live color oracle.')
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-ball-palette-audit.json').write_text(json.dumps(report,indent=2))
print('PASS original Ball course palette bytes and bitmap indices audited:',report['different_used_colors'],'of',len(rows),'used visible colors differ; live registration flags/order still unresolved.')
