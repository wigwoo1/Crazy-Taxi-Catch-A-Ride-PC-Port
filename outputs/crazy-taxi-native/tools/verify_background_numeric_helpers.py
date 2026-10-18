"""Execute original numeric library over every background half-pitch value."""
from pathlib import Path
import json,hashlib
exec(Path('outputs/crazy-taxi-native/tools/original_thumb_numeric_probe.py').read_text())
root=Path('outputs/crazy-taxi-native');upper=struct.unpack_from('<II',ROM,0x2b760);lower=struct.unpack_from('<II',ROM,0x2b768)
limits=[struct.unpack('>d',struct.pack('>II',*v))[0] for v in [upper,lower]];steps=conversion_steps=comparison_steps=0
for value in range(-1024,1024):
 p=NumericProbe();p.reg[0]=value&MASK;got=p.run(0x08033340);expected=struct.unpack('>II',struct.pack('>d',float(value)));assert tuple(got)==expected,(value,got,expected);conversion_steps+=p.steps
 for target,bits,limit in [(0x08033210,upper,limits[0]),(0x080332a8,lower,limits[1])]:
  p=NumericProbe();p.reg[:4]=[*got,*bits];result=p.run(target);expected=(value>limit)-(value<limit);assert p.signed(result[0])==expected,(value,hex(target),result,expected);comparison_steps+=p.steps
 if value%256==0:print('PASS through',value,flush=True)
report={'status':'PASS','integer_values':2048,'original_integer_to_double_calls':2048,'original_double_comparison_calls':4096,'original_conversion_instructions':conversion_steps,'original_comparison_instructions':comparison_steps,'double_word_order':'High32bit word inr0, low32bit word inr1 (originalAPCS)','encoded_limits':limits,'probe_sha256':hashlib.sha256((root/'tools/original_thumb_numeric_probe.py').read_bytes()).hexdigest(),'scope':'Actualoriginal33340 conversion and33210/332A8 comparisons plus nested original32728 packing/32870 unpacking/comparison helpers; every integer-1024..1023 covering all normalized camera pitch halves. No externalnumeric contracts or emulator in native runtime. This verifies finite sourceprefix inputs; genericNaN/infinity/all32bitconversion not claimed. Complete2B6A0 prefix/frame/pixels/cycles stillnotexecuted.'}
(root/'reports/background-numeric-helpers-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
