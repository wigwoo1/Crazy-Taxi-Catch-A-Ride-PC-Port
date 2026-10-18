$ErrorActionPreference='Stop'
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskCompiler=Join-Path $taskRoot '../../work/compiler/ziglang/zig.exe'
& $taskCompiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $taskRoot 'src') (Join-Path $taskRoot 'tests/source240_real_background_probe.c') (Join-Path $taskRoot 'src/world_background.c') (Join-Path $taskRoot 'src/indexed_asset.c') -o (Join-Path $taskRoot 'bin/source240-real-background-probe.exe')
if($LASTEXITCODE -ne 0){throw 'Real background probe compilation failed'}
$taskSources=@('indexed_pixel','indexed_asset','pair_row_raster','pair_column_setup','pair_column_raster','flat_row_raster','projection_divide','render_queue') | ForEach-Object { Join-Path $taskRoot "src/$_.c" }
& $taskCompiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $taskRoot 'src') (Join-Path $taskRoot 'tests/source240_background_scenery_probe.c') @taskSources -o (Join-Path $taskRoot 'bin/source240-background-scenery-probe.exe')
if($LASTEXITCODE -ne 0){throw 'Background scenery probe compilation failed'}
