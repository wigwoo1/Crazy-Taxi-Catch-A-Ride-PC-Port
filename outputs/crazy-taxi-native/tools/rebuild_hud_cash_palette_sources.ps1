$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('hud_cash_pixels','fare','hud_digit_copies','hud_digit_copies_owned','effect_sprite','impact','obj_tiles_rgba','effect_asset','effect_render_owned','taxi_tiles_rgba','taxi_graphics_package','taxi_render_owned','taxi_placement_requests','taxi_affine_queue','title_obj_queue','taxi_graphics_owned','taxi_frame_cache_owned','taxi_frame_copy_owned','taxi_frame_palette_owned','sprite_replacement_owned','sprite_replacement','sprite_copy_flush_owned','sprite_copy_flush','ordinary_graphics_owned','title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj_queue','startup_obj')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/hud_cash_palette_sources_probe.c') -o (Join-Path $root 'bin/hud-cash-palette-sources-probe.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi graphics composition build failed'}
