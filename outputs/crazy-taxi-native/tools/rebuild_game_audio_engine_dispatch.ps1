$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_engine.c') (Join-Path $root 'tests/game_audio_engine_dispatch_test.c') -o (Join-Path $root 'bin/game-audio-engine-dispatch-test.exe')
if($LASTEXITCODE -ne 0){throw 'Engine dispatch build failed'}
& (Join-Path $root 'bin/game-audio-engine-dispatch-test.exe')
if($LASTEXITCODE -ne 0){throw 'Engine dispatch test failed'}
