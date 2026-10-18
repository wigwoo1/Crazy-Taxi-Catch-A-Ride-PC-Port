$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('startup_screens','startup_bitmap','bitmap_fill','display_mode','palette_registration','palette_bank','shade_table','screen_sequence','screen_flow','presentation_service','timing','palette_fade_setup','palette_fade_registered','palette_fade')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/startup-screens-check.c') -o (Join-Path $root 'bin/startup-screens-check.exe')
if($LASTEXITCODE -ne 0){throw 'Startup screens compilation failed'}
