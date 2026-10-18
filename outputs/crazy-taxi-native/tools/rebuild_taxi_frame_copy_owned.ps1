$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('taxi_frame_copy_owned','sprite_replacement_owned','sprite_replacement')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/taxi_frame_copy_owned_check.c') -o (Join-Path $root 'bin/taxi-frame-copy-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi frame copy build failed'}
& (Join-Path $root 'bin/taxi-frame-copy-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi frame copy check failed'}
