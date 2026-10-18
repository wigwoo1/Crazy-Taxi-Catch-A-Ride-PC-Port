$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/audio_configuration.c') (Join-Path $root 'tests/audio-configuration-rom-compare.c') -o (Join-Path $root 'bin/audio-configuration-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Audio configuration compilation failed'}
