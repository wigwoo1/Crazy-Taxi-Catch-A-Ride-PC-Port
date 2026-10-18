$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/taxi_half_plan.c') (Join-Path $root 'src/obj_order.c') (Join-Path $root 'tests/taxi-half-plan-rom-compare.c') -o (Join-Path $root 'bin/taxi-half-plan-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi half plan build failed'}
