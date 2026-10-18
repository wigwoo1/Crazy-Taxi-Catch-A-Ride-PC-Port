param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_reward_event.c') (Join-Path $root 'tests/game_audio_reward_event_rom_compare.c') -o (Join-Path $root 'bin/game-audio-reward-event-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Game audio reward event build failed'}
