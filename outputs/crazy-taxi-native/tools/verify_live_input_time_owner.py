import pathlib,json
r=pathlib.Path('outputs/crazy-taxi-native/reports');host=[list(map(int,x.split())) for x in (r/'input-owner-host.txt').read_text().splitlines()];samples=[list(map(int,x.split())) for x in (r/'input-owner-samples.txt').read_text().splitlines()];expected=[];last=0
for n,row in enumerate(host):
 if row[7]>last:
  assert row[7]==last+1
  expected.append([1,1] if n==0 else [host[n-1][4],host[n-1][2]])
 last=row[7]
assert len(samples)==len(expected)==500
assert all(a[:2]==b for a,b in zip(samples,expected))
assert any(a[0]!=a[1] for a in samples)
report={'status':'PASS_LIVE_PRIOR_INPUT_POST_PHYSICS_OWNER','updates':len(samples),'different_input_physics_deltas':sum(a[0]!=a[1] for a in samples),'scope':'Actual connected candidate sampled input delta equals prior presentation owner; physics delta equals newly committed owner. Startup smoke only, no physical input or original full-game trajectory equivalence.'};(r/'input-owner-live-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
