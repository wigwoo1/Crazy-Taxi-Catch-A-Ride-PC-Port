$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/world_background.c') (Join-Path $root 'src/indexed_asset.c') (Join-Path $root 'tests/world-background-rom-compare.c') -o (Join-Path $root 'bin/world-background-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'World background source comparison build failed'}
