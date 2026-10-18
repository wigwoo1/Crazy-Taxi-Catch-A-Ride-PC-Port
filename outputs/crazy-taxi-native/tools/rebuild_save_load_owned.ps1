$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$project=Split-Path -Parent (Split-Path -Parent $root)
$names=@('music_asset','music_state','music_tracker','music_effects','music_mix','music_event','music_note','audio_mix','random','music_buffer','audio_effects_buffer','audio_channel','audio_asset','audio_output','audio_frame','audio_reset','frame_wait','frame_event','frame_audio_owned','audio_bootstrap','audio_configuration','music_playback','save_block','save_load','save_file_device','save_load_owned','startup_init','startup_init_owned','startup_save_owned','input','save_audio_host','audio_host')
$env:PYTHONHOME=Join-Path $project 'work\python-3.13.7-embed-amd64'
$env:PYTHONPATH='';$env:PYTHONDONTWRITEBYTECODE='1'
$env:TEMP=Join-Path $root 'work\native-save-frame-owner-20261015';$env:TMP=$env:TEMP
[IO.Directory]::CreateDirectory($env:TEMP)|Out-Null
& (Join-Path $env:PYTHONHOME 'python.exe') -B (Join-Path $PSScriptRoot 'build_save_load_owned.py')
if($LASTEXITCODE -ne 0){throw 'Native save/frame callback composition verification failed'}
