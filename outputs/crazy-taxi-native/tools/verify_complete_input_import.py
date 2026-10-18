import pathlib,json,struct,subprocess,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ref=root/'reports/original-core-reference';records=[]
for side in ('left','right'):
 p=json.loads((ref/f'completeinput{side}.packed-driving-trace.json').read_text())['records']
 assert len(p)==361 and all(x['input_base']=='020034DC' and len(x['input_history'])==136 for x in p)
 baseline=(ref/'boot4200.ewram').read_bytes();assert bytes.fromhex(p[0]['input_history'])==baseline[0x34dc:0x3520]
 assert json.loads((ref/f'completeinput{side}.actor-trace.json').read_text())==json.loads((ref/f'driving{side}.actor-trace.json').read_text())
 records.extend(p)
cp=subprocess.run([str(root/'bin/input-snapshot-probe.exe')],input=''.join(r['input_history']+'\n' for r in records),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(records)
for r,row in zip(records,rows):assert row==list(struct.unpack('<3H40B10H',bytes.fromhex(r['input_history'])[:66]))
report={'status':'PASS_COMPLETE_ORIGINAL_INPUT_IMPORT','snapshots':len(records),'fields_per_snapshot':53,'scope':'Original baseline+720VBlank input snapshots imported into native TaxiInput: auxiliary/raw/pressed masks, held/previous/repeat/pressed bytes and10countdowns. Baselines byte-exact independentEWRAM;720lite trajectories unchanged. Diagnostic ingestion only, not full native input producer or continuous driving equivalence.','binary_sha256':hashlib.sha256((root/'bin/input-snapshot-probe.exe').read_bytes()).hexdigest()}
(root/'reports/complete-input-import-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
