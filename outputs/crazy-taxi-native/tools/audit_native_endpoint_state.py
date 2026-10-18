"""Read-only comparison of carried native state with the preserved source endpoint."""
import pathlib,json,hashlib,struct
P=pathlib.Path('./')
B=P/'outputs/crazy-taxi-native';R=B/'reports/original-core-reference';W=B/'work/llvm-build-20261014'
def read(p):return p.read_bytes()
def sha(b):return hashlib.sha256(b).hexdigest()
def u32(b,o):return struct.unpack_from('<I',b,o)[0]
def s32(b,o):return struct.unpack_from('<i',b,o)[0]
ram=read(R/'dispatchclean1200.ewram')
assert sha(ram)=='deb821560e88c1ad807d9919db6eface95055c9b81cde9a823dcd0ef4e35b8c0','Source endpoint identity changed'
doc=json.loads(read(R/'dispatchclean1200.json'));assert doc['status']=='PASS' and doc['frames']==1200 and doc['instrumentation'] is None
prior=json.loads(read(R/'continuous-reward-235-20261014.json'))
old=read(W/'reward-traffic-235-native.log');assert sha(old)==prior['native_log_sha256']
prefix=read(W/'state-traffic-235-native.log').decode().splitlines()
assert [l for l in prefix if not l.startswith(('B ','U ','W ','H '))]==old.decode().splitlines(),'Corrected import changed the verified235-update output'
log=read(W/'state-traffic-236-endpoint.log');lines=log.decode().splitlines()
b=next(l.split() for l in lines if l.startswith('B '));u=next(l.split() for l in lines if l.startswith('U '));w=list(map(int,next(l.split()[1:] for l in lines if l.startswith('W '))))
assert int(b[1])==int(u[1])==w[0]==236
nt=bytes.fromhex(b[9]);np=bytes.fromhex(u[2]);assert len(nt)==560 and len(np)==5632
def compare(got,expected,stride):
 d=[{'record':i//stride,'offset':i%stride,'native':a,'source':b} for i,(a,b) in enumerate(zip(got,expected)) if a!=b]
 return {'equal':got==expected,'bytes_compared':len(expected),'difference_count':len(d),'first_differences':d[:40]}
tips=compare(nt,ram[0x9abc:0x9abc+560],56);people=compare(np,ram[0x5f2c:0x5f2c+5632],44)
nr=list(map(int,b[2:9]));sr=[ram[0x9aa4],ram[0x9aa5],ram[0x9aa7],ram[0x9aa6],s32(ram,0x9aa8),s32(ram,0x9aac),u32(ram,0x3b04)]
ctp=read(B/'assets/city1.ctp');customer=(u32(ram,0x1688)-0x02005f2c)//44;assert 0<=customer<u32(ctp,4)
sw=[s32(ram,0x16a8),ram[0x16c2],s32(ram,0x16c4),s32(ram,0x16c8),s32(ram,0x16a0),s32(ram,0x16a4),ram[0x1693],ram[0x1691],customer,ctp[16+customer*32+1]]
manifest=json.loads(read(B/'assets/audio/manifest.json'));payload={int(r['source_pointer'],16)+8+r['selector_prefix']:r['index'] for r in manifest['records']}
h=[list(map(int,l.split()[1:])) for l in lines if l.startswith('H ')];assert len(h)==4
native=[{'slot':p[1],**dict(zip(['stamp','flags4','flags5','volume','last','rate','step','phase','sound'],p[2:]))} for p in h]
source=[]
for slot in range(4):
 o=0x7574+slot*32;ptr=u32(ram,o+24)
 source.append({'slot':slot,'stamp':u32(ram,o),'flags4':ram[o+4],'flags5':ram[o+5],'volume':ram[o+6],'last':ram[o+7],'rate':struct.unpack_from('<H',ram,o+8)[0],'step':u32(ram,o+16),'phase':u32(ram,o+20),'sound':payload[ptr] if ptr else -1})
sc=(u32(ram,0x2604)-0x02007574)//32
v=list(map(int,[l for l in lines if l.startswith('V ')][-1].split()[1:]));nc=v[3]-1
assert 0<=sc<4 and 0<=nc<4
cd={k:{'source':source[sc][k],'native':native[nc][k]} for k in source[sc] if k!='slot' and source[sc][k]!=native[nc][k]}
a=manifest['records'][source[sc]['sound']];assert a['index']==source[sc]['sound']
assert sha(read(B/'assets/audio'/('%03d.cts'%a['index'])))==a['encoded_sha256']
match=tips['equal'] and people['equal'] and nr==sr and w[1:]==sw and not cd
report={'status':'PASS_RAW_NATIVE_ENDPOINT_WITH_LOGICAL_AUDIO_MAPPING' if match else 'PARTIAL_RAW_NATIVE_ENDPOINT_STATE','source_rom_sha256':doc['rom_sha256'],'source_core_sha256':doc['core_sha256'],'source_ram_sha256':sha(ram),'native_log_sha256':sha(log),'native_binary_sha256':sha(read(W/'continuous-driver-traffic-state.exe')),'native_test_sha256':sha(read(B/'tests/continuous_driver_traffic_state_probe.c')),'corrected_tip_base':'0x02009ABC','earlier235_output_unchanged':True,'source_last_dispatch_tick':1198,'source_endpoint_tick':1199,'audio_tail_banks':1,'raw_tip_records':tips,'raw_passenger_records':people,'reward_scalars':{'equal':nr==sr,'native':nr,'source':sr},'clock_fare_display_scalars':{'equal':w[1:]==sw,'native':w[1:],'source':sw},'context_audio':{'native_slot':nc,'source_slot':sc,'sound':a['index'],'differences':cd,'native':native[nc],'source':source[sc]},'source_channels':source,'native_channels':native,'limits':['Engine channels remain reserved in the diagnostic; physical channel allocation and all engine fields are not source-exact.','Endpoint audio tail comes from captured timing, not an autonomous scheduler.','Final1198 actor dispatch has no complete following window;235 completed actor-window claim is unchanged.','Full glyph/camera/pixel and delivery/settlement/results parity is not claimed.']}
(R/'native-endpoint-state-20261014.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
print(json.dumps({k:report[k] for k in ['status','raw_tip_records','raw_passenger_records','reward_scalars','clock_fare_display_scalars','context_audio']},indent=2))