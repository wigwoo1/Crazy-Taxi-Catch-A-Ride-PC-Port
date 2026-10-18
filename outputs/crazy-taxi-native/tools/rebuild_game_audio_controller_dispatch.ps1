$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('game_audio_controller','game_audio_levels','game_audio_engine','game_audio_events','game_audio_aux','game_audio_vehicle_loop','game_audio_reward_event','game_audio_turn_loop','game_audio_voice','game_audio_context_voice','game_audio_channels','audio_channel','audio_mix','audio_asset')
$files=@($names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')})
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'src/game_audio_reset_state.c') (Join-Path $root 'tests/game_audio_controller_dispatch_test.c') -o (Join-Path $root 'bin/game-audio-controller-dispatch-test.exe')
if($LASTEXITCODE -ne 0){throw 'Full dispatch build failed'}
& (Join-Path $root 'bin/game-audio-controller-dispatch-test.exe') (Join-Path $root 'assets/audio')
if($LASTEXITCODE -ne 0){throw 'Full dispatch test failed'}

$rewardFiles=@('reward','random' | ForEach-Object {Join-Path $root ('src/'+$_+'.c')})
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'src/game_audio_reset_state.c') @rewardFiles (Join-Path $root 'tests/game_audio_controller_reward_shared_test.c') -o (Join-Path $root 'bin/game-audio-controller-reward-shared-test.exe')
if($LASTEXITCODE -ne 0){throw 'Shared reward dispatch build failed'}
& (Join-Path $root 'bin/game-audio-controller-reward-shared-test.exe')
if($LASTEXITCODE -ne 0){throw 'Shared reward dispatch test failed'}

& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -Wno-misleading-indentation -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'src/game_audio_reset_state.c') (Join-Path $root 'tests/game_audio_controller_reset_rom_compare.c') -o (Join-Path $root 'bin/game-audio-controller-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Full reset comparison build failed'}

