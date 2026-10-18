$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('host_ordinary_presentation','host_presentation','ordinary_presentation_owned','presentation_service','timing','city_palette_owned','palette_fade_registered','palette_fade_setup','palette_fade','palette_registration','palette_registration_small','palette_bank','shade_table','startup_obj_transport','startup_obj_queue','startup_obj','obj_order','sprite_copy_flush','sprite_copy_flush_owned')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/host_ordinary_presentation_check.c') -o (Join-Path $root 'bin/host-ordinary-presentation-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary presentation build failed'}
& (Join-Path $root 'bin/host-ordinary-presentation-check.exe')
if($LASTEXITCODE -ne 0){throw 'Ordinary presentation check failed'}
