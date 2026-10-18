#!/usr/bin/env python3
"""Verify the captured voice-owner transitions and traffic dispatch phase probe."""
import hashlib
import json
import pathlib
import re
import struct

ROOT = pathlib.Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
EXPECTED_ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"


def read_json(name):
    return json.loads((REPORTS / name).read_text(encoding="utf-8"))


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def lfsr(state):
    state = state or 0x4652414E
    state = (state << 1) & 0xFFFFFFFF
    if state & 0x80000000:
        state ^= 0x1D872B41
    return ((state & 0x7FFFFFFF) ^ 0x06000000) & 0x7FFFFFFF


def rows_jsonl(name, last):
    with (REPORTS / name).open(encoding="utf-8") as stream:
        header = json.loads(next(stream))["header"]
        rows = [json.loads(next(stream)) for _ in range(last + 1)]
    return header, rows


def verify_trace_pair(baseline, instrumented, fields=None):
    if len(baseline) != len(instrumented):
        return False, ["length"]
    differences = []
    for index, (left, right) in enumerate(zip(baseline, instrumented)):
        keys = fields or set(left) | set(right)
        for key in keys:
            if left.get(key) != right.get(key):
                differences.append({"row": index, "field": key})
                break
    return not differences, differences[:8]


def main():
    # The two-call trace uses the smallest instrumentation set, with a paired
    # unmodified capture to catch changes caused by the temporary ROM wrapper.
    short_base = read_json("voiceeventbaseline510.actor-trace.json")
    short_probe = read_json("voiceeventtrace510.actor-trace.json")
    short_equal, short_diff = verify_trace_pair(short_base, short_probe)
    calls = read_json("voiceeventtrace510.rng-callers.json")
    events = calls["call_events"]
    expected_events = [
        {"tick": 499, "call_site": "0x800b19c", "caller_r5": "0x2007004"},
        {"tick": 499, "call_site": "0x800b1b4", "caller_r5": "0x2007004"},
    ]
    observed_events = [{k: event[k] for k in ("tick", "call_site", "caller_r5")} for event in events]
    assert calls["instrumentation"]["source_rom_sha256"] == EXPECTED_ROM
    assert calls["instrumentation"]["call_sites"] == ["0x800b19c", "0x800b1b4"]
    assert observed_events == expected_events, observed_events
    assert short_equal, short_diff

    # Check the exact two source RNG calls against packed source checkpoints.
    fixture_header, rows = rows_jsonl("route31poolcontext4-native-traffic-fixture.jsonl", 104)
    before, after = rows[101]["source"], rows[102]["source"]
    rng_before = before["globals"]["rng_3b04"]
    first = lfsr(rng_before)
    second = lfsr(first)
    first_result, second_result = first % 65536, second % 65536
    mode = first_result % 2 + 2
    voice = (second_result & 1) + 20
    owner_offset = 32 + 192 + 32 + 16 + 32
    owner = bytes.fromhex(after["player_state"]["shared_owners"]["020025D0"])
    assert (first, second) == (1752365651, 1264771047)
    assert rng_before == 2046486665 and after["globals"]["rng_3b04"] == second
    assert before["globals"]["voice_25d6"] == 255 and after["globals"]["voice_25d6"] == voice == 21
    assert owner[6] == voice and owner[7] == mode == 3
    assert rows[101]["schedule"][9] == 494 and rows[102]["schedule"][9] == 500

    # Capture with dispatcher entry instrumentation proves the hook itself did
    # not alter the 700-frame original execution, including all 50 actor bytes.
    dispatch_base = read_json("dispatchbaseline700.actor-trace.json")
    dispatch_probe = read_json("dispatchcadence700.actor-trace.json")
    dispatch_equal, dispatch_diff = verify_trace_pair(dispatch_base, dispatch_probe)
    dispatch = read_json("dispatchcadence700.traffic-dispatch-entry-trace.json")
    assert dispatch["instrumentation"]["source_rom_sha256"] == EXPECTED_ROM
    assert dispatch["logged_count"] == len(dispatch["call_events"]) < dispatch["instrumentation"]["ewram_log_count_capacity"]
    assert not dispatch["log_saturated"] and dispatch_equal, dispatch_diff
    near_entries = [e for e in dispatch["call_events"] if 4690 <= e["source_counter"] <= 4730]
    assert [(e["observed_after_core_tick"], e["source_counter"], e["frame_delta"]) for e in near_entries] == [
        (499, 4695, 6), (505, 4701, 6), (515, 4711, 6), (520, 4716, 9),
        (525, 4721, 5), (531, 4727, 6),
    ]
    pool_changes = [
        {"tick": row["tick"], "slots": row["traffic_pool_changed_from_previous_vblank"]}
        for row in dispatch_base[500:521]
        if row.get("traffic_pool_changed_from_previous_vblank") is not None
    ]
    assert [item["tick"] for item in pool_changes] == [505, 515, 520]
    assert dispatch_base[511]["traffic_camera"]["position"] == [-246527, -37657, 271961]
    assert dispatch_base[511]["traffic_actor_records"]["raw"] == dispatch_base[510]["traffic_actor_records"]["raw"]
    assert dispatch_base[514]["traffic_actor_records"]["raw"] == dispatch_base[510]["traffic_actor_records"]["raw"]

    # Full continuous replay consumes the observed reset and selection writes;
    # it must stop at the next real actor divergence, without checkpoint rebases.
    replay = {}
    for label in ("y0", "y1"):
        log = (REPORTS / f"voiceowner-dispatch-replay-{label}.log").read_text(encoding="utf-8")
        match = re.search(r"TRAFFIC_REPLAY matched=(\d+).*?rng_rebases=(\d+) shared_voice_rebases=(\d+) actor_mismatches=(\d+) actor_pool_reseeds=(\d+) stop=(.*?) scope=", log)
        assert match, label
        parsed = {"matched_rows": int(match[1]), "rng_rebases": int(match[2]), "voice_rebases": int(match[3]),
                  "actor_mismatches": int(match[4]), "actor_pool_reseeds": int(match[5]), "stop": match[6]}
        assert parsed["matched_rows"] == 104 and not any(parsed[k] for k in ("rng_rebases", "voice_rebases", "actor_mismatches", "actor_pool_reseeds")), parsed
        assert "actor0 flags=0/0 type=0/0 countdown=180/-1" in parsed["stop"]
        replay[label] = parsed

    instructions = json.loads((ROOT / "reports/analysis.json").read_text(encoding="utf-8"))["instructions"]
    address_map = {item["address"]: {"bytes": item["bytes"], "mnemonic": item["mnemonic"], "operands": item["operands"]}
                   for item in instructions if item["address"] in {"0x800b19c", "0x800b1a0", "0x800b1a2", "0x800b1a4", "0x800b1a6", "0x800b1a8", "0x800b1aa", "0x800b1ac", "0x800b1ae", "0x800b1b4", "0x800b1b8", "0x800b1ba", "0x800b1bc", "0x800b1be"}}
    source_paths = [
        ROOT / "tools/capture_original_libretro.py", ROOT / "tests/native_traffic_dispatch_probe.c",
        ROOT / "src/random.c", ROOT / "reports/analysis.json",
        REPORTS / "voiceeventtrace510.rng-callers.json", REPORTS / "dispatchcadence700.traffic-dispatch-entry-trace.json",
        REPORTS / "route31poolcontext4-native-traffic-dispatch.bin",
        REPORTS / "voiceowner-dispatch-replay-y0.log", REPORTS / "voiceowner-dispatch-replay-y1.log",
    ]
    report = {
        "status": "PASS_VOICE_OWNERS_REPRODUCED_NEXT_DISPATCH_PHASE_UNRESOLVED",
        "source_rom_sha256": EXPECTED_ROM,
        "focused_rng_trace": {"paired_frames": len(short_base), "unmodified_vs_instrumented_all_fields_equal": short_equal,
                               "call_events": observed_events},
        "voice_selection_source_transition": {"fixture": "route31poolcontext4-native-traffic-fixture.jsonl", "from_row": 101, "to_row": 102,
            "source_rng_before": rng_before, "native_rng_after_two_original_draws": second, "source_rng_after": after["globals"]["rng_3b04"],
            "source_voice_before": before["globals"]["voice_25d6"], "native_voice_after": voice,
            "source_voice_after": after["globals"]["voice_25d6"], "source_adjacent_mode_byte": owner[7],
            "observed_caller_register": "r5=0x02007004", "static_instructions": address_map,
            "scope": "The two original RNG calls and their two bytes reproduce one captured source boundary; no audio playback or complete customer/voice subsystem claim."},
        "dispatcher_phase_capture": {"frames": len(dispatch_base), "entries": dispatch["logged_count"], "capacity": dispatch["instrumentation"]["ewram_log_count_capacity"],
            "log_saturated": dispatch["log_saturated"], "unmodified_vs_instrumented_all_fields_equal": dispatch_equal,
            "entry_events_near_boundary": [{k: e[k] for k in ("observed_after_core_tick", "source_counter", "frame_delta")} for e in near_entries],
            "full_pool_change_ticks": pool_changes,
            "camera_switch_tick": 511, "camera_before": dispatch_base[510]["traffic_camera"]["position"],
            "camera_after": dispatch_base[511]["traffic_camera"]["position"],
            "actor_pool_unchanged_ticks": [511, 512, 513, 514],
            "next_entry_after_camera_switch": {"tick": 515, "source_counter": 4711}},
        "continuous_native_diagnostic_replay": replay,
        "interpretation": "The exact voice reset and two-draw selection remove the earlier shared-owner stops. Both preserve-query-Y branches now match 104 source rows with zero actor mismatches or RNG/voice rebases. The first mismatch appears when native presentation culls actor 0 to countdown 180 while source remains -1; source rows 103 and 104 have identical actor pools but the source camera changes sharply. The 700-frame original trace shows the actor pool remains unchanged across that camera transition until the next dispatcher entry at tick515. This points to a service-entry phase/order mismatch, not a recovered actor-state transition. No one-row actor override was added.",
        "full_playable_port_claim": False,
        "rom_or_production_runtime_modified": False,
        "asset_upscale_text_policy": "Fonts, HUD, menus, logos, signage and lettering remain original and excluded from any upscale; only verified text-free scenery may be considered.",
        "files_sha256": {str(path.relative_to(ROOT)): sha(path) for path in source_paths},
    }
    output = REPORTS / "voice-owner-dispatch-phase-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "report": str(output), "sha256": sha(output),
                      "rows_matched": 104, "unresolved": "source dispatcher entry/cadence mapping at first camera-cull divergence"}, indent=2))


if __name__ == "__main__":
    main()
