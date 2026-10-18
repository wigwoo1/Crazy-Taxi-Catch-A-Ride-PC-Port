$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$vswhere = './'
if (-not (Test-Path -LiteralPath $vswhere)) { throw 'Visual Studio discovery tool not found.' }
$install = (& $vswhere -latest -products '*' -property installationPath | Select-Object -First 1)
if (-not $install) { throw 'Visual Studio C++ Build Tools installation not found.' }
$devcmd = Join-Path $install 'Common7\Tools\VsDevCmd.bat'
if (-not (Test-Path -LiteralPath $devcmd)) { throw 'Visual Studio developer environment script not found.' }
$bin = Join-Path $project 'bin'
New-Item -ItemType Directory -Force -Path $bin | Out-Null
$src = Join-Path $project 'src'
$test = Join-Path $project 'tests\traffic_follow_source_powertrain_probe.c'
$output = Join-Path $bin 'traffic-follow-source-powertrain-probe.exe'
$frameTest = Join-Path $project 'tests\traffic_follow_source_frame_probe.c'
$frameOutput = Join-Path $bin 'traffic-follow-source-frame-probe.exe'
$sources = @('traffic_actor.c','traffic_follow.c','traffic_collision.c','traffic_effects.c','traffic_path.c','gears.c','random.c','impact.c') | ForEach-Object { '"' + (Join-Path $src $_) + '"' }
$commandFile = Join-Path $env:TEMP 'crazy-taxi-counter5294-powertrain-build.cmd'
$lines = @(
    '@echo off',
    ('call "' + $devcmd + '" -arch=x64 -host_arch=x64 >nul'),
    'if errorlevel 1 exit /b %errorlevel%',
    ('cd /d "' + $bin + '"'),
    ('cl /nologo /std:c11 /W4 /WX /O2 /D_CRT_SECURE_NO_WARNINGS /I"' + $src + '" "' + $test + '" ' + ($sources -join ' ') + ' /Fe:"' + $output + '"'),
    'if errorlevel 1 exit /b %errorlevel%',
    ('cl /nologo /std:c11 /W4 /WX /O2 /D_CRT_SECURE_NO_WARNINGS /I"' + $src + '" "' + $frameTest + '" ' + ($sources -join ' ') + ' /Fe:"' + $frameOutput + '"'),
    'exit /b %errorlevel%'
)
Set-Content -LiteralPath $commandFile -Value $lines -Encoding ascii
& $env:ComSpec /d /c $commandFile
if ($LASTEXITCODE -ne 0) { throw 'Counter5294 source powertrain probe build failed.' }
Write-Output 'PASS strict MSVC counter5294 source powertrain probe build'
