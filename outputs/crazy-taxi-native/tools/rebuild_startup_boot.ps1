$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/startup_boot.c') (Join-Path $root 'tests/startup-boot-rom-compare.c') -o (Join-Path $root 'bin/startup-boot-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Startup init compilation failed'}
