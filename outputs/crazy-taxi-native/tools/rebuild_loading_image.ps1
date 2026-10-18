$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/loading_image.c') (Join-Path $root 'src/palette_fade.c') (Join-Path $root 'tests/loading_image_rom_compare.c') -o (Join-Path $root 'bin/loading-image-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Loading image build failed'}
