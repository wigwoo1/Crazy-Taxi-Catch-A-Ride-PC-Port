$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$sources=@('startup_obj_queue','startup_obj','obj_order','title_obj_queue','obj_queue_owned') | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/obj_queue_owned_rom_compare.c') -o (Join-Path $root 'bin/obj-queue-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Owned object queue build failed'}
