$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/render_queue.c') (Join-Path $root 'tests/render-queue-consume-rom-compare.c') -o (Join-Path $root 'bin/render-queue-consume-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Render queue source comparison build failed'}
