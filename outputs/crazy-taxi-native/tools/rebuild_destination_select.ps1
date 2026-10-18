$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/material_blink.c') (Join-Path $root 'tests/destination-select-rom-compare.c') -o (Join-Path $root 'bin/destination-select-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Destination selection comparison build failed'}
