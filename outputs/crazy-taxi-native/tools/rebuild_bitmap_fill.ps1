$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/bitmap_fill.c') (Join-Path $root 'tests/bitmap-fill-rom-compare.c') -o (Join-Path $root 'bin/bitmap-fill-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Bitmap fill compilation failed'}
