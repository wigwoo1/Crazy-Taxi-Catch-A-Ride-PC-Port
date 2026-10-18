$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -UNDEBUG -O2 (Join-Path $root 'src/world_render_visit.c') (Join-Path $root 'src/world_polygon_stamp.c') (Join-Path $root 'tests/world-render-stamp-test.c') -o (Join-Path $root 'bin/world-render-stamp-test.exe')
if($LASTEXITCODE -ne 0){throw 'Persistent traversal build failed'}
& (Join-Path $root 'bin/world-render-stamp-test.exe')
if($LASTEXITCODE -ne 0){throw 'Persistent traversal test failed'}
