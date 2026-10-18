"""Original ordinary session mute reset store with qualified zero register."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
ins=list(Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0xb782:0xb786],0x800b782))
assert [(i.mnemonic,i.op_str) for i in ins]==[('ldr','r0, [pc, #0x118]'),('strb','r4, [r0]')]
assert r[0xb740:0xb742]==bytes.fromhex('0024') # movs r4,#0, no intervening r4 write in this scalar block.
audit_cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB);audit_cs.detail=True
for i in audit_cs.disasm(r[0xb742:0xb782],0x800b742):assert 'r4' not in [i.reg_name(reg) for reg in i.regs_access()[1]]
allowed_thumb.update({i.address:bytes(i.bytes) for i in ins});steps=0
for prior in range(256):
 p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.write(0x2001534,prior,1);p.reg[4]=0;p.run(entry=0x800b782,stop=0x800b786)
 assert p.read(0x2001534,1)==0;steps+=p.steps
report=dict(cases=256,thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='ActualB782..B784 mute reset store; r4zero provenance B740 and no intervening register write statically checked. All prior byte values reset0. Native session memset init and explicit field bridge preserve this owner. Not completeB700 initialization or exhaustivechallenge setter proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/fare_session.c','src/fare_session.h','src/game_audio_frame_bridge.c','tools/compare_game_audio_mute_reset_rom.py']})
(root/'reports/game-audio-mute-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
