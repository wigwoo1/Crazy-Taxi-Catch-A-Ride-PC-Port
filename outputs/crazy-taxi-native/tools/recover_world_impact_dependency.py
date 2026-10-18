import pathlib,json,sys
root=pathlib.Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_live_ordinary_world_rom.py').read_text().split('import sys')[0],ns)
r=ns['r'];md=ns['md'];todo=[0x802abf4];seen=set();calls=[]
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x802abf4<=a<0x802ac6c,hex(a);seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1))
 ns['allowed_thumb'][a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='bl':calls.append(hex(i.operands[0].imm))
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
report={'classification_instructions':len(seen),'classification_calls':sorted(set(calls)),'impact_cfg':'challenge-bound-impact-cfg.json'}
(root/'reports/world-impact-dependency.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
