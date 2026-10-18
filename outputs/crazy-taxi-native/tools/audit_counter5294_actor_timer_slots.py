"""Audit the untouched actor-pool delta at source counter 5294.

This distinguishes timer-only rows from moving actors without treating the
instrumented actor hook as source truth. A matching timer equation does not by
itself prove the original dispatcher selected the native routine.
"""
from __future__ import annotations
import hashlib
import json
import pathlib
import struct

ROOT = pathlib.Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
BEFORE = REPORTS / "r5294base1100.ewram-vblank-01097.bin"
TRACE = REPORTS / "r5294plain3.actor-trace.json"
FOLLOW_TRACE = REPORTS / "r5294followprobe.actor-trace.json"
DISPATCH_TRACE = REPORTS / "r5294dispatch3.actor-trace.json"
DISPATCH_EVENTS = REPORTS / "r5294dispatch3.traffic-dispatch-entry-trace.json"
HOOK = REPORTS / "r5294followprobe.traffic-follow-entry-trace.json"
OUTPUT = REPORTS / "counter5294-untouched-actor-delta-audit.json"
BASE = 0xF4
STRIDE = 0x5C
SLOTS = (0, 2, 3, 10, 11)

def s32(raw: bytes, offset: int) -> int:
    return struct.unpack_from("<i", raw, offset)[0]

def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

before_ram = BEFORE.read_bytes()
frames = json.loads(TRACE.read_text(encoding="utf-8"))
assert len(before_ram) == 0x40000
frame = next(row for row in frames if row["counter"] == 5294)
assert frame["frame_delta"] == 5
assert frame["traffic_frame_context"]["freeze_5f20"] == 0
after = bytes.fromhex(frame["traffic_actor_records"]["raw"])
assert len(after) == STRIDE * 50
rows = []
for slot in SLOTS:
    o = slot * STRIDE
    old = before_ram[BASE + o:BASE + o + STRIDE]
    new = after[o:o + STRIDE]
    changed = [i for i, (a, b) in enumerate(zip(old, new)) if a != b]
    rows.append({
        "slot": slot,
        "flags": struct.unpack_from("<I", old, 0)[0],
        "type": struct.unpack_from("<I", old, 4)[0],
        "countdown_before": s32(old, 0x10),
        "countdown_after": s32(new, 0x10),
        "position_x_before_after": [s32(old, 0x14), s32(new, 0x14)],
        "position_z_before_after": [s32(old, 0x1C), s32(new, 0x1C)],
        "path_distance_before_after": [s32(old, 0x28), s32(new, 0x28)],
        "changed_record_offsets": changed,
        "timer_rule_matches": bool(s32(old, 0x10) > 0 and s32(new, 0x10) == s32(old, 0x10) - frame["frame_delta"]),
        "countdown_only": all(0x10 <= i < 0x14 for i in changed),
    })
for row in rows[:3]:
    assert row["flags"] & 1
    assert row["timer_rule_matches"] and row["countdown_only"]
for row in rows[3:]:
    assert row["flags"] & 1
    assert row["position_x_before_after"][0] != row["position_x_before_after"][1]
    assert row["path_distance_before_after"][1] == row["path_distance_before_after"][0] + frame["frame_delta"]

# The hook's full-pool sample is timing-perturbed, but the two requested actor
# records can still be compared independently with untouched source snapshots.
hook = json.loads(HOOK.read_text(encoding="utf-8"))
for row in rows[3:]:
    slot = row["slot"]
    pointer = f"0x{0x020000F4 + slot * STRIDE:07x}"
    event = next(e for e in hook["call_events"] if e["actor_pointer"].lower() == pointer.lower())
    entry = bytes.fromhex(event["actor_raw"])
    returned = bytes.fromhex(event["actor_after_raw"])
    source_before = before_ram[BASE + slot * STRIDE:BASE + (slot + 1) * STRIDE]
    source_after = after[slot * STRIDE:(slot + 1) * STRIDE]
    assert entry == source_before, f"slot{slot} callback entry differs from untouched source actor"
    assert returned == source_after, f"slot{slot} callback return differs from untouched source actor"
    row["follow_callback_entry_matches_untouched_pre_actor"] = True
    row["follow_callback_return_matches_untouched_post_actor"] = True
    row["observed_callback_counter"] = event["source_counter"]
    row["observed_callback_pointer"] = event["actor_pointer"]
    row["callback_vs_source_counter_note"] = "Callback logger labels this counter 5295 although its actor input/output exactly spans the untouched counter5293->5294 actor-pool snapshots; call timing/order remains unresolved."
adjacent_timer_checks = []
for prior, current in zip(frames, frames[1:]):
    prior_pool = bytes.fromhex(prior["traffic_actor_records"]["raw"])
    current_pool = bytes.fromhex(current["traffic_actor_records"]["raw"])
    changed_slots = []
    for slot in range(50):
        start, end = slot * STRIDE, (slot + 1) * STRIDE
        before_record = prior_pool[start:end]
        after_record = current_pool[start:end]
        if before_record == after_record:
            continue
        changed_offsets = [i for i, (a, b) in enumerate(zip(before_record, after_record)) if a != b]
        assert all(0x10 <= i < 0x14 for i in changed_offsets), (prior["counter"], slot, changed_offsets)
        countdown = s32(before_record, 0x10)
        delta = current["frame_delta"]
        assert countdown > 0 and s32(after_record, 0x10) == countdown - delta
        predicted = bytearray(before_record)
        struct.pack_into("<i", predicted, 0x10, countdown - delta)
        assert bytes(predicted) == after_record
        changed_slots.append(slot)
    adjacent_timer_checks.append({
        "counter_transition": [prior["counter"], current["counter"]],
        "frame_delta": current["frame_delta"],
        "countdown_only_slots": changed_slots,
        "full_actor_pool_matches_generic_timer_model": True,
    })
assert adjacent_timer_checks[0]["countdown_only_slots"] == list(range(12, 27))
assert adjacent_timer_checks[1]["countdown_only_slots"] == []
def changed_slot_rows(path: pathlib.Path):
    trace = json.loads(path.read_text(encoding="utf-8"))
    return [{"tick": row["tick"], "counter": row["counter"],
             "changed_slots": row.get("traffic_pool_changed_from_previous_vblank", [])}
            for row in trace]

baseline_schedule = changed_slot_rows(TRACE)
follow_hook_schedule = changed_slot_rows(FOLLOW_TRACE)
dispatch_hook_schedule = changed_slot_rows(DISPATCH_TRACE)
dispatch = json.loads(DISPATCH_EVENTS.read_text(encoding="utf-8"))
assert [row["changed_slots"] for row in baseline_schedule] == [list(range(12)), list(range(12, 27)), []]
assert [row["changed_slots"] for row in follow_hook_schedule] == [list(range(10)), list(range(10, 27)), []]
assert [row["changed_slots"] for row in dispatch_hook_schedule] == [list(range(27)), [], []]
assert len(dispatch["call_events"]) == 1
assert dispatch["call_events"][0]["source_counter"] == 5294
assert dispatch["call_events"][0]["observed_after_core_tick"] == 0
report = {
    "status": "PASS_UNTOUCHED_COUNTER5294_TIMER_ROWS_SEPARATED_FROM_MOVING_ACTORS",
    "adjacent_source_frame_timer_checks": adjacent_timer_checks,
    "dispatcher_vblank_schedule_comparison": {
        "untouched": baseline_schedule,
        "follow_hooked": follow_hook_schedule,
        "dispatcher_entry_hooked": dispatch_hook_schedule,
        "dispatcher_entry_events": dispatch["call_events"],
        "finding": "The untouched capture changes actor slots0-11 at tick0 and countdown-only slots12-26 at tick1, with no changed actors at tick2. A separate follow-hook capture splits slots0-9 and10-26 across ticks0/1; a dispatcher-entry-hook capture moves slots0-26 into tick0. Both diagnostic hooks therefore alter sampled scheduling. The dispatcher-only trace observes one entry at tick0/counter5294, so the later untouched tick1 actor changes do not correspond to a second logged dispatcher entry and are consistent with an in-flight dispatcher service spanning a VBlank boundary. This explains the counter-label mismatch as instrumentation-sensitive phase timing, but does not prove the exact source instruction at the boundary; hooked VBlank tables are excluded as canonical state."
    },
    "source_rom_sha256": "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5",
    "counter": frame["counter"], "frame_delta": frame["frame_delta"],
    "pause_or_freeze": frame["traffic_frame_context"]["freeze_5f20"],
    "slots": rows,
    "interpretation": "Slots0,2,3 change only countdown by the frame delta, exactly as the recovered native generic traffic-frame timer rule predicts for positive timers. This is source-state consistency, not proof that this code path executed in this original frame. Slots10,11 have flags bit0 set and move by five path units while countdown is nonpositive; their source owner/order remains unresolved. The failed counter5294 0x08006CE8 one-shot hook is excluded because it did not fire and its capture was instrumented.",
    "scope": "Untouched source checkpoint EWRAM compared with the first plain source VBlank at counter5294. Does not claim whole-frame parity or playable gameplay.",
    "input_sha256": {str(BEFORE.name): sha(BEFORE), str(TRACE.name): sha(TRACE), str(HOOK.name): sha(HOOK),
                       str(FOLLOW_TRACE.name): sha(FOLLOW_TRACE),
                       str(DISPATCH_TRACE.name): sha(DISPATCH_TRACE),
                       str(DISPATCH_EVENTS.name): sha(DISPATCH_EVENTS)},
}
OUTPUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({"status": report["status"], "report": str(OUTPUT), "slots": rows}, indent=2))




