$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('title_callback','title_scroll','title_prompt','random')|ForEach-Object{Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/title-callback-rom-compare.c') -o (Join-Path $root 'bin/title-callback-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title callback comparison compilation failed'}
