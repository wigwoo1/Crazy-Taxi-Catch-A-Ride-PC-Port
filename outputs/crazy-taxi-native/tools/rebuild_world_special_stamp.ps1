$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/world_polygon_stamp.c') (Join-Path $root 'tests/world-special-stamp-rom-compare.c') -o (Join-Path $root 'bin/world-special-stamp-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Special stamp comparison build failed'}
