param([string]$Compiler='')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $projectRoot '../../work/compiler/ziglang/zig.exe'}
$cacheRoot=Join-Path ([IO.Path]::GetTempPath()) 'crazy-taxi-native-zig-cache'
New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $cacheRoot 'global'
$env:ZIG_LOCAL_CACHE_DIR=Join-Path $cacheRoot 'local'
$taskArgs=@('cc','-std=c11','-Wall','-Wextra','-Werror','-O2',(Join-Path $projectRoot 'tests/traffic_follow_exact_event_probe.c'))
foreach($name in @('traffic_actor.c','traffic_follow.c','traffic_collision.c','traffic_effects.c','traffic_path.c','gears.c','random.c','impact.c')){$taskArgs+=(Join-Path $projectRoot ('src/'+$name))}
$taskArgs+=@('-I',(Join-Path $projectRoot 'src'),'-o',(Join-Path $projectRoot 'bin/traffic-follow-exact-event-probe.exe'))
& $Compiler $taskArgs
if($LASTEXITCODE -ne 0){throw 'Exact traffic-follow event probe compilation failed'}
