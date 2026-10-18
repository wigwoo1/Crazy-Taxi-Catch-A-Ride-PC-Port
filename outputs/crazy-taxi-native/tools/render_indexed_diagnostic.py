"""Display native indexed diagnostics using original scene RGB555 palette.
Source-phase capture uses producer depth0/draw depth=far; partial capture is
an artificial producer-far shading stress fixture. Background/panel fidelity pending.
"""
import pathlib,struct
from png_writer import write_png
root=pathlib.Path('outputs/crazy-taxi-native')
for city,variant in ((city,variant) for city in (1,2) for variant in ("partial","source-phase")):
    pixels=(root/f'reports/city{city}-{variant}-indexed.raw').read_bytes()
    palette=struct.unpack('<256H',(root/f'assets/city{city}-palette.rgb555').read_bytes())
    assert len(pixels)==38400
    colors=[bytes([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255]) for color in palette]
    rgba=b''.join(colors[index] for index in pixels)
    write_png(root/f'reports/city{city}-{variant}-indexed.png',240,160,rgba)
    print('Rendered original-size native diagnostic',city,variant)
