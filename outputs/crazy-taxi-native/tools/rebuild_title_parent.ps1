$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/title_parent.c') (Join-Path $root 'tests/title-parent-rom-compare.c') -o (Join-Path $root 'bin/title-parent-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title parent compilation failed'}
