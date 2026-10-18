$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 (Join-Path $root 'src/presentation_service.c') (Join-Path $root 'src/timing.c') (Join-Path $root 'tests/presentation_reset_rom_compare.c') -o (Join-Path $root 'bin/presentation-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Presentation reset build failed'}
