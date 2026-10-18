import hashlib, json, pathlib

project = pathlib.Path(__file__).resolve().parents[1]
reports = project / "reports" / "original-core-reference"
load = lambda path: json.loads(path.read_text(encoding="utf-8"))
sha = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
rom = pathlib.Path(r"Crazy Taxi - Catch a Ride (USA).gba")
expected_rom = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
manifest_path = project / "assets" / "audio" / "manifest.json"
manifest = load(manifest_path)
start_path = reports / "route31contextstartdirect.audio-context-start-trace.json"
start = load(start_path)
assert sha(rom) == expected_rom == start["instrumentation"]["source_rom_sha256"]
assert start["record_count"] == 1
event = start["call_events"][0]
assert (event["source_counter"], event["selection"], event["classification"], event["returned_handle"]) == (4738, 13, 1, "0x2007594")
asset_index = 83 + 24 * event["classification"] + event["selection"]
asset = manifest["records"][asset_index]
assert asset_index == 120 and asset["source_pointer"].lower() == "083700a8"
assert asset["rate"] == 10512 and asset["decoded_samples"] == 15288
base_ewram_path = reports / "route31contextstartbase25.ewram-vblank-00025.bin"
probe_ewram_path = reports / "route31contextstartdirect.ewram-vblank-00025.bin"
base_ewram, probe_ewram = base_ewram_path.read_bytes(), probe_ewram_path.read_bytes()
assert len(base_ewram) == len(probe_ewram) == 262144
asset_header = bytes.fromhex("b0003708")
assert base_ewram[0x75B0:0x75B4] == probe_ewram[0x75B0:0x75B4] == asset_header
base_trace_path = reports / "route31contextstartbase25.actor-trace.json"
probe_trace_path = reports / "route31contextstartdirect.actor-trace.json"
base_trace, probe_trace = load(base_trace_path), load(probe_trace_path)
assert len(base_trace) == 31 and len(probe_trace) == 120 and probe_trace[:31] == base_trace
logger_range = range(0x3F000, 0x3F284)
ewram_diffs = [i for i, (a, b) in enumerate(zip(base_ewram, probe_ewram)) if a != b and i not in logger_range]
busy_path = reports / "route31busyreturnlive.audio-busy-return-trace.json"
busy = load(busy_path)
busy_events = busy["call_events"]
assert len(busy_events) == 17 and [e["busy_helper_return"] for e in busy_events] == [1] * 16 + [0]
assert busy_events[-1]["source_counter"] == 4828
base_route = load(reports / "route31contextstartbase25.route-follow.json")
probe_route = load(reports / "route31contextstartdirect.route-follow.json")
assert base_route["status"] == probe_route["status"] == "INCOMPLETE_SOURCE_ROUTE31_CAPTURE"
assert base_route["completed_count"] == probe_route["completed_count"] == 0
assert base_route["fare"] == probe_route["fare"] == [0, 0]
evidence_paths = [start_path, base_ewram_path, probe_ewram_path, base_trace_path, probe_trace_path, busy_path,
                  reports / "route31contextstartbase25.route-follow.json",
                  reports / "route31contextstartdirect.route-follow.json", manifest_path, rom]
report = {
    "status": "PASS_CONTEXT_HANDLE_START_ASSET_AND_BUSY_END_CORRELATED",
    "rom_sha256": expected_rom,
    "evidence_sha256": {str(p.relative_to(project)) if project in p.parents else str(p): sha(p) for p in evidence_paths},
    "start_event": event,
    "sound_mapping": {"formula": "83 + 24 * classification + selection", "asset_index": asset_index,
                      "manifest_source_pointer": "0x" + asset["source_pointer"], "header_pointer": "0x083700B0",
                      "sample_rate_hz": asset["rate"], "decoded_samples": asset["decoded_samples"],
                      "approx_duration_seconds": asset["decoded_samples"] / asset["rate"]},
    "checkpoint_comparison": {"asset_header_pointer_offset": "0x020075B0",
                              "asset_pointer_matches_in_baseline_and_probe": True,
                              "actor_trace_baseline_rows": len(base_trace), "actor_trace_instrumented_rows": len(probe_trace),
                              "actor_trace_31_row_prefix_equal": True,
                              "non_logger_ewram_difference_count": len(ewram_diffs),
                              "non_logger_ewram_difference_offsets": [hex(i) for i in ewram_diffs]},
    "busy_end": {"calls": len(busy_events), "returns": [e["busy_helper_return"] for e in busy_events],
                 "completion_source_counter": busy_events[-1]["source_counter"]},
    "route_status": {"baseline": base_route["status"], "instrumented": probe_route["status"],
                     "completed_count": probe_route["completed_count"], "fare": probe_route["fare"]},
    "conclusion": ("At source counter 4738 the original contextual voice-start caller received handle 0x02007594 "
                   "for selection 13/classification 1. The recovered index formula maps this to extracted sound asset 120; "
                   "both matched EWRAM snapshots contain asset 120 header pointer 0x083700B0, and the uninstrumented "
                   "31-row actor trace equals the corrected probe prefix. Six EWRAM byte differences outside the logger "
                   "region are reported rather than treated as equivalent state. A separate direct query trace returns "
                   "busy=1 sixteen times and busy=0 at counter 4828. This correlates the start handle with the requested "
                   "asset and completion response, but does not prove hardware channel identity or audible playback. Both "
                   "captures are incomplete diagnostic route probes with zero delivery/fare; no playable-port claim.")
}
out = reports / "route31-context-voice-asset-owner-audit.json"
out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({"status": report["status"], "asset": asset_index, "prefix_equal": True,
                  "nonlogger_ewram_differences": [hex(i) for i in ewram_diffs],
                  "busy_returns": report["busy_end"]["returns"], "route": report["route_status"],
                  "report": str(out)}, indent=2))

