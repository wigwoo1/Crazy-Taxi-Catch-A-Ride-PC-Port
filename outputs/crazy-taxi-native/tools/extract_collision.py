"""Recover the ground-query cell index consumed by 08029664.

The first 24 bytes of each cell and high-bit references remain uninterpreted.
This exports collision candidates, not a finished collision solver.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from extract_scene import BASE, EXPECTED, SCENES


def extract(data, scene):
    if hashlib.sha256(data).hexdigest() != EXPECTED:
        raise ValueError('Unsupported ROM revision')

    def region(address, size):
        offset = address - BASE
        if offset < 0 or size < 0 or offset + size > len(data):
            raise ValueError('Collision reference outside ROM')
        return data[offset:offset + size]

    header = region(SCENES[scene], 104)
    nx, nz = struct.unpack_from('<HH', header, 0x3c)
    sx, sz = struct.unpack_from('<ii', header, 0x40)
    ox, oy, oz = struct.unpack_from('<iii', header, 0x48)
    polygons = struct.unpack_from('<H', header, 0x0a)[0]
    table = struct.unpack_from('<I', header, 0x60)[0]
    if not nx or not nz or sx <= 0 or sz <= 0:
        raise ValueError('Invalid collision grid dimensions')
    pointers = struct.unpack('<%dI' % (nx * nz), region(table, nx * nz * 4))
    cells, flagged = [], 0
    for pointer in pointers:
        if not pointer:
            cells.append(None)
            continue
        raw = region(pointer, 28)
        count = struct.unpack_from('<I', raw, 24)[0]
        refs = list(struct.unpack('<%dH' % count, region(pointer + 28, count * 2)))
        if any(r >= polygons for r in refs if not r & 0x8000):
            raise ValueError('Ground cell polygon outside scene')
        flagged += sum(bool(r & 0x8000) for r in refs)
        cells.append((raw[:24], refs))
    metadata = dict(scene=scene, rom_sha256=EXPECTED, grid_width=nx, grid_depth=nz,
                    cell_size=[sx, sz], origin=[ox, oy, oz],
                    populated_cells=sum(c is not None for c in cells),
                    references=sum(len(c[1]) for c in cells if c is not None),
                    flagged_references=flagged,
                    status='Verified ground-query candidate grid; narrow-phase and wall response not ported.')
    # CTC1: dimensions, sizes, origin; then one record per row-major cell.
    # Empty: ffffffff. Present: count, opaque 24 bytes, count uint16 refs.
    out = bytearray(b'CTC1' + struct.pack('<II5i', nx, nz, sx, sz, ox, oy, oz))
    for cell in cells:
        if cell is None:
            out += struct.pack('<I', 0xffffffff)
        else:
            raw, refs = cell
            out += struct.pack('<I', len(refs)) + raw + struct.pack('<%dH' % len(refs), *refs)
    return bytes(out), metadata


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    data = args.rom.read_bytes()
    args.output.mkdir(parents=True, exist_ok=True)
    for scene in SCENES:
        binary, metadata = extract(data, scene)
        (args.output / (scene + '.ctc')).write_bytes(binary)
        (args.output / (scene + '-collision.json')).write_text(json.dumps(metadata, indent=2) + '\n')
        print(json.dumps(metadata))
