$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/startup_obj_queue.c') (Join-Path $root 'src/startup_obj.c') (Join-Path $root 'src/obj_order.c') (Join-Path $root 'tests/startup-obj-queue-rom-compare.c') -o (Join-Path $root 'bin/startup-obj-queue-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Startup OBJ queue compilation failed'}
