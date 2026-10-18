$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('scene_registration','scene_registration_render','restart_render_context','world_background','indexed_asset','shade_table','render_viewport') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/scene_registration_render_test.c') -o (Join-Path $root 'bin/scene-registration-render-test.exe')
if($LASTEXITCODE -ne 0){throw 'Scene registration composition build failed'}
foreach($city in @('city1','city2')){
 & (Join-Path $root 'bin/scene-registration-render-test.exe') (Join-Path $root "assets/scene-descriptors/$city.bin") (Join-Path $root "assets/indexed-scenes/$city.cti") (Join-Path $root "assets/$city-palette.rgb555")
 if($LASTEXITCODE -ne 0){throw "Scene registration composition failed $city"}
}
