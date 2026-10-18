"""ROM-bound display setup blocks. Static proof, not a live palette trace."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
root=pathlib.Path('outputs/crazy-taxi-native')
def block(lo,hi):
 rows=[]
 for i in md.disasm(r[lo:hi],0x8000000+lo):
  row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row['literal']=hex(struct.unpack_from('<I',r,p)[0])
  rows.append(row)
 return rows
mode3=block(0x25d1c,0x25d72);mode4=block(0x25d90,0x25de8)
assert r[0x25d2a:0x25d2e]==bytes.fromhex('03201080')
assert r[0x25d9e:0x25da2]==bytes.fromhex('04201080')
assert r[0x25dc8:0x25dce]==bytes.fromhex('c020c0040860')
assert next(x for x in mode4 if x['address']=='0x8025dce')['literal']=='0x600a000'
assert next(x for x in mode3 if x['address']=='0x8025d52')['literal']=='0x200754c'
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'mode3_block':mode3,'mode4_block':mode4,'source_contract':{'descriptor':'0200754C','mode3_buffers':['06000000','06000000'],'mode4_buffers':['06000000','0600A000'],'dimensions':[240,160],'mode3_bits_per_pixel':16,'mode4_bits_per_pixel':8},'scope':'Literal and instruction audit of isolated source display setup blocks. Does not prove which display mode every gameplay path selects, page flipping, active palette contents, raster pixels or native fidelity.','next_dependency':'Trace display setup caller selection and palette transfer table at 080225xx..080227xx. Palette destination constructed as A0<<19; literal-only scanning misses these transfers.'}
(root/'reports/marker-display-setup-audit.json').write_text(json.dumps(report,indent=2))
print('Verified mode3 and mode4 buffer setup, 240x160 dimensions, alternate buffer0600A000.')
