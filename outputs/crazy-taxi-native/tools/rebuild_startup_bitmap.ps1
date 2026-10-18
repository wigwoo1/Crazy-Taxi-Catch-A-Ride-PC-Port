$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('startup_bitmap','bitmap_fill','display_mode','palette_registration','palette_bank','shade_table')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/startup-bitmap-check.c') -o (Join-Path $root 'bin/startup-bitmap-check.exe')
if($LASTEXITCODE -ne 0){throw 'Startup bitmap compilation failed'}
