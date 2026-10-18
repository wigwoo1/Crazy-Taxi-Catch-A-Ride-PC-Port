param([string]$Compiler='')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $projectRoot '../../work/compiler/ziglang/zig.exe'}
$cacheRoot=Join-Path ([IO.Path]::GetTempPath()) 'crazy-taxi-native-zig-cache'
New-Item -ItemType Directory -Force -Path $cacheRoot | Out-Null
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $cacheRoot 'global'
$env:ZIG_LOCAL_CACHE_DIR=Join-Path $cacheRoot 'local'
$sources=@('drive_world','world_update','vehicle_start_driver','vehicle_start_world','approach',
 'driving_shared_snapshot','input_snapshot','input','actions','runtime_asset','spawn_placement',
 'world_asset','ground','driving','driver','steering_input','acceleration','engine_curve','braking',
 'free_rev','gears','vehicle_update','motion','orientation','traction','vehicle_math','impact',
 'movement','height_update','body_pose','camera_angle','engine_rotate_owned','engine_matrix_call',
 'engine_callback_identity','vehicle_config','reference_driving','traffic_scene','traffic_frame',
 'traffic_init','traffic_spawn','traffic_grid','traffic_actor','traffic_actor_impact','traffic_motion','traffic_follow',
 'traffic_collision','traffic_effects','traffic_path','traffic_presentation','traffic_view',
 'traffic_sprite','traffic_heading','ai_path','passenger_search','passenger_idle','passenger_boarding','random')
$taskArgs=@('cc','-std=c11','-Wall','-Wextra','-Werror','-O2','-UNDEBUG','-I',(Join-Path $projectRoot 'src'))
$taskArgs+=(Join-Path $projectRoot 'tests/continuous_driver_traffic_probe.c')
foreach($source in $sources){$taskArgs+=(Join-Path $projectRoot "src/$source.c")}
$binary=Join-Path $projectRoot 'bin/continuous-driver-traffic-probe.exe'
$taskArgs+=@('-o',$binary)
& $Compiler $taskArgs
if($LASTEXITCODE -ne 0){throw 'Continuous native driver/traffic probe compilation failed'}
Write-Output 'PASS strict continuous native driver/traffic probe build'
