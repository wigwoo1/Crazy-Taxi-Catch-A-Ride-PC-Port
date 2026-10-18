from pathlib import Path
import json,hashlib,collections
root=Path('outputs/crazy-taxi-native');d=root/'reports/original-core-reference';rows=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];old=json.loads((d/'cpuTimingLeft.cpu-timing-trace.json').read_text())['records'];packed=json.loads((d/'cpuWindowsLeft.packed-driving-trace.json').read_text())['records'];assert packed==json.loads((d/'longsourceleft.packed-driving-trace.json').read_text())['records'];assert len(rows)==len(old)==1201
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5';mapped=[];unmapped=[];cache={}
for row,previous in zip(rows,old):
 assert {k:row[k] for k in previous}==previous
 if not row['iwram_window']:continue
 data=bytes.fromhex(row['iwram_window']);assert len(data)==64
 if data not in cache:
  first=rom.find(data);second=rom.find(data,first+1) if first>=0 else -1;cache[data]=(first,second)
 first,second=cache[data]
 if first>=0 and second<0:
  mapped.append(dict(tick=row['tick'],saved_pc=hex(row['gprs'][15]),iwram_window_base=hex(row['iwram_window_base']),rom_window_base=hex(0x8000000+first),rom_minus_iwram=hex(0x8000000+first-row['iwram_window_base']),bytes=data.hex()))
 else:unmapped.append(dict(tick=row['tick'],saved_pc=hex(row['gprs'][15]),matches='none' if first<0 else 'multiple'))
report=dict(status='COMPLETED_OFFLINE_IWRAM_WINDOW_MAPPING',samples=len(rows),packed_and_prior_cpu_records_unchanged=len(rows),iwram_windows=len(mapped)+len(unmapped),unique_exact64byte_rom_matches=len(mapped),not_uniquely_mapped=len(unmapped),relocation_differences=dict(collections.Counter(x['rom_minus_iwram'] for x in mapped)),mapped=mapped,unmapped=unmapped,scope='Captured actual64byteIWRAM windows around savedPC at offlineVBlankreturns. Matches are byte-exact unique ROM ranges, not wholefunction identity or cycle cost; savedPCpipeline/IRQsampling still qualified. No runtimeCPUemulation or guessed source-physics policy. All1201packed and priorCPU snapshot fields unchanged.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),root/'tools/capture_original_libretro.py',d/'cpuWindowsLeft.cpu-timing-trace.json',d/'cpuWindowsLeft.packed-driving-trace.json']})
(root/'reports/offline-iwram-window-mapping.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k not in ['mapped','unmapped','scope','hashes']})
