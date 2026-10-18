$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/graphics_generation.c') (Join-Path $root 'tests/graphics-generation-rom-compare.c') -o (Join-Path $root 'bin/graphics-generation-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Graphics generation build failed'}
