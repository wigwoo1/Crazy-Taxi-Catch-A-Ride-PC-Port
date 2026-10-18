"""Execute original ordinary caller through its first movement/camera call; offline only."""
import pathlib,json
ns={};root=pathlib.Path('outputs/crazy-taxi-native')
exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
todo=[0x800afd4];seen=set()
while todo:
 a=todo.pop()
 if a in seen or a==0x800b320:continue
 assert 0x800afd4<=a<0x800b320,hex(a)
 seen.add(a);i=next(ns['md'].disasm(ns['r'][a-0x8000000:a-0x8000000+4],a,count=1));ns['allowed_thumb'][a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class FirstCall(Exception):pass
class CallerProbe(ns['InitProbe']):
 def external(self,target):
  self.calls.append(hex(target))
  if target in [0x80130ac,0x8011a3c,0x8014dd8]:raise FirstCall
  assert target==0x8029264,('unqualified prior call',hex(target))
  self.reg[0]=0
rows=[]
for active in [0,1]:
 for freeze in [0,1]:
  for ended in [0,1]:
   for demo in [0,1]:
    p=CallerProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
    p.scalars.update({0x2005f20:freeze,0x2001708:0,0x2001696:active,0x2001697:ended,0x2007700:0,0x2001700:0,0x2001704:0,0x2003b00:1,0x30053c0:1,0x3004ca0:0x2002634})
    # SB is demo1708, not pause. State1696/1697 reside in the packed initializer region, not scalars.
    p.write(0x2005f20,freeze,1);p.write(0x2001696,active,1);p.write(0x2001697,ended,1);p.write(0x2001708,demo,1)
    p.reg[9]=0x2001708;p.reg[7]=0x2002634;p.reg[13]=0x10000400;p.reg[14]=0x10020001
    try:p.run(entry=0x800afd4,stop=0x800b320)
    except FirstCall:pass
    rows.append(dict(active_after_countdown=active,freeze=freeze,ended=ended,demo=demo,first_call=p.calls[-1],prior_calls=p.calls[:-1],thumb_instructions=p.steps))
assert next(x for x in rows if (x['active_after_countdown'],x['freeze'],x['ended'],x['demo'])==(0,0,0,0))['first_call']=='0x8011a3c'
assert next(x for x in rows if (x['active_after_countdown'],x['freeze'],x['ended'],x['demo'])==(1,0,0,0))['first_call']=='0x80130ac'
report=dict(status='PASS',cases=rows,scope='Original AFD4 caller decisions after BA04 return through first gear/driver/camera boundary; 29264 bookkeeping call explicit no-state contract, SB actual1708 demo pointer established by AF7A/AF7C literals; other globals supplied; no full frame proof.')
(root/'reports/ordinary-first-movement-rom.json').write_text(json.dumps(report,indent=2));print(json.dumps(report))

