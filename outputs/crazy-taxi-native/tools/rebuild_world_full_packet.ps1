$ErrorActionPreference='Stop'
$taskScript=(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'rebuild_viewer.ps1') -Raw).Replace("'src/scene_viewer.c'","'tests/world-full-packet-rom-compare.c'").Replace("'scene-viewer.exe'","'world-full-packet-rom-compare.exe'")
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskScript=$taskScript.Replace('$projectRoot = Split-Path -Parent $PSScriptRoot', ('$projectRoot = '+[char]39+$taskRoot+[char]39))
Invoke-Expression $taskScript
