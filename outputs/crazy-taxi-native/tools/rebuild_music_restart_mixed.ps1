$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$names=@('music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix','random')
$sources=@()
foreach($name in $names){$sources+=Join-Path $root ('src/'+$name+'.c')}
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 @sources (Join-Path $root 'tests/music-restart-mixed-rom-compare.c') -o (Join-Path $root 'bin/music-restart-mixed-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Music row restart compilation failed'}
