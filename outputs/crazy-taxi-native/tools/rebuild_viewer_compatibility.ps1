param([string]$Compiler='', [string]$Output='scene-viewer-compatibility.exe')
$ErrorActionPreference='Stop'
$base=Split-Path -Parent $PSScriptRoot
$project=Split-Path -Parent (Split-Path -Parent $base)
if(!$Compiler){$Compiler=Join-Path $project 'work/compiler/llvm-mingw-20261006/llvm-mingw-20261006-ucrt-x86_64/bin/x86_64-w64-mingw32-clang.exe'}
$env:PYTHONHOME=Join-Path $project 'work/python-3.13.7-embed-amd64';$env:PYTHONPATH='';$env:PYTHONDONTWRITEBYTECODE='1'
$env:TEMP=Join-Path $base 'work/native-viewer-baseline-objects';$env:TMP=$env:TEMP
[IO.Directory]::CreateDirectory($env:TEMP)|Out-Null
& (Join-Path $env:PYTHONHOME 'python.exe') -B (Join-Path $PSScriptRoot 'build_native_viewer.py') --compiler $Compiler --output $Output
if($LASTEXITCODE -ne 0){throw 'Canonical baseline viewer build failed'}