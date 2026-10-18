"""Fail-closed native comparison for a captured original 08015BB8 return."""
import hashlib
import json
import pathlib
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]
ACTOR_KEY = '0x2002634'
ACTOR_SIZE = 0xe88


def validate_capture(audit):
    if audit.get('status') != 'ORIGINAL_RESPONSE_RETURN_ABI_PASS':
        raise ValueError('A completed original response return is required')
    before, after = audit['before'], audit['after']
    if len(before) != 16 or len(after) != 16 or before[0] != int(ACTOR_KEY, 16):
        raise ValueError('Invalid captured response registers')
    if before[4:12] != after[4:12] or before[13] != after[13]:
        raise ValueError('Original response did not preserve the calling convention')
    if not isinstance(audit.get('instructions'), int) or audit['instructions'] <= 0:
        raise ValueError('Missing original instruction count')
    initial = bytes.fromhex(audit['state_before'][ACTOR_KEY])
    original = bytes.fromhex(audit['state_after'][ACTOR_KEY])
    if len(initial) != ACTOR_SIZE or len(original) != ACTOR_SIZE:
        raise ValueError('The complete actor before and after is required')
    # Extra regions must have matching coverage and remain unchanged.
    if audit['state_before'].keys() != audit['state_after'].keys():
        raise ValueError('Captured region coverage differs')
    for key, value in audit['state_before'].items():
        if key != ACTOR_KEY and bytes.fromhex(value) != bytes.fromhex(audit['state_after'][key]):
            raise ValueError('Unexpected original response side effect: ' + key)
    return initial, original


def verify(path):
    raw = path.read_bytes()
    audit = json.loads(raw)
    initial, original = validate_capture(audit)
    binary = ROOT / 'bin/retained-response-compare.exe'
    with tempfile.TemporaryDirectory(prefix='taxi-response-') as directory:
        source = pathlib.Path(directory) / 'before.bin'
        dest = pathlib.Path(directory) / 'native.bin'
        source.write_bytes(initial)
        subprocess.run([str(binary), str(source), str(dest)], check=True, capture_output=True)
        native = dest.read_bytes()
    report = {
        'audit_sha256': hashlib.sha256(raw).hexdigest(),
        'binary_sha256': hashlib.sha256(binary.read_bytes()).hexdigest(),
        'instructions': audit['instructions'],
        'actor_bytes_compared': ACTOR_SIZE,
        'native_bytes': len(native),
        'scope': 'One captured original response call only; not full movement, driving, or playable gameplay.'
    }
    differences = [i for i in range(min(len(native), ACTOR_SIZE)) if native[i] != original[i]]
    if differences or len(native) != ACTOR_SIZE:
        report.update(status='NATIVE_RETAINED_RESPONSE_STATE_MISMATCH',
                      different_byte_count=len(differences),
                      first_differences=[{'address': hex(int(ACTOR_KEY, 16) + i),
                                          'original': original[i], 'native': native[i]}
                                         for i in differences[:32]])
        (ROOT / 'reports/native-retained-response-mismatch.json').write_text(json.dumps(report, indent=2) + '\n')
        raise ValueError('Native response differs from the complete original actor')
    report['status'] = 'NATIVE_RETAINED_RESPONSE_STATE_PASS'
    (ROOT / 'reports/native-retained-response-comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(report['status'])


if __name__ == '__main__':
    verify(pathlib.Path(sys.argv[1]))
