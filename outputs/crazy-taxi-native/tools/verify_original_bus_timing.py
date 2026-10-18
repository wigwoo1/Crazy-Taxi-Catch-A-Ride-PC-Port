from pathlib import Path
import json,collections,hashlib
root=Path('outputs/crazy-taxi-native');d=root/'reports/original-core-reference'
new=json.loads((d/'busTimingLeft.cpu-timing-trace.json').read_text())['records'];old=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];assert len(new)==len(old)==1201
assert [{k:v for k,v in n.items() if k!='bus_timing'} for n in new]==old
assert json.loads((d/'busTimingLeft.packed-driving-trace.json').read_text())==json.loads((d/'cpuWindowsLeft.packed-driving-trace.json').read_text())
fields={k:dict(collections.Counter(n['bus_timing'][k] for n in new)) for k in ('waitcnt','display_control','display_status','vcount','interrupt_enable','interrupt_flags','interrupt_master')}
report={'status':'PASS','samples':len(new),'prior_cpu_and_gameplay_records_exact':True,'observed_io_values':fields,'scope':'Read-only original core mapped IO snapshots at VBlank return. WAITCNT configuration data for offline cycle recovery; mapped dynamic IO values can be cached and are not instruction-entry timing or active DMA ownership. No faithful cadence policy inferred.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in (d/'busTimingLeft.cpu-timing-trace.json',root/'tools/capture_original_libretro.py')}}
(root/'reports/original-bus-timing-observation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(fields));print('PASS all1201 prior CPU and packed gameplay records exact')
