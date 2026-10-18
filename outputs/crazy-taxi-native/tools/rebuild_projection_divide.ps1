$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/projection_divide.c') (Join-Path $root 'tests/projection_divide_rom_compare.c') -o (Join-Path $root 'bin/projection-divide-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Projection arithmetic build failed'}
