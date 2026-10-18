$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/sprite_replacement.c') (Join-Path $root 'tests/sprite-replacement-rom-compare.c') -o (Join-Path $root 'bin/sprite-replacement-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Sprite replacement compilation failed'}
