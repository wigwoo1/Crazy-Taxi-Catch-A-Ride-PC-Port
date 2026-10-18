"""Check source relationships, including real ambiguous/missing routes."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from extract_passengers import extract

data = Path(sys.argv[1]).read_bytes()
for scene, expected in [('city1', (128, 75, 33)), ('city2', (128, 81, 27)), ('startup', (0, 0, 0))]:
    result = extract(data, scene)
    candidates = [p for p in result['pedestrians'] if p['customer_candidate']]
    assert (len(result['pedestrians']), len(candidates), len(result['destinations'])) == expected
    routes = {r['index']: r for r in result['routes']}
    for p in candidates:
        for index in p['matching_route_indices']:
            assert routes[index]['points_raw'][0][1:3] == p['position_xyz'][::2]
    if scene == 'city1':
        assert [p['index'] for p in candidates if not p['matching_route_indices']] == [104]
        assert [p['index'] for p in candidates if len(p['matching_route_indices']) > 1] == [51, 52, 53]
    elif scene == 'city2':
        assert all(len(p['matching_route_indices']) == 1 for p in candidates)
try:
    extract(data[:-1], 'city1')
except ValueError:
    pass
else:
    raise AssertionError('Wrong ROM accepted')
print('Passenger source relationship checks passed; endpoint eligibility remains unported.')
