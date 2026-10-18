$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/title_background.c') (Join-Path $root 'src/palette_registration_small.c') (Join-Path $root 'tests/title-background-rom-compare.c') -o (Join-Path $root 'bin/title-background-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title background compilation failed'}
