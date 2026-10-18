$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('src/title_obj_queue.c','src/startup_obj_queue.c','src/startup_obj.c','src/obj_order.c','tests/title-obj-queue-rom-compare.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources -o (Join-Path $root 'bin/title-obj-queue-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title object queue compilation failed'}
