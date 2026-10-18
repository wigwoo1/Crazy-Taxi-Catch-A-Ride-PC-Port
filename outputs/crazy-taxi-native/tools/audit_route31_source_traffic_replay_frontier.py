#!/usr/bin/env python3
"""Validate the exact native traffic-replay frontier against source probes."""
import gzip, hashlib, json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
R = ROOT / "reports/original-core-reference"
ROM_PATH = Path(r"Crazy Taxi - Catch a Ride (USA).gba")
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
replay = json.loads((R / "route31-source-traffic-replay-to-1412.json").read_text())
trace = json.loads(gzip.decompress((R / "route31nativeinputreplaytrace.actor-trace.json.gz").read_bytes()))
rng_probe = json.loads((R / "route31rngprobe627.rng-callers.json").read_text())
voice_probe = json.loads((R / "route31voicewriteprobe627.voice-reset-writes.json").read_text())
capture = json.loads((R / "route31rngprobe627.route-follow.json").read_text())
assert hashlib.sha256(ROM_PATH.read_bytes()).hexdigest() == ROM_SHA
assert replay["status"] == "FIRST_SOURCE_TRAFFIC_DIVERGENCE_FOUND"
assert replay["exact_prefix_updates"] == 21
frontier = replay["first_mismatch"]
assert frontier["tick"] == 632 and not frontier["vehicle_mismatches"]
assert frontier["actor_mismatch_count"] == 0 and not frontier["actor_mismatches"]
source_by_tick = {row["tick"]: row for row in trace}
impact = source_by_tick[627]["traffic_globals"]
reset = source_by_tick[632]["traffic_globals"]
assert (impact["voice_25d6"], impact["rng_3b04"]) == (8, 478488862)
assert (reset["voice_25d6"], reset["rng_3b04"]) == (255, 1057641020)
events = {e["tick"]: e for e in rng_probe["call_events"]}
assert events[110]["call_site"] == "0x802a91c"
assert events[110]["rng_before"] == 222467215
assert events[115]["call_site"] == "0x80113ea"
assert events[115]["caller_r5"] == "0x20025d6"
assert events[115]["rng_before"] == 478488862
writes = voice_probe["call_events"]
assert len(writes) == 1
write = writes[0]
assert (write["observed_after_core_tick"], write["source_counter"], write["target"],
        write["before_value"], write["write_value"]) == (115, 4828, "0x20025d6", 8, 255)
assert capture["input_replay"]["sha256"] == "2170b94617a5eb397cda65657f3aff898484b7f67a2da42f6072b0666bb603e7"
report = {
    "status": "PASS_SOURCE_FRONTIER_OWNERSHIP_LOCALIZED",
    "rom_sha256": ROM_SHA,
    "route_trace_sha256": hashlib.sha256((R / "route31nativeinputreplaytrace.actor-trace.json.gz").read_bytes()).hexdigest(),
    "native_replay": {
        "exact_full_updates": replay["exact_prefix_updates"],
        "first_divergence_tick": 632,
        "vehicle_mismatches": frontier["vehicle_mismatches"],
        "traffic_actor_mismatch_count": frontier["actor_mismatch_count"],
        "global_mismatches": frontier["global_mismatches"],
    },
    "source_events": {
        "tick_627_collision_voice_rng": {
            "rng_callsite": events[110]["call_site"],
            "rng_before": events[110]["rng_before"],
            "voice_after": impact["voice_25d6"],
            "rng_after": impact["rng_3b04"],
            "native_routine": "taxi_impact_voice; diagnostic replay integration only",
        },
        "tick_632_voice_reset_frontier": {
            "rng_callsite": events[115]["call_site"],
            "caller_r5": events[115]["caller_r5"],
            "rng_before": events[115]["rng_before"],
            "voice_store_target": write["target"],
            "voice_before": write["before_value"],
            "voice_after": write["write_value"],
            "source_counter": write["source_counter"],
        },
    },
    "scope": "The test-only native replay matches vehicle, all 50 traffic records and compared globals through tick 627 after calling the recovered impact-voice routine. Source writes and RNG callers localize the next passenger/audio-state frontier at tick 632. This does not yet establish which higher-level owner schedules that reset, passenger/route/fare parity, or a playable port. No production runtime or supplied ROM was modified.",
}
(R / "route31-source-traffic-replay-frontier.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({"status": report["status"], **report["native_replay"]}, indent=2))
