$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/boot_services.c') (Join-Path $root 'tests/boot-services-compare.c') -o (Join-Path $root 'bin/boot-services-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Boot services compilation failed'}
