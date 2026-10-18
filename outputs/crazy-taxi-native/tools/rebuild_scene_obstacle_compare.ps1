$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'tests/scene_obstacle_rom_compare.c') (Join-Path $root 'src/world_asset.c') (Join-Path $root 'src/ground.c') -o (Join-Path $root 'bin/scene-obstacle-rom-compare.exe')
if($LASTEXITCODE){throw 'Scene obstacle comparison build failed'}
