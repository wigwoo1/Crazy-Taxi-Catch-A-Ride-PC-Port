$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('vehicle_start_finalize','vehicle_start_random','random') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/vehicle_start_finalize_rom_compare.c') -o (Join-Path $root 'bin/vehicle-start-finalize-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Vehicle startup suffix build failed'}
