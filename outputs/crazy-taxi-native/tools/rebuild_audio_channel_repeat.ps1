$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/audio_channel.c') (Join-Path $root 'src/audio_mix.c') (Join-Path $root 'tests/audio_channel_repeat_test.c') -o (Join-Path $root 'bin/audio-channel-repeat-test.exe')
if($LASTEXITCODE -ne 0){throw 'Repeat build failed'}
& (Join-Path $root 'bin/audio-channel-repeat-test.exe')
if($LASTEXITCODE -ne 0){throw 'Repeat test failed'}
