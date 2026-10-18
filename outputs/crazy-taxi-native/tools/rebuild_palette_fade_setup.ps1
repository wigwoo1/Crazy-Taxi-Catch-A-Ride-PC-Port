$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/palette_fade_setup.c') (Join-Path $root 'src/palette_fade.c') (Join-Path $root 'tests/palette_fade_setup_rom_compare.c') -o (Join-Path $root 'bin/palette-fade-setup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Palette fade mask setup build failed'}

& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/palette_fade_setup.c') (Join-Path $root 'src/palette_fade.c') (Join-Path $root 'tests/palette_fade_start_rom_compare.c') -o (Join-Path $root 'bin/palette-fade-start-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Normal fade start probe build failed'}
