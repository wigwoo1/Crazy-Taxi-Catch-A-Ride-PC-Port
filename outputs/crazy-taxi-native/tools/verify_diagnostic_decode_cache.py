"""Verify decode caching with original/native command fixture comparison."""
import pathlib, sys, hashlib, json
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from diagnostic_decode_cache import CachedDisassembler
root=pathlib.Path('outputs/crazy-taxi-native')
p=root/'tools/compare_player_command_rom.py'
source=p.read_text()
anchor='cases=[(mask,seed)'
assert source.count(anchor)==1
source=source.replace(anchor,"decode_cache=CachedDisassembler(CommandProbe.run.__globals__['md'])\nCommandProbe.run.__globals__['md']=decode_cache\n"+anchor,1)
source=source.replace('reports/player-command-rom-comparison.json','reports/diagnostic-cache-player-command-comparison.json')
ctx={'CachedDisassembler':CachedDisassembler,'__name__':'__main__'}
exec(compile(source,str(p),'exec'),ctx)
cache=ctx['decode_cache'];assert cache.hits>0 and cache.misses>0
report_path=root/'reports/diagnostic-cache-player-command-comparison.json'
report=json.loads(report_path.read_text())
report['cache']={'hits':cache.hits,'misses':cache.misses,'entries':len(cache.cache),'scope':'Decode cache only; exact code bytes, address and count form key. Same diagnostic command fixtures, no retained trajectory or game-runtime claim.'}
report['validation_source_sha256']=hashlib.sha256(source.encode()).hexdigest()
report['cache_source_sha256']=hashlib.sha256((root/'tools/diagnostic_decode_cache.py').read_bytes()).hexdigest()
report_path.write_text(json.dumps(report,indent=2)+'\n')
print('DECODE_CACHE_PASS',report['cases'],report['cache'])
