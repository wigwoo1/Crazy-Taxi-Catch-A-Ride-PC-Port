$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_render_context.c') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/restart_render_context_test.c') -o (Join-Path $root 'bin/restart-render-context-test.exe')
if($LASTEXITCODE -ne 0){throw 'Restart renderer adapter build failed'}
& (Join-Path $root 'bin/restart-render-context-test.exe')
if($LASTEXITCODE -ne 0){throw 'Restart renderer adapter test failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_render_context.c') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/restart_clear_frames_rom_compare.c') -o (Join-Path $root 'bin/restart-clear-frames-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart clear frames ROM probe build failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_render_context.c') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/restart_shade_gate_rom_compare.c') -o (Join-Path $root 'bin/restart-shade-gate-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart shade gate ROM probe build failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_render_context.c') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/restart_shade_full_rom_compare.c') -o (Join-Path $root 'bin/restart-shade-full-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart shade full ROM probe build failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_render_context.c') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'src/shade_table.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/restart_material_rom_compare.c') -o (Join-Path $root 'bin/restart-material-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart material shade probe build failed'}
