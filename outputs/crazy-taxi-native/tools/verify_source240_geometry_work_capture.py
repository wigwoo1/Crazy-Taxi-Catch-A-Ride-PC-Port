from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');old=json.loads((root/'reports/source240-preparation.json.source240.json').read_text());new=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text())
assert all(old[k]==new[k] for k in ['camera','yaw','pitch','phase']);assert len(new['packets'])==37
for a,b in zip(old['packets'],new['packets']):
 assert all(a[k]==b[k] for k in a);assert b['work_result']==1
 if b['kind']==0:assert 3<=len(b['vertices'])<=8 and all(len(v)==8 for v in b['vertices'])
 else:assert not b['vertices']
assert not (root/'reports/source240-geometry-work.err').read_bytes()
a=(root/'reports/source240-preparation.bmp').read_bytes();b=(root/'reports/source240-geometry-work.bmp').read_bytes();assert a==b,'Diagnostic capture altered snapshot'
special=[p for p in new['packets'] if p['kind']==2];v=json.loads((root/'reports/source240-special-raster-work-verification.json').read_text())
assert sum(p['texture_byte_reads'] for p in special)==v['texture_byte_reads']
assert sum(p['textured_halfword_writes'] for p in special)==v['framebuffer_halfword_writes']
report={'status':'PASS','packets':37,'prepared_vertices':sum(len(p['vertices']) for p in new['packets']),'depthclipped_packets':sum(bool(p['raster_flags']&3) for p in new['packets']),'texture_byte_reads':sum(p['texture_byte_reads'] for p in new['packets']),'textured_halfword_writes':sum(p['textured_halfword_writes'] for p in new['packets']),'flat_halfword_writes':sum(p['flat_halfword_writes'] for p in new['packets']),'flat_word_writes':sum(p['flat_word_writes'] for p in new['packets']),'snapshot_sha256':hashlib.sha256(b).hexdigest(),'snapshot_unchanged_bytes':len(b),'scope':'Actual isolated native source240 preparedscenery workload/geometry capture, all37nativeworkresults1. Snapshot byteexact unchanged; four specialcounts match originalROM. Native polygoncounts not yet compared as wholeoriginalscene, depthclippedconsumer still unverified; notcycles/cadenceproof.'}
(root/'reports/source240-geometry-work-capture-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
