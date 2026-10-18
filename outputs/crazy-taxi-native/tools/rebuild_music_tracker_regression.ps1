$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix')
$sources=@()
foreach($name in $names){$sources+=Join-Path $root ('src/'+$name+'.c')}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root "src") @sources (Join-Path $root 'tests/music_tracker_test.c') -o (Join-Path $root 'bin/music-tracker-regression.exe')
if($LASTEXITCODE -ne 0){throw 'Music row restart compilation failed'}
