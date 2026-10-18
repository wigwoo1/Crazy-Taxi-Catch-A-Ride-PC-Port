$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('normal_restart','normal_restart_graphics','normal_restart_entry','normal_restart_flow','restart_render_context','world_background','indexed_asset','shade_table','render_viewport','scene_registration','scene_registration_render','render_frame_init','projection_divide','engine_callback_identity','engine_projection_call','render_frame_owned','graphics_storage','graphics_bootstrap_tail','impact','vehicle_math','palette_registration','palette_bank','palette_fade_setup','palette_fade','palette_fade_registered','graphics_memory_base','display_mode','game_audio_reset_state','effect_pool','restart_block_reset','session_init','session_init_fare','traffic_init') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/restart_flow_render_test.c') -o (Join-Path $root 'bin/restart-flow-render-test.exe')
if($LASTEXITCODE -ne 0){throw 'Restart flow render adapter build failed'}
$cityIndex=0
foreach($city in @('city1','city2')){
 & (Join-Path $root 'bin/restart-flow-render-test.exe') (Join-Path $root "assets/$city-palette.rgb555") (Join-Path $root "assets/scene-descriptors/$city.bin") (Join-Path $root "assets/indexed-scenes/$city.cti") $cityIndex (Join-Path $root "assets/driving.ctr")
 $cityIndex++
 if($LASTEXITCODE -ne 0){throw "Restart flow render test failed $city"}
}
