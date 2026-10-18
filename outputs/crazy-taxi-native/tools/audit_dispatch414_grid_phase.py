#!/usr/bin/env python3
"""Cross-check tick414 native/source camera and traffic-grid selection."""
from __future__ import annotations
import gzip, hashlib, json, struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PORT = ROOT / "outputs/crazy-taxi-native"
REPORTS = PORT / "reports/original-core-reference"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def main() -> None:
    capture = json.loads((REPORTS / "dispatchclean1200.json").read_text())
    with gzip.open(REPORTS / "dispatchclean1200.actor-trace.json.gz", "rt") as f:
        rows = json.load(f)
    diagnostic = json.loads((REPORTS / "dispatchclean-camera-grid-diagnostic-audit.json").read_text())
    grid = (PORT / "assets/traffic/city1-grid.ctg").read_bytes()
    assert capture["rom_sha256"] == ROM_SHA and len(rows) >= 420
    assert diagnostic["source_rom_sha256"] == ROM_SHA
    width, height, cell_x, cell_z, origin_x, _origin_y, origin_z, _extent = struct.unpack_from("<HHiiiiii", grid, 4)

    def actor(tick: int, slot: int) -> dict[str, int]:
        raw = bytes.fromhex(rows[tick]["traffic_actor_records"]["raw"])
        off = slot * 92
        return {"flags": struct.unpack_from("<I", raw, off)[0],
                "type": struct.unpack_from("<I", raw, off + 4)[0],
                "countdown": struct.unpack_from("<i", raw, off + 16)[0],
                "y": struct.unpack_from("<i", raw, off + 24)[0]}

    row = rows[414]
    camera = row["traffic_camera"]["position"]
    col = int((camera[0] - origin_x) / cell_x)
    grid_row = int((camera[2] - origin_z) / cell_z)
    index = grid_row * width + col
    offset = 40 + index * 28
    source_pointer = struct.unpack_from("<I", grid, offset)[0]
    routes = grid[offset + 4:offset + 12]
    phases = struct.unpack_from("<8H", grid, offset + 12)
    count = 8
    while count and phases[count - 1] == 0:
        count -= 1
    selector = row["traffic_globals"]["selection_counter_4c98"]
    selected = selector % count
    source_grid = {"cell": index, "row": grid_row, "column": col,
                   "source_pointer": hex(source_pointer), "selector": selector,
                   "route": routes[selected], "phase": phases[selected] * 4}
    native_row = next(x for x in diagnostic["camera_grid_diagnostics"] if x["step"] == 85)
    source_rows = {"414": {"actor8": actor(414, 8),
                            "selection_counter": rows[414]["traffic_globals"]["selection_counter_4c98"],
                            "rng": rows[414]["traffic_globals"]["rng_3b04"]},
                   "415": {"actor8": actor(415, 8),
                            "selection_counter": rows[415]["traffic_globals"]["selection_counter_4c98"],
                            "rng": rows[415]["traffic_globals"]["rng_3b04"]}}
    assert native_row["source_camera_position"] == camera
    assert native_row["native_grid_selection"]["cell_index"] == source_grid["cell"]
    assert native_row["native_grid_selection"]["route"] == source_grid["route"]
    assert native_row["native_grid_selection"]["phase"] == source_grid["phase"]
    assert source_rows["414"]["actor8"]["flags"] == 0
    assert source_rows["414"]["actor8"]["countdown"] == -2
    assert source_rows["415"]["actor8"]["flags"] == 1
    assert source_rows["415"]["selection_counter"] == 13
    assert source_rows["414"]["selection_counter"] == 12
    report = {
        "status": "PASS_SOURCE_NATIVE_GRID_AND_TRANSITION_CROSSCHECK",
        "source_rom_sha256": ROM_SHA,
        "source_core_sha256": capture["core_sha256"],
        "source_tick414_camera_position": camera,
        "runtime_grid_geometry": {"width": width, "height": height,
            "cell_size": [cell_x, cell_z], "origin_xz": [origin_x, origin_z]},
        "source_grid_selection_at_tick414": source_grid,
        "native_grid_selection_at_step85": native_row["native_grid_selection"],
        "native_source_camera_match": native_row["camera_position_matches"],
        "source_transition": source_rows,
        "finding": "The tick414 source camera and extracted grid select the same cell, route, and phase as native. Between clean source rows414 and415, slot8 becomes active, the selection counter advances12→13, and RNG advances. The dispatcher-entry log records calls at counters4610/tick414 and4615/tick419, so this proves the transition interval but not its instruction owner or exact sub-frame phase.",
        "next_dependency": "Capture source writes/call ownership for actor slot8 flags, selection counter, and RNG during the tick414→415 interval; require timing impact to be compared with the clean run before using the result as a scheduling oracle.",
        "scope": "Read-only cross-check of a bounded source/native diagnostic. It does not establish dispatcher parity, a production fix, a completed port, or playable trip behavior.",
    }
    out = REPORTS / "dispatch414-grid-phase-audit.json"
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "source_grid_selection_at_tick414", "source_transition", "next_dependency")}, indent=2))

if __name__ == "__main__":
    main()
