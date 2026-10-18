$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/passenger_init.c') (Join-Path $root 'src/passenger_route.c') (Join-Path $root 'tests/passenger-init-resolved-rom-compare.c') -o (Join-Path $root 'bin/passenger-init-resolved-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Passenger init source comparison build failed'}
