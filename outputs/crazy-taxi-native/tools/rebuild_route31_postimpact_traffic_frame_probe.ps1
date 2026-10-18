param([string]$Compiler='')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $projectRoot '../../work/compiler/ziglang/zig.exe'}
$sourceNames=@('material_blink','destination_panel','distance_bitmap','world_background','world_indexed','indexed_asset','indexed_pixel','raster_dispatch','view_transform','render_interpolation','taxi_shadow_render','passenger_render','world_frame','world_material','world_clip','world_render','world_render_visit','world_polygon_stamp','render_queue','render_viewport','traffic_assets','traffic_init','traffic_drive','traffic_scene','traffic_frame','traffic_spawn','traffic_grid','traffic_actor','traffic_actor_impact','traffic_motion','traffic_follow','traffic_collision','traffic_effects','traffic_path','traffic_presentation','traffic_view','traffic_sprite','traffic_heading','ai_path','gears','random','world_asset','ground','vehicle_math','movement','traction','impact','projection_divide','pair_row_raster','pair_column_setup','pair_column_raster','flat_row_raster','indexed_preview_scale','destination_preview_border','runtime_asset','vehicle_config','spawn_placement','body_pose')
$taskArgs=@('cc','-std=c11','-Wall','-Wextra','-Werror','-O2','-UNDEBUG','-I',(Join-Path $projectRoot 'src'))
foreach($sourceName in $sourceNames){$taskArgs+=(Join-Path $projectRoot "src/$sourceName.c")}
$taskArgs+=(Join-Path $projectRoot 'tests/route31_postimpact_traffic_frame_probe.c')
$binary=Join-Path $projectRoot 'bin/route31-postimpact-traffic-frame-probe.exe'
$taskArgs+=@('-o',$binary)
& $Compiler $taskArgs
if($LASTEXITCODE -ne 0){throw 'Route31 post-impact frame probe compilation failed'}
$report=Join-Path $projectRoot 'reports/original-core-reference'
$manifest=Get-Content -LiteralPath (Join-Path $report 'route31-postimpact-frame-fixtures.json') -Raw | ConvertFrom-Json
$cases=@($manifest.cases);if($cases.Count -ne 2){throw 'Expected exactly two phase-aligned source cases'}
$runRoot=Split-Path -Parent (Split-Path -Parent $projectRoot)
Push-Location $runRoot
try {
    $log=Join-Path $report $manifest.sequence_log
    & $binary (Join-Path $report $cases[0].pool_fixture) (Join-Path $report $cases[0].config) (Join-Path $report $cases[1].config) | Set-Content -LiteralPath $log -Encoding utf8
    if($LASTEXITCODE -ne 0){throw 'Consecutive route31 frame probe execution failed'}
} finally {Pop-Location}
