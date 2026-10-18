$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$files=@('game_audio_events','game_audio_final_event','game_audio_reward_event','game_audio_channels','game_audio_engine','audio_channel','audio_mix','audio_asset') | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'tests/game_audio_event_dispatch_test.c') -o (Join-Path $root 'bin/game-audio-event-dispatch-test.exe')
if($LASTEXITCODE -ne 0){throw 'Event dispatch build failed'}
& (Join-Path $root 'bin/game-audio-event-dispatch-test.exe') (Join-Path $root 'assets/audio')
if($LASTEXITCODE -ne 0){throw 'Event dispatch test failed'}
