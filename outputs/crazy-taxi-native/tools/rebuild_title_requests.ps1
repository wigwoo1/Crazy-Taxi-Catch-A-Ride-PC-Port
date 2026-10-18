$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('src/title_requests.c','src/title_obj_queue.c','src/title_callback.c','src/title_scroll.c','src/title_prompt.c','src/random.c','src/startup_obj_queue.c','src/startup_obj.c','src/obj_order.c','tests/title-requests-rom-compare.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources -o (Join-Path $root 'bin/title-requests-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title requests compilation failed'}
