$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('taxi_graphics_owned','taxi_frame_cache_owned','taxi_frame_copy_owned','taxi_frame_palette_owned','sprite_replacement_owned','sprite_replacement','sprite_copy_flush_owned','sprite_copy_flush','ordinary_graphics_owned','title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj_queue','startup_obj')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/taxi_graphics_owned_check.c') -o (Join-Path $root 'bin/taxi-graphics-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi graphics composition build failed'}
& (Join-Path $root 'bin/taxi-graphics-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi graphics composition check failed'}
