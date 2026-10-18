$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/screen_sequence.c') (Join-Path $root 'tests/screen-sequence-rom-compare.c') -o (Join-Path $root 'bin/screen-sequence-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Screen sequence compilation failed'}
