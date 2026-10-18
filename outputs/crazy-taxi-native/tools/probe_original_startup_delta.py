import pathlib,json
root=pathlib.Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_live_ordinary_world_rom.py').read_text().split('import sys')[0],ns);raw=(root/'reports/original-core-reference/boot2700.ewram').read_bytes()[0x2634:0x2634+0xea0];records=[]
for delta in (1,4):
 p=ns['WorldProbe']((0,0,0,0,0,0,0,1));base=p.base
 for i,v in enumerate(raw):p.write(base+i,v,1)
 for a in [0x200153c,0x2001540,0x2001544,0x2001548,0x200154c,0x2001694]:p.arm.mem[a]=0
 p.arm.mem.update({0x2001eb0:0,0x2001534:0,0x30053c0:delta,0x3006810:0x87556cc,0x3006a9c:0x8000b78,0x3006ad0:0x8000a1c});p.reg[:2]=[0,base];p.reg[14]=0x10020001;p.run(entry=0x8011f80,stop=0x10020000)
 records.append({'delta':delta,'input_y':int.from_bytes(raw[4:8],'little',signed=True),'output_y':ns['signed'](p.read(base+4)),'flags28':p.read(base+0x28,1),'body_pitch':p.read(base+0x18,2),'body_roll':p.read(base+0x1a,2),'orientation':[ns['signed'](p.read(base+o)) for o in (0x6f4,0x6f8,0x6fc)],'gravity':ns['signed'](p.read(base+0x698))})
assert records[0]['flags28']&8 and not records[1]['flags28']&8
assert records[1]['output_y']==19200 and records[1]['body_pitch']==records[1]['body_roll']==0
report={'status':'PASS','records':records,'source':'Original08011F80 instructions seeded with independently captured boot2700 actor bytes; explicit no-input/settings/globals/arithmetic/containment contracts.','finding':'At original start height ground+64, delta1 incorrectly induces airborne transition relative to observed original delta4, which directly grounds taxi with zero body angles. Original height threshold ground+(delta<<5) accounts for divergent startup body evolution.','limit':'One source world update only; full native variable timestep integration not yet implemented or verified.'};(root/'reports/original-startup-delta-causal-probe.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
