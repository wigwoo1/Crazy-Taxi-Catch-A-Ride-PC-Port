$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/passenger_idle.c') (Join-Path $root 'src/random.c') (Join-Path $root 'tests/passenger_paused_rom_compare.c') -o (Join-Path $root 'bin/passenger-paused-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Paused passenger build failed'}
