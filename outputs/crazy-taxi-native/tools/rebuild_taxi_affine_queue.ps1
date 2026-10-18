$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('taxi_affine_queue','obj_order','title_obj_queue','startup_obj_queue','startup_obj')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/taxi_affine_queue_check.c') -o (Join-Path $root 'bin/taxi-affine-queue-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi affine build failed'}
& (Join-Path $root 'bin/taxi-affine-queue-check.exe')
if($LASTEXITCODE -ne 0){throw 'Taxi affine check failed'}
