$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/restart_block_reset.c') (Join-Path $root 'tests/restart_block_reset_rom_compare.c') -o (Join-Path $root 'bin/restart-block-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart block reset build failed'}
