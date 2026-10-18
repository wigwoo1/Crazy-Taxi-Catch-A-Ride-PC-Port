$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/sprite_replacement.c') (Join-Path $root 'src/title_prompt_setup.c') (Join-Path $root 'tests/title-prompt-setup-rom-compare.c') -o (Join-Path $root 'bin/title-prompt-setup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Title prompt setup compilation failed'}
