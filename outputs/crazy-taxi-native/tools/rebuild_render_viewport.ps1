$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/render_viewport.c') (Join-Path $root 'tests/render_viewport_rom_compare.c') -o (Join-Path $root 'bin/render-viewport-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Render viewport build failed'}
