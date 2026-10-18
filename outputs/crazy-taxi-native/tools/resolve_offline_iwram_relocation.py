from pathlib import Path
import json,struct,hashlib,collections
r=Path('outputs/crazy-taxi-native');d=r/'reports/original-core-reference';mapping=json.loads((r/'reports/offline-iwram-window-mapping.json').read_text());cpu=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5';delta=0x50006f8;exact=patched=0;patches=[];nonunique={x['tick']:x['matches'] for x in mapping['unmapped']}
for x in cpu:
 if not x['iwram_window']:continue
 live=bytes.fromhex(x['iwram_window']);base=x['iwram_window_base'];source_base=base+delta;offset=source_base-0x8000000;original=rom[offset:offset+64];assert len(original)==64
 differing=[i for i in range(0,64,4) if live[i:i+4]!=original[i:i+4]]
 if not differing:exact+=1;continue
 assert len(differing)==1
 i=differing[0];source_pointer=struct.unpack_from('<I',original,i)[0];live_pointer=struct.unpack_from('<I',live,i)[0];assert source_base+i==0x8002d0c and source_pointer==0x8001dc4 and live_pointer==0x30016cc and source_pointer-live_pointer==delta
 canonical=bytearray(live);canonical[i:i+4]=original[i:i+4];assert bytes(canonical)==original
 patched+=1;patches.append(dict(tick=x['tick'],saved_pc=hex(x['gprs'][15]),rom_literal_address=hex(source_base+i),source_pointer=hex(source_pointer),live_pointer=hex(live_pointer)))
assert exact==762 and patched==7 and exact+patched==769
assert collections.Counter(nonunique.values())=={'multiple':113,'none':7}
report=dict(status='PASS_ALL_SAMPLED_IWRAM_WINDOWS_RESOLVED',sampled_iwram_windows=exact+patched,exact_at_established_relocation=exact,windows_with_one_verified_pointer_relocation=patched,previous_nonunique_matches=113,previous_no_match_samples=7,rom_minus_iwram=hex(delta),patches=patches,scope='All769 sampled64byteIWRAM windows resolve at one establishedROM-RAMoffset:762 byte-exact,7 differ only in one literal pointer relocated from08001DC4 to030016CC.113 previously ambiguous windows matched multiple ROM ranges; they were not nonmatching. No executable-instruction changes observed in these windows; pointer-literal classification is based on exact relocated addresses, not a decoded opcode. Does not establish fullcopierentry/function identities, all RAM code mutations, function cycles or general native cadence.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),d/'cpuWindowsLeft.cpu-timing-trace.json',r/'reports/offline-iwram-window-mapping.json']})
(r/'reports/offline-iwram-relocation-resolution.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS all769 RAMwindows:762 exact,7 one relocated pointer;113 earlier ambiguity resolved')
