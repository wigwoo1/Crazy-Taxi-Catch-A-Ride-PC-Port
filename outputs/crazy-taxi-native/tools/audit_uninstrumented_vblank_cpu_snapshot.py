import hashlib, json, pathlib, statistics

root = pathlib.Path(__file__).resolve().parents[1]
reports = root / "reports" / "original-core-reference"
rom = pathlib.Path(r"Crazy Taxi - Catch a Ride (USA).gba")
expected_rom = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
load = lambda path: json.loads(path.read_text(encoding="utf-8"))
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
base_name, cpu_name = "mainloopvblankpickupbase", "mainloopvblankpickupcpu"
base_meta, cpu_meta = load(reports / f"{base_name}.json"), load(reports / f"{cpu_name}.json")
base_route = load(reports / f"{base_name}.route-follow.json")
cpu_route = load(reports / f"{cpu_name}.route-follow.json")
base_trace = load(reports / f"{base_name}.actor-trace.json")
cpu_trace = load(reports / f"{cpu_name}.actor-trace.json")
cpu_doc = load(reports / f"{cpu_name}.cpu-timing-trace.json")
cpu_records = cpu_doc["records"]
source_state = reports / "route31pickupstate516.state"
assert sha(rom) == expected_rom
assert base_meta["status"] == cpu_meta["status"] == "PASS"
assert base_meta["rom_sha256"] == cpu_meta["rom_sha256"] == expected_rom
assert base_meta["core_sha256"] == cpu_meta["core_sha256"]
assert base_meta["instrumentation"] is None and cpu_meta["instrumentation"] is None
assert base_route["state_sha256"] == cpu_route["state_sha256"]
assert len(base_trace) == len(cpu_trace) == 1200 and base_trace == cpu_trace
assert len(cpu_records) == 1201 and [r["tick"] for r in cpu_records] == [-1] + list(range(1200))
assert base_route["status"] == cpu_route["status"] == "INCOMPLETE_SOURCE_ROUTE31_CAPTURE"
assert base_route["completed_count"] == cpu_route["completed_count"] == 0
assert base_route["fare"] == cpu_route["fare"] == [0, 0]
intervals = [cpu_records[i+1]["master_cycles"] - cpu_records[i]["master_cycles"] for i in range(1200)]
assert min(intervals) > 0
pc_counts = {}
for record in cpu_records[1:]:
    pc = record["gprs"][15]
    pc_counts[pc] = pc_counts.get(pc, 0) + 1
report = {
    "status": "PASS_UNINSTRUMENTED_VBLANK_CPU_SNAPSHOT_TRACE_PARITY",
    "rom_sha256": expected_rom,
    "core_sha256": cpu_meta["core_sha256"],
    "input_state": {"file": source_state.name, "sha256": sha(source_state)},
    "capture_pair": {
        "baseline": base_name, "cpu_snapshot": cpu_name, "frames_each": 1200,
        "rom_hooks": False, "core_sha256_equal": True,
        "full_actor_trace_equal": True, "equal_actor_rows": 1200,
        "trace_includes_all_50_traffic_actor_records": True,
        "cpu_snapshots": len(cpu_records), "snapshot_ticks": [-1, 0, 1199],
        "route_outcome": {"status": cpu_route["status"], "final_waypoint": cpu_route["final_waypoint"],
                          "completed_count": cpu_route["completed_count"], "fare": cpu_route["fare"]},
    },
    "vblank_return_cycles": {
        "per_run_intervals": len(intervals), "master_cycle_delta_min": min(intervals),
        "master_cycle_delta_median": statistics.median(intervals), "master_cycle_delta_max": max(intervals),
        "distinct_deltas": len(set(intervals)),
        "interpretation": "mGBA serialized master-cycle differences between successive retro_run return snapshots; emulator diagnostic only, not native PC frame-rate or monitor scanout measurement."
    },
    "raw_pc_snapshot_distribution": {
        "unique_raw_pcs": len(pc_counts),
        "most_frequent_raw_pcs": [{"pc": hex(pc), "samples": count} for pc, count in sorted(pc_counts.items(), key=lambda x: (-x[1], x[0]))[:12]],
        "interpretation": cpu_doc["scope"],
    },
    "conclusion": (
        "The CPU/VBlank snapshot option was run in a separate capture from the same saved pickup-state fixture, "
        "without temporary ROM hooks. All 1,200 player, camera, route and 50-actor rows exactly match the plain "
        "capture, so the read-only serialization snapshots did not alter the sampled source route state. The saved "
        "CPU registers and master-cycle values locate the serialized state at each retro_run/VBlank return, but "
        "these snapshots do not identify the main-loop iteration that led to that return. Route progress stopped "
        "at waypoint 4 with zero completed fares; this is not a playable-port or full-loop parity result."
    ),
    "evidence_sha256": {
        f"reports/original-core-reference/{name}": sha(reports / name)
        for name in [
            f"{base_name}.json", f"{base_name}.route-follow.json", f"{base_name}.actor-trace.json",
            f"{cpu_name}.json", f"{cpu_name}.route-follow.json", f"{cpu_name}.actor-trace.json",
            f"{cpu_name}.cpu-timing-trace.json", "route31pickupstate516.state"
        ]
    }
}
out = reports / "route31-uninstrumented-vblank-cpu-snapshot-audit.json"
out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({
    "status": report["status"],
    "full_actor_trace_equal": report["capture_pair"]["full_actor_trace_equal"],
    "rows": report["capture_pair"]["equal_actor_rows"],
    "cycle_delta_min_median_max": [min(intervals), statistics.median(intervals), max(intervals)],
    "route": report["capture_pair"]["route_outcome"],
    "report": str(out)
}, indent=2))

