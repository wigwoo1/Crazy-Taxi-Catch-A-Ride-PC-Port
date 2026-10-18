"""Original B6EC slot initialization slice; native null pointer -> -1 index."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_reset_rom.py').read().split('\nseeds=')[0])
for i in md.disasm(r[0xb7aa:0xb7c6],0x0800b7aa):allowed_thumb[i.address]=bytes(i.bytes)
rows=subprocess.run([str(root/'bin/effect-session-slots-rom-compare.exe')],text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==16;steps=0
for seed,row in enumerate(rows):
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.pool=bytearray((seed+i*37)&255 for i in range(704));before=bytes(p.pool);p.reg[1]=0
 p.run(entry=0x0800b7aa,stop=0x0800b7c6);parts=row.split();assert bytes(p.pool[:648])==bytes.fromhex(parts[0]),seed
 original_slots=[p.read(0x02005ee8+i*4) for i in range(12)];assert original_slots==[0]*12,seed
 mapped=[-1 if v==0 else (v-0x02005c60)//72 for v in original_slots];assert mapped==list(map(int,parts[1:])),seed
 assert p.pool[696:]==before[696:],seed;steps+=p.steps
report={'status':'PASS','cases':16,'actual_thumb_instructions':steps,'scope':'B6EC B7AA..B7C4 exact twelve slot-pointer clears, source-null to native-1 mapping, all648records and8tailbytes retained. RestofB6EC/session initializer excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/effect-session-slots-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
