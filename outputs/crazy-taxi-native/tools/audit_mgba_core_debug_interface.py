import hashlib, json, pathlib, struct

root = pathlib.Path(__file__).resolve().parents[1]
dll = pathlib.Path(r"./")
info = pathlib.Path(r"./")
data = dll.read_bytes()
pe = struct.unpack_from("<I", data, 0x3C)[0]
assert data[pe:pe+4] == b"PE\0\0"
machine, section_count = struct.unpack_from("<HH", data, pe+4)
optional = pe + 24
magic = struct.unpack_from("<H", data, optional)[0]
directory = optional + (112 if magic == 0x20B else 96)
export_rva, export_size = struct.unpack_from("<II", data, directory)
sections = []
section_table = optional + struct.unpack_from("<H", data, pe+20)[0]
for i in range(section_count):
    off = section_table + i * 40
    name = data[off:off+8].split(b"\0", 1)[0].decode("ascii", "replace")
    vsize, va, raw_size, raw = struct.unpack_from("<IIII", data, off+8)
    sections.append((name, va, max(vsize, raw_size), raw))
def file_offset(rva):
    for _, va, size, raw in sections:
        if va <= rva < va + size:
            return raw + rva - va
    raise ValueError(f"unmapped RVA 0x{rva:x}")
export = file_offset(export_rva)
(_, _, _, _, _, _, function_count, name_count, functions_rva,
 names_rva, ordinals_rva) = struct.unpack_from("<IIHHIIIIIII", data, export)
names = []
for i in range(name_count):
    name_rva = struct.unpack_from("<I", data, file_offset(names_rva + i*4))[0]
    ordinal = struct.unpack_from("<H", data, file_offset(ordinals_rva + i*2))[0]
    start = file_offset(name_rva)
    end = data.index(b"\0", start)
    names.append(data[start:end].decode("ascii", "replace"))
names.sort()
version_line = next((line.strip() for line in info.read_text(encoding="utf-8", errors="replace").splitlines()
                     if line.strip().startswith("display_version")), None)
assert machine == 0x8664 and function_count == name_count == 25
assert all(name.startswith("retro_") for name in names)
assert not any(any(term in name.lower() for term in ("debug", "trace", "breakpoint", "step"))
               for name in names)
report = {
    "status": "PASS_INSTALLED_LIBRETRO_CORE_HAS_NO_EXPORTED_DEBUGGER_TRACE_API",
    "core_path": str(dll),
    "core_sha256": hashlib.sha256(data).hexdigest(),
    "reported_version": version_line,
    "pe_machine": hex(machine),
    "export_count": name_count,
    "exports": names,
    "debugger_trace_breakpoint_exports": [],
    "scope": (
        "Inspected the installed DLL PE export table. This proves only that its exported ABI offers no direct "
        "debugger/breakpoint/instruction-trace entry point; it does not prove that mGBA has no internal debugger "
        "code or that a source-built core could not expose one."
    ),
    "next": (
        "Use matching mGBA source or core-level instrumentation to observe dispatcher and frame-service "
        "boundaries without patching the game ROM, then revalidate against an uninstrumented source capture."
    )
}
out = root / "reports" / "original-core-reference" / "mgba-core-debug-interface-audit.json"
out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({k: report[k] for k in ("status", "core_sha256", "reported_version", "export_count",
                                        "debugger_trace_breakpoint_exports", "scope", "next")}, indent=2))

