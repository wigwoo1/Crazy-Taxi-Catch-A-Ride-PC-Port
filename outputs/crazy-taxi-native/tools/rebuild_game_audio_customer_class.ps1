$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_context_voice.c') (Join-Path $root 'tests/game_audio_customer_class_test.c') -o (Join-Path $root 'bin/game-audio-customer-class-test.exe')
if($LASTEXITCODE -ne 0){throw 'Customer class build failed'}
& (Join-Path $root 'bin/game-audio-customer-class-test.exe')
if($LASTEXITCODE -ne 0){throw 'Customer class test failed'}

& (Join-Path $root '../../work/compiler/ziglang/zig.exe') cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/game_audio_context_voice.c') (Join-Path $root 'tests/game_audio_customer_class_rom_compare.c') -o (Join-Path $root 'bin/game-audio-customer-class-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Customer class ROM harness build failed'}
