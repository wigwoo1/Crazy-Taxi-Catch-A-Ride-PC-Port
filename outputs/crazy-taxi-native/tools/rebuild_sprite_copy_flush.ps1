$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/sprite_copy_flush.c') (Join-Path $root 'tests/sprite_copy_flush_rom_compare.c') -o (Join-Path $root 'bin/sprite-copy-flush-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Sprite copy flush build failed'}
