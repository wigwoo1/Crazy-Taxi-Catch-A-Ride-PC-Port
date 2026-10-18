param([string]$Compiler='')
$ErrorActionPreference='Stop'
$taskProject=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $taskProject '../../work/compiler/ziglang/zig.exe'}
foreach($taskTest in @('xbox_selection_test','xbox_test')){
 $taskSources=@((Join-Path $taskProject ('tests/'+$taskTest+'.c')),(Join-Path $taskProject 'src/xbox.c'))
 if($taskTest -eq 'xbox_test'){$taskSources+=(Join-Path $taskProject 'src/steering_input.c')}
 $taskBinary=Join-Path $taskProject ('bin/'+$taskTest+'-current.exe')
 & $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -UNDEBUG -I (Join-Path $taskProject 'src') $taskSources -o $taskBinary
 if($LASTEXITCODE -ne 0){throw ('Failed build '+$taskTest)}
 & $taskBinary
 if($LASTEXITCODE -ne 0){throw ('Failed verification '+$taskTest)}
}
