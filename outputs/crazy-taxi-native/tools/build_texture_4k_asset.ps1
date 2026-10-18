param(
    [string]$SourcePng,
    [string]$OutputPng,
    [string]$OutputBgra,
    [string]$Report,
    [string]$ApprovalManifest,
    [string]$Material = 'city1 CTM6 texture index 141'
)
$ErrorActionPreference = 'Stop'
$asset = Join-Path $PSScriptRoot '..\assets\texture-ai-experiments\city1-tile-141-text-free-approved'
$approvalPath = if ($ApprovalManifest) { $ApprovalManifest } else { Join-Path $asset 'text-free-approval.json' }
if (-not $SourcePng) { $SourcePng = Join-Path $asset 'source-preserving-4x-128.png' }
if (-not $OutputPng) { $OutputPng = Join-Path $asset 'city1-texture141-4k.png' }
if (-not $OutputBgra) { $OutputBgra = Join-Path $asset 'city1-texture141-4k.bgra' }
if (-not $Report) { $Report = Join-Path $PSScriptRoot '..\reports\texture-ai-trial\texture141-source-preserving\4k-build.json' }
$SourcePng = [System.IO.Path]::GetFullPath($SourcePng)
$OutputPng = [System.IO.Path]::GetFullPath($OutputPng)
$OutputBgra = [System.IO.Path]::GetFullPath($OutputBgra)
$Report = [System.IO.Path]::GetFullPath($Report)
$approval = Get-Content -LiteralPath $approvalPath -Raw | ConvertFrom-Json
if ($approval.status -ne 'APPROVED_TEXT_FREE_DIAGNOSTIC' -or
    $approval.text_policy.source_contains_text -ne $false -or
    $approval.text_policy.generated_detail_contains_text -ne $false -or
    $approval.text_policy.exclude_text_fonts_logos_signage -ne $true -or
    $approval.text_policy.reviewed_by_human -ne $true) {
    throw 'Refusing 4K texture build: approved text-free manifest is required.'
}
$sourceHash = (Get-FileHash -LiteralPath $SourcePng -Algorithm SHA256).Hash.ToLowerInvariant()
if ($sourceHash -ne $approval.approved_candidate_sha256) {
    throw 'Refusing 4K texture build: source candidate is not on the reviewed text-free allowlist.'
}
$sourceImage = [System.Drawing.Image]::FromFile($SourcePng)
$bitmap = $null; $graphics = $null; $attributes = $null; $locked = $false
try {
    if ($sourceImage.Width -ne 128 -or $sourceImage.Height -ne 128) { throw 'Expected the reviewed 128x128 material candidate.' }
    $bitmap = [System.Drawing.Bitmap]::new(4096,4096,[System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.Clear([System.Drawing.Color]::FromArgb(255,0,0,0))
    $graphics.CompositingMode = [System.Drawing.Drawing2D.CompositingMode]::SourceCopy
    $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $attributes = [System.Drawing.Imaging.ImageAttributes]::new()
    $attributes.SetWrapMode([System.Drawing.Drawing2D.WrapMode]::TileFlipXY)
    $dest = [System.Drawing.Rectangle]::new(0,0,4096,4096)
    $graphics.DrawImage($sourceImage,$dest,0,0,$sourceImage.Width,$sourceImage.Height,[System.Drawing.GraphicsUnit]::Pixel,$attributes)
    $pngParent = Split-Path -Parent $OutputPng; if (-not (Test-Path -LiteralPath $pngParent)) { New-Item -ItemType Directory -Path $pngParent | Out-Null }
    $rawParent = Split-Path -Parent $OutputBgra; if (-not (Test-Path -LiteralPath $rawParent)) { New-Item -ItemType Directory -Path $rawParent | Out-Null }
    $reportParent = Split-Path -Parent $Report; if (-not (Test-Path -LiteralPath $reportParent)) { New-Item -ItemType Directory -Path $reportParent | Out-Null }
    $bitmap.Save($OutputPng,[System.Drawing.Imaging.ImageFormat]::Png)
    $rect = [System.Drawing.Rectangle]::new(0,0,4096,4096)
    $data = $bitmap.LockBits($rect,[System.Drawing.Imaging.ImageLockMode]::ReadOnly,[System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $locked = $true
    if ([Math]::Abs($data.Stride) -ne 4096*4) { throw "Unexpected RGBA stride $($data.Stride)." }
    $pixels = [byte[]]::new(4096*4096*4)
    if ($data.Stride -gt 0) {
        [System.Runtime.InteropServices.Marshal]::Copy($data.Scan0,$pixels,0,$pixels.Length)
    } else {
        $row = [byte[]]::new(4096*4)
        for ($y=0; $y -lt 4096; $y++) {
            [System.Runtime.InteropServices.Marshal]::Copy([IntPtr]::Add($data.Scan0,$y*$data.Stride),$row,0,$row.Length)
            [Array]::Copy($row,0,$pixels,$y*$row.Length,$row.Length)
        }
    }
    $bitmap.UnlockBits($data); $locked = $false
    [System.IO.File]::WriteAllBytes($OutputBgra,$pixels)
    $reportObject = [ordered]@{
        status = 'PASS_TEXT_FREE_4K_TEXTURE_BUILD'
        material = $Material
        source_sha256 = $sourceHash
        generated_png_sha256 = (Get-FileHash -LiteralPath $OutputPng -Algorithm SHA256).Hash.ToLowerInvariant()
        runtime_bgra_sha256 = (Get-FileHash -LiteralPath $OutputBgra -Algorithm SHA256).Hash.ToLowerInvariant()
        dimensions = @(4096,4096)
        runtime_format = 'BGRA8, tightly packed, 4096*4096*4 bytes'
        text_exclusion_enforced = $true
        excluded_content = @('text','fonts','logos','signage','lettering')
        method = 'Approved source-preserving 128x128 tile resized to4096x4096 with high-quality bicubic sampling and mirrored edge extension; original path retained as fallback.'
        scope = 'Single material diagnostic; not a complete 4K art pack, and style/gameplay visual parity still requires in-scene review.'
    }
    $reportObject | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $Report -Encoding utf8
    $reportObject | ConvertTo-Json -Depth 6
} finally {
    if ($locked) { $bitmap.UnlockBits($data) }
    if ($attributes) { $attributes.Dispose() }
    if ($graphics) { $graphics.Dispose() }
    if ($bitmap) { $bitmap.Dispose() }
    if ($sourceImage) { $sourceImage.Dispose() }
}

