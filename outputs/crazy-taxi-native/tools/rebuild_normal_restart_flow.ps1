$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('normal_restart','normal_restart_graphics','normal_restart_entry','normal_restart_flow') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/normal_restart_flow_rom_compare.c') -o (Join-Path $root 'bin/normal-restart-flow-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart flow build failed'}
