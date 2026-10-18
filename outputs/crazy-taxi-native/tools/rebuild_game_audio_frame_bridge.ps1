$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_context_voice.c') (Join-Path $root 'src/game_audio_frame_bridge.c') (Join-Path $root 'tests/game_audio_frame_bridge_test.c') -o (Join-Path $root 'bin/game-audio-frame-bridge-test.exe')
if($LASTEXITCODE -ne 0){throw 'Frame bridge build failed'}
& (Join-Path $root 'bin/game-audio-frame-bridge-test.exe')
if($LASTEXITCODE -ne 0){throw 'Frame bridge test failed'}
