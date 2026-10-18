$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('loading_text_owned','obj_queue_owned','text_layout','font_setup','string_lookup','startup_obj_queue','startup_obj','obj_order','title_obj_queue')
$sources=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'tests/loading_text_owned_rom_compare.c') -o (Join-Path $root 'bin/loading-text-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Loading text owner build failed'}
