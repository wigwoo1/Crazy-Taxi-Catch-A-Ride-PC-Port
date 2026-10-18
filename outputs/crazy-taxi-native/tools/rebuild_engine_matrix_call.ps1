$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/engine_matrix_call.c') (Join-Path $root 'src/engine_callback_identity.c') (Join-Path $root 'tests/engine_matrix_call_rom_compare.c') -o (Join-Path $root 'bin/engine-matrix-call-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Engine matrix call build failed'}
