$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/display_mode.c') (Join-Path $root 'tests/display_mode_rom_compare.c') -o (Join-Path $root 'bin/display-mode-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Display mode build failed'}
