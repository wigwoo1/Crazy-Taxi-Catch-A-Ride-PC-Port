$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/vehicle_config.c') (Join-Path $root 'tests/vehicle_speed_divisor_rom_compare.c') -o (Join-Path $root 'bin/vehicle-speed-divisor-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Speed divisor build failed'}
