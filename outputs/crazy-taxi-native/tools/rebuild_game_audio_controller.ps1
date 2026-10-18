$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$names=@('controller','levels','engine','events','aux','vehicle_loop','reward_event','turn_loop','voice','context_voice')
$sources=@($names | ForEach-Object {Join-Path $root ('src/game_audio_'+$_+'.c')})
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @sources (Join-Path $root 'src/game_audio_reset_state.c') (Join-Path $root 'tests/game_audio_controller_test.c') -o (Join-Path $root 'bin/game-audio-controller-test.exe')
if($LASTEXITCODE -ne 0){throw 'Controller build failed'}
& (Join-Path $root 'bin/game-audio-controller-test.exe')
if($LASTEXITCODE -ne 0){throw 'Controller test failed'}

& (Join-Path $root "../../work/compiler/ziglang/zig.exe") cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root "src") @sources (Join-Path $root 'src/game_audio_reset_state.c') (Join-Path $root "tests/game_audio_controller_rom_compare.c") -o (Join-Path $root "bin/game-audio-controller-rom-compare.exe")
if($LASTEXITCODE -ne 0){throw "Controller comparison build failed"}
