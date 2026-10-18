"""Cross-check recovered ROM frame order against the current native caller order.

Static BL/caller evidence only. The report does not prove runtime timing or full-frame parity.
"""
from __future__ import annotations
import argparse
import json
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
EXPECTED_CALLS = [(0x0800B368, 0x08020A20), (0x0800B36C, 0x08022DF4),
                  (0x0800B370, 0x0801ED8C), (0x0800B378, 0x080066B4)]

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", type=Path, required=True)
    ap.add_argument("--output", type=Path, required=True)
    a = ap.parse_args()
    report_dir = a.root / "reports"
    source_dir = a.root / "src"
    analysis = json.loads((report_dir / "analysis.json").read_text(encoding="utf-8"))
    if analysis.get("rom_sha256") != ROM_SHA:
        raise SystemExit("Static ROM analysis hash differs from the inspected supplied ROM")
    calls = {(int(row["from"], 16), int(row["to"], 16)) for row in analysis["calls"]}
    if not all(pair in calls for pair in EXPECTED_CALLS):
        raise SystemExit("Expected main-loop call sequence is absent from the static analysis")
    effects = json.loads((report_dir / "effect-frame-order-audit.json").read_text(encoding="utf-8"))
    powertrain = json.loads((report_dir / "original-core-reference" / "counter5294-source-powertrain-audit.json").read_text(encoding="utf-8"))
    scene = (source_dir / "scene_viewer.c").read_text(encoding="utf-8")
    driver = scene.index("int step_result=paused?")
    traffic = scene.index("taxi_traffic_game_update(", driver)
    delivery = scene.index("taxi_passenger_scene_delivery_service_observed(", traffic)
    driver_call = effects["normal_path"]["driver"]
    if driver_call != "0800B022 -> 08011A3C":
        raise SystemExit("Recovered normal-path driver callsite differs from reviewed report")
    source_expected = powertrain["source_event"]["post_speed_544"]
    source_before = powertrain["source_event"]["entry_speed_544"]
    callback = powertrain["native_source_ordered_callback_window_replay"]
    if callback["status"] != "PASS" or "powertrain_exact=yes" not in callback["stdout"]:
        raise SystemExit("Source-ordered traffic callback replay no longer passes")
    report = {
        "status": "PASS_STATIC_SOURCE_NATIVE_CALL_ORDER_CROSSCHECK",
        "rom_sha256": ROM_SHA,
        "source_main_loop_calls_before_traffic": [
            {"site": f"0x{site:08X}", "target": f"0x{target:08X}"} for site, target in EXPECTED_CALLS
        ],
        "source_normal_path_driver_call": driver_call,
        "native_scene_viewer_order": {
            "driver_world_step_source_line": scene[:driver].count("\n") + 1,
            "traffic_update_source_line": scene[:traffic].count("\n") + 1,
            "passenger_delivery_source_line": scene[:delivery].count("\n") + 1,
            "driver_before_traffic": driver < traffic,
            "traffic_before_delivery": traffic < delivery
        },
        "source_collision_speed": {"entry": source_before, "post_callback": source_expected},
        "native_source_ordered_callback_window": callback["stdout"],
        "finding": "Static source call order and native top-level order both place the normal driver update before traffic; the source-ordered callback can then change shared taxi speed for a later driver update. Do not move traffic before the current driver step based on the cross-counter trace alone.",
        "scope": "Static source callsites plus source-matched native callback window and source-text order. This does not prove exact within-VBlank execution timing, full traffic frame parity, or playable driving."
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "native_scene_viewer_order": report["native_scene_viewer_order"], "source_collision_speed": report["source_collision_speed"], "scope": report["scope"]}, indent=2))

if __name__ == "__main__":
    main()
