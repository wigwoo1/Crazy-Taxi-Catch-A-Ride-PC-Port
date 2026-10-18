param([string]$Compiler='')
$ErrorActionPreference='Stop'
$projectRoot=Split-Path -Parent $PSScriptRoot
if(!$Compiler){$Compiler=Join-Path $projectRoot '../../work/compiler/ziglang/zig.exe'}
$env:ZIG_GLOBAL_CACHE_DIR=Join-Path $projectRoot '.zig-global-cache'
$env:ZIG_LOCAL_CACHE_DIR=Join-Path $projectRoot '.zig-local-cache'
New-Item -ItemType Directory -Force -Path $env:ZIG_GLOBAL_CACHE_DIR,$env:ZIG_LOCAL_CACHE_DIR | Out-Null
$sourceNames=@('drive_world.c','world_update.c','vehicle_start_driver.c','vehicle_start_world.c','driving_shared_snapshot.c','input_snapshot.c','input.c','actions.c','runtime_asset.c','spawn_placement.c','world_asset.c','ground.c','driving.c','driver.c','steering_input.c','acceleration.c','engine_curve.c','braking.c','free_rev.c','gears.c','vehicle_update.c','motion.c','orientation.c','traction.c','vehicle_math.c','impact.c','movement.c','height_update.c','body_pose.c','camera_angle.c','engine_rotate_owned.c','engine_matrix_call.c','engine_callback_identity.c','vehicle_config.c','approach.c','traffic_actor.c','traffic_actor_impact.c','traffic_motion.c','traffic_follow.c','traffic_collision.c','traffic_effects.c','traffic_path.c','traffic_grid.c','traffic_spawn.c','traffic_frame.c','traffic_init.c','traffic_presentation.c','traffic_heading.c','ai_path.c','random.c')
$taskArgs=@('cc','-std=c11','-Wall','-Wextra','-Werror','-O2','-UNDEBUG',(Join-Path $projectRoot 'tests/route31_source_traffic_replay_probe.c'))
foreach($sourceName in $sourceNames){$taskArgs+=(Join-Path $projectRoot "src/$sourceName")}
$taskArgs+=@('-o',(Join-Path $projectRoot 'bin/route31-source-traffic-replay-probe.exe'))
& $Compiler $taskArgs
if($LASTEXITCODE -ne 0){throw 'Iterative source traffic replay probe compilation failed'}
