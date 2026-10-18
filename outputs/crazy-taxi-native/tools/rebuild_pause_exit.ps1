param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
$sources=@('pause_exit','normal_exit','pause_runtime','pause_menu','pause_render','menu_input','palette_fade') | ForEach-Object {Join-Path $root "src/$_.c"}
$output=Join-Path $root 'bin/pause-exit-test.exe'
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/pause_exit_test.c') -o $output
if($LASTEXITCODE -ne 0){throw 'Pause exit build failed'}
& $output
if($LASTEXITCODE -ne 0){throw 'Pause exit tests failed'}

# Old returning-child comparison retired by retry-transfer-rom-audit.json.
