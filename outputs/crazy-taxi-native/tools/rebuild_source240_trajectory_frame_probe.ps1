$ErrorActionPreference='Stop'
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskCompiler=Join-Path $taskRoot '../../work/compiler/ziglang/zig.exe'
$taskSources=@('world_asset','indexed_asset','world_frame','world_render','world_clip','world_material','world_render_visit','world_polygon_stamp','render_queue','raster_dispatch','vehicle_math','projection_divide','ground','impact','traffic_view','traction','traffic_sprite') | ForEach-Object { Join-Path $taskRoot "src/$_.c" }
& $taskCompiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 (Join-Path $taskRoot 'tests/source240_trajectory_frame_probe.c') @taskSources -o (Join-Path $taskRoot 'bin/source240-trajectory-frame-probe.exe')
if($LASTEXITCODE -ne 0){throw 'Native scene trajectory probe compilation failed'}
