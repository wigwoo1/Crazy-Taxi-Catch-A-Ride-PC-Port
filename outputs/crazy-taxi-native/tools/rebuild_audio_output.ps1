$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/audio_output.c') (Join-Path $root 'src/audio_mix.c') (Join-Path $root 'tests/audio-output-rom-compare.c') -o (Join-Path $root 'bin/audio-output-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Audio output compilation failed'}
