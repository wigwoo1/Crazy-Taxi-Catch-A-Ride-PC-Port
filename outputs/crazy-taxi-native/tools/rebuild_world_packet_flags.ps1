$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/raster_dispatch.c') (Join-Path $root 'tests/world-packet-flags-rom-compare.c') -o (Join-Path $root 'bin/world-packet-flags-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Packet flags comparison build failed'}
