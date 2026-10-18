"""Build a bounded Thumb return-state probe paired to each follow-entry record.

This emits only diagnostic code for an in-memory ROM copy. The source ROM is
read and hash-checked by capture_original_libretro.py and is never modified.
"""
from pathlib import Path
import hashlib
import json
import struct

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
ENTRY_SOURCE = TOOLS / "collision_entry_stream_hook.bin"
ENTRY_OUTPUT = TOOLS / "collision_entry_stream_return_hook_entry.bin"
RETURN_OUTPUT = TOOLS / "collision_entry_stream_return_hook.bin"
RETURN_JSON = TOOLS / "collision_entry_stream_return_hook.json"
RETURN_ADDRESS = 0x08044E00
ROM_FILL_SIZE = 272
ENTRY_LOG = 0x02037000
ENTRY_CAPACITY = 15
RECORD_STRIDE = 256
INPUT_BYTES = 136
OUTPUT_BYTES = 120
OUTPUT_COUNT = ENTRY_LOG + 0xF04


class Thumb:
    """Tiny emitter for the ARM7 Thumb-1 instructions used by this probe."""
    def __init__(self, origin):
        self.origin = origin
        self.items = []
        self.labels = {}
        self.literals = []

    def emit(self, word):
        self.items.append(("word", word & 0xFFFF))

    def label(self, name):
        self.items.append(("label", name))

    def ldr_literal(self, rd, value):
        self.items.append(("literal", rd, value & 0xFFFFFFFF))

    def branch(self, label, condition=None):
        self.items.append(("branch", label, condition))

    def assemble(self):
        pc = self.origin
        positions = []
        for item in self.items:
            positions.append(pc)
            if item[0] == "label":
                self.labels[item[1]] = pc
            else:
                pc += 2
        while pc & 3:
            positions.append(pc)
            self.items.append(("word", 0x46C0))  # mov r0,r0 (NOP)
            pc += 2
        literal_addresses = {}
        for item in self.items:
            if item[0] == "literal":
                value = item[2]
                if value not in literal_addresses:
                    literal_addresses[value] = pc
                    pc += 4
        out = bytearray()
        for item, address in zip(self.items, positions):
            kind = item[0]
            if kind == "word":
                out.extend(struct.pack("<H", item[1]))
            elif kind == "label":
                continue
            elif kind == "literal":
                rd, value = item[1:]
                literal_pc = (address + 4) & ~3
                delta = literal_addresses[value] - literal_pc
                assert delta >= 0 and delta % 4 == 0 and delta // 4 <= 255
                out.extend(struct.pack("<H", 0x4800 | (rd << 8) | (delta // 4)))
            elif kind == "branch":
                label, condition = item[1:]
                delta = self.labels[label] - (address + 4)
                assert delta % 2 == 0
                units = delta // 2
                if condition is None:
                    assert -1024 <= units <= 1023
                    out.extend(struct.pack("<H", 0xE000 | (units & 0x7FF)))
                else:
                    assert -128 <= units <= 127
                    out.extend(struct.pack("<H", 0xD000 | (condition << 8) | (units & 0xFF)))
            else:
                raise AssertionError(item)
        for value, address in literal_addresses.items():
            out.extend(struct.pack("<I", value))
        assert len(out) == pc - self.origin, (len(out), pc - self.origin)
        return bytes(out)


def reg_imm(base, rn, imm):
    assert 0 <= imm <= 31
    return base | (imm << 6) | (rn << 3) | rn


def imm8(base, rd, imm):
    assert 0 <= imm <= 255
    return base | (rd << 8) | imm


def mem_imm(base, rd, rn, byte_offset, scale):
    assert byte_offset % scale == 0 and 0 <= byte_offset // scale <= 31
    return base | ((byte_offset // scale) << 6) | (rn << 3) | rd


def build_return_hook():
    a = Thumb(RETURN_ADDRESS)
    # Save the caller's registers. At 0x08006BBC, r0 already contains the
    # original return address popped by the routine's epilogue.
    a.emit(0xB5FF)                         # push {r0-r7,lr}
    a.ldr_literal(4, ENTRY_LOG)
    a.emit(mem_imm(0x6800, 5, 4, 0, 4))    # ldr r5,[r4]
    a.emit(imm8(0x2800, 5, ENTRY_CAPACITY))
    a.branch("restore", condition=2)      # bhs: entry overflow/missing record
    a.ldr_literal(0, OUTPUT_COUNT)
    a.emit(mem_imm(0x6800, 7, 0, 0, 4))   # ldr r7,[r0]
    a.emit(imm8(0x2800, 7, ENTRY_CAPACITY))
    a.branch("mark_overflow", condition=2)
    a.emit(0x42AF)                         # cmp r7,r5
    a.branch("mark_overflow", condition=2) # bhs: output index must be < input count
    a.emit(0x1C3B)                         # mov r3,r7
    a.emit(0x021B)                         # lsls r3,r3,#8 (record index * 256)
    a.emit(imm8(0x3000, 4, 4))             # adds r4,#4
    a.emit(0x18E4)                         # adds r4,r4,r3 (input record)
    a.emit(0x1C26)                         # mov r6,r4
    a.emit(imm8(0x3000, 6, INPUT_BYTES))   # output subrecord begins at +136

    # Capture live post-call player position (x,y,z), heading and speed.
    a.ldr_literal(0, 0x02002634)
    a.emit(0x2203)                         # movs r2,#3
    a.label("copy_position")
    a.emit(mem_imm(0x6800, 1, 0, 0, 4))
    a.emit(mem_imm(0x6000, 1, 6, 0, 4))
    a.emit(imm8(0x3000, 0, 4))
    a.emit(imm8(0x3000, 6, 4))
    a.emit(imm8(0x3800, 2, 1))
    a.branch("copy_position", condition=1) # bne
    a.ldr_literal(0, 0x02002650)
    a.emit(mem_imm(0x6800, 1, 0, 0, 4))
    a.emit(mem_imm(0x6000, 1, 6, 0, 4))
    a.emit(imm8(0x3000, 6, 4))
    a.ldr_literal(0, 0x02002B78)
    a.emit(mem_imm(0x6800, 1, 0, 0, 4))
    a.emit(mem_imm(0x6000, 1, 6, 0, 4))
    a.emit(imm8(0x3000, 6, 4))

    # Recover this call's actor pointer from its matching input row, then save
    # the post-call 92-byte actor record.
    a.emit(mem_imm(0x6800, 0, 4, 4, 4))
    a.emit(0x1C33)                         # mov r3,r6
    a.emit(0x2217)                         # movs r2,#23
    a.label("copy_actor")
    a.emit(mem_imm(0x6800, 1, 0, 0, 4))
    a.emit(mem_imm(0x6000, 1, 3, 0, 4))
    a.emit(imm8(0x3000, 0, 4))
    a.emit(imm8(0x3000, 3, 4))
    a.emit(imm8(0x3800, 2, 1))
    a.branch("copy_actor", condition=1)

    # Capture the collision/follow side-effect globals at the same return.
    a.ldr_literal(0, 0x020000E4)
    a.emit(mem_imm(0x8800, 1, 0, 0, 2))
    a.emit(mem_imm(0x8000, 1, 3, 0, 2))
    a.emit(imm8(0x3000, 3, 2))
    a.ldr_literal(0, 0x020025D3)
    a.emit(mem_imm(0x7800, 1, 0, 0, 1))
    a.emit(mem_imm(0x7000, 1, 3, 0, 1))
    a.emit(imm8(0x3000, 3, 2))
    a.ldr_literal(0, 0x02003B04)
    a.emit(mem_imm(0x6800, 1, 0, 0, 4))
    a.emit(mem_imm(0x6000, 1, 3, 0, 4))

    # Advance the return-record count only after the complete output exists.
    a.ldr_literal(0, OUTPUT_COUNT)
    a.emit(mem_imm(0x6800, 7, 0, 0, 4))
    a.emit(imm8(0x3000, 7, 1))
    a.emit(mem_imm(0x6000, 7, 0, 0, 4))
    a.branch("restore")

    a.label("mark_overflow")
    a.ldr_literal(0, OUTPUT_COUNT)
    a.emit(mem_imm(0x6800, 7, 0, 0, 4))
    a.ldr_literal(1, 0x80000000)
    a.emit(0x430F)                         # orrs r7,r1
    a.emit(mem_imm(0x6000, 7, 0, 0, 4))

    a.label("restore")
    a.emit(0xBCFF)                         # pop {r0-r7}
    a.emit(0xB001)                         # add sp,#4 (discard saved lr)
    a.emit(0x4700)                         # bx r0 (original caller return)
    return a.assemble()


def main():
    entry = bytearray(ENTRY_SOURCE.read_bytes())
    assert len(entry) == 192
    assert struct.unpack_from("<H", entry, 6)[0] == 0x2D1E
    struct.pack_into("<H", entry, 6, 0x2D00 | ENTRY_CAPACITY)
    assert struct.unpack_from("<HHH", entry, 0x0A) == (0x1C2B, 0x01DB, 0x1C2A)
    struct.pack_into("<5H", entry, 0x0A, 0x1C2B, 0x021B, 0x46C0, 0x46C0, 0x46C0)
    old_literals = tuple(struct.pack("<I", address) for address in
                         (0x0203F000, 0x0203E000, 0x0203D000))
    old_literal = next((literal for literal in old_literals if entry.count(literal) == 1), None)
    assert old_literal is not None
    entry[entry.index(old_literal):entry.index(old_literal) + 4] = struct.pack("<I", ENTRY_LOG)
    ENTRY_OUTPUT.write_bytes(entry)

    return_hook = build_return_hook()
    assert len(return_hook) <= ROM_FILL_SIZE, (len(return_hook), ROM_FILL_SIZE)
    RETURN_OUTPUT.write_bytes(return_hook)
    report = {
        "entry_hook_sha256": hashlib.sha256(entry).hexdigest(),
        "entry_hook_bytes": len(entry),
        "return_hook_sha256": hashlib.sha256(return_hook).hexdigest(),
        "return_hook_address": hex(RETURN_ADDRESS),
        "return_hook_bytes": len(return_hook),
        "rom_padding_budget_bytes": ROM_FILL_SIZE,
        "record_capacity_per_source_frame": ENTRY_CAPACITY,
        "record_stride_bytes": RECORD_STRIDE,
        "input_bytes": INPUT_BYTES,
        "output_bytes": OUTPUT_BYTES,
        "output_count_address": hex(OUTPUT_COUNT),
        "scope": "ARM7TDMI diagnostic probes only; the capture tool applies them to temporary in-memory ROM content and preserves the supplied ROM.",
    }
    RETURN_JSON.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report))


if __name__ == "__main__":
    main()
