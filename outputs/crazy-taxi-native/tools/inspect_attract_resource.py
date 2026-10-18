exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
base=0x5a338;n=struct.unpack_from('<I',r,base)[0];print('groups',n)
rows=[]
assert n<128
for g in range(n):
 offset=base+struct.unpack_from('<I',r,base+4+g*4)[0];header=struct.unpack_from('<5I',r,offset);row={'group':g,'identity':hex(offset+0x8000000),'header':list(header)};rows.append(row);print(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-resource-structure.json').write_text(__import__('json').dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Raw source descriptor/header inspection only; no allocation or decode proof','groups':rows},indent=2)+'\n')
