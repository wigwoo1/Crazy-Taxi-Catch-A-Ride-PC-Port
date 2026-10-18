"""Export original pedestrian/route/destination relationships for recovery.

Diagnostic data only: endpoint eligibility, pickup state machine and fares are
not implemented. Unknown record fields are retained without invented meanings.
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
            raise ValueError('Passenger reference outside ROM')
        return data[offset:offset + size]

    header = region(SCENES[scene], 104)
    count, path_count, destination_count = struct.unpack_from('<3H', header, 14)
    records, paths, destinations = struct.unpack_from('<3I', header, 48)
    routes = []
    for index in range(path_count):
        row = region(paths + index * 20, 20)
        if row[4:8] != b'PED_':
            continue
        point_count, = struct.unpack_from('<I', row)
        address, = struct.unpack_from('<I', row, 16)
        if not point_count:
            raise ValueError('Empty pedestrian route')
        points = [list(p) for p in struct.iter_unpack('<4i', region(address, point_count * 16))]
        routes.append({'index': index, 'address': hex(paths + index * 20),
                       'name': row[4:16].split(b'\0')[0].decode('ascii'),
                       'points_address': hex(address), 'points_raw': points})
    dest = []
    for index in range(destination_count):
        row = region(destinations + index * 48, 48)
        dest.append({'index': index, 'address': hex(destinations + index * 48),
                     'raw_hex': row.hex(), 'target_xz': [struct.unpack_from('<i', row, k)[0] for k in (24, 32)],
                     'accepted_material_slots': list(struct.unpack_from('<4H', row, 40))})
    pedestrians = []
    for index in range(count):
        row = region(records + index * 32, 32)
        x, y, z = struct.unpack_from('<3i', row, 4)
        customer = row[3] <= 9
        if customer and row[1] >= destination_count:
            raise ValueError('Invalid customer destination index')
        matching = [r['index'] for r in routes if r['points_raw'][0][1:3] == [x, z]]
        pedestrians.append({'index': index, 'address': hex(records + index * 32),
                            'raw_hex': row.hex(), 'kind': row[3],
                            'customer_candidate': customer, 'destination_index': row[1],
                            'position_xyz': [x, y, z], 'matching_route_indices': matching})
    return {'scene': scene, 'rom_sha256': EXPECTED,
            'status': 'Diagnostic source data; customer eligibility and gameplay not yet ported.',
            'evidence': {'initializer': '080227ac..08022cc6',
                         'customer_kind_gate': '08022812..08022816: byte3 <= 9',
                         'route_match': '08022856..0802288c: PED_ prefix and first point X/Z',
                         'destination_stride': '08022b38..08022b56: 48 bytes, target X/Z at 24/32'},
            'pedestrians': pedestrians, 'routes': routes, 'destinations': dest}


def write_asset(result, path):
    """CTP1: raw source/dest records, ordered PED_ routes with raw points."""
    p,r,d=result['pedestrians'],result['routes'],result['destinations']
    with path.open('wb') as out:
        out.write(struct.pack('<4s3I',b'CTP1',len(p),len(r),len(d)))
        for row in p:out.write(bytes.fromhex(row['raw_hex']))
        for row in d:out.write(bytes.fromhex(row['raw_hex']))
        for row in r:
            out.write(struct.pack('<2I',row['index'],len(row['points_raw'])))
            for point in row['points_raw']:out.write(struct.pack('<4i',*point))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--assets', type=Path)
    args = parser.parse_args()
    data = args.rom.read_bytes()
    args.output.mkdir(parents=True, exist_ok=True)
    if args.assets:args.assets.mkdir(parents=True,exist_ok=True)
    for scene in SCENES:
        result = extract(data, scene)
        if args.assets:write_asset(result,args.assets/(scene+'.ctp'))
        (args.output / (scene + '-passengers.json')).write_text(json.dumps(result, indent=2) + '\n')
        candidates = [p for p in result['pedestrians'] if p['customer_candidate']]
        print(scene, 'pedestrians', len(result['pedestrians']), 'customer candidates', len(candidates),
              'routes', len(result['routes']), 'destinations', len(result['destinations']),
              'candidates without route', sum(not p['matching_route_indices'] for p in candidates))


if __name__ == '__main__':
    main()

