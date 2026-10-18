exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
lines=['Hash verified. Loader5778 callers: nearest direct r0 literal is a candidate, not control-flow proof.']
for off in range(0,0x34000,2):
 h=struct.unpack_from('<H',r,off)[0]
 if h&0xf800!=0xf000: continue
 ins=list(md.disasm(r[off:off+4],0x08000000+off))
 if not any(i.mnemonic=='bl' and i.op_str=='#0x8005778' for i in ins): continue
 lines.append(f'\nCALL {off:x}')
 start=max(0,off-24)
 for i in md.disasm(r[start:off+4],0x08000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  q=struct.unpack_from('<H',r,i.address-0x08000000)[0]
  if q&0xf800==0x4800:
   p=((i.address-0x08000000+4)&~3)+(q&255)*4
   v=struct.unpack_from('<I',r,p)[0];row+=f' literal={v:08x}'
   if ((q>>8)&7)==0 and 0x08000000<=v<len(r)+0x08000000-132:
    a,b=struct.unpack_from('<HH',r,v-0x08000000+0x80);row+=f' candidate_header_80_82={a},{b}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/effect-blend-header-callers.txt').write_text('\n'.join(lines)+'\n')
print('\n'.join(lines))
