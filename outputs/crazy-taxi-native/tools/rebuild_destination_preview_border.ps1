$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/destination_preview_border.c') (Join-Path $root 'tests/destination-preview-border-rom-compare.c') -o (Join-Path $root 'bin/destination-preview-border-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Destination preview border build failed'}
