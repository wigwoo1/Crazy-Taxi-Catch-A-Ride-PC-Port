$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/sprite_replacement.c') (Join-Path $root 'src/sprite_replacement_owned.c') (Join-Path $root 'tests/sprite_replacement_owned_rom_compare.c') -o (Join-Path $root 'bin/sprite-replacement-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Owned sprite replacement build failed'}
