"""Cross-check the exact counter5294 traffic-collision powertrain inputs.

The collision callback trace captures entry speed, while untouched EWRAM and
post-frame snapshots provide stable gear-response parameters and output state.
This is a diagnostic cross-check; it does not claim full dispatcher parity.
"""
import hashlib
import json
import pathlib
import struct
import subprocess

PROJECT = pathlib.Path(__file__).resolve().parents[1]
REPORTS = PROJECT / "reports/original-core-reference"
CHECKPOINT = "r5294base1100"
BASELINE = "r5294plain3"
HOOKED = "r5294followprobe"


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def i32(value):
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


def read_s32(data, offset):
    return struct.unpack_from("<i", data, offset)[0]


def player_actor(frame):
    raw = bytes.fromhex(frame["traffic_player_state"]["actor"])
    assert len(raw) == 0xEA0
    return raw


def gear(raw):
    return {
        "flags_4ac": raw[0x4AC],
        "ratio_0_4b8": read_s32(raw, 0x4B8),
        "final_ratio_4d8": read_s32(raw, 0x4D8),
        "scale_5b8": read_s32(raw, 0x5B8),
        "scale_5cc": read_s32(raw, 0x5CC),
        "boost_75c": read_s32(raw, 0x75C),
        "speed_544": read_s32(raw, 0x544),
        "maximum_550": read_s32(raw, 0x550),
        "gear_4dc": raw[0x4DC],
        "rpm_538": struct.unpack_from("<h", raw, 0x538)[0],
        "scale_e84": read_s32(raw, 0xE84),
        "response_timers_e6c_e80": raw[0xE6C:0xE84].hex(),
    }


def c_asr(value, bits):
    return i32(value) >> bits


def source_response(entry_speed, entry_gear):
    """Mirror only the recovered source 0x08006908 player response arithmetic."""
    speed = int(entry_speed / 4)  # C integer division truncates toward zero.
    ratio_product = i32(entry_gear["ratio_0_4b8"] * entry_gear["final_ratio_4d8"])
    ratio = int(ratio_product / 4096)
    mode = (entry_gear["flags_4ac"] >> 2) & 3
    denominator = entry_gear["scale_5b8"] if mode == 1 else entry_gear["scale_5cc"]
    assert mode < 2 and denominator > 0
    rpm_numerator = i32(abs(speed) * 60)
    rpm_base = int(rpm_numerator / denominator)
    rpm_product = i32(rpm_base * abs(ratio))
    rpm = c_asr(rpm_product, 12) & 0xFFFF
    boost = entry_gear["boost_75c"]
    if boost != -1:
        floor = int(boost / 2)
        if rpm >= floor:
            boost = -1
        else:
            rpm = floor
            boost = i32(boost - 50)
    return {"speed_544": speed, "gear_4dc": 1, "rpm_538": rpm,
            "scale_e84": 4096, "boost_75c": boost,
            "response_timers_e6c_e80": "00" * 0x18}


def main():
    pre_ram_path = REPORTS / (CHECKPOINT + ".ewram-vblank-01097.bin")
    pre_ram = pre_ram_path.read_bytes()
    assert len(pre_ram) == 0x40000
    pre = pre_ram[0x2634:0x2634 + 0xEA0]
    baseline_path = REPORTS / (BASELINE + ".actor-trace.json")
    baseline = json.loads(baseline_path.read_text(encoding="utf-8"))
    assert len(baseline) == 3
    assert [frame["counter"] for frame in baseline] == [5294, 5295, 5296]
    post0 = player_actor(baseline[0])
    post1 = player_actor(baseline[1])

    hook_path = REPORTS / (HOOKED + ".traffic-follow-entry-trace.json")
    hook = json.loads(hook_path.read_text(encoding="utf-8"))
    frame_events = [event for event in hook["call_events"]
                    if event["source_counter"] in (5294, 5295)]
    assert len(frame_events) == 9
    assert [event["actor_pointer"] for event in frame_events[-2:]] == ["0x200048c", "0x20004e8"]
    event = next(event for event in frame_events if event["actor_pointer"] == "0x2000150")
    assert event["player_speed_544"] == 97367
    assert event["player_maximum_550"] == 97367
    pre_gear, post_gear0, post_gear1 = gear(pre), gear(post0), gear(post1)
    stable = ("flags_4ac", "ratio_0_4b8", "final_ratio_4d8", "scale_5b8",
              "scale_5cc", "boost_75c")
    assert all(pre_gear[key] == post_gear0[key] == post_gear1[key] for key in stable)
    assert struct.unpack_from("<3i", post0, 0) == tuple(event["player_after_position"])
    assert read_s32(post0, 0x1C) == event["player_after_heading"]
    assert post_gear0["speed_544"] == event["player_after_speed_544"] == 24341

    before_actor_path = REPORTS / "counter5294-source-before.actor"
    after_actor_path = REPORTS / "counter5294-source-after.actor"
    pre_player_path = REPORTS / "counter5294-source-player-before.actor"
    post_player_path = REPORTS / "counter5294-source-player-after.actor"
    pre_ewram_path = REPORTS / "counter5294-source-pre.ewram"
    post_globals_path = REPORTS / "counter5294-source-post.globals"
    before_actor_path.write_bytes(bytes.fromhex(event["actor_raw"]))
    after_actor_path.write_bytes(bytes.fromhex(event["actor_after_raw"]))
    pre_player_path.write_bytes(pre)
    post_player_path.write_bytes(post0)
    pre_ewram_path.write_bytes(pre_ram)
    after_globals = baseline[0]["traffic_globals"]
    post_globals_path.write_bytes(struct.pack(
        "<h4B2i2I", after_globals["cooldown_e4"],
        after_globals["near_miss_25d3"], after_globals["voice_25d6"],
        after_globals["combo_flag_9aa6"], after_globals["timer_25d5"],
        after_globals["combo_9aa8"], after_globals["combo_9aac"],
        after_globals["rng_3b04"], after_globals["selection_counter_4c98"]))

    roads_path = PROJECT / "assets/traffic/city1-roads.json"
    roads = json.loads(roads_path.read_text(encoding="utf-8"))
    descriptor_to_route = {int(item["descriptor_address"], 16): item["index"]
                           for item in roads["paths"]}
    source_pre_pool = pre_ram[0xF4:0xF4 + 50 * 92]
    source_post_pool = bytes.fromhex(baseline[0]["traffic_actor_records"]["raw"])
    replayed_pool = bytearray(source_pre_pool)
    frame_blob = bytearray(struct.pack("<8sI", b"CT5294V1", len(frame_events)))
    current_speed = frame_events[0]["player_speed_544"]
    context = baseline[0]["traffic_frame_context"]
    for call in frame_events:
        slot = (int(call["actor_pointer"], 16) - 0x020000F4) // 92
        assert 0 <= slot < 50
        before_raw, after_raw = bytes.fromhex(call["actor_raw"]), bytes.fromhex(call["actor_after_raw"])
        assert before_raw == source_pre_pool[slot * 92:(slot + 1) * 92]
        assert after_raw == source_post_pool[slot * 92:(slot + 1) * 92]
        replayed_pool[slot * 92:(slot + 1) * 92] = after_raw
        route = descriptor_to_route[struct.unpack_from("<I", before_raw, 0x24)[0]]
        assert call["player_speed_544"] == current_speed
        current_speed = call["player_after_speed_544"]
        collision = int(call["player_after_speed_544"] != call["player_speed_544"] or
                        call["cooldown_after_e4"] != call["cooldown_e4"])
        values = (slot, route, *call["player_position"], call["player_heading"],
                  call["player_speed_544"], call["player_after_speed_544"],
                  call["argument_r1"], call["cooldown_e4"],
                  call["cooldown_after_e4"], call["near_miss_25d3"],
                  call["near_miss_after_25d3"], call["rng_3b04"],
                  call["rng_after_3b04"], context["sound_7602"],
                  context["trip_active_1694"], collision)
        assert len(values) == 18
        frame_blob.extend(struct.pack("<18i", *values))
        frame_blob.extend(before_raw)
        frame_blob.extend(after_raw)
    frame_fixture_path = REPORTS / "counter5294-source-ordered-events.bin"
    frame_fixture_path.write_bytes(frame_blob)
    outside_callback_slots = [slot for slot in range(50)
                              if replayed_pool[slot * 92:(slot + 1) * 92] !=
                              source_post_pool[slot * 92:(slot + 1) * 92]]
    assert outside_callback_slots == [0, 2, 3], outside_callback_slots
    timer_slots = []
    frame_delta = baseline[0]["frame_delta"]
    for slot in outside_callback_slots:
        start, end = slot * 92, (slot + 1) * 92
        before_record = source_pre_pool[start:end]
        after_record = source_post_pool[start:end]
        assert struct.unpack_from("<I", before_record, 0)[0] & 1
        countdown = read_s32(before_record, 0x10)
        assert countdown > 0
        predicted = bytearray(before_record)
        struct.pack_into("<i", predicted, 0x10, countdown - frame_delta)
        assert bytes(predicted) == after_record
        replayed_pool[start:end] = predicted
        timer_slots.append({"slot": slot, "countdown_before": countdown,
                            "countdown_after": read_s32(after_record, 0x10),
                            "frame_delta": frame_delta,
                            "only_countdown_changed": True})
    assert replayed_pool == source_post_pool

    predicted = source_response(event["player_speed_544"], pre_gear)
    observed = {key: post_gear0[key] for key in predicted}
    assert predicted == observed, {"predicted": predicted, "source": observed}

    report = {
        "status": "PASS_COUNTER5294_SOURCE_POWERTRAIN_INPUTS_CROSSCHECK",
        "rom_sha256": hook["instrumentation"]["source_rom_sha256"],
        "source_event": {
            "counter": event["source_counter"], "actor_pointer": event["actor_pointer"],
            "entry_speed_544": event["player_speed_544"],
            "entry_maximum_550": event["player_maximum_550"],
            "post_speed_544": event["player_after_speed_544"],
        },
        "unmodeled_source_actor_slots_after_nine_callback_window": [],
        "whole_actor_pool_reconciliation": {"status": "PASS", "actor_count": 50,
            "callback_slots": [(int(call["actor_pointer"], 16) - 0x020000F4) // 92 for call in frame_events],
            "countdown_only_slots": timer_slots,
            "all_actor_records_match_untouched_post_pool": True},
        "powertrain_parameters_unchanged_in_untouched_snapshots": {
            key: pre_gear[key] for key in stable
        },
        "source_response_check": {
            "predicted": predicted, "untouched_source_post_event": observed,
            "match": True,
            "gear_entry_note": "The recovered response assigns gear=1 before recalculating RPM; the entry gear byte is not a response input.",
        },
        "project_progress": {
            "estimate_percent": 30,
            "confidence": "low",
            "rough_days_remaining": "180-365",
            "playable_native_port_complete": False,
        },
        "input_sha256": {
            pre_ram_path.name: sha(pre_ram_path),
            baseline_path.name: sha(baseline_path),
            hook_path.name: sha(hook_path),
        },
        "scope": "Exact route134 source speed plus stable source ratio/scaling/boost fields reproduce the untouched source post-event speed, gear, RPM, scale, boost, and response timers. Source gear parameters are recovered from counter5293 EWRAM and remain identical in untouched counter5294/5295 snapshots. The callback capture is instrumented; only individually matched actor records and logged per-call inputs/outputs are used, not its perturbed whole-pool table. The nine-callback window remains short of full dispatcher-frame parity or playable gameplay.",
    }
    native_exe = PROJECT / "bin/traffic-follow-source-powertrain-probe.exe"
    if native_exe.exists():
        native = subprocess.run([
            str(native_exe), str(before_actor_path), str(after_actor_path),
            str(PROJECT / "assets/traffic/city1-road-134.ctrp"),
            str(PROJECT / "assets/driving.ctr"), str(pre_player_path),
            str(post_player_path), str(pre_ewram_path), str(post_globals_path),
            str(event["player_position"][0]), str(event["player_position"][2]),
            str(event["player_heading"]), str(event["player_speed_544"]),
            str(event["argument_r1"])], check=True, capture_output=True, text=True)
        assert "actor_exact=yes" in native.stdout and "powertrain_exact=yes" in native.stdout and "globals_exact=yes" in native.stdout, native.stdout
        report["native_powertrain_probe"] = {
            "status": "PASS", "stdout": native.stdout.strip(),
            "binary_sha256": sha(native_exe),
            "scope": "Runs the native callback with the exact event speed and source-derived gear parameters; compares actor, collision, player response and traffic globals to untouched source outputs.",
        }
        report["scope"] += " Native callback execution also passed against the source event input/output records."
    else:
        report["native_powertrain_probe"] = {
            "status": "NOT_RUN", "reason": "Build tools/rebuild_counter5294_source_powertrain_probe.ps1 first.",
        }
    frame_exe = PROJECT / "bin/traffic-follow-source-frame-probe.exe"
    if frame_exe.exists():
        frame_native = subprocess.run([
            str(frame_exe), str(frame_fixture_path), str(pre_player_path),
            str(pre_ewram_path), str(post_player_path), str(post_globals_path)],
            cwd=PROJECT.parent.parent, check=True, capture_output=True, text=True)
        assert ("callbacks=9" in frame_native.stdout and
                "actor_returns_exact=yes" in frame_native.stdout and
                "powertrain_exact=yes" in frame_native.stdout and
                "globals_exact=yes" in frame_native.stdout), frame_native.stdout
        report["native_source_ordered_callback_window_replay"] = {
            "status": "PASS", "stdout": frame_native.stdout.strip(),
            "binary_sha256": sha(frame_exe), "fixture_sha256": sha(frame_fixture_path),
            "actor_slots": [(int(call["actor_pointer"], 16) - 0x020000F4) // 92
                            for call in frame_events],
            "observed_source_counters": [call["source_counter"] for call in frame_events],
            "scope": "Runs the nine observed 0x08006908 callbacks in logged order across source counters5294 and5295 with shared player powertrain/globals carried forward. Each actor return and callback speed/cooldown/RNG state matches source. The two final actor records individually match untouched pre/post pools even though their logged counter is one greater. Generic countdown-only slots0,2,3 are validated separately. This is a multi-counter callback window, not proof of one complete dispatcher frame or its exact VBlank ordering.",
        }
        report["scope"] += " The nine observed follow callbacks match in native callback-window replay, but their counter labels straddle 5294/5295 and do not establish a single dispatcher frame."
    else:
        report["native_source_ordered_callback_window_replay"] = {
            "status": "NOT_RUN", "reason": "Build tools/rebuild_counter5294_source_powertrain_probe.ps1 first.",
        }
    for path in (before_actor_path, after_actor_path, pre_player_path, post_player_path,
                 pre_ewram_path, post_globals_path, roads_path, frame_fixture_path):
        report["input_sha256"][path.name] = sha(path)
    output = REPORTS / "counter5294-source-powertrain-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "predicted": predicted,
                      "source": observed, "output": str(output)}))


if __name__ == "__main__":
    main()
