"""Original E388 sequential credits allocation, clean BG registry."""
exec(open('outputs/crazy-taxi-native/tools/audit_credits_layer_palette_calls.py').read().split('\np=InitProbe')[0])
reg=json.loads((root/'reports/palette-registration-full-cfg.json').read_text())['registration16'];assert not reg['outgoing_boundaries']
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in reg['instructions']})
p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
for i in reg['instructions']:
 if 'literal' in i:
  a=int(i['literal'],16)
  if 0x2000000<=a<0x2040000:
   for off in range(0,64,4):p.write((a&~3)+off,0)
rows=[];steps=0
for bank,pal in enumerate((0x80be7ec,0x80b927c,*[0x80be7ec+32*k for k in range(5)])):
 p.reg[:3]=[pal,32,1];p.reg[14]=0x10020001;p.run(entry=0x800e388,stop=0x10020000)
 assert p.read(0x2001782,2)==32*(bank+1) and signed(p.reg[0])==bank
 steps+=p.steps;rows.append({'bank':bank,'source':hex(pal),'allocation_bytes':32*(bank+1)})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'allocations':rows,'thumb_instructions':steps,'text_prefix0_bank':4,'text_prefix1_bank':5,'scope':'Actual E388 seven sequential BG palette allocations, clean offset/table fields. First two ordered callers qualified by original5778 descriptor probe, nextfive explicit243CE..243FC pointer arguments. DMA completion without pixel-copy/hardware; proves returned bank indices and allocation byte counts. Does not prove credits wholebootstrap clean state, fade palette contents or later overwrite/pixel oracle.','source_sha256':hashlib.sha256((root/'tools/audit_credits_palette_banks_rom.py').read_bytes()).hexdigest()}
(root/'reports/credits-palette-bank-rom-audit.json').write_text(json.dumps(report,indent=2));print('PASS',report)
