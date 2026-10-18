$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sourceFiles=@('src/startup_init.c','src/startup_init_owned.c','src/input.c','src/save_block.c','tests/startup-init-owned-rom-compare.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sourceFiles -o (Join-Path $root 'bin/startup-init-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Native owned startup comparison build failed'}
