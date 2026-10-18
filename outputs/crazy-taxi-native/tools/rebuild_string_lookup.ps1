$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/string_lookup.c') (Join-Path $root 'tests/string_lookup_rom_compare.c') -o (Join-Path $root 'bin/string-lookup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'String lookup build failed'}
