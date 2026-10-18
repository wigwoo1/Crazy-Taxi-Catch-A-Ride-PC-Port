"""Compare a complete actual original orientation return with native code."""
import hashlib,json,pathlib,subprocess,sys,tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
def verify(path):
    raw=path.read_bytes();audit=json.loads(raw)
    assert audit['status']=='ORIGINAL_ORIENTATION_RETURN_ABI_PASS'
    before=audit['before'];after=audit['after']
    assert len(before)==len(after)==16 and before[0]==0x02002634
    assert before[4:12]==after[4:12] and before[13]==after[13]
    regions=[('0x2002634',0xe88),('0x2001534',0x980),('0x30053c0',4),('0x202c4f0',16384),('0x20304f0',16384)]
    initial=[];original=[]
    for key,size in regions:
        assert key in audit['state_before'] and key in audit['state_after'], 'Missing exact captured region '+key
        b=bytes.fromhex(audit['state_before'][key]);a=bytes.fromhex(audit['state_after'][key])
        assert len(b)==len(a)==size
        initial.append(b);original.append(a)
    command_before=bytes.fromhex(audit['state_before']['0x30053d0'])
    command_after=bytes.fromhex(audit['state_after']['0x30053d0'])
    assert len(command_before)==len(command_after)==16 and command_before==command_after
    assert initial[2:]==original[2:], 'Original timing or trig changed'
    binary=ROOT/'bin/retained-orientation-compare.exe'
    with tempfile.TemporaryDirectory(prefix='taxi-orientation-') as directory:
        source=pathlib.Path(directory)/'before.bin';dest=pathlib.Path(directory)/'native.bin'
        source.write_bytes(b''.join(initial))
        result=subprocess.run([str(binary),str(source),str(dest)],capture_output=True,text=True,check=True)
        effects=list(map(int,result.stdout.split()));assert len(effects)==4
        assert effects==[0,0,0,0], 'Reward side effects require separate original capture verification'
        native=dest.read_bytes();expected=b''.join(original)
        if native!=expected:
            differences=[];offset=0
            for (key,size),observed in zip(regions,original):
                actual=native[offset:offset+size]
                changed=[i for i in range(min(len(actual),size)) if actual[i]!=observed[i]]
                if changed or len(actual)!=size:
                    differences.append({'region':key,'expected_bytes':size,'native_bytes':len(actual),
                        'different_byte_count':len(changed),'first_differences':[
                            {'address':hex(int(key,16)+i),'original':observed[i],'native':actual[i]} for i in changed[:32]]})
                offset+=size
            failure={'status':'NATIVE_RETAINED_ORIENTATION_STATE_MISMATCH',
                'audit_sha256':hashlib.sha256(raw).hexdigest(),
                'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),
                'expected_bytes':len(expected),'native_bytes':len(native),'differences':differences,
                'scope':'Diagnostic discrepancy only; no native parity or gameplay claim.'}
            (ROOT/'reports/native-retained-orientation-mismatch.json').write_text(json.dumps(failure,indent=2)+'\n')
            raise AssertionError('Complete native orientation actor/context differs from ROM; see native-retained-orientation-mismatch.json')
    report={'status':'NATIVE_RETAINED_ORIENTATION_STATE_PASS','audit_sha256':hashlib.sha256(raw).hexdigest(),
        'binary_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'instructions':audit['instructions'],
        'actor_bytes_compared':0xe88,'context_bytes_compared':0x980,'effects':effects,
        'scope':'Single actual original12C6C return, complete captured actor/context, unchanged timing/trig/command and no native reward request. Not full movement, driver, gameplay or physical Xbox verification.'}
    (ROOT/'reports/native-retained-orientation-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    print(report['status'])
if __name__=='__main__':verify(pathlib.Path(sys.argv[1]))
