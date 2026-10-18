$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$sources=@('startup_obj_queue','startup_obj','obj_order','title_obj_queue') | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/loading_obj_queue_rom_compare.c') -o (Join-Path $root 'bin/loading-obj-queue-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Loading object queue build failed'}
