"""SHA-guarded original result voice table/payload qualification."""
import pathlib,struct,hashlib,json
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
ptrs=struct.unpack_from('<203I',rom,0x7ec658)
rows=[]
for label,offset in [('initial_failure',0x134),('failed',0x118),('retry',0x11c),('return',0x120),('success',0x130),('best',0x110),('placed',0x114)]:
 index=offset//4;pointer=ptrs[index];start=pointer-0x8000000
 rate,prefix,length=struct.unpack_from('<HHI',rom,start)
 size=prefix+(length+1)//2 if prefix else length
 raw=rom[start:start+8+size]
 exported=pathlib.Path(f'outputs/crazy-taxi-native/assets/audio/{index:03}.cts').read_bytes()
 assert exported==raw
 rows.append(dict(event=label,table_offset=hex(offset),index=index,source_pointer=hex(pointer),rate=rate,prefix=prefix,samples=length,aliases=[i for i,p in enumerate(ptrs) if p==pointer],sha256=hashlib.sha256(raw).hexdigest()))
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-result-audio-qualified.json').write_text(json.dumps(rows,indent=2))
for row in rows:print(row)
print('PASS seven source result requests map to byte-identical extracted CTS records; metadata only, not playback/channel timing proof.')

