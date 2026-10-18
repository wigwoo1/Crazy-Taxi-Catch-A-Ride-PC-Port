$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix','random','music_buffer','audio_effects_buffer','audio_channel','audio_asset','audio_output','audio_frame','audio_reset','frame_wait','frame_event','audio_bootstrap','audio_configuration')
$sources=@()
foreach($name in $names){$sources+=Join-Path $root ('src/'+$name+'.c')}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/audio-reinitialize-rom-compare.c') -o (Join-Path $root 'bin/audio-reinitialize-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Audio reset compilation failed'}
