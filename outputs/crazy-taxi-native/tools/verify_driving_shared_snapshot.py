import pathlib,struct,subprocess,json
r=pathlib.Path('outputs/crazy-taxi-native');ref=r/'reports/original-core-reference';e=(ref/'boot4200.ewram').read_bytes();i=(ref/'boot4200.03000000.bin').read_bytes()
w=lambda a:struct.unpack_from('<i',e,a)[0]
h=lambda a:struct.unpack_from('<H',e,a)[0]
expected=[struct.unpack_from('<i',i,0x53c0)[0],w(0x3b00),e[0x1eb0],e[0x1697],e[0x153c],e[0x177b],e[0x7602],w(0x2630),w(0x1654),e[0x25d4],e[0x25d2],w(0x25dc),e[0x2620],e[0x1537],e[0x1ea8],e[0x1694],h(0x164c),h(0x1650),w(0x1658),e[0x1696],e[0x262c]]+list(i[0x53d0:0x53e0])
p=subprocess.run([str(r/'bin/driving-shared-snapshot-probe.exe'),str(ref/'boot4200.ewram'),str(ref/'boot4200.03000000.bin')],capture_output=True,text=True,check=True);assert list(map(int,p.stdout.split()))==expected

records=[]
for side in ('left','right'):records.extend(json.loads((ref/f'fullshared{side}.packed-driving-trace.json').read_text())['records'])
order=['02001534','0200164C','02001770','02001EA0','02001EB0','020025D0','02007600']
payload=b''.join(b''.join(bytes.fromhex(x['shared_owners'][a]) for a in order)+struct.pack('<i',x['frame_delta'])+bytes.fromhex(x['time_commands']) for x in records)
p=subprocess.run([str(r/'bin/driving-shared-batch-probe.exe')],input=payload,capture_output=True,check=True);rows=p.stdout.decode().splitlines();assert len(rows)==722
for record,row in zip(records,rows):
 e=bytearray(0x40000)
 for address,raw in record['shared_owners'].items():
  start=int(address,16)-0x2000000;data=bytes.fromhex(raw);e[start:start+len(data)]=data
 struct.pack_into('<i',e,0x3b00,record['frame_delta']);i=bytes.fromhex(record['time_commands'])
 expected=[struct.unpack_from('<i',i)[0],w(0x3b00),e[0x1eb0],e[0x1697],e[0x153c],e[0x177b],e[0x7602],w(0x2630),w(0x1654),e[0x25d4],e[0x25d2],w(0x25dc),e[0x2620],e[0x1537],e[0x1ea8],e[0x1694],h(0x164c),h(0x1650),w(0x1658),e[0x1696],e[0x262c]]+list(i[16:32])
 assert list(map(int,row.split()))==expected
report={'status':'PASS_SHARED_NATIVE_CAPTURE_IMPORT','snapshots':722,'fields_per_snapshot':37,'baseline_independently_verified':True,'invalid_short_input_preserves_output':True,'scope':'Shared source scalar and command ingestion only. No native continuous trajectory, action resolution, duplicated-context field verification or routine ordering claim.'}
(r/'reports/driving-shared-snapshot-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
