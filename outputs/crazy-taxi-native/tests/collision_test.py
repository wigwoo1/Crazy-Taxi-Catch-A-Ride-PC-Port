"""Compare every exported cell against its original ROM record."""
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from extract_collision import extract
from extract_scene import SCENES, BASE

data = Path(sys.argv[1]).read_bytes()
for name, address in SCENES.items():
    binary, metadata = extract(data, name)
    h = address - BASE
    nx, nz = struct.unpack_from('<HH', data, h + 0x3c)
    table = struct.unpack_from('<I', data, h + 0x60)[0] - BASE
    assert binary[:4] == b'CTC1'
    assert struct.unpack_from('<II', binary, 4) == (nx, nz)
    cursor = 32
    nonempty = 0
    for index in range(nx * nz):
        pointer = struct.unpack_from('<I', data, table + index * 4)[0]
        count = struct.unpack_from('<I', binary, cursor)[0]
        cursor += 4
        if pointer == 0:
            assert count == 0xffffffff
            continue
        nonempty += 1
        p = pointer - BASE
        assert count == struct.unpack_from('<I', data, p + 24)[0]
        assert binary[cursor:cursor+24] == data[p:p+24]
        cursor += 24
        assert binary[cursor:cursor+count*2] == data[p+28:p+28+count*2]
        cursor += count * 2
    assert cursor == len(binary)
    assert nonempty == metadata['populated_cells']
    print(name, 'all', nx*nz, 'cell records match ROM')
try:
    extract(data[:-1], 'city1')
except ValueError:
    pass
else:
    raise AssertionError('Modified ROM accepted')
