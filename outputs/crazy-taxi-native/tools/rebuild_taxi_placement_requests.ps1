$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/taxi_placement_requests.c') (Join-Path $root 'tests/taxi-placement-requests-rom-compare.c') -o (Join-Path $root 'bin/taxi-placement-requests-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi placement request build failed'}
