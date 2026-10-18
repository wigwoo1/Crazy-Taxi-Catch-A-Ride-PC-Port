$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/graphics_generation.c') (Join-Path $root 'src/world_polygon_stamp.c') (Join-Path $root 'tests/graphics-preview-stamp-sequence.c') -o (Join-Path $root 'bin/graphics-preview-stamp-sequence.exe')
if($LASTEXITCODE -ne 0){throw 'Preview stamp sequence build failed'}
