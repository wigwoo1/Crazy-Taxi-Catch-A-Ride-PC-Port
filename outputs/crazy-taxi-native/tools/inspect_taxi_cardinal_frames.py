"""Lossless decoded-frame inspection sheet; no asset edits or redraws."""
import pathlib,json,hashlib
from PIL import Image,ImageDraw
root=pathlib.Path('outputs/crazy-taxi-native');manifest=json.loads((root/'assets/taxis/taxis.json').read_text())
frames=(0,16,32,48);sheet=Image.new('RGB',(1024,640),(30,30,30));draw=ImageDraw.Draw(sheet);records=[]
for driver,record in enumerate(manifest['characters']):
 raw=(root/'assets/taxis'/record['pixels']).read_bytes();assert hashlib.sha256(raw).hexdigest()==record['sha256']
 for col,frame in enumerate(frames):
  payload=raw[frame*32768:(frame+1)*32768];assert len(payload)==32768
  image=Image.frombytes('RGBA',(128,64),payload).resize((256,128),Image.Resampling.NEAREST)
  x=col*256;y=driver*160;draw.text((x+8,y+4),f'Driver {driver}, ROM frame {frame}',fill='white');sheet.paste(image,(x,y+24),image)
  records.append({'driver':driver,'frame':frame,'rgba_sha256':hashlib.sha256(payload).hexdigest()})
sheet.save(root/'reports/taxi-cardinal-source-sheet.png')
(root/'reports/taxi-cardinal-source-sheet.json').write_text(json.dumps({'source':'verified extracted ROM frames, nearest-neighbor2x, labels only','records':records,'scope':'Visual anatomy aid; does not label front/rear or prove camera/motion fidelity.'},indent=2))
print('PASS16 labelled source frames; all4 asset hashes verified.')
