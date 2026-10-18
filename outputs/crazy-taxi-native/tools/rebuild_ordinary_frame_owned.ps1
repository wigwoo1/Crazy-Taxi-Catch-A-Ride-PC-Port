$ErrorActionPreference='Stop'
$base=Split-Path -Parent $PSScriptRoot
$project=Split-Path -Parent (Split-Path -Parent $base)
$case=Join-Path $base 'work/native-ordinary-frame-20261015'
$env:PYTHONHOME=Join-Path $project 'work/python-3.13.7-embed-amd64'
$env:PYTHONPATH='';$env:PYTHONDONTWRITEBYTECODE='1';$env:TEMP=$case;$env:TMP=$case
Push-Location -LiteralPath $project
try{
 & (Join-Path $env:PYTHONHOME 'python.exe') -B (Join-Path $case 'build-and-check.py')
 if($LASTEXITCODE -ne 0){throw 'Native ordinary frame preparation/publication verification failed'}
}finally{Pop-Location}