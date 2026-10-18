$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/startup_animation.c') (Join-Path $root 'tests/startup-animation-rom-compare.c') -o (Join-Path $root 'bin/startup-animation-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Startup animation compilation failed'}
