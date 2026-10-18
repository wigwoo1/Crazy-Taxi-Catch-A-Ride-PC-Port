$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('startup_intro','startup_intro_pixels','palette_fade','startup_animation','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/startup-intro-check.c') -o (Join-Path $root 'bin/startup-intro-check.exe')
if($LASTEXITCODE -ne 0){throw 'Native intro compilation failed'}
