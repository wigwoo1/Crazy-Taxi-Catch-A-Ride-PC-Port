$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj','startup_obj_queue','ordinary_graphics_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/ordinary_assets_check.c') -o (Join-Path $root 'bin/ordinary-assets-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary graphics build failed'}
& (Join-Path $root 'bin/ordinary-assets-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary graphics asset check failed'}
