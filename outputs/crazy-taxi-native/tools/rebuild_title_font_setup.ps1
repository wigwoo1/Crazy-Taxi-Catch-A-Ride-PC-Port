$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/font_setup.c') (Join-Path $root 'src/title_font_setup.c') (Join-Path $root 'tests/title-font-setup-rom-compare.c') -o (Join-Path $root 'bin/title-font-setup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title font setup compilation failed'}
