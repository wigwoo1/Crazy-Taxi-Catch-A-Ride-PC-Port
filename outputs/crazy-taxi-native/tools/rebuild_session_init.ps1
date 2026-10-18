$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/session_init.c') (Join-Path $root 'tests/session_init_rom_compare.c') -o (Join-Path $root 'bin/session-init-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Session initialization build failed'}
