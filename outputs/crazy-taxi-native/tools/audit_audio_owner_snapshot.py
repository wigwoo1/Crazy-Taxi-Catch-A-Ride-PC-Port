"""Independent source-field mapping for actual native audio ownership tests."""
import pathlib,json,struct,hashlib
P=pathlib.Path('./');B=P/'outputs/crazy-taxi-native';R=B/'reports/original-core-reference';W=B/'work/llvm-build-20261014'
def sha(b):return hashlib.sha256(b).hexdigest()
def u32(b,o):return struct.unpack_from('<I',b,o)[0]
def s32(b,o):return struct.unpack_from('<i',b,o)[0]
def s16(b,o):return struct.unpack_from('<h',b,o)[0]
def sb(v):return v if v<128 else v-256
def handle(p):
 if not p:return 0
 assert 0x02007574<=p<0x020075f4 and (p-0x02007574)%32==0
 return (p-0x02007574)//32+1
manifest=json.loads((B/'assets/audio/manifest.json').read_bytes());rom=(pathlib.Path('./')/'Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert sha(rom)==manifest['rom_sha256']=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
payload={};table=bytearray(203*4)
for record in manifest['records']:
 index=record['index'];pointer=int(record['source_pointer'],16)
 assert u32(rom,0x7ec658+index*4)==pointer
 assert struct.unpack_from('<H',rom,pointer-0x08000000+2)[0]==record['selector_prefix']
 sound=(B/'assets/audio'/('%03d.cts'%index)).read_bytes();assert sha(sound)==record['encoded_sha256']
 assert sound==rom[pointer-0x08000000:pointer-0x08000000+len(sound)],('Sound bytes differ from supplied ROM',index)
 data=pointer+8+record['selector_prefix'];assert data not in payload;payload[data]=index
 struct.pack_into('<I',table,index*4,data)
assert bytes(table)==(W/'audio-source-payloads.bin').read_bytes()
lines=(W/'audio-owner-snapshot.log').read_text().splitlines();controls={};channels={};passes={}
for line in lines:
 p=line.split()
 if p[0]=='C':controls[int(p[1])]=list(map(int,p[2:]))
 if p[0]=='H':channels[(int(p[1]),int(p[2]))]=list(map(int,p[3:]))
 if p[0]=='PASS':passes[int(p[1])]=line
files=['boot4200.ewram','dispatchcleanunhooked378.ewram-vblank-00377.bin','dispatch414clean420.ewram-vblank-00414.bin','route31pickupstate516.ewram-vblank-00516.bin','dispatchclean1200.ewram']
checks=[]
for i,name in enumerate(files):
 ram=(R/name).read_bytes();assert len(ram)==0x40000
 expected=[handle(u32(ram,0x25e8+j*4)) for j in range(10)]+[s32(ram,0x2614),s32(ram,0x25dc),s32(ram,0x25e4),s16(ram,0x25da)]+list(ram[0x25d0:0x25d6])+[ram[0x25d8],sb(ram[0x25d6]),sb(ram[0x25d7]),sb(ram[0x2610])]
 assert len(expected)==24 and controls[i]==expected,('Native controller state differs',name)
 for slot in range(4):
  o=0x7574+slot*32;ptr=u32(ram,o+24)
  expected_channel=[u32(ram,o),ram[o+4],ram[o+5],ram[o+6],ram[o+7],struct.unpack_from('<H',ram,o+8)[0],u32(ram,o+16),u32(ram,o+20),payload[ptr] if ptr else -1]
  assert channels[(i,slot)]==expected_channel,('Native channel state differs',name,slot)
 assert 'preserves_voices' in passes[i] and 'original_mixed=' in passes[i]
 checks.append({'source_snapshot':name,'source_snapshot_sha256':sha(ram),'controller_scalars_matched':24,'channel_scalars_matched':36,'source_engine_handles':controls[i][:2],'source_context_handle':controls[i][7],'source_driver_handle':controls[i][8],'native_lifecycle_test':passes[i]})
assert len(controls)==len(passes)==5 and len(channels)==20
report={'status':'PASS_FULL_NATIVE_AUDIO_SNAPSHOT_AND_ENGINE_OWNERSHIP','source_rom_sha256':sha(rom),'snapshots':checks,'sound_payload_entries_verified':203,'encoded_sound_assets_verified':203,'native_binary_sha256':sha((W/'audio-owner-snapshot.exe').read_bytes()),'native_probe_source_sha256':sha((B/'tests/audio_owner_snapshot_probe.c').read_bytes()),'native_importer_sha256':sha((B/'tests/audio_owner_snapshot.h').read_bytes()),'native_log_sha256':sha((W/'audio-owner-snapshot.log').read_bytes()),'limits':['Importer is an offline fixture boundary, never production startup or a ROM memory runtime.','Native playback and engine lifecycle execute from real source owner/channel state, but their subsequent PCM/phase output is not compared to a source-after-service capture.','The full combined controller has not yet replaced reserved engines in the continuous replay.','Unrepresented source decoder pointers at channel+12/+28 are not imported or claimed exact.'],'next_dependency':'Use this verified fixture importer with all4-channel mixing and the recovered full controller in the continuously carried native replay. Align audio service before driver physics and prove dynamic physical channel allocation against source checkpoints.'}
(R/'audio-owner-snapshot-20261014.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
print(json.dumps({'status':report['status'],'snapshots':checks,'sound_payload_entries_verified':203},indent=2))