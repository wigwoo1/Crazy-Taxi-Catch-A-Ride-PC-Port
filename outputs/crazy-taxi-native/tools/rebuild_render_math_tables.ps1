$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/impact.c') (Join-Path $root 'src/vehicle_math.c') (Join-Path $root 'tests/render_math_tables_rom_compare.c') -o (Join-Path $root 'bin/render-math-tables-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Graphics math table build failed'}
