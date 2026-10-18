param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$bin=Join-Path $root 'bin'
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
$sources=Get-ChildItem (Join-Path $root 'src/music_*.c') | ForEach-Object {$_.FullName}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'src/audio_mix.c') (Join-Path $root 'tests/music_pause_tracker_test.c') -o (Join-Path $bin 'music-pause-tracker-test.exe')
if($LASTEXITCODE -ne 0){throw 'Pause music build failed'}
& (Join-Path $bin 'music-pause-tracker-test.exe') (Join-Path $root 'assets/music/original-module.bin') (Join-Path $root 'assets/music/pitch-table.bin')
if($LASTEXITCODE -ne 0){throw 'Pause music tests failed'}
