$ErrorActionPreference='Stop'
$taskScript=(Get-Content -LiteralPath (Join-Path $PSScriptRoot 'rebuild_viewer.ps1') -Raw).Replace("'src/scene_viewer.c'","'tests/world-panel-cache-sequence.c'").Replace("'scene-viewer.exe'","'world-panel-cache-sequence.exe'")
$taskRoot=Split-Path -Parent $PSScriptRoot
$taskScript=$taskScript.Replace('$projectRoot = Split-Path -Parent $PSScriptRoot', ('$projectRoot = '+[char]39+$taskRoot+[char]39))
$taskScript=$taskScript.Replace('-lopengl32', '(Join-Path $projectRoot ''src/graphics_generation.c'') -lopengl32')
Invoke-Expression $taskScript
