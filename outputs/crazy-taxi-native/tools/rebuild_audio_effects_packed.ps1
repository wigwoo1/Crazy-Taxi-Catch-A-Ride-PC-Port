$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $root 'src/audio_effects_buffer.c') (Join-Path $root 'src/audio_channel.c') (Join-Path $root 'src/audio_mix.c') (Join-Path $root 'src/audio_asset.c') (Join-Path $root 'tests/audio-effects-packed-rom-compare.c') -o (Join-Path $root 'bin/audio-effects-packed-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Audio effects compilation failed'}
