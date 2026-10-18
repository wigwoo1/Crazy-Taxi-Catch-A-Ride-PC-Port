#!/usr/bin/env python3
"""Audit the slot-10 spawn-height transition and direct-store probe perturbation."""
from pathlib import Path
import hashlib
import json
import struct

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CORE_SHA256 = "73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
SLOT = 10
STRIDE = 0x5C
Y_OFFSET = 0x18


def read_json(name):
    return json.loads((REF / name).read_text(encoding="utf-8"))


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def actor_record(row, slot=SLOT):
    records = row["traffic_actor_records"]
    assert records["stride"] == STRIDE and records["count"] == 50
    raw = bytes.fromhex(records["raw"])
    offset = slot * STRIDE
    return raw[offset : offset + STRIDE]


def actor_y(row, slot=SLOT):
    return struct.unpack_from("<i", actor_record(row, slot), Y_OFFSET)[0]


def main():
    baseline = read_json("mgbasefixedref.actor-trace.json")
    probed = read_json("slot10ywriteprobe.actor-trace.json")
    spawn = read_json("slot10spawnprobe2.traffic-spawn-return-trace.json")
    write = read_json("slot10ywriteprobe.traffic-slot-y-write-trace.json")
    cpu_audit = read_json("route31-uninstrumented-vblank-cpu-snapshot-audit.json")
    cpu_trace_path = REF / "mainloopvblankpickupcpu.cpu-timing-trace.json"
    cpu_records = json.loads(cpu_trace_path.read_text(encoding="utf-8"))["records"]
    source_rom = Path(r"Crazy Taxi - Catch a Ride (USA).gba")
    input_csv = REF / "route31pickupstate516-inputs.csv"
    source_state = REF / "route31pickupstate516.state"

    assert sha256(source_rom) == ROM_SHA256
    assert spawn["rom_sha256"] == write["rom_sha256"] == ROM_SHA256
    assert spawn["core_sha256"] == write["core_sha256"] == CORE_SHA256
    assert len(baseline) == 1200 and len(probed) == 260
    assert spawn["record_count"] == 1 and write["record_count"] == 1
    assert cpu_audit["capture_pair"]["rom_hooks"] is False
    assert cpu_audit["capture_pair"]["full_actor_trace_equal"] is True
    assert cpu_audit["capture_pair"]["core_sha256_equal"] is True
    cpu255 = next(row for row in cpu_records if row["tick"] == 255)
    assert cpu255["gprs"][6] == 0x0200048C
    assert cpu255["gprs"][15] == 0x0800684C
    assert cpu255["gprs"][14] == 0x0800683F
    spawn_event = spawn["events"][0]
    write_event = write["events"][0]
    assert (spawn_event["source_counter"], spawn_event["matching_vblank_ticks"], spawn_event["y"]) == (4968, [255], 0)
    assert (write_event["source_counter"], write_event["matching_vblank_ticks"], write_event["written_y"]) == (4969, [256], 19200)
    assert write_event["flags"] == 1 and write_event["route_pointer"] == "0x85bc3b8"
    assert actor_y(baseline[255]) == 19200 and actor_y(probed[255]) == 0
    assert actor_y(baseline[256]) == actor_y(probed[256]) == 19200

    mismatches = []
    for tick, (left, right) in enumerate(zip(baseline, probed)):
        differing = [key for key in left if left.get(key) != right.get(key)]
        if differing:
            mismatches.append({"tick": tick, "counter": left["counter"], "fields": differing})
            if tick == 255:
                left_raw = bytes.fromhex(left["traffic_actor_records"]["raw"])
                right_raw = bytes.fromhex(right["traffic_actor_records"]["raw"])
                changed = [i for i, (a, b) in enumerate(zip(left_raw, right_raw)) if a != b]
                assert changed == [SLOT * STRIDE + Y_OFFSET + 1]
            elif tick == 256:
                assert differing == ["traffic_pool_changed_from_previous_vblank"]
            else:
                raise AssertionError(("unexpected probe mismatch", tick, differing))
    assert [row["tick"] for row in mismatches] == [255, 256]

    report = {
        "status": "PASS_UNINSTRUMENTED_SLOT10_STORE_PHASE_WITH_PERTURBING_CROSSCHECK",
        "source_rom_sha256": ROM_SHA256,
        "installed_core_sha256": CORE_SHA256,
        "fixed_input_csv": input_csv.name,
        "fixed_input_csv_sha256": sha256(input_csv),
        "saved_state": source_state.name,
        "saved_state_sha256": sha256(source_state),
        "baseline_trace": "mgbasefixedref.actor-trace.json",
        "baseline_trace_sha256": sha256(REF / "mgbasefixedref.actor-trace.json"),
        "spawn_return_probe": spawn["instrumentation"],
        "spawn_return_event": spawn_event,
        "direct_y_store_probe": write["instrumentation"],
        "direct_y_store_event": write_event,
        "baseline_tick255_actor_y": actor_y(baseline[255]),
        "spawn_return_tick255_y_before_common_path": spawn_event["y"],
        "direct_probe_tick255_actor_y": actor_y(probed[255]),
        "direct_probe_tick256_actor_y": actor_y(probed[256]),
        "probe_trace_mismatches_in_260_tick_window": mismatches,
        "uninstrumented_vblank_cpu_checkpoint": {
            "trace": cpu_trace_path.name,
            "trace_sha256": sha256(cpu_trace_path),
            "audit": "route31-uninstrumented-vblank-cpu-snapshot-audit.json",
            "rom_hooks": False,
            "tick": 255,
            "source_counter": baseline[255]["counter"],
            "master_cycles": cpu255["master_cycles"],
            "r6_actor_pointer": hex(cpu255["gprs"][6]),
            "saved_pc_with_thumb_pipeline_semantics": hex(cpu255["gprs"][15]),
            "saved_lr": hex(cpu255["gprs"][14]),
            "actor_y_at_same_vblank": actor_y(baseline[255]),
            "interpretation": "This snapshot came from the uninstrumented replay whose complete 1,200-row actor trace matches the plain baseline. At tick255 the saved PC is downstream of the 0x08006820 store and r6 still identifies slot10, so the original slot10 height write has executed by this VBlank return. Raw PC retains the core's Thumb pipeline semantics; it is a frame-boundary checkpoint, not an instruction trace.",
        },
        "static_rom_bytes": {
            "0x0800681C_to_0x08006823": source_rom.read_bytes()[0x681C:0x6824].hex(),
            "interpretation": "The original dispatcher performs the height query call and contains STR r0,[r6,#0x18] at 0x08006820 followed by LDR r0,[r6,#0x58].",
        },
        "interpretation": "The uninstrumented installed-core reference has slot10 Y=19200 at VBlank tick255, while the spawn-return probe records Y=0 for the same actor at source counter4968. The separate uninstrumented CPU snapshot at tick255 has r6=slot10 and a saved PC downstream of the original 0x08006820 Y store, proving that this original write has executed by the tick255 VBlank return. The direct-store wrapper independently observes that store writing19200, but its branch shifts the sample and is not accepted as timing-neutral. The only 260-tick differences in that probe are slot10 Y at tick255 and the corresponding changed-slot list at tick256. Thus the frame-boundary phase is now established by uninstrumented evidence; the exact subframe cycle remains unknown.",
        "scope": "Short scripted route replay with zero fare. Diagnostic source-ROM and native frame-order evidence only; not proof of full traffic cadence, native gameplay parity, or a playable port.",
    }
    path = REF / "mgba-slot10-spawn-height-boundary-audit.json"
    path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "audit": str(path), "mismatches": mismatches}, indent=2))


if __name__ == "__main__":
    main()
