$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/world_clip.c') (Join-Path $root 'tests/world-project-rom-compare.c') -o (Join-Path $root 'bin/world-project-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Texture source comparison build failed'}
