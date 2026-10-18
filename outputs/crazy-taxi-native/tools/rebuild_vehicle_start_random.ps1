$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/vehicle_start_random.c') (Join-Path $root 'src/random.c') (Join-Path $root 'tests/vehicle_start_random_test.c') -o (Join-Path $root 'bin/vehicle-start-random-test.exe')
if($LASTEXITCODE -ne 0){throw 'Vehicle random startup build failed'}
