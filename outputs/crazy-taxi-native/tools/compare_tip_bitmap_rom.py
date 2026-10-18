"""Original31EF8 indexed framebuffer oracle for tip images."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/tip-bitmap-copy-cfg.json').read_text())['instructions']})
class BitmapProbe(UpdateProbe):
 def external(self,target):raise AssertionError(('Unexpected bitmap external',hex(target)))
bases=[0x80ea67c,0x80e9474,0x80e826c];assets=(root/'assets/hud/tips-indexed.bin').read_bytes();assert len(assets)==12288
for kind,base in enumerate(bases):assert assets[kind*4096:(kind+1)*4096]==r[base-0x8000000:base-0x8000000+4096]
cases=[(112,y,k,f) for y in [17,140] for k in range(3) for f in range(16)]
cases += [(x,y,1,5) for x,y in [(-8,17),(235,17),(112,155),(235,155),(-16,17),(240,17),(112,160),(112,0)]]
output=bytearray(struct.pack('<I',len(cases)));steps=changed=0
for x,y,kind,frame in cases:
 p=BitmapProbe((0,0,0,0,0,1,0,0,0,0));fb=0x10000000
 for i in range(0,38400,4):p.write(fb+i,0x6f6f6f6f)
 p.write(0x2007550,fb);p.reg[:4]=[x&MASK,y&MASK,0x100010,bases[kind]+frame*256];p.write(p.reg[13],1)
 p.run(entry=0x8031ef8,stop=0x10020000)
 actual=bytes(p.read(fb+i,1) for i in range(38400));expected=bytearray([111])*38400;image=assets[kind*4096+frame*256:kind*4096+(frame+1)*256]
 for dy in range(16):
  for dx in range(16):
   if 0<=x+dx<240 and 0<=y+dy<160 and image[dy*16+dx]:expected[(y+dy)*240+x+dx]=image[dy*16+dx]
 assert actual==expected,(x,y,kind,frame);steps+=p.steps;changed+=sum(a!=111 for a in actual)
 output+=struct.pack('<iiii',x,y,kind,frame)+actual
(root/'reports/tip-bitmap-oracles.bin').write_bytes(output)
report=dict(cases=len(cases),thumb_instructions=steps,pixels_compared=len(cases)*38400,changed_pixels=changed,match=True,scope='Completeoriginal31EF8 queue1 throughreturn vsnative texture nearest indexed copy semantics: all48class/frame assets at two valid tip Y anchors plus8horizontal/bottomclip/offscreen cases. Constant111 framebuffer explicit; negativeY notcovered; palette/GPU/fullHUD notyetproved.',rom_sha256=hashlib.sha256(r).hexdigest(),oracle_sha256=hashlib.sha256(output).hexdigest())
(root/'reports/tip-bitmap-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
