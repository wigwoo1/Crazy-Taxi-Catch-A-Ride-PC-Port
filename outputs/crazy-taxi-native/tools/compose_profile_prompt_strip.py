"""Initial profile BG plus source secondary-queue semitransparent strip; no UI claim."""
import pathlib, struct, hashlib, json
from extract_guidance import EXPECTED
from png_writer import write_png
r = pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest() == EXPECTED
root = pathlib.Path('outputs/crazy-taxi-native/assets')
out = root / 'profile'
front = (out / 'background.indices').read_bytes()
back = (root / 'profile-layer1/background.indices').read_bytes()
indices = (out / 'prompts.indices').read_bytes()
assert len(front) == len(back) == 512 * 256 and len(indices) == 1152
assert not any(front[y * 512 + x] for y in range(160) for x in range(240))
bgpal = struct.unpack_from('<256H', r, 0xb458c)
objpal = struct.unpack_from('<16H', r, 0x8aaa0)
pixels = bytearray((out / 'background-composite.rgba').read_bytes())
assert len(pixels) == 240 * 160 * 4
blended = opaque = 0
layers = [(bgpal[back[y*512+x]] if back[y*512+x] else bgpal[0], 0, False) for y in range(160) for x in range(240)]
# Callback normal profile fixture: five32px tiles and8px end, y144.
# Secondary queue appended after main OBJ queue; main text/icons overlay this.
for x0, w, offset in [(x * 32, 32, 512) for x in range(5)] + [(160, 8, 1024)]:
    for y in range(16):
        for x in range(w):
            index = indices[offset + y * w + x]
            if not index:
                continue
            bgindex = back[(144 + y) * 512 + x0 + x]
            color = objpal[index]
            layers[(144+y)*240+x0+x] = (color, bgpal[bgindex], bool(bgindex))
            # BLDCNT1f4f second targets BG0..3/OBJ, excluding backdrop.
            if bgindex:
                color = sum(min(31, (((color >> s) & 31) + ((bgpal[bgindex] >> s) & 31)) // 2) << s for s in (0, 5, 10))
                blended += 1
            else:
                opaque += 1
            p = ((144 + y) * 240 + x0 + x) * 4
            pixels[p:p + 4] = bytes([((color >> s) & 31) * 255 // 31 for s in (0, 5, 10)] + [255])
(out / 'background-prompts.rgba').write_bytes(pixels)
write_png(out / 'background-prompts.png', 240, 160, pixels)
(out / 'background-prompts.json').write_text(json.dumps(dict(rom_sha256=EXPECTED, queue_merge='08027e74..08027ebe', strip_helper='0801af96', callback_fixture='normal five tiles, startX0', blended_pixels=blended, opaque_over_backdrop=opaque, rgba_sha256=hashlib.sha256(pixels).hexdigest(), qualification='BG and strip only; foreground native prompts/editor/fade and dynamic callback variants pending.'), indent=2) + '\n')
print(f'Composed original profile strip: {blended} blended pixels, {opaque} opaque over backdrop.')

# Source palette cap is applied BEFORE display alpha mixing, never after.
phases = bytearray()
for cap in range(32):
    for a, b, mix in layers:
        channels = [((min((a >> shift) & 31, cap) + min((b >> shift) & 31, cap)) // 2 if mix else min((a >> shift) & 31, cap)) * 255 // 31 for shift in (0, 5, 10)]
        phases.extend(channels + [255])
assert phases[-153600:] == pixels and not any(phases[i] for i in range(153600) if i%4 != 3)
(out / 'background-fade.rgba').write_bytes(phases)
print('Recovered32 source palette-before-blend background phases:',len(phases),'bytes.')
