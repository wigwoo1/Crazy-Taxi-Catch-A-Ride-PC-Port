"""Export source-observed dispatcher entry counters for offline replay alignment."""
import hashlib
import json
import pathlib
import sys


def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("usage: extract_native_traffic_dispatch_schedule.py TRACE OUTPUT")
    source = pathlib.Path(sys.argv[1])
    output = pathlib.Path(sys.argv[2])
    trace = json.loads(source.read_text(encoding="utf-8"))
    instrumentation = trace.get("instrumentation", {})
    if instrumentation.get("source_rom_sha256") != "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5":
        raise SystemExit("dispatcher trace does not match the supplied ROM hash")
    if trace.get("log_saturated"):
        raise SystemExit("dispatcher trace is saturated and cannot define a complete schedule")
    events = trace.get("call_events", [])
    counters = [int(event["source_counter"]) for event in events]
    if not counters or any(b <= a for a, b in zip(counters, counters[1:])):
        raise SystemExit("dispatcher source counters are empty or not strictly increasing")
    output.write_text(f"SOURCE_DISPATCH_COUNTERS_V1 {len(counters)}\n" + "\n".join(map(str, counters)) + "\n", encoding="ascii")
    report = {
        "status": "PASS_EXTRACTED_SOURCE_DISPATCH_COUNTERS",
        "source_trace": str(source),
        "source_trace_sha256": sha(source),
        "source_rom_sha256": instrumentation["source_rom_sha256"],
        "entries": len(counters),
        "first_counter": counters[0],
        "last_counter": counters[-1],
        "log_saturated": False,
        "output": str(output),
        "output_sha256": sha(output),
        "scope": "Observed dispatcher entry counters from temporary in-memory ROM instrumentation. This schedule aligns offline diagnostic frames; it does not alone prove full actor/global parity or playable port behavior.",
    }
    output.with_suffix(output.suffix + ".json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report))


if __name__ == "__main__":
    main()
