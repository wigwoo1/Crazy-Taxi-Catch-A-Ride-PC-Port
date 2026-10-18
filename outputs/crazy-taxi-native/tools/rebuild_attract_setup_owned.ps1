$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','attract_graphics','attract_graphics_owned','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue','attract_setup_owned','normal_restart','normal_restart_owned','restart_resources','display_mode','graphics_memory_base','music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix','random','music_playback','city_palette_owned','palette_fade_registered','palette_fade_setup','palette_fade')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/attract-setup-owned-check.c') -o (Join-Path $root 'bin/attract-setup-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract setup build failed'}
& (Join-Path $root 'bin/attract-setup-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract setup check failed'}
