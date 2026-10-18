$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/effect_pool.c') (Join-Path $root 'tests/effect_pool_reset_rom_compare.c') -o (Join-Path $root 'bin/effect-pool-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Effect pool reset build failed'}
