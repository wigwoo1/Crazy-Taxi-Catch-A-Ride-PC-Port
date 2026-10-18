"""Summarize source-batch and delta-1 reference-probe divergence points."""
from __future__ import annotations
import hashlib, json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
PATTERN = re.compile(r"REFERENCE DIVERGENCE step=(\d+).*?tick=(\d+).*?expected=(-?\d+),(-?\d+),(-?\d+),(-?\d+),(-?\d+).*?actual=(-?\d+),(-?\d+),(-?\d+),(-?\d+),(-?\d+)")

def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def parse(path: pathlib.Path) -> dict:
    match = PATTERN.search(path.read_text(encoding="utf-8"))
    if not match:
        raise SystemExit(f"No reference divergence record in {path}")
    nums = [int(value) for value in match.groups()]
    return {"step": nums[0], "tick": nums[1], "expected": {"position": nums[2:5], "heading": nums[5], "speed": nums[6]},
            "actual": {"position": nums[7:10], "heading": nums[10], "speed": nums[11]}}

def parse_native_states(path: pathlib.Path) -> list[dict]:
    text = path.read_text(encoding="utf-8")
    pattern = re.compile(r"NATIVE_STATE update=(\d+) pos=(-?\d+),(-?\d+),(-?\d+) heading=(-?\d+),(-?\d+).*? speed=(-?\d+).*?displacement=(-?\d+),(-?\d+),(-?\d+)\\nNATIVE_GEAR.*?steer=(-?\d+).*?NATIVE_GLOBAL.*?heldA=(\d+) prevA=(\d+) repeatA=(\d+) pressA=(\d+) countA=(\d+)", re.S)
    states = []
    for match in pattern.finditer(text):
        n = [int(value) for value in match.groups()]
        states.append({"update": n[0], "position": n[1:4], "heading": n[4], "speed": n[6],
                       "displacement": n[7:10], "steering_angle": n[10],
                       "A": {"held": n[11], "previous": n[12], "repeat": n[13], "pressed": n[14], "countdown": n[15]}})
    return states

def main() -> None:
    batch_log = REF / "rboot4200delta600.pose.native.stderr"
    delta_log = REF / "rboot4200delta600.pose.delta1.native.stderr"
    original_batch_log = REF / "rboot4200delta600.native-batch.stderr"
    delta5_trace = REF / "rboot4200delta600.first-batch-delta5.native.log"
    delta1_trace = REF / "rboot4200delta600.first-batch-delta1.native.log"
    trace = REF / "rboot4200delta600.actor-trace.json"
    batch_schedule = REF / "rboot4200delta600.pose.schedule.txt"
    delta_schedule = REF / "rboot4200delta600.pose.delta1.schedule.txt"
    batch_speed = parse(original_batch_log)
    report = {
        "status": "PASS_DIAGNOSTIC_SOURCE_BATCH_VS_DELTA1_COMPARISON",
        "source_rom_sha256": ROM,
        "source_trace_sha256": sha(trace),
        "source_trace": trace.name,
        "source_update_count": 120,
        "source_frame_delta_histogram": {"4": 14, "5": 93, "6": 12},
        "native_delta1_step_count": 602,
        "source_batch_pose_heading_only": parse(batch_log),
        "delta1_pose_heading_only": parse(delta_log),
        "source_batch_speed_control": batch_speed,
        "first_batch_native_substeps": {"delta5": parse_native_states(delta5_trace), "delta1": parse_native_states(delta1_trace)},
        "first_batch_interpretation": "In the native trace, a single delta-5 update ends at speed1084 and moves by(3,0,4); five delta-1 updates end at speed1080 and do not accumulate a whole-unit displacement. Both end with steering angle -640. The A input has no repeat on ticks2-5 (countdown59..56), so repeat events cannot explain this first pose difference. This is consistent with per-call integer truncation in acceleration.c (divide by 60/delta) and motion.c (delta*speed/60 followed by fixed-point shifts); it is evidence about the recovered native implementation, not an independent GBA instruction-level proof.",
        "speed_control_interpretation": "At source counter 4286 (native schedule step 19), car position and heading matched, but source shared speed was 5535 and native was 22140. Separate per-actor entry/return evidence records actor0 changing the shared speed during this dispatch. The pose-only comparison therefore proceeds past this speed-owner mismatch; it is not a full source-batch pass.",
        "result": "The delta-1 schedule first differs in pose and heading at its first five-substep endpoint (tick 5). The five-VBlank source-batched schedule matches pose and heading through 19 update endpoints, then differs in position at its next endpoint (tick 96), with heading still equal. This demonstrates that the current recovered native routine and held-input schedule are not delta-1 equivalent; it does not identify a correct production 60 Hz simulation policy.",
        "scope": "Offline recovered driving-routine diagnostic only. Source schedule came from an adaptive scripted pickup-route capture that did not complete a fare. It does not establish complete game, traffic, passenger, Xbox, rendering, or playable-port parity.",
        "files": {},
    }
    for path in (trace, batch_schedule, delta_schedule, batch_log, delta_log, original_batch_log, delta5_trace, delta1_trace):
        report["files"][path.name] = sha(path)
    output = REF / "native-update-cadence-comparison-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "report": str(output),
                      "delta1_first_divergence": report["delta1_pose_heading_only"],
                      "batch_first_pose_divergence": report["source_batch_pose_heading_only"],
                      "speed_owner_control": report["source_batch_speed_control"]}, indent=2))

if __name__ == "__main__":
    main()
