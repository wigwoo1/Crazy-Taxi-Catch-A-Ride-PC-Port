param([string]$Compiler='')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'}
$cache=Join-Path ([IO.Path]::GetTempPath()) 'crazy-taxi-traffic-dispatch-cache'
New-Item -ItemType Directory -Force -Path $cache | Out-Null
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $cache 'global'
$env:ZIG_LOCAL_CACHE_DIR=Join-Path $cache 'local'
$src=Join-Path $root 'src'
$compileArgs=@('cc','-std=c11','-Wall','-Wextra','-Werror','-O2','-UNDEBUG',(Join-Path $root 'tests/native_traffic_dispatch_probe.c'))
$names=@(
 'reference_driving.c','drive_world.c','world_update.c','vehicle_start_driver.c','vehicle_start_world.c',
 'approach.c','driving_shared_snapshot.c','input_snapshot.c','input.c','actions.c','runtime_asset.c',
 'spawn_placement.c','world_asset.c','ground.c','driving.c','driver.c','steering_input.c','acceleration.c',
 'engine_curve.c','braking.c','free_rev.c','gears.c','vehicle_update.c','motion.c','orientation.c',
 'traction.c','vehicle_math.c','impact.c','movement.c','height_update.c','body_pose.c','camera_angle.c',
 'engine_rotate_owned.c','engine_matrix_call.c','engine_callback_identity.c','vehicle_config.c',
 'traffic_actor.c','traffic_actor_impact.c','traffic_follow.c','traffic_collision.c','traffic_effects.c',
 'traffic_motion.c','traffic_path.c','traffic_grid.c','traffic_spawn.c','traffic_frame.c',
 'traffic_presentation.c','traffic_heading.c','traffic_init.c','passenger_idle.c','random.c'
)
foreach($name in $names){$compileArgs+=(Join-Path $src $name)}
$compileArgs+=@('-o',(Join-Path $root 'bin/native-traffic-dispatch-probe.exe'))
& $Compiler $compileArgs
if($LASTEXITCODE -ne 0){throw 'Native traffic dispatcher diagnostic build failed'}
Write-Output 'PASS strict native traffic dispatcher diagnostic build'
