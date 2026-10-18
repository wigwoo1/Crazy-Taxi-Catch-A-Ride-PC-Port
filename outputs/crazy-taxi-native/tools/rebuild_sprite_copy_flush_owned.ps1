$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('sprite_copy_flush','sprite_copy_flush_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/sprite_copy_flush_owned_rom_compare.c') -o (Join-Path $root 'bin/sprite-copy-flush-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Owned sprite copy flush build failed'}
