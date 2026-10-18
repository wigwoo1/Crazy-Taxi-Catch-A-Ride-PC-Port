$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/orientation.c') (Join-Path $root 'src/impact.c') (Join-Path $root 'tests/ordinary_orientation_rom_compare.c') -o (Join-Path $root 'bin/ordinary-orientation-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary orientation build failed'}
