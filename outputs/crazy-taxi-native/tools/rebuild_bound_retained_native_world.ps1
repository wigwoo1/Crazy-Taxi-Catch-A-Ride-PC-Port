$ErrorActionPreference='Stop'
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskCompiler=Join-Path $taskRoot '../../work/compiler/ziglang/zig.exe'
$taskSources=@('world_asset','indexed_asset','world_frame','world_render','world_clip','world_material','world_render_visit','world_polygon_stamp','render_queue','raster_dispatch','vehicle_math','projection_divide','ground','impact','traffic_view','traction','traffic_sprite','world_source_frame','source_frame_native','engine_callback_identity','world_background','indexed_pixel','pair_row_raster','pair_column_raster','pair_column_setup','pair_column_draw','flat_row_raster') | ForEach-Object { Join-Path $taskRoot "src/$_.c" }
& $taskCompiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -Wno-misleading-indentation -O2 (Join-Path $taskRoot 'tests/bound_retained_native_world_frame.c') @taskSources -o (Join-Path $taskRoot 'bin/bound-retained-native-world-frame.exe')
if($LASTEXITCODE -ne 0){throw 'Native generated world compilation failed'}
