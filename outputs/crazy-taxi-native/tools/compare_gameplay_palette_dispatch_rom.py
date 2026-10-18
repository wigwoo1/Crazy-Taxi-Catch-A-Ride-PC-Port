"""Original display palette request dispatcher against native ownership state."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0x26320:0x26348],0x8026320):allowed_thumb[i.address]=bytes(i.bytes)
class RequestProbe(DispatchProbe):
 def external(self,target):
  assert target==0x8022508,hex(target)
  assert self.reg[0] in (0,1),self.reg[0]
  self.transfers.append(self.reg[0])
cp=subprocess.run([str(root/'bin/gameplay-palette-dispatch-rom-compare.exe')],text=True,capture_output=True,check=True)
rows=[list(map(int,row.split())) for row in cp.stdout.splitlines()];assert len(rows)==512
steps=0
for before,value,request,alternate,mode in rows:
 p=RequestProbe((0,0,0,0,0,1,0,0,0,0));p.transfers=[];p.write(0x2005f24,value,1)
 p.run(entry=0x8026320,stop=0x8026348)
 expected_mode=p.transfers[0] if p.transfers else -1
 expected_alternate=expected_mode if p.transfers else before
 assert [request,alternate,mode]==[p.read(0x2005f24,1),expected_alternate,expected_mode],(before,value)
 assert len(p.transfers)<=1;steps+=p.steps
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':512,'original_thumb_instructions':steps,'equal':True,'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/palette_fade.c','src/palette_fade.h','tests/gameplay_palette_dispatch_rom_compare.c','tools/compare_gameplay_palette_dispatch_rom.py']},'scope':'Actual original26320..26346 to26348, all256request bytes/two prior alternate states. Transfer22508 explicit captured mode boundary; native dispatch request clearing/palette ownership compared. Prior16208 and laterdisplay swap excluded; source request producers still require controller integration.'}
(root/'reports/gameplay-palette-dispatch-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS512 original/native palette requests, clearing and retained colour state;',steps,'Thumb instructions')
