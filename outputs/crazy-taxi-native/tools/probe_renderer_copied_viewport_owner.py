from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/audit_graphics_bootstrap_cold_source.py').read_text().split("report={'status'")[0])
rows=[]
for off in [0x1db0,0x1db4,0x1db8,0x1dbc,0x1dc4,0x49e8,0x49ec]:
 at=0x03000008+off-0x700;rows.append({'rom_offset':f'{off:08x}','copied_address':f'{at:08x}','rom_word':f'{int.from_bytes(r[off:off+4],"little"):08x}','copied_word':f'{p.read(at):08x}'})
slots=[]
for at in [0x0300692c,0x03006938,0x03006960]:
 value=p.read(at);slots.append({'slot':f'{at:08x}','value':f'{value:08x}','pointed_words':[f'{p.read(value+i*4):08x}' for i in range(4)] if 0x03000000<=value<0x03008000 else None})
report={'status':'OBSERVED','copied_words':rows,'slots':slots,'scope':'Actual cold bootstrap memory observation only; no viewport consumer correction inferred.'}
(root/'reports/renderer-copied-viewport-owner-observation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
