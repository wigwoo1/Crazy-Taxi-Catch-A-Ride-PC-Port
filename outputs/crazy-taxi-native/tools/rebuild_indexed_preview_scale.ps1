$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/indexed_preview_scale.c') (Join-Path $root 'tests/indexed-preview-scale-rom-compare.c') -o (Join-Path $root 'bin/indexed-preview-scale-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Indexed preview scaler build failed'}
