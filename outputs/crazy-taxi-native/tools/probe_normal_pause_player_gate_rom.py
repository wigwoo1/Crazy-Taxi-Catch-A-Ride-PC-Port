"""Original ordinary freeze branch reaches camera without driver/effect owners."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_movement_gate_rom.py').read().split('\nvalues=')[0])
rows=json.loads((root/'reports/normal-loop-cfg.json').read_text())['instructions']
for i in rows:
 a=int(i['address'],16)
 if 0x800afd4<=a<=0x800affc or 0x800b27c<=a<=0x800b312:allowed_thumb[a]=bytes.fromhex(i['bytes'])
steps=0
for freeze in range(256):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0))
 p.write(0x2005f20,freeze,1);p.write(0x2001696,0,1);p.write(0x2001697,0,1);p.write(0x2001708,0,1);p.reg[9]=0x2001708
 p.run(entry=0x800afd4,stop={0x800affc,0x800b312})
 assert p.stopped_pc==(0x800b312 if freeze else 0x800affc),(freeze,hex(p.stopped_pc))
 steps+=p.steps
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':256,'thumb_instructions':steps,'equal':True,'scope':'Actual AFD4 freeze-byte branch and B27C..B2DC paused continuation in ordinary mode1708=0/intro1696=0/ended1697=0. Nonzero freeze reachesB312 camera owner without driver/world/body/emitter/effectpool owners; zero freeze reachesordinary live dispatch AFFC. No camera/other callee execution or full normal-frame proof.'}
(root/'reports/normal-pause-player-gate-rom-probe.json').write_text(json.dumps(report,indent=2));print('PASS',report)
