param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/audio_channel.c') (Join-Path $root 'src/audio_mix.c') (Join-Path $root 'tests/audio_stop_one_rom_compare.c') -o (Join-Path $root 'bin/audio-stop-one-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Single-channel stop build failed'}

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_cleanup.c') (Join-Path $root 'tests/game_audio_cleanup_rom_compare.c') -o (Join-Path $root 'bin/game-audio-cleanup-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Game audio cleanup build failed'}

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_cleanup.c') (Join-Path $root 'tests/game_audio_reset_rom_compare.c') -o (Join-Path $root 'bin/game-audio-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Game audio reset build failed'}
