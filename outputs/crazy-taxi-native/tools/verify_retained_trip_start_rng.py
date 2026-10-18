"""Replay original RNG using the captured shared trip-start seed, offline only."""
exec(open('outputs/crazy-taxi-native/tools/compare_random_range_rom.py').read().split('\ncases=')[0])
capture=json.loads((root/'reports/active-held-input/trip-start-owner-call.json').read_text())
upper=int.from_bytes(r[0xcbfc:0xcc00],'little')
assert upper==65535,upper
p=RandomProbe((0,0,0,0,0,1,0,0,0,0));p.arm.mem[0x2003b04]=capture['rng_before'];p.reg[0]=0;p.reg[1]=upper
p.run(entry=0x801e72c,stop=0x10020000)
assert p.read(0x2003b04)==capture['rng_after']
result=signed(p.reg[0]);assert result%2+12==capture['after'][10]
report=dict(seed=capture['rng_before'],after=capture['rng_after'],range=[0,upper],result=result,instructions=p.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Isolated original1E72C replay from actual shared CB84 seed; state transition and voice parity exact. Signed remainder explicit contract. Not complete native trip input comparison.')
(root/'reports/retained-trip-start-rng-replay.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
