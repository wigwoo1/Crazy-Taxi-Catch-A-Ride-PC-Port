$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('loading_present','loading_strings_owned','string_lookup','loading_indexed_owned','loading_image','palette_fade','world_background','indexed_asset','sprite_replacement','sprite_replacement_owned','loading_sprites_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/loading_sprites_owned_rom_compare.c') -o (Join-Path $root 'bin/loading-sprites-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Loading sprite composition build failed'}
