$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$files=@('game_audio_voice','game_audio_context_voice','game_audio_channels','game_audio_engine','audio_channel','audio_mix','audio_asset') | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'tests/game_audio_voice_dispatch_test.c') -o (Join-Path $root 'bin/game-audio-voice-dispatch-test.exe')
if($LASTEXITCODE -ne 0){throw 'Voice dispatch build failed'}
& (Join-Path $root 'bin/game-audio-voice-dispatch-test.exe') (Join-Path $root 'assets/audio')
if($LASTEXITCODE -ne 0){throw 'Voice dispatch test failed'}
