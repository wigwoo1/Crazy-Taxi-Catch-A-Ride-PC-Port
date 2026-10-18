"""Verify the supported ROM before scanning it with the native ground checker."""
import hashlib
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root / 'tools'))
from extract_scene import EXPECTED

rom = Path(sys.argv[1]).resolve()
if hashlib.sha256(rom.read_bytes()).hexdigest() != EXPECTED:
    raise ValueError('Unsupported ROM revision')
subprocess.run([str(root / 'bin/ground-rom-check.exe'), str(rom), str(root / 'assets')], check=True)
