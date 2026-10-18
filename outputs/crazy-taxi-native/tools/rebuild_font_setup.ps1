$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/font_setup.c') (Join-Path $root 'tests/font-setup-rom-compare.c') -o (Join-Path $root 'bin/font-setup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Font setup compilation failed'}
