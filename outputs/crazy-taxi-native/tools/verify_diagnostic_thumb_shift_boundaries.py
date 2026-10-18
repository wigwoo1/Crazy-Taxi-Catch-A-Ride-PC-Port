from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/diagnostic_thumb_shift.py').read_text(),ns)
f=ns['thumb_shift_result'];cases=[]
expected={'lsls':[(0x80000001,None),(2,True),(0x80000000,False),(0,True),(0,False),(0,False),(0x80000001,None),(2,True)],'lsrs':[(0x80000001,None),(0x40000000,True),(1,False),(0,True),(0,False),(0,False),(0x80000001,None),(0x40000000,True)],'asrs':[(0x80000001,None),(0xc0000000,True),(0xffffffff,False),(0xffffffff,True),(0xffffffff,True),(0xffffffff,True),(0x80000001,None),(0xc0000000,True)]}
for kind,values in expected.items():
 for count,(value,carry) in zip((0,1,31,32,33,255,256,257),values):
  for prior in (False,True):
   want=(value,prior if carry is None else carry);got=f(kind,0x80000001,count,prior,True);assert got==want,(kind,count,got,want);cases.append([kind,count,prior,got])
for kind in ('lsrs','asrs'):
 for prior in (False,True):assert f(kind,0x80000001,0,prior,False)==((0 if kind=='lsrs' else 0xffffffff),True)
r={'status':'PASS','register_cases':len(cases),'immediate_zero_cases':4,'scope':'Explicit boundary expected result/carry cases for supported Thumb shifts; no firmware/platform oracle or fullframe proof.'}
(root/'reports/diagnostic-thumb-shift-boundaries.json').write_text(json.dumps(r,indent=2)+'\n');print(r)
