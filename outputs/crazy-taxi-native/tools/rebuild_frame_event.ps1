$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/frame_event.c') (Join-Path $root 'tests/frame-event-rom-compare.c') -o (Join-Path $root 'bin/frame-event-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Frame event compilation failed'}
