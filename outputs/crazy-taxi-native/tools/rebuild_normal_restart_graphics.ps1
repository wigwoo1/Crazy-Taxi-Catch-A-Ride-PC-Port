$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/normal_restart_graphics.c') (Join-Path $root 'tests/normal_restart_graphics_rom_compare.c') -o (Join-Path $root 'bin/normal-restart-graphics-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart graphics build failed'}
