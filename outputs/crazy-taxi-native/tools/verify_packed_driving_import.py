import pathlib,json,struct,subprocess,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ref=root/'reports/original-core-reference';records=[]
for side in ('left','right'):records.extend(json.loads((ref/f'shared{side}.packed-driving-trace.json').read_text())['records'])
def inputs(r):
 shared=r['shared_owners'];owner=bytes.fromhex(shared['020025D0']);state=struct.unpack_from('<i',owner,0x60)[0];settings=bytes.fromhex(shared['02001EB0'])[0];return state,settings
payload=''.join(r['actor']+' '+str(inputs(r)[0])+' '+str(inputs(r)[1])+'\n' for r in records)
binary=root/'bin/packed-driving-import-probe.exe'
result=subprocess.run([str(binary)],input=payload,text=True,capture_output=True,check=True);rows=result.stdout.splitlines();assert len(rows)==len(records)==722
for r,line in zip(records,rows):
 fields=line.split();values=list(map(int,fields[:-1]));actor=bytes.fromhex(r['actor']);state,settings=inputs(r)
 xyz=list(struct.unpack_from('<iii',actor));heading=struct.unpack_from('<i',actor,0x1c)[0];speed=struct.unpack_from('<i',actor,0x544)[0]
 assert values==xyz+[heading,speed]+xyz+[heading,state,state,state,settings]
 assert bytes.fromhex(fields[-1])==actor
report={'status':'PASS_PACKED_NATIVE_DRIVING_IMPORT','snapshots':len(rows),'checks':'Native driver and world pose/heading/speed owners match independently decoded captured actor; shared2630 synchronized across three contexts; settings owner retained; full actor byte-exact after mapped driver/world roundtrip, including unmapped bytes.','scope':'Diagnostic state import only; no continuous step, native trajectory, complete shared context import, or source call ordering verified.','binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest()}
(root/'reports/packed-driving-import-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
