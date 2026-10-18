$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
foreach($test in @('audio-host-stop','audio-host-stall')){
 & $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/audio_host.c') (Join-Path $root ('tests/'+$test+'.c')) -lwinmm -o (Join-Path $root ('bin/'+$test+'.exe'))
 if($LASTEXITCODE -ne 0){throw "$test compilation failed"}
 & (Join-Path $root ('bin/'+$test+'.exe'))
 if($LASTEXITCODE -ne 0){throw "$test verification failed"}
}
