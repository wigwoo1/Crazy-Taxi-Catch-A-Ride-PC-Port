$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/palette_bank.c') (Join-Path $root 'src/palette_registration.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'tests/palette_bank_rom_compare.c') -o (Join-Path $root 'bin/palette-bank-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Native palette bank build failed'}
