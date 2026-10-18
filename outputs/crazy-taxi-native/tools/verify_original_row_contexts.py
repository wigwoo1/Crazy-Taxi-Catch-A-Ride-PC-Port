from pathlib import Path
import json,struct,hashlib,collections
root=Path('outputs/crazy-taxi-native');d=root/'reports/original-core-reference';path=d/'rowContextsLeft.cpu-timing-trace.json';new=json.loads(path.read_text())['records'];old=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];assert len(new)==len(old)==1201 and [{k:v for k,v in n.items() if k!='row_context'} for n in new]==old
assert json.loads((d/'rowContextsLeft.packed-driving-trace.json').read_text())==json.loads((d/'cpuWindowsLeft.packed-driving-trace.json').read_text())
cases=[x for x in new if 'row_context' in x];assert len(cases)==73;rows=0
for x in cases:
 c=x['row_context'];first,last=c['outer_range'];data=bytes.fromhex(c['row_descriptors']);assert len(data)==(last-first)*32;rows+=last-first
 mx,my,sx,sy,texture=struct.unpack('<5I',bytes.fromhex(c['stack_parameters']));assert (mx,my,sx,sy) in ((63,4032,14,8),(31,992,14,9),(127,8064,14,7)) and 0x08000000<=texture<=0x08800000-(mx+my+1)
 table_offset=x['gprs'][10]-c['row_table'];row,remainder=divmod(table_offset,32);assert first<=row<last and remainder in (0,16)
 # Raw saved ARM PC1970 follows the destination-store1968 before table advance196C.
 destination_row=row+1 if c['source_rom_pc']==0x08001970 and remainder==16 else row
 destination=c['row_destination'];page=0x0600a000 if destination>=0x0600a000 else 0x06000000;assert destination==page+destination_row*240,(x['tick'],row,hex(destination))
report=dict(status='PASS',contexts=len(cases),prepared_rows=rows,prior_cpu_and_gameplay_samples_exact=1201,row_table='020344F0',source_loop='1730 outer rows/1934..195C inner',scope='Actual original VBlank midloop snapshots, complete prepared row ranges and real texture/stack parameters. Current table/destination agree including one observed destination advance before table advance at rawPC1970. Not true function-entry capture/fullframe/native comparison/cycle proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (path,root/'tools/capture_original_libretro.py')});(root/'reports/original-row-context-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'original row contexts',rows,'prepared rows, actual table/destination ownership;1201 CPU/gameplay samples exact')
