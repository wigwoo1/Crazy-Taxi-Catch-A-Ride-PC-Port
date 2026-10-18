$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$compiler=Join-Path $root '../../work/compiler/ziglang/zig.exe'
$files=@('src/save_file_device.c','src/save_load.c','src/save_block.c','tests/save-file-device-check.c') | ForEach-Object {Join-Path $root $_}
& $compiler cc -std=c11 -UNDEBUG -Wall -Wextra -Werror -O2 -I (Join-Path $root 'src') @files -o (Join-Path $root 'bin/save-file-device-check.exe')
if($LASTEXITCODE -ne 0){throw 'Read-only save device build failed'}
