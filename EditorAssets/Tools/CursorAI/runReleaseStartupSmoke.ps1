# Release|x64 ビルド成果物の最小起動スモーク（CI / ローカル共通）
param(
  [string] $Configuration = "Release",
  [int] $SmokeFrames = 90
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
$ExePath = Join-Path $RepoRoot "Generated\Outputs\$Configuration\CG2_BuildUp.exe"

if (!(Test-Path $ExePath)) {
  Write-Error "Release exe not found: $ExePath"
  exit 1
}

$WorkDir = Split-Path $ExePath -Parent
$env:ATRUM_RELEASE_SMOKE_FRAMES = "$SmokeFrames"

Write-Host "Smoke: $ExePath ($SmokeFrames frames)"
$p = Start-Process -FilePath $ExePath -WorkingDirectory $WorkDir -PassThru -Wait
if ($p.ExitCode -ne 0) {
  Write-Error "Smoke failed with exit code $($p.ExitCode)"
  exit $p.ExitCode
}

Write-Host "Smoke OK"
exit 0