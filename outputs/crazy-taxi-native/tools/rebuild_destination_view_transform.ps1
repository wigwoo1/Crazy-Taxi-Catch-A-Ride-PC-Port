$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/view_transform.c') (Join-Path $root 'src/impact.c') (Join-Path $root 'tests/destination-view-transform-rom-compare.c') -o (Join-Path $root 'bin/destination-view-transform-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Destination transform source comparison build failed'}
