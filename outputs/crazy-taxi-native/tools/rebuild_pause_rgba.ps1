param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/palette_fade.c') (Join-Path $root 'tests/pause_rgba_test.c') -o (Join-Path $root 'bin/pause-rgba-test.exe')
if($LASTEXITCODE -ne 0){throw 'Pause RGBA build failed'}
& (Join-Path $root 'bin/pause-rgba-test.exe')
if($LASTEXITCODE -ne 0){throw 'Pause RGBA test failed'}
