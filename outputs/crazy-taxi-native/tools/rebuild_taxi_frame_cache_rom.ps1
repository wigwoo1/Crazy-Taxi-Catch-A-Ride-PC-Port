$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('taxi_frame_copy_owned','taxi_frame_cache_owned','sprite_replacement_owned','sprite_replacement')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/taxi-frame-cache-rom-compare.c') -o (Join-Path $root 'bin/taxi-frame-cache-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi frame cache comparison build failed'}
