$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_context_voice.c') (Join-Path $root 'tests/game_audio_context_voice_rom_compare.c') -o (Join-Path $root 'bin/game-audio-context-voice-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Context voice build failed'}
