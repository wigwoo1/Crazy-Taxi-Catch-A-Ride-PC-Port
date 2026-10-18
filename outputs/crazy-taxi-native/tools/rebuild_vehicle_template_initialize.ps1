$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/vehicle_template_load.c') (Join-Path $root 'src/vehicle_config.c') (Join-Path $root 'src/vehicle_reset_packed.c') (Join-Path $root 'tests/vehicle_template_initialize_rom_compare.c') -o (Join-Path $root 'bin/vehicle-template-initialize-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Vehicle template load build failed'}
