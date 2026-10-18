$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/graphics_memory_base.c') (Join-Path $root 'tests/graphics_memory_base_rom_compare.c') -o (Join-Path $root 'bin/graphics-memory-base-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Graphics memory base build failed'}
