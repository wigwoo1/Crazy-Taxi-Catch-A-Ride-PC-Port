$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('src/title_background.c','src/title_assets.c','src/palette_registration_small.c','src/title_prepare.c','src/title_font_setup.c','src/font_setup.c','src/title_prompt_setup.c','src/sprite_replacement.c','src/title_load.c','src/startup_obj_setup.c','src/startup_obj_transport.c','src/startup_obj_queue.c','src/startup_obj.c','src/obj_order.c','src/title_entry.c','src/title_flow.c','src/palette_fade.c','src/palette_fade_setup.c','src/palette_fade_registered.c','tests/title-load-check.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources -o (Join-Path $root 'bin/title-load-check.exe')
if($LASTEXITCODE -ne 0){throw 'Title load compilation failed'}
