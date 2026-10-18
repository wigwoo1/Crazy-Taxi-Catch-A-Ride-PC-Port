$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('loading_present','loading_strings_owned','string_lookup','loading_indexed_owned','loading_image','palette_fade','world_background','indexed_asset','sprite_replacement','sprite_replacement_owned','loading_sprites_owned','loading_objects_owned','obj_queue_owned','startup_obj_queue','startup_obj','obj_order','title_obj_queue','loading_text_owned','text_layout','font_setup','loading_content_owned','title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','startup_obj_setup','startup_obj_transport','ordinary_graphics_owned','ordinary_loading_owned','ordinary_loading_frame','ordinary_presentation_owned','presentation_service','timing','city_palette_owned','palette_fade_registered','palette_fade_setup','sprite_copy_flush','sprite_copy_flush_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/ordinary_loading_frame_check.c') -o (Join-Path $root 'bin/ordinary-loading-frame-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary loading composition build failed'}

& (Join-Path $root 'bin/ordinary-loading-frame-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary loading frame check failed'}
