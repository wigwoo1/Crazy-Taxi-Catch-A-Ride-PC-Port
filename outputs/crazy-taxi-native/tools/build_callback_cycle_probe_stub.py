#!/usr/bin/env python3
"""Extract the Thumb probe .text and resolve its callback-return literal."""
import argparse
import hashlib
import struct
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("object", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--base", type=lambda x: int(x, 0), default=0x08044D00)
    args = parser.parse_args()
    blob = args.object.read_bytes()
    if blob[:6] != b"\x7fELF\x01\x01":
        raise SystemExit("expected a 32-bit little-endian ARM ELF object")
    shoff = struct.unpack_from("<I", blob, 32)[0]
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", blob, 46)
    sections = []
    for index in range(shnum):
        off = shoff + index * shentsize
        sections.append(struct.unpack_from("<IIIIIIIIII", blob, off))
    names_sec = sections[shstrndx]
    section_names = blob[names_sec[4]:names_sec[4] + names_sec[5]]

    def name(offset):
        return section_names[offset:].split(b"\0", 1)[0].decode("ascii")

    text_sec = next((section for section in sections if name(section[0]) == ".text"), None)
    if text_sec is None:
        raise SystemExit("probe object has no .text section")
    if any(section[1] in (4, 9) and section[7] == sections.index(text_sec)
           for section in sections):
        raise SystemExit("probe .text contains unresolved relocations")
    text = bytearray(blob[text_sec[4]:text_sec[4] + text_sec[5]])

    symtab_index = next((i for i, section in enumerate(sections) if section[1] == 2), None)
    if symtab_index is None:
        raise SystemExit("probe object has no symbol table")
    symtab = sections[symtab_index]
    strtab = sections[symtab[6]]
    symbols = blob[strtab[4]:strtab[4] + strtab[5]]
    return_offset = None
    for off in range(symtab[4], symtab[4] + symtab[5], symtab[9]):
        st_name, st_value, st_size, st_info, st_other, st_shndx = struct.unpack_from("<IIIBBH", blob, off)
        symbol_name = symbols[st_name:].split(b"\0", 1)[0].decode("ascii")
        if symbol_name == "callback_cycle_probe_return":
            if st_shndx != sections.index(text_sec):
                raise SystemExit("callback return label is not in .text")
            return_offset = st_value & ~1
            break
    if return_offset is None:
        raise SystemExit("callback return symbol is missing")
    placeholder = struct.pack("<I", 0xDEADBEEF)
    positions = [i for i in range(len(text)) if text.startswith(placeholder, i)]
    if len(positions) != 1:
        raise SystemExit(f"expected one callback return placeholder, found {len(positions)}")
    return_address = args.base + return_offset | 1
    struct.pack_into("<I", text, positions[0], return_address)
    args.output.write_bytes(text)
    print(f"PASS: {len(text)}-byte callback timing stub; return=0x{return_address:08x}; sha256={hashlib.sha256(text).hexdigest()}")


if __name__ == "__main__":
    main()
