$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/attract_prepare.c') (Join-Path $root 'src/title_parent.c') (Join-Path $root 'src/title_attract_owned.c') (Join-Path $root 'tests/title-attract-owned-check.c') -o (Join-Path $root 'bin/title-attract-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract owner build failed'}
& (Join-Path $root 'bin/title-attract-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract owner check failed'}
