$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/startup_obj.c') (Join-Path $root 'src/obj_order.c') (Join-Path $root 'tests/startup-obj-rom-compare.c') -o (Join-Path $root 'bin/startup-obj-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Startup OBJ compilation failed'}
