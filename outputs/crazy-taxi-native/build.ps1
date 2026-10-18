param([string]$Compiler = (Join-Path $PSScriptRoot '../../work/compiler/ziglang/zig.exe'))
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'verify-traffic.ps1') -Compiler $Compiler
$bin = Join-Path $PSScriptRoot 'bin'
New-Item -ItemType Directory -Force -Path $bin | Out-Null
$pcDisplayTest = Join-Path $bin 'pc-display-settings-test.exe'
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/pc_display_settings.c') (Join-Path $PSScriptRoot 'tests/pc_display_settings_test.c') -o $pcDisplayTest
if ($LASTEXITCODE -ne 0) { throw 'PC display settings compilation failed' }
& $pcDisplayTest
if ($LASTEXITCODE -ne 0) { throw 'PC display settings checks failed' }
# Text smoothing remains an archived, offline experiment. Keep text out of all
# standard generated-texture/build paths; the PC Display UI locks it to Original.
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/city_background.c') (Join-Path $PSScriptRoot 'tests/city_background_test.c') -o (Join-Path $bin 'city-background-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'City background compilation failed' }
& (Join-Path $bin 'city-background-test.exe') (Join-Path $PSScriptRoot 'assets/city-selection')
if ($LASTEXITCODE -ne 0) { throw 'City background checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/city_flow.c') (Join-Path $PSScriptRoot 'src/city_choice.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/city_flow_test.c') -o (Join-Path $bin 'city-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'City flow compilation failed' }
& (Join-Path $bin 'city-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'City flow checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/city_choice.c') (Join-Path $PSScriptRoot 'tests/city_choice_test.c') -o (Join-Path $bin 'city-choice-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'City choice compilation failed' }
& (Join-Path $bin 'city-choice-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'City choice checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/driver_background.c') (Join-Path $PSScriptRoot 'tests/driver_background_test.c') -o (Join-Path $bin 'driver-background-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver background compilation failed' }
& (Join-Path $bin 'driver-background-test.exe') (Join-Path $PSScriptRoot 'assets/driver-selection')
if ($LASTEXITCODE -ne 0) { throw 'Driver background checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/driver_flow.c') (Join-Path $PSScriptRoot 'src/driver_choice.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/driver_flow_test.c') -o (Join-Path $bin 'driver-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver flow compilation failed' }
& (Join-Path $bin 'driver-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver flow checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/driver_choice.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'tests/driver_choice_test.c') -o (Join-Path $bin 'driver-choice-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver choice compilation failed' }
& (Join-Path $bin 'driver-choice-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver choice checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/save_file.c') (Join-Path $PSScriptRoot 'src/save_block.c') (Join-Path $PSScriptRoot 'tests/save_file_test.c') -o (Join-Path $bin 'save-file-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Save file compilation failed' }
& (Join-Path $bin 'save-file-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Save file checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/profile_commit.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/profile_commit_test.c') -o (Join-Path $bin 'profile-commit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile commit compilation failed' }
& (Join-Path $bin 'profile-commit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile commit checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/profile_score_test.c') -o (Join-Path $bin 'profile-score-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile score compilation failed' }
& (Join-Path $bin 'profile-score-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile score checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/save_block.c') (Join-Path $PSScriptRoot 'tests/save_defaults_test.c') -o (Join-Path $bin 'save-defaults-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Save defaults compilation failed' }
& (Join-Path $bin 'save-defaults-test.exe') (Join-Path $PSScriptRoot 'assets/results/default-scores.cts')
if ($LASTEXITCODE -ne 0) { throw 'Save defaults checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/profile_flow.c') (Join-Path $PSScriptRoot 'src/profile_choice.c') (Join-Path $PSScriptRoot 'src/profile_edit.c') (Join-Path $PSScriptRoot 'src/name_entry.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/profile_flow_test.c') -o (Join-Path $bin 'profile-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile flow compilation failed' }
& (Join-Path $bin 'profile-flow-test.exe') (Join-Path $PSScriptRoot 'assets/results/name-characters.ctn')
if ($LASTEXITCODE -ne 0) { throw 'Profile flow checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/profile_choice.c') (Join-Path $PSScriptRoot 'src/profile_edit.c') (Join-Path $PSScriptRoot 'src/name_entry.c') (Join-Path $PSScriptRoot 'tests/profile_choice_test.c') -o (Join-Path $bin 'profile-choice-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile choice compilation failed' }
& (Join-Path $bin 'profile-choice-test.exe') (Join-Path $PSScriptRoot 'assets/results/name-characters.ctn')
if ($LASTEXITCODE -ne 0) { throw 'Profile choice checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/profile_edit.c') (Join-Path $PSScriptRoot 'src/name_entry.c') (Join-Path $PSScriptRoot 'tests/profile_edit_test.c') -o (Join-Path $bin 'profile-edit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Profile edit compilation failed' }
& (Join-Path $bin 'profile-edit-test.exe') (Join-Path $PSScriptRoot 'assets/results/name-characters.ctn')
if ($LASTEXITCODE -ne 0) { throw 'Profile edit checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/save_block.c') (Join-Path $PSScriptRoot 'tests/save_block_test.c') -o (Join-Path $bin 'save-block-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Save block compilation failed' }
& (Join-Path $bin 'save-block-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Save block checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'tests/menu_input_test.c') -o (Join-Path $bin 'menu-input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Menu input compilation failed' }
& (Join-Path $bin 'menu-input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Menu input checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/name_flow.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/name_entry.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/name_flow_test.c') -o (Join-Path $bin 'name-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Name flow compilation failed' }
& (Join-Path $bin 'name-flow-test.exe') (Join-Path $PSScriptRoot 'assets/name-entry/layout.cnl') (Join-Path $PSScriptRoot 'assets/results/name-characters.ctn')
if ($LASTEXITCODE -ne 0) { throw 'Name flow checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/game_over.c') (Join-Path $PSScriptRoot 'tests/game_over_test.c') -o (Join-Path $bin 'game-over-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Game Over compilation failed' }
& (Join-Path $bin 'game-over-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Game Over checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/score_insert_test.c') -o (Join-Path $bin 'score-insert-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Score insert compilation failed' }
& (Join-Path $bin 'score-insert-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Score insert checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/name_entry.c') (Join-Path $PSScriptRoot 'tests/name_entry_test.c') -o (Join-Path $bin 'name-entry-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Name entry compilation failed' }
& (Join-Path $bin 'name-entry-test.exe') (Join-Path $PSScriptRoot 'assets/results/name-characters.ctn') (Join-Path $PSScriptRoot 'assets/name-entry/layout.cnl')
if ($LASTEXITCODE -ne 0) { throw 'Name entry checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/results_flow.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/results_flow_test.c') -o (Join-Path $bin 'results-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Results flow compilation failed' }
& (Join-Path $bin 'results-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Results flow checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'tests/results_grade_test.c') -o (Join-Path $bin 'results-grade-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Results grade compilation failed' }
& (Join-Path $bin 'results-grade-test.exe') (Join-Path $PSScriptRoot 'assets/results/default-scores.cts')
if ($LASTEXITCODE -ne 0) { throw 'Results grade checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'tests/results_state_test.c') -o (Join-Path $bin 'results-state-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Results state compilation failed' }
& (Join-Path $bin 'results-state-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Results state checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'tests/font_asset_test.c') -o (Join-Path $bin 'font-asset-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Font asset compilation failed' }
& (Join-Path $bin 'font-asset-test.exe') (Join-Path $PSScriptRoot 'assets/results/font0-metrics.ctf') (Join-Path $PSScriptRoot 'assets/results/font0.rgba') (Join-Path $PSScriptRoot 'assets/results/font1-metrics.ctf') (Join-Path $PSScriptRoot 'assets/results/font1.rgba') (Join-Path $PSScriptRoot 'assets/results/font2-metrics.ctf') (Join-Path $PSScriptRoot 'assets/results/font2.rgba') (Join-Path $PSScriptRoot 'assets/name-entry/font0-metrics.ctf') (Join-Path $PSScriptRoot 'assets/name-entry/font0.rgba') (Join-Path $PSScriptRoot 'assets/name-entry/font1-metrics.ctf') (Join-Path $PSScriptRoot 'assets/name-entry/font1.rgba') (Join-Path $PSScriptRoot 'assets/name-entry/font2-metrics.ctf') (Join-Path $PSScriptRoot 'assets/name-entry/font2.rgba') (Join-Path $PSScriptRoot 'assets/ranking/font0-metrics.ctf') (Join-Path $PSScriptRoot 'assets/ranking/font0.rgba') (Join-Path $PSScriptRoot 'assets/profile/font0-metrics.ctf') (Join-Path $PSScriptRoot 'assets/profile/font0.rgba') (Join-Path $PSScriptRoot 'assets/profile/font1-metrics.ctf') (Join-Path $PSScriptRoot 'assets/profile/font1.rgba')
if ($LASTEXITCODE -ne 0) { throw 'Font asset checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/text_layout.c') (Join-Path $PSScriptRoot 'tests/text_layout_test.c') -o (Join-Path $bin 'text-layout-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Text layout compilation failed' }
& (Join-Path $bin 'text-layout-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Text layout checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/palette_fade_test.c') -o (Join-Path $bin 'palette-fade-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette fade compilation failed' }
& (Join-Path $bin 'palette-fade-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette fade checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'tests/random_test.c') -o (Join-Path $bin 'random-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Original RNG compilation failed' }
& (Join-Path $bin 'random-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Original RNG checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/destination_guidance.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/destination_guidance_test.c') -o (Join-Path $bin 'destination-guidance-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Destination guidance compilation failed' }
& (Join-Path $bin 'destination-guidance-test.exe') (Join-Path $PSScriptRoot 'assets/guidance/city1.ctg') (Join-Path $PSScriptRoot 'assets/guidance/city2.ctg') (Join-Path $PSScriptRoot 'assets/guidance/startup.ctg')
if ($LASTEXITCODE -ne 0) { throw 'Destination guidance checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_idle.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'tests/passenger_idle_test.c') -o (Join-Path $bin 'passenger-idle-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger idle compilation failed' }
& (Join-Path $bin 'passenger-idle-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger idle checks failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_exit.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/passenger_exit_test.c') -o (Join-Path $bin 'passenger-exit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger exit compilation failed' }
& (Join-Path $bin 'passenger-exit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger exit tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'src/fare_session.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'tests/fare_session_test.c') -o (Join-Path $bin 'fare-session-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Fare session compilation failed' }
& (Join-Path $bin 'fare-session-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Fare session tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'tests/trip_clock_test.c') -o (Join-Path $bin 'trip-clock-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip clock compilation failed' }
& (Join-Path $bin 'trip-clock-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip clock tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'tests/game_settings_test.c') -o (Join-Path $bin 'game-settings-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Game settings compilation failed' }
& (Join-Path $bin 'game-settings-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Game settings tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/passenger_idle.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'src/passenger_scene.c') (Join-Path $PSScriptRoot 'src/rank_display.c') (Join-Path $PSScriptRoot 'src/rank_display_owned.c') (Join-Path $PSScriptRoot 'src/settlement_display.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_exit.c') (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'src/fare_session.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'src/passenger_boarding.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/passenger_scene_test.c') -o (Join-Path $bin 'passenger-scene-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger scene compilation failed' }
foreach ($city in @('city1','city2','startup')) {
    & (Join-Path $bin 'passenger-scene-test.exe') (Join-Path $PSScriptRoot "assets/$city.ctw") (Join-Path $PSScriptRoot "assets/$city.ctp") (Join-Path $PSScriptRoot "assets/driving.ctr")
    if ($LASTEXITCODE -ne 0) { throw 'Passenger scene tests failed' }
}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'tests/fare_test.c') -o (Join-Path $bin 'fare-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Fare compilation failed' }
& (Join-Path $bin 'fare-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Fare tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'tests/trip_end_test.c') -o (Join-Path $bin 'trip-end-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip end compilation failed' }
& (Join-Path $bin 'trip-end-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip end tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'tests/passenger_delivery_test.c') -o (Join-Path $bin 'passenger-delivery-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger delivery compilation failed' }
& (Join-Path $bin 'passenger-delivery-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger delivery tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'tests/trip_start_test.c') -o (Join-Path $bin 'trip-start-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip start compilation failed' }
& (Join-Path $bin 'trip-start-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Trip start tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'tests/passenger_asset_check.c') -o (Join-Path $bin 'passenger-asset-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger asset check compilation failed' }
foreach ($city in @('city1','city2','startup')) {
    & (Join-Path $bin 'passenger-asset-check.exe') (Join-Path $PSScriptRoot "assets/$city.ctp")
    if ($LASTEXITCODE -ne 0) { throw 'Passenger asset check failed' }
}
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_sprite_asset.c') (Join-Path $PSScriptRoot 'tests/passenger_sprite_asset_test.c') -o (Join-Path $bin 'passenger-sprite-asset-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger sprite asset compilation failed' }
& (Join-Path $bin 'passenger-sprite-asset-test.exe') (Join-Path $PSScriptRoot 'assets/passengers')
if ($LASTEXITCODE -ne 0) { throw 'Passenger sprite asset tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_boarding.c') (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'tests/passenger_boarding_test.c') -o (Join-Path $bin 'passenger-boarding-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger boarding compilation failed' }
& (Join-Path $bin 'passenger-boarding-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger boarding tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'tests/passenger_search_test.c') -o (Join-Path $bin 'passenger-search-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger search compilation failed' }
& (Join-Path $bin 'passenger-search-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger search tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'tests/passenger_pickup_test.c') -o (Join-Path $bin 'passenger-pickup-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger pickup compilation failed' }
& (Join-Path $bin 'passenger-pickup-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger pickup tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/passenger_init_test.c') -o (Join-Path $bin 'passenger-init-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger initialization compilation failed' }
& (Join-Path $bin 'passenger-init-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger initialization tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/passenger_route_test.c') -o (Join-Path $bin 'passenger-route-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger route compilation failed' }
& (Join-Path $bin 'passenger-route-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger route tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/passenger_route_probe.c') -o (Join-Path $bin 'passenger-route-probe.exe')
if ($LASTEXITCODE -ne 0) { throw 'Passenger route probe compilation failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'tests/destination_test.c') -o (Join-Path $bin 'destination-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Destination compilation failed' }
& (Join-Path $bin 'destination-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Destination tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'tests/braking_test.c') -o (Join-Path $bin 'braking-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Braking compilation failed' }
& (Join-Path $bin 'braking-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Braking tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'tests/acceleration_test.c') -o (Join-Path $bin 'acceleration-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Acceleration compilation failed' }
& (Join-Path $bin 'acceleration-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Acceleration tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'tests/vehicle_reset_test.c') -o (Join-Path $bin 'vehicle-reset-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'vehicle_reset compilation failed' }
& (Join-Path $bin 'vehicle-reset-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'vehicle_reset tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'tests/engine_curve_test.c') -o (Join-Path $bin 'engine-curve-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'engine_curve compilation failed' }
& (Join-Path $bin 'engine-curve-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'engine_curve tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'tests/vehicle_config_test.c') -o (Join-Path $bin 'vehicle-config-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle configuration test compilation failed' }
& (Join-Path $bin 'vehicle-config-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle configuration tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'tests/vehicle_update_test.c') -o (Join-Path $bin 'vehicle-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle update test compilation failed' }
& (Join-Path $bin 'vehicle-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle update tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'tests/retained_motion_compare.c') -o (Join-Path $bin 'retained-motion-compare.exe')
if ($LASTEXITCODE -ne 0) { throw 'Retained motion comparison compilation failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/traction_test.c') -o (Join-Path $bin 'traction-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Traction test compilation failed' }
& (Join-Path $bin 'traction-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Traction tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'tests/motion_test.c') -o (Join-Path $bin 'motion-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Motion test compilation failed' }
& (Join-Path $bin 'motion-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Motion tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/actions.c') (Join-Path $PSScriptRoot 'tests/gears_test.c') -o (Join-Path $bin 'gears-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Gear test compilation failed' }
& (Join-Path $bin 'gears-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Gear tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'tests/approach_test.c') -o (Join-Path $bin 'approach-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Approach compilation failed' }
& (Join-Path $bin 'approach-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Approach tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_start_driver.c') (Join-Path $PSScriptRoot 'tests/vehicle_start_driver_state_test.c') -o (Join-Path $bin 'vehicle-start-driver-state-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle start driver state test compilation failed' }
& (Join-Path $bin 'vehicle-start-driver-state-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle start driver state tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/input.c') (Join-Path $PSScriptRoot 'tests/driving_test.c') -o (Join-Path $bin 'driving-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driving test compilation failed' }
& (Join-Path $bin 'driving-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driving tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/impact_test.c') -o (Join-Path $bin 'impact-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Impact test compilation failed' }
& (Join-Path $bin 'impact-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Impact tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/movement_test.c') -o (Join-Path $bin 'movement-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Movement test compilation failed' }
& (Join-Path $bin 'movement-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Movement tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/ground_test.c') -o (Join-Path $bin 'ground-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ground test compilation failed' }
& (Join-Path $bin 'ground-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ground tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'tests/ground_rom_check.c') -o (Join-Path $bin 'ground-rom-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ground ROM check compilation failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'tests/vehicle_math_test.c') -o (Join-Path $bin 'vehicle-math-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle math test compilation failed' }
& (Join-Path $bin 'vehicle-math-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Vehicle math tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/input.c') (Join-Path $PSScriptRoot 'tests/input_test.c') -o (Join-Path $bin 'input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Input test compilation failed' }
& (Join-Path $bin 'input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Input tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/actions.c') (Join-Path $PSScriptRoot 'tests/actions_test.c') -o (Join-Path $bin 'actions-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Action resolver test compilation failed' }
& (Join-Path $bin 'actions-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Action resolver tests failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/input.c') (Join-Path $PSScriptRoot 'src/xbox.c') (Join-Path $PSScriptRoot 'tests/controller_check.c') -o (Join-Path $bin 'controller-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Controller check compilation failed' }
$sceneViewerSourceNames = @(
    'music_playback.c'
    'pause_runtime.c'
    'pause_menu.c'
    'pause_render.c'
    'ground.c'
    'movement.c'
    'impact.c'
    'motion.c'
    'orientation.c'
    'driving.c'
    'body_pose.c'
    'height_update.c'
    'world_update.c'
    'vehicle_math.c'
    'drive_world.c'
    'startup_world.c'
    'game_start.c'
    'driver.c'
    'approach.c'
    'free_rev.c'
    'steering_input.c'
    'acceleration.c'
    'engine_curve.c'
    'braking.c'
    'gears.c'
    'vehicle_config.c'
    'vehicle_update.c'
    'traction.c'
    'spawn_placement.c'
    'simulation_start.c'
    'vehicle_reset.c'
    'world_asset.c'
    'runtime_asset.c'
    'camera_angle.c'
    'passenger_idle.c'
    'random.c'
    'passenger_scene.c'
    'passenger_delivery.c'
    'passenger_exit.c'
    'results_grade.c'
    'fare_session.c'
    'rank_hud.c','rank_bitmap.c'
    'rank_display.c'
    'rank_display_owned.c'
    'settlement_display.c'
    'challenge_delivery.c'
    'fare.c'
    'trip_clock.c'
    'game_settings.c'
    'trip_start.c'
    'trip_end.c'
    'passenger_asset.c'
    'passenger_sprite_asset.c'
    'passenger_init.c'
    'passenger_init_pending.c'
    'passenger_init_world.c'
    'passenger_setup_owned.c'
    'restart_setup_native.c'
    'taxi_graphics_owned.c'
    'loading_present.c'
    'loading_strings_owned.c'
    'string_lookup.c'
    'loading_indexed_owned.c'
    'loading_sprites_owned.c'
    'loading_objects_owned.c'
    'obj_queue_owned.c'
    'loading_text_owned.c'
    'loading_content_owned.c'
    'ordinary_loading_owned.c'
    'ordinary_loading_frame.c'
    'ordinary_loading_pending.c'
    'normal_restart.c'
    'normal_restart_owned.c'
    'normal_restart_native.c'
    'restart_visual_owned.c'
    'restart_resources.c'
    'graphics_memory_base.c'
    'normal_restart_graphics.c'
    'normal_restart_entry.c'
    'normal_restart_flow.c'
    'normal_restart_presentation_owned.c'
    'ordinary_loading_buffers.c'
    'noncustomer_init.c'
    'noncustomer_pool.c'
    'passenger_route.c'
    'passenger_search.c'
    'passenger_pickup.c'
    'passenger_boarding.c'
    'destination.c'
    'destination_guidance.c'
    'palette_fade.c'
    'font_asset.c'
    'text_layout.c'
    'results_state.c'
    'results_flow.c'
    'game_over.c'
    'name_entry.c'
    'name_flow.c'
    'menu_input.c'
    'profile_flow.c'
    'profile_choice.c'
    'profile_edit.c'
    'save_block.c'
    'profile_commit.c'
    'score_insert.c'
    'save_file.c'
    'driver_background.c'
    'city_background.c'
    'city_flow.c'
    'city_choice.c'
    'driver_choice.c'
    'driver_flow.c'
    'normal_menu_background.c'
    'normal_menu_text.c'
    'font_sdf.c'
    'pc_display_settings.c'
    'menu_footer_layout.c'
    'traffic_init.c'
    'traffic_sprite.c'
    'traffic_assets.c'
    'traffic_grid.c'
    'traffic_path.c'
    'ai_path.c'
    'traffic_drive.c'
    'traffic_scene.c'
    'traffic_frame.c'
    'traffic_spawn.c'
    'traffic_actor.c'
    'traffic_actor_impact.c'
    'traffic_motion.c'
    'traffic_follow.c'
    'traffic_collision.c'
    'traffic_effects.c'
    'traffic_presentation.c'
    'traffic_view.c'
    'traffic_heading.c'
    'world_frame.c'
    'world_render.c'
    'world_render_visit.c'
    'world_polygon_stamp.c'
    'world_material.c'
    'world_clip.c'
    'render_queue.c'
    'passenger_render.c'
    'taxi_shadow_render.c'
    'taxi_shadow.c'
    'render_interpolation.c'
    'audio_host.c'
    'audio_channel.c'
    'game_audio_engine.c'
    'game_audio_frame_bridge.c'
    'game_audio_reset_state.c'
    'game_audio_cleanup.c'
    'normal_exit.c'
    'normal_retry_owned.c'
    'credits_flow.c'
    'credits_runtime.c'
    'credits_asset.c'
    'game_audio_controller.c'
    'game_audio_events.c'
    'game_audio_aux.c'
    'game_audio_vehicle_loop.c'
    'game_audio_reward_event.c'
    'game_audio_turn_loop.c'
    'game_audio_context_voice.c'
    'game_audio_levels.c'
    'game_audio_voice.c'
    'audio_asset.c'
    'audio_mix.c'
    'music_asset.c'
    'music_session.c'
    'music_state.c'
    'music_note.c'
    'music_event.c'
    'music_effects.c'
    'music_mix.c'
    'music_tracker.c'
    'input.c'
    'actions.c'
    'raster_dispatch.c'
    'view_transform.c'
    'render_viewport.c'
    'render_frame_init.c'
    'projection_divide.c'
    'scene_registration.c'
    'scene_registration_render.c'
    'restart_render_context.c'
    'graphics_storage.c'
    'graphics_bootstrap_tail.c'
    'graphics_bootstrap_prefix.c'
    'graphics_bootstrap_owned.c'
    'palette_registration.c'
    'session_init.c'
    'session_init_fare.c'
    'destination_panel.c'
    'world_background.c'
    'world_indexed.c'
    'indexed_asset.c'
    'indexed_pixel.c'
    'distance_bitmap.c'
    'indexed_preview_scale.c'
    'destination_preview_border.c'
    'material_blink.c'
    'shade_table.c'
    'material_shader.c'
    'axis_shader.c'
    'obj_shader.c'
    'obj_order.c'
    'title_prompt.c'
    'title_scroll.c'
    'title_flow.c'
    'title_runtime.c'
    'title_callback.c'
    'attract_session.c'
    'attract_flow.c'
    'ai_controller.c'
    'ai_command.c'
    'challenge_assets.c'
    'challenge_flag_course.c'
    'challenge_flag_hud.c'
    'challenge_flag_hud_pixels.c'
    'challenge_ball_hud.c'
    'challenge_drift_hud.c'
    'taxi_oam_layer.c'
    'world_cpu_scene.c'
    'scene_viewer.c','timing_tape.c','timing_tape_format.c','timing_compact.c','timing_compact_file.c'
    'vehicle_start_gameplay.c'
    'vehicle_start_scene.c'
    'vehicle_start_packed.c'
    'vehicle_start_world.c'
    'vehicle_start_driver.c'
    'vehicle_template_load.c'
    'vehicle_reset_packed.c'
    'vehicle_start_finalize.c'
    'vehicle_start_random.c'
    'challenge_flag.c'
    'challenge_flag_projection.c'
    'challenge_ball_projection.c'
    'challenge_ball_course.c'
    'challenge_drift_course.c'
    'challenge_drift.c'
    'challenge_ball.c'
    'integer_root.c'
    'challenge_ball_palette.c'
    'challenge_flag_indexed.c'
    'control_layout.c'
    'audio_settings_state.c'
    'options_menu_flow.c'
    'options_menu_control.c'
    'options_menu_route.c'
    'options_settings.c'
    'options_settings_render.c'
    'audio_menu_flow.c'
    'audio_menu_control.c'
    'audio_menu_navigation.c'
    'sound_options.c'
    'audio_volume_render.c'
    'audio_test_render.c'
    'loading_image.c'
    'loading_flow.c'
    'xbox.c'
    'timing.c'
    'taxi_sprite.c'
    'normal_menu_flow.c'
    'normal_menu_route.c'
    'body_points.c'
    'actor_effects.c'
    'effect_emit.c'
    'effect_pool.c'
    'effect_update.c'
    'effect_sprite.c'
    'effect_asset.c'
    'challenge_completion.c'
    'challenge_result_strings.c'
    'challenge_profile_commit.c'
    'challenge_turn_course.c'
    'challenge_bound.c'
    'challenge_bound_course.c'
    'challenge_turn.c'
    'engine_rotate_owned.c'
    'render_frame_owned.c'
    'pair_row_raster.c'
    'pair_column_setup.c'
    'pair_column_raster.c'
    'flat_row_raster.c'
    'obj_tiles_rgba.c'
    'frame_time.c'
    'challenge_exit.c'
    'reward_update.c'
    'reward_session.c'
    'material_phase.c'
    'start_art_owned.c'
    'sprite_copy_flush_owned.c'
    'reference_driving.c'
    'render_pose_history.c'
    'host_time.c'
    'frame_wait.c'
    'frame_event.c'
    'taxi_graphics_package.c'
    'title_load.c'
    'title_entry.c'
    'palette_fade_setup.c'
    'city_palette_owned.c'
    'start_art_submission.c'
    'audio_frame.c'
    'frame_audio_owned.c'
    'effect_presentation_slots.c'
    'effect_presentation_interpolation.c'
    'ordinary_graphics_owned.c'
    'bound_graphics_owned.c'
    'hud_start_owned.c'
    'title_requests.c'
    'title_publish.c'
    'title_pixels.c'
    'tip_hud.c'
    'taxi_half_plan.c'
    'taxi_render_owned.c'
    'effect_render_owned.c'
    'taxi_tiles_rgba.c'
    'hud_cash_pixels.c'
    'engine_matrix_call.c'
    'engine_callback_identity.c'
    'engine_projection_call.c'
    'reward.c'
    'start_art_replace.c'
    'sprite_replacement_owned.c'
    'sprite_copy_flush.c'
    'startup_title_viewer.c'
    'driving_shared_snapshot.c'
    'input_snapshot.c'
    'render_history.c'
    'title_background.c'
    'title_prepare.c'
    'palette_fade_registered.c'
    'startup_obj_setup.c'
    'palette_bank.c'
    'music_buffer.c'
    'audio_effects_buffer.c'
    'audio_output.c'
    'title_assets.c'
    'font_setup.c'
    'startup_obj_transport.c'
    'hud_start.c'
    'title_obj_queue.c'
    'taxi_frame_cache_owned.c'
    'taxi_placement_requests.c'
    'taxi_affine_queue.c'
    'taxi_frame_palette_owned.c'
    'hud_digit_copies_owned.c'
    'sprite_replacement.c'
    'startup_screens.c'
    'startup_intro_fade.c'
    'startup_intro_pixels.c'
    'palette_registration_small.c'
    'title_font_setup.c'
    'title_prompt_setup.c'
    'startup_obj_queue.c'
    'startup_obj.c'
    'taxi_frame_copy_owned.c'
    'hud_digit_copies.c'
    'startup_bitmap.c'
    'screen_sequence.c'
    'presentation_service.c'
    'screen_flow.c'
    'startup_intro.c'
    'display_mode.c'
    'bitmap_fill.c'
    'startup_animation.c'
    'ordinary_frame_owned.c'
    'taxi_render_tail_owned.c'
    'host_presentation.c'
    'ordinary_presentation_owned.c'
    'host_ordinary_presentation.c'
    'pause_presentation_owned.c'
)
$sceneViewerArguments = @('-std=c11','-Wall','-Wextra','-Werror','-O2','-I',(Join-Path $PSScriptRoot 'src')) + @($sceneViewerSourceNames | ForEach-Object { Join-Path $PSScriptRoot "src/$_" }) + @('-lopengl32','-lgdi32','-luser32','-lwinmm','-lm','-o',(Join-Path $bin 'scene-viewer.exe'))
$sceneViewerResponse = Join-Path $env:TEMP ('crazy-taxi-scene-' + [guid]::NewGuid().ToString('N') + '.rsp')
$sceneViewerCompileExit = 1
try {
    [System.IO.File]::WriteAllLines($sceneViewerResponse,($sceneViewerArguments | ForEach-Object { '"' + $_.Replace('"','\"') + '"' }),[System.Text.Encoding]::ASCII)
    & (Join-Path $PSScriptRoot 'tools/rebuild_viewer.ps1')
    $sceneViewerCompileExit = $LASTEXITCODE
} finally {
    Remove-Item -LiteralPath $sceneViewerResponse -ErrorAction SilentlyContinue
}
if ($sceneViewerCompileExit -ne 0) { throw 'Scene viewer compilation failed' }
if ($LASTEXITCODE -ne 0) { throw 'Scene viewer compilation failed' }



& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'tests/powertrain_rom_check.c') -o (Join-Path $bin 'powertrain-rom-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Powertrain probe compilation failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/driver_rom_check.c') -o (Join-Path $bin 'driver-rom-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Driver probe compilation failed' }
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'tests/steering_input_test.c') -o (Join-Path $bin 'steering-input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Steering input compilation failed' }
& (Join-Path $bin 'steering-input-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Steering input tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/timing.c') (Join-Path $PSScriptRoot 'tests/timing_test.c') -o (Join-Path $bin 'timing-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Timing compilation failed' }
& (Join-Path $bin 'timing-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Timing tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/body_pose_test.c') -o (Join-Path $bin 'body-pose-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Body pose compilation failed' }
& (Join-Path $bin 'body-pose-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Body pose tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'tests/height_update_test.c') -o (Join-Path $bin 'height-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Height update compilation failed' }
& (Join-Path $bin 'height-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Height update tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'tests/challenge_completion_test.c') -o (Join-Path $bin 'challenge-completion-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge completion compilation failed' }
& (Join-Path $bin 'challenge-completion-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge completion tests failed' }


& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'tests/world_update_test.c') -o (Join-Path $bin 'world-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'World update compilation failed' }
& (Join-Path $bin 'world-update-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'World update tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'tests/free_rev_test.c') -o (Join-Path $bin 'free-rev-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Free rev compilation failed' }
& (Join-Path $bin 'free-rev-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Free rev tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/taxi_sprite.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/taxi_sprite_test.c') -o (Join-Path $bin 'taxi-sprite-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Taxi sprite compilation failed' }
& (Join-Path $bin 'taxi-sprite-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Taxi sprite tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/world_asset_check.c') -o (Join-Path $bin 'world-asset-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'World asset check compilation failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/taxi_sprite.c') (Join-Path $PSScriptRoot 'tests/native_runtime_check.c') -o (Join-Path $bin 'native-runtime-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Native runtime compilation failed' }
& (Join-Path $bin 'native-runtime-check.exe') (Join-Path $PSScriptRoot 'assets')
if ($LASTEXITCODE -ne 0) { throw 'Native runtime checks failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/camera_angle.c') (Join-Path $PSScriptRoot 'src/engine_rotate_owned.c') (Join-Path $PSScriptRoot 'src/engine_matrix_call.c') (Join-Path $PSScriptRoot 'src/engine_callback_identity.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/camera_angle_test.c') -o (Join-Path $bin 'camera-angle-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Camera angle compilation failed' }
& (Join-Path $bin 'camera-angle-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Camera angle tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/loading_image.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/loading_image_test.c') -o (Join-Path $bin 'loading-image-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading image compilation failed' }
& (Join-Path $bin 'loading-image-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading image tests failed' }




& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/loading_flow.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/loading_flow_test.c') -o (Join-Path $bin 'loading-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading flow compilation failed' }
& (Join-Path $bin 'loading-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading flow tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/loading_flow.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/loading_passenger_work_test.c') -o (Join-Path $bin 'loading-passenger-work-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading passenger work compilation failed' }
& (Join-Path $bin 'loading-passenger-work-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Loading passenger work tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/normal_menu_route_test.c') -o (Join-Path $bin 'normal-menu-route-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu routing compilation failed' }
& (Join-Path $bin 'normal-menu-route-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu routing tests failed' }

# Original challenge briefing text coverage (all27 rule/advice pages).
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/normal_menu_text.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/text_layout.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/menu_footer_layout.c') (Join-Path $PSScriptRoot 'src/normal_menu_background.c') (Join-Path $PSScriptRoot 'tests/challenge_briefing_text_test.c') -o (Join-Path $bin 'challenge-briefing-text-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge briefing text compilation failed' }
& (Join-Path $bin 'challenge-briefing-text-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge briefing text tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/normal_menu_background.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/normal_menu_text.c') (Join-Path $PSScriptRoot 'src/menu_footer_layout.c') (Join-Path $PSScriptRoot 'src/text_layout.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/normal_menu_background_test.c') (Join-Path $PSScriptRoot 'src/options_settings_render.c') (Join-Path $PSScriptRoot 'src/audio_volume_render.c') (Join-Path $PSScriptRoot 'src/audio_test_render.c') -o (Join-Path $bin 'normal-menu-background-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu background compilation failed' }
& (Join-Path $bin 'normal-menu-background-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu background tests failed' }




& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/palette_scale_test.c') -o (Join-Path $bin 'palette-scale-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette scaling compilation failed' }
& (Join-Path $bin 'palette-scale-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette scaling tests failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow_test.c') -o (Join-Path $bin 'normal-menu-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu flow compilation failed' }
& (Join-Path $bin 'normal-menu-flow-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Normal menu flow checks failed' }

















# Original Crazy Box spawn markers against native extracted level collision.
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -ffunction-sections '-Wl,--gc-sections' (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'tests/challenge_spawn_check.c') -o (Join-Path $bin 'challenge-spawn-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge spawn check build failed' }
& (Join-Path $bin 'challenge-spawn-check.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge spawn ground check failed' }

# Original challenge Jump result formatting and packed profile records.
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_result_strings_test.c') -o (Join-Path $bin 'challenge-result-strings-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge result strings compile failed.' }
& (Join-Path $bin 'challenge-result-strings-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge result strings verification failed.' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -ffunction-sections '-Wl,--gc-sections' (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'tests/challenge_assets_test.c') -o (Join-Path $bin 'challenge-assets-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge assets compile failed.' }
& (Join-Path $bin 'challenge-assets-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge assets verification failed.' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_course_test.c') -o (Join-Path $bin 'challenge-flag-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag course build failed' }
& (Join-Path $bin 'challenge-flag-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag course test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_actor_test.c') -o (Join-Path $bin 'challenge-flag-actor-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag actor build failed' }
& (Join-Path $bin 'challenge-flag-actor-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag actor test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_ground_test.c') -o (Join-Path $bin 'challenge-flag-ground-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag ground build failed' }
& (Join-Path $bin 'challenge-flag-ground-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag ground test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_profile_test.c') -o (Join-Path $bin 'challenge-flag-profile-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Flag profile build failed' }
& (Join-Path $bin 'challenge-flag-profile-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Flag profile test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_score_test.c') -o (Join-Path $bin 'challenge-flag-score-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag score build failed' }
& (Join-Path $bin 'challenge-flag-score-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag score test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_test.c') -o (Join-Path $bin 'challenge-flag-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag callback build failed' }
& (Join-Path $bin 'challenge-flag-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag callback test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_projection.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_projection_test.c') -o (Join-Path $bin 'challenge-flag-projection-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag projection build failed' }
& (Join-Path $bin 'challenge-flag-projection-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag projection test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_projection.c') (Join-Path $PSScriptRoot 'src/challenge_flag_indexed.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'src/engine_callback_identity.c') (Join-Path $PSScriptRoot 'tests/challenge_flag_indexed_test.c') -o (Join-Path $bin 'challenge-flag-indexed-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag indexed build failed' }
& (Join-Path $bin 'challenge-flag-indexed-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Flag indexed test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_jump_recalculate_test.c') -o (Join-Path $bin 'challenge-jump-recalculate-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Jump record recalculation build failed' }
& (Join-Path $bin 'challenge-jump-recalculate-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Jump record recalculation test failed' }

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_profile_commit_test.c') -o (Join-Path $bin 'challenge-profile-commit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Jump registration commit build failed' }
& (Join-Path $bin 'challenge-profile-commit-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Jump registration commit test failed' }


# Source-derived Crazy Ball initialization/motion gate; challenge integration pending.
& $Compiler cc -std=c11 -DNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_test.c') -o (Join-Path $bin 'challenge-ball-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Ball motion compile failed' }
& (Join-Path $bin 'challenge-ball-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Crazy Ball motion test failed' }

# Original Ball resource/actor/car-start gate.
& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/challenge_ball_course.c') (Join-Path $PSScriptRoot 'src/challenge_drift_course.c') (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/traffic_presentation.c') (Join-Path $PSScriptRoot 'src/traffic_heading.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/render_interpolation.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/integer_root.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_course_test.c') -o (Join-Path $bin 'challenge-ball-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball course compilation failed' }
& (Join-Path $bin 'challenge-ball-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball course verification failed' }

# Source unrotated Ball indexed projection gate; live GPU integration pending.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_projection_test.c') -o (Join-Path $bin 'challenge-ball-projection-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball projection compile failed' }
& (Join-Path $bin 'challenge-ball-projection-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball projection checks failed' }

# Original Ball HUD request and counter pixels; full live composition pending.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_ball_hud.c') (Join-Path $PSScriptRoot 'src/challenge_drift_hud.c') (Join-Path $PSScriptRoot 'src/text_layout.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_hud_test.c') -o (Join-Path $bin 'challenge-ball-hud-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball HUD compilation failed' }
& (Join-Path $bin 'challenge-ball-hud-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball HUD verification failed' }

# Original E154 source choice only; retained flag/DMA/fade ownership pending.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/palette_registration.c') (Join-Path $PSScriptRoot 'src/shade_table.c') (Join-Path $PSScriptRoot 'tests/palette_registration_test.c') -o (Join-Path $bin 'palette-registration-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette selection compilation failed' }
& (Join-Path $bin 'palette-registration-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Palette selection verification failed' }

# Source-qualified registered Ball scene palette cap conversion; live GPU pending.
& $Compiler cc -std=c11 -O2 -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/challenge_ball_palette.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_palette_test.c') -o (Join-Path $bin 'challenge-ball-palette-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball registered palette compilation failed' }
& (Join-Path $bin 'challenge-ball-palette-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Ball registered palette verification failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'tests/challenge_ball_results_test.c') -o (Join-Path $bin 'challenge-ball-results-test.exe')
if($LASTEXITCODE -ne 0){throw 'Ball results compile failed'}
& (Join-Path $bin 'challenge-ball-results-test.exe')
if($LASTEXITCODE -ne 0){throw 'Ball results test failed'}

# Original post-field sprite origin arithmetic; live mode owner remains separate.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/obj_screen_origin_test.c') -o (Join-Path $bin 'obj-screen-origin-test.exe')
if($LASTEXITCODE -ne 0){throw 'OBJ origin compile failed'}
& (Join-Path $bin 'obj-screen-origin-test.exe')
if($LASTEXITCODE -ne 0){throw 'OBJ origin test failed'}

# Original Crazy Drift controller and recovered orientation counter fixture.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'tests/challenge_drift_test.c') -o (Join-Path $bin 'challenge-drift-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift controller compile failed'}
& (Join-Path $bin 'challenge-drift-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift controller test failed'}

# Original Drift course/native controller integration.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/challenge_ball_course.c') (Join-Path $PSScriptRoot 'src/challenge_drift_course.c') (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/traffic_presentation.c') (Join-Path $PSScriptRoot 'src/traffic_heading.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/render_interpolation.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/integer_root.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/challenge_drift_course_test.c') -o (Join-Path $bin 'challenge-drift-course-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift course compile failed'}
& (Join-Path $bin 'challenge-drift-course-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift course test failed'}

# Original Drift HUD layout and font composition.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_ball_hud.c') (Join-Path $PSScriptRoot 'src/challenge_drift_hud.c') (Join-Path $PSScriptRoot 'src/text_layout.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'tests/challenge_drift_hud_test.c') -o (Join-Path $bin 'challenge-drift-hud-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift HUD compile failed'}
& (Join-Path $bin 'challenge-drift-hud-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift HUD test failed'}

# Original Drift combo score records and snapshot recalculation.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'tests/challenge_drift_score_test.c') -o (Join-Path $bin 'challenge-drift-score-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift score compile failed'}
& (Join-Path $bin 'challenge-drift-score-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift score test failed'}

# Original Drift results and profile registration.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'tests/challenge_drift_results_test.c') -o (Join-Path $bin 'challenge-drift-results-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift results compile failed'}
& (Join-Path $bin 'challenge-drift-results-test.exe')
if($LASTEXITCODE -ne 0){throw 'Drift results test failed'}

# Xbox pending original drift transition adaptation (synthetic inputs).
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/xbox.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'tests/xbox_drift_entry_test.c') -o (Join-Path $bin 'xbox-drift-entry-test.exe')
if($LASTEXITCODE -ne 0){throw 'Xbox drift-entry compile failed'}
& (Join-Path $bin 'xbox-drift-entry-test.exe')
if($LASTEXITCODE -ne 0){throw 'Xbox drift-entry test failed'}

# Original Crazy Turn scalar controller; world/success dependencies remain external.
& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/challenge_turn.c') (Join-Path $PSScriptRoot 'tests/challenge_turn_test.c') -o (Join-Path $bin 'challenge-turn-test.exe')
if($LASTEXITCODE -ne 0){throw 'Turn scalar compile failed'}
& (Join-Path $bin 'challenge-turn-test.exe')
if($LASTEXITCODE -ne 0){throw 'Turn scalar test failed'}

& $Compiler cc -std=c11 -Wall -Wextra -Werror -O2 (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'tests/challenge_delivery_test.c') -o (Join-Path $bin 'challenge-delivery-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge delivery build failed' }
& (Join-Path $bin 'challenge-delivery-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Challenge delivery tests failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'src/challenge_ball_course.c') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/challenge_drift_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_bound.c') (Join-Path $PSScriptRoot 'src/challenge_bound_course.c') (Join-Path $PSScriptRoot 'src/challenge_turn.c') (Join-Path $PSScriptRoot 'src/challenge_turn_course.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'src/fare_session.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/integer_root.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_boarding.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_exit.c') (Join-Path $PSScriptRoot 'src/passenger_idle.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/passenger_scene.c') (Join-Path $PSScriptRoot 'src/rank_display.c') (Join-Path $PSScriptRoot 'src/rank_display_owned.c') (Join-Path $PSScriptRoot 'src/settlement_display.c') (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'src/render_interpolation.c') (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/traffic_heading.c') (Join-Path $PSScriptRoot 'src/traffic_presentation.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'tests/challenge_turn_course_test.c') -o (Join-Path $bin 'challenge-turn-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn course build failed' }
& (Join-Path $bin 'challenge-turn-course-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn course tests failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'tests/trip_end_turn_test.c') -o (Join-Path $bin 'trip-end-turn-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn trip end build failed' }
& (Join-Path $bin 'trip-end-turn-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn trip end tests failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'src/challenge_ball_course.c') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/challenge_drift_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_bound.c') (Join-Path $PSScriptRoot 'src/challenge_bound_course.c') (Join-Path $PSScriptRoot 'src/challenge_turn.c') (Join-Path $PSScriptRoot 'src/challenge_turn_course.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'src/fare_session.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/integer_root.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_boarding.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_exit.c') (Join-Path $PSScriptRoot 'src/passenger_idle.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/passenger_scene.c') (Join-Path $PSScriptRoot 'src/rank_display.c') (Join-Path $PSScriptRoot 'src/rank_display_owned.c') (Join-Path $PSScriptRoot 'src/settlement_display.c') (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'src/render_interpolation.c') (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/traffic_heading.c') (Join-Path $PSScriptRoot 'src/traffic_presentation.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'tests/challenge_turn_trip_session_test.c') -o (Join-Path $bin 'challenge-turn-trip-session-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn trip session build failed' }
& (Join-Path $bin 'challenge-turn-trip-session-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn trip session tests failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror -I (Join-Path $PSScriptRoot 'src') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'tests/challenge_turn_results_test.c') -o (Join-Path $bin 'challenge-turn-results-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn results build failed' }
& (Join-Path $bin 'challenge-turn-results-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn results tests failed' }

& $Compiler cc -std=c11 -O2 -DNDEBUG -Wall -Wextra -Werror (Join-Path $PSScriptRoot 'src/acceleration.c') (Join-Path $PSScriptRoot 'src/body_pose.c') (Join-Path $PSScriptRoot 'src/braking.c') (Join-Path $PSScriptRoot 'src/challenge_assets.c') (Join-Path $PSScriptRoot 'src/challenge_ball.c') (Join-Path $PSScriptRoot 'src/challenge_ball_course.c') (Join-Path $PSScriptRoot 'src/challenge_ball_projection.c') (Join-Path $PSScriptRoot 'src/challenge_completion.c') (Join-Path $PSScriptRoot 'src/challenge_drift.c') (Join-Path $PSScriptRoot 'src/challenge_drift_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag.c') (Join-Path $PSScriptRoot 'src/challenge_flag_course.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud.c') (Join-Path $PSScriptRoot 'src/challenge_flag_hud_pixels.c') (Join-Path $PSScriptRoot 'src/challenge_profile_commit.c') (Join-Path $PSScriptRoot 'src/challenge_result_strings.c') (Join-Path $PSScriptRoot 'src/challenge_bound.c') (Join-Path $PSScriptRoot 'src/challenge_bound_course.c') (Join-Path $PSScriptRoot 'src/challenge_turn.c') (Join-Path $PSScriptRoot 'src/challenge_turn_course.c') (Join-Path $PSScriptRoot 'src/destination.c') (Join-Path $PSScriptRoot 'src/drive_world.c') (Join-Path $PSScriptRoot 'src/driver.c') (Join-Path $PSScriptRoot 'src/approach.c') (Join-Path $PSScriptRoot 'src/driving.c') (Join-Path $PSScriptRoot 'src/engine_curve.c') (Join-Path $PSScriptRoot 'src/fare.c') (Join-Path $PSScriptRoot 'src/fare_session.c') (Join-Path $PSScriptRoot 'src/font_asset.c') (Join-Path $PSScriptRoot 'src/free_rev.c') (Join-Path $PSScriptRoot 'src/game_settings.c') (Join-Path $PSScriptRoot 'src/game_start.c') (Join-Path $PSScriptRoot 'src/gears.c') (Join-Path $PSScriptRoot 'src/ground.c') (Join-Path $PSScriptRoot 'src/height_update.c') (Join-Path $PSScriptRoot 'src/impact.c') (Join-Path $PSScriptRoot 'src/indexed_asset.c') (Join-Path $PSScriptRoot 'src/indexed_pixel.c') (Join-Path $PSScriptRoot 'src/projection_divide.c') (Join-Path $PSScriptRoot 'src/pair_row_raster.c') (Join-Path $PSScriptRoot 'src/pair_column_setup.c') (Join-Path $PSScriptRoot 'src/pair_column_raster.c') (Join-Path $PSScriptRoot 'src/flat_row_raster.c') (Join-Path $PSScriptRoot 'src/integer_root.c') (Join-Path $PSScriptRoot 'src/menu_input.c') (Join-Path $PSScriptRoot 'src/motion.c') (Join-Path $PSScriptRoot 'src/movement.c') (Join-Path $PSScriptRoot 'src/normal_menu_flow.c') (Join-Path $PSScriptRoot 'src/normal_menu_route.c') (Join-Path $PSScriptRoot 'src/obj_order.c') (Join-Path $PSScriptRoot 'src/orientation.c') (Join-Path $PSScriptRoot 'src/palette_fade.c') (Join-Path $PSScriptRoot 'src/passenger_asset.c') (Join-Path $PSScriptRoot 'src/passenger_boarding.c') (Join-Path $PSScriptRoot 'src/passenger_delivery.c') (Join-Path $PSScriptRoot 'src/passenger_exit.c') (Join-Path $PSScriptRoot 'src/passenger_idle.c') (Join-Path $PSScriptRoot 'src/passenger_init.c') (Join-Path $PSScriptRoot 'src/passenger_pickup.c') (Join-Path $PSScriptRoot 'src/passenger_route.c') (Join-Path $PSScriptRoot 'src/passenger_scene.c') (Join-Path $PSScriptRoot 'src/rank_display.c') (Join-Path $PSScriptRoot 'src/rank_display_owned.c') (Join-Path $PSScriptRoot 'src/settlement_display.c') (Join-Path $PSScriptRoot 'src/passenger_search.c') (Join-Path $PSScriptRoot 'src/random.c') (Join-Path $PSScriptRoot 'src/render_interpolation.c') (Join-Path $PSScriptRoot 'src/results_grade.c') (Join-Path $PSScriptRoot 'src/results_state.c') (Join-Path $PSScriptRoot 'src/runtime_asset.c') (Join-Path $PSScriptRoot 'src/score_insert.c') (Join-Path $PSScriptRoot 'src/simulation_start.c') (Join-Path $PSScriptRoot 'src/spawn_placement.c') (Join-Path $PSScriptRoot 'src/startup_world.c') (Join-Path $PSScriptRoot 'src/steering_input.c') (Join-Path $PSScriptRoot 'src/traction.c') (Join-Path $PSScriptRoot 'src/traffic_heading.c') (Join-Path $PSScriptRoot 'src/traffic_presentation.c') (Join-Path $PSScriptRoot 'src/traffic_sprite.c') (Join-Path $PSScriptRoot 'src/traffic_view.c') (Join-Path $PSScriptRoot 'src/trip_clock.c') (Join-Path $PSScriptRoot 'src/trip_end.c') (Join-Path $PSScriptRoot 'src/trip_start.c') (Join-Path $PSScriptRoot 'src/vehicle_config.c') (Join-Path $PSScriptRoot 'src/vehicle_math.c') (Join-Path $PSScriptRoot 'src/vehicle_reset.c') (Join-Path $PSScriptRoot 'src/vehicle_update.c') (Join-Path $PSScriptRoot 'src/world_asset.c') (Join-Path $PSScriptRoot 'src/challenge_delivery.c') (Join-Path $PSScriptRoot 'src/world_update.c') (Join-Path $PSScriptRoot 'tests/challenge_turn_frame_test.c') -o (Join-Path $bin 'challenge-turn-frame-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn frame build failed' }
& (Join-Path $bin 'challenge-turn-frame-test.exe')
if ($LASTEXITCODE -ne 0) { throw 'Turn frame tests failed' }

