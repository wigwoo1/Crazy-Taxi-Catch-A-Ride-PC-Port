param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/normal_restart.c') (Join-Path $root 'tests/normal_restart_rom_compare.c') -o (Join-Path $root 'bin/normal-restart-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart prefix comparison build failed'}

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/normal_restart.c') (Join-Path $root 'tests/normal_restart_setup_rom_compare.c') -o (Join-Path $root 'bin/normal-restart-setup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Restart game setup comparison build failed'}
