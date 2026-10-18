$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('normal_restart','normal_restart_owned','music_session','music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix','random','music_playback','restart_resources','palette_registration','shade_table','display_mode','graphics_memory_base')
$files=$names | ForEach-Object {Join-Path $root ('src/'+$_+'.c')}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @files (Join-Path $root 'tests/normal-restart-owned-check.c') -o (Join-Path $root 'bin/normal-restart-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Native restart owner build failed'}
& (Join-Path $root 'bin/normal-restart-owned-check.exe')
if($LASTEXITCODE -ne 0){throw 'Native restart owner verification failed'}
