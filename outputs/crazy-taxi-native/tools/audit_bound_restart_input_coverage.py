from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');rep=root/'reports'
prefix=json.loads((rep/'bound-restart-input-prefix-rom-audit.json').read_text())
package='bound-package-joined-object-input-rom-comparison.json';small='bound-restart-small-callees-input-rom.json';palette='bound-restart-palette-chain-input-rom.json';music='bound-restart-music-input-rom.json'
refs={'0x800f5e8':palette,'0x8025c18':'bound-restart-display-input-rom.json','0x8007378':package,'0x80073c0':package,'0x8027994':package,'0x800fec8':package,'0x800e154':palette,'0x800e5a8':palette,'0x802ac6c':'bound-background-bootstrap-input-rom.json','0x802c834':'bound-restart-window-fade-input-rom.json','0x802c688':'bound-restart-window-fade-input-rom.json','0x80203c4':'bound-terrain-resource-input-rom.json','0x8027730':music,'0x8026ef8':music,'0x8027110':music,'0x8026440':small}
rows=[]
for call in prefix['cases'][0]['calls']:
 file=refs.get(call);item=dict(call=call,component_evidence=file,joined_full_prefix=False)
 if file:
  path=rep/file;data=json.loads(path.read_text());item.update(evidence_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),scope=data.get('scope',''))
 else:item['gap']='No complete retained-input helper evidence in mapped current reports.'
 rows.append(item)
report=dict(status='INCOMPLETE_FULL_RESTART_INPUT_OWNERSHIP',original_prefix_helpers=rows,direct_gaps=[q['call'] for q in rows if not q['component_evidence']],remaining=['Conditional3244C callback dispatcher invokes descriptor084A4FF4 targetAAD0; ABC4 helper chain and retained memory effects remain unjoined','1E72Csignedremainder callee memory effects','203C4string/sqrt/division helper memory effects','Complete joined743Cprefix withactualretainedwarmowners','DescriptorAAD0 andremainingbootstrap continuation includingactor/vehicle/input sampling'],scope='Evidence inventory foractual18prefix calls; component PASS doesnotprovewholeprefix orauthorizeinputretentionpromotion. Componentfixture/helpercontract limitations remainexplicit.')
(rep/'bound-restart-input-coverage.json').write_text(json.dumps(report,indent=2)+'\n');print('direct gaps',report['direct_gaps'])
