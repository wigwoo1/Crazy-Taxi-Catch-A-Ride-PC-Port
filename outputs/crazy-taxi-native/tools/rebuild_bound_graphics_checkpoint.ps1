$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('bound_graphics_owned','ordinary_graphics_owned','title_assets','palette_registration_small','palette_registration','palette_bank','shade_table','font_setup','startup_obj_setup','startup_obj_transport','obj_order','startup_obj_queue','startup_obj','taxi_graphics_package','palette_fade_registered','palette_fade_setup','palette_fade','city_palette_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 (Join-Path $root 'tests/bound-graphics-prefix-retained-compare.c') @sources -o (Join-Path $root 'bin/bound-graphics-prefix-retained-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Bound graphics checkpoint build failed'}