$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/screen_flow.c') (Join-Path $root 'tests/screen-flow-rom-compare.c') -o (Join-Path $root 'bin/screen-flow-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Screen flow compilation failed'}
