$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/attract_entry.c') (Join-Path $root 'src/random.c') (Join-Path $root 'src/attract_prepare.c') (Join-Path $root 'tests/attract-entry-rom-compare.c') -o (Join-Path $root 'bin/attract-entry-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Attract entry compilation failed'}
