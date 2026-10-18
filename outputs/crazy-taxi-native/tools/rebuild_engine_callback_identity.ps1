$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/engine_callback_identity.c') (Join-Path $root 'src/raster_dispatch.c') (Join-Path $root 'tests/engine_callback_identity_check.c') -o (Join-Path $root 'bin/engine-callback-identity-check.exe')
if($LASTEXITCODE -ne 0){throw 'Engine callback identity build failed'}
