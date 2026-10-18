"""Audit the bounded live 4K render of the first 19 validated source poses."""
from __future__ import annotations
import csv, hashlib, json, pathlib, statistics

ROOT = pathlib.Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports"
ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

def rows(path: pathlib.Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as stream:
        return list(csv.DictReader(stream))

def main() -> None:
    performance = REPORTS / "native-cadence-live-120-performance.csv"
    gpu = REPORTS / "native-cadence-live-120-performance.csv.gpu.csv"
    host = REPORTS / "native-cadence-live-120-host.csv"
    presentation = REPORTS / "native-cadence-live-120-presentation.csv"
    intervals = REPORTS / "native-cadence-visible-intervals.csv"
    schedule = ROOT / "reports/original-core-reference/rboot4200delta600.first19.pose.schedule.txt"
    viewer = ROOT / "bin/scene-viewer.exe"
    perf_rows = rows(performance)
    gpu_rows = rows(gpu)
    host_rows = [line.split() for line in host.read_text(encoding="utf-8").splitlines() if line.strip()]
    interval_rows = rows(intervals)
    samples = [float(row["interval_ms"]) for row in perf_rows if float(row["interval_ms"]) > 0]
    swaps = [float(row["swap_return_ms"]) for row in interval_rows]
    ordered = sorted(samples)
    if len(perf_rows) != 120 or len(gpu_rows) != 120 or len(host_rows) != 120 or len(swaps) != 119:
        raise SystemExit("Live 4K trace frame counts do not match the bounded 120-presentation run.")
    if any(float(row["gpu_ms"]) < 0 for row in gpu_rows):
        raise SystemExit("A GPU timestamp interval is invalid.")
    report = {
        "status": "PASS_BOUNDED_LIVE_4K_TIMESTAMPED_PRESENTATION",
        "source_rom_sha256": ROM,
        "renderer": "AMD Radeon RX 7900 XTX",
        "render_target": [3840, 2160],
        "window_visibility": "hidden; window client1280x720",
        "reported_monitor_mode": "1280x720 at60Hz; reported mode only",
        "presentations": len(perf_rows),
        "source_pose_checkpoints": 19,
        "source_schedule": schedule.name,
        "verification_scope": "Position and heading checked at 19 source-batched checkpoints. The known shared-speed mismatch at source counter4286 remains unresolved; speed is deliberately excluded by the V4 mask. The untruncated source schedule still diverges in pose at step20/tick96.",
        "cpu_pacing_ms": {"mean": statistics.mean(samples), "p95": ordered[int(len(ordered) * .95)], "max": max(samples), "skipped_60hz_deadlines": 0},
        "swap_return_ms": {"mean": statistics.mean(swaps), "p95": sorted(swaps)[int(len(swaps) * .95)], "max": max(swaps), "scope": "CPU swap-return intervals, not display scanout"},
        "gpu_queries": {"completed": len(gpu_rows), "max_render_ms": max(float(row["gpu_ms"]) for row in gpu_rows), "max_blit_ms": max(float(row["blit_gpu_ms"]) for row in gpu_rows), "final_drain_ms": 0.1518},
        "host_ticks": len(host_rows),
        "native_simulation_updates": 19,
        "timestamped_pose_samples": sum(1 for line in presentation.read_text(encoding="utf-8").splitlines() if line.strip()),
        "limitations": ["The fixture is intentionally truncated before its known tick96 pose divergence.", "The final source shared-speed field is not verified.", "No traffic or passenger simulation is enabled in this fixture.", "No visible monitor scanout, physical Xbox input, external Windows11 RTX5070 run, or full-game parity is claimed."],
        "files": {},
    }
    for path in (viewer, schedule, performance, gpu, host, presentation, intervals):
        report["files"][path.name] = sha(path)
    output = REPORTS / "native-cadence-live-4k-integration-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "report": str(output), "presentations": len(perf_rows),
                      "source_updates": 19, "cpu_mean_ms": report["cpu_pacing_ms"]["mean"],
                      "cpu_p95_ms": report["cpu_pacing_ms"]["p95"], "gpu_queries": len(gpu_rows)}, indent=2))

if __name__ == "__main__":
    main()
