"""Compare actual returned original129E0 state to native steering. No fixtures."""
import hashlib,json,pathlib,struct,subprocess,sys,tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
REGIONS=[('0x2002634',0xe88),('0x30053d0',16),('0x2001534',4),('0x202c4f0',16384),('0x20304f0',16384)]
def verify(path):
    raw=path.read_bytes();audit=json.loads(raw)
    if audit.get('status')!='ORIGINAL_STEERING_RETURN_ABI_PASS':
        raise ValueError('Requires an actual original steering return audit')
    before=audit['before'];after=audit['after']
    assert before[:2]==[0x02002634,0]
    assert len(before)==len(after)==16 and before[13]==after[13] and before[4:12]==after[4:12]
    states=[]
    for name in ['state_before','state_after']:
        chunks=[]
        for address,size in REGIONS:
            chunk=bytes.fromhex(audit[name][address]);assert len(chunk)==size
            chunks.append(chunk)
        states.append(chunks)
    initial,original=states
    binary=ROOT/'bin/retained-steering-compare.exe'
    with tempfile.TemporaryDirectory(prefix='taxi-steering-') as directory:
        payload=pathlib.Path(directory)/'input.bin';payload.write_bytes(b''.join(initial))
        result=subprocess.run([str(binary),str(payload)],check=True,text=True,capture_output=True)
    values=[int(x) for x in result.stdout.split()]
    assert len(values)==4
    rc,response,flags,state=values
    assert rc==0 and response==struct.unpack('<i',struct.pack('<I',after[0]))[0]
    expected=bytearray(initial[0]);expected[0x28]=flags;struct.pack_into('<i',expected,0x7b8,state)
    assert bytes(expected)==original[0], 'Complete captured actor differs from native steering output'
    assert initial[1:]==original[1:], 'Command/settings/trig changed during original steering'
    report={'status':'NATIVE_RETAINED_STEERING_STATE_PASS','audit_sha256':hashlib.sha256(raw).hexdigest(),
        'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'instructions':audit['instructions'],
        'native_output':values,'actor_bytes_compared':0xe88,
        'scope':'Single actual retained129E0 return response and complete captured actor; command/settings/trig preservation. Not full movement, driver, gameplay or physical Xbox verification.'}
    (ROOT/'reports/native-retained-steering-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    print(report['status'])
if __name__=='__main__':
    verify(pathlib.Path(sys.argv[1]))
