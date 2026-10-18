$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
& $compiler cc -std=c11 -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') (Join-Path $root 'src/reward_session.c') (Join-Path $root 'src/reward.c') (Join-Path $root 'src/reward_update.c') (Join-Path $root 'src/random.c') (Join-Path $root 'src/fare.c') (Join-Path $root 'tests/reward_reset_rom_compare.c') -o (Join-Path $root 'bin/reward-reset-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Reward reset build failed'}


