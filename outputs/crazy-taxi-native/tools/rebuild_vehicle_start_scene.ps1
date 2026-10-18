$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('vehicle_start_gameplay','vehicle_start_scene','vehicle_start_world','vehicle_start_driver','vehicle_start_packed','vehicle_template_load','vehicle_reset_packed','vehicle_config','vehicle_start_finalize','vehicle_start_random','random','world_asset','ground','spawn_placement') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/vehicle_start_scene_rom_compare.c') -o (Join-Path $root 'bin/vehicle-start-scene-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Scene vehicle startup build failed'}
