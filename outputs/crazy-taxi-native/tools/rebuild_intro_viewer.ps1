param([string]$Output='intro-viewer.exe')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('startup_intro_viewer','startup_intro_fade','palette_fade_setup','palette_fade_registered','startup_intro','startup_intro_pixels','palette_fade','startup_animation','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue','xbox','startup_screens','startup_bitmap','bitmap_fill','display_mode','palette_registration','palette_bank','shade_table','screen_sequence','screen_flow','presentation_service','timing')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -DTAXI_INTRO_STANDALONE @sources -lgdi32 -luser32 -o (Join-Path $root "bin/$Output")
if($LASTEXITCODE -ne 0){throw 'Intro viewer compilation failed'}
