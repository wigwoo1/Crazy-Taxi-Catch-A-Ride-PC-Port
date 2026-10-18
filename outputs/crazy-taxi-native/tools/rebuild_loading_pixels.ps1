$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/loading_pixels.c') (Join-Path $root 'tests/loading_pixels_check.c') -o (Join-Path $root 'bin/loading-pixels-check.exe')
if($LASTEXITCODE -ne 0){throw 'Loading pixels build failed'}
& (Join-Path $root 'bin/loading-pixels-check.exe')
if($LASTEXITCODE -ne 0){throw 'Loading pixels check failed'}
