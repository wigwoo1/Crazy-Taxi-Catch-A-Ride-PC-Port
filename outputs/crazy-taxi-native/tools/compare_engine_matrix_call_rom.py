exec(open('outputs/crazy-taxi-native/tools/compare_camera_matrix_arm.py').read().split('\nrng=')[0])
root=pathlib.Path('outputs/crazy-taxi-native');rng=random.Random(0x738);cases=[[rng.getrandbits(32) for i in range(32)] for j in range(256)]
rows=subprocess.run([str(root/'bin/engine-matrix-call-rom-compare.exe')],input=''.join(' '.join(f'{v:08x}' for v in c)+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for c,row in zip(cases,rows):
 p=Probe()
 for i,v in enumerate(c):p.write(0x10000000+4*i,v)
 p.run(0x08000738,[0x10000000,0x10000040,0x10000080]);got=[p.read(0x10000080+4*i) for i in range(16)];assert got==[int(v,16) for v in row.split()];steps+=p.steps
report=dict(equal=True,cases=len(cases),actual_arm_instructions=steps,scope='Original738 whole matrix product output versusnative global03006A98 routedQ8 product. Distinctinput/outputbuffers, full32bit randomwrap. Noaliasing/fullengine/graphicsbootstrap proof.',rom_sha256=hashlib.sha256(r).hexdigest());(root/'reports/engine-matrix-call-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
