from __future__ import annotations
import json, pathlib, sys

ROOT = pathlib.Path('outputs/crazy-taxi-native').resolve()
OUT = ROOT / 'reports' / 'original-core-reference'
FIELDS = {
    'input_keys': lambda r: r.get('input_keys'),
    'keyinput': lambda r: r.get('keyinput'),
    'position': lambda r: r.get('position'),
    'heading': lambda r: r.get('heading'),
    'speed': lambda r: r.get('speed'),
    'counter': lambda r: r.get('counter'),
    'previous': lambda r: r.get('previous'),
    'frame_delta': lambda r: r.get('frame_delta'),
    'delta': lambda r: r.get('delta'),
    'traffic_actor_records': lambda r: (r.get('traffic_actor_records') or {}).get('raw'),
    'traffic_globals': lambda r: r.get('traffic_globals'),
    'traffic_frame_context': lambda r: r.get('traffic_frame_context'),
    'traffic_camera_position': lambda r: (r.get('traffic_camera') or {}).get('position'),
    'route_driver': lambda r: r.get('route_driver'),
}

def load(path: pathlib.Path):
    data = json.loads(path.read_text(encoding='utf-8'))
    if not isinstance(data, list) or not data or not all(isinstance(r, dict) and 'tick' in r for r in data):
        raise ValueError(f'not a nonempty actor trace: {path}')
    return data

def actor_record_differences(base_hex, resumed_hex):
    if not isinstance(base_hex, str) or not isinstance(resumed_hex, str) or len(base_hex) != len(resumed_hex) or len(base_hex) != 50 * 92 * 2:
        return None
    slots = []
    for slot in range(50):
        a = bytes.fromhex(base_hex[slot * 92 * 2:(slot + 1) * 92 * 2])
        b = bytes.fromhex(resumed_hex[slot * 92 * 2:(slot + 1) * 92 * 2])
        offsets = [i for i, (left, right) in enumerate(zip(a, b)) if left != right]
        if offsets:
            slots.append({'slot': slot, 'differing_byte_offsets': offsets, 'baseline_hex': a.hex(), 'resumed_hex': b.hex()})
    return slots

def compare(baseline_path: pathlib.Path, resumed_path: pathlib.Path, start_tick: int):
    baseline = load(baseline_path); resumed = load(resumed_path)
    by_tick = {r['tick']: r for r in baseline}
    frames = []; first_divergence = None; first_missing_tick = None
    for offset, resumed_row in enumerate(resumed):
        source_tick = start_tick + offset
        source_row = by_tick.get(source_tick)
        if source_row is None:
            first_missing_tick = source_tick
            break
        differences = [name for name, getter in FIELDS.items() if getter(source_row) != getter(resumed_row)]
        item = {'resumed_index': offset, 'baseline_tick': source_tick, 'resumed_tick': resumed_row['tick'], 'differences': differences}
        if differences and first_divergence is None:
            first_divergence = {'resumed_index': offset, 'baseline_tick': source_tick, 'resumed_tick': resumed_row['tick'], 'fields': differences}
            item['baseline_values'] = {name: FIELDS[name](source_row) for name in differences if name != 'traffic_actor_records'}
            item['resumed_values'] = {name: FIELDS[name](resumed_row) for name in differences if name != 'traffic_actor_records'}
            if 'traffic_actor_records' in differences:
                record_differences = actor_record_differences(FIELDS['traffic_actor_records'](source_row), FIELDS['traffic_actor_records'](resumed_row))
                item['actor_record_differences'] = record_differences
                first_divergence['actor_record_differences'] = record_differences
        frames.append(item)
    matched = sum(not item['differences'] for item in frames)
    status = 'DIVERGENCE' if first_divergence is not None else ('INSUFFICIENT_BASELINE_COVERAGE' if first_missing_tick is not None else 'PASS_EXACT_RESUMED_TRACE')
    return {
        'status': status,
        'scope': 'Offline original-ROM actor-trace comparison only; not a native gameplay proof.',
        'rom_sha256': '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5',
        'baseline': str(baseline_path), 'resumed': str(resumed_path),
        'baseline_start_tick': start_tick, 'resumed_frames': len(resumed), 'compared_frames': len(frames),
        'exact_frames': matched, 'first_divergence': first_divergence, 'first_missing_baseline_tick': first_missing_tick,
        'scope_fields': list(FIELDS), 'frames': frames,
    }

if len(sys.argv) not in (4, 5):
    raise SystemExit('usage: audit_saved_state_resume.py BASELINE_TRACE RESUMED_TRACE BASELINE_START_TICK [OUTPUT_JSON]')
result = compare(pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), int(sys.argv[3]))
out = pathlib.Path(sys.argv[4]) if len(sys.argv) == 5 else OUT / 'saved-state-resume-audit.json'
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({k: result[k] for k in ('status','baseline_start_tick','resumed_frames','compared_frames','exact_frames','first_divergence','first_missing_baseline_tick','scope_fields')}, separators=(',',':')))
