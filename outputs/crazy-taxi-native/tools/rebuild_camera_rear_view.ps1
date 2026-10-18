$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('camera_angle','orientation','impact','engine_rotate_owned','engine_matrix_call','engine_callback_identity')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/camera_rear_view_rom_compare.c') -o (Join-Path $root 'bin/camera-rear-view-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Camera engine binding build failed'}
