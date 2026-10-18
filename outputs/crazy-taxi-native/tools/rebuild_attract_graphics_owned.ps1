$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','attract_graphics','attract_graphics_owned','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/attract-assets-check.c') -o (Join-Path $root 'bin/attract-assets-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract graphics build failed'}
& (Join-Path $root 'bin/attract-assets-check.exe')
if($LASTEXITCODE -ne 0){throw 'Attract graphics check failed'}
