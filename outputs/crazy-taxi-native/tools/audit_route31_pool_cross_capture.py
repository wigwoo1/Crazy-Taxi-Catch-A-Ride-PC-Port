#!/usr/bin/env python3
"""Cross-check two untouched-ROM traffic-pool captures at V3 schedule rows."""
import hashlib
import json
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
EXPECTED_ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    labels = {
        "poolglobals": "route31poolglobals",
        "poolcontext6": "route31poolcontext6",
    }
    captures = {}
    for label, stem in labels.items():
        capture = read_json(REPORTS / f"{stem}.json")
        mapping = read_json(REPORTS / f"{stem}-pool-route-mapping-lifecycle.json")
        lifecycle = read_json(REPORTS / f"{stem}-lifecycle-events.json")
        assert capture["status"] == "PASS" and capture["frames"] == 12000
        assert capture["rom_sha256"] == EXPECTED_ROM
        assert "original-ROM" in capture["scope"]
        assert "traffic_dispatch_instrumentation" not in capture
        assert mapping["rom_sha256"] == EXPECTED_ROM
        assert mapping["ticks"] == 12000 and mapping["slots"] == 50
        assert mapping["schedule_rows"] == 1755 and mapping["schedule_actor_pool_alignment_pass"]
        assert mapping["unknown_route_descriptors"] == {}
        assert mapping["point_pointer_mismatch_examples"] == []
        assert lifecycle["status"] == "PASS_SOURCE_ACTOR_POOL_LIFECYCLE_EXTRACTION"
        assert lifecycle["trace_sha256"] == mapping["trace_sha256"]
        captures[label] = {
            "capture": capture,
            "mapping": mapping,
            "lifecycle": lifecycle,
            "capture_path": REPORTS / f"{stem}.json",
            "mapping_path": REPORTS / f"{stem}-pool-route-mapping-lifecycle.json",
            "lifecycle_path": REPORTS / f"{stem}-lifecycle-events.json",
        }

    first, second = captures["poolglobals"], captures["poolcontext6"]
    a, b = first["mapping"], second["mapping"]
    la, lb = first["lifecycle"], second["lifecycle"]
    assert a["schedule_sha256"] == b["schedule_sha256"]
    assert a["aligned_actor_pool_sha256"] == b["aligned_actor_pool_sha256"]
    assert a["traffic_global_change_counts_aligned_schedule"] == b["traffic_global_change_counts_aligned_schedule"]
    assert la["events"] == lb["events"]
    assert la["event_counts"] == lb["event_counts"]
    assert la["transition_signatures"] == lb["transition_signatures"]
    assert a["aligned_camera_observations"] == 0 and not a["aligned_camera_coverage_complete"]
    assert b["aligned_camera_observations"] == b["schedule_rows"] and b["aligned_camera_coverage_complete"]

    result = {
        "status": "PASS_CROSS_CAPTURE_TRAFFIC_POOL_AND_LIFECYCLE_MATCH",
        "rom_sha256": EXPECTED_ROM,
        "source_vblank_frames_per_capture": 12000,
        "schedule_rows_aligned": a["schedule_rows"],
        "aligned_50_actor_pool_sha256": a["aligned_actor_pool_sha256"],
        "aligned_traffic_global_change_counts": a["traffic_global_change_counts_aligned_schedule"],
        "lifecycle_event_count": len(la["events"]),
        "lifecycle_event_counts": la["event_counts"],
        "candidate_spawn_rejection_signature": la["transition_signatures"][
            "candidate_path_write_then_180_retry_without_rng_or_selection_advance"],
        "camera_coverage": {
            "poolglobals": {"observations": a["aligned_camera_observations"],
                            "complete": a["aligned_camera_coverage_complete"]},
            "poolcontext6": {"observations": b["aligned_camera_observations"],
                             "complete": b["aligned_camera_coverage_complete"]},
        },
        "input_sha256": {
            str(item[key].relative_to(ROOT)): sha(item[key])
            for item in captures.values()
            for key in ("capture_path", "mapping_path", "lifecycle_path")
        },
        "interpretation": "Two separate original-ROM captures agree byte-for-byte on the concatenated 50-actor pool at all 1,755 schedule-aligned snapshots and on every extracted lifecycle event. The context6 capture additionally contains camera snapshots for all schedule rows. This cross-capture check validates source evidence consistency, not exact per-call dispatch order or native traffic parity. The 180-tick retry signature remains observational and is not proof of its exact caller.",
        "scope": "Read-only verification of captured original-ROM RAM traces; not a playable-port test.",
    }
    output = REPORTS / "route31pool-cross-capture-audit.json"
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": result["status"], "output": str(output),
                      "aligned_schedule_rows": result["schedule_rows_aligned"],
                      "actor_pool_hash": result["aligned_50_actor_pool_sha256"],
                      "lifecycle_events": result["lifecycle_event_count"],
                      "camera_coverage": result["camera_coverage"],
                      "sha256": sha(output)}, indent=2))


if __name__ == "__main__":
    main()
