"""Verify saved indexed assets independently of the ROM extraction run."""
import pathlib,json,hashlib
p=pathlib.Path('outputs/crazy-taxi-native/assets/indexed-scenes')
total=0;extra=set()
for name in ('city1','city2','startup'):
    m=json.loads((p/(name+'.json')).read_text());b=(p/(name+'.indices')).read_bytes()
    assert hashlib.sha256(b).hexdigest()==m['blob_sha256']
    end=0
    for t in m['textures']:
        assert t['byte_offset']==end and t['byte_length']==t['width']*t['height']
        end+=t['byte_length']
        assert hashlib.sha256(b[t['byte_offset']:end]).hexdigest()==t['sha256']
        extra.add(t['extra_word']);total+=1
    assert end==len(b) and len(m['textures'])==m['texture_count']
print(total,'texture ranges and digests verified; extra descriptor words:',sorted(extra))
