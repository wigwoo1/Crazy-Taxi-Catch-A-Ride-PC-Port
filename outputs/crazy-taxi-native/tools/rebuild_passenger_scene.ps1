$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$Compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$bin=Join-Path $root 'bin'
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -UNDEBUG (Join-Path $root 'src/impact.c') (Join-Path $root 'src/passenger_idle.c') (Join-Path $root 'src/random.c') (Join-Path $root 'src/passenger_scene.c') (Join-Path $root 'src/passenger_delivery.c') (Join-Path $root 'src/passenger_exit.c') (Join-Path $root 'src/results_grade.c') (Join-Path $root 'src/fare_session.c') (Join-Path $root 'src/challenge_delivery.c') (Join-Path $root 'src/fare.c') (Join-Path $root 'src/trip_clock.c') (Join-Path $root 'src/game_settings.c') (Join-Path $root 'src/trip_start.c') (Join-Path $root 'src/trip_end.c') (Join-Path $root 'src/passenger_asset.c') (Join-Path $root 'src/passenger_init.c') (Join-Path $root 'src/passenger_route.c') (Join-Path $root 'src/passenger_search.c') (Join-Path $root 'src/passenger_pickup.c') (Join-Path $root 'src/passenger_boarding.c') (Join-Path $root 'src/destination.c') (Join-Path $root 'src/world_asset.c') (Join-Path $root 'src/ground.c') (Join-Path $root 'tests/passenger_scene_test.c') -o (Join-Path $bin 'passenger-scene-test.exe')
if($LASTEXITCODE -ne 0){throw 'Passenger scene build failed'}
foreach($city in @('city1','city2')){
 & (Join-Path $bin 'passenger-scene-test.exe') (Join-Path $root "assets/$city.ctw") (Join-Path $root "assets/$city.ctp") (Join-Path $root 'assets/driving.ctr')
 if($LASTEXITCODE -ne 0){throw "Passenger scene test failed: $city"}
}
