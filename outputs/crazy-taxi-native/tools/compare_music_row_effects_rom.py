"""Whole271E0 one-channel row callback for pure effect commands12..15, no note/sample."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x80271e0;todo=[entry];seen=set();cfg=[]
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<0x80274bc,hex(a)
 seen.add(a);decoded=list(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));assert decoded,('undecodable',hex(a),cfg[-5:]);i=decoded[0];allowed_thumb[a]=bytes(i.bytes);cfg.append({'address':hex(a),'mnemonic':i.mnemonic,'operands':i.op_str,'bytes':i.bytes.hex()})
 if i.mnemonic=='mov' and i.op_str=='pc, r0':
  assert a==0x8027300
  targets=[int.from_bytes(r[0x27324+j*4:0x27328+j*4],'little') for j in range(10)]
  assert all(0x80271e0<=t<0x80274bc and t%2==0 for t in targets)
  todo.extend(targets);continue
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
(root/'reports/music-row-callback-cfg.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'instructions':sorted(cfg,key=lambda x:int(x['address'],16))},indent=2)+'\n')
saved_allowed=allowed_thumb.copy()
source=source.replace("    value=val(ops[1]);self.reg[rn(ops[0])]=value", "    value=val(ops[1]);self.reg[rn(ops[0])]=value\n    if rn(ops[0])==15:nextpc=value&~1")
source=source.replace("'bhi':c and not z", "'bhs':c,'blo':not c,'bhi':c and not z")
exec(source);allowed_thumb.update(saved_allowed);InitProbe.run=MotionProbe.run
class RowProbe(InitProbe):
 def external(self,t):raise AssertionError(('unexpected helper for pure effect',hex(t)))
cases=[(cmd,param,seed) for cmd in [12,13,14,15] for param in range(256) for seed in [0,165]]
rows=subprocess.run([str(root/'bin/music-row-effects-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 command,param,seed=case;p=RowProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([seed]*16),0x2007618:bytearray([seed]*32),0x10011000:bytearray([0,0,0,0,command,param])};p.scalars={0x2007828:0x8421fc0}
 p.write(0x200760a,1,1);p.write(0x200760e,63 if seed else 0,1)
 # Source selects event by retained row; place the diagnostic event at that exact address.
 event=0x10011000+(63 if seed else 0)*5;p.regions.pop(0x10011000);p.regions[event]=bytearray([0,0,0,0,command,param]);p.write(0x2007614,0x10011000)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 state,voice=line.split();assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(state),(case,'state',p.regions[0x2007608].hex(),state)
 assert bytes(p.regions[0x2007618])==bytes.fromhex(voice),(case,'voice',p.regions[0x2007618].hex(),voice);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole271E0 entry/one-channel/return for commands12 volume,13 BCD break,14 loop,15 speed, everyparameterbyte and two retainedpatterns. Zero sample/note/volume prefix is explicit diagnostic fixture. All12trackerbytes and32voicebytes exact. Command11 restart/note/sample helpers/multiplechannels/real module trajectories excluded.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_event.c',Path(__file__)]}}
(root/'reports/music-row-effects-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
