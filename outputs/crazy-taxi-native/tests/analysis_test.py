"""Run against the supplied ROM, after installing tools/requirements.txt."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from analyze_rom import analyze

result = analyze(Path(sys.argv[1]).read_bytes())
addresses = {int(i['address'], 16) for i in result['instructions']}
pools = {int(i['pool_address'], 16) for i in result['literal_loads']}
assert not addresses & pools, 'Reachable instruction overlaps a known literal pool'
for address in (0x080000c0, 0x08005404, 0x0801e274, 0x080174dc, 0x08015c3c):
    assert address in addresses
for address in (0x08009c7e, 0x08009e22, 0x08009fb8, 0x0800ba9e):
    assert address not in addresses, 'False fall-through after shared function exit'
assert any(i['instruction'] == '0x8015c4a' and i['value'] == '0x4000130' for i in result['literal_loads'])
try:
    analyze(b'unsupported ROM')
except ValueError:
    pass
else:
    raise AssertionError('Unsupported ROM accepted')
print('PASS: ROM guard, confirmed entries, input register, literal-pool separation, shared-exit regression checks.')
