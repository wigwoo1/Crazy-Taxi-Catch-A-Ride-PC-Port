"""Conservative reachable-code survey; does not execute or emulate the ROM.

Requires Python 3 and capstone==5.0.9. Findings are candidates, not a decompilation.
"""
import argparse
import hashlib
import json
import struct
from collections import deque
from pathlib import Path

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM, CS_MODE_THUMB, CS_GRP_JUMP
from capstone.arm import ARM_OP_IMM, ARM_OP_MEM, ARM_REG_PC

BASE = 0x08000000
EXPECTED = '39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'


def analyze(data):
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED:
        raise ValueError('Unsupported ROM revision: SHA-256 does not match inspected USA ROM.')
    decoders = {False: Cs(CS_ARCH_ARM, CS_MODE_ARM), True: Cs(CS_ARCH_ARM, CS_MODE_THUMB)}
    for decoder in decoders.values():
        decoder.detail = True
    startup = struct.unpack_from('<I', data, 0x1d0)[0]
    # Verified callback assignments: 0801e248 and 0801e2ca, through bx-r0 thunk 0803244c.
    callbacks = [struct.unpack_from('<I', data, offset)[0] for offset in (0x1e26c, 0x1e2e0)]
    queue = deque([(BASE + 0xc0, False), (BASE + 0xfc, False), (startup & ~1, bool(startup & 1))]
                  + [(value & ~1, bool(value & 1)) for value in callbacks])
    seen, instructions, calls, literals, unresolved = set(), [], [], [], []
    while queue:
        address, thumb = queue.popleft()
        while BASE <= address < BASE + len(data) and (address, thumb) not in seen:
            seen.add((address, thumb))
            offset = address - BASE
            ins = next(decoders[thumb].disasm(data[offset:offset+4], address, count=1), None)
            if ins is None:
                unresolved.append({'address': hex(address), 'reason': 'decode failed'})
                break
            instructions.append({'address': hex(address), 'mode': 'thumb' if thumb else 'arm',
                                 'bytes': ins.bytes.hex(), 'mnemonic': ins.mnemonic, 'operands': ins.op_str})
            for operand in ins.operands:
                if operand.type == ARM_OP_MEM and operand.mem.base == ARM_REG_PC:
                    location = ((address + 4) & ~3 if thumb else address + 8) + operand.mem.disp
                    if BASE <= location <= BASE + len(data) - 4 and ins.mnemonic == 'ldr':
                        value = struct.unpack_from('<I', data, location-BASE)[0]
                        literals.append({'instruction': hex(address), 'pool_address': hex(location), 'value': hex(value)})
            target = ins.operands[0].imm if ins.operands and ins.operands[0].type == ARM_OP_IMM else None
            if ins.mnemonic in ('bl', 'blx'):
                if target is not None:
                    target_thumb = not thumb if ins.mnemonic == 'blx' else thumb
                    calls.append({'from': hex(address), 'to': hex(target & ~1), 'mode': 'thumb' if target_thumb else 'arm'})
                    queue.append((target & ~1, target_thumb))
                    # Game-specific long-range BL to shared stack-unwinding epilogues.
                    # These restore the saved caller PC and do not return to this BL.
                    if thumb and target in (0x0800a92a, 0x0800a96a, 0x0800c524):
                        break
                else:
                    unresolved.append({'address': hex(address), 'reason': 'indirect call'})
            elif ins.group(CS_GRP_JUMP):
                if target is not None:
                    queue.append((target & ~1, thumb))
                    if ins.mnemonic == 'b':
                        break
                else:
                    unresolved.append({'address': hex(address), 'reason': 'indirect branch or return', 'instruction': ins.mnemonic+' '+ins.op_str})
                    break
            elif ('pc' in ins.op_str and ins.mnemonic in ('pop', 'ldm', 'ldmib', 'ldmdb')) or (ins.op_str.startswith('pc,') and ins.mnemonic in ('mov', 'ldr', 'add')):
                unresolved.append({'address': hex(address), 'reason': 'PC write / return / dispatch', 'instruction': ins.mnemonic+' '+ins.op_str})
                break
            address += ins.size
    instructions.sort(key=lambda item: (int(item['address'], 16), item['mode']))
    hardware = [item for item in literals if 0x04000000 <= int(item['value'], 16) < 0x04000400 or 0x05000000 <= int(item['value'],16) < 0x07000400]
    return {'rom_sha256': digest, 'limitations': ['Static candidates only; no runtime validation.', 'Indirect control flow, RAM code, and callback tables are not resolved.', 'Capstone may decode instructions outside ARM7TDMI; reachable candidates require review.', 'Hardware constants are references, not proof of a read or write.'],
            'startup_thumb_entry': hex(startup & ~1), 'instruction_count': len(instructions),
            'direct_call_target_count': len({(c['to'],c['mode']) for c in calls}),
            'instructions': instructions, 'calls': calls, 'literal_loads': literals,
            'hardware_literal_candidates': hardware, 'unresolved_control_flow': unresolved}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom', type=Path)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = analyze(args.rom.read_bytes())
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output/'analysis.json').write_text(json.dumps(result, indent=2)+'\n')
    (args.output/'reachable-code.txt').write_text('\n'.join(f"{i['address']} {i['mode']:5} {i['bytes']:8} {i['mnemonic']:8} {i['operands']}" for i in result['instructions'])+'\n')
    print(json.dumps({k:result[k] for k in ('rom_sha256','startup_thumb_entry','instruction_count','direct_call_target_count')}, indent=2))


if __name__ == '__main__':
    main()
