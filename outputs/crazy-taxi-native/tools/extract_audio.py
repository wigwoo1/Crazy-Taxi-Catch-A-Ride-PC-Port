"""Hash-guarded original sound-table extraction; no audio synthesis or ROM edits.
CTS files retain the source 8-byte header and encoded payload. Reference PCM8
is diagnostic decoding for independent comparison with the native C mixer.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

ROM_SHA = '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'

def extract(data, output, reference):
    if hashlib.sha256(data).hexdigest() != ROM_SHA:
        raise ValueError('Unsupported ROM revision')
    codebook = data[0x49dd80:0x49dfe0]  # Original DMA98 words =608 bytes.
    records = []
    tails = []
    for index in range(203):  # Original sound-table index gate <=0xca.
        pointer = struct.unpack_from('<I', data, 0x7ec658+4*index)[0]
        offset = pointer-0x08000000
        if not 0 <= offset <= len(data)-8:
            raise ValueError(f'Sound {index}: pointer outside ROM')
        rate, prefix, length = struct.unpack_from('<HHI', data, offset)
        size = prefix+(length+1)//2 if prefix else length
        if length > 2_000_000 or offset+8+size > len(data):
            raise ValueError(f'Sound {index}: payload outside ROM')
        payload = data[offset+8:offset+8+size]
        if prefix:
            if prefix < (length+15)//16:
                raise ValueError(f'Sound {index}: short selector stream')
            decoded = bytearray()
            for sample in range(length):
                selector = payload[sample//16]
                packed = payload[prefix+sample//2]
                code = selector*16+((packed >> (4*(sample&1))) & 15)
                if code >= len(codebook):
                    raise ValueError(f'Sound {index}: codebook index outside bounds')
                decoded.append(codebook[code])
            decoded = bytes(decoded)
        else:
            decoded = payload
        if not prefix:
            tail_offset=offset+8+length
            if tail_offset+256>len(data):
                raise ValueError(f'Sound {index}: loop tail outside ROM')
            tails.append((index,data[tail_offset:tail_offset+256],tail_offset))
        raw = data[offset:offset+8+size]
        records.append((index, raw, decoded, dict(index=index, source_pointer=f'{pointer:08x}',
            rate=rate, selector_prefix=prefix, decoded_samples=length,
            encoded_sha256=hashlib.sha256(raw).hexdigest(),
            decoded_sha256=hashlib.sha256(decoded).hexdigest())))
    output.mkdir(parents=True, exist_ok=True)
    reference.mkdir(parents=True, exist_ok=True)
    (output/'codebook.bin').write_bytes(codebook)
    for index, raw, decoded, metadata in records:
        (output/f'{index:03}.cts').write_bytes(raw)
        (reference/f'{index:03}.pcm8').write_bytes(decoded)
    for index,tail,tail_offset in tails:
        (output/f'{index:03}.tail').write_bytes(tail)
    manifest = dict(raw_loop_tails=[dict(index=i,source_offset=f'{off:08x}',size=len(t),sha256=hashlib.sha256(t).hexdigest()) for i,t,off in tails],rom_sha256=ROM_SHA, codebook_sha256=hashlib.sha256(codebook).hexdigest(),
                    records=[record[3] for record in records])
    (output/'manifest.json').write_text(json.dumps(manifest, indent=2))
    print(f'Extracted {len(records)} original sound records; {sum(len(r[2]) for r in records)} diagnostic decoded samples.')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--reference', type=Path, required=True)
    args = parser.parse_args()
    extract(args.rom.read_bytes(), args.output, args.reference)
