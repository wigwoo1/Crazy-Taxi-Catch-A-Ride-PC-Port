exec(open('outputs/crazy-taxi-native/tools/compare_bound_pivot.py').read().split('# Source motion')[0])
exec(open('outputs/crazy-taxi-native/tools/angle_rom_probe.py').read())
import random
rng=random.Random(20261005)
cases=[(a,b) for a in [-2147483648,-4097,-4096,-2048,-1,0,1,2047,2048,2049,4095,4096,2147483647] for b in [-2147483648,-4097,-4096,-2048,-1,0,1,2047,2048,2049,4095,4096,2147483647]]+[(a,(a+d)&4095) for a in range(4096) for d in [0,1,2047,2048,2049,4095]]+[(rng.randint(-2147483648,2147483647),rng.randint(-2147483648,2147483647)) for _ in range(10000)]
res=subprocess.run(['outputs/crazy-taxi-native/bin/angle-rom-compare.exe'],input=''.join(f'{a} {b}\n' for a,b in cases),text=True,capture_output=True,check=True);rows=list(map(int,res.stdout.split()));assert len(rows)==len(cases)
probe=AngleProbe(r)
for case,expected in zip(cases,rows):assert probe.run(*case)==expected,case
report=dict(cases=len(cases),original_arm_instructions=probe.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original ARM 080052B4 angle difference execution vs native function, all 4096 residues at six offsets plus signed extremes/random inputs. Not full gameplay.')
pathlib.Path('outputs/crazy-taxi-native/reports/angle-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
