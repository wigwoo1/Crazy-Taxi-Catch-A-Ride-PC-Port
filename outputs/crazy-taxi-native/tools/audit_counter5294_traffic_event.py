"""Audit the route31 counter5294 traffic call against paired source/native state."""
from __future__ import annotations

import hashlib
import json
import pathlib
import struct
import subprocess

PROJECT = pathlib.Path("outputs/crazy-taxi-native")
REPORTS = PROJECT / "reports/original-core-reference"
BASE = "r5294plain3"
PROBE = "r5294followprobe"
CHECKPOINT = "r5294base1100"


def read_json(name: str):
    return json.loads((REPORTS / name).read_text(encoding="utf-8"))


def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def stream_json_array(path: pathlib.Path):
    decoder = json.JSONDecoder()
    with path.open(encoding="utf-8") as source:
        buffer = source.read(65536)
        position = 0
        eof = not bool(buffer)
        started = False
        while True:
            while True:
                while position < len(buffer) and (buffer[position].isspace() or buffer[position] == ","):
                    position += 1
                if position < len(buffer):
                    break
                if eof:
                    return
                buffer = buffer[position:] + source.read(65536)
                position = 0
                eof = not bool(buffer)
            if not started:
                assert buffer[position] == "["
                position += 1
                started = True
                continue
            if buffer[position] == "]":
                return
            try:
                item, end = decoder.raw_decode(buffer, position)
            except json.JSONDecodeError:
                if eof:
                    raise
                buffer = buffer[position:] + source.read(65536)
                position = 0
                eof = not bool(buffer)
                continue
            position = end
            yield item


baseline = read_json(BASE + ".actor-trace.json")
instrumented = read_json(PROBE + ".actor-trace.json")
entry_trace = read_json(PROBE + ".traffic-follow-entry-trace.json")
assert len(baseline) == len(instrumented) == 3
fields = (
    "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation",
    "gravity", "airborne", "raw_input_history", "traffic_camera", "driver_owners",
    "traffic_actor_records", "traffic_globals", "traffic_frame_context",
    "completed_count", "fare_whole", "fare_fraction", "remaining_clock",
)
pool_mismatches = []
for tick, (plain, hooked) in enumerate(zip(baseline, instrumented)):
    mismatch = [key for key in fields if key != "traffic_actor_records" and plain[key] != hooked[key]]
    assert not mismatch, ("short instrumentation changed source", tick, mismatch)
    assert plain["counter"] == 5294 + tick
    plain_pool = bytes.fromhex(plain["traffic_actor_records"]["raw"])
    hooked_pool = bytes.fromhex(hooked["traffic_actor_records"]["raw"])
    changed_slots = [slot for slot in range(50)
                     if plain_pool[slot * 92:(slot + 1) * 92] != hooked_pool[slot * 92:(slot + 1) * 92]]
    pool_mismatches.append({"tick": tick, "slots": changed_slots})
assert pool_mismatches == [
    {"tick": 0, "slots": [10, 11]},
    {"tick": 1, "slots": []},
    {"tick": 2, "slots": []},
]

# Recheck that this route-follow checkpoint reproduces the beginning of the
# archived 16,000-frame source run, through the exact pre-event boundary.
prefix = read_json(CHECKPOINT + ".actor-trace.json")
assert len(prefix) == 1098 and prefix[-1]["counter"] == 5293
archived = PROJECT / "reports/original-core-reference/route31fast14pool.actor-trace.json"
for index, (old, new) in enumerate(zip(stream_json_array(archived), prefix)):
    assert old == new, ("route baseline diverged from archived source", index)
    if index + 1 == len(prefix):
        break

pre_ram_path = REPORTS / (CHECKPOINT + ".ewram-vblank-01097.bin")
pre_ram = pre_ram_path.read_bytes()
assert len(pre_ram) == 0x40000
pre_pool = pre_ram[0xF4:0xF4 + 50 * 92]
post_pool = bytes.fromhex(baseline[0]["traffic_actor_records"]["raw"])
assert len(post_pool) == 50 * 92
route_manifest = json.loads((PROJECT / "assets/traffic/city1-roads.json").read_text())
descriptor_to_route = {int(path["descriptor_address"], 16): path["index"] for path in route_manifest["paths"]}
events = [event for event in entry_trace["call_events"] if event["source_counter"] == 5294]
assert len(events) == 7
call_summary = []
for event in events:
    slot = (int(event["actor_pointer"], 16) - 0x020000F4) // 92
    assert 0 <= slot < 50
    before = bytes.fromhex(event["actor_raw"])
    after = bytes.fromhex(event["actor_after_raw"])
    assert before == pre_pool[slot * 92:(slot + 1) * 92], (slot, "entry actor mismatch")
    assert after == post_pool[slot * 92:(slot + 1) * 92], (slot, "return actor mismatch")
    descriptor = struct.unpack_from("<I", before, 0x24)[0]
    call_summary.append({
        "call_index": event["call_index"],
        "slot": slot,
        "route": descriptor_to_route.get(descriptor),
        "entry_speed": event["player_speed_544"],
        "return_speed": event["player_after_speed_544"],
        "cooldown_before_after": [event["cooldown_e4"], event["cooldown_after_e4"]],
        "rng_before_after": [event["rng_3b04"], event["rng_after_3b04"]],
        "entry_matches_pre_pool": True,
        "return_matches_post_pool": True,
    })

collision = events[0]
assert collision["actor_pointer"] == "0x2000150"
assert descriptor_to_route[struct.unpack_from("<I", bytes.fromhex(collision["actor_raw"]), 0x24)[0]] == 134
assert collision["player_speed_544"] == 97367
assert collision["player_after_speed_544"] == 24341
assert collision["cooldown_e4"] == 0 and collision["cooldown_after_e4"] == 60
assert collision["rng_3b04"] == 238862784 and collision["rng_after_3b04"] == 1676021248

before_actor = REPORTS / "counter5294-route134-before.actor"
after_actor = REPORTS / "counter5294-route134-after.actor"
before_actor.write_bytes(bytes.fromhex(collision["actor_raw"]))
after_actor.write_bytes(bytes.fromhex(collision["actor_after_raw"]))
pre_cooldown = struct.unpack_from("<h", pre_ram, 0xE4)[0]
pre_near = pre_ram[0x25D3]
pre_voice = pre_ram[0x25D6]
pre_combo_flag = pre_ram[0x9AA6]
pre_combo_a = struct.unpack_from("<i", pre_ram, 0x9AA8)[0]
pre_combo_b = struct.unpack_from("<i", pre_ram, 0x9AAC)[0]
pre_rng = struct.unpack_from("<I", pre_ram, 0x3B04)[0]
pre_context = baseline[0]["traffic_frame_context"]
assert pre_rng == collision["rng_3b04"]
assert pre_context["sound_7602"] == 1 and pre_context["trip_active_1694"] == 1

exe = PROJECT / "bin/traffic-follow-exact-event-probe.exe"
native = subprocess.run([
    str(exe), str(before_actor), str(after_actor),
    str(PROJECT / "assets/traffic/city1-road-134.ctrp"),
    str(PROJECT / "assets/driving.ctr"), "134",
    str(collision["player_position"][0]), str(collision["player_position"][2]),
    str(collision["player_heading"]), str(collision["player_speed_544"]),
    str(pre_cooldown), str(pre_near), str(pre_voice), str(pre_combo_flag),
    str(pre_combo_a), str(pre_combo_b), str(pre_rng),
    str(pre_context["sound_7602"]), str(pre_context["trip_active_1694"]),
    str(collision["argument_r1"]), str(collision["player_after_speed_544"]),
    str(collision["cooldown_after_e4"]), str(baseline[0]["traffic_globals"]["near_miss_25d3"]),
    str(baseline[0]["traffic_globals"]["voice_25d6"]), str(collision["rng_after_3b04"]),
], check=True, capture_output=True, text=True)
assert "event_match=yes" in native.stdout, native.stdout

inputs = [
    REPORTS / (CHECKPOINT + ".state-vblank-01097.state"),
    pre_ram_path,
    REPORTS / (CHECKPOINT + ".actor-trace.json"),
    REPORTS / (BASE + ".actor-trace.json"),
    REPORTS / (PROBE + ".actor-trace.json"),
    REPORTS / (PROBE + ".traffic-follow-entry-trace.json"),
    before_actor,
    after_actor,
    PROJECT / "assets/traffic/city1-roads.json",
    PROJECT / "assets/traffic/city1-road-134.ctrp",
    PROJECT / "assets/driving.ctr",
    PROJECT / "tools/capture_original_libretro.py",
    PROJECT / "tools/rebuild_traffic_follow_exact_event_probe.ps1",
    PROJECT / "tests/traffic_follow_exact_event_probe.c",
    exe,
]
report = {
    "status": "PASS_COUNTER5294_TARGET_EVENT_WITH_BOUNDED_HOOK_PERTURBATION",
    "rom_sha256": entry_trace["instrumentation"]["source_rom_sha256"],
    "archived_route_baseline_exact_prefix_frames": len(prefix),
    "pre_event_counter": 5293,
    "short_untouched_vs_hooked_frames": len(baseline),
    "matched_fields_except_actor_pool": [key for key in fields if key != "traffic_actor_records"],
    "instrumentation_actor_pool_differences": pool_mismatches,
    "actor_pool_slot_count": 50,
    "routine": "original 0x08006908 traffic-follow callback",
    "source_call_order_at_counter5294": call_summary,
    "first_collision": {
        "slot": 1,
        "route": 134,
        "position": collision["player_position"],
        "heading": collision["player_heading"],
        "speed_before_after": [collision["player_speed_544"], collision["player_after_speed_544"]],
        "cooldown_before_after": [collision["cooldown_e4"], collision["cooldown_after_e4"]],
        "rng_before_after": [collision["rng_3b04"], collision["rng_after_3b04"]],
        "entry_actor_matches_pre_event_pool": True,
        "return_actor_matches_post_event_pool": True,
    },
    "native_callback_probe": {
        "status": "PASS",
        "stdout": native.stdout.strip(),
        "binary_sha256": sha(exe),
        "scope": "Exact route134 actor transition, collision result, speed, cooldown, voice and RNG match. The taxi gear record uses a minimal synthetic seed; this isolated routine result does not prove full vehicle powertrain state or general dispatcher parity.",
    },
    "scope_limit": "The short instrumentation run changes unrelated slots10 and11 on its first post-event snapshot, then the actor tables match again on the next two frames. The slot1/route134 callback input and return independently match the untouched pre/post source pool; do not promote the hooked pool or unlogged actor evolution as globally timing-neutral.",
    "artifact_sha256": {path.name: sha(path) for path in inputs},
    "project_progress": {
        "estimate_percent": 30,
        "confidence": "low",
        "rough_days_remaining": "180-365",
        "playable_native_port_complete": False,
    },
}
output = REPORTS / "counter5294-traffic-event-audit.json"
output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({"status": report["status"], "native": native.stdout.strip(), "output": str(output)}))
