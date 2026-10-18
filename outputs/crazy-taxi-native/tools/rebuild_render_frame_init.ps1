$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/render_frame_init.c') (Join-Path $root 'src/projection_divide.c') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/render_frame_init_test.c') -o (Join-Path $root 'bin/render-frame-init-test.exe')
if($LASTEXITCODE -ne 0){throw 'Frame initialization build failed'}
& (Join-Path $root 'bin/render-frame-init-test.exe')
if($LASTEXITCODE -ne 0){throw 'Frame initialization test failed'}
