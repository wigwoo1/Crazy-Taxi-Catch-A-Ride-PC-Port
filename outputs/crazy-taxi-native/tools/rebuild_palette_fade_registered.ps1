$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$sources=@('palette_fade_registered','palette_fade_setup','palette_fade') | ForEach-Object {Join-Path $root "src/$_.c"}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/palette_fade_registered_rom_compare.c') -o (Join-Path $root 'bin/palette-fade-registered-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Registered fade build failed'}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/palette_fade_registered_frame_rom_compare.c') -o (Join-Path $root 'bin/palette-fade-registered-frame-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Registered frame fade build failed'}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') $sources (Join-Path $root 'tests/palette_fade_service_rom_compare.c') -o (Join-Path $root 'bin/palette-fade-service-rom-compare.exe')
if($LASTEXITCODE -ne 0){throw 'Normal fade service build failed'}
