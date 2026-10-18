$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'tests/palette_pair_rom_compare.c') -o (Join-Path $root 'bin/palette-pair-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Palette pair build failed'}
