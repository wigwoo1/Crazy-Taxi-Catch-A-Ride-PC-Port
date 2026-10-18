"""BG0-only settled driver-selection crops; no OBJ/font completion claim."""
import pathlib, json, hashlib
from png_writer import write_png
root=pathlib.Path('outputs/crazy-taxi-native/assets/driver-selection')
meta=json.loads((root/'background.json').read_text())
pixels=(root/'background.rgba').read_bytes()
assert meta['width']==meta['height']==512 and len(pixels)==512*512*4
assert hashlib.sha256(pixels).hexdigest()==meta['rgba_sha256']
panels=[]
for driver in range(4):
    x=256*(driver&1);y=256*(driver//2)
    rgba=b''.join(pixels[((y+row)*512+x)*4:((y+row)*512+x+240)*4] for row in range(160))
    assert len(rgba)==240*160*4
    (root/f'panel-{driver}.rgba').write_bytes(rgba)
    write_png(root/f'panel-{driver}.png',240,160,rgba)
    panels.append(dict(driver_index=driver,scroll=[x,y],rgba_sha256=hashlib.sha256(rgba).hexdigest()))
(root/'panels.json').write_text(json.dumps(dict(rom_sha256=meta['rom_sha256'],descriptor=meta['descriptor'],
    viewport=[240,160],source='0801cc06 initial scroll;0801ce2c..54 and0801ce92..ba selected scroll',
    qualification='Settled BG0-only panels. Original character OBJ, labels, blend/palette ownership and scrolling transition still need integration.',panels=panels),indent=2)+'\n')
print('Original driver BG0: four settled240x160 panels,153600 bytes each; foreground incomplete.')
