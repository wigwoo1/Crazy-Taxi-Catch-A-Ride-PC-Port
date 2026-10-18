param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
foreach($test in @('normal_exit_test','normal_exit_rom_compare','normal_exit_credits_compare')){
 $output=Join-Path $root ('bin/'+$test.Replace('_','-')+'.exe')
 & $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/normal_exit.c') (Join-Path $root "tests/$test.c") -o $output
 if($LASTEXITCODE -ne 0){throw "Normal exit build failed: $test"}
 if($test -eq 'normal_exit_test'){& $output;if($LASTEXITCODE -ne 0){throw 'Normal exit lifecycle tests failed'}}
}
