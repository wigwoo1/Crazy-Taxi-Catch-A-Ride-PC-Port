"""Extract results fonts with original group metrics and explicit registration."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent))
from png_writer import write_png
from extract_guidance import EXPECTED
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(r).hexdigest()==EXPECTED
screen=sys.argv[1] if len(sys.argv)>1 else 'results'
config={'attract':(0x5a338,[(0,1,0x4097b8),(1,-1,None)],'0800acce..0800acfc'), 'normal-menu':(0x34634,[(0,0,0x3fd3b4),(1,1,0x4097b8)],'08017546..08017562 / 080177a4..080177c0'), 'loading':(0x70e50,[(0,1,0x415bbc)],'0800ad40..0800ad62'),'results':(0x64f38,[(0,3,None),(1,1,0x4097b8),(2,0,None)],'0802395a..0802398a'),
'name-entry':(0x76d10,[(0,3,0x3fd3b4),(1,1,0x4097b8),(2,0,None)],'08025436..08025464'),
'profile':(0x860bc,[(0,3,0x3fd3b4),(1,1,0x4097b8)],'080246f6..08024718'),
'ranking':(0x60c40,[(0,-2,None)],'080240c6..080240d8')}
b,registrations,source=config[screen];o=pathlib.Path('outputs/crazy-taxi-native/assets')/screen;o.mkdir(exist_ok=True)
for slot,spacing,pair in registrations:
 g=b+struct.unpack_from('<I',r,b+4+slot*4)[0];count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g);assert bits==16
 pal=g+20+count*4;pix=pal+pals*32;rows=[];frames=bytearray();height=((count+15)//16)*32*pals;atlas=bytearray(512*height*4)
 for bank in range(pals):
  vals=struct.unpack_from('<16H',r,pal+bank*32);colors=[bytes([((v>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0]) for i,v in enumerate(vals)]
  for index in range(count):
   a=b+struct.unpack_from('<I',r,g+20+index*4)[0];m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
   assert tx<=4 and ty<=4 and tile+tx*ty<=tiles
   frame=bytearray(32*32*4)
   for y in range(ty*8):
    for x in range(tx*8):
     p=pix+(tile+(y//8)*tx+x//8)*32+y%8*4+x%8//2;idx=(r[p]>>(4*(x&1)))&15;col=colors[idx]
     dst=(y*32+x)*4;frame[dst:dst+4]=col
     sx=(index%16)*32+x;sy=(bank*((count+15)//16)+index//16)*32+y;dst=(sy*512+sx)*4
     atlas[dst:dst+4]=col if idx else bytes((40,40,40,255))
   frames.extend(frame)
   if bank==0:rows.append(dict(index=index,character_byte=index+32,descriptor=hex(a+0x8000000),metrics=list(m)))
 if pair:
  assert struct.unpack_from('<H',r,pair)[0]==count
  kerning=r[pair+2:pair+2+count*count]
 else:kerning=bytes(count*count)
 metrics=b'CTF1'+struct.pack('<HH',count,spacing&65535)+b''.join(struct.pack('<9H',*z['metrics']) for z in rows)+kerning
 (o/f'font{slot}.rgba').write_bytes(frames);(o/f'font{slot}-metrics.ctf').write_bytes(metrics);write_png(o/f'font{slot}-atlas.png',512,height,atlas)
 (o/f'font{slot}.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,registration=source,slot=slot,group=hex(g+0x8000000),count=count,palettes=pals,character_base=32,spacing=spacing,pair_table=hex(pair+0x8000000) if pair else None,pair_table_export='source bytes' if pair else 'zero placeholder for absent pair table; caller must preserve null-table semantics',format='palette-major RGBA8 32x32 cells; source index0 transparent',rgba_sha256=hashlib.sha256(frames).hexdigest(),glyphs=rows),indent=2)+'\n')
 print(f'Font{slot}: {count} glyphs x{pals} palettes, spacing{spacing}, pair table {pair}.')

