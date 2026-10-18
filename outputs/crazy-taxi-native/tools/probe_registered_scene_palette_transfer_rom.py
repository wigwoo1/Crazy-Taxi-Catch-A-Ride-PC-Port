"""Actual original22508 direct/alternate transfer on registered scene bank0."""
exec(open('outputs/crazy-taxi-native/tools/probe_scene_bg_palette_fade_rom.py').read().split('\np=FadeProbe')[0])
transfer=json.loads((root/'reports/marker-palette-transfer-audit.json').read_text())['instructions']
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in transfer})
results=[]
for name,scene in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
 p=FadeProbe((0,0,0,0,0,1,0,0,0,0));p.dma=[]
 for i in cfg+rows+transfer:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000 or 0x3000000<=a<0x3010000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for a in range(0x5000000,0x5000400,4):p.write(a,0)
 palette=struct.unpack_from('<I',r,scene+24)[0]
 p.reg[:3]=[palette,32,1];p.reg[14]=0x10020001;p.run(entry=0x800e154,stop=0x10020000)
 source_colors=r[palette-0x8000000:palette-0x8000000+512];cases=[]
 for mode in [0,1,0,1]:
  p.steps=0;before=len(p.dma);p.reg[0]=mode;p.reg[14]=0x10020001;p.run(entry=0x8022508,stop=0x10020000)
  actual=bytes(p.read(0x5000000+k,1) for k in range(512));assert actual==source_colors,(name,mode,'scene BG changed')
  if mode==0:assert p.dma[before:]==[{'source':hex(palette),'destination':'0x5000000','bytes':512}],p.dma[before:]
  else:assert len(p.dma)==before,p.dma[before:]
  cases.append({'mode':mode,'instructions':p.steps,'dma':p.dma[before:],'scene_bg_equal':True})
 results.append({'scene':name,'registered_scene_palette':hex(palette),'cases':cases})
normal=json.loads((root/'reports/normal-loop-cfg.json').read_text())
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'scenes':results,'scope':'Actual E154 registration then22508 modes0/1/0/1, cleanbank0 scene registration, explicit512byte synchronousDMA contract. Mode0 copies original512 scene colors, mode1 skips bank0 and retains them. Multi-bank alternate transforms and fullsharedbootstrap/laterreset orrecordmutation unexecuted.','source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['tools/probe_registered_scene_palette_transfer_rom.py','tools/probe_scene_bg_palette_fade_rom.py','tools/probe_scene_bg_palette_registration_rom.py']},'display_handoff':'Original26318 reads signed02005F24:2 calls22508(1),1 calls22508(0),then clearsflag; normal-loop CFG calls26318.'}
(root/'reports/registered-scene-palette-transfer-rom-probe.json').write_text(json.dumps(report,indent=2));print('PASS3 scenes/12 persistent transfer calls, scene512bytes exact, alternatebank0 untouched')
