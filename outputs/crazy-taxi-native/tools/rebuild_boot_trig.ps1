$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/boot_memory.c') (Join-Path $root 'tests/boot-trig-compare.c') -o (Join-Path $root 'bin/boot-trig-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Boot trig compilation failed'}
