$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/projection_divide.c') (Join-Path $root 'src/engine_callback_identity.c') (Join-Path $root 'src/engine_projection_call.c') (Join-Path $root 'tests/engine_projection_call_rom_compare.c') -o (Join-Path $root 'bin/engine-projection-call-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Projection arithmetic build failed'}
