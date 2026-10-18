$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('src/title_assets.c','src/palette_registration_small.c','src/title_prepare.c','src/title_font_setup.c','src/font_setup.c','src/title_prompt_setup.c','src/sprite_replacement.c','tests/title-prepare-compare.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources -o (Join-Path $root 'bin/title-prepare-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title prepare compilation failed'}
