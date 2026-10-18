$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/presentation_service.c') (Join-Path $root 'src/timing.c') (Join-Path $root 'tests/presentation-service-rom-compare.c') -o (Join-Path $root 'bin/presentation-service-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Presentation service compilation failed'}
