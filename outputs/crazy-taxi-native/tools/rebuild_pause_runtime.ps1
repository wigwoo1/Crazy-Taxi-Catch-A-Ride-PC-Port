param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
$names=@('pause_runtime','pause_menu','pause_render','menu_input','palette_fade')
$sources=$names | ForEach-Object {Join-Path $root "src/$_.c"}
foreach($test in @('pause_runtime_test','pause_phase_test','pause_runtime_rom_compare')){
 $output=Join-Path $root ('bin/'+$test.Replace('_','-')+'.exe')
 & $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root "tests/$test.c") -o $output
 if($LASTEXITCODE -ne 0){throw "Pause build failed: $test"}
 if($test -ne 'pause_runtime_rom_compare'){& $output;if($LASTEXITCODE -ne 0){throw 'Pause lifecycle tests failed'}}
}
