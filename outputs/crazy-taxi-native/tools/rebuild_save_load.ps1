$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/save_load.c') (Join-Path $root 'src/save_block.c') (Join-Path $root 'tests/save-load-rom-compare.c') -o (Join-Path $root 'bin/save-load-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Native save-load lifecycle comparison build failed'}
