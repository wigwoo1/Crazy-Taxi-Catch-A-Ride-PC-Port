$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/frame_wait.c') (Join-Path $root 'tests/frame-wait-rom-compare.c') -o (Join-Path $root 'bin/frame-wait-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Frame wait compilation failed'}
