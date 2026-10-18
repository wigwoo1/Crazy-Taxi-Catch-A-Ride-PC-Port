"""Recover profile prompt icons/strip from original container, without drawing claims."""
import pathlib, struct, hashlib, json
from extract_guidance import EXPECTED
from png_writer import write_png

r = pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest() == EXPECTED
b = 0x49c0c
out = pathlib.Path('outputs/crazy-taxi-native/assets/driver-selection')
pixels = bytearray()
indices_blob = bytearray()
rows = []
for group, indices in ((2, (4, 5)),):
    g = b + struct.unpack_from('<I', r, b + 4 + group * 4)[0]
    count, bits, tiles, start, palettes = struct.unpack_from('<5I', r, g)
    assert bits == 16 and palettes in (1, 2)
    pal = g + 20 + count * 4
    pix = pal + 32 * palettes
    colors = struct.unpack_from('<16H', r, pal)
    for index in indices:
        assert index < count
        a = b + struct.unpack_from('<I', r, g + 20 + index * 4)[0]
        metrics = struct.unpack_from('<9H', r, a)
        tx, ty, ox, oy, cx, cy, ink_w, ink_h, tile = metrics
        assert 0 < tx <= 8 and 0 < ty <= 8 and tile + tx * ty <= tiles
        assert pix + tiles * 32 <= len(r)
        frame = bytearray()
        indices_frame = bytearray()
        for y in range(ty * 8):
            for x in range(tx * 8):
                p = pix + (tile + (y // 8) * tx + x // 8) * 32 + (y % 8) * 4 + (x % 8) // 2
                i = (r[p] >> (4 * (x & 1))) & 15
                indices_frame.append(i)
                color = colors[i]
                frame.extend([((color >> s) & 31) * 255 // 31 for s in (0, 5, 10)] + [255 if i else 0])
        rows.append(dict(group=group, index=index, palette_bank=0, metrics=list(metrics), descriptor=hex(a + 0x8000000), palette=hex(pal + 0x8000000), pixels=hex(pix + 0x8000000), byte_offset=len(pixels), byte_length=len(frame)))
        pixels.extend(frame)
        indices_blob.extend(indices_frame)
        write_png(out / f'prompt-{group}-{index}.png', tx * 8, ty * 8, frame)
(out / 'prompts.rgba').write_bytes(pixels)
(out / 'prompts.indices').write_bytes(indices_blob)
(out / 'prompts.crs').write_bytes(b'CRS1' + struct.pack('<I', len(rows)) + b''.join(struct.pack('<9H', *row['metrics']) for row in rows))
texts = []
base = 0x49dfe0
for i in (0, 1):
    offset = struct.unpack_from('<I', r, base + 4 + i * 4)[0]
    assert offset
    a = base + offset
    text = r[a:r.index(0, a)]
    assert 0 < len(text) < 32 and all(c >= 32 for c in text)
    texts.append(dict(id=i, address=hex(a + 0x8000000), text=text.decode('latin1')))
(out / 'prompts.cpr').write_bytes(b'CPP1' + b''.join(struct.pack('<HH', row['id'], len(row['text'])) + row['text'].encode('latin1') for row in texts))
(out / 'prompts.json').write_text(json.dumps(dict(rom_sha256=EXPECTED, container='08049c0c', frames_sha256=hashlib.sha256(pixels).hexdigest(), frames=rows, texts=texts, qualification='Original driver group2 icons4/5 and localized labels0/1, main queue/priority0. Footer strip and palette composition pending.'), indent=2) + '\n')
print(f'Recovered {len(rows)} source prompt frames ({len(pixels)} bytes) and two localized labels.')
