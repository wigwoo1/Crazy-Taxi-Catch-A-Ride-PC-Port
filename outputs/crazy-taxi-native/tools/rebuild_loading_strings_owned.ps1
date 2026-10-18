$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/loading_present.c') (Join-Path $root 'src/string_lookup.c') (Join-Path $root 'src/loading_strings_owned.c') (Join-Path $root 'tests/loading_strings_owned_rom_compare.c') -o (Join-Path $root 'bin/loading-strings-owned-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Loading strings build failed'}
