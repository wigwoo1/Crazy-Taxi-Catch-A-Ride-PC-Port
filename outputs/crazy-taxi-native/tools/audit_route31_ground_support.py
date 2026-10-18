#!/usr/bin/env python3
"""Audit an original-ROM route trace against the extracted native city ground mesh."""
from __future__ import annotations
import argparse, hashlib, json, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument('--trace', type=pathlib.Path, default=ROOT/'reports/original-core-reference/route31radius4k.actor-trace.json')
    ap.add_argument('--asset', type=pathlib.Path, default=ROOT/'assets/city1.ctw')
    ap.add_argument('--probe', type=pathlib.Path, default=ROOT/'bin/route-ground-probe.exe')
    ap.add_argument('--output', type=pathlib.Path, default=ROOT/'reports/original-core-reference/route31radius4k-ground-support-audit.json')
    args = ap.parse_args()
    trace = json.loads(args.trace.read_text(encoding='utf-8'))
    if not isinstance(trace, list) or not trace:
        raise SystemExit('Expected nonempty VBlank actor trace')
    payload = ''.join(f"{e['position'][0]} {e['position'][1]} {e['position'][2]}\n" for e in trace)
    run = subprocess.run([str(args.probe), str(args.asset)], input=payload, text=True, capture_output=True, check=True)
    rows = [list(map(int, line.split())) for line in run.stdout.splitlines()]
    if len(rows) != len(trace) or any(len(r) != 7 for r in rows):
        raise SystemExit(f'Probe returned malformed rows: {len(rows)} for {len(trace)} frames')
    misses, mismatches = [], []
    for sample, ground in zip(trace, rows):
        rc, height, nx, ny, nz, material, polygon = ground
        pos = sample['position']
        detail = {'tick': sample['tick'], 'position': pos, 'ground_height': height,
                  'delta_y': height-pos[1] if rc else None, 'normal': [nx,ny,nz],
                  'material': material, 'polygon': polygon,
                  'waypoint': sample.get('route_driver',{}).get('waypoint')}
        if rc != 1:
            misses.append(detail)
        elif height != pos[1]:
            mismatches.append(detail)
    report = {
        'samples': len(trace), 'asset': str(args.asset.relative_to(ROOT)).replace('\\','/'),
        'hit_count': sum(r[0] == 1 for r in rows), 'miss_count': len(misses),
        'exact_height_match_count': len(trace)-len(misses)-len(mismatches),
        'height_mismatch_count': len(mismatches),
        'max_abs_height_delta': max((abs(x['delta_y']) for x in mismatches), default=0),
        'first_mismatches': mismatches[:12], 'last_mismatches': mismatches[-12:],
        'first_misses': misses[:12],
        'sha256': {'trace': sha(args.trace), 'asset': sha(args.asset),
                   'probe_source': sha(ROOT/'tests/route_ground_probe.c'), 'probe_binary': sha(args.probe)},
        'scope': 'Independent native CTW source-mesh ground queries at every sampled original-ROM route-capture position. This checks geometric support and ground height only; it does not prove road-surface semantics, tire contact, obstacles, handling, correct route policy, or successful delivery.'
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k:report[k] for k in ('samples','hit_count','miss_count','exact_height_match_count','height_mismatch_count','max_abs_height_delta','sha256')}, separators=(',',':')))

if __name__ == '__main__': main()

