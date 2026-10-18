$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -ffunction-sections -I (Join-Path $root 'src') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'tests/world_clear_rect_test.c') -o (Join-Path $root 'bin/world-clear-test.exe')
if($LASTEXITCODE -ne 0){throw 'World clear build failed'}
& (Join-Path $root 'bin/world-clear-test.exe')
if($LASTEXITCODE -ne 0){throw 'World clear test failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'tests/world_clear_rom_compare.c') -o (Join-Path $root 'bin/world-clear-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'World clear ROM probe build failed'}
