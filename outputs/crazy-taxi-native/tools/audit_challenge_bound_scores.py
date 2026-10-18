exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
checks={0xa46c:('ldrsh','r1, [r3, r5]'),0xa46e:('movs','r0, #0x96'),0xa470:('lsls','r0, r0, #4'),0xa472:('b','#0x800a92a'),0xa92a:('subs','r1, r0, r1'),0xa92c:('lsls','r1, r1, #0xf'),0xa308:('cmp','r1, #5'),0xa346:('ldrsh','r1, [r2, r0]'),0xa348:('movs','r2, #0x96'),0xa34a:('lsls','r2, r2, #4'),0xa34c:('subs','r1, r2, r1'),0xa350:('lsrs','r0, r0, #0xf'),0xa354:('blt','#0x800a358'),0x1ff50:('cmp','r4, #5'),0x1ff58:('ldrsh','r1, [r0, r3]'),0x1ff5a:('movs','r0, #0x96'),0x1ff5c:('lsls','r0, r0, #4'),0x1ff5e:('b','#0x801ff84'),0x1ff88:('bl','#0x8010988'),0x8fa6:('ldrsh','r0, [r0, r2]'),0x8fa8:('str','r0, [r1]'),0x8faa:('bl','#0x802083c')}
for off,want in checks.items():
 i=next(md.disasm(r[off:off+4],0x8000000+off,count=1));assert(i.mnemonic,i.op_str)==want,(hex(off),i.mnemonic,i.op_str)
for off,value in [(0xa342,0x02001660),(0x1ff54,0x02001660),(0x8fa0,0x020016a8),(0x8fa2,0x02001660)]:
 h=struct.unpack_from('<H',r,off)[0];assert h&0xf800==0x4800;assert struct.unpack_from('<I',r,((off+4)&~3)+(h&255)*4)[0]==value
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'finding':'Bound submode5 original elapsed2400-signed1660, strictlower previous packedword>>15. Dynamic108+12*5+208*profile yields168/376. Source result1FF50..5E joins original10988 timeformatter; HUD8FA0..AA copies signed1660 into retained16A8 before2083C.','guards':{hex(k):v for k,v in checks.items()},'scope':'Exact source-local opcode/literal guards only; full score packing, profile dispatch, independent GBA execution and playable Bound remain unverified.'}
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-score-source.json').write_text(json.dumps(report,indent=2))
print('PASS Bound elapsed2400/signedtimer/strictlower/resultformatter/retainedHUDclock source guards.')
