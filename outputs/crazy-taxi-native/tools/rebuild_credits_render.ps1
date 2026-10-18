$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/credits_flow.c') (Join-Path $root 'src/credits_asset.c') (Join-Path $root 'tests/credits_render_check.c') -o (Join-Path $root 'bin/credits-render-check.exe')
if($LASTEXITCODE -ne 0){throw 'Credits render build failed'}
