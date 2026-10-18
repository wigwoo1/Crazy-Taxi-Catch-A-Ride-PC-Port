#!/usr/bin/env python3
"""Validate no-hook CPU snapshots against the plain counter-5294 replay."""
from pathlib import Path
import hashlib
import json

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CORE_SHA256 = "73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
STATE_SHA256 = "a20a48db7c4514d28c072de7ad80ed1576d77076a06d5e7b6c038ce8b0404a01"


def read_json(name):
    return json.loads((REF / name).read_text(encoding="utf-8"))


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    baseline = read_json("r5294plain3.actor-trace.json")
    captured = read_json("r5294cpucapture.actor-trace.json")
    run = read_json("r5294cpucapture.json")
    route = read_json("r5294cpucapture.route-follow.json")
    cpu_path = REF / "r5294cpucapture.cpu-timing-trace.json"
    cpu = json.loads(cpu_path.read_text(encoding="utf-8"))
    assert run["rom_sha256"] == route["rom_sha256"] == ROM_SHA256
    assert run["core_sha256"] == route["core_sha256"] == CORE_SHA256
    assert run["instrumentation"] is None
    assert run["traffic_dispatch_instrumentation"] is None
    assert run["traffic_follow_entry_instrumentation"] is None
    assert run["state_crc_rebase"]["source_state_file_sha256"] == STATE_SHA256
    assert route["state_sha256"] == STATE_SHA256
    assert len(baseline) == len(captured) == 3

    per_tick = []
    for tick, (plain, snap) in enumerate(zip(baseline, captured)):
        assert all(plain[k] == snap.get(k) for k in plain if k != "route_driver"), (
            "unhooked baseline game-state field changed", tick
        )
        per_tick.append({
            "tick": plain["tick"],
            "source_counter": plain["counter"],
            "input_keys": plain["input_keys"],
            "raw_50_actor_pool_equal": True,
            "vehicle_and_camera_equal": True,
            "route_driver_telemetry_equal": plain.get("route_driver") == snap.get("route_driver"),
        })

    records = cpu["records"]
    assert [r["tick"] for r in records] == [-1, 0, 1, 2]
    checkpoints = []
    for record in records:
        checkpoints.append({
            "tick": record["tick"],
            "master_cycles": record["master_cycles"],
            "raw_saved_pc": hex(record["gprs"][15]),
            "lr": hex(record["gprs"][14]),
        })

    report = {
        "status": "PASS_UNINSTRUMENTED_COUNTER5294_CPU_SNAPSHOT_PARITY",
        "source_rom_sha256": ROM_SHA256,
        "core_sha256": CORE_SHA256,
        "input_state_sha256": STATE_SHA256,
        "baseline_trace": "r5294plain3.actor-trace.json",
        "baseline_trace_sha256": sha256(REF / "r5294plain3.actor-trace.json"),
        "cpu_snapshot_trace": cpu_path.name,
        "cpu_snapshot_trace_sha256": sha256(cpu_path),
        "frames": 3,
        "rom_instrumentation": None,
        "gameplay_rows_equal_excluding_only_driver_telemetry": True,
        "per_tick": per_tick,
        "vblank_cpu_checkpoints": checkpoints,
        "interpretation": "Read-only serialized CPU snapshots preserve the three-frame route-follow replay exactly for every game-state field: input keys, vehicle/camera, route state, and all 50 raw traffic actors. The route-driver metadata is deliberately excluded because it is external controller telemetry. The returned PCs span ROM/IWRAM contexts and do not identify the precise owner of the split actor-pool writes; use them only as saved VBlank-return checkpoints, not as a source instruction trace.",
        "scope": "Three-frame pickup/navigation replay starting at source counter5294 with zero deliveries/fare. No-hook determinism and snapshot parity only; not whole-frame dispatcher parity or a playable-port result.",
    }
    output = REF / "counter5294-uninstrumented-cpu-snapshot-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "output": str(output), "frames": len(per_tick), "cpu_records": len(checkpoints)}, indent=2))


if __name__ == "__main__":
    main()
