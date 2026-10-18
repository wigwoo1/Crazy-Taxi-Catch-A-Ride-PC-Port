from __future__ import annotations
import hashlib, json, struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]
source=root/"tools/collision_entry_stream_return_hook_entry.bin"
out=root/"tools/collision_entry_stream_return_hook_entry_selector.bin"
b=bytearray(source.read_bytes())
assert len(b)==192
assert struct.unpack_from("<H",b,0x58)[0]==0x4814
assert struct.unpack_from("<I",b,0xAC)[0]==0x020000E4
assert b[0x60:0x68]==bytes.fromhex("1348017821700234")
# The entry snapshot stores the full live IWRAM selector at +128. This replaces
# two traffic globals (cooldown and near-miss) only in the diagnostic log.
struct.pack_into("<4H",b,0x58,0x4814,0x6801,0x6021,0x3404)
struct.pack_into("<4H",b,0x60,0x46C0,0x46C0,0x46C0,0x46C0)
struct.pack_into("<I",b,0xAC,0x03004C98)
assert b[0x58:0x60]==bytes.fromhex("1448016821600434")
assert b[0x60:0x68]==bytes.fromhex("c046c046c046c046")
assert struct.unpack_from("<I",b,0xAC)[0]==0x03004C98
out.write_bytes(b)
report={"status":"PASS_VERSIONED_HELPER_SELECTOR_ENTRY_STUB","source_stub_sha256":hashlib.sha256(source.read_bytes()).hexdigest(),"selector_stub_sha256":hashlib.sha256(b).hexdigest(),"stub_bytes":len(b),"reused_literal_offset":"0xAC","selector_address":"0x03004C98","entry_snapshot_offset":"0x80","input_layout_change":"replace cooldown_e4 and near_miss_25d3 input fields with full selector_counter_4c98_u32; output fields unchanged","scope":"Diagnostic in-memory ROM probe only; only a saved source snapshot field is replaced in log data. The helper instructions are replayed unchanged after the hook. State timing impact must be compared against clean actor snapshots."}
(root/"tools/collision_entry_stream_return_hook_entry_selector.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
print(json.dumps(report))

