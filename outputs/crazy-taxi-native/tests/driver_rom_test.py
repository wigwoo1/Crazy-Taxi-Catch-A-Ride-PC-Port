"""Hash-checked native powertrain and driver probes. No GBA execution."""
import hashlib
from pathlib import Path
import subprocess
import sys

EXPECTED = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def main():
    if len(sys.argv) != 2:
        raise SystemExit("Usage: python tests/driver_rom_test.py PATH_TO_ROM")
    rom = Path(sys.argv[1]).resolve()
    if hashlib.sha256(rom.read_bytes()).hexdigest() != EXPECTED:
        raise SystemExit("ROM revision/hash does not match the verified USA image.")
    root = Path(__file__).resolve().parents[1]
    for name in ("vehicle-config-test", "traction-test", "body-pose-test", "powertrain-rom-check", "driver-rom-check"):
        subprocess.run([str(root / "bin" / (name + ".exe")), str(rom)], check=True)

if __name__ == "__main__":
    main()
