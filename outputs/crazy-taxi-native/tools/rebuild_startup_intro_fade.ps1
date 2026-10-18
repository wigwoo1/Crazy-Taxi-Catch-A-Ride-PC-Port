$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('startup_intro_fade','palette_fade','palette_fade_setup','palette_fade_registered','startup_intro','startup_animation','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/startup-intro-fade-check.c') -o (Join-Path $root 'bin/startup-intro-fade-check.exe')
if($LASTEXITCODE -ne 0){throw 'Intro fade compilation failed'}
